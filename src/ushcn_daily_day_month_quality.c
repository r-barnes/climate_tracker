/*
Todo: Note that there are no safety checks for find_coopid's return
##CLIMATE DATA##
COOP ID                 1-6             Character
YEAR                    7-10            Integer
MONTH                   11-12           Integer
ELEMENT                 13-16           Character
VALUE1                  17-21           Integer
MFLAG1                  22              Character
QFLAG1                  23              Character
SFLAG1                  24              Character
VALUE2                  25-29           Integer
MFLAG2                  30              Character
QFLAG2                  31              Character
SFLAG2                  32              Character
.               .               .
.               .               .
.               .               .
.               .               .
VALUE31                 257-261         Integer
MFLAG31                 262             Character
QFLAG31                 263             Character
SFLAG31                 264             Character

##STATION DATA##
COOP ID                 1-6             Character
LATITUDE                8-15            Real
LONGITUDE               17-25           Real
ELEVATION               27-32           Real
STATE                   34-35           Character
NAME                    37-66           Character
COMPONENT 1             68-73           Character
COMPONENT 2             75-80           Character
COMPONENT 3             82-87           Character
UTC OFFSET              89-90           Integer

//Walk down list, associating each new station found with an ID assigned by this program
//Calculate yearly averages for all elements. Then average over 30 years, or whatever is available, to find climate.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define STATION_MAX 2000
#define YEAR_MAX	180
#define START_YEAR	(2011-YEAR_MAX)
#define ELEM_TYPES	5
#define COOPID_LEN	6
#define ELEM_LEN	4
#define CLIMATE_INT	30
#define max(A,B)	((A>B)?A:B)
#define min(A,B)	((A<B)?A:B)

#define ELEM_TMAX	0
#define ELEM_TMIN	1
#define ELEM_PRCP	2
#define ELEM_SNOW	3
#define ELEM_SNWD	4
#define ELEM_TAVG	5

#define false	0
#define true	1
typedef unsigned char	bool;
typedef unsigned char	uint8;
typedef unsigned short	uint16;

struct coop_struct {
	char id[COOPID_LEN+1];
	float x;
	float y;
	float z;
	char state[3];
	char name[30];
	bool admit;
	bool found;
} coops[STATION_MAX];
char elems[ELEM_TYPES][ELEM_LEN+1]={"TMAX","TMIN","PRCP","SNOW","SNWD"};
int coop_max,year_min=-1,year_max=-1;

//Data variables
uint16	quality[STATION_MAX][ELEM_TYPES][YEAR_MAX];
uint8	qualitym[STATION_MAX][ELEM_TYPES][YEAR_MAX][12];			//Quality on a monthly basis
float	average[STATION_MAX][ELEM_TYPES][YEAR_MAX];

int dpm(int year, int month){	//Months are zero indexed
	const static int dpm_list[12]={31,28,31,30,31,30,31,31,30,31,30,31};
	if (month==1 && year%400 ==0 || (year%100 != 0 && year%4 == 0))
		return 29;
	else
		return dpm_list[month];
}

int get_elem_id(char buff[]){
	if(strncmp(buff,"TMAX",ELEM_LEN)==0)
		return ELEM_TMAX;
	else if(strncmp(buff,"TMIN",ELEM_LEN)==0)
		return ELEM_TMIN;
	else if(strncmp(buff,"PRCP",ELEM_LEN)==0)
		return ELEM_PRCP;
	else if(strncmp(buff,"SNOW",ELEM_LEN)==0)
		return ELEM_SNOW;
	else if(strncmp(buff,"SNWD",ELEM_LEN)==0)
		return ELEM_SNWD;
	else if(strncmp(buff,"TAVG",ELEM_LEN)==0)	//Virtual element for combining TMAX & TMIN
		return ELEM_TAVG;
}

int find_coopid(char buff[]){
	int i;
	for(i=0;i<coop_max;i++)
		if(strncmp(coops[i].id,buff,COOPID_LEN)==0)
			return i;
	return -1;
}

bool pgood_data(char buff[],int year,int month,int day){
	day++;
	//Is the date good (check leap years as well)?
	if (day>dpm(year,month))
		return false;

	//Is the data present?
	if(strncmp(buff,"-9999",5)==0)
		return false;

	return true;
	//Could do things with flags. MFLAG is buff+5, QFLAG is buff+6, SFLAG is buff+7
}

char* strcasestrn(char *haystack, char *needle, int n){
	int i;
	while(*haystack!='\0'){
		for(i=0;i<n;i++){
			if(*(haystack+n)=='\0') return NULL;
			if(*(haystack+n)!=*(needle+n)) break;
		}
		if(i==n) return haystack;
		haystack++;
	}
}

int main(int argc, char **argv){
	//Data reading variables
	FILE *data_in, *stations_in, *fout;
	char buff[266+1];			//Used for reading USHCN Daily Data. Lines are 264chars long. Add 2 chars for "\r\n"
	int i,j,k,l,m;				//Iteration variables
	int day,year,month,datum;	//Used as sscanf targets when decoding the buffer
	int coops_admitted=0;

	//Command-line variables
	int clswitch;						//Holds value of current command-line argument
	bool clgood=true;					//Used to determine if we should run the program
	char *file_ushcn_data, *file_station_list;
	int lyear=0,uyear=0;				//Upper and lower limit for years
	int dlimit=32,mlimit=13;			//Days per month, months per year required for month/year to be admissible
	char *station_list=NULL, *state_list=NULL;
	bool graph_total_availability=false, graph_month_quality=false;

	//Data Processing variables
	int sum[ELEM_TYPES];

	//Statistics
	int climatedef[ELEM_TYPES][32][32];
	int climatedefd=32;
	int climatedefm=31;
	int ml,dl;
	int mgood[12];
	bool cgood;
	int cperiods=0;
	bool first=true;


	////////////////////////////////
	//Process command line
	////////////////////////////////
	opterr = 0;
	while ((clswitch = getopt (argc, argv, "+L:U:D:M:I:S:tm")) != -1){
		switch (clswitch){
			case 'L':
				clgood=clgood && sscanf(optarg,"%d",&lyear)==1;break;
			case 'U':
				clgood=clgood && sscanf(optarg,"%d",&uyear)==1;break;
			case 'D':
				clgood=clgood && sscanf(optarg,"%d",&dlimit)==1;break;
			case 'M':
				clgood=clgood && sscanf(optarg,"%d",&mlimit)==1;break;
			case 'I':
				station_list=optarg;
				break;
			case 'S':
				state_list=optarg;
				break;
			case 't':
				graph_total_availability=true;
				break;
			case 'm':
				graph_month_quality=true;
				break;
			case '?':
			default:
				printf("uh oh\n");
				clgood=false;
		}
	}

	if(optind+1==argc-1 && argc>=3){
		file_ushcn_data=argv[optind];
		file_station_list=argv[optind+1];
	} else
		clgood=false;



	////////////////////////////////
	//Program Description
	////////////////////////////////
	if(!clgood){
		printf("Syntax: ushcn_proc [ARGS] <DATA> <STATIONS>\n");
		printf("\nAdmissibility\n");
		printf(" -L: Lower bound year. (Default: Lowest)\n");
		printf(" -U: Upper bound year. (Default: Highest)\n");
		printf(" -D: Fewer than D days per month must be missing for the month to count. (Default: 32)\n");
		printf(" -M: Fewer than M months per year must be missing for the year to count. (Default: 13)\n");
		printf(" -I: File containing list of stations to admit\n");
		printf(" -S: List of states to admit\n");
		printf("       Note: If neither -I nor -S are specified, all stations are admitted.\n");
		printf("       Otherwise, the station must on either -I or in a state specified by -S.");
		printf("\nCharts\n");
		printf(" -t: Chart of total availability.                 X: year, Y: %%available\n");
		printf(" -m: Grayscale chart of month quality by station. X: month, Y: station, Z: quality (0-255)\n");
		return -1;
	}


	////////////////////////////////
	//Read Stations
	////////////////////////////////
	stations_in=fopen(file_station_list, "r");			//Open
	if(stations_in==NULL){
		printf("Failed to open stations file.\n");
		return -1;
	}
	i=0;
	while(fgets(buff, 92, stations_in)!=NULL){			//Read
		sscanf(buff,"%6s %f %f %f %2s %30s",&coops[i].id[0],&coops[i].x,&coops[i].y,&coops[i].z,coops[i].state,coops[i].name);
		coops[i].state[2]='\0';
		coops[i].state[COOPID_LEN]='\0';
		coops[i].found=false;
		if(station_list==NULL && state_list==NULL)
			coops[i].admit=true;
		else
			coops[i].admit=false;
		i++;
		coop_max++;
	}
	fclose(stations_in);								//Close


	//Admit stations by station list
	if(station_list!=NULL){
		stations_in=fopen(station_list, "r");				//Open
		if(stations_in==NULL){
			printf("Failed to open station list file.\n");
			return -1;
		}
		while(fgets(buff, 20, stations_in)!=NULL){			//Read
			i=find_coopid(buff);
			if(i<0){
				printf("Coud not admit station \"%.6s\", USHCN does not have a record for it.\n",buff);
				continue;
			}
			coops[i].admit=true;
			coops_admitted++;
		}
		fclose(stations_in);								//Close
	}

	//Admit stations by state list
	if(state_list!=NULL){
		for(i=0;i<coop_max;i++)
			if(strcasestr(state_list,coops[i].state)!=NULL){
				if(!coops[i].admit) coops_admitted++;
				coops[i].admit=true;
			}
	}

	////////////////////////////////
	//Read Data
	////////////////////////////////
	data_in=fopen(file_ushcn_data, "r");
	if(data_in==NULL){
		printf("Failed to open data file.\n");
		return -1;
	}

	//INITIATE
	for(i=0;i<STATION_MAX;i++)
		for(j=0;j<ELEM_TYPES;j++)
			for(k=0;k<YEAR_MAX;k++){
				average[i][j][k]=0;
				quality[i][j][k]=0;
				for(m=0;m<12;m++)
					qualitym[i][j][k][m]=0;
			}

	//READ DATA
	while(fgets(buff, 266, data_in)!=NULL){
		i=find_coopid(buff);
		if(!coops[i].admit) continue; //Prevents reading of coop or adding it to processing list if it's unwanted
		sscanf(buff+6,"%4d%2d",&year,&month);
		if( (lyear!=0 && year<lyear) || (uyear!=0 && year>uyear) ) continue;
		if(year<year_min || year_min==-1) year_min=year;
		if(year>year_max || year_max==-1) year_max=year;
		coops[i].found=true;
		j=get_elem_id(buff+12);
		month--;
		for(day=0;day<31;day++){
			if(pgood_data(buff+16+day*8,year,month,day)){
				sscanf(buff+16+day*8,"%5d",&datum);
				average[i][j][year-START_YEAR]+=datum;
				quality[i][j][year-START_YEAR]++;
				qualitym[i][j][year-START_YEAR][month]++;
			}
		}
	}
	fclose(data_in);

//	for(i=0;i<coop_max;i++) printf("%d",coops[i].admit);
//	for(i=0;i<coop_max;i++) printf("%d",coops[i].found);
	coops_admitted=0;
	for(i=0;i<coop_max;i++)
		if(coops[i].found!=true)
			coops[i].admit=false;
		else if (coops[i].admit)
			coops_admitted++;

	//Set Limits
	if(lyear==0) lyear=year_min;
	if(uyear==0) uyear=year_max;

	printf("Number of stations: %d\n",coops_admitted);
	printf("Covering %d-%d\n",year_min,year_max);
	printf("Array can hold %d-%d\n",2011-YEAR_MAX,2011);
	printf("Values are in the form of AVERAGE-QUALITY%%.\n");
	printf("AVERAGE is a simple algebraic average.\n");
	printf("QUALITY is the number of missing points in a year.\n");

	for(j=0;j<ELEM_TYPES;j++)
	for(ml=0;ml<climatedefm;ml++)
	for(dl=0;dl<climatedefd;dl++)
		climatedef[j][ml][dl]=0;

	for(i=0;i<coop_max;i++){
		if(coops[i].admit){
			for(j=0;j<ELEM_TYPES;j++){
				for(k=uyear;k>=lyear+30;k--){
					if(first) cperiods++;
					for(dl=0;dl<climatedefd;dl++){
						for(m=0;m<12;m++)					//Clear mgood buffer
							mgood[m]=0;
						for(l=max(k-CLIMATE_INT,lyear);l<k;l++)
							for(m=0;m<12;m++)
								if(dpm(l,m)-qualitym[i][j][l-START_YEAR][m]<=dl)
									mgood[m]++;
						for(ml=0;ml<climatedefm;ml++){
							cgood=true;
							for(m=0;m<12;m++){
								if(CLIMATE_INT-mgood[m]>ml)
									cgood=false;
							}
							if(cgood)
								climatedef[j][ml][dl]++;
						}
					}
				}
				first=false;
			}
		}
	}

	printf("Made it with %d periods.!\n",cperiods);

	for(j=0;j<ELEM_TYPES;j++){
		printf("#!###%.4s\n",elems[j]);
		printf("   ");for(dl=0;dl<climatedefd;dl++) printf("%2d ",dl);printf("\n");
		for(ml=0;ml<climatedefm;ml++){
			printf("%2d ",ml);
			for(dl=0;dl<climatedefd;dl++)
				printf("%2d ",(int)(climatedef[j][ml][dl]/(double)coops_admitted/cperiods*100));
			printf("\n");
		}
	}

	//AVERAGE years to form a climate; where there are too few years, do what we can
/*	for(i=0;i<coop_max;i++)
		if(coops[i].admit){
			for(j=0;j<ELEM_TYPES;j++)
				for(k=year_max;k>=year_min;k--){
					for(l=max(k-CLIMATE_INT,START_YEAR)+1;l<k;l++){	//Is it a problem that we're adding in years that may not have data?
						average[i][j][k-START_YEAR]+=average[i][j][l-START_YEAR];	//Sum up the averages of the years
						quality[i][j][k-START_YEAR]+=quality[i][j][l-START_YEAR];	//Sum up the quality of the years
					}
					if(quality[i][j][k-START_YEAR]>0) average[i][j][k-START_YEAR]/=quality[i][j][k-START_YEAR];
				}
		}*/


}
