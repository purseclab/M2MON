
;; Function device::Device* PX4IO_serial_interface() (_Z22PX4IO_serial_interfacev, funcdef_no=249, decl_uid=11781, cgraph_uid=244, symbol_order=247)

device::Device* PX4IO_serial_interface() ()
{
  void * D.11919;
  struct Device * D.12626;

  D.11919 = operator new (112);
  PX4IO_serial::PX4IO_serial (D.11919);
  D.12626 = D.11919;
  goto <D.12627>;
  <D.12627>:
  return D.12626;
}



;; Function void test_routine(unsigned int) (_Z12test_routinej, funcdef_no=250, decl_uid=11922, cgraph_uid=245, symbol_order=248)

__attribute__((irqbox))
void test_routine(unsigned int) (unsigned int status)
{
  int * p;
  int array[2];

  p = &array;
  *p = 10;
  array = {CLOBBER};
  return;
}



;; Function PX4IO_serial::PX4IO_serial() (_ZN12PX4IO_serialC2Ev, funcdef_no=252, decl_uid=11853, cgraph_uid=247, symbol_order=250)

PX4IO_serial::PX4IO_serial() (struct PX4IO_serial * const this)
{
  struct PX4IO_serial * D.12640;
  struct perf_ctr_header * D.12639;
  struct perf_ctr_header * D.12638;
  struct perf_ctr_header * D.12637;
  struct perf_ctr_header * D.12636;
  struct perf_ctr_header * D.12635;
  struct perf_ctr_header * D.12634;
  struct perf_ctr_header * D.12633;
  struct perf_ctr_header * D.12632;
  struct perf_ctr_header * D.12631;
  struct perf_ctr_header * D.12630;
  int (*__vtbl_ptr_type) () * D.12629;
  struct Device * D.12628;

  MEM[(struct  &)this] = {CLOBBER};
  D.12628 = &this->D.11864;
  device::Device::Device (D.12628, "PX4IO_serial", 0);
  D.12629 = &_ZTV12PX4IO_serial + 8;
  this->D.11864._vptr.Device = D.12629;
  this->_tx_dma = 0B;
  this->_rx_dma = 0B;
  this->_rx_dma_status = 2147483648;
  this->_bus_semaphore.semcount = 0;
  this->_bus_semaphore.holder.htcb = 0B;
  this->_bus_semaphore.holder.counts = 0;
  this->_completion_semaphore.semcount = 0;
  this->_completion_semaphore.holder.htcb = 0B;
  this->_completion_semaphore.holder.counts = 0;
  D.12630 = perf_alloc (1, "io_txns     ");
  this->_pc_txns = D.12630;
  D.12631 = perf_alloc (1, "io_dmasetup ");
  this->_pc_dmasetup = D.12631;
  D.12632 = perf_alloc (0, "io_retries  ");
  this->_pc_retries = D.12632;
  D.12633 = perf_alloc (0, "io_timeouts ");
  this->_pc_timeouts = D.12633;
  D.12634 = perf_alloc (0, "io_crcerrs  ");
  this->_pc_crcerrs = D.12634;
  D.12635 = perf_alloc (0, "io_dmaerrs  ");
  this->_pc_dmaerrs = D.12635;
  D.12636 = perf_alloc (0, "io_protoerrs");
  this->_pc_protoerrs = D.12636;
  D.12637 = perf_alloc (0, "io_uarterrs ");
  this->_pc_uerrs = D.12637;
  D.12638 = perf_alloc (0, "io_idle     ");
  this->_pc_idle = D.12638;
  D.12639 = perf_alloc (0, "io_badidle  ");
  this->_pc_badidle = D.12639;
  g_interface = this;
  <D.11986>:
  D.12640 = this;
  goto <D.12641>;
  <D.12641>:
  return D.12640;
}



;; Function PX4IO_serial::~PX4IO_serial() (_ZN12PX4IO_serialD2Ev, funcdef_no=255, decl_uid=11861, cgraph_uid=250, symbol_order=253)

