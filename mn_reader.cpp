
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_reader.h"
#include <string.h>

READER::READER(const char* _fname, int _b_create_map, unsigned _buf_size)
{
buf_size = 0;
p_file = 0;
b_create_map = _b_create_map;
n_rows = 0;

if( !_fname ) return;
if( _fname[0] == 0 ) return;

hg_fname.SetSize(sizeof(char));
hg_fname.Append((void*)_fname, 1+strlen(_fname));

p_file = fopen(_fname, "rb");
if( !p_file ) return;

buf_size = _buf_size;
if( buf_size < 1024 ) buf_size = 1024;

if( !hg_buf.Alloc(buf_size, sizeof(char)) )
  {
   fclose(p_file);
   p_file = 0;
   buf_size = 0;
  }
}
//------------------------

READER::~READER()
{
if( p_file ) fclose(p_file);
}
//------------------------

int READER::ReadFirstLine(VECT* pMem)
{
if( !pMem ) return 0;
pMem->Free();
if( !p_file ) return 0;
if( ftell(p_file) > 0 ) return 0;

pMem->SetSize(sizeof(char));

char buf[1024];
int beof = 0;
int cnt = 0;
while(1)
  {
   int nr = fread(buf+cnt, sizeof(char), 1, p_file);
   if( nr == 1 ) cnt++;
   else beof = 1;

   int bNL = 0;
   if( cnt > 0 ) { if( buf[cnt-1] == '\n' ) bNL = 1; }

   if( bNL || beof || cnt == 1024 )
     {
      if( cnt > 0 )
        {
         if( !pMem->Append(buf, cnt) ) { pMem->Free(); fclose(p_file); p_file = 0; return 0; }
        }
      if( bNL || beof ) break;
      cnt = 0;
     }
  }

if( beof ) { fclose(p_file); p_file = 0; }
return pMem->Nelem();
}
//------------------------

void READER::Reset()
{
if( p_file ) fseek(p_file, 0, SEEK_SET);
}
//------------------------

int READER::ReadHelp(VECT* pMem, int* pWasNL)
{
if( !pMem || !pWasNL ) return 0;
pMem->Free();
*pWasNL = 0;
if( !p_file ) return 0;

pMem->SetSize(sizeof(char));

unsigned ntmp = hg_tmp.Nelem();
if( ntmp )
  {
   if( !pMem->Append(hg_tmp.Ptr(), ntmp) ) return 0;
  }
hg_tmp.Free();

int beof = 0;
while(1)
  {
   char* ptr = (char*)hg_buf.Ptr();
   int nr = fread(ptr, sizeof(char), buf_size, p_file);
   if( nr != buf_size ) beof = 1;

   if( beof )
     {
      if( nr > 0 )
        {
         if( !pMem->Append(ptr, nr) ) { pMem->Free(); fclose(p_file); p_file = 0; return 0; }
        }
      break;
     }

   int last_NL = 0;
   for(int i=nr-1; i>=0; i--)
     {
      if( ptr[i] == '\n' ) { last_NL = i+1; break; }
     }

   if( last_NL > 0 && last_NL < nr )
     {
      hg_tmp.SetSize(sizeof(char));
      hg_tmp.Append(ptr+last_NL, nr-last_NL);
      nr = last_NL;
     }

   if( last_NL ) *pWasNL = 1;

   if( !pMem->Append(ptr, nr) ) { pMem->Free(); return 0; }
   if( last_NL > 0 ) break;
  }

if( beof ) { fclose(p_file); p_file = 0; }
return pMem->Nelem();
}
//------------------------

int READER::Read(VECT* pMem)
{
if( !pMem ) return 0;
pMem->Free();
if( !p_file ) return 0;

pMem->SetSize(sizeof(char));
VECT hg;
while( 1 )
  {
   int bNL = 0;
   int nr = ReadHelp(&hg, &bNL);
   if( !nr ) break;

   char* ptr = (char*)hg.Ptr();
   if( !ptr ) break;
   if( !pMem->Append(ptr, nr) ) { pMem->Free(); return 0; }

   if( bNL || !p_file ) break;
  }

int nel = pMem->Nelem();
if( b_create_map && nel > 0 )
  {
   int cnt = 0;
   char* ptr = (char*)hg.Ptr();
   for(int i=0; i<nel; i++)
     {
      if( ptr[i] == '\n' || i == nel-1 ) cnt++;
     }

   if( cnt > 0 )
     {
      int n1 = hg_map.Nelem();
      if( n1 == 0 ) hg_map.SetSize(1);

      n_rows += cnt;
      my_int64 n2 = n_rows/8;
      if( n_rows - 8*n2 ) n2++;
      if( n2 > n1 )
        {
         if( !hg_map.ReAlloc(n2) ) { pMem->Free(); return 0; }
        }
     }
  }

return nel;
}
//------------------------

int READER::SetMap(my_int64 row)
{
if( row <= 0 || row > n_rows ) return 0;

int pos = row/8;
int b = row - 8*pos;
if( !b ) { pos--; b = 7; }
else b--;

char* ptr = (char*)hg_map.Ptr(pos);
if( !ptr ) return 0;

(*ptr) |= (1<<b);
return 1;
}
//------------------------

int READER::GetMap(my_int64 row)
{
if( row <= 0 || row > n_rows ) return 0;

int pos = row/8;
int b = row - 8*pos;
if( !b ) { pos--; b = 7; }
else b--;

char* ptr = (char*)hg_map.Ptr(pos);
if( !ptr ) return 0;

return ((*ptr) & (1<<b));
}
//------------------------

int READER::Reopen()
{
if( p_file ) return 0;

char* fname = (char*)hg_fname.Ptr();
if( !fname ) return 0;
if( fname[0] == 0 ) return 0;

p_file = fopen(fname, "rb");
if( !p_file ) return 0;

hg_tmp.Free();
b_create_map = 0;
return 1;
}

