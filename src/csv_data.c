#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "master_decoder.h"

void add_station(const char csn[], const char station_name[], const char state[], const char airport[], float x, float y, float z, int16_t firstyear, int16_t lastyear){
	return;
}

void store_datum(float datum, char flag, uint16_t year, uint16_t month, uint16_t day, uint16_t doy, char element, const char station_csn[]){
	static char ostation_csn[8]="";
	static int oyear=0;
	static int omonth=0;
	static char oelement=-1;
	if(strcmp(ostation_csn,station_csn)!=0 || oyear!=year || omonth!=month || oelement!=element){
		strcpy(ostation_csn, station_csn);
		oyear=year;
		omonth=month;
		oelement=element;
		printf("%s, %d, %d, %d: ",station_csn,year,month,element);
	}
	printf("%8.2f(%u), ",datum,flag);
}

void store_monthly(float datum, char flag, int year, int month, char element, const char station_csn[]){
	static char ostation_csn[8]="";
	static int oyear=0;
	static char oelement=-1;
	if(oelement!=element){
		printf("##########ELEMENT %d###########\n",element);
		oelement=element;
	}
	if(strcmp(ostation_csn,station_csn)!=0 || oyear!=year){
		strcpy(ostation_csn, station_csn);
		oyear=year;
		printf("%s (%d): ",station_csn,year);
	}
	printf("%8.2f(%u), ",datum,flag);
}

int main(int argc, char **argv){
	int opterr, clswitch;
	bool clgood=1;
	opterr = 0;
	while ((clswitch = getopt (argc, argv, "+c:u:d:")) != -1){
		switch (clswitch){
			case 'c':
				if(!canada_load(optarg,0))
					fprintf(stderr,"Failed to work with Canadian data!\n");
				break;
			case 'u':
				if(!ushcn_load_stations(optarg))
					fprintf(stderr,"Failed to work with USHCN stations file!\n");
				break;
			case 'd':
				if(!ushcn_load_data(optarg))
					fprintf(stderr,"Failed to work with USHCN daily data!\n");
				break;
			case '?':
			default:
				printf("Unrecognised option.\n");
				clgood=0;
		}
	}

	if(!clgood || optind==1){
		printf("Syntax: csv_data [-c <CANADIAN METAFILE>] [-u <USHCN STATIONS FILE>] [-d <USHCN DATA FILE]\n");
		printf("-u must precede -d\n");
		return 0;
	}
}
