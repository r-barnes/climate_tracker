#!/usr/bin/python
#Takes piped data in "LOWER_BOUND,UPPER_BOUND" pairs
import sys
import re
import numpy

raw_data=[]
for i in sys.stdin.readlines():
	i=i.strip()
	if len(i)==0:
		continue
	raw_data+=[tuple(map(float, re.split("[ \t,]",i)))]

data=[]
for i in raw_data:
	if i[0]>i[1]:
		data+=[(i[1],1),(i[0],-1)]
	else:
		data+=[(i[0],1),(i[1],-1)]

data=sorted(data, key=lambda x: x[0]) #Arrange in order of initiation
demin=min(data,key=lambda x: x[0])[0]
demax=max(data,key=lambda x: x[0])[0]

level=0
curpoint=0
for i in numpy.arange(demin,demax,1):
	while i>=data[curpoint][0]:
		level+=data[curpoint][1]
		curpoint+=1
	print i,level
