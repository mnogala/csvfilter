
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_defs.h"
#include "mn_alg.h"
#include "mn_fastbuf.h"
#include "mn_reader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DEF_MAX_COLNAME 32 // maksymalna dlugosc nazwy kolumny w naglowku
#define DEF_MAXTHREADS 512 // maksymalna liczba watkow

/***********************************/

static inline int _IsWhiteChar(char c)
{
return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' );
}
//-----------------------------------

static char* _NextChar(char* buf)
{
if( !buf ) return 0;
while( _IsWhiteChar(*buf) ) buf++;
return buf;
}
//-----------------------------------

static inline int _IsSep(char c)
{
return ( c == ';' || c == '|' || c == ',' || c == ' ' || c == '\t' || c == '\r' || c == '\n' );
}
//-----------------------------------

static char* _FindSep(char* buf)
{
if( !buf ) return 0;
while( *buf )
  {
   if( _IsSep(*buf) ) return buf;
   buf++;
  }
return 0;
}
//--------------------------

static char* _FindSep2(char* buf)
{
if( !buf ) return 0;
int bcudz = 0;
while( *buf )
  {
   if( *buf == '\"' ) bcudz = !bcudz;
   if( !bcudz && _IsSep(*buf) ) return buf;
   buf++;
  }
return 0;
}
//-----------------------------------

static inline int _IsWhite2(char c)
{
return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\"' );
}
//-----------------------------------

// Wycina biale znaki oraz znaki " z poczatku i konca tekstu.
// Przerabia tekst w miejscu
static int _MyTrim(char *s)
{
if( !s ) return 0;
int len = strlen(s);
if(!len) return 0;

if( !_IsWhite2(s[0]) && !_IsWhite2(s[len-1]) ) return 0;

char* p = s + len - 1;
while( p >= s && _IsWhite2(*p) )
  {
   *p = 0;
   if( p > s ) p--;
  }

if( !_IsWhite2(s[0]) ) return 1;

p = s;
while( _IsWhite2(*s) ) s++;
while( *s ) *p++ = *s++;
*p = 0;

return 1;
}
//-----------------------------

static int csv_get_header(FASTBUF* p_Row, VECT* pColNames, VECT* pValList)
{
if( pValList ) pValList->Free();
if( !p_Row || !pColNames ) return 0;

pColNames->Free();
pColNames->SetSize(DEF_MAX_COLNAME);
char buf_name[DEF_MAX_COLNAME];

int nR = p_Row->Nelem();
if( nR <= 1 ) return 0;

char* RowBuf = (char*)p_Row->Ptr();
if( !RowBuf ) return 0;

char* p1 = _NextChar(RowBuf);
if( *p1 == 0 ) return 0;

int balpha = 0;
for(int i=0; i<nR-1; i++)
  {
   char c = RowBuf[i];
   if( c && !isdigit(c) && c != '.' && !_IsSep(c) ) { balpha = 1; break; }
  }

if( !balpha ) return 0;

while(1)
  {
   memset(buf_name, 0, DEF_MAX_COLNAME);
   char c;
   char* p2 = 0;

   if( *p1 )
     {
      p2 = _FindSep2(p1);
      if( p2 ) { c = *p2; *p2 = 0; }
      if( p2 != p1 ) strncpy(buf_name, p1, DEF_MAX_COLNAME-1);
     }

   pColNames->Append(buf_name);

   if( p2 ) *p2 = c;
   else break;

   if( *p2 == '\r' || *p2 == '\n' ) break;
   p1 = _NextChar(p2+1);
  }

int n = pColNames->Nelem();
if( n > 0 )
  {
   IDVAL_STRUCT vs;
   if( pValList ) pValList->SetSize(sizeof(IDVAL_STRUCT));

   int sz = pColNames->Size();
   for(int i=0; i<n; i++)
     {
      char* p = (char*)pColNames->Ptr(i);
      if( !p ) continue;

      int balpha = 0;
      for(int j=0; j<sz; j++)
        {
         char c = p[j];
         if( !c ) break;
         if( !isdigit(c) && c != '.' && !_IsSep(c) ) { balpha = 1; break; }
        }

      if( !balpha ) { pColNames->Free(); if(pValList) pValList->Free(); return -9; } // Brak znakow alfanumerycznych wewnatrz nazwy

      _MyTrim(p);
      if( strchr(p, '\"') ) { pColNames->Free(); if(pValList) pValList->Free(); return -10; } // niedozwolony znak (cudzyslow) wewnatrz nazwy
      if( strchr(p, '\'') ) { pColNames->Free(); if(pValList) pValList->Free(); return -11; } // niedozwolony znak (apostrof) wewnatrz nazwy

      if( pValList )
        {
         memset(vs.id, 0, sizeof(vs.id));
         strncpy(vs.id, p, sizeof(vs.id)-1);
         pValList->Append(&vs);
        }
     }
  }

return 1;
}
//--------------------------

