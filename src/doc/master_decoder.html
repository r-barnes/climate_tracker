<html>
<head>
	<title>Master Decoder &amp; Utilities</title>
	<style>
		li:first-word {font-weight:bold;}
	</style>
</head>
<body>
<h1>The Master Decoder</h1>
<p>The Master Decoder is a tool for decoding and normalising climate data from USHCN and Canadian sources. Presently, it can read in <a href="http://cdiac.ornl.gov/epubs/ndp/ushcn/daily_doc.html">USHCN Daily</a>, <a href="http://cdiac.ornl.gov/epubs/ndp/ushcn/monthly_doc.html">USHCNv2 Monthly</a>, and <a href="http://climate.weatheroffice.gc.ca/prods_servs/index_e.html#cdcd">Canadian Daily Climate Data (CDCD)</a>. It takes all the different units and data types used by these repositories and automatically converts them to the English system and hands them to users' programs in an easy-to-understand format.</p>

<p>The master decoder is a C source file and C header file that together abstract the process of reading data from USHCN and Canadian sources. Your code can use it with the addition of three simple functions, a header statement, and by co-compiling its code when you build your project.</p>

<h1>Preparation</h1>
<p>The USHCNv2 Monthly data requires no preparation. The only files you need are the monthly data you wish to process (e.g. "9641C_200912_F52.avg") and the station data file (e.g. "ushcn-stations.txt").</p>

<p>The Adjusted and Homogenized Canadian Climate Data (AHCCD) needs some preparation. First, resave the XLS files labeled "BTemperature_stations_Dec2008.xls" and "BPrecipitation_stations_Dec2008.xls" into CSV format using fixed column with. The file should not contain commas or quote marks. Header lines may be left in, the Master Decoder will deal with these.</p>

