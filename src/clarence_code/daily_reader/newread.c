/*---------------------------------------------------------------------------*
READ ALL DAILY DATA FROM PRE-PACKAGED FILE.

The USHCN data are recorded in a character file of the following format. Each
record is a line of 264 characters representing a single month. Any data that
are missing in that month are marked with "missing data" codes (MISS=-9999).
However, the entire records for many consecutive months may be missing with
no notification in the file.

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
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define D20th 100*12*32                //Pseudo-days in the 20th century.
#define D21st  10*12*32                //Pseudo-days in first decade of 21st.
#define TYPES 5                        //Number of data types.
#define DAYS  (D20th+D21st)            //Number of day tracked.
#define STNS  1200                     //Number of weather stations.

#define ATMIN 0                        //Minimum temperature array.
#define ATMAX 1                        //Maximmum temperature array.
#define APRCP 2                        //Precipitation array.
#define ASNOW 3                        //Snowfall array.
#define ASNWD 4                        //Snow depth array.

#define EOM   -10000.                  //Month separator.
#define MISS   -9999.                  //Missing value code.
#define TRACE   .001                   //Trace precipitation code.

#define LINE 1000
char line[LINE], buf[LINE];

float A[TYPES][STNS][DAYS];            //Array of all data elements
int ID[STNS];                          //Table of station numbers.
int id = -1;                           //Current index into table ID.
char stid[7];                          //Character form of station name.
int  seq;                              //Sequence number for lines.


int dpm[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
//               J   F   M   A   M   J   J   A   S   O   N   D

main()
{ int i, j, k, n, fd;
  int y, m, d, a;
  float v;

  printf("Reading binary database...\n");    //Read the assembled data from a
  fd = open("ushcn.bin", O_RDONLY, 0);       //binary file.
  if(fd<0) error(762.0);
  n = sizeof(A);
  printf("Preparing for %d bytes input\n", n);
  k = read(fd, A, n);
  printf("%d of %d bytes reported read\n", k, n);
  if(k<n) error(763.0);
  printf("File read.\n");

  exit(0);                                   //Return to operating system.
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

