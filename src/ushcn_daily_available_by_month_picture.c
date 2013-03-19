/*
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
#include <string.h>

#define STATION_MAX 2000
#define YEAR_MAX	180
#define START_YEAR	(2011-YEAR_MAX)
#define ELEM_TYPES	10
#define COOPID_LEN	6
#define ELEM_LEN	4
#define CLIMATE_INT	30
#define max(A,B)	((A>B)?A:B)
#define min(A,B)	((A<B)?A:B)
int quality[STATION_MAX][ELEM_TYPES][YEAR_MAX][12];
struct coop_struct {
	char id[COOPID_LEN];
	float x;
	float y;
	float z;
	char state[2];
	char name[30];
} coops[STATION_MAX];
char elems[ELEM_TYPES][ELEM_LEN];
int dpm[12]={31,28,31,30,31,30,31,31,30,31,30,31};
int coopid_max,year_min=-1,year_max=-1,elem_max=0;

int get_elem_id(char buff[]){
	static int current_elem=-1;
	int i;
	if (current_elem==-1){										//Initiate with first element	
		current_elem=0;
		strncpy(elems[current_elem],buff,ELEM_LEN);
	} else if (strncmp(elems[current_elem],buff,ELEM_LEN)!=0){	//Current element is different
		for(i=0;i<=elem_max;i++){								//Do we know it already?
			if(strncmp(elems[i],buff,ELEM_LEN)==0){				//	Yes
				current_elem=i;
				break;
			}
		}
		if (i>elem_max){										//	No
			elem_max++;
			current_elem=elem_max;
			strncpy(elems[current_elem],buff,ELEM_LEN);
		}
	}

	return current_elem;
}

int get_coopid(char buff[]){
	static int current_station=-1;
	if (current_station==-1){
		current_station=0;
		coopid_max=1;
		strncpy(coops[current_station].id,buff,COOPID_LEN);
	}
	if (strncmp(coops[current_station].id,buff,COOPID_LEN)!=0){
		current_station++;
		coopid_max++;
		strncpy(coops[current_station].id,buff,COOPID_LEN);
	}
	return current_station;
}

int find_coopid(char buff[]){
	int i;
	for(i=0;i<coopid_max;i++)
		if(strncmp(coops[i].id,buff,COOPID_LEN)==0)
			return i;
	return -1;
}

bool pgood_data(char buff[],int year,int month,int day){
	day++;
	//Is the date good (check leap years as well)?
	if (day>dpm[month-1] && !(month==2 && day==29 &&	((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)))
		return false;

	//Is the data present?
	if(strncmp(buff,"-9999",5)==0)
		return false;

	return true;
	//Could do things with flags. MFLAG is buff+5, QFLAG is buff+6, SFLAG is buff+7
}

int main(int argc, const char* argv[]){
	FILE *fin;
	char buff[266+1];	//File lines are 264 characters long. We take two extra characters for \r\n
	char *bufptr;
	bool good;
	int i,j,k,l,m;
	int day,year,month,datum;
	char getstate[2];

	if(argc!=4){
		printf("Syntax: <PROG> <DATA> <STATIONS>\nPerforms quality checking on the data.\n");
		printf("Returns a chart with station y-axis as coopid, x-axis as year, and z-axis as AVERAGE-NUMBER OF POINTS AVERAGED.\n");
		return -1;
	}

	sscanf(argv[3],"%2s",getstate);

	fin=fopen(argv[1], "r");
	if(fin==NULL){
		printf("Failed to open data file.\n");
		return -1;
	}

	//Clear array
	for(i=0;i<STATION_MAX;i++)
		for(j=0;j<ELEM_TYPES;j++)
			for(k=0;k<YEAR_MAX;k++)
				for(m=0;m<12;m++){
					quality[i][j][k][m]=0;
				}

	//READ
	while((bufptr=fgets(buff, 266, fin))!=NULL){
		sscanf(buff+6,"%4d%2d",&year,&month);
		if(year<year_min || year_min==-1) year_min=year;
		if(year>year_max || year_max==-1) year_max=year;
		for(day=0;day<31;day++){
			if(pgood_data(buff+16+day*8,year,month,day))
				quality[get_coopid(buff)][get_elem_id(buff+12)][year-START_YEAR][month]++;
		}
	}

	printf("Number of stations: %d\n",coopid_max);
	printf("Number of elements: %d\n",elem_max);
	printf("Covering %d-%d\n",year_min,year_max);
	printf("Array can hold %d-%d\n",2011-YEAR_MAX,2011);
	printf("Values are in the form of AVERAGE-QUALITY%%.\n");
	printf("AVERAGE is a simple algebraic average.\n");
	printf("QUALITY is the number of missing points in a year.\n");

	fclose(fin);

	//STATIONS, get
	fin=fopen(argv[2], "r");
	if(fin==NULL){
		printf("Failed to open stations file.\n");
		return -1;
	}

	//STATIONS, read
	while((bufptr=fgets(buff, 92, fin))!=NULL){
		i=find_coopid(buff);	//Get the key for the coopid
		sscanf(buff+6," %f %f %f %2s %30s",&coops[i].x,&coops[i].y,&coops[i].z,coops[i].state,coops[i].name);
	}

	fclose(fin);

	//Output
	for(j=0;j<=elem_max;j++){
		printf("###Element %.4s\n",elems[j]);
		for(i=0;i<coopid_max;i++){
			if(strncmp(coops[i].state,getstate,2)==0 || strncmp(getstate,"--",2)==0){
//				printf("%.6s %.2s: ",coops[i].id,coops[i].state);
				for(k=year_min;k<year_max;k++){
					for(m=0;m<12;m++)
						printf("%4d",(int)(quality[i][j][k-START_YEAR][m]*(255.0/dpm[m]))); //Number of days averaged 
				}
				printf("\n");
			}
		}
		printf("\n\n\n");
	}

}
