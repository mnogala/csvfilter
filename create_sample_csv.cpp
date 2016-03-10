
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include <stdio.h>
#include <stdlib.h>

static int _random(int num)
{
return (int)(rand()%(num));
}
//------------------------

int main()
{
srand(0);

int nRows = 90000000;
int nCols = 11;

FILE* fout = fopen("input.csv", "wt");
if( !fout ) return 0;

for(int i=0; i<nRows; i++)
  {
   for(int j=0; j<nCols; j++)
     {
      int r = (j==0)? (i+1): _random(10000);
      if( j < nCols-1 ) fprintf(fout, "%d, ", r);
      else
        {
         if( i < nRows-1 ) fprintf(fout, "%d\n", r);
         else fprintf(fout, "%d", r);
        }
     }
  }

fclose(fout);
return 0;
}
