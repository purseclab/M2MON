#!/usr/bin/env python
import sys
import os
import csv 

# Model extraction
args = sys.argv[1:]
filename = args[0]
dev_info_file = args[1]
devices = dict()
with open(dev_info_file) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')
    line_count = 0
    for row in csv_reader:
        print row
        devices[row[0]] = row[1]
print devices
sel = raw_input("Select Device:")
print "You selected:"+sel
print "cat " + args[0] + "| grep '" + devices[sel] + ",' > "+sel
os.system("cat " + args[0] + "| grep '" + devices[sel] + ",' > "+sel)
os.system("sed -i 's/"+devices[sel]+", //g' " + sel)
    
file = open("baroLogPattern","r");
lastState = 2;
lineNum = 0;
numbers = 0
lineMap = dict()
for line in file:
    if line not in lineMap:
        lineMap[line]=dict()
        numbers += 1;
    if lineNum > 0:
        lineMap[lastLine][line]=lineNum
    lastLine = line
    lineNum += 1
print "Total number of values " + str(numbers)
for edge,edges in lineMap.items():
    print "For Edge " + edge + str(len(edges))

print lineMap

def sidentwrite(file, str):
    file.write("    "+str)

def didentwrite(file,str):
    sidentwrite(file,"    "+str)

# Model Generation
#TODO: 

# Code Generation
pred = os.path.splitext(filename)[0]
f= open(sel+".c","w+")
f.write("void "+sel+"_filter(unsigned char * send) { \n")
f.write("static unsigned char lastTrans = -1; \n")
f.write("switch (lastTrans) { \n")
for edge,edges in lineMap.items():
    sidentwrite(f,"case "+edge.split(",")[0]+": \n")
    didentwrite(f,"if (")
    written = 0
    for allowed,times in edges.items():
        f.write("send[0] !="+allowed.split(",")[0] + "")
        written += 1
        if (written != len(edges) ):
            f.write("||")
    f.write(") \n printf(\"Malicious Write\"); \n break; \n")
f.write("}\n")
f.write("lastTrans = send[0];\n")
f.write("}\n")