PX4IO_serial::~PX4IO_serial() (struct PX4IO_serial * const this)
{
  void * D.12671;
  struct Device * D.12670;
  struct PX4IO_serial * g_interface.0;
  struct perf_ctr_header * D.12665;
  struct perf_ctr_header * D.12664;
  struct perf_ctr_header * D.12663;
  struct perf_ctr_header * D.12662;
  struct perf_ctr_header * D.12661;
  struct perf_ctr_header * D.12660;
  struct perf_ctr_header * D.12659;
  struct perf_ctr_header * D.12658;
  struct perf_ctr_header * D.12657;
  struct perf_ctr_header * D.12656;
  struct sem_t * D.12655;
  struct sem_t * D.12654;
  volatile uint32_t * D.12653;
  volatile uint32_t * D.12652;
  volatile uint32_t * D.12651;
  void * D.12647;
  void * D.12643;
  int (*__vtbl_ptr_type) () * D.12642;

  D.12642 = &_ZTV12PX4IO_serial + 8;
  this->D.11864._vptr.Device = D.12642;
  D.12643 = this->_tx_dma;
  if (D.12643 != 0B) goto <D.12644>; else goto <D.12645>;
  <D.12644>:
  D.12643 = this->_tx_dma;
  stm32_dmastop (D.12643);
  D.12643 = this->_tx_dma;
  stm32_dmafree (D.12643);
  goto <D.12646>;
  <D.12645>:
  <D.12646>:
  D.12647 = this->_rx_dma;
  if (D.12647 != 0B) goto <D.12648>; else goto <D.12649>;
  <D.12648>:
  D.12647 = this->_rx_dma;
  stm32_dmastop (D.12647);
  D.12647 = this->_rx_dma;
  stm32_dmafree (D.12647);
  goto <D.12650>;
  <D.12649>:
  <D.12650>:
  D.12651 = 1073812492B;
  *D.12651 = 0;
  D.12652 = 1073812496B;
  *D.12652 = 0;
  D.12653 = 1073812500B;
  *D.12653 = 0;
  up_disable_irq (87);
  irq_attach (87, 0B);
  stm32_unconfiggpio (625702);
  stm32_unconfiggpio (625703);
  D.12654 = &this->_completion_semaphore;
  sem_destroy (D.12654);
  D.12655 = &this->_bus_semaphore;
  sem_destroy (D.12655);
  D.12656 = this->_pc_txns;
  perf_free (D.12656);
  D.12657 = this->_pc_dmasetup;
  perf_free (D.12657);
  D.12658 = this->_pc_retries;
  perf_free (D.12658);
  D.12659 = this->_pc_timeouts;
  perf_free (D.12659);
  D.12660 = this->_pc_crcerrs;
  perf_free (D.12660);
  D.12661 = this->_pc_dmaerrs;
  perf_free (D.12661);
  D.12662 = this->_pc_protoerrs;
  perf_free (D.12662);
  D.12663 = this->_pc_uerrs;
  perf_free (D.12663);
  D.12664 = this->_pc_idle;
  perf_free (D.12664);
  D.12665 = this->_pc_badidle;
  perf_free (D.12665);
  g_interface.0 = g_interface;
  if (g_interface.0 == this) goto <D.12667>; else goto <D.12668>;
  <D.12667>:
  g_interface = 0B;
  goto <D.12669>;
  <D.12668>:
  <D.12669>:
  D.12670 = &this->D.11864;
  device::Device::~Device (D.12670);
  MEM[(struct  &)this] = {CLOBBER};
  <D.11996>:
  D.12671 = this;
  goto <D.12672>;
  <D.12672>:
  return D.12671;
}



;; Function virtual PX4IO_serial::~PX4IO_serial() (_ZN12PX4IO_serialD0Ev, funcdef_no=257, decl_uid=11855, cgraph_uid=252, symbol_order=255)

virtual PX4IO_serial::~PX4IO_serial() (struct PX4IO_serial * const this)
{
  void * D.12673;

  PX4IO_serial::~PX4IO_serial (this);
  <D.12000>:
  operator delete (this);
  D.12673 = this;
  goto <D.12674>;
  <D.12674>:
  return D.12673;
}



;; Function virtual int PX4IO_serial::init() (_ZN12PX4IO_serial4initEv, funcdef_no=258, decl_uid=11789, cgraph_uid=253, symbol_order=256)

