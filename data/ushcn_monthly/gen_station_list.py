#!/usr/bin/python
with open('ushcn-stations.txt') as fin, open('formatted_stations.dat','w') as fout:
  for s in fin:
    fout.write("%s\t%s\t%s\t%s, %s (%s ft)\n" % (s[8:15],s[16:25],s[0:6],s[36:67].strip(),s[33:35],s[26:32]))