<h1>Outputs</h1>
<p>The master decoder automagically converts all data sources to the same units and data identifiers. These are mapped through the following functions:</p>
<ul>
	<li><b>add_station</b><br><i>void add_station(const char csn[], const char station_name[], const char state[], const char airport[], float x, float y, float z, int16_t firstyear, int16_t lastyear);</i>
		<ul>
			<li><b>csn</b> is a unique station ID.<br>In the US this is the 6-character U.S. Cooperative Observer Network station identification code.<br>In Canada this is the 7-character station id.<br>Other differences aside, the two cannot be confused because of their different length.</li>
			<li><b>station_name</b> is the station name. In Canada it is up to 24 characters, in the US it is up to 30 characters.</li>
			<li><b>state</b> is either a two-character capitalised US state abbrevation<br>OR a 3-character Canadian climate district code (three-digit integer numbers).</li>
			<li><b>x</b> (longitude) is returned in decimal degrees. They are negative, because they are West. (Canadian data converted from DDMM integer format.)</li>
			<li><b>y</b> (latitude) is returned in decimal degrees. (Canadian data converted from DDMM integer format.)</li>
			<li><b>z</b> (elevation) is returned in meters.<br>The US data's most accurate digit is 1/10th meter. Canada's is 1 meter. Both are specified in the data files as meters.<br>US stations mark missing elevation as -999.9.<br>Canada doesn't document a missing elevation datum; though there are several in the set with 0.0m, none have a negative elevation.</li>
			<li><b>firstyear</b> is only non-missing for Canadian data. Missing=-9999.</li>
			<li><b>lastyear</b> is only non-missing for Canadian data. Missing=-9999.</li>
		</ul>
	</li>
	<li><b>store_daily</b><br><i>void store_daily(float datum, char flag, int year, int month, int day, int doy, char element, const char station_csn[])</i>
		<ul>
			<li><b>datum</b>: A floating-point signed integer representation of an Element. See below.</li>
			<li><b>flag</b>: One of several possible values defined below.</li>
			<li><b>year</b> is an absolute year in A.D. or C.E.</li>
			<li><b>month</b> is one-indexed. 1-12.</li>
			<li><b>day</b> is one-indexd. 1-31.</li>
			<li><b>doy</b> (Day of Year) is one-indexed. 1-366. Day 60 is February 29th and is only valid on a leap year. The user should be sure to check for this.</li>
			<li><b>element</b>: See below</li>
			<li><b>station_csn</b>: Used for identifying which station the data comes from. See above.</li>
		</ul>
	</li>
	<li><b>store_monthly</b><br><i>void store_monthly(float datum, char flag, int year, int month, char element, const char station_csn[]);</i>
		<ul>
			<li><b>datum</b>: A floating-point signed integer representation of an Element. See below.</li>
			<li><b>flag</b>: One of several possible values defined below.</li>
			<li><b>year</b> is an absolute year in A.D. or C.E.</li>
			<li><b>month</b> is one-indexed. 1-12.</li>
			<li><b>element</b>: See below</li>
			<li><b>station_csn</b>: Used for identifying which station the data comes from. See above.</li>
		</ul>
	</li>
	<li>Element
		<ul>
			<li><b>CLIMATE_ELEMENTS</b>: Lists the number of elements</li>
			<li><b>CLIMATE_ELEMENT_NAME_LEN</b>: The length of the names of the elements (w/o the '\0')</li>
			<li><b>CLIMATE_NAMES</b>: An array containing the names of the elements</li>
			<li>---</li>
			<li><b>CLIMATE_MAX_TEMP</b><br>USHCN Daily measures this in &deg;F.<br>USHCN Monthly measures this in tenth-&deg;F (mean of month).<br>Canada measures this in tenth-&deg;C.<br>Output is in &deg;F.</li>
			<li><b>CLIMATE_MIN_TEMP</b><br>USHCN Daily measures this in &deg;F.<br>USHCN Monthly measures this in tenth-&deg;F (mean of month).<br>Canada measures this in tenth-&deg;C.<br>Output is in &deg;F.</li>
			<li><b>CLIMATE_AVG_TEMP</b><br>This is not directly provided for USHCN Daily and Canadian data, but the flag is provided for use by the user.<br>USHCN Monthly measures this in tenth-&deg;F (mean of months).</li>
			<li><b>CLIMATE_PRCP</b><br>USHCN Daily measures this in hundredth-inches.<br>USHCN Monthly measures this in hundredth-inches.<br>Canada measures this in tenth-mm.<br>Output is in inches.</li>
			<li><b>CLIMATE_SNOW</b><br>USHCN Daily measures this in tenth-inches.<br>Canada measures this in tenth-cm.<br>Output is in tenth-cm.</li>
			<li><b>CLIMATE_SNWD</b><br>USHCN Daily measures this in inches.<br>Canada measures this in tenth-cm.<br>Output is in tenth-cm.</li>
			<li><b>CLIMATE_CANADA_1DAY_RAIN</b><br>USHCN Daily does not measure this.<br>Canada measures this in tenth-mm.<br>Output is in tenth-mm.</li>
		</ul>
	</li>
	<li>Flags
		<ul>
			<li><b>CLIMATE_FLAG_ESTIMATE</b></li>
			<li><b>CLIMATE_FLAG_TRACE</b></li>
			<li><b>CLIMATE_FLAG_ACCUMULATED</b></li>
			<li><b>CLIMATE_FLAG_NOT_RECORDED</b></li>
			<li><b>CLIMATE_FLAG_MISSING</b></li>
			<li><b>CLIMATE_FLAG_GOOD</b><br>Applies to both daily and monthly data</li>
			<li><b>CLIMATE_FLAG_MONTHLY_ESTIMATE</b><br>Value is an estimate of surrounding values; no original value is available</li>
			<li><b>CLIMATE_FLAG_MONTHLY_INCOMPLETE</b><br>Monthly vlaue calculated from incomplete data (1--9 days were missing)</li>
			<li><b>CLIMATE_FLAG_MONTHLY_BAD_Q</b><br>Value is an estimate from surrounding values. Original value was flagged by monthly quality control algorithms</li>
			<li><b>CLIMATE_FLAG_MONTHLY_SHORT</b><br>Value is an estimate from surrounding values; the original was part of block of monthly values that was too short to adjust in the temperature homogenization algorithm</li>
			<li><b>CLIMATE_FLAG_ADJUSTED</b><br>Used in AHCCD data. "A value that has been adjusted due to homogeneity assessment."</li>
		</ul>
	</li>
