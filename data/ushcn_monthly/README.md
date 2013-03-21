Data Retrieval
==============
Downloaded USHCN monthly data from [here](http://cdiac.ornl.gov/ftp/ushcn_v2_monthly/) on 02013-03-21.

Data downloaded was daily average temperatures ("9641C\_201112\_F52.avg.gz") and
daily total precipitation ("9641C\_201112\_F52.pcp.gz"). Both had modification dates of 02012-04-05.

The USHCN station list ("ushcn-stations.txt") was also downloaded. It had a modification date of 02011-08-25.

Data Processing
===============

 1. [gen_station_list](gen_station_list) was run.
 2. This resulted in [formatted_stations.dat](formatted_stations.dat), which is a list of the _lat, lon, title, description_ of each station, where the title is the station number and the description is its name, province, and elevation (in feet).
