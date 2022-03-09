target extended /dev/ttyACM0
monitor swdp_scan
attach 1
monitor vector_catch disable hard
set mem inaccessible-by-default off
monitor option erase
set print pretty