</ul>

<h1>Inputs</h1>
<p>These return true on complete success, false if any errors occur.</p>
<ul>
	<li><b>set_admissible_elements</b><br><i>bool set_admit_elements(char *elements)</i>
		<ul>
			<li><b>elements</b>: A string character-array containing one character for each element. If the character is '1' the element is admitted.<br>The value NULL can also be used to reset this so all elements are admitted, which is the default setting.</li>
		</ul>
	</li>
	<li><b>load_admisible_states</b><br><i>bool load_admissible_states(const char *admit_states_list)</i>
		<ul>
			<li><b>admit_states_list</b>: Pointer to a string character-array containing a list of delineated states to be admitted to the calculation</li>
		</ul>
	</li>
	<li><b>load_admissible_stations</b><br><i>bool load_admissible_stations(const char *admit_stations_file)</i>
		<ul>
			<li><b>admit_stations_file</b>: Pointer to a string character-array indicating a file containing a '\n' delineated list of stations to admit to the current calculation.</li>
			<li>A station is admitted if it is on EITHER the states list or stations list.</li>
			<li>This function should be called with NULL before exiting to free its memory.</li>
		</ul>
	</li>
	<li><b>canada_load</b><br><i>int canada_load(const char *fname, bool stations_only)</i>
		<ul>
			<li><b>fname</b>: Name of a MetaIndex file. This file contains the relative paths from the MetaIndex file to all the Canadian index files you wish to load.</li>
			<li><b>stations_only</b>: Load only the station information</li>
		</ul>
	</li>
	<li><b>ushcn_load_stations</b><br><i>bool ushcn_load_stations(const char *stations_file)</i>
		<ul>
			<li><b>stations_file</b>: The file containing the USHCN stations data. Usually labeled "station_data".</li>
		</ul>
	</li>
	<li><b>ushcn_load_daily</b><br><i>bool ushcn_load_daily(const char *file_ushcn_data)</i>
		<ul>
			<li><b>file_ushcn_data</b>: USHCN data file, generally labeled "us_daily.txt".</li>
		</ul>
	</li>
	<li><b>ushcn_load_monthly</b><br><i>bool ushcn_load_monthly(char *directory_monthly_data, bool literal)</i>
		<ul>
			<li><b>directory_monthly_data</b>: This is A MONTHLY DATA FILE path. The file's name will be manipulated to load the other elements of the monthly data.</li>
			<li><b>literal</b>: Unless this is set true, in which case only the indicated file is loaded.</li>
		</ul>
	</li>
</ul>

<h1>Data Order</h1>
<p>The order in which data is shuffled to the output store functions is dependent on the file structure we are given:</p>
<p>USHCN daily data is sorted in the following order: Station, Year, Month, Element, Day</p>
<p>USHCN monthly data is sorted in the following order: Station, Element, Year, Month</p>
<p>Canadian data is sorted in the following order: Station, Year, Element, Month/Day</p>

<h1>Files</h1>
<p>The master decoder consists of:</p>
<ul>
	<li><a href="master_decoder.c">master_decoder.c</a></li>
	<li><a href="master_decoder.h">master_decoder.h</a></li>
</ul>

<h1>Utilities</h1>
<p>The following handy utilites are based on the Master Decoder.</p>
<ul>
	<li><a href="station_extract.c">station_extract.c</a>: Extracts station information</li>
	<li><a href="csv_data.c">csv_data.c</a>: Extracts data to quasi-csv format</li>
	<li><a href="monthly_climate_matlab.cpp">monthly_climate_matlab.cpp</a>: Converts monthly data to Matlab format for Esther's code.</li>
</ul>
</body>
</html>
