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

#define ANALYSIS_AVERAGES	1
#define ANALYSIS_STABILITY	2
#define ANALYSIS_GROWTH		3
char analysis_type=ANALYSIS_AVERAGES;

char CLIMATE_NAMES[CLIMATE_ELEMENTS][CLIMATE_ELEMENT_NAME_LEN+1]={"TMAX","TMIN","PRCP","SNOW","SNWD","RAIN","TAVG"};

int lower_year=1900,upper_year=2010;

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

float average_data[STATIONS][CLIMATE_ELEMENTS][YEARS];
float stddev_data[STATIONS][CLIMATE_ELEMENTS][YEARS];
float gdd[STATIONS][YEARS];
char  gddm[STATIONS][YEARS];
bool  do_gdd=false;
float gsp[STATIONS][YEARS];
char  gspm[STATIONS][YEARS];
bool  do_gsp=false;
bool do_output[CLIMATE_ELEMENTS];

int ycount[STATIONS][CLIMATE_ELEMENTS][YEARS];

int allow_months[14];
int by_months=0;

void add_station(const char csn[], const char station_name[], const char state[], const char airport[], float x, float y, float z, int firstyear, int lastyear){
	struct station_struct temp={csn,station_name,state,x,y,z};
	stations[csn]=temp;
	ntocsn[station_key]=csn;
	csnton[csn]=station_key;
	station_key++;
	return;
}

void store_daily(float datum, char flag, int year, int month, int day, int doy, char element, const char station_csn[]){
	return;
}

void store_monthly(float datum, char flag, int year, int month, char element, const char station_csn[]){
	int s=csnton[station_csn];
	if(s==0)
		fprintf(stderr,"Unrecognised station \"%s\" found in monthly data!\n",station_csn);

	if(datum==-9999) return;
	if(!allow_months[month]) return;
	if(year < lower_year || year > upper_year) return;

	if( (element==CLIMATE_PRCP || element==CLIMATE_AVG_TEMP) && (5<=month && month<=8)){
		for(int y=year;y<=year+29 && y<HIGHEST_YEAR;y++){
			if(element==CLIMATE_PRCP && 5<=month && month<=8){
				gsp[s][y-LOWEST_YEAR]+=datum;
				gspm[s][y-LOWEST_YEAR]++;
				do_gsp=true;
			}
			if(element==CLIMATE_AVG_TEMP && 5<=month && month<=8){
				gddm[s][y-LOWEST_YEAR]++;
				do_gdd=true;
				if(datum<50)
					gdd[s][y-LOWEST_YEAR]+=50-50;
				else if (datum>86)
					gdd[s][y-LOWEST_YEAR]+=86-50;
				else
					gdd[s][y-LOWEST_YEAR]+=datum-50;
			}
		}
	}

	if(month!=13 && !by_months) return;			//What follows is only for yearly values

	do_output[element]=true;

	for(int y=year;y<=year+29 && y<HIGHEST_YEAR;y++){
		average_data[s][element][y-LOWEST_YEAR]+=datum;
		stddev_data[s][element][y-LOWEST_YEAR]+=datum*datum;
		ycount[s][element][y-LOWEST_YEAR]++;	//Todo: This isn't what you thought it was
	}
	return;
}

