/****************************************************************************
 *
 *   Copyright (c) 2013-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

 /**
  * @file px4io_serial.cpp
  *
  * Serial interface for PX4IO
  */

/* XXX trim includes */
#include <px4_config.h>

#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <debug.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <arch/board/board.h>

/* XXX might be able to prune these */
#include <nuttx/arch.h>
#include <chip.h>
#include <up_internal.h>
#include <up_arch.h>

#include <debug.h>

#include <drivers/device/device.h>
#include <drivers/drv_hrt.h>
#include <board_config.h>

#include <systemlib/perf_counter.h>

#include <modules/px4iofirmware/protocol.h>

#ifdef PX4IO_SERIAL_BASE

device::Device	*PX4IO_serial_interface();

/* serial register accessors */
#define REG(_x)		(*(volatile uint32_t *)(PX4IO_SERIAL_BASE + _x))
#define rSR		REG(STM32_USART_SR_OFFSET)
#define rDR		REG(STM32_USART_DR_OFFSET)
#define rBRR		REG(STM32_USART_BRR_OFFSET)
#define rCR1		REG(STM32_USART_CR1_OFFSET)
#define rCR2		REG(STM32_USART_CR2_OFFSET)
#define rCR3		REG(STM32_USART_CR3_OFFSET)
#define rGTPR		REG(STM32_USART_GTPR_OFFSET)

class PX4IO_serial : public device::Device
{
public:
	PX4IO_serial();
	virtual ~PX4IO_serial();

	virtual int	init();
	virtual int	read(unsigned offset, void *data, unsigned count = 1);
	virtual int	write(unsigned address, void *data, unsigned count = 1);
	virtual int	ioctl(unsigned operation, unsigned &arg);

private:
	/*
	 * XXX tune this value
	 *
	 * At 1.5Mbps each register takes 13.3µs, and we always transfer a full packet.
	 * Packet overhead is 26µs for the four-byte header.
	 *
	 * 32 registers = 451µs
	 *
	 * Maybe we can just send smaller packets (e.g. 8 regs) and loop for larger (less common)
	 * transfers? Could cause issues with any regs expecting to be written atomically...
	 */
	static IOPacket		_dma_buffer;		// XXX static to ensure DMA-able memory

	DMA_HANDLE		_tx_dma;
	DMA_HANDLE		_rx_dma;

	/** saved DMA status */
	static const unsigned	_dma_status_inactive = 0x80000000;	// low bits overlap DMA_STATUS_* values
	static const unsigned	_dma_status_waiting  = 0x00000000;
	volatile unsigned	_rx_dma_status;

	/** bus-ownership lock */
	sem_t			_bus_semaphore;

	/** client-waiting lock/signal */
	sem_t			_completion_semaphore;

	/**
	 * Start the transaction with IO and wait for it to complete.
	 */
	int			_wait_complete();

	/**
	 * DMA completion handler.
	 */
	static void		_dma_callback(DMA_HANDLE handle, uint8_t status, void *arg);
	void			_do_rx_dma_callback(unsigned status) __attribute__((irqbox));

	/**
	 * Serial interrupt handler.
	 */
	static int		_interrupt(int vector, void *context);
	void			_do_interrupt();

	/**
	 * Cancel any DMA in progress with an error.
	 */
	void			_abort_dma();

	/**
	 * Performance counters.
	 */
	perf_counter_t		_pc_txns;
	perf_counter_t		_pc_dmasetup;
	perf_counter_t		_pc_retries;
	perf_counter_t		_pc_timeouts;
	perf_counter_t		_pc_crcerrs;
	perf_counter_t		_pc_dmaerrs;
	perf_counter_t		_pc_protoerrs;
	perf_counter_t		_pc_uerrs;
	perf_counter_t		_pc_idle;
	perf_counter_t		_pc_badidle;

	/* do not allow top copying this class */
	PX4IO_serial(PX4IO_serial &);
	PX4IO_serial& operator = (const PX4IO_serial &);

};

IOPacket PX4IO_serial::_dma_buffer;
static PX4IO_serial *g_interface;

