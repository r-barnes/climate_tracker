Data Retrieval
==============
Downloaded air temperature data from [AHCCD](http://www.ec.gc.ca/dccha-ahccd/default.asp?lang=en&n=1EEECD01-1) on 02013-03-19.
Website listed date modified as 02012-04-16.
Data downloaded was "Monthly mean of daily mean temperature".


Downloaded precipitation data from [AHCCD](http://www.ec.gc.ca/dccha-ahccd/default.asp?lang=en&n=2E5F8A39-1).
Website listed date modified as 02012-04-16.
Data downloaded was "Monthly total of daily adjusted total precipitation".

Data Processing
===============

 1. [prcp/BPrecipitation_Stations.xls](prcp/BPrecipitation_Stations.xls) was saved as [prcp/PrcpStations.csv](prcp/PrcpStations.csv)
 2. [temp/BTemperature_Stations.xls](temp/BTemperature_Stations.xls) was saved as [temp/TempStations.csv](temp/TempStations.csv)
 3. [gen_station_list](gen_station_list) was run.
 4. This resulted in [formatted_stations.dat](formatted_stations.dat), which is a list of the _lat, lon, title, description_ of each station, where the title is the station number and the description is its name, province, and elevation (in feet).
