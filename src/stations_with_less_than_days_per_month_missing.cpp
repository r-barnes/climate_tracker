#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <unistd.h>
#include <cstdio>
#include "master_decoder.h"
#include <cmath>
#include <cstdlib>
using namespace std;

#define STATIONS 3000

char CLIMATE_NAMES[CLIMATE_ELEMENTS][CLIMATE_ELEMENT_NAME_LEN+1]={"TMAX","TMIN","PRCP","SNOW","SNWD","RAIN","TAVG"};

int lower_year=1900,upper_year=2009;

struct station_struct {
	string csn;
	string name;
	string state;
	float x;
	float y;
	float z;
};

map<string,struct station_struct> stations;
map<string,int> csnton;
map<int,string> ntocsn;
int station_key=1;

int amount_data[STATIONS][CLIMATE_ELEMENTS][YEARS][12];
bool do_output[CLIMATE_ELEMENTS];

int allow_months[12];

void add_station(const char csn[], const char station_name[], const char state[], const char airport[], float x, float y, float z, int firstyear, int lastyear){
	struct station_struct temp={csn,station_name,state,x,y,z};
	stations[csn]=temp;
	ntocsn[station_key]=csn;
	csnton[csn]=station_key;
	station_key++;
	return;
}

void store_daily(float datum, char flag, int year, int month, int day, int doy, char element, const char station_csn[]){
	int s=csnton[station_csn];
	if(s==0)
		fprintf(stderr,"Unrecognised station \"%s\" found in monthly data!\n",station_csn);

	if(datum==-9999) return;
	if(!allow_months[month]) return;
	if(year < lower_year || year > upper_year) return;
	do_output[element]=true;
	amount_data[s][element][year-LOWEST_YEAR][month-1]++;

	return;
}

void store_monthly(float datum, char flag, int year, int month, char element, const char station_csn[]){
	return;
}

int main(int argc, char **argv){
	int opterr, clswitch;
	bool clgood=true, stability=false;

	int end_pmonth_string;
	int segment_start=0;
	int allowed_month=-1;

	opterr = 0;
	//set_admissible_elements("0010001");
	FILE *fout;

	//INITIATE
	for(int s=0;s<STATIONS;s++)
		for(int e=0;e<CLIMATE_ELEMENTS;e++)
			for(int y=0;y<YEARS;y++)
				for(int m=0;m<12;m++)
					amount_data[s][e][y][m]=0;

	for(int e=0;e<CLIMATE_ELEMENTS;e++)
		do_output[e]=false;

	for(int i=0;i<13;i++)
		allow_months[i]=true;

	while ((clswitch = getopt (argc, argv, "+A:a:s:m:M:L:U:d:")) != -1){
		switch (clswitch){
			case 'A':
				if(!load_admissible_states(optarg))
					fprintf(stderr,"Failed to load admissible states!\n",optarg);
				break;
			case 'a':
				if(!load_admissible_stations(optarg))
					fprintf(stderr,"Failed to load admissible stations!\n",optarg);
				break;
			case 's':
				if(!ushcn_load_stations(optarg))
					fprintf(stderr,"Failed to work with USHCN stations file \"%s\"!\n",optarg);
				break;
			case 'L':
				lower_year=atoi(optarg);
				break;
			case 'U':
				upper_year=atoi(optarg);
				break;
			case 'd':
				if(!ushcn_load_daily(optarg))
					fprintf(stderr,"Failed to work with USHCN daily data file \"%s\"!\n",optarg);
				break;
			case 'M':
				end_pmonth_string=strlen(optarg)+1;

				for(int i=0;i<13;i++)
					allow_months[i]=false;

				for(int i=0;i<end_pmonth_string;i++)
					if(optarg[i]==',' || optarg[i]=='\0'){
						optarg[i]='\0';
						allowed_month=atoi(&optarg[segment_start]);
						if(!(allowed_month>13 || allowed_month<1))
							allow_months[allowed_month]=true;
						segment_start=i+1;
					}
				break;
			case '?':
			default:
				printf("Unrecognised option.\n");
				clgood=0;
		}
	}

	if(!clgood || optind==1){
		printf("Syntax: station_extract\n");
		printf("                        [-A <ADMISSIBLE STATES LIST (e.g. \"ND,MN,WY\")>]\n");
		printf("                        [-a <ADMISSIBLE STATIONS FILE>]\n");
		printf("                        [-M <Permissible Months (e.g. \"1,5,6\")>]\n");
		printf("                        [-L <Lower bound year, inclusive (default: 1900)>]\n");
		printf("                        [-U <Upper bound year, inclusive (default: 2009)>]\n");
		printf("                        [-s <USHCN STATIONS FILE>]\n");
		printf("                        [-d <USHCN DAILY DATA FILE>]\n");
		printf("It's kinda important to use the above options in this order.\n");
		printf("                        [-P <OUTPUT FILE PREFIX>]\n");
		return 0;
	}

	fout=fopen("swdpmm.dat","w");
	if(fout==NULL){
		printf("Failed to open the output file!\n");
		return -1;
	}

	fprintf(fout,"#YEAR");
	for(int e=0;e<CLIMATE_ELEMENTS;e++){
		if(!do_output[e]) continue;
		fprintf(fout," %s",CLIMATE_NAMES[e]);
	}
	fprintf(fout,"\n");

	for(int MISSING=0;MISSING<32;MISSING++){

		bool sgood[CLIMATE_ELEMENTS][STATIONS];
		for(int e=0;e<CLIMATE_ELEMENTS;e++)
			for(int s=0;s<STATIONS;s++)
				sgood[e][s]=true;

		for(int e=0;e<CLIMATE_ELEMENTS;e++){
			if(!do_output[e]) continue;
			for(int s=0;s<station_key;s++){
				for(int y=0;y<YEARS;y++){
					if(y+LOWEST_YEAR<lower_year || y+LOWEST_YEAR>upper_year) continue;
					if(!sgood[e][s]) break;
					for(int m=0;m<12;m++){
						if(!allow_months[m]) continue;
//						printf("%d-%d-%d-%d=%d\n",s,e,y+LOWEST_YEAR,m+1,amount_data[s][e][y][m]);
						if(amount_data[s][e][y][m]+MISSING<dpm(y+LOWEST_YEAR,m+1)){
							sgood[e][s]=false;
							break;
						}
					}
				}
			}
		}

		fprintf(fout,"%d",MISSING);
		for(int e=0;e<CLIMATE_ELEMENTS;e++){
			if(!do_output[e]) continue;

			int sgood_sum=0;
			for(int s=0;s<station_key;s++)
				if(sgood[e][s]) sgood_sum++;

			fprintf(fout," %d",sgood_sum);
		}
		fprintf(fout,"\n");
	}

	fclose(fout);
}