int main(int argc, char **argv){
	int opterr, clswitch;
	bool clgood=true, stability=false;
	char *fname_prefix=NULL;

	int end_pmonth_string;
	int segment_start=0;
	int allowed_month=-1;

	opterr = 0;
	set_admissible_elements("0010001");
	FILE *fout;

	//INITIATE
	for(int s=0;s<STATIONS;s++)
		for(int e=0;e<CLIMATE_ELEMENTS;e++)
			for(int y=0;y<YEARS;y++){
				average_data[s][e][y]=0;
				stddev_data[s][e][y]=0;
				ycount[s][e][y]=0;
			}
	for(int s=0;s<STATIONS;s++)
		for(int y=0;y<YEARS;y++){
			gdd[s][y]=0;
			gsp[s][y]=0;
			gddm[s][y]=0;
			gspm[s][y]=0;
		}

	for(int e=0;e<CLIMATE_ELEMENTS;e++)
		do_output[e]=false;

	for(int i=0;i<14;i++)
		allow_months[i]=false;
	allow_months[13]=true;

	while ((clswitch = getopt (argc, argv, "+A:a:s:m:P:M:L:U:tg")) != -1){
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
			case 'm':
				if(!ushcn_load_monthly(optarg,true))
					fprintf(stderr,"Failed to work with USHCN montly data file \"%s\"!\n",optarg);
				break;
			case 'M':
				end_pmonth_string=strlen(optarg)+1;

				for(int i=0;i<14;i++)
					allow_months[i]=false;

				for(int i=0;i<end_pmonth_string;i++)
					if(optarg[i]==',' || optarg[i]=='\0'){
						by_months++;
						optarg[i]='\0';
						allowed_month=atoi(&optarg[segment_start]);
						if(!(allowed_month>13 || allowed_month<1))
							allow_months[allowed_month]=true;
						segment_start=i+1;
					}
				break;
			case 'P':
				fname_prefix=optarg;
				break;
			case 't':
				analysis_type=ANALYSIS_STABILITY;
				break;
			case 'g':
				analysis_type=ANALYSIS_GROWTH;
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
		printf("                        [-U <Upper bound year, inclusive (default: 2010)>]\n");
		printf("                        [-s <USHCN STATIONS FILE>]\n");
		printf("                        [-m <USHCN MONTHLY DATA FILE>]\n");
		printf("It's kinda important to use the above options in this order.\n");
		printf("                        [-P <OUTPUT FILE PREFIX>]\n");
		printf("                         -t Calculate stability\n");
		printf("                         -g Calculate growth\n");
		printf("Growth and stability cannot be done simultaneously. Use only one flag.\n");
		return 0;
	}

	if(analysis_type==ANALYSIS_GROWTH){
		string fname="";
		if(do_gdd){
			if(fname_prefix!=NULL){
				fname+=fname_prefix;
				fname+=".GDD";
			}
			fout=fopen(fname.c_str(),"w");
			if(fout==NULL){
				printf("Failed to open the file \"%s\"!",fname.c_str());
				return -1;
			}
			for(int y=0;y<YEARS;y++){
				for(int s=0;s<STATIONS;s++){
					if(gddm[s][y]>0){
						gdd[s][y]=gdd[s][y]/gddm[s][y]*4*30.75;
						fprintf(fout,"%d %f %f %f\n",LOWEST_YEAR+y,stations[ntocsn[s]].y,stations[ntocsn[s]].x,gdd[s][y]);
					}
				}
				fprintf(fout,"NaN NaN NaN NaN\n");
			}
			fclose(fout);
		}

		if(do_gsp){
			fname="";
			if(fname_prefix!=NULL){
				fname+=fname_prefix;
				fname+=".GSP";
			}
			fout=fopen(fname.c_str(),"w");
			if(fout==NULL){
				printf("Failed to open the file \"%s\"!",fname.c_str());
				return -1;
			}
			for(int y=0;y<YEARS;y++){
				for(int s=0;s<STATIONS;s++){
					if(gspm[s][y]>0){
						gsp[s][y]=gsp[s][y]/gspm[s][y]*4;
						fprintf(fout,"%d %f %f %f\n",LOWEST_YEAR+y,stations[ntocsn[s]].y,stations[ntocsn[s]].x,gsp[s][y]);
					}
				}
				fprintf(fout,"NaN NaN NaN NaN\n");
			}
			fclose(fout);
		}
	} else {
		for(int e=0;e<CLIMATE_ELEMENTS;e++){
			if(!do_output[e]) continue;
			string fname="";
			if(fname_prefix!=NULL){
				fname+=fname_prefix;
				fname+=".";
			}
			fname+=CLIMATE_NAMES[e];
			fout=fopen(fname.c_str(),"w");
			if(fout==NULL){
				printf("Failed to open the file \"%s\"!",fname.c_str());
				continue;
			}
			for(int y=0;y<YEARS;y++){
				for(int s=0;s<STATIONS;s++){
					if(ycount[s][e][y]<28) continue;			//Todo: This is the not right.
					average_data[s][e][y]/=ycount[s][e][y];
					stddev_data[s][e][y]/=ycount[s][e][y];	//Average of the squares
					stddev_data[s][e][y]=sqrt(stddev_data[s][e][y]-average_data[s][e][y]*average_data[s][e][y]);	//SQRT(Average of squares - Square of averages)

					switch(analysis_type){
						case ANALYSIS_AVERAGES:
							fprintf(fout,"%d %f %f %f\n",LOWEST_YEAR+y,stations[ntocsn[s]].y,stations[ntocsn[s]].x,average_data[s][e][y]);
							break;
						case ANALYSIS_STABILITY:
							fprintf(fout,"%d %f %f %f\n",LOWEST_YEAR+y,stations[ntocsn[s]].y,stations[ntocsn[s]].x,average_data[s][e][y]/stddev_data[s][e][y]);
							break;
						default:
							printf("BIG PROBLEM!\n");
							break;
					}
				}
				fprintf(fout,"NaN NaN NaN NaN\n");
			}
			fclose(fout);
		}
	}

}
