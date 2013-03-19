//Todo: Check stdint.h compatibility with printf. See inttypes.h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>		//For integer lengths
#include <math.h>		//For NaN definition
#include <errno.h>		//For conversions to float
#include "master_decoder.h"

#define CANADA_INDEX_LINE  67

#define CANADA_DATA_LINE  1071

#define USHCN_CSN_LEN		6

//const char *strcasestr(const char *haystack, const char *needle);	//Prototype

const char *admissible_states=NULL;

bool ushcn_loaded_stations=false;	//Control variable to prevent us from loading data without having loaded stations
bool canada_loaded_stations=false;
bool ahccd_loaded_stations=false;	//Todo: Consider using sep vars for temp/prcp

////////////////////////////////////////////////////////////////////////////
//							UTILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////
int doy_to_month[366]={		//Given a zero-indexed day-of-year, this tells you what month it's in
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 
10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 
11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 
12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 
12, 12, 12};

int doy_to_mday[366]={		//Given a zero-indexed day-of-year, this tells you what one-indexed day of the month it is
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 
22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 
13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 1, 2, 3, 4, 
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
26, 27, 28, 29, 30, 31, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1, 2, 3, 4, 5, 6, 7, 8, 
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 
29, 30, 31, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 
12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 
23, 24, 25, 26, 27, 28, 29, 30, 31, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 
14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1, 2, 3, 4, 
5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 
26, 27, 28, 29, 30, 31, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 
17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 1, 2, 3, 4, 5, 6, 7, 8, 
9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 
29, 30, 31};

//Given a zero-indexed month, this tells you the zero-indexed day-of-year of its first day
//Becomes one-indexed upon being passed out of master_decoder
int month_to_doy[12]={0,31,60,91,121,152,182,213,244,274,305,335};

