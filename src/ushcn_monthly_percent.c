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
#include <math.h>

#define STATION_MAX 1500
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

#define AVG_CHECK	10
#define MONTH_GOOD	6

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
uint16 quality[STATION_MAX][ELEM_TYPES][YEAR_MAX];
uint8  qualitym[STATION_MAX][ELEM_TYPES][YEAR_MAX][12];			//Quality on a monthly basis
float average[STATION_MAX][ELEM_TYPES][YEAR_MAX];
float averagem[STATION_MAX][ELEM_TYPES][YEAR_MAX][12];

//Stats variables
float avgdiff[ELEM_TYPES][12][AVG_CHECK];
float monthbuf[CLIMATE_INT];
float avgtemp[AVG_CHECK];
int avggood;
float runsum;
int numfound[ELEM_TYPES][12];

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

int dpm(int year, int month){
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

	//Test
	int q=0;


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
				for(m=0;m<12;m++){
					qualitym[i][j][k][m]=0;
					averagem[i][j][k][m]=0;
				}
			}

	//Stats variables
	for(j=0;j<ELEM_TYPES;j++)
	for(m=0;m<12;m++)
	for(l=0;l<AVG_CHECK;l++)
		avgdiff[j][m][l]=0;

	for(j=0;j<ELEM_TYPES;j++)
	for(m=0;m<12;m++)
		numfound[j][m]=0;

	//READ DATA
	while(fgets(buff, 266, data_in)!=NULL){
		i=find_coopid(buff);
		if(!coops[i].admit) continue; //Prevents reading of coop or adding it to processing list if it's unwanted
		sscanf(buff+6,"%4d%2d",&year,&month);
		if( (lyear!=0 && year<lyear) || (uyear!=0 && year>uyear) ) continue;
		if(year<year_min || year_min==-1) year_min=year;
		if(year>year_max || year_max==-1) year_max=year;
		month--;						//Zero index the months
		j=get_elem_id(buff+12);
		coops[i].found=true;
		for(day=0;day<31;day++){
			if(pgood_data(buff+16+day*8,year,month,day)){
				sscanf(buff+16+day*8,"%5d",&datum);
				if(j==ELEM_TMAX || j==ELEM_TMIN) datum=(datum-32)*5.0/9.0+273.15;			//Convert to Kelvin
				if(j==ELEM_PRCP) datum/=100;							//Convert from hundredth-inches to inches
				average[i][j][year-START_YEAR]+=datum;
				averagem[i][j][year-START_YEAR][month]+=datum;
				quality[i][j][year-START_YEAR]++;
				qualitym[i][j][year-START_YEAR][month]++;
			}
		}
		if(j==ELEM_TMAX || j==ELEM_TMIN)	//Average temp of month
			averagem[i][j][year-START_YEAR][month]/=qualitym[i][j][year-START_YEAR][month];
		else if (j==ELEM_PRCP)				//Prorated accumulated precip of month
			averagem[i][j][year-START_YEAR][month]=averagem[i][j][year-START_YEAR][month]/qualitym[i][j][year-START_YEAR][month]*dpm(year,month);
	}
	fclose(data_in);

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
	//AVERAGE years to form a climate; where there are too few years, do what we can
	for(i=0;i<coop_max;i++)
		if(coops[i].admit)
			for(j=0;j<ELEM_TYPES;j++)
				for(m=0;m<12;m++)
					for(k=year_max;k>=year_min;k--){
						avggood=0;
						//Todo: Seems to store 1900-1929 in 1930, is this okay?
						for(l=max(k-CLIMATE_INT,START_YEAR);l<k;l++){	//Todo: Check that this is 30 and attributed to the right year
							if(dpm(l,m)-qualitym[i][j][l-START_YEAR][m]<MONTH_GOOD){
								monthbuf[avggood]=averagem[i][j][l-START_YEAR][m];
								avggood++;
							}
						}
						if(avggood!=CLIMATE_INT) continue;
						numfound[j][m]++;
						shuffle_array(monthbuf,CLIMATE_INT);
						runsum=0;
						for(l=0;l<CLIMATE_INT;l++){
							runsum+=monthbuf[l];
							if(CLIMATE_INT-(l+1)<AVG_CHECK)
								avgtemp[CLIMATE_INT-(l+1)]=runsum/(l+1);
						}
						for(l=0;l<AVG_CHECK;l++)
							if(j==ELEM_TMAX || j==ELEM_TMIN)// || j==ELEM_PRCP)
								avgdiff[j][m][l]+=fabs(avgtemp[0]-avgtemp[l]);
							else
								if(avgtemp[0]!=0)
									avgdiff[j][m][l]+=fabs((avgtemp[0]-avgtemp[l])/avgtemp[0]*100);
					}

	printf("Numbers found by month:\n");
	for(j=0;j<ELEM_TYPES;j++){
		printf("%s ",elems[j]);
		for(m=0;m<12;m++)
			printf("%d (%d) ",m+1,numfound[j][m]);
		printf("\n");
	}

	for(j=0;j<ELEM_TYPES;j++)
	for(m=0;m<12;m++)
	for(l=0;l<AVG_CHECK;l++)
		avgdiff[j][m][l]/=numfound[j][m];


	//Output
	printf("For months with <%d missing days, the average absolute percent difference from means was found.\n",MONTH_GOOD);
	for(j=0;j<ELEM_TYPES;j++){
		printf("#########%s\n",elems[j]);
		printf("        Month\n");
		printf("Mis.");
		for(m=1;m<=12;m++)
			printf(" %4d",m);
		printf("\n");
		for(l=0;l<AVG_CHECK;l++){
			printf(" %7d",l);
			for(m=0;m<12;m++){
				if(j==ELEM_TMAX || j==ELEM_TMIN) avgdiff[j][m][l]*=9.0/5.0;		//Convert from dKelvin to dFahrenheit
				printf(" %7.2f",avgdiff[j][m][l]);
			}
			printf("\n");
		}
	}

	return 0;
}