static int csv_put_row(FASTBUF* p_Row, FILE* p_File)
{
if( !p_Row || !p_File ) return 0;
int n = p_Row->Nelem();
if( n <= 1 ) return 0;

int nw = fwrite(p_Row->Ptr(), sizeof(char), n-1, p_File);
return (nw==n-1);
}
//--------------------------

static int csv_put_row2(char* Buf, unsigned Cnt, FILE* p_File)
{
if( !Buf || !Cnt || !p_File ) return 0;

int nw = fwrite(Buf, sizeof(char), Cnt, p_File);
return (nw==Cnt);
}
//--------------------------

static int _GetValues(VECT* pValList, char* RowBuf, VECT* pColNames)
{
if( !pValList || !RowBuf ) return 0;

if( !pValList->Nelem() ) pValList->SetSize(sizeof(IDVAL_STRUCT));

char* p1 = _NextChar(RowBuf);
if( *p1 == 0 ) return 0;

IDVAL_STRUCT vs;
int cnt = 0;
while(1)
  {
   int bAdd = 0;
   IDVAL_STRUCT* pvs = (IDVAL_STRUCT*)pValList->Ptr(cnt);
   if( !pvs ) { pvs = &vs; bAdd = 1; }

   pvs->scale = 0;
   pvs->bnull = 1;
   pvs->val = 0;
   if( bAdd ) memset(pvs->id, 0, sizeof(pvs->id));

   char c;
   char* p2 = 0;

   if( *p1 )
     {
      p2 = _FindSep(p1);
      if( p2 ) { c = *p2; *p2 = 0; }

      if( p2 != p1 )
        {
         int scale = 0;
         char* p = strchr(p1, '.');
         if( !p ) p = strchr(p1, ',');
         if( p ) scale = strlen(p+1);

         int ret = _StringToInt64(p1, scale, &(pvs->val));
         if( ret ) { pvs->bnull = 0; pvs->scale = scale+1; }
         //pvs->val = atof(p1); pvs->bnull = 0;
        }
     }

   if( bAdd && pColNames )
     {
      char* pcolname = (char*)pColNames->Ptr(cnt);
      if( pcolname ) strncpy(pvs->id, pcolname, sizeof(pvs->id)-1);
     }

   cnt++;
   if( bAdd )
     {
      if( pvs->id[0] == 0 ) sprintf(pvs->id, "k%d", cnt);
      pValList->Append(pvs);
     }

   if( p2 ) *p2 = c;
   else break;

   if( *p2 == '\r' || *p2 == '\n' ) break;
   p1 = _NextChar(p2+1);
  }

int nv = pValList->Nelem();
if( cnt < nv )
  {
   IDVAL_STRUCT* pvs = (IDVAL_STRUCT*)pValList->Ptr(0);
   for(int i=cnt; i<nv; i++)
     {
      (pvs+i)->bnull = 1;
      (pvs+i)->scale = 0;
      (pvs+i)->val = 0;
     }
  }

return 1;
}
//------------------------------------