virtual int PX4IO_serial::init() (struct PX4IO_serial * const this)
{
  uint32_t fraction;
  uint32_t mantissa;
  uint32_t usartdiv32;
  struct sem_t * D.12697;
  struct sem_t * D.12696;
  unsigned int D.12695;
  unsigned int D.12694;
  volatile uint32_t * D.12693;
  unsigned int D.12692;
  unsigned int D.12691;
  unsigned int D.12690;
  unsigned int vol.2;
  volatile uint32_t * D.12688;
  unsigned int vol.1;
  volatile uint32_t * D.12686;
  volatile uint32_t * D.12685;
  volatile uint32_t * D.12684;
  volatile uint32_t * D.12683;
  int D.12682;
  void * D.12681;
  void * D.12679;
  void * D.12676;
  void * D.12675;

  D.12675 = stm32_dmachannel (125);
  this->_tx_dma = D.12675;
  D.12676 = stm32_dmachannel (85);
  this->_rx_dma = D.12676;
  D.12679 = this->_tx_dma;
  if (D.12679 == 0B) goto <D.12677>; else goto <D.12680>;
  <D.12680>:
  D.12681 = this->_rx_dma;
  if (D.12681 == 0B) goto <D.12677>; else goto <D.12678>;
  <D.12677>:
  D.12682 = -1;
  goto <D.12698>;
  <D.12678>:
  stm32_configgpio (625702);
  stm32_configgpio (625703);
  D.12683 = 1073812492B;
  *D.12683 = 0;
  D.12684 = 1073812496B;
  *D.12684 = 0;
  D.12685 = 1073812500B;
  *D.12685 = 0;
  D.12686 = 1073812480B;
  vol.1 = *D.12686;
  D.12688 = 1073812484B;
  vol.2 = *D.12688;
  usartdiv32 = 112;
  mantissa = usartdiv32 >> 5;
  D.12690 = mantissa << 5;
  D.12691 = usartdiv32 - D.12690;
  D.12692 = D.12691 + 1;
  fraction = D.12692 >> 1;
  D.12693 = 1073812488B;
  D.12694 = mantissa << 4;
  D.12695 = D.12694 | fraction;
  *D.12693 = D.12695;
  irq_attach (87, _interrupt);
  up_enable_irq (87);
  D.12685 = 1073812500B;
  *D.12685 = 1;
  D.12683 = 1073812492B;
  *D.12683 = 8220;
  D.12696 = &this->_completion_semaphore;
  sem_init (D.12696, 0, 0);
  D.12697 = &this->_bus_semaphore;
  sem_init (D.12697, 0, 1);
  D.12682 = 0;
  goto <D.12698>;
  <D.12698>:
  return D.12682;
}



;; Function virtual int PX4IO_serial::ioctl(unsigned int, unsigned int&) (_ZN12PX4IO_serial5ioctlEjRj, funcdef_no=259, decl_uid=11803, cgraph_uid=254, symbol_order=257)

virtual int PX4IO_serial::ioctl(unsigned int, unsigned int&) (struct PX4IO_serial * const this, unsigned int operation, unsigned int & arg)
{
  uint16_t value;
  unsigned int count;
  unsigned int fails;
  struct perf_ctr_header * D.12735;
  struct perf_ctr_header * D.12734;
  struct perf_ctr_header * D.12733;
  struct perf_ctr_header * D.12732;
  struct perf_ctr_header * D.12731;
  struct perf_ctr_header * D.12730;
  struct perf_ctr_header * D.12729;
  struct perf_ctr_header * D.12728;
  struct perf_ctr_header * D.12727;
  struct perf_ctr_header * D.12726;
  int D.12720;
  int (*__vtbl_ptr_type) () D.12719;
  int (*__vtbl_ptr_type) () * D.12718;
  int (*__vtbl_ptr_type) () * D.12717;
  bool retval.5;
  short unsigned int value.4;
  int D.12713;
  volatile uint32_t * D.12712;
  unsigned int D.12710;
  unsigned int D.12709;
  volatile uint32_t * D.12708;
  bool retval.3;
  unsigned int D.12704;
  unsigned int D.12703;
  volatile uint32_t * D.12702;
  void * D.12701;
  void * D.12700;
  unsigned int D.12699;

  switch (operation) <default: <D.12019>, case 1: <D.12012>>
  <D.12012>:
  D.12699 = *arg;
  switch (D.12699) <default: <D.12737>, case 0: <D.12013>, case 1: <D.12014>, case 2: <D.12018>>
  <D.12013>:
  lowsyslog ("test 0\n");
  D.12700 = this->_tx_dma;
  stm32_dmastop (D.12700);
  D.12701 = this->_rx_dma;
  stm32_dmastop (D.12701);
  D.12702 = 1073812500B;
  D.12702 = 1073812500B;
  D.12703 = *D.12702;
  D.12704 = D.12703 & 4294967103;
  *D.12702 = D.12704;
  <D.12705>:
  <D.12706>:
  D.12708 = 1073812480B;
  D.12709 = *D.12708;
  D.12710 = D.12709 & 128;
  retval.3 = D.12710 == 0;
  if (retval.3 != 0) goto <D.12711>; else goto <D.12024>;
  <D.12711>:
  goto <D.12706>;
  <D.12024>:
  D.12712 = 1073812484B;
  *D.12712 = 85;
  goto <D.12705>;
  D.12713 = 0;
  goto <D.12738>;
  <D.12014>:
  fails = 0;
  count = 0;
  <D.12714>:
  value.4 = (short unsigned int) count;
  value = value.4;
  D.12717 = this->D.11864._vptr.Device;
  D.12718 = D.12717 + 20;
  D.12719 = *D.12718;
  D.12720 = OBJ_TYPE_REF(D.12719;(struct PX4IO_serial)this->5) (this, 32512, &value, 1);
  retval.5 = D.12720 != 0;
  if (retval.5 != 0) goto <D.12721>; else goto <D.12722>;
  <D.12721>:
  fails = fails + 1;
  goto <D.12723>;
  <D.12722>:
  <D.12723>:
  if (count > 4999) goto <D.12724>; else goto <D.12725>;
  <D.12724>:
  lowsyslog ("==== test 1 : %u failures ====\n", fails);
  D.12726 = this->_pc_txns;
  perf_print_counter (D.12726);
  D.12727 = this->_pc_dmasetup;
  perf_print_counter (D.12727);
  D.12728 = this->_pc_retries;
  perf_print_counter (D.12728);
  D.12729 = this->_pc_timeouts;
  perf_print_counter (D.12729);
  D.12730 = this->_pc_crcerrs;
  perf_print_counter (D.12730);
  D.12731 = this->_pc_dmaerrs;
  perf_print_counter (D.12731);
  D.12732 = this->_pc_protoerrs;
  perf_print_counter (D.12732);
  D.12733 = this->_pc_uerrs;
  perf_print_counter (D.12733);
  D.12734 = this->_pc_idle;
  perf_print_counter (D.12734);
  D.12735 = this->_pc_badidle;
  perf_print_counter (D.12735);
  count = 0;
  goto <D.12736>;
  <D.12725>:
  <D.12736>:
  value = {CLOBBER};
  count = count + 1;
  goto <D.12714>;
  D.12713 = 0;
  goto <D.12738>;
  <D.12018>:
  lowsyslog ("test 2\n");
  D.12713 = 0;
  goto <D.12738>;
  <D.12737>:
  <D.12019>:
  goto <D.12020>;
  <D.12020>:
  D.12713 = -1;
  goto <D.12738>;
  <D.12738>:
  return D.12713;
}



