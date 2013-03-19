#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "master_decoder.h"

void add_station(const char csn[], const char station_name[], const char state[], const char airport[], float x, float y, float z, int firstyear, int lastyear){
	printf("%8.3f\t%8.3f\t%s\t%s, %s\n",y,x,csn,station_name,state);
}

void store_daily(float datum, char flag, int year, int month, int day, int doy, char element, const char station_csn[]){
	return;
}

void store_monthly(float datum, char flag, int year, int month, char element, const char station_csn[]){
	printf("%8.3f\t%c\t%d/%d\t%d\t%s\n",datum,flag,year,month,element,station_csn);
}

int main(int argc, char **argv){
	int opterr, clswitch;
	bool clgood=1;
	opterr = 0;

	while ((clswitch = getopt (argc, argv, "+A:a:c:u:T:P:")) != -1){
		switch (clswitch){
			case 'A':
				if(!load_admissible_states(optarg))
					fprintf(stderr,"Failed to load admissible states!\n",optarg);
				break;
			case 'a':
				if(!load_admissible_stations(optarg))
					fprintf(stderr,"Failed to load admissible stations!\n",optarg);
				break;
			case 'c':
				if(!canada_load(optarg,1))
					fprintf(stderr,"Failed to work with Canadian data!\n");
				break;
			case 'T':
				if(!ahccd_load(optarg,CLIMATE_AVG_TEMP,false))
					fprintf(stderr,"Failed to work with AHCCD temperature data!\n");
				break;
			case 'P':
				if(!ahccd_load(optarg,CLIMATE_PRCP,true))
					fprintf(stderr,"Failed to work with AHCCD precipitation data!\n");
				break;
			case 'u':
				if(!ushcn_load_stations(optarg))
					fprintf(stderr,"Failed to work with USHCN stations file!\n");
				break;
			case '?':
			default:
				printf("Unrecognised option.\n");
				clgood=0;
		}
	}

	if(!clgood || optind==1){
		printf("Syntax: station_extract [-c <CANADIAN METAFILE>] [-u <USHCN STATIONS FILE>] [-T <AHCCD TEMP STATIONS FILE>] [-P <AHCCD PRCP STATIONS FILE>]\n");
		return 0;
	}

	return 0;
}