static int csv_filter(FASTBUF* p_Row, WZOR* p_Form, VECT* pColNames, VECT* pValList, VECT* pErrList, int bTest)
{
if( !p_Row || !p_Form || !pValList ) return 0;
if( !p_Row->Nelem() ) return 0;

if( !_GetValues(pValList, (char*)p_Row->Ptr(), pColNames) ) return 0;
if( !pValList->Nelem() ) return 0;

my_int64 Res;
if( bTest )
  {
   int ret = p_Form->Calculate(&Res, pValList, pErrList, 1);
   if( ret != 1 && ret != DEF_NULL_ERROR ) return 0;
  }

int ret = p_Form->Calculate(&Res, pValList, 0/*pErrList*/, 0);
if( ret != 1 ) return 0;

my_int64* px = &Res;
int formscale = p_Form->GetScale();
if( formscale != 1 ) // chcemy numeric o skali 0
  {
   if( formscale > 0 ) *px = _Int64Rescale(Res, formscale-1, 0, 0);
   else *px = _DoubleToInt64(*((double*)&Res), 0);
  }

return ((*px != 0)? 1: 0);
}
//--------------------------

struct THR_STRUCT
{
int Nr;
int bTest;
volatile int bEnd;
my_mutex* pMutex;
volatile int* pError;
volatile my_int64* pnChars;
volatile my_int64* pnInput;
volatile my_int64* pnResult;
WZOR* pForm;
VECT* pColNames;
VECT* pValList;
VECT* pErrList;
READER* pReader;
my_thread thr;
};
//--------------------------

static void _Lock(THR_STRUCT* pts)
{
if( pts && pts->pMutex ) my_lock(pts->pMutex);
}
//--------------------------

static void _Unlock(THR_STRUCT* pts)
{
if( pts && pts->pMutex ) my_unlock(pts->pMutex);
}
//--------------------------

static void* _ThrProc(void* ptr)
{
if( !ptr ) return 0;
THR_STRUCT* pts = (THR_STRUCT*)ptr;

if( *(pts->pError) ) { pts->bEnd = 1; return 0; }

WZOR frm;
VECT ColNames;
VECT ValList;

_Lock(pts);
if( pts->pForm ) frm.CopyFrom(*(pts->pForm));
if( pts->pColNames ) ColNames.CopyFrom(*(pts->pColNames));
if( pts->pValList ) ValList.CopyFrom(*(pts->pValList));
_Unlock(pts);

my_int64 n_chars = 0;
my_int64 n_input = 0;
my_int64 n_result = 0;
my_int64 RowNr;
FASTBUF row;
FASTBUF found(sizeof(my_int64), 8192);
VECT hgr;
VECT ErrList;
int b_test = 0;
while( 1 )
  {
   if( *(pts->pError) ) { frm.Clear(); pts->bEnd = 1; return 0; }

   _Lock(pts);
   RowNr = pts->pReader->Rows();
   int ret = pts->pReader->Read(&hgr);
   _Unlock(pts);
   if( !ret ) break;

   if( pts->bTest && RowNr == 0 ) b_test = 1;

   int nr = hgr.Nelem();
   char* ptr = (char*)hgr.Ptr();
   if( !ptr ) break;

   for(int i=0; i<nr; i++)
     {
      if( *(pts->pError) ) { frm.Clear(); pts->bEnd = 1; return 0; }

      char c = ptr[i];
      n_chars++;
      row.Add(&c);

      if( c == '\n' || i == nr-1 )
        {
         char zero = 0;
         row.Add(&zero);

         n_input++;
         RowNr++;
         if( csv_filter(&row, &frm, &ColNames, &ValList, &ErrList, b_test) )
           {
            found.Add(&RowNr);
            n_result++;
           }
         else if( b_test && ErrList.Nelem() )
           {
            _Lock(pts);
            *(pts->pError) = 1;
            if( pts->pErrList ) pts->pErrList->CopyFrom(ErrList);
            _Unlock(pts);

            frm.Clear();
            pts->bEnd = 1;
            return 0; // -12
           }

         b_test = 0;
         row.Clear();
        }
     }
  }

_Lock(pts);
unsigned nf = found.Nelem();
if( nf )
  {
   my_int64* ptr = (my_int64*)found.Ptr();
   for(int i=0; i<nf; i++) pts->pReader->SetMap(ptr[i]);
  }
if( n_chars ) *(pts->pnChars) += n_chars;
if( n_input ) *(pts->pnInput) += n_input;
if( n_result ) *(pts->pnResult) += n_result;
_Unlock(pts);

frm.Clear();
pts->bEnd = 1;

return 0;
}
//--------------------------