;; Function virtual int PX4IO_serial::write(unsigned int, void*, unsigned int) (_ZN12PX4IO_serial5writeEjPvj, funcdef_no=260, decl_uid=11799, cgraph_uid=255, symbol_order=258)

virtual int PX4IO_serial::write(unsigned int, void*, unsigned int) (struct PX4IO_serial * const this, unsigned int address, void * data, unsigned int count)
{
  unsigned int i;
  unsigned int retries;
  int result;
  const uint16_t * values;
  uint8_t offset;
  uint8_t page;
  struct perf_ctr_header * D.12761;
  struct perf_ctr_header * D.12759;
  int D.12756;
  int D.12755;
  unsigned char D.12754;
  unsigned int D.12749;
  unsigned char D.12748;
  unsigned char D.12747;
  struct sem_t * D.12744;
  int D.12743;
  unsigned int D.12740;

  D.12740 = address >> 8;
  page = (uint8_t) D.12740;
  offset = (uint8_t) address;
  values = data;
  if (count > 32) goto <D.12741>; else goto <D.12742>;
  <D.12741>:
  D.12743 = -22;
  goto <D.12765>;
  <D.12742>:
  D.12744 = &this->_bus_semaphore;
  sem_wait (D.12744);
  retries = 0;
  <D.12745>:
  if (retries > 2) goto <D.12040>; else goto <D.12746>;
  <D.12746>:
  D.12747 = (unsigned char) count;
  D.12748 = D.12747 | 64;
  _dma_buffer.count_code = D.12748;
  _dma_buffer.page = page;
  _dma_buffer.offset = offset;
  D.12749 = count * 2;
  memcpy (&_dma_buffer.regs[0], values, D.12749);
  i = count;
  <D.12750>:
  if (i > 31) goto <D.12042>; else goto <D.12751>;
  <D.12751>:
  _dma_buffer.regs[i] = 21930;
  i = i + 1;
  goto <D.12750>;
  <D.12042>:
  result = PX4IO_serial::_wait_complete (this);
  if (result == 0) goto <D.12752>; else goto <D.12753>;
  <D.12752>:
  D.12754 = _dma_buffer.count_code;
  D.12755 = (int) D.12754;
  D.12756 = D.12755 & 192;
  if (D.12756 == 128) goto <D.12757>; else goto <D.12758>;
  <D.12757>:
  result = -22;
  D.12759 = this->_pc_protoerrs;
  perf_count (D.12759);
  goto <D.12760>;
  <D.12758>:
  <D.12760>:
  goto <D.12040>;
  <D.12753>:
  D.12761 = this->_pc_retries;
  perf_count (D.12761);
  retries = retries + 1;
  goto <D.12745>;
  <D.12040>:
  D.12744 = &this->_bus_semaphore;
  sem_post (D.12744);
  if (result == 0) goto <D.12762>; else goto <D.12763>;
  <D.12762>:
  result = (int) count;
  goto <D.12764>;
  <D.12763>:
  <D.12764>:
  D.12743 = result;
  goto <D.12765>;
  <D.12765>:
  return D.12743;
}



