
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_int64.h"

#include <math.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
//------------------------

int _CheckInt64Len(char* s)
{
if( !s ) return 0;

int cnt = 0;
int i = 0;
while( s[i] )
  {
   if( isdigit(s[i]) ) cnt++;
   else if( s[i] != ' ' && s[i] != '.' && s[i] != ',' && s[i] != '+' && s[i] != '-' ) return 0;
   i++;
  }
if( cnt > 18 ) return 0;
else return 1;
}
//----------------------------------

my_int64 _Int64Rescale(my_int64 x, int OldScale, int NewScale, int Option) // Option = (0-normalnie, 1-w gore, 2- wdol)
{
if( NewScale == OldScale ) return x;
else if( NewScale > OldScale )
  {
   int diff = NewScale - OldScale;
   my_int64 tens = 10;
   if( diff == 2 ) tens = 100;
   else if( diff == 3 ) tens = 1000;
   else if( diff == 4 ) tens = 10000;
   else if( diff == 5 ) tens = 100000;
   else if( diff == 6 ) tens = 1000000;
   else if( diff == 7 ) tens = 10000000;
   else if( diff == 8 ) tens = 100000000;
   else if( diff == 9 ) tens = 1000000000;
   else if( diff > 1 )
     {
      for(int i=1; i<diff; i++) tens *= 10;
     }
   x *= tens;
  }
else
  {
   int diff = OldScale - NewScale;

   /*my_int64 tens = 1;
   for(int i=0; i<diff; i++) tens *= 10;*/

   my_int64 tens = 10;
   if( diff == 2 ) tens = 100;
   else if( diff == 3 ) tens = 1000;
   else if( diff == 4 ) tens = 10000;
   else if( diff == 5 ) tens = 100000;
   else if( diff == 6 ) tens = 1000000;
   else if( diff == 7 ) tens = 10000000;
   else if( diff == 8 ) tens = 100000000;
   else if( diff == 9 ) tens = 1000000000;
   else if( diff > 1 )
     {
      for(int i=1; i<diff; i++) tens *= 10;
     }

   my_int64 x1 = x;
   if( tens > 1 ) x1 /= tens;

   if( x >= 0 )
     {
      my_int64 diff = x - x1*tens;
      if( Option == 0 ) x = ( diff >= tens/2 )? x1+1 : x1;
      else if( Option==1 ) x = ( diff > 0 )? x1+1 : x1;
      else if( Option==2 ) x = x1;
     }
   else
     {
      my_int64 diff = x1*tens - x;
      if( Option == 0 ) x = ( diff >= tens/2 )? x1-1 : x1;
      else if( Option==1 ) x = ( diff > 0 )? x1-1 : x1;
      else if( Option==2 ) x = x1;
     }
  }

return x;
}
//-------------------------------------------------------

int _StringToInt64(char* s, int scale, my_int64* px)
{
if( !px || !s ) return 0;
if( scale < 0 ) scale = -scale;

if( !_CheckInt64Len(s) ) return 0;

*px = 0;
char* pc = strchr(s, ',');
if(!pc) pc = strchr(s, '.');

if( !pc )
  {
   *px = my_atoi64(s);
   for(int i=0; i<scale; i++) *px *= 10;
   return 1;
  }
else
  {
   int orgscale = -1;

   int ndec = strlen(pc+1);
   if( ndec > scale ) // zmiana skali
     {
      orgscale = scale;
      scale = ndec;
     }

   if( !scale )
     {
      char c = *pc;
      *pc = 0;
      *px = my_atoi64(s);
      *pc = c;
      return 1;
     }
   else
     {
      char abuf[64];

      int n = strlen(s);
      int nn = 1+n+scale;
      char* buf = abuf;
      int bfree = 0;
      if( nn > 64 )
        {
         buf = (char*)malloc(nn*sizeof(char));
         if( !buf ) return 0;
         bfree = 1;
        }

      char c = *pc;
      *pc = 0;
      strcpy(buf, s);
      *pc = c;
      strcat(buf, pc+1);

      for(int i=0; i<scale-ndec; i++) strcat(buf, "0");
      if( !_CheckInt64Len(buf) ) { if(bfree) free(buf); return 0; }

      *px = my_atoi64(buf);
      if(bfree) free(buf);

      if( orgscale >= 0 ) *px = _Int64Rescale(*px, scale, orgscale, 0); // powrot do poprzedniej skali
      return 1;
     }
  }
}
//-------------------------------------------------------

double _Int64ToDouble(my_int64 x, int scale)
{
if( scale == 0 ) return (double)x;

double v = (double)x;

if( scale < 0 ) scale = -scale;
my_int64 tens = 1;
for(int i=0; i<scale; i++) tens *= 10;
if( tens > 1 ) v /= tens;

return v;
}
//-------------------------------------------------------

my_int64 _DoubleToInt64(double x, int scale)
{
if( scale < 0 ) scale = -scale;
double tens = 1;
for(int i=0; i<scale; i++) tens *= 10;
if( tens > 2 ) x *= tens; // od 2 dla bezpieczenstwa (nie poprawiaj !!!)

//zaokraglenie do calosci
double fl = floor( x );
if( x-fl >= 0.5 - 1e-6 ) fl++;

return (my_int64)fl;
}
//-------------------------------------------------------

