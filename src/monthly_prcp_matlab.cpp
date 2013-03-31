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

int lower_year=1900,upper_year=2011;

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

float average_data[STATIONS][YEARS][13];
char average_count[STATIONS][YEARS];
bool do_output[CLIMATE_ELEMENTS];

int allow_months[14];
bool by_months=false;

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
		fprintf(stdout,"Unrecognised station \"%s\" found in monthly data!\n",station_csn);

	if(datum==-9999 || flag==CLIMATE_FLAG_MISSING) return;
	if(!allow_months[month]) return;
	if(year < lower_year || year > upper_year) return;
	if(element!=CLIMATE_PRCP) return;

	do_output[element]=true;

	average_data[s][year-LOWEST_YEAR][month-1]=datum;
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
	set_admissible_elements("0010000");
	FILE *fout;

	//INITIATE
	for(int s=0;s<STATIONS;s++)
	for(int y=0;y<YEARS;y++){
		average_count[s][y]=0;
		for(int m=0;m<13;m++)
			average_data[s][y][m]=-9999;
	}

	do_output[CLIMATE_PRCP]=false;

	for(int i=0;i<14;i++)
		allow_months[i]=false;
	allow_months[13]=true;

	while ((clswitch = getopt (argc, argv, "+A:a:s:m:P:M:L:U:c:")) != -1){
		switch (clswitch){
			case 'A':
				if(!load_admissible_states(optarg))
					fprintf(stdout,"Failed to load admissible states!\n",optarg);
				break;
			case 'a':
				if(!load_admissible_stations(optarg))
					fprintf(stdout,"Failed to load admissible stations!\n",optarg);
				break;
			case 's':
				if(!ushcn_load_stations(optarg))
					fprintf(stdout,"Failed to work with USHCN stations file \"%s\"!\n",optarg);
				break;
			case 'c':
				if(!ahccd_load(optarg, CLIMATE_PRCP, false))
					fprintf(stdout,"Failed to work with AHCCD data file \"%s\"!\n",optarg);
				break;
			case 'L':
				lower_year=atoi(optarg);
				break;
			case 'U':
				upper_year=atoi(optarg);
				break;
			case 'm':
				if(!ushcn_load_monthly(optarg,true))
					fprintf(stdout,"Failed to work with USHCN montly data file \"%s\"!\n",optarg);
				break;
			case 'M':
				end_pmonth_string=strlen(optarg)+1;
				by_months=true;

				for(int i=0;i<14;i++)
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
			case 'P':
				fname_prefix=optarg;
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
		printf("                        [-U <Upper bound year, inclusive (default: 2011)>]\n");
		printf("                        [-s <USHCN STATIONS FILE>]\n");
		printf("                        [-m <USHCN MONTHLY DATA FILE>]\n");
		printf("                        [-c <AHCCD PRECIPITATION STATION FILE>]\n");
		printf("It's kinda important to use the above options in this order.\n");
		printf("                        [-P <OUTPUT FILE PREFIX>]\n");
		return 0;
	}

	if(!do_output[CLIMATE_PRCP]) return 0;
	string fname="";
	if(fname_prefix!=NULL){
		fname+=fname_prefix;
		fname+=".";
	}
	fname+=CLIMATE_NAMES[CLIMATE_PRCP];
	fout=fopen(fname.c_str(),"w");
	if(fout==NULL){
		printf("Failed to open the file \"%s\"!",fname.c_str());
		return -1;
	}

	if(by_months){
		for(int s=1;s<station_key;s++)
			for(int y=0;y<YEARS;y++){
				if(y+LOWEST_YEAR<lower_year || y+LOWEST_YEAR>upper_year) continue;
				int mcount=0;
				average_data[s][y][12]=0;
				for(int m=0;m<12;m++)
					if(average_data[s][y][m]!=-9999){
						mcount++;
						average_data[s][y][12]+=average_data[s][y][m];
					}
//				average_data[s][y][12]/=mcount;
			}
	}

	for(int s=1;s<station_key;s++)
		for(int y=YEARS-1;y>=0;y--){		//Actually calculates a 30-year average
			if(y+LOWEST_YEAR<lower_year || y+LOWEST_YEAR>upper_year) continue;
			int ycount=0;

			if(average_data[s][y][12]==-9999)
				average_data[s][y][12]=0;
			else
				ycount++;

			for(int o=((y-29>0)?(y-29):0);o<y;o++)
				if(average_data[s][o][12]!=-9999){
					ycount++;
					average_data[s][y][12]+=average_data[s][o][12];
				}

			average_count[s][y]=ycount;

			if(ycount>=30)
				average_data[s][y][12]/=ycount;
			else
				average_data[s][y][12]=-9999;
		}

	for(int y=0;y<YEARS;y++){
		if(y+LOWEST_YEAR<lower_year || y+LOWEST_YEAR>upper_year) continue;
		for(int s=1;s<station_key;s++){
			if(average_data[s][y][12]==-9999 || average_data[s][y][12]!=average_data[s][y][12]) continue;
			fprintf(fout,"%d %f %f %f %d\n",LOWEST_YEAR+y,stations[ntocsn[s]].y,stations[ntocsn[s]].x,average_data[s][y][12],average_count[s][y]);
		}
		fprintf(fout,"NaN NaN NaN NaN\n");
	}
	fclose(fout);

}