;; Function virtual int PX4IO_serial::read(unsigned int, void*, unsigned int) (_ZN12PX4IO_serial4readEjPvj, funcdef_no=261, decl_uid=11794, cgraph_uid=256, symbol_order=259)

virtual int PX4IO_serial::read(unsigned int, void*, unsigned int) (struct PX4IO_serial * const this, unsigned int address, void * data, unsigned int count)
{
  unsigned int retries;
  int result;
  uint16_t * values;
  uint8_t offset;
  uint8_t page;
  struct perf_ctr_header * D.12789;
  unsigned int D.12788;
  unsigned int D.12784;
  unsigned int D.12783;
  struct perf_ctr_header * D.12781;
  int D.12778;
  int D.12777;
  unsigned char D.12776;
  unsigned char D.12773;
  struct sem_t * D.12770;
  int D.12769;
  unsigned int D.12766;

  D.12766 = address >> 8;
  page = (uint8_t) D.12766;
  offset = (uint8_t) address;
  values = data;
  if (count > 32) goto <D.12767>; else goto <D.12768>;
  <D.12767>:
  D.12769 = -22;
  goto <D.12793>;
  <D.12768>:
  D.12770 = &this->_bus_semaphore;
  sem_wait (D.12770);
  retries = 0;
  <D.12771>:
  if (retries > 2) goto <D.12055>; else goto <D.12772>;
  <D.12772>:
  D.12773 = (unsigned char) count;
  _dma_buffer.count_code = D.12773;
  _dma_buffer.page = page;
  _dma_buffer.offset = offset;
  result = PX4IO_serial::_wait_complete (this);
  if (result == 0) goto <D.12774>; else goto <D.12775>;
  <D.12774>:
  D.12776 = _dma_buffer.count_code;
  D.12777 = (int) D.12776;
  D.12778 = D.12777 & 192;
  if (D.12778 == 128) goto <D.12779>; else goto <D.12780>;
  <D.12779>:
  result = -22;
  D.12781 = this->_pc_protoerrs;
  perf_count (D.12781);
  goto <D.12782>;
  <D.12780>:
  D.12776 = _dma_buffer.count_code;
  D.12783 = (unsigned int) D.12776;
  D.12784 = D.12783 & 63;
  if (D.12784 != count) goto <D.12785>; else goto <D.12786>;
  <D.12785>:
  result = -5;
  D.12781 = this->_pc_protoerrs;
  perf_count (D.12781);
  goto <D.12787>;
  <D.12786>:
  D.12788 = count * 2;
  memcpy (values, &_dma_buffer.regs[0], D.12788);
  <D.12787>:
  <D.12782>:
  goto <D.12055>;
  <D.12775>:
  D.12789 = this->_pc_retries;
  perf_count (D.12789);
  retries = retries + 1;
  goto <D.12771>;
  <D.12055>:
  D.12770 = &this->_bus_semaphore;
  sem_post (D.12770);
  if (result == 0) goto <D.12790>; else goto <D.12791>;
  <D.12790>:
  result = (int) count;
  goto <D.12792>;
  <D.12791>:
  <D.12792>:
  D.12769 = result;
  goto <D.12793>;
  <D.12793>:
  return D.12769;
}



;; Function int PX4IO_serial::_wait_complete() (_ZN12PX4IO_serial14_wait_completeEv, funcdef_no=262, decl_uid=11813, cgraph_uid=257, symbol_order=260)

