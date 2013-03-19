#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INDEX_HEADERLEN  67
#define INDEX_STATIONLEN 67
#define INDEX_BUFFLEN 67+1

#define DATA_LINE  1071
#define DATA_BUFFLEN DATA_LINE+1

#define CANADA_MAX_TEMP		1
#define CANADA_MIN_TEMP		2
#define CANADA_ONE_DAY_RAIN	3
#define CANADA_ONE_DAY_SNOW	4
#define CANADA_ONE_DAY_PRCP	5
#define CANADA_SNWD			6

typedef unsigned short uint2;
typedef signed short int2;

struct canadian_data_header{
	char id[5];				//Includes '\0'
	char csn[8];			//Includes '\0'
	char station_name[25];	//Includes '\0'
	char airport[4];		//Includes '\0'
	uint2 latitude;
	uint2 longitude;
	uint2 elev;
	uint2 firstyear;
	uint2 lastyear;
	uint2 recnumb[300];
	char data_available[300];
	char filler[123];			//Can be dropped
};

struct canadian_data_record{
	int2 data[366];
	char flags[366];
	char summary[12][13];
	char elem_type;
	int year;
	char station_name[25];	//Includes '\0'
};

/*struct canadian_index_header{
	char id[4];
	uint2 maxlat;
	uint2 minlat;
	uint2 maxlong;
	uint2 minlong;
	uint2 maxelev;
	uint2 minelev;
	uint2 earliestyear;
	uint2 latestyear;
	uint2 numberofstations;
	char districtid[3];
	char districtname[42];
};*/

struct canadian_index_record{
	char csn[5];				//Includes /0
	char station_name[25];		//Includes /0
	char airport[4];			//Includes /0
	uint2 latitude;
	uint2 longitude;
	uint2 elevation;
	uint2 firstyear[7];
	uint2 lastyear[7];
	uint2 startingrecordnumber;
};

//struct canadian_index_header *canadian_index_headers;
struct canadian_index_record *canadian_index_records=NULL;
struct canadian_data_header *canadian_data_headers=NULL;
struct canadian_data_record *canadian_data_records=NULL;
//int cihs=0;
//int cihp=0;
int cirs=0;
int cdhs=0;
int cdrs=0;
int cirp=0;
int cdhp=0;
int cdrp=0;

uint2 strtouint2(char *buff){
	unsigned char high=*(buff+1);
	unsigned char low =*buff;
	return (((uint2)high)<<8) | ((uint2)low);
}

int2 strtoint2(char *buff){		//Two's complement conversion
	signed short temp=((*(buff+1))<<8) | *buff;
	if(temp&0x8000) {temp=(-(~temp))-1;}
	return temp;
}

void read_canadian_index(FILE *fin){
	char buff[INDEX_BUFFLEN];
	struct canadian_index_record *cir_temp;
	uint2 temp;
	//Get overall header
	if(fread(buff,1,INDEX_HEADERLEN,fin)==INDEX_HEADERLEN){
		temp=strtouint2(buff+20);
		if((cir_temp=realloc(canadian_index_records,sizeof(struct canadian_index_record)*(cirs+temp)))==NULL){
			fprintf(stderr,"Failed to reallocate memory!\n");
			return;
		}
		canadian_index_records=cir_temp;
		cirs+=temp;
	} else {
		fprintf(stderr,"Failed to read header record of index file.\n");
		return;
	}

	while(fread(buff,1,INDEX_STATIONLEN,fin)==INDEX_STATIONLEN){
		strncpy(canadian_index_records[cirp].csn,buff,4);
		canadian_index_records[cirp].csn[4]='\0';
		strncpy(canadian_index_records[cirp].station_name,buff+4,24);
		canadian_index_records[cirp].station_name[24]='\0';
		strncpy(canadian_index_records[cirp].airport,buff+24,3);
		canadian_index_records[cirp].airport[3]='\0';
		canadian_index_records[cirp].latitude=strtouint2(buff+31);
		canadian_index_records[cirp].longitude=strtouint2(buff+33);
		canadian_index_records[cirp].elevation=strtouint2(buff+35);
		for(temp=0;temp<7;temp++)
			canadian_index_records[cirp].firstyear[temp]=strtouint2(buff+37+2*temp);
		for(temp=0;temp<7;temp++)
			canadian_index_records[cirp].lastyear[temp]=strtouint2(buff+51+2*temp);
		canadian_index_records[cirp].startingrecordnumber=strtouint2(buff+65);
		cirp++;
	}
}

