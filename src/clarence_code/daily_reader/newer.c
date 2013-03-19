/*---------------------------------------------------------------------------*
PROCESS  DAILY WEATHER DATA.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define D20th 100*12*32                //Pseudo-days in the 20th century.
#define D21st  10*12*32                //Pseudo-days in first decade of 21st.
#define DAYS  (D20th+D21st)            //Number of day tracked.
#define TYPES 5                        //Number of data types.
#define STNS  1200                     //Number of weather stations.

#define ATMIN 0                        //Minimum temperature array.
#define ATMAX 1                        //Maximum temperature array.
#define APRCP 2                        //Precipitation array.
#define ASNOW 3                        //Snowfall array.
#define ASNWD 4                        //Snow depth array.

#define EOD   -10001.                  //End of days for this station.
#define EOM   -10000.                  //Month separator.
#define MISS   -9999.                  //Missing value code.
#define TRACE   .001                   //Trace precipitation code.

#define LINE 1000
char line[LINE], buf[LINE];

float A[TYPES][STNS][DAYS+1];          //Array of all data elements
int ID[STNS];                          //Table of station numbers.
int id = -1;                           //Current index into table ID.
char stid[7];                          //Character form of station name.
int  seq;                              //Sequence number for lines.

main(int argv, char *argc[])
{ int i, j;
  int data, miss;

  FileRead();
  printf("%d stations\n",id+1);

  for(i=0; i<=id; i++)
  { for(data=miss=0, j=0; A[ATMIN][i][j]!=EOD; j++)
    { if(A[ATMIN][i][j]==EOM) continue;
      if(A[ATMIN][i][j]!=MISS && A[ATMAX][i][j]!=MISS) data += 1;
      else                                             miss += 1; }
    printf("stn=%d data=%d miss=%d tot=%d\n", ID[i], data, miss, data+miss);
  }

  for(i=0; i<=id; i++)
  { for(j=0; A[ATMIN][i][j]!=EOD; j++)
    {      if(A[ATMIN][i][j]==EOM)  printf(".");
      else if(A[ATMIN][i][j]!=MISS && A[ATMAX][i][j]!=MISS) printf("+");
      else                                                  printf("0"); }
    printf("\n\n\n\n");
  }


}


/*----------------------------------------------------------------------------*
READ DAILY WEATHER DATA

The USHCN data are recorded in a character file of the following format.

 Variable      Columns    Type
 COOP ID       1-6        Character
 YEAR          7-10       Integer
 MONTH         11-12      Integer
 ELEMENT       13-16      Character
 VALUE1        17-21      Integer
 MFLAG1        22         Character
 QFLAG1        23         Character
 SFLAG1        24         Character
 VALUE2        25-29      Integer
 MFLAG2        30         Character
 QFLAG2        31         Character
 SFLAG2        32         Character
 .             .          .
 .             .          .
 .             .          .
 .             .          .
 VALUE31       257-261    Integer
 MFLAG31       262        Character
 QFLAG31       263        Character
 SFLAG31       264        Character

The number of days from 1900 to 2010 is approximately 40,000 (39,187 to be
exact), and the number of weather stations is less than 1200. That multiplies
out to about 48 million entries per type of data (precipitation, mininimum
temperature, etc.) At 32 bits (4 bytes) per entry, that is approximately 200
million bytes per table. Outrageous in the old days (e.g. 2007), but not a
problem now. Five such arrays are needed, occupying approximately one gigabyte.

ENTRY: 'stdin' contains the data to be processed as a file in the above format.

EXIT:  'A' contains the input data organized for direct access.
*/