int PX4IO_serial::_wait_complete() (struct PX4IO_serial * const this)
{
  uint8_t crc;
  int ret;
  struct timespec abstime;
  int D.12851;
  int D.12850;
  int * D.12849;
  struct perf_ctr_header * D.12848;
  int D.12845;
  int * D.12844;
  bool retval.12;
  struct perf_ctr_header * D.12842;
  bool D.12839;
  int D.12838;
  bool D.12837;
  unsigned char D.12836;
  bool retval.11;
  struct perf_ctr_header * D.12834;
  unsigned int D.12831;
  unsigned int D.12830;
  bool retval.10;
  struct sem_t * D.12826;
  long int D.12823;
  unsigned int D.12822;
  unsigned int D.12821;
  long int D.12818;
  long int D.12817;
  unsigned int D.12816;
  unsigned int D.12815;
  unsigned int D.12814;
  int D.12813;
  int _dma_buffer.9;
  int D.12811;
  uint16_t * D.12810;
  int D.12809;
  int D.12808;
  unsigned char D.12807;
  void * D.12806;
  unsigned char D.12805;
  unsigned int D.12804;
  unsigned int D.12803;
  volatile uint32_t * D.12802;
  unsigned int _dma_buffer.8;
  void * D.12800;
  struct perf_ctr_header * D.12799;
  struct perf_ctr_header * D.12798;
  unsigned int vol.7;
  volatile uint32_t * D.12796;
  unsigned int vol.6;
  volatile uint32_t * D.12794;

  D.12794 = 1073812480B;
  vol.6 = *D.12794;
  D.12796 = 1073812484B;
  vol.7 = *D.12796;
  D.12798 = this->_pc_txns;
  perf_begin (D.12798);
  D.12799 = this->_pc_dmasetup;
  perf_begin (D.12799);
  this->_rx_dma_status = 0;
  D.12800 = this->_rx_dma;
  _dma_buffer.8 = (unsigned int) &_dma_buffer;
  stm32_dmasetup (D.12800, 1073812484, _dma_buffer.8, 68, 1280);
  D.12800 = this->_rx_dma;
  stm32_dmastart (D.12800, _dma_callback, this, 0);
  D.12802 = 1073812500B;
  D.12802 = 1073812500B;
  D.12803 = *D.12802;
  D.12804 = D.12803 | 64;
  *D.12802 = D.12804;
  _dma_buffer.crc = 0;
  D.12805 = crc_packet (&_dma_buffer);
  _dma_buffer.crc = D.12805;
  D.12806 = this->_tx_dma;
  _dma_buffer.8 = (unsigned int) &_dma_buffer;
  D.12807 = _dma_buffer.count_code;
  D.12808 = (int) D.12807;
  D.12809 = D.12808 & 63;
  D.12810 = &_dma_buffer.regs[D.12809];
  D.12811 = (int) D.12810;
  _dma_buffer.9 = (int) &_dma_buffer;
  D.12813 = D.12811 - _dma_buffer.9;
  D.12814 = (unsigned int) D.12813;
  stm32_dmasetup (D.12806, 1073812484, _dma_buffer.8, D.12814, 1088);
  D.12806 = this->_tx_dma;
  stm32_dmastart (D.12806, 0B, 0B, 0);
  D.12802 = 1073812500B;
  D.12802 = 1073812500B;
  D.12815 = *D.12802;
  D.12816 = D.12815 | 128;
  *D.12802 = D.12816;
  D.12799 = this->_pc_dmasetup;
  perf_end (D.12799);
  clock_gettime (0, &abstime);
  D.12817 = abstime.tv_nsec;
  D.12818 = D.12817 + 10000000;
  abstime.tv_nsec = D.12818;
  D.12817 = abstime.tv_nsec;
  if (D.12817 > 999999999) goto <D.12819>; else goto <D.12820>;
  <D.12819>:
  D.12821 = abstime.tv_sec;
  D.12822 = D.12821 + 1;
  abstime.tv_sec = D.12822;
  D.12817 = abstime.tv_nsec;
  D.12823 = D.12817 + -1000000000;
  abstime.tv_nsec = D.12823;
  goto <D.12824>;
  <D.12820>:
  <D.12824>:
  <D.12825>:
  D.12826 = &this->_completion_semaphore;
  ret = sem_timedwait (D.12826, &abstime);
  if (ret == 0) goto <D.12827>; else goto <D.12828>;
  <D.12827>:
  D.12830 = this->_rx_dma_status;
  D.12831 = D.12830 & 8;
  retval.10 = D.12831 != 0;
  if (retval.10 != 0) goto <D.12832>; else goto <D.12833>;
  <D.12832>:
  D.12834 = this->_pc_dmaerrs;
  perf_count (D.12834);
  ret = -5;
  goto <D.12096>;
  <D.12833>:
  crc = _dma_buffer.crc;
  _dma_buffer.crc = 0;
  D.12836 = crc_packet (&_dma_buffer);
  D.12837 = D.12836 != crc;
  D.12807 = _dma_buffer.count_code;
  D.12808 = (int) D.12807;
  D.12838 = D.12808 & 192;
  D.12839 = D.12838 == 64;
  retval.11 = D.12837 | D.12839;
  if (retval.11 != 0) goto <D.12840>; else goto <D.12841>;
  <D.12840>:
  D.12842 = this->_pc_crcerrs;
  perf_count (D.12842);
  ret = -5;
  goto <D.12096>;
  <D.12841>:
  goto <D.12096>;
  <D.12828>:
  D.12844 = get_errno_ptr ();
  D.12845 = *D.12844;
  retval.12 = D.12845 == 110;
  if (retval.12 != 0) goto <D.12846>; else goto <D.12847>;
  <D.12846>:
  PX4IO_serial::_abort_dma (this);
  D.12848 = this->_pc_timeouts;
  perf_count (D.12848);
  D.12798 = this->_pc_txns;
  perf_cancel (D.12798);
  goto <D.12096>;
  <D.12847>:
  D.12849 = get_errno_ptr ();
  D.12850 = *D.12849;
  lowsyslog ("unexpected ret %d/%d\n", ret, D.12850);
  goto <D.12825>;
  <D.12096>:
  this->_rx_dma_status = 2147483648;
  D.12798 = this->_pc_txns;
  perf_end (D.12798);
  D.12851 = ret;
  goto <D.12853>;
  <D.12853>:
  abstime = {CLOBBER};
  goto <D.12852>;
  <D.12852>:
  return D.12851;
}