//Tells you the number of days in a given one-indexed month of a given year
int dpm(int year, int month){
	const static int dpm_list[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	if (month==2 && (year%400==0 || (year%100 != 0 && year%4 == 0)))
		return 29;
	else
		return dpm_list[month-1];
}

//If there's a file-read error, this figures that out and displays a message
static bool fcheck(FILE *fin, const char msg[]){
	if(ferror(fin)){
		fprintf(stderr,"%s\n",msg);
		return false;
	}
	return true;
}

static bool conv_float(char sdatum[], float *datum){
	char *endptr;
	errno=0;
	*datum=strtof(sdatum,&endptr);
	if(errno!=0 || *endptr!='\0'){
		fprintf(stderr,"Error converting a float value!\n");
		return false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////
//					ELEMENT ADMISSIBILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////
const char *admit_elements=NULL;

bool set_admissible_elements(const char *elements){
	if(elements!=NULL && strlen(elements)!=CLIMATE_ELEMENTS)
		return false;
	admit_elements=elements;
}

static bool admit_element(int element){
	return (admit_elements==NULL || admit_elements[element]=='1');
}

////////////////////////////////////////////////////////////////////////////
//					STATION ADMISSIBILITY FUNCTIONS
////////////////////////////////////////////////////////////////////////////
char admissible_stations[10000][8];
int admissible_stationsn=0;
bool admissible_sorted=false;

static void swap_admissible_stations(int a, int b){
	char adtemp[8];
	strcpy(adtemp,admissible_stations[a]);
	strcpy(admissible_stations[a],admissible_stations[b]);
	strcpy(admissible_stations[b],adtemp);
}

static void sort_admissible_stations(){
	bool didwork=true;
	int i;
	while(didwork){
		didwork=false;
		for(i=1;i<admissible_stationsn;i++){
			if(strcmp(admissible_stations[i-1],admissible_stations[i])>0){
				swap_admissible_stations(i-1,i);
				didwork=true;
			}
		}
	}
	admissible_sorted=true;
}

static bool admit_station_helper(const char csn[], int l, int u){
	int m=(l+u)/2;
	int sc=strcmp(csn,admissible_stations[m]);
	if(sc==0)
		return true;
	else if(u-l==1)
		return (strcmp(csn,admissible_stations[u])==0);
	else if(u==l)
		return (sc==0);
	else if (sc<0)
		return admit_station_helper(csn, l, m);
	else if (sc>0)
		return admit_station_helper(csn, m, u);
}

static bool admit_station(const char csn[]){
	if(admissible_stationsn==0) return false;
	if(!admissible_sorted) sort_admissible_stations();
	return admit_station_helper(csn, 0, admissible_stationsn-1);
}

static void append_admissible_station(const char csn[]){
//	if(admit_station(csn)) return;			//Todo: Doesn't really matter if there are duplicates
	strcpy(admissible_stations[admissible_stationsn],csn);
	admissible_stationsn++;
	admissible_sorted=false;
}

static bool use_admit(){
	return (admissible_states!=NULL || admissible_stationsn>0);
}

static bool admit_state(const char state[]){
	if(admissible_states==NULL || strcasestr(admissible_states,state)==NULL)
		return false;
	return true;
}

bool load_admissible_states(const char *admit_states_list){
	admissible_states=admit_states_list;
	return true;
}

bool load_admissible_stations(const char *admit_stations_file){
	FILE *fin;
	char temp[8];

	if( (fin=fopen(admit_stations_file, "r"))==NULL ){
		fprintf(stderr,"Failed to open USHCN admissible stations file.\n");
		return false;
	}

	while(fscanf(fin,"%7s",temp)==1)
		append_admissible_station(temp);

	if(!fcheck(fin,"Failed to correctly read USHCN admissible stations file.")) return false;

	fclose(fin);
	return true;
}

////////////////////////////////////////////////////////////////////////////
//							UNIFIERS
////////////////////////////////////////////////////////////////////////////

static char canada_unify_flag(unsigned char flag){
	switch(flag){
		case 0:
			return CLIMATE_FLAG_GOOD;
		case 1:				//E: Estimated
			return CLIMATE_FLAG_ESTIMATE;
		case 2:				//T: Trace
			return CLIMATE_FLAG_TRACE;
		case 6:				//F: Accumulated precip and estimated
		case 5:				//A: Accumulated
			return CLIMATE_FLAG_ACCUMULATED;
		case 3:				//C: Precip occurred, recorded value is zero, look for an A soon
		case 4:				//L: Precip may or may not have occurred, recorded value is zero, look for an A soon
			return CLIMATE_FLAG_NOT_RECORDED;
		case 15:			//M: Missing
			return CLIMATE_FLAG_MISSING;
		default:			//14 is a valid flag implying February 29th on a non-leap year, but it should always be skipped
			fprintf(stderr,"An unrecognised flag \"%c\" (%u) was found in the Canadian Data.\n",flag,(unsigned int)flag);
			return CLIMATE_FLAG_MISSING;
	}
}

////////////////////////////////////////////////////////////////////////////
//							CANADA (CDCD)
////////////////////////////////////////////////////////////////////////////

static inline uint16_t canada_strtouint16_t(char *buff){
	unsigned char high=*(buff+1);
	unsigned char low =*buff;
	return (((uint16_t)high)<<8) | ((uint16_t)low);
}

/*static inline int16_t canada_strtoint16_t(char *buff){        //Two's complement conversion
    signed short temp=((*(buff+1))<<8) | *buff;
    if(temp&0x8000) {temp=(-(~temp))-1;}
    return temp;
}*/

static inline int16_t canada_strtoint16_t(char *buff){		//Two's complement conversion
	//Todo: Consider unions
	unsigned char bhigh=*(buff+1);
	unsigned char blow=*buff;
	unsigned short high=bhigh<<8;
	unsigned short low=blow;
	signed short ans=(high|low);
	if(*(buff+1)&0x8000)
		return (-(~ans))-1;
	else
		return ans;
}

static void canada_grab_element(FILE *fin, char element, int year, char *csn){
	int doy;
	char flag;
	float datum;
	char buff[CANADA_DATA_LINE+1];
	if(fread(buff,1,CANADA_DATA_LINE,fin)==CANADA_DATA_LINE){
		for(doy=0;doy<366;doy++){
			if(doy_to_mday[doy]>dpm(year,doy_to_month[doy])) continue;

			datum=(float)canada_strtoint16_t(buff+2*doy);
			flag=*(buff+732+doy/2);					//Flags follow data and are packed two per byte
			if(doy%2==0)
				flag=(flag&0xF0)>>4;
			else
				flag=(flag&0x0F);
			if(flag==14)
				fprintf(stderr,"%d of %d in %d at %ld\n",doy_to_mday[doy],dpm(year,doy_to_month[doy]),year,ftell(fin));
			flag=canada_unify_flag(flag);

			switch(element){
				case CLIMATE_MAX_TEMP:
				case CLIMATE_MIN_TEMP:
					datum=(datum/10.0)*(9.0/5.0)+32.0;		//Convert tenth-Celsius to Fahrenheit
					break;
				case CLIMATE_PRCP:
					datum=(datum/10.0)*0.0393700787;		//Convert tenth-mm to inches
					break;
				case CLIMATE_SNOW:
					datum=(datum/10.0)*0.393700787;		//Convert tenth-cm to inches
					break;
				case CLIMATE_SNWD:
					datum=(datum/10.0)*0.393700787;		//Convert tenth-cm to inches
					break;
				case CLIMATE_CANADA_1DAY_RAIN:
					datum=(datum/10.0)*0.0393700787;		//Convert tenth-mm to inches
					break;
			}

			store_daily(datum, flag, year, doy_to_month[doy], doy_to_mday[doy], doy+1, element, csn);
		}
		//I'm ignoring the data summary, which is 12 sets of 13 bytes
	} else {
		fprintf(stderr,"Failed to read a data line.\n");
		return;
	}
}

static int read_canadian_data(FILE *fin, char *csn, int16_t firstyear, int16_t lastyear){
	char buff[CANADA_DATA_LINE+1];
	char temp;
	int i,year;
	uint16_t recnumb[300];
	char data_available[300];
	int element;

	if(fread(buff,1,CANADA_DATA_LINE,fin)==CANADA_DATA_LINE){
		//Skip beginning, which is just the station information again
		for(i=0;i<300;i++)
			recnumb[i]=canada_strtouint16_t(buff+48+2*i);
		for(i=0;i<300;i++)
			data_available[i]=*(buff+648+i);

		//Skip 948-1071: Filler block to make all records the same length

		for(i=firstyear-1801;i<=lastyear-1801;i++)
			if((temp=data_available[i])!=0){
				year=1801+i;		//TODO: This shouldn't be an elseif structure. Each one of these could be true
				if(temp&0x1) canada_grab_element(fin,CLIMATE_MAX_TEMP,year,csn);
				else if(temp&0x2) canada_grab_element(fin,CLIMATE_MIN_TEMP,year,csn);
				else if(temp&0x4) canada_grab_element(fin,CLIMATE_CANADA_1DAY_RAIN,year,csn);
				else if(temp&0x8) canada_grab_element(fin,CLIMATE_SNOW,year,csn);
				else if(temp&0x10) canada_grab_element(fin,CLIMATE_PRCP,year,csn);
				else if(temp&0x20) canada_grab_element(fin,CLIMATE_SNWD,year,csn);
				else {
					fprintf(stderr,"Unknown data type \"%d\" in Canadian data near %ld!\n",temp,ftell(fin));
					return false;
				}
			}

		return true;
	} else
		return false;
}

bool read_canadian_index(FILE *fin, bool stations_only, char mipath[], char *mipath_entry_end){
	char buff[CANADA_INDEX_LINE+1];
	char districtid[4]="   ";		//This is a three-character section of the buffer
	char csn[8];
	char station_name[25];
	char airport[4];
	int i;
	int most,firstyear,lastyear;
	uint16_t coordinate;
	int16_t temp;
	float x,y,z;
	FILE *data_file=NULL;
	//Get overall header
	if(fread(buff,1,CANADA_INDEX_LINE,fin)==CANADA_INDEX_LINE){
//		temp=canada_strtouint16_t(buff+20);		//Number of Stations
		strncpy(districtid,buff+22,3);			//District ID
		strcpy(mipath_entry_end,"DATA.");
		strncpy(&mipath_entry_end[5],districtid,3);
		mipath_entry_end[8]='\0';
		if(!stations_only){
			data_file=fopen(mipath,"rb");
			if(data_file==NULL){
				fprintf(stderr,"Couldn't open \"%s\"!\n",mipath);
				return false;
			}
		}
	} else {
		fprintf(stderr,"Failed to read header record of index file \"%s\".\n",mipath);
		return false;
	}

//	fprintf(stderr,"Read header, starting on stations...\n");
	while(fread(buff,1,CANADA_INDEX_LINE,fin)==CANADA_INDEX_LINE){
		strncpy(csn,districtid,3);
		strncpy(&csn[3],buff,4);			csn[7]='\0';

		strncpy(station_name,buff+4,24);	station_name[24]='\0';
		strncpy(airport,buff+24,3);			airport[3]='\0';

//		printf("%s: %X %X: ",station_name,*(buff+31),*(buff+32));
		coordinate=canada_strtoint16_t(buff+31);
		y=((coordinate/100)+(coordinate-((coordinate/100)*100))/60.0);
		coordinate=canada_strtoint16_t(buff+33);
		x=-((coordinate/100)+(coordinate-((coordinate/100)*100))/60.0);
		z=canada_strtoint16_t(buff+35);

		//This section of the station information line contains a listing of the first and last years
		//for each element. To determine the firstyear and lastyear we will pass to the user,
		//we parse through and find the minimum and maximum of all these.
		most=canada_strtoint16_t(buff+37);
		for(i=1;i<7;i++)
			if( (temp=canada_strtoint16_t(buff+37+2*i))<most && temp!=-9999)
				most=temp;
		firstyear=most;

		most=canada_strtoint16_t(buff+51);
		for(i=1;i<7;i++)
			if( (temp=canada_strtoint16_t(buff+51+2*i))>most && temp!=-9999)
				most=temp;
		lastyear=most;

		if(!canada_loaded_stations)
			if(!use_admit() || (admit_state(districtid) || admit_station(csn))){
				if(use_admit() && admit_state(districtid))	//If it's an admissible state, add all its stations
					append_admissible_station(csn);
				add_station(csn, station_name, districtid, airport, x, y, z, firstyear, lastyear);
			}

		if(stations_only)
			continue;

		if(use_admit() && !admit_station(csn))
			continue;

		temp=canada_strtouint16_t(buff+65);	//Starting record number
		if(fseek(data_file,(temp-1)*CANADA_DATA_LINE,SEEK_SET)!=0)
			fprintf(stderr,"Failed to fseek in \"%s\"\n",mipath);
		else if(!read_canadian_data(data_file, csn, firstyear, lastyear))
			fprintf(stderr,"Failed to read station \"%s\" at %d in \"%s\".\n",station_name,temp*CANADA_DATA_LINE,mipath);
	}

	if(!stations_only)		//Todo: Is there a way to do this without having to have an if s		return (strcmp(csn,admissible_stations[l])==0 || strcmp(csn,admissible_stations[u])==0);tatement?
		fclose(data_file);

	return true;
}

bool canada_load(const char *fname, bool stations_only){
	FILE *canada_metaindex, *canada_index;
	char mipath[256];
	char *mipath_end=NULL;
	char *mipath_entry_end=NULL;
//	fprintf(stderr,"#################CANADA: CDCD DATA#################\n");
	if((canada_metaindex=fopen(fname,"r"))==NULL){
		fprintf(stderr,"Failed to open Canada Metaindex File!\n");
		return false;
	}

	strcpy(mipath,fname);
	mipath_end=strrchr(mipath,'/');		//Find last '/' on the path to CANDA_METAINDEX. This will allow us to identify where to put file names to open them.
	if(mipath_end==NULL)					//There was no '/'
		mipath_end=mipath;
	else
		mipath_end++;						//We don't want to erase the '/'

	while(fscanf(canada_metaindex,"%s",mipath_end)==1){
		mipath_entry_end=strrchr(mipath,'/');
		if(mipath_entry_end==NULL)					//There was no '/'
			mipath_entry_end=mipath;
		else
			mipath_entry_end++;						//We don't want to erase the '/'
		if((canada_index=fopen(mipath,"rb"))==NULL){
			fprintf(stderr,"Failed to open \"%s\"!\n",mipath);
			continue;
		}
		if(!read_canadian_index(canada_index, stations_only, mipath, mipath_entry_end)){
			fprintf(stderr,"Failed to read a Canadian index file!\n");
			return false;
		}
		fclose(canada_index);
	}
	if (!fcheck(canada_metaindex,"Could not read entirety of Canada MetaIndex file!")) return false;

	fclose(canada_metaindex);

	canada_loaded_stations=true;

	return true;
}

////////////////////////////////////////////////////////////////////////////
//			Adjusted and Homogenized Canadian Climate Data (AHCCD)
////////////////////////////////////////////////////////////////////////////

static bool ahccd_load_data(const char *dfile, const char station_csn[], int dtype){
	FILE *fin;
	char buff[200];	//Longest line I found was 192 characters
	int header=0;

	int m;
	int year;
	float datum;
	char flag;

	if( (fin=fopen(dfile, "r"))==NULL){
		fprintf(stderr,"Failed to open the AHCCD data file \"%s\"!\n",dfile);
		return false;
	}

	while(fgets(buff,200,fin)!=NULL){
		if(header++<4) continue;	//Throw out the first two lines

		if(sscanf(buff,"%4d,",&year)!=1){
			fprintf(stderr,"Failed to read a year from the AHCCD data file \"%s\" at line %d!\n",dfile,header);
			return false;
		}

		for(m=0;m<13;m++){
			if(sscanf(buff+5+m*11,"%f,%c,",&datum,&flag)!=2){
				fprintf(stderr,"Failed to read data from the AHCCD data file \"%s\" at line %d, point %d: \"%15s\"!\n",dfile,header,m+1,buff+5+m*10);
				return false;
			}

			switch(flag){
				case 'M':
					flag=CLIMATE_FLAG_MISSING;break;
				case 'E':
					flag=CLIMATE_FLAG_ESTIMATE;break;
				case 'e':	//Todo: May not be used in the 2010 version.
					flag=CLIMATE_FLAG_MONTHLY_ESTIMATE;break;
				case 'a':
					flag=CLIMATE_FLAG_ADJUSTED;break;
				default:
					flag=CLIMATE_FLAG_GOOD;break;
			}

			if(datum==-9999.9 || flag==CLIMATE_FLAG_MISSING)
				true;
			else if(dtype==CLIMATE_AVG_TEMP)
				datum=datum*(9.0/5.0)+32.0;			//Convert Celsius to Fahrenheit
			else if (dtype==CLIMATE_PRCP)
				datum*=0.0393700787;				//Convert mm to inches
			else{
				fprintf(stderr,"Invalid data type sent to AHCCD decoder!\n");
				return false;
			}

			store_monthly(datum, flag, year, m+1, dtype, station_csn);
		}
	}
}

bool get_str_token(char *buff, char *dest, const char * error_message){
	char * token;
	token=strtok(buff,",");
	if(token==NULL){
		fprintf(stderr,"%s\n",error_message);
		return false;
	} else {
		strcpy(dest,token);
		return true;
	}
}

bool get_int_token(char *buff, int * dest, const char * error_message){
	char * token;
	token=strtok(buff,",");
	if(token==NULL){
		fprintf(stderr,"%s\n",error_message);
		return false;
	} else {
		*dest=atoi(token);
		return true;
	}
}

bool get_float_token(char *buff, float * dest, const char * error_message){
	char * token;
	token=strtok(buff,",");
	if(token==NULL){
		fprintf(stderr,"%s\n",error_message);
		return false;
	} else {
		*dest=atof(token);
		return true;
	}
}

bool ahccd_load(const char *stations_file, int dtype, bool stations_only){
	FILE *fin;

	char buff[150];	//Longest line I found in either file was 129
	int header=0;

	char csn[8];
	char station_name[31];	//Longest name I found was 30
	char province[4];
	float x,y,z;
	int firstyear,lastyear;

	char dpath[256];
	char *dpath_prefix_end=NULL;

	char * token;

	station_name[30]='\0';
	province[3]='\0';
	csn[7]='\0';

	if( (fin=fopen(stations_file, "r"))==NULL){
		fprintf(stderr,"Failed to open AHCCD stations file.\n");
		return false;
	}

	strcpy(dpath,stations_file);
	dpath_prefix_end=strrchr(dpath,'/');	//Find last '/' character
	if(dpath_prefix_end==NULL)				//There was no '/'
		dpath_prefix_end=dpath;
	else
		dpath_prefix_end++;					//Keep the '/' character
	if(dtype==CLIMATE_AVG_TEMP){
		strcpy(dpath_prefix_end,"mm");		//Add mm or mpt to the path.
		dpath_prefix_end+=2;
	} else if (dtype==CLIMATE_PRCP){
		strcpy(dpath_prefix_end,"mt");
		dpath_prefix_end+=2;
	} else{
		fprintf(stderr,"Invalid data type sent to AHCCD decoder!\n");
		return false;
	}

	while(fgets(buff,150,fin)!=NULL){
		if(header++<4) continue;	//Throw out the first four lines

		if(!get_str_token(buff,province,"Could not retrieve AHCCD station province!"))
			return false;
		if(!get_str_token(NULL,station_name,"Could not retrieve AHCCD station name!"))
			return false;
		if(!get_str_token(NULL,csn,"Could not retrieve AHCCD station CSN!"))
			return false;
		if(!get_int_token(NULL,&firstyear,"Could not retrieve AHCCD station first-year!"))
			return false;
		strtok(NULL,","); //Month
		if(!get_int_token(NULL,&lastyear,"Could not retrieve AHCCD station last-year!"))
			return false;
		strtok(NULL,","); //Month
		if(!get_float_token(NULL,&y,"Could not retrieve AHCCD station latitude!"))
			return false;
		if(!get_float_token(NULL,&x,"Could not retrieve AHCCD station longitude!"))
			return false;
		if(!get_float_token(NULL,&z,"Could not retrieve AHCCD station elevation!"))
			return false;

		if(!use_admit() || (admit_state(province) || admit_station(csn))){
			if(use_admit() && admit_state(province))			//If it's an admissible state, add all its stations
				append_admissible_station(csn);
			add_station(csn, station_name, province, "\0", x, y, z, -9999, -9999);

			//Grab the data!
			if(!stations_only){
				strcpy(dpath_prefix_end,csn);
				strcpy(dpath_prefix_end+7,".txt");
				ahccd_load_data(dpath, csn, dtype);
			}
		}
	}

	if(!fcheck(fin,"Error reading AHCCD stations file!")) return false;

	fclose(fin);
	ahccd_loaded_stations=true;
	return true;
}


////////////////////////////////////////////////////////////////////////////
//							USHCN DAILY
////////////////////////////////////////////////////////////////////////////

bool ushcn_load_stations(const char *stations_file){
	FILE *fin;

	char buff[94];
	char csn[7];
	char station_name[31];
	char state[3];
	float x,y,z;

	station_name[30]='\0';
	state[3]='\0';
	csn[6]='\0';

	if( (fin=fopen(stations_file, "r"))==NULL){
		fprintf(stderr,"Failed to open USHCN stations file.\n");
		return false;
	}
	while(fgets(buff,94,fin)!=NULL){

		if(sscanf(buff,"%6c %f %f %f %2s %30c",csn,&y,&x,&z,state,station_name)!=6){
			fprintf(stderr,"Failed to read a station from the USHCN stations file!\n");
			return false;
		}

		if(!use_admit() || (admit_state(state) || admit_station(csn))){
			if(use_admit() && admit_state(state))			//If it's an admissible state, add all its stations
				append_admissible_station(csn);
			add_station(csn, station_name, state, "\0", x, y, z, -9999, -9999);
		}
	}

	if(!fcheck(fin,"Error reading USHCN stations file!")) return false;

	fclose(fin);
	ushcn_loaded_stations=true;
	return true;
}

bool ushcn_load_daily(const char *file_ushcn_data){	//Todo: Test this
	FILE *fin;
	int year,month,day,element;
	float datum;
	char buff[267];
	char sdatum[6]="     ";
	char flag;
	char sflag[3];
	char selement[5]="    ";
	char station_csn[USHCN_CSN_LEN+1];

	if(!ushcn_loaded_stations){
		fprintf(stderr,"USHCN stations not loaded. Cannot load USHCN daily data!\n");
		return false;
	}

	if( (fin=fopen(file_ushcn_data, "r"))==NULL ){
		printf("Failed to open USHCN daily data file.\n");
		return false;
	}

	while(fgets(buff,267,fin)!=NULL){
		if(sscanf(buff,"%6s%4d%2d%4c",station_csn,&year,&month,selement)!=4){
			fprintf(stderr,"Failed to read a line header in the USHCN daily data file!\n");
			return false;
		}

		if(use_admit() && !admit_station(station_csn))
			continue;

		switch(selement[2]){	//3rd character of each element name is unique
			case 'A':	//TMAX
				element=CLIMATE_MAX_TEMP;break;
			case 'I':	//TMIN
				element=CLIMATE_MIN_TEMP;break;
			case 'C':	//PRCP
				element=CLIMATE_PRCP;break;
			case 'O':	//SNOW
				element=CLIMATE_SNOW;break;
			case 'W':	//SNWD
				element=CLIMATE_SNWD;break;
			default:
				fprintf(stderr,"Unknown USHCN daily data type \"%c\"!\n",selement[2]);
				return false;
		}

		if(!admit_element(element))
			continue;

		for(day=0;day<=dpm(year,month)-1;day++){
			if(sscanf(buff+16+day*8,"%5c%3c",sdatum,sflag)!=2){
				fprintf(stderr,"Error reading values of USHCN daily data near %ld!\n",ftell(fin));
				return false;
			}
			if(!conv_float(sdatum,&datum)) return false;

			//Determine Flag
			if(datum==-9999)
				flag=CLIMATE_FLAG_MISSING;
			else if (sflag[0]=='T')
				flag=CLIMATE_FLAG_TRACE;
			else
				flag=CLIMATE_FLAG_GOOD;

			//Perform element-specific conversions
			if(datum!=-9999)
				switch(element){	//month_to_doy[month-1]+day+1 becomes one-indexed because of the +1
					//TODO: CHECK CONVERSIONS!
					case CLIMATE_MAX_TEMP:
					case CLIMATE_MIN_TEMP:
						break;			//Convert Fahrenheit to Fahrenheit
					case CLIMATE_PRCP:
						datum/=100;		//Convert hundredth-inches to inches
					case CLIMATE_SNOW:
						datum/=10;		//Convert tenth-inches to inches
						break;
					case CLIMATE_SNWD:
						break;			//Convert inches to inches
				}

			store_daily(datum, flag, year, month, day+1, month_to_doy[month-1]+day+1, element, station_csn);
		}
	}
	if(feof(fin))
		return true;
	else if(!fcheck(fin,"An error occurred reading a line header in the USHCN daily data!"))
		return false;
	else {
		fprintf(stderr,"Program reached an impossible place trying to read USHCN daily data!\n");
		return false;
	}

	fclose(fin);

	return true;
}

static bool ushcn_load_monthly_data(FILE *fin){
	int year,month;
	float datum;
	char buff[105];
	char sdatum[7]="      ";
	char element,flag;
	char station_csn[USHCN_CSN_LEN+1];
	while(fgets(buff,105,fin)!=NULL){
		if(sscanf(buff,"%6s%c%4d",station_csn,&element,&year)!=3){
			fprintf(stderr,"Failed to read a line header in the USHCN monthly data!\n");
			return false;
		}

		if(use_admit() && !admit_station(station_csn))
			continue;

		switch(element){
			case '1':
				element=CLIMATE_MAX_TEMP;break;
			case '2':
				element=CLIMATE_MIN_TEMP;break;
			case '3':
				element=CLIMATE_AVG_TEMP;break;
			case '4':
				element=CLIMATE_PRCP;break;
			default:
				fprintf(stderr,"Unrecognised element in USHCN monthly data!\n");
				return false;
		}

		if(!admit_element(element))
			continue;

		for(month=0;month<13;month++){	//13th month is yearly summary
			if(sscanf(buff+11+month*7,"%6c%c",sdatum,&flag)!=2){
				fprintf(stderr,"An error occurred reading values of the USHCN monthly data near %ld!\n",ftell(fin));
				return false;
			}
			if(!conv_float(sdatum,&datum)) return false;

			switch(flag){
				case ' ':	//No flag is applicable
					flag=CLIMATE_FLAG_GOOD;break;
				case 'E':	//Value is an estimate of surrounding values; no original value is available
					flag=CLIMATE_FLAG_MONTHLY_ESTIMATE;break;
				case 'I':	//Monthly vlaue calculated from incomplete data (1--9 days were missing)
					flag=CLIMATE_FLAG_MONTHLY_INCOMPLETE;break;
				case 'Q':	//Value is an estimate from surrounding values. Original value was flagged by monthly quality control algorithms
					flag=CLIMATE_FLAG_MONTHLY_BAD_Q;break;
				case 'X':	//Value is an estimate from surrounding values; the original was part of block of monthly values that was too short to adjust in the temperature homogenization algorithm
					flag=CLIMATE_FLAG_MONTHLY_SHORT;break;
				default:
					fprintf(stderr,"Unrecognised flag \"%c\" in USHCN monthly data!\n",flag);
					return false;
			}
			if(datum==-9999)
				flag=CLIMATE_FLAG_MISSING;
			else
				switch(element){
					case CLIMATE_MAX_TEMP:
					case CLIMATE_MIN_TEMP:
					case CLIMATE_AVG_TEMP:
						datum/=10.0;	//Convert tenth-Fahrenheit to Fahrenheit
						break;
					case CLIMATE_PRCP:
						datum/=100.0;	//Convert hundredth-inches to inches
						break;
				}

			store_monthly(datum, flag, year, month+1, element, station_csn);
		}
	}
	if(feof(fin))
		return true;
	else if(!fcheck(fin,"An error occurred reading a line header in the USHCN monthly data!"))
		return false;
	else {
		fprintf(stderr,"Program reached an impossible place trying to read USHCN monthly data!\n");
		return false;
	}
}

bool ushcn_load_monthly(char *directory_monthly_data, bool literal){
	FILE *fin;
	char *last_period;
	char fnames[4][4]={"max","min","pcp","avg"};
	char felements[4]={CLIMATE_MAX_TEMP,CLIMATE_MIN_TEMP,CLIMATE_PRCP,CLIMATE_AVG_TEMP};
	int i;

	if(!ushcn_loaded_stations){
		fprintf(stderr,"USHCN stations not loaded. Cannot load USHCN monthly data!\n");
		return false;
	}

	if(!literal){
		last_period=strrchr(directory_monthly_data,'.');
		if(last_period==NULL){
			fprintf(stderr,"Invalid monthly data path!\n");
			return false;
		}
		last_period++;

		for(i=0;i<4;i++){
			if(!admit_element(felements[i])) continue;
			strcpy(last_period,fnames[i]);
			if( (fin=fopen(directory_monthly_data, "r"))==NULL ){
				fprintf(stderr,"Failed to open USHCN monthly data file \"%s\"!\n",directory_monthly_data);
				return false;
			}
			if(!ushcn_load_monthly_data(fin)){
				fprintf(stderr,"Failed to work with USHCN monthly data file \"%s\"!\n",directory_monthly_data);
				return false;
			}
			fclose(fin);
		}
	} else{
		if( (fin=fopen(directory_monthly_data, "r"))==NULL ) {
			fprintf(stderr,"Failed to open USHCN monthly data file \"%s\"!\n",directory_monthly_data);
			return false;
		}
		if(!ushcn_load_monthly_data(fin)){
			fprintf(stderr,"Failed to work with USHCN monthly data file \"%s\"!\n",directory_monthly_data);
			return false;
		}
	}

	return true;
}