void grab_element(FILE *fin, char *buff, int elem_type, int year, char *stationname){
	char temp;
	int i,k;
	if(fread(buff,1,DATA_LINE,fin)==DATA_LINE){
		for(i=0;i<366;i++)
			canadian_data_records[cdrp].data[i]=strtoint2(buff+2*i);
		for(i=0;i<183;i++){	//366 nibbles
			temp=*(buff+732+i);
			canadian_data_records[cdrp].flags[2*i]=(temp&0xF0)>>4;
			canadian_data_records[cdrp].flags[2*i+1]=temp&0x0F;
		}
		for(i=0;i<12;i++)
			for(k=0;k<13;k++)
				canadian_data_records[cdrp].summary[i][k]=*(buff+915+13*i+k);
		canadian_data_records[cdrp].elem_type=elem_type;
		canadian_data_records[cdrp].year=year;
		strcpy(canadian_data_records[cdrp].station_name,stationname);
		cdrp++;
	} else {
		fprintf(stderr,"Failed to read a data line.\n");
		return;
	}
}

void read_canadian_data(FILE *fin){
	char buff[DATA_BUFFLEN];
	int datas;
	char temp;
	int i,j,k;
	int year;
	char *sname;
	struct canadian_data_header *cdhtemp;
	struct canadian_data_record *cdrtemp;

	while(fread(buff,1,DATA_LINE,fin)==DATA_LINE){
		if((cdhtemp=realloc(canadian_data_headers,sizeof(struct canadian_data_header)*(cdhs+1)))==NULL){
			fprintf(stderr,"Failed to reallocate memory!\n");
			return;
		}
		canadian_data_headers=cdhtemp;
		cdhs++;
		strncpy(canadian_data_headers[cdhp].id,buff,4);
		canadian_data_headers[cdhp].id[4]='\0';
		strncpy(canadian_data_headers[cdhp].csn,buff+4,4);
		canadian_data_headers[cdhp].csn[4]='\0';
		strncpy(canadian_data_headers[cdhp].station_name,buff+11,24);
		canadian_data_headers[cdhp].station_name[24]='\0';
		strncpy(canadian_data_headers[cdhp].airport,buff+35,3);
		canadian_data_headers[cdhp].airport[3]='\0';
		canadian_data_headers[cdhp].latitude=strtouint2(buff+38);
		canadian_data_headers[cdhp].longitude=strtouint2(buff+40);
		canadian_data_headers[cdhp].elev=strtouint2(buff+42);
		canadian_data_headers[cdhp].firstyear=strtouint2(buff+44);
		canadian_data_headers[cdhp].lastyear=strtouint2(buff+46);
		for(i=0;i<300;i++)
			canadian_data_headers[cdhp].recnumb[i]=strtouint2(buff+48+2*i);

		datas=0;
		for(i=0;i<300;i++){
			temp=*(buff+648+i);
			canadian_data_headers[cdhp].data_available[i]=temp;
			if(temp&0x1) datas++;
			if(temp&0x2) datas++;
			if(temp&0x4) datas++;
			if(temp&0x8) datas++;
			if(temp&0x10) datas++;
			if(temp&0x20) datas++;
		}
		for(i=0;i<123;i++)		//Todo: Can be dropped
			canadian_data_headers[cdhp].filler[i]=*(buff+948+i);

		if((cdrtemp=realloc(canadian_data_records,sizeof(struct canadian_data_record)*(cdrs+datas)))==NULL){
			fprintf(stderr,"Failed to reallocate memory!\n");
			return;
		}
		canadian_data_records=cdrtemp;
		cdrs+=datas;

		for(j=canadian_data_headers[cdhp].firstyear-1801;j<=canadian_data_headers[cdhp].lastyear-1801;j++)
			if((temp=canadian_data_headers[cdhp].data_available[j])!=0){
				year=1801+j;
				sname=canadian_data_headers[cdhp].station_name;
				if(temp&0x1) grab_element(fin,buff,CANADA_MAX_TEMP,year,sname);
				if(temp&0x2) grab_element(fin,buff,CANADA_MIN_TEMP,year,sname);
				if(temp&0x4) grab_element(fin,buff,CANADA_ONE_DAY_RAIN,year,sname);
				if(temp&0x8) grab_element(fin,buff,CANADA_ONE_DAY_SNOW,year,sname);
				if(temp&0x10) grab_element(fin,buff,CANADA_ONE_DAY_PRCP,year,sname);
				if(temp&0x20) grab_element(fin,buff,CANADA_SNWD,year,sname);
			}

		cdhp++;
	}
}