PX4IO_serial::PX4IO_serial() :
	Device("PX4IO_serial"),
	_tx_dma(nullptr),
	_rx_dma(nullptr),
	_rx_dma_status(_dma_status_inactive),
	_bus_semaphore(SEM_INITIALIZER(0)),
	_completion_semaphore(SEM_INITIALIZER(0)),
	_pc_txns(perf_alloc(PC_ELAPSED, "io_txns     ")),
	_pc_dmasetup(perf_alloc(PC_ELAPSED,	"io_dmasetup ")),
	_pc_retries(perf_alloc(PC_COUNT,	"io_retries  ")),
	_pc_timeouts(perf_alloc(PC_COUNT,	"io_timeouts ")),
	_pc_crcerrs(perf_alloc(PC_COUNT,	"io_crcerrs  ")),
	_pc_dmaerrs(perf_alloc(PC_COUNT,	"io_dmaerrs  ")),
	_pc_protoerrs(perf_alloc(PC_COUNT,	"io_protoerrs")),
	_pc_uerrs(perf_alloc(PC_COUNT,		"io_uarterrs ")),
	_pc_idle(perf_alloc(PC_COUNT,		"io_idle     ")),
	_pc_badidle(perf_alloc(PC_COUNT,	"io_badidle  "))
{
	g_interface = this;
}

void
PX4IO_serial::_dma_callback(DMA_HANDLE handle, uint8_t status, void *arg)
{
	if (arg != nullptr) {
		PX4IO_serial *ps = reinterpret_cast<PX4IO_serial *>(arg);

		ps->_do_rx_dma_callback(status);
	}
}

void
PX4IO_serial::_do_rx_dma_callback(unsigned status)
{
	/* on completion of a reply, wake the waiter */
	int billbords[100];

	int *p= &billbords[100];

	*p =10;
}

int
PX4IO_serial::_interrupt(int irq, void *context)
{
	if (g_interface != nullptr)
		g_interface->_do_interrupt();
	return 0;
}

void
PX4IO_serial::_do_interrupt()
{
	uint32_t sr = rSR;	/* get UART status register */
	(void)rDR;		/* read DR to clear status */

	if (sr & (USART_SR_ORE |	/* overrun error - packet was too big for DMA or DMA was too slow */
		USART_SR_NE |		/* noise error - we have lost a byte due to noise */
		USART_SR_FE)) {		/* framing error - start/stop bit lost or line break */
		
		/* 
		 * If we are in the process of listening for something, these are all fatal;
		 * abort the DMA with an error.
		 */
		if (_rx_dma_status == _dma_status_waiting) {
			_abort_dma();

			perf_count(_pc_uerrs);
			/* complete DMA as though in error */
			_do_rx_dma_callback(DMA_STATUS_TEIF);

			return;
		}

		/* XXX we might want to use FE / line break as an out-of-band handshake ... handle it here */

		/* don't attempt to handle IDLE if it's set - things went bad */
		return;
	}

	if (sr & USART_SR_IDLE) {

		/* if there is DMA reception going on, this is a short packet */
		if (_rx_dma_status == _dma_status_waiting) {

			/* verify that the received packet is complete */
			size_t length = sizeof(_dma_buffer) - stm32_dmaresidual(_rx_dma);
			if ((length < 1) || (length < PKT_SIZE(_dma_buffer))) {
				perf_count(_pc_badidle);

				/* stop the receive DMA */
				stm32_dmastop(_rx_dma);

				/* complete the short reception */
				_do_rx_dma_callback(DMA_STATUS_TEIF);
				return;
			}

			perf_count(_pc_idle);

			/* stop the receive DMA */
			stm32_dmastop(_rx_dma);

			/* complete the short reception */
			_do_rx_dma_callback(DMA_STATUS_TCIF);
		}
	}
}

void
PX4IO_serial::_abort_dma()
{
	/* disable UART DMA */
	rCR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);
	(void)rSR;
	(void)rDR;
	(void)rDR;

	/* stop DMA */
	stm32_dmastop(_tx_dma);
	stm32_dmastop(_rx_dma);
}

#endif /* PX4IO_SERIAL_BASE */