static void start_thread(THR_STRUCT* pts)
{
if( pts ) my_thread_start(&(pts->thr), _ThrProc, pts);
}
//--------------------------

static void join_threads(THR_STRUCT* pts, int nts)
{
if( !pts || !nts ) return;

#ifdef linux
for(int i=0; i<nts; i++) pthread_join(pts[i].thr, 0);
#else
while(1)
  {
   int bStop = 1;
   for(int i=0; i<nts; i++)
     {
      if( (pts+i)->bEnd == 0 ) { bStop = 0; break; }
     }
   if( bStop ) break;
   Sleep(50);
  }
#endif
}
//--------------------------

static int find_header(READER* pReader, FILE* pFout, VECT* pColNames, VECT* pValList, volatile my_int64* p_nChars)
{
if( p_nChars ) *p_nChars = 0;
if( !pReader || !pColNames || !pValList ) return 0;

VECT hgr;
int nr = pReader->ReadFirstLine(&hgr);
if( !nr ) return 0;

char* ptr = (char*)hgr.Ptr();
if( !ptr ) return 0;

FASTBUF row;
for(int i=0; i<nr; i++) row.Add(ptr+i);
char zero = 0;
row.Add(&zero);

int ret = csv_get_header(&row, pColNames, pValList);
if( ret == 1 && pFout ) csv_put_row(&row, pFout);

if( p_nChars ) *p_nChars += nr;
return ret;
}
//--------------------------

static int save_result(READER* pReader, FILE* pFout, int bHeader, my_int64 nResult)
{
if( !pReader || !pFout || !nResult ) return 0;

my_int64 nRows = pReader->Rows();
if( !nRows ) return 0;

if( !pReader->Reopen() ) return 0;

VECT hgr;
if( bHeader )
  {
   int n = pReader->ReadFirstLine(&hgr);
   if( !n ) return 0;
  }

my_int64 RowNr = 1;
int bSave = pReader->GetMap(RowNr);
my_int64 n_result = 0;

int nBuf = 2048;
char* Buf = (char*)malloc(nBuf*sizeof(char));
if( !Buf ) return 0;

while( pReader->Read(&hgr) )
  {
   int nr = hgr.Nelem();
   char* ptr = (char*)hgr.Ptr();
   if( !ptr ) break;

   unsigned cnt = 0;
   char* p;
   for(int i=0; i<nr; i++)
     {
      p = ptr+i;

      if( bSave )
        {
         if( cnt == nBuf )
           {
            nBuf += nBuf;
            Buf = (char*)realloc(Buf, nBuf*sizeof(char));
            if( !Buf ) return 0;
           }
         Buf[cnt++] = *p;
        }

      if( *p == '\n' || i == nr-1 )
        {
         if( bSave ) { csv_put_row2(Buf, cnt, pFout); n_result++; cnt = 0; }
         RowNr++;
         bSave = pReader->GetMap(RowNr);
        }
     }
  }

free(Buf);
if( n_result != nResult ) return -13;
return 1;
}
//--------------------------