int dpm[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//               J   F   M   A   M   J   J   A   S   O   N   D

FileRead()
{ int i, j, k, n, fd;
  int y, m, d, a;
  float v;

  while(fgets(line, LINE, stdin))            //Advance to the next line.
  { seq += 1;
    if(0) printf("%4d. %s", seq, line);

    k = strlen(line);                        //Check the line for valid
    for(i=0; i<k; i++)                       //characters.
    { if(line[i]=='\n') continue;
      if(line[i]==' ' || line[i]=='-') continue;
      if(line[i]>='0' && line[i]<='9') continue;
      if(line[i]>='A' && line[i]<='Z') continue;
      error1(776.0, &line[i]);
      }

    subcpy(buf, line, 0, 6);                 //Extract the station number and
    if(ne(buf,stid))                         //save it under a new index if it
    { ID[++id] = atoi(buf);                  //this is a new station.
      strcpy(stid,buf); }

    subcpy(buf, line, 6, 4);                 //Extract the year and make sure it
    y = atoi(buf);                           //is within range.
    if(y<1900) continue;
    if(y>2010) error(785.0);
    y -= 1900;

    subcpy(buf, line, 10, 2);                //Extract the month and make sure
    m = atoi(buf);                           //it is within range.
    if(m<1 || m>12) error(786.0);

    subcpy(buf, line, 12, 4);                //Determine the type of element.
         if(eq(buf,"TMIN")) a = ATMIN;
    else if(eq(buf,"TMAX")) a = ATMAX;
    else if(eq(buf,"PRCP")) a = APRCP;
    else if(eq(buf,"SNOW")) a = ASNOW;
    else if(eq(buf,"SNWD")) a = ASNWD;
    else error1(787.0, buf);

    if((m==2 &&                              //If this is a leap year, add a day
      (y%4==0 && y%100!=0) || y%400==0))     //to February.
         dpm[1] = 29;
    else dpm[1] = 28;

    for(i=0; i<32; i++)                      //Advance to the next day and check
    { if(i>=dpm[m-1]) v = EOM;               //for end of month.

      else
      { subcpy(buf, line, 8*i+16, 5);        //Extract the next value from the
        v = atof(buf); }                     //line.

      if(v!=MISS)
      {      if(a==APRCP) v /= 100;          //Convert from hundredths or tenths
        else if(a==ASNOW) v /=  10;          //of an inch, depending on type.

        subcpy(buf, line, 8*i+21, 2);        //Depending on the flag field,
        if(buf[0]=='T') v = TRACE;           //record trace precipitation or
      //if(buf[1]!=' ') v = MISS;            //missing data.
      }

      A[a][id][y*12*32+m*32+i] = v;          //Store the final value in the array.
    }
  }

  for(a=0; a<TYPES; a++)                     //Scan all arrays and fill any
  for(i=0; i<=id; i++)                       //months that were absent from the
  for(j=0; j<DAYS; j+=32)                    //data file, for whatever reason,
  { if(A[a][i][j+31]==0)                     //with missing data codes, and with
    { for(k=0; k<31; k++)                    //a normal end of month code.
        A[a][i][j+k] = MISS;
      A[a][i][j+31]  = EOM; } }

  for(a=0; a<TYPES; a++)                     //Mark the end of the line for
  for(i=0; i<=id; i++)                       //every station.
    A[a][i][DAYS] = EOD;

  printf("%d lines read.\n", seq);
}

/*----------------------------------------------------------------------------*
COPY SUBSTRING

This routine copies a fixed-length substring to a specified area, with a null
terminator (binary zero terminator).

ENTRY: 'to' defines the destination, which must have enough room for 'lth'+1
        characters.
       'from' defines the source string. This string is treated as if it ends
        with an infinite series of blanks.
       'beg' defines the starting position in 'from', with 0 as the first
        character.
       'lth' contains the number of characters to be copied.

EXIT:  'to' contains the specified substring, terminated with a null character.
*/

subcpy(char *to, char *from, int beg, int lth)
{ int i;

 i = strlen(from);                           //Make sure the starting point is
 if(beg>i) beg = i;                          //not beyond the end of the string.

 for(i=0; i<lth; i++)                        //Copy the specified number of
   if(from[beg]==0 || from[beg]=='\n')       //characters, filling to the end
     to[i] = ' ';                            //with blanks.
   else to[i] = from[beg++];

 to[i] = 0;                                  //Terminate the string.
}

/*----------------------------------------------------------------------------*
COMPARISON ROUTINES

These routine simply compare two character strings and return a true or false
condition based on the test being performed.

ENTER: 'a' and 'b' define the two strings to be compared.

EXIT:  'eq', 'ne', 'lt', 'le', 'gt', and 'ge' are 1 if the two strings are
        equal, not equal, etc, and 0 otherwise.
*/

int eq(char *a, char *b) { return strcmp(a,b)==0; }
int ne(char *a, char *b) { return strcmp(a,b)!=0; }
int lt(char *a, char *b) { return strcmp(a,b)< 0; }
int le(char *a, char *b) { return strcmp(a,b)<=0; }
int gt(char *a, char *b) { return strcmp(a,b)> 0; }
int ge(char *a, char *b) { return strcmp(a,b)>=0; }

/*----------------------------------------------------------------------------*
ERRORS ARISING
*/

error(double n)
{ printf("ERROR %g\n", n);
  if(n<700) return;
  exit(3);
}

error1(double n, char *s)
{ printf("ERROR %g (%s)\n", n, s);
  if(n<700) return;
  exit(3);
}

// CLARENCE LEHMAN AND SHELBY WILLIAMS, MARCH 2010.