int main(int argc, char *argv[]){
	FILE *fin;
	int i,j,k;

	if(argc<2){
		fprintf(stderr,"Syntax: <PROG> <FILENAME>\n");
		return -1;
	}

	for(i=1;i<argc;i++){
		fin=fopen(argv[i],"r");
		if(fin==NULL){
			fprintf(stderr,"Couldn't open file \"%s\"!\n",argv[i]);
			return -1;
		}

		if(strstr(argv[i],"/INDEX.")!=NULL || strncmp(argv[i],"INDEX.",6)==0)
			read_canadian_index(fin);
		else if(strstr(argv[i],"/DATA.")!=NULL || strncmp(argv[i],"DATA.",5)==0)
			read_canadian_data(fin);
		else
			fprintf(stderr,"File \"%s\" was not a recognised type!\n",argv[i]);
		fclose(fin);
	}

	printf("########INDEX######### %d\n",cirs);
	for(i=0;i<cirs;i++){
		printf("CSN: %s\n",canadian_index_records[i].csn);
		printf("Station Name: %s\n",canadian_index_records[i].station_name);
		printf("Airport: %s\n",canadian_index_records[i].airport);
		printf("Lat: %d\n",canadian_index_records[i].latitude);
		printf("Long: %d\n",canadian_index_records[i].longitude);
		printf("Elev: %d\n",canadian_index_records[i].elevation);
		printf("First Year: ");
		for(j=0;j<7;j++)
			printf("%d, ",canadian_index_records[i].firstyear[j]);
		printf("\n");
		printf("Last Year: ");
		for(j=0;j<7;j++)
			printf("%d, ",canadian_index_records[i].lastyear[j]);
		printf("\n");
		printf("Starting Record #: %d\n",canadian_index_records[i].startingrecordnumber);
		printf("----------------------\n");
	}

	printf("########DATA HEADERS######### %d\n",cdhs);
	for(i=0;i<cdhs;i++){
		printf("ID: %s\n",canadian_data_headers[i].id);
		printf("CSN: %s\n",canadian_data_headers[i].csn);
		printf("Station Name: %s\n",canadian_data_headers[i].station_name);
		printf("Airport: %s\n",canadian_data_headers[i].airport);
		printf("Lat: %d\n",canadian_data_headers[i].latitude);
		printf("Long: %d\n",canadian_data_headers[i].longitude);
		printf("Elev: %d\n",canadian_data_headers[i].elev);
		printf("First Year: %d\n",canadian_data_headers[i].firstyear);
		printf("Last Year: %d\n",canadian_data_headers[i].lastyear);
		printf("Recn: ");
		for(j=canadian_data_headers[i].firstyear-1801;j<canadian_data_headers[i].lastyear-1801;j++)
			printf("%5d",canadian_data_headers[i].recnumb[j]);
		printf("\n");
		printf("Dava: ");
		for(j=canadian_data_headers[i].firstyear-1801;j<canadian_data_headers[i].lastyear-1801;j++)
			printf("%5d",(int)canadian_data_headers[i].data_available[j]);
		printf("\n");
		printf("----------------------\n");
	}

	printf("########DATA RECORDS######### %d\n",cdrs);
	for(i=0;i<cdrs;i++){
		printf("%d,%d\n",canadian_data_records[i].elem_type,canadian_data_records[i].year);
		printf("Name: %s\n",canadian_data_records[i].station_name);
		printf("Data: ");
		for(j=0;j<366;j++)
			printf("%6d",canadian_data_records[i].data[j]);
		printf("\n");
		printf("Flag: ");
		for(j=0;j<366;j++)
			printf("%6d",canadian_data_records[i].flags[j]);
		printf("\n");
		for(j=0;j<12;j++){
			switch(canadian_data_records[i].elem_type){
				case CANADA_MAX_TEMP:
					printf("Summary ##: Mean Max|# Missing|# Days>0|Max Max|Date|Min Max|Date|Max Missing Row|# Av Missing|Max Missing Avg Row\n");
					printf("Summary %2d: ",j);
					printf("%8d|%9d|%8d|%7d|%4d|%7d|%4d|%15d|%8d\n",strtoint2(&canadian_data_records[i].summary[j][0]),canadian_data_records[i].summary[j][2],canadian_data_records[i].summary[j][3],strtoint2(&canadian_data_records[i].summary[j][4]),canadian_data_records[i].summary[j][6],canadian_data_records[i].summary[j][7],canadian_data_records[i].summary[j][8],canadian_data_records[i].summary[j][9],canadian_data_records[i].summary[j][10],canadian_data_records[i].summary[j][11]);
					break;
				case CANADA_MIN_TEMP:
				case CANADA_ONE_DAY_RAIN:
				case CANADA_ONE_DAY_SNOW:
				case CANADA_ONE_DAY_PRCP:
				case CANADA_SNWD:
					break;
			}
//			for(k=0;k<13;k++)
//				printf("%5d",canadian_data_records[i].summary[j][k]);
			printf("\n");
		}
		printf("----------------------\n");
	}

	free(canadian_data_headers);
	free(canadian_data_records);
	free(canadian_index_records);
}