static void csv_get_error(int ErrCode, VECT* pErrList, char* Buf, unsigned BufSize)
{
if( !Buf || !BufSize ) return;
if( ErrCode >= 0 ) { Buf[0] = 0; return; }
if( BufSize == 1 ) { Buf[0] = 0; return; }

memset(Buf, 0, BufSize);
int nChars = BufSize-1;

if( ErrCode == -1 ) strncpy(Buf, "Wadliwy rozmiar typu INT64", nChars);
else if( ErrCode == -2 ) strncpy(Buf, "Brak nazwy pliku zrodlowego", nChars);
else if( ErrCode == -3 ) strncpy(Buf, "Brak warunku filtrowania", nChars);
else if( ErrCode == -4 ) strncpy(Buf, "Blad otwarcia lub pusty plik zrodlowy", nChars);
else if( ErrCode == -5 ) strncpy(Buf, "Plik wynikowy juz istnieje", nChars);
else if( ErrCode == -6 ) strncpy(Buf, "Nie mozna otworzyc pliku wynikowego", nChars);
else if( ErrCode == -7 ) strncpy(Buf, "Bledny warunek filtrowania", nChars);
else if( ErrCode == -8 ) strncpy(Buf, "Pusty plik zrodlowy", nChars);
else if( ErrCode == -9 ) strncpy(Buf, "Brak znakow alfanumerycznych wewnatrz nazwy kolumny w naglowku", nChars);
else if( ErrCode == -10 ) strncpy(Buf, "Niedozwolony znak (cudzyslow) wewnatrz nazwy kolumny w naglowku", nChars);
else if( ErrCode == -11 ) strncpy(Buf, "Niedozwolony znak (apostrof) wewnatrz nazwy kolumny w naglowku", nChars);
else if( ErrCode == -12 ) strncpy(Buf, "Blad podczas filtrowania", nChars);
else if( ErrCode == -13 ) strncpy(Buf, "Blad zapisu rezultatu (niezgodna liczba wierszy)", nChars);
else if( ErrCode == -14 ) strncpy(Buf, "Blad zapisu rezultatu", nChars);
else strncpy(Buf, "Blad inny", nChars);

if( pErrList )
  {
   int nerr = pErrList->Nelem();
   if( nerr > 1 )
     {
      VECT hg;
      hg.SetSize(sizeof(char));
      int nbuf = strlen(Buf);
      if( nbuf > 0 ) hg.Append(Buf, nbuf);
      char NL = '\n';
      if( !hg.Append(&NL, 1) ) return;
      if( !hg.Append((char*)pErrList->Ptr(), 1+nerr) ) return;
      strncpy(Buf, (char*)hg.Ptr(), nChars);
     }
  }
}
//--------------------------

