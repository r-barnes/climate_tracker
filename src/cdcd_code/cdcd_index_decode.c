#include <stdio.h>

#define HEADERLEN  67
#define STATIONLEN 67
#define BUFFLEN	   67+1

typedef unsigned short uint2;

uint2 strtouint2(char *buff){
	unsigned char high=*(buff+1);
	unsigned char low =*buff;
	return (((uint2)high)<<8) | ((uint2)low);
}

int main(int argc, char *argv[]){
	FILE *fin;
	char buff[BUFFLEN];

	if(argc!=2){
		fprintf(stderr,"Syntax: <PROG> <FILENAME>\n");
		return -1;
	}

	fin=fopen(argv[1],"r");
	if(fin==NULL){
		fprintf(stderr,"Couldn't open file!\n");
		return -1;
	}

/*ID AS STRING * 4
MaxLat AS INTEGER
MinLat AS INTEGER
MaxLong AS INTEGER
MinLong AS INTEGER
MaxElev AS INTEGER
MinElev AS INTEGER
EarliestYear AS INTEGER
LatestYear AS INTEGER
NumberOfStns AS INTEGER
DistrictID AS STRING * 3
DistrictName AS STRING * 42*/
	//Get overall header
	if(fgets(buff,HEADERLEN+1,fin)!=NULL){
		printf("Data Type: %.4s\n",buff);
		printf("MaxLat: %d\n",strtouint2(buff+4));
		printf("MinLat: %d\n",strtouint2(buff+6));
		printf("MaxLong: %d\n",strtouint2(buff+8));
		printf("MinLong: %d\n",strtouint2(buff+10));
		printf("Elevation: %d-%dm\n",strtouint2(buff+14),strtouint2(buff+12));
		printf("Year Range: %d-%d\n",strtouint2(buff+16),strtouint2(buff+18));
		printf("Number of Stations: %d\n",strtouint2(buff+20));
		printf("District ID: %.3s\n",buff+22);
		printf("District Name: %.42s\n",buff+25);
	} else {
		fprintf(stderr,"Failed to read header record of index file.\n");
		fclose(fin);
		return -1;
	}

	printf("--------------------------------------------------\n");

	//Get station infos
	while(fread(buff,1,STATIONLEN,fin)==STATIONLEN){
		printf("%.4s: %.24s at (%dN,%5dW) at %5dm %d-%d. Record at: %d\n",buff,buff+4,strtouint2(buff+31),strtouint2(buff+33),strtouint2(buff+35),strtouint2(buff+37),strtouint2(buff+51),strtouint2(buff+65));
	}

/*Index File data record structure:
		CSN AS STRING * 4
		StationName AS STRING * 24
		Airport AS STRING * 3
		Latitude AS INTEGER
		Longitude AS INTEGER
		Elevation AS INTEGER
		FirstYear(0 TO 6) AS INTEGER
		LastYear(0 TO 6)  AS INTEGER
		StartingRecordNumber AS INTEGER*/
}