;; Function uint8_t crc_packet(IOPacket*) (_ZL10crc_packetP8IOPacket, funcdef_no=248, decl_uid=11772, cgraph_uid=243, symbol_order=244)

__attribute__((unused))
uint8_t crc_packet(IOPacket*) (struct IOPacket * pkt)
{
  uint8_t c;
  uint8_t * p;
  uint8_t * end;
  uint8_t D.12863;
  int D.12862;
  unsigned char D.12861;
  unsigned char D.12860;
  uint8_t * p.13;
  int D.12856;
  int D.12855;
  unsigned char D.12854;

  D.12854 = pkt->count_code;
  D.12855 = (int) D.12854;
  D.12856 = D.12855 & 63;
  end = &pkt->regs[D.12856];
  p = pkt;
  c = 0;
  <D.12857>:
  if (p >= end) goto <D.11779>; else goto <D.12858>;
  <D.12858>:
  p.13 = p;
  p = p.13 + 1;
  D.12860 = *p.13;
  D.12861 = D.12860 ^ c;
  D.12862 = (int) D.12861;
  c = crc8_tab[D.12862];
  goto <D.12857>;
  <D.11779>:
  D.12863 = c;
  goto <D.12864>;
  <D.12864>:
  return D.12863;
}



;; Function static void PX4IO_serial::_dma_callback(DMA_HANDLE, uint8_t, void*) (_ZN12PX4IO_serial13_dma_callbackEPvhS0_, funcdef_no=263, decl_uid=11818, cgraph_uid=258, symbol_order=261)

static void PX4IO_serial::_dma_callback(DMA_HANDLE, uint8_t, void*) (void * handle, uint8_t status, void * arg)
{
  struct PX4IO_serial * ps;
  unsigned int D.12867;

  if (arg != 0B) goto <D.12865>; else goto <D.12866>;
  <D.12865>:
  ps = arg;
  D.12867 = (unsigned int) status;
  PX4IO_serial::_do_rx_dma_callback (ps, D.12867);
  goto <D.12868>;
  <D.12866>:
  <D.12868>:
  return;
}



;; Function void PX4IO_serial::_do_rx_dma_callback(unsigned int) (_ZN12PX4IO_serial19_do_rx_dma_callbackEj, funcdef_no=264, decl_uid=11820, cgraph_uid=259, symbol_order=262)

__attribute__((irqbox))
void PX4IO_serial::_do_rx_dma_callback(unsigned int) (struct PX4IO_serial * const this, unsigned int status)
{
  uint32_t sr;
  struct sem_t * D.12883;
  unsigned int D.12882;
  unsigned int D.12881;
  volatile uint32_t * D.12880;
  unsigned int vol.15;
  volatile uint32_t * D.12877;
  unsigned int D.12874;
  volatile uint32_t * D.12873;
  unsigned int D.12870;
  bool retval.14;

  D.12870 = this->_rx_dma_status;
  retval.14 = D.12870 == 0;
  if (retval.14 != 0) goto <D.12871>; else goto <D.12872>;
  <D.12871>:
  D.12873 = 1073812480B;
  sr = *D.12873;
  D.12874 = sr & 40;
  if (D.12874 != 0) goto <D.12875>; else goto <D.12876>;
  <D.12875>:
  D.12877 = 1073812484B;
  vol.15 = *D.12877;
  status = 8;
  goto <D.12879>;
  <D.12876>:
  <D.12879>:
  this->_rx_dma_status = status;
  D.12880 = 1073812500B;
  D.12880 = 1073812500B;
  D.12881 = *D.12880;
  D.12882 = D.12881 & 4294967103;
  *D.12880 = D.12882;
  D.12883 = &this->_completion_semaphore;
  sem_post (D.12883);
  goto <D.12884>;
  <D.12872>:
  <D.12884>:
  test_routine (status);
  return;
}



;; Function static int PX4IO_serial::_interrupt(int, void*) (_ZN12PX4IO_serial10_interruptEiPv, funcdef_no=265, decl_uid=11824, cgraph_uid=260, symbol_order=263)

static int PX4IO_serial::_interrupt(int, void*) (int irq, void * context)
{
  int D.12889;
  struct PX4IO_serial * g_interface.16;

  g_interface.16 = g_interface;
  if (g_interface.16 != 0B) goto <D.12886>; else goto <D.12887>;
  <D.12886>:
  g_interface.16 = g_interface;
  PX4IO_serial::_do_interrupt (g_interface.16);
  goto <D.12888>;
  <D.12887>:
  <D.12888>:
  D.12889 = 0;
  goto <D.12890>;
  <D.12890>:
  return D.12889;
}