extern "C"
int csvfilter(const char* InputName, const char* ResultName, const char* Filter, unsigned nThreads, int bOverWriteResult, int bTest, my_int64* p_nInputRows, my_int64* p_nResultRows, char* ErrorBuf, unsigned ErrorBufSize, char* AstBuf, unsigned AstBufSize)
{
if( p_nInputRows ) *p_nInputRows = 0;
if( p_nResultRows ) *p_nResultRows = 0;
if( ErrorBuf && ErrorBufSize > 0 ) memset(ErrorBuf, 0, ErrorBufSize);
if( AstBuf && AstBufSize > 0 ) memset(AstBuf, 0, AstBufSize);

if( sizeof(my_int64) < 8 ) { csv_get_error(-1, 0, ErrorBuf, ErrorBufSize); return -1; }
if( !InputName || InputName[0] == 0 ) { csv_get_error(-2, 0, ErrorBuf, ErrorBufSize); return -2; }
if( !Filter || Filter[0] == 0 ) { csv_get_error(-3, 0, ErrorBuf, ErrorBufSize); return -3; }

if( nThreads == 0 ) nThreads = 1;
if( nThreads > DEF_MAXTHREADS ) nThreads = DEF_MAXTHREADS;

READER rd(InputName, nThreads>1);
if( !rd.IsOpened() ) { csv_get_error(-4, 0, ErrorBuf, ErrorBufSize); return -4; }

FILE* fout = 0;
if( !bOverWriteResult && ResultName && ResultName[0] )
  {
   fout = fopen(ResultName, "rb");
   if( fout ) { fclose(fout); csv_get_error(-5, 0, ErrorBuf, ErrorBufSize); return -5; }
  }

if( ResultName && ResultName[0] )
  {
   fout = fopen(ResultName, "wb");
   if( !fout ) { csv_get_error(-6, 0, ErrorBuf, ErrorBufSize); return -6; }
  }

VECT ErrList;
WZOR frm;
int ret = frm.Scan((char*)Filter, &ErrList);
if( ret != 1 )
  {
   frm.Clear();
   if(fout) fclose(fout);
   csv_get_error(-7, &ErrList, ErrorBuf, ErrorBufSize);
   return -7;
  }

ErrList.Free();

VECT hg;
frm.Format(&hg);
if( hg.Nelem() > 1 && AstBuf && AstBufSize > 0 ) strncpy(AstBuf, (char*)hg.Ptr(), AstBufSize-1);

VECT ColNames;
VECT ValList;
volatile my_int64 n_chars = 0;
volatile my_int64 n_input = 0;
volatile my_int64 n_result = 0;

if( nThreads > 1 )
  {
   THR_STRUCT* pts = (THR_STRUCT*)malloc(nThreads*sizeof(THR_STRUCT));
   if( !pts ) { if(fout) fclose(fout); frm.Clear(); csv_get_error(0, 0, ErrorBuf, ErrorBufSize); return 0; }

   int bHeader = 0;
   ret = find_header(&rd, fout, &ColNames, &ValList, &n_chars);
   if( ret < 0 ) { free(pts); if(fout) fclose(fout); frm.Clear(); csv_get_error(ret, 0, ErrorBuf, ErrorBufSize); return ret; }
   if( !ret ) rd.Reset();
   else bHeader = 1;

   my_mutex Mtx;
   my_mutex_init(&Mtx);
   volatile int bError = 0;

   for(int i=0; i<nThreads; i++)
     {
      (pts+i)->Nr = i+1;
      (pts+i)->bTest = bTest;
      (pts+i)->bEnd = 0;
      (pts+i)->pMutex = &Mtx;
      (pts+i)->pError = &bError;
      (pts+i)->pnChars = &n_chars;
      (pts+i)->pnInput = &n_input;
      (pts+i)->pnResult = &n_result;
      (pts+i)->pForm = &frm;
      (pts+i)->pColNames = &ColNames;
      (pts+i)->pValList = &ValList;
      (pts+i)->pErrList = &ErrList;
      (pts+i)->pReader = &rd;
      start_thread(pts+i);
     }

   join_threads(pts, nThreads);
   free(pts);
   my_mutex_exit(&Mtx);

   if( n_result > 0 && fout && !bError )
     {
      ret = save_result(&rd, fout, bHeader, n_result);
      if( ret == 0 ) ret = -14;
      if( ret < 0 ) { csv_get_error(ret, 0, ErrorBuf, ErrorBufSize); return ret; }
     }

   frm.Clear();
   if(fout) fclose(fout);
   if( bError )
     {
      csv_get_error(-12, &ErrList, ErrorBuf, ErrorBufSize);
      return -12;
     }
   if( !n_chars ) { csv_get_error(-8, 0, ErrorBuf, ErrorBufSize); return -8; }
   if( p_nInputRows ) *p_nInputRows = n_input;
   if( p_nResultRows ) *p_nResultRows = n_result;
   return 1;
  }

FASTBUF row;
VECT hgr;
int b_chkheader = 0;
int b_test = bTest;
while( rd.Read(&hgr) )
  {
   int nr = hgr.Nelem();
   char* ptr = (char*)hgr.Ptr();
   if( !ptr ) break;

   for(int i=0; i<nr; i++)
     {
      char c = ptr[i];
      n_chars++;
      row.Add(&c);

      if( c == '\n' || i == nr-1 )
        {
         char zero = 0;
         row.Add(&zero); // null char

         if( !b_chkheader )
           {
            b_chkheader = 1;
            ret = csv_get_header(&row, &ColNames, &ValList);
            if( ret < 0 ) { if(fout) fclose(fout); frm.Clear(); csv_get_error(ret, 0, ErrorBuf, ErrorBufSize); return ret; }
            if( ret == 1 )
              {
               if(fout) csv_put_row(&row, fout);
               row.Clear();
               continue;
              }
           }

         n_input++;
         if( csv_filter(&row, &frm, &ColNames, &ValList, &ErrList, b_test) ) { if(fout) csv_put_row(&row, fout); n_result++; }
         else if( b_test && ErrList.Nelem() )
           {
            frm.Clear();
            if(fout) fclose(fout);
            csv_get_error(-12, &ErrList, ErrorBuf, ErrorBufSize);
            return -12;
           }

         b_test = 0;
         row.Clear();
        }
     }
  }

frm.Clear();
if(fout) fclose(fout);
if( !n_chars ) { csv_get_error(-8, 0, ErrorBuf, ErrorBufSize); return -8; }
if( p_nInputRows ) *p_nInputRows = n_input;
if( p_nResultRows ) *p_nResultRows = n_result;
return 1;
}

