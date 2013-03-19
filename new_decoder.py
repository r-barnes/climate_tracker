#!/usr/bin/python
from optparse import OptionParser
import struct
import sys
import numpy

parser = OptionParser(usage="%prog [options]", version="%prog 1.0")

parser.add_option("-A", dest="admissible_states", type="string", help="Admissible states list (e.g. \"ND,MN,WY\")")
parser.add_option("-a", dest="admissible_stations", help="Admissible stations file", metavar="FILE")
parser.add_option("-M", dest="admissible_months", type="string", help="Admissible months (e.g. \"1,2,3,12\")")
parser.add_option("-L", dest="lower_year", type="int", help="Lower bound year (e.g. 1900)")
parser.add_option("-U", dest="upper_year", type="int", help="Upper bound year (e.g. 2000)")
parser.add_option("-s", dest="ushcn_stations_file", type="string", help="USHCN stations file", metavar="FILE")
parser.add_option("-m", dest="ushcn_data_file", type="string", help="USHCN data file", metavar="FILE")
parser.add_option("-o", dest="out_file", help="Output File")

(options, args) = parser.parse_args()
#parser.print_help()

if options.admissible_states:
	options.admissible_states=options.admissible_states.split(",")

if options.admissible_months:
	options.admissible_months=map(lambda x: int(x),options.admissible_months.split(","))
else:
	options.admissible_months=[13]

if options.admissible_stations:
	fin=open(options.admissible_stations,"r")
	options.admissible_stations=fin.readlines()
	fin.close()
	options.admissible_stations=map(lambda x: x.replace("\n",""),options.admissible_stations)

if not options.ushcn_stations_file or not options.ushcn_data_file:
	parser.print_help()
	sys.exit(0)

stations={}

fin=open(options.ushcn_stations_file,"r")
for line in fin:
	(coop_id, lat, lon, elev, state, name, c1, c2, c3, utc_offset) = struct.unpack("6sx8sx9sx6sx2sx30sx6sx6sx6sx2s", line.strip("\n"))
	(lat, lon, elev) = map(float, (lat, lon, elev))
	if options.admissible_stations and not coop_id in options.admissible_stations:
		continue
	if options.admissible_states and not state in options.admissible_states:
		continue
	stations[coop_id]={}
	stations[coop_id]['lat']=lat
	stations[coop_id]['lon']=lon
	stations[coop_id]['elev']=elev
	stations[coop_id]['name']=name.strip()+", "+state.strip()
	stations[coop_id]['data']={}
fin.close()

overall_year_min=9999
overall_year_max=-9999

fin=open(options.ushcn_data_file,"r")
for line in fin:
	stuff = struct.unpack("6s1c4sx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1cx5s1c", line.strip("\n"))
	coop_id=stuff[0]
	if not stations.has_key(coop_id):
		continue
	element=int(stuff[1])
	year=int(stuff[2])
	overall_year_min=min(overall_year_min,year)
	overall_year_max=max(overall_year_max,year)
	values=numpy.array([float(stuff[(x-1)*2+3]) for x in options.admissible_months])
	if -9999 in values:
		stations[coop_id]['data'][year]=-9999
	else:
		if element==1 or element==2 or element==3:
			stations[coop_id]['data'][year]=values.sum()/len(values)
		elif element==4:
			stations[coop_id]['data'][year]=values.sum()
fin.close()

for s in stations.keys():
	ymin=min(stations[s]['data'].keys())
	ymax=max(stations[s]['data'].keys())
	for y in range(ymax,ymin-1,-1):
		if not stations[s]['data'].has_key(y):
			continue
		yc=1
		for n in range(max(y-29,ymin),y):
			if not stations[s]['data'].has_key(n) or stations[s]['data'][n]==-9999:
				continue
			stations[s]['data'][y]+=stations[s]['data'][n]
			yc+=1
		if yc!=30:
			del stations[s]['data'][y]
			continue

		stations[s]['data'][y]/=yc
		if element==1 or element==2 or element==3:
			stations[s]['data'][y]/=10.0
		elif element==4:
			stations[s]['data'][y]/=100.0

if not options.out_file:
	fout=sys.stdout
else:
	try:
		fout=open(options.out_file,"w")
	except:
		print "Failed to open output file!"
		sys.exit(-1)

for y in range(overall_year_min,overall_year_max+1):
	for s in stations.keys():
		if stations[s]['data'].has_key(y):
			fout.write("%d %f %f %f\n" % (y,stations[s]['lat'],stations[s]['lon'],stations[s]['data'][y]))
	fout.write("NaN NaN NaN NaN\n")

if not fout==sys.stdout:
	fout.close()

sys.exit(0)
