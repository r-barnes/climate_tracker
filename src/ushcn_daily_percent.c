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

#define AVG_CHECK	28					//Statistic variable for average checing

struct coop_struct {
	char id[COOPID_LEN+1];
	float x;
	float y;
	float z;
	char state[3];
	char name[30];
	bool admit;
} coops[STATION_MAX];
char elems[ELEM_TYPES][ELEM_LEN+1]={"TMAX","TMIN","PRCP","SNOW","SNWD"};
int coop_max,year_min=-1,year_max=-1;

//Data variables
uint16 quality[STATION_MAX][ELEM_TYPES][YEAR_MAX];
uint8 qualitym[STATION_MAX][ELEM_TYPES][YEAR_MAX][12];			//Quality on a monthly basis
float average[STATION_MAX][ELEM_TYPES][YEAR_MAX];

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

void shuffle_array(float ar[], int n){ //Uses the Knuth-Fisher-Yates algorithm
	int i, c, r;
	static bool init=false;
//	if (!init){init=true;srand(time(NULL));}
	for(i=n-1;i>0;i--){
		r=(int)((rand()/(double)RAND_MAX)*(i+1));
		c=ar[i];
		ar[i]=ar[r];
		ar[r]=c;
	}
}

int main(int argc, char **argv){
	//Data reading variables
	FILE *data_in, *stations_in, *fout;
	char buff[300];				//Used for reading USHCN Daily Data. Lines are 264chars long. Add 2 chars for "\r\n"
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

	//Statistic Variables for Averaging
	double asum[12][ELEM_TYPES][AVG_CHECK];
	double psum[AVG_CHECK];
	int mgood[12][ELEM_TYPES];
	double ppsum;
	float daybuff[31];
	int pgood;


	////////////////////////////////
	//Process command line
	////////////////////////////////
	opterr = 0;
	while ((clswitch = getopt (argc, argv, "+L:U:D:M:I:S:")) != -1){
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
		return -1;
	}


	////////////////////////////////
	//Read Stations
	////////////////////////////////
	printf("Reading stations file...\n");
	stations_in=fopen(file_station_list, "r");			//Open
	if(stations_in==NULL){
		printf("failure.\n");
		return -1;
	}
	i=0;
	while(fgets(buff, 92, stations_in)!=NULL){			//Read
		sscanf(buff,"%6s %f %f %f %2s %30s",&coops[i].id[0],&coops[i].x,&coops[i].y,&coops[i].z,coops[i].state,coops[i].name);
		coops[i].state[2]='\0';
		coops[i].state[COOPID_LEN]='\0';
		if(station_list==NULL && state_list==NULL)
			coops[i].admit=true;
		else
			coops[i].admit=false;
		i++;
		coop_max++;
	}
	fclose(stations_in);								//Close
	printf("done.\n");

	//Admit stations by station list
	if(station_list!=NULL){
		printf("Admitting stations...\n");
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
		printf("done.\n");
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

	//INITATE STATISTICS
	printf("Initiating storage arrays...\n");
	for(m=0;m<12;m++)
	for(j=0;j<ELEM_TYPES;j++){
		mgood[m][j]=0;
		for(k=0;k<AVG_CHECK;k++)
			asum[m][j][k]=0;
	}
	printf("done.\n");

	//READ DATA
	printf("Reading data...\n");
	while(fgets(buff, 300, data_in)!=NULL){
		i=find_coopid(buff);
		if(!coops[i].admit) continue;
		sscanf(buff+6,"%4d%2d",&year,&month);
		if(year<year_min || year_min==-1) year_min=year;
		if(year>year_max || year_max==-1) year_max=year;
		j=get_elem_id(buff+12);
		pgood=0;
		for(day=0;day<31;day++){
			if(pgood_data(buff+16+day*8,year,month-1,day)){
				sscanf(buff+16+day*8,"%5f",&daybuff[day]);
				pgood++;
			}
		}
		if(pgood<dpm(year,month-1)) continue;
		mgood[month-1][j]++;						//The month is good
		shuffle_array(daybuff,dpm(year,month-1));	//Shuffle the month
		ppsum=0;
		for(i=0;i<dpm(year,month-1);i++){
			if(j==ELEM_TMAX || j==ELEM_TMIN) daybuff[i]=(daybuff[i]-32)*5.0/9.0+273.15;	//Convert to Kelvin
			ppsum+=daybuff[i];
			if(dpm(year,month-1)-(i+1)<AVG_CHECK)
				if(j==ELEM_TMAX || j==ELEM_TMIN)
					psum[dpm(year,month-1)-(i+1)]=ppsum/(i+1);	//Daily average
				else
					psum[dpm(year,month-1)-(i+1)]=ppsum;		//Accumulation. Only using for PRCP right now.
		}
		if(psum[0]>0){
			for(i=0;i<AVG_CHECK;i++)
				asum[month-1][j][i]+=abs(psum[i]-psum[0]);					//Absolute difference
//				asum[month-1][j][i]+=abs((psum[i]-psum[0])*100.0/psum[0]);	//Percent difference
		}
	}
	printf("done.\n");

	printf("Averaging over complete months...\n");
	for(j=0;j<ELEM_TYPES;j++)
	for(m=0;m<12;m++)
	for(k=0;k<AVG_CHECK;k++)
		asum[m][j][k]/=mgood[m][j];
	printf("done.\n");

	printf("Results:\n");
	printf("Elemmax: %d, AVG_Check %d\n",ELEM_TYPES,AVG_CHECK);
	for(j=0;j<ELEM_TYPES;j++){
		if(!(j==ELEM_TMAX || j==ELEM_TMIN || j==ELEM_PRCP)) continue;
		printf("\n#!###%.4s\n",elems[j]);
		for(k=0;k<AVG_CHECK;k++){
			printf("%d ",k);	//# of missing points
			for(m=0;m<12;m++){
				if(j==ELEM_TMAX || j==ELEM_TMIN) asum[m][j][k]*=9.0/5.0;	//Convert from dKelvin to dFahrenheit
				if(j==ELEM_PRCP) asum[m][j][k]/=100;	//Convert from hundredths of inches to inches
				printf("%2.2f ",asum[m][j][k]);
			}
			printf("\n");
		}
	}

	return 0;
}