;; Function void PX4IO_serial::_do_interrupt() (_ZN12PX4IO_serial13_do_interruptEv, funcdef_no=266, decl_uid=11825, cgraph_uid=261, symbol_order=264)

void PX4IO_serial::_do_interrupt() (struct PX4IO_serial * const this)
{
  size_t length;
  uint32_t sr;
  struct perf_ctr_header * D.12923;
  struct perf_ctr_header * D.12922;
  unsigned int D.12921;
  int D.12920;
  int _dma_buffer.20;
  int D.12918;
  uint16_t * D.12917;
  int D.12916;
  int D.12915;
  unsigned char D.12914;
  unsigned int D.12910;
  void * D.12909;
  unsigned int D.12906;
  bool retval.19;
  unsigned int D.12902;
  struct perf_ctr_header * D.12901;
  unsigned int D.12898;
  bool retval.18;
  unsigned int D.12894;
  unsigned int vol.17;
  volatile uint32_t * D.12892;
  volatile uint32_t * D.12891;

  D.12891 = 1073812480B;
  sr = *D.12891;
  D.12892 = 1073812484B;
  vol.17 = *D.12892;
  D.12894 = sr & 14;
  if (D.12894 != 0) goto <D.12895>; else goto <D.12896>;
  <D.12895>:
  D.12898 = this->_rx_dma_status;
  retval.18 = D.12898 == 0;
  if (retval.18 != 0) goto <D.12899>; else goto <D.12900>;
  <D.12899>:
  PX4IO_serial::_abort_dma (this);
  D.12901 = this->_pc_uerrs;
  perf_count (D.12901);
  PX4IO_serial::_do_rx_dma_callback (this, 8);
  goto <D.12926>;
  <D.12900>:
  goto <D.12926>;
  <D.12896>:
  D.12902 = sr & 16;
  if (D.12902 != 0) goto <D.12903>; else goto <D.12904>;
  <D.12903>:
  D.12906 = this->_rx_dma_status;
  retval.19 = D.12906 == 0;
  if (retval.19 != 0) goto <D.12907>; else goto <D.12908>;
  <D.12907>:
  D.12909 = this->_rx_dma;
  D.12910 = stm32_dmaresidual (D.12909);
  length = 68 - D.12910;
  if (length == 0) goto <D.12911>; else goto <D.12913>;
  <D.12913>:
  D.12914 = _dma_buffer.count_code;
  D.12915 = (int) D.12914;
  D.12916 = D.12915 & 63;
  D.12917 = &_dma_buffer.regs[D.12916];
  D.12918 = (int) D.12917;
  _dma_buffer.20 = (int) &_dma_buffer;
  D.12920 = D.12918 - _dma_buffer.20;
  D.12921 = (unsigned int) D.12920;
  if (D.12921 > length) goto <D.12911>; else goto <D.12912>;
  <D.12911>:
  D.12922 = this->_pc_badidle;
  perf_count (D.12922);
  D.12909 = this->_rx_dma;
  stm32_dmastop (D.12909);
  PX4IO_serial::_do_rx_dma_callback (this, 8);
  goto <D.12926>;
  <D.12912>:
  D.12923 = this->_pc_idle;
  perf_count (D.12923);
  D.12909 = this->_rx_dma;
  stm32_dmastop (D.12909);
  PX4IO_serial::_do_rx_dma_callback (this, 32);
  goto <D.12924>;
  <D.12908>:
  <D.12924>:
  goto <D.12925>;
  <D.12904>:
  <D.12925>:
  <D.12926>:
  return;
}



;; Function void PX4IO_serial::_abort_dma() (_ZN12PX4IO_serial10_abort_dmaEv, funcdef_no=267, decl_uid=11827, cgraph_uid=262, symbol_order=265)

void PX4IO_serial::_abort_dma() (struct PX4IO_serial * const this)
{
  void * D.12936;
  void * D.12935;
  unsigned int vol.23;
  unsigned int vol.22;
  volatile uint32_t * D.12932;
  unsigned int vol.21;
  volatile uint32_t * D.12930;
  unsigned int D.12929;
  unsigned int D.12928;
  volatile uint32_t * D.12927;

  D.12927 = 1073812500B;
  D.12927 = 1073812500B;
  D.12928 = *D.12927;
  D.12929 = D.12928 & 4294967103;
  *D.12927 = D.12929;
  D.12930 = 1073812480B;
  vol.21 = *D.12930;
  D.12932 = 1073812484B;
  vol.22 = *D.12932;
  D.12932 = 1073812484B;
  vol.23 = *D.12932;
  D.12935 = this->_tx_dma;
  stm32_dmastop (D.12935);
  D.12936 = this->_rx_dma;
  stm32_dmastop (D.12936);
  return;
}

