#ifndef __cplusplus
	typedef unsigned char	bool;
	#define true	1
	#define false	0
#endif

#ifndef MASTER_DECODER
#define MASTER_DECODER
//Find all the function names: cat master_uploader.c | grep "^[a-z].*" | grep -v static | grep "(.*)"
int dpm(int year, int month);	//Months are one-indexed
bool load_admissible_states(const char *admit_states_list);	//Call with null before ending program.
bool load_admissible_stations(const char *admit_stations_file);
bool canada_load(const char *fname, bool stations_only);
bool ushcn_load_stations(const char *stations_file);
bool ushcn_load_daily(const char *file_ushcn_data);
bool ushcn_load_monthly(char *directory_monthly_data, bool literal);
bool ahccd_load(const char *stations_file, int dtype, bool stations_only);
bool set_admissible_elements(const char *elements);

#define CLIMATE_MAX_TEMP			0
#define CLIMATE_MIN_TEMP			1
#define CLIMATE_PRCP				2
#define CLIMATE_SNOW				3
#define CLIMATE_SNWD				4
#define CLIMATE_CANADA_1DAY_RAIN	5
#define CLIMATE_AVG_TEMP			6

#define CLIMATE_ELEMENTS			7
#define CLIMATE_ELEMENT_NAME_LEN	4
//char CLIMATE_NAMES[CLIMATE_ELEMENTS][CLIMATE_ELEMENT_NAME_LEN+1]={"TMAX","TMIN","PRCP","SNOW","SNWD","RAIN","TAVG"};

#define CLIMATE_STATION_NAME_LEN	7

#define CLIMATE_FLAG_ESTIMATE		'E'
#define CLIMATE_FLAG_TRACE			'T'
#define CLIMATE_FLAG_ACCUMULATED	'A'
#define CLIMATE_FLAG_NOT_RECORDED	'C'
#define CLIMATE_FLAG_MISSING		0
#define CLIMATE_FLAG_GOOD			1

#define CLIMATE_FLAG_MONTHLY_ESTIMATE		'e'
#define CLIMATE_FLAG_MONTHLY_INCOMPLETE		'i'
#define CLIMATE_FLAG_MONTHLY_BAD_Q			'q'
#define CLIMATE_FLAG_MONTHLY_SHORT			'x'

#define CLIMATE_FLAG_ADJUSTED		'a'

#define HIGHEST_YEAR	2012			//TODO
#define LOWEST_YEAR		1801			//TODO
#define YEARS			(HIGHEST_YEAR-LOWEST_YEAR)

////////////////////////////////////////////////////////////////////////////
//							EXTERNAL FUNCTIONS
////////////////////////////////////////////////////////////////////////////
void add_station(const char csn[], const char station_name[], const char state[], const char airport[], float x, float y, float z, int firstyear, int lastyear);

void store_daily(float datum, char flag, int year, int month, int day, int doy, char element, const char station_csn[]);

void store_monthly(float datum, char flag, int year, int month, char element, const char station_csn[]);

#endif
