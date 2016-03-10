
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_alg.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//***********************************

static int _mycmp(char* s1, char* s2)
{
while(1)
  {
   if( *s1 != *s2 ) return 1;
   if( *s1 == 0 && *s2 == 0 ) return 0;
   s1++;
   s2++;
  }
}
//---------------------------------

static int _CompareValues(double* p1, int scale1, double* p2, int scale2)
{
if( scale1 <= 0 && scale2 <= 0 ) // oba double
  {
   if( *p1 > *p2 ) return 1;
   else if( *p1 < *p2 ) return -1;
   else return 0;
  }

if( scale1 > 0 && scale2 > 0 )
  {
   my_int64 x1 = *((my_int64*)p1);
   my_int64 x2 = *((my_int64*)p2);

   if( scale1 == scale2 )
     {
      if( x1 > x2 ) return 1;
      else if( x1 < x2 ) return -1;
      else return 0;
     }
   else
     {
      my_int64 y1 = _Int64Rescale(x1, scale1-1, 0, 0); // do skali 0
      my_int64 y2 = _Int64Rescale(x2, scale2-1, 0, 0); // do skali 0

      if( y1 > y2 ) return 1;
      else if( y1 < y2 ) return -1;

      if( scale1 > scale2 )
        {
         y1 = x1;
         y2 = _Int64Rescale(x2, scale2-1, scale1-1, 0);
        }
      else
        {
         y1 = _Int64Rescale(x1, scale1-1, scale2-1, 0);
         y2 = x2;
        }

      if( y1 > y2 ) return 1;
      else if( y1 < y2 ) return -1;
      else return 0;
     }
  }

if( scale1 > 0 )
  {
   my_int64 x1 = *((my_int64*)p1);

   my_int64 y1 = _Int64Rescale(x1, scale1-1, 0, 0); // do skali 0
   my_int64 y2 = (my_int64)*p2;

   if( y1 > y2 ) return 1;
   else if( y1 < y2 ) return -1;

   double v1 = _Int64ToDouble(*((my_int64*)p1), scale1-1); // bo trzeba od skali odjac 1

   if( v1 > *p2 ) return 1;
   else if( v1 < *p2 ) return -1;
   else return 0;
  }
else if( scale2 > 0 )
  {
   my_int64 x2 = *((my_int64*)p2);

   my_int64 y1 = (my_int64)*p1;
   my_int64 y2 = _Int64Rescale(x2, scale2-1, 0, 0); // do skali 0

   if( y1 > y2 ) return 1;
   else if( y1 < y2 ) return -1;

   double v2 = _Int64ToDouble(*((my_int64*)p2), scale2-1);

   if( *p1 > v2 ) return 1;
   else if( *p1 < v2 ) return -1;
   else return 0;
  }
else return 0; // niemozliwe
}
//--------------------------

static void _AddValues(double* p1, int scale1, double* p2, int scale2, double* pRes, short* pResScale)
{
if( scale1 <= 0 && scale2 <= 0 )
  {
   *pRes = (*p1) + (*p2);
   *pResScale = 0;
   return;
  }

if( scale1 > 0 && scale2 > 0 )
  {
   my_int64 x1 = *((my_int64*)p1);
   my_int64 x2 = *((my_int64*)p2);

   int sc = (scale1 >= scale2)? scale1 : scale2;

   if( scale1 != sc ) x1 = _Int64Rescale(x1, scale1-1, sc-1, 0);
   if( scale2 != sc ) x2 = _Int64Rescale(x2, scale2-1, sc-1, 0);

   *((my_int64*)pRes) = x1 + x2;
   *pResScale = sc;
   return;
  }

if( scale1 > 0 )
  {
   double v1 = _Int64ToDouble(*((my_int64*)p1), scale1-1);
   *pRes = v1 + (*p2);
   *pResScale = 0;
  }
else if( scale2 > 0 )
  {
   double v2 = _Int64ToDouble(*((my_int64*)p2), scale2-1);
   *pRes = *p1 + v2;
   *pResScale = 0;
  }
}
//--------------------------

static void _SubtractValues(double* p1, int scale1, double* p2, int scale2, double* pRes, short* pResScale)
{
if( scale1 <= 0 && scale2 <= 0 )
  {
   *pRes = (*p1) - (*p2);
   *pResScale = 0;
   return;
  }

if( scale1 > 0 && scale2 > 0 )
  {
   my_int64 x1 = *((my_int64*)p1);
   my_int64 x2 = *((my_int64*)p2);

   int sc = (scale1 >= scale2)? scale1 : scale2;

   if( scale1 != sc ) x1 = _Int64Rescale(x1, scale1-1, sc-1, 0);
   if( scale2 != sc ) x2 = _Int64Rescale(x2, scale2-1, sc-1, 0);

   *((my_int64*)pRes) = x1 - x2;
   *pResScale = sc;
   return;
  }

if( scale1 > 0 )
  {
   double v1 = _Int64ToDouble(*((my_int64*)p1), scale1-1);
   *pRes = v1 - (*p2);
   *pResScale = 0;
  }
else if( scale2 > 0 )
  {
   double v2 = _Int64ToDouble(*((my_int64*)p2), scale2-1);
   *pRes = *p1 - v2;
   *pResScale = 0;
  }
}
//--------------------------

static void _MultiplyValues(double* p1, int scale1, double* p2, int scale2, double* pRes, int* pResScale)
{
if( scale1 <= 0 && scale2 <= 0 )
  {
   *pRes = (*p1) * (*p2);
   *pResScale = 0;
   return;
  }

if( scale1 > 0 && scale2 > 0 )
  {
   my_int64 x1 = *((my_int64*)p1);
   my_int64 x2 = *((my_int64*)p2);

   *((my_int64*)pRes) = x1 * x2;
   *pResScale = (scale1-1) + (scale2-1) + 1; //trzeba dodac 1
   return;
  }

if( scale1 > 0 )
  {
   double v1 = _Int64ToDouble(*((my_int64*)p1), scale1-1);
   *pRes = v1 * (*p2);
   *pResScale = 0;
  }
else if( scale2 > 0 )
  {
   double v2 = _Int64ToDouble(*((my_int64*)p2), scale2-1);
   *pRes = *p1 * v2;
   *pResScale = 0;
  }
}
//--------------------------

static void _DivideValues(double* p1, int scale1, double* p2, int scale2, double* pRes, int* pResScale)
{
if( scale1 <= 0 && scale2 <= 0 )
  {
   *pRes = (*p1) / (*p2);
   *pResScale = 0;
   return;
  }

if( scale1 > 0 && scale2 > 0 )
  {
   my_int64 x1 = *((my_int64*)p1);
   my_int64 x2 = *((my_int64*)p2);

   int s = (scale1-1) + (scale2-1) + (scale2-1);
   my_int64 y1 = _Int64Rescale(x1, scale1-1, s, 0);

   *((my_int64*)pRes) = y1 / x2;
   *pResScale = (scale1-1) + (scale2-1) + 1; //trzeba dodac 1
   return;
  }

if( scale1 > 0 )
  {
   double v1 = _Int64ToDouble(*((my_int64*)p1), scale1-1);
   *pRes = v1 / (*p2);
   *pResScale = 0;
  }
else if( scale2 > 0 )
  {
   double v2 = _Int64ToDouble(*((my_int64*)p2), scale2-1);
   *pRes = *p1 / v2;
   *pResScale = 0;
  }
}

//******************** Funkcje matematyczne *****************

static void _SetError(VECT* pErrList, char* ErrText)
{
if( !pErrList ) return;
pErrList->Free();
pErrList->SetSize(sizeof(char));
pErrList->Append(ErrText, 1+strlen(ErrText));
}
//----------------------

static int _math_sqrt(double x, double* pRes)
{
if( !pRes ) return 0;

if( x < 0 ) { memset(pRes, 0, sizeof(double)); return DEF_MATHLIB_ERROR; }
else
  {
   if( x == 0 ) *pRes = 0.0;
   else *pRes = sqrt(x);
   return 1;
  }
}
//----------------------

static int _math_pow(double x, double power, double* pRes)
{
if( !pRes ) return 0;

if( x < 0 ) { memset(pRes, 0, sizeof(double)); return DEF_MATHLIB_ERROR; }
else if( x == 0 )
  {
   if( power > 0 ) { *pRes = 0.0; return 1; }
   else { memset(pRes, 0, sizeof(double)); return DEF_MATHLIB_ERROR; }
  }
else
  {
   *pRes = pow(x, power);
   return 1;
  }
}
//----------------------

static int _Power(double val, double power, double* pResult, VECT* pErrList)
{
if( !pResult ) return 0;
if( fmod(power, 1.0) != 0 )
  {
   int ret;
   if( power == 0.5 ) ret = _math_sqrt(val, pResult);
   else ret = _math_pow(val, power, pResult);
   return ret;
  }

int pw = (int)power;

if( pw == 1 ) { *pResult = val; return 1; }
if( pw == 0 ) { *pResult = 1; return 1; }
if( pw < 0 && val == 0 )
  {
   if( pErrList )
     {
      char buf[64];
      strcpy(buf, "Blad dzielenia przez 0 !!!");
      _SetError(pErrList, buf);
     }
   *pResult = 0;
   return -1;
  }// dzielenie przez 0 !!!

int abspow = ( pw > 0 )? pw: -pw;

double res = 1;
for(int i=0; i<abspow; i++) res *= val;

if( pw < 0 ) res = 1.0/res;

*pResult = res;
return 1;
}
//-----------------------------------

static inline int _IsWhiteChar(char c)
{
return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' );
}
//-----------------------------------

static inline int _IsOper(char* p)
{
return ( *p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '^' ||
*p == '>' || *p == '<' || *p == '=' ||
//(*p == '=' && *(p+1) == '=') ||
(*p == '!' && *(p+1) == '=') ||
(*p == '|' && *(p+1) == '|') ||
(*p == '&' && *(p+1) == '&') ||
((*p == 'O' || *p == 'o') && (*(p+1) == 'R' || *(p+1) == 'r')) ||
((*p == 'A' || *p == 'a') && (*(p+1) == 'N' || *(p+1) == 'n') && (*(p+2) == 'D' || *(p+2) == 'd')) );
}
//-----------------------------------

static inline int _OperCode(char* p)
{
if( *p == '+' ) return 1;
else if( *p == '-' ) return 2;
else if( *p == '*' ) return 3;
else if( *p == '/' ) return 4;
else if( *p == '^' ) return 5;

else if( *p == '<' && *(p+1) == '>' ) return 106; // to samo co !=
else if( *p == '>' && *(p+1) == '=' ) return 101;
else if( *p == '<' && *(p+1) == '=' ) return 102;
else if( *p == '>' ) return 103;
else if( *p == '<' ) return 104;
else if( *p == '=' && *(p+1) == '=' ) return 105;
else if( *p == '!' && *(p+1) == '=' ) return 106;
else if( *p == '=' ) return 1051;
else if( *p == '|' && *(p+1) == '|' ) return 107;
else if( *p == '&' && *(p+1) == '&' ) return 108;

else if( (*p == 'O' || *p == 'o') && (*(p+1) == 'R' || *(p+1) == 'r') ) return 107;
else if( (*p == 'A' || *p == 'a') && (*(p+1) == 'N' || *(p+1) == 'n') && (*(p+2) == 'D' || *(p+2) == 'd') ) return 1081;

else return 0;
}
//-----------------------------------

static inline int _IsLogic(int OpCode)
{
return ( OpCode == 107 || OpCode == 108 || OpCode == 1081 );
}
//-----------------------------------

static inline int _ToLogic(double* pval, int scale)
{
if( scale > 0 )
  {
   my_int64 x = *((my_int64*)pval);
   return (x)? 1: 0;
  }
else return (*pval)? 1: 0;
}
//-----------------------------------

static inline int _IsCompare(int OpCode)
{
return ( (OpCode >= 101 && OpCode <= 106) || OpCode == 1051 );
}
//-----------------------------------

static inline int _OperLen(int OpCode)
{
if( OpCode == 101 || OpCode == 102 || OpCode == 105 || OpCode == 106 || OpCode == 107 || OpCode == 108 ) return 2;
else if( OpCode == 1081 ) return 3;
else return 1;
}
//-----------------------------------

static char* _NextChar(char* buf)
{
while( *buf && _IsWhiteChar(*buf) ) buf++;
return buf;
}
//-----------------------------------

static char* _FindNextOper(char* p)
{
int cnt = 0;
int bcudz = 0;
int bap = 0;

while(*p)
  {
   if( *p == '\"' )
     {
      if(!bap) bcudz = !bcudz;
     }
   else if( *p == '\'' )
     {
      if( bap )
        {
         if( *(p-1) != '\\' ) bap = 0;
        }
      else bap = 1;
     }

   if( !bcudz && !bap )
     {
      if( *p == '(' ) cnt++;
      else if( *p == ')' ) cnt--;

      if( cnt == 0 && _IsOper(p) ) return p;
     }
   p++;
  }
return p;
}
//-----------------------------------

static char* _FindKet(char* p, int nb = -1) // szuka ')'
{
if( nb == 0 ) return 0;

int cnt = 1;
int bcudz = 0;
int bap = 0;

char* p0 = p;

while(*p)
  {
   if( *p == '\"' )
     {
      if(!bap) bcudz = !bcudz;
     }
   else if( *p == '\'' )
     {
      if( bap )
        {
         if( *(p-1) != '\\' ) bap = 0;
        }
      else bap = 1;
     }

   if( !bcudz && !bap )
     {
      if( *p == '(' ) cnt++;
      else if( *p == ')' )
        {
         cnt--;
         if( cnt == 0 ) return p;
        }
     }
   p++;

   if( nb >= 0 && p-p0 >= nb ) break;
  }
return 0;
}
//-----------------------------------

static char* _FindComma(char* p) // szuka ','
{
int cnt = 0;
int bcudz = 0;
int bap = 0;
while(*p)
  {
   if( *p == '\"' )
     {
      if(!bap) bcudz = !bcudz;
     }
   else if( *p == '\'' )
     {
      if( bap )
        {
         if( *(p-1) != '\\' ) bap = 0;
        }
      else bap = 1;
     }

   if( !bcudz && !bap )
     {
      if( *p == '(' ) cnt++;
      else if( *p == ')' ) cnt--;

      if( cnt == 0 && *p == ',' ) return p;
     }
   p++;
  }
return 0;
}
//-----------------------------------

static int _GetId(char* buf, VECT* pid_hg)
{
pid_hg->Free();
pid_hg->SetSize(sizeof(char));

if( *buf == '\'' ) return 3; // niedozwolony znak

if( *buf == '\"' ) // jesli identyfikator w ""
  {
   char* p = strchr(buf+1, *buf);
   if( p )
     {
      int len = (int)((p-buf)-1);
      if( len >= DEF_ID_MAXLEN ) return 0;
      if(len>0)
        {
         pid_hg->Alloc(1+len, sizeof(char));
         memcpy(pid_hg->Ptr(), buf+1, len*sizeof(char));
        }
      return 2; // potem trzeba strlen zwiekszyc o 2
     }
   else return 4; // brak konczacego znaku "
  }

char* p = buf;
int cnt = 0;
while( *p && !_IsOper(p) && *p != '(' && *p != ')' && !_IsWhiteChar(*p) )
  {
   if( cnt >= DEF_ID_MAXLEN-1 ) return 0;
   cnt++;
   p++;
  }

if( cnt > 0 )
  {
   pid_hg->Alloc(1+cnt, sizeof(char));
   memcpy(pid_hg->Ptr(), buf, cnt*sizeof(char));
  }

return 1;
}

//****************** WZOR ************************

WZOR::WZOR()
{
id_hg.SetSize(sizeof(char));

val = 0;
oper = 1;
bnull = 0;
numericscale = 0; // domyslnie wynik val jest double
list.SetSize(sizeof(WZOR));
}
//--------------------------

void WZOR::Clear()
{
int n = list.Nelem();

WZOR* ptr = (WZOR*)list.Ptr();
for(int i=0; i<n; i++) ptr[i].Clear();

id_hg.Free();
id_hg.SetSize(sizeof(char));

val = 0;
oper = 1;
bnull = 0;
numericscale = 0;

list.Free();
list.SetSize(sizeof(WZOR));
}
//--------------------------

void WZOR::CopyFrom(WZOR& wz)
{
Clear();

id_hg.Init();
id_hg.CopyFrom(wz.id_hg);

oper = wz.oper;
numericscale = wz.numericscale;

list.Init();
list.CopyFrom(wz.list);

int n = list.Nelem();
WZOR* p1 = (WZOR*)list.Ptr();
WZOR* p2 = (WZOR*)wz.list.Ptr();
for(int i=0; i<n; i++)
  {
   p1[i].id_hg.Init();
   p1[i].list.Init();
   p1[i].CopyFrom( p2[i] );
  }
}
//--------------------------

static int _StrToDouble(char* s, my_int64* pRes, short* pScale)
{
if( pScale ) *pScale = 0; // zadajemy double
if( !s || !pRes ) return 0;
if( *s == 0 ) { *pRes = 0; return 0; }

if( _CheckInt64Len(s) ) // identyfikatory liczbowe bedziemy trzymac jako int64
  {
   int scale = 0;
   char* p = strchr(s, '.');
   if(!p) p = strchr(s, ',');
   if( p ) scale = strlen(p+1);

   int ret = _StringToInt64(s, scale, pRes);
   if( ret )
     {
      if( pScale ) *pScale = scale+1; // Wazne: dodajemy 1 (bo numericscale jest o 1 wieksze)
      return 1;
     }
  }

if( !_mycmp(s, (char*)"NULL") )
  {
   *pRes = 0;
   return -2; // numeric null
  }

*pRes = 0;
return -1;
}
//--------------------------

int WZOR::SetVal(VECT* pValList) // 1 - znalazl i zadal, 0 - nie znalazl i zadal (poprzez StrToDouble), -1 - nawet nie znalazl
{
if( !pValList ) return -1; // niemozliwe

bnull = 0;
numericscale = 0;
val = 0;

char* id = (char*)id_hg.Ptr();
if( !id ) return 0;
if( *id == 0 ) return 0;

int found = 0;
int n = pValList->Nelem();
IDVAL_STRUCT* ptr = (IDVAL_STRUCT*)pValList->Ptr();
for(int i=0; i<n; i++)
  {
   if( !_mycmp(id, ptr[i].id) )
     {
      bnull = ptr[i].bnull; // mozna zadac null !!!
      val = (ptr+i)->val;
      numericscale = (ptr+i)->scale;
      found = 1;
      break;
     }
  }

if( !found )
  {
   int ok = _StrToDouble(id, &val, &numericscale); // tu zostanie zadane numericscale
   if( ok == -1 ) return -1;
   else if( ok == -2 ) return -2; // numeric null
   else return 0;
  }
else return 1;
}
//--------------------------

int WZOR::Calculate(my_int64* pResult, VECT* pValList, VECT* pErrList, int bTest)
{
char* id = (char*)id_hg.Ptr();
if( !id ) id = (char*)"";

VECT errlist;
if( pErrList ) errlist.CopyFrom(*pErrList);

bnull = 0;

if( !pResult ) { bnull = 1; return 0; }
int retval = SetVal(pValList);
if( retval == -2 )
  {
   *pResult = 0;
   if( bTest ) return 1;
   bnull = 2; // zadane jako NULL
   return DEF_NULL_ERROR;
  }

if( retval == -1 )
  {
   *pResult = 0;
   if( pErrList )
     {
      char buf[1024];
      sprintf(buf, "Niezdefiniowany symbol \"%s\" ...", id);
      _SetError(pErrList, buf);
     }
   bnull = 1;
   return -3;// niezdefiniowany id
  }

int n = list.Nelem();
WZOR* ptr = (WZOR*)list.Ptr();

if( !n )
  {
   *pResult = val;
   if( bnull ) return DEF_NULL_ERROR;
   return 1;
  }

//------------
int bLogic = 0;
for(int i=0; i<n; i++)
  {
   if( _IsLogic((ptr+i)->oper) ) bLogic = 1;
   if( bLogic ) break;
  }

//--------- liczymy wartosci

/*VECT CalcList;
CalcList.Alloc(n, sizeof(double));
double* pCalc = (double*)CalcList.Ptr();*/

double CalcTab[256];
VECT CalcList;
double* pCalc = CalcTab;
if( n > 256 )
  {
   CalcList.Alloc(n, sizeof(double));
   pCalc = (double*)CalcList.Ptr();
  }

// jesli alternatywa to szybko mozna sprawdzic, czy jest true
if( bLogic && !bTest )
  {
   int true1 = 0;
   int true2 = 0;
   for(int i=0; i<n; i++)
     {
      int oper = (ptr+i)->oper;
      if( i == 0 && oper == 1 ) oper = 107;

      if( true1 && oper == 107 )
        {
         *pResult = 1;
         numericscale = 1;
         return 1;
        }

      if( pErrList ) pErrList->CopyFrom(errlist);
      int ret = (ptr+i)->Calculate((my_int64*)(pCalc+i), pValList, pErrList, bTest);
      if( ret != 1 && ret != DEF_NULL_ERROR ) { *pResult = 0; bnull = 1; return ret; }

      int logv = _ToLogic(pCalc+i, (ptr+i)->numericscale);
      int lognull = (ptr+i)->bnull;
      true2 = (logv && !lognull)? 1: 0;

      if( true2 && i==n-1 && oper == 107 )
        {
         *pResult = 1;
         numericscale = 1;
         return 1;
        }

      true1 = (true2 && oper == 107);
     }
  }
else
  {
   for(int i=0; i<n; i++)
     {
      if( pErrList ) pErrList->CopyFrom(errlist);
      int ret = (ptr+i)->Calculate((my_int64*)(pCalc+i), pValList, pErrList, bTest);
      if( ret != 1 && ret != DEF_NULL_ERROR ) { *pResult = 0; bnull = 1; return ret; }
     }
  }

//------------ jesli logiczne
if( bLogic )
  {
   int logres = 0;
   int isNull1 = 0;
   for(int i=0; i<n; i++)
     {
      int logv = _ToLogic(pCalc+i, (ptr+i)->numericscale);
      int lognull = (ptr+i)->bnull;

      int operation = (ptr+i)->oper;
      if( i == 0 && operation == 1 ) operation = 107; // ||

      if( i<n-1 )
        {
         int isNull2 = 0;
         int cnt = 0;
         for(int j=i+1; j<n; j++)
           {
            int op = (ptr+j)->oper;
            if( op == 108 || op == 1081 ) // &&
              {
               //double wyn = pCalc[j];
               if( (ptr+j)->bnull ) isNull2 = 1;
               else logv *= _ToLogic(/*&wyn*/pCalc+j, (ptr+j)->numericscale);
              }
            else break;
            cnt++;
           }
         if( isNull2 && logv ) lognull = 1;

         if( cnt > 0 ) i += cnt;
        }

      if( operation == 107 )
        {
         if( lognull ) isNull1 = 1;
         else logres += logv;
        }
     }

   if( isNull1 && !logres ) { *pResult = 0; bnull = 1; return DEF_NULL_ERROR; }

   *pResult = (logres)? 1: 0;
   numericscale = 1; // operacje logiczne daja wynik numeric o skali 0
   return 1;
  }

//------------ jesli porownawcze

if( n == 2 && _IsCompare((ptr+1)->oper) )
  {
   int op = (ptr+1)->oper;

   // przyrownanie do numerycznego NULL (bnull=2)
   if( (op == 105 || op == 1051) && ptr->bnull && (ptr+1)->bnull && (ptr->bnull==2 || (ptr+1)->bnull==2) )
     { *pResult = 1; numericscale = 1; return 1; }
   else if( op == 106 && ((ptr->bnull==0 && (ptr+1)->bnull==2) || (ptr->bnull==2 && (ptr+1)->bnull==0)) )
     { *pResult = 1; numericscale = 1; return 1; }

   // porownanie z null daje zawsze 0, a nie null
   if( ptr->bnull || (ptr+1)->bnull ) { *pResult = 0; numericscale = 1; return 1; }

   int cmp = _CompareValues(pCalc, ptr->numericscale, pCalc+1, (ptr+1)->numericscale);

   int logres = 0;
   if( op == 101 ) logres = (cmp >= 0); //(v1 >= v2);
   else if( op == 102 ) logres = (cmp <= 0); //(v1 <= v2);
   else if( op == 103 ) logres = (cmp > 0); //(v1 > v2);
   else if( op == 104 ) logres = (cmp < 0); //(v1 < v2);
   else if( op == 105 || op == 1051 ) logres = (cmp == 0); //(v1 == v2);
   else if( op == 106 ) logres = (cmp != 0); //(v1 != v2);

   *pResult = (logres)? 1: 0;
   numericscale = 1; // operacje porownywania daja wynik numeric o skali 0
   return 1;
  }
//------------

// operacje arytmetyczne na liczbach
int isNull = 0;
double res;
memset(&res, 0, sizeof(double));
int bFirst = 1;
for(int i=0; i<n; i++)
  {
   double v;
   memcpy(&v, pCalc+i, sizeof(double));
   int scale = (ptr+i)->numericscale;

   int operation = (ptr+i)->oper;
   int iNull = (ptr+i)->bnull;

   if( i<n-1 )
     {
      if( (ptr+i+1)->oper == 5 )
        {
         int cnt = 0;
         for(int j=i+1; j<n; j++)
           {
            if( (ptr+j)->oper == 5 ) cnt++;
            else break;
           }

         double power = 1;
         double wyn;
         for(int j=i+cnt; j>i; j--)
           {
            memcpy(&wyn, pCalc+j, sizeof(double));
            if( (ptr+j)->numericscale > 0 ) wyn = _Int64ToDouble(*((my_int64*)&wyn), (ptr+j)->numericscale - 1);

            if( (ptr+j)->bnull ) { isNull = 1; continue; } //omijamy null
            if( pErrList ) pErrList->CopyFrom(errlist);
            int ret = _Power(wyn, power, &wyn, pErrList);
            if( bTest ) pErrList->Free();
            if( ret != 1 && !bTest ) { *pResult = 0; return ret; }
            power = wyn;
           }

         double vd;
         memcpy(&vd, &v, sizeof(double));
         if( scale > 0 ) vd = _Int64ToDouble(*((my_int64*)&v), scale-1);

         if( pErrList ) pErrList->CopyFrom(errlist);
         int ret = _Power(vd, power, &wyn, pErrList);
         if( bTest ) pErrList->Free();
         if( ret != 1 && !bTest ) { *pResult = 0; return ret; }

         if( scale > 0 ) *((my_int64*)&v) = _DoubleToInt64(wyn, scale-1); // z powrotem na int64
         else v = wyn;

         i += cnt;
        }

      int cnt = 0;
      for(int j=i+1; j<n; j++)
        {
         if( (ptr+j)->bnull ) { isNull = 1; continue; } //omijamy null
         int op = (ptr+j)->oper;
         if( op == 3 )
           {
            _MultiplyValues(&v, scale, pCalc+j, (ptr+j)->numericscale, &v, &scale); // na wyjsciu zadaje skale
           }
         else if( op == 4 )
           {
            if( !bTest ) // jesli bTest to dla bezpieczenstwa dzielimy przez 1 (czyli nie robimy nic)
              {
               double wyn;
               memcpy(&wyn, pCalc+j, sizeof(double));

               int bZero = 0;
               if( (ptr+j)->numericscale > 0 )
                 {
                  if( *((my_int64*)&wyn) == 0 ) bZero = 1;
                 }
               else
                 {
                  if( wyn == 0 ) bZero = 1;
                 }

               if( bZero )
                 {
                  if( pErrList )
                    {
                     char buf[64];
                     strcpy(buf, "Blad dzielenia przez 0 !!!");
                     _SetError(pErrList, buf);
                    }

                  *pResult = 0;
                  return -1;
                 } // dzielenie przez 0

               _DivideValues(&v, scale, &wyn, (ptr+j)->numericscale, &v, &scale); // na wyjsciu zadaje skale
              }
           }
         else break;
         cnt++;
        }
      if( cnt > 0 ) i += cnt;
     }

   if( iNull )
     {
      isNull = 1;
      if( bTest ) continue;
      else break;
     } //omijamy null

   if( operation == 1 )
     {
      if( bFirst ) { memcpy(&res, &v, sizeof(double)); numericscale = scale; }
      else _AddValues(&res, numericscale, &v, scale, &res, &numericscale); // na wyjsciu zadaje skale
     }
   else if( operation == 2 )
     {
      if( bFirst )
        {
         numericscale = scale;
         if( scale > 0 ) *((my_int64*)&res) = - *((my_int64*)&v);
         else res = -v;
        }
      else _SubtractValues(&res, numericscale, &v, scale, &res, &numericscale); // na wyjsciu zadaje skale
     }

   if( bFirst ) bFirst = 0;
  }

if( isNull ) { *pResult = 0; bnull = 1; return DEF_NULL_ERROR; }

memcpy(pResult, &res, sizeof(double));
return 1;
}
//--------------------------

int WZOR::Scan(char* Buf, VECT* pErrList)
{
Clear();

int CurrOp = 1;

char* p = _NextChar(Buf);
while( *p )
  {
   int OpCode = _OperCode(p);
   if( OpCode )
     {
      CurrOp = OpCode;
      int OpLen = _OperLen(OpCode);

      char* pn = _NextChar(p + OpLen);
      if( *pn != '(' )
        {
         char* p1 = _FindNextOper(p + OpLen);

         WZOR wz;
         char* pTemp = (char*)malloc((p1-p)*sizeof(char));
         memcpy(pTemp, p+OpLen, (p1-p-OpLen+1)*sizeof(char));
         pTemp[p1-p-OpLen] = 0;
         int ret = wz.Scan(pTemp, pErrList);
         free(pTemp);
         if( ret < 0 ) { Clear(); return ret; }
         wz.oper = CurrOp;
         list.Append(&wz);
         wz.id_hg.Init();
         wz.list.Init();
         p = _NextChar(p1);
        }
      else p = pn;
     }
   else if( *p == '(' )
     {
      char* p1 = _FindKet(p+1);
      if(!p1)
        {
         if( pErrList )
           {
            char buf[64];
            strcpy(buf, "Brak koncowego nawiasu ')' ...");
            _SetError(pErrList, buf);
           }
         Clear();
         return -1; //brak konczacego nawiasu
        }

      if( p1 != _NextChar(p+1) ) // omijamy pute nawiasy (), ( ), ...
        {
         WZOR wz;
         char* pTemp = (char*)malloc((p1-p)*sizeof(char));
         memcpy(pTemp, p+1, (p1-p)*sizeof(char));
         pTemp[p1-p-1] = 0;
         int ret = wz.Scan(pTemp, pErrList);
         free(pTemp);
         if( ret < 0 ) { Clear(); return ret; }
         wz.oper = CurrOp;
         list.Append(&wz);
         wz.id_hg.Init();
         wz.list.Init();
        }
      p = _NextChar(p1+1);
     }
   else
     {
      char* p1 = _FindNextOper(p);

      if( _IsOper(p1) )
        {
         WZOR wz;
         char* pTemp = (char*)malloc((1+p1-p)*sizeof(char));
         memcpy(pTemp, p, (p1-p)*sizeof(char));
         pTemp[p1-p] = 0;
         int ret = wz.Scan(pTemp, pErrList);
         free(pTemp);
         if( ret < 0 ) { Clear(); return ret; }
         wz.oper = CurrOp;
         list.Append(&wz);
         wz.id_hg.Init();
         wz.list.Init();
         p = _NextChar(p1);
        }
      else
        {
         if( *p == ')' )
           {
            if( pErrList )
              {
               char buf[1024];
               sprintf(buf, "Brak poczatkowego nawiasu '(' dla nawiasu koncowego: \"%s\" ...", p);
               _SetError(pErrList, buf);
              }
            Clear();
            return -4;
           }

         int ok = _GetId(p, &id_hg);
         if( !ok )
           {
            if( pErrList )
              {
               char buf[1024];
               sprintf(buf, "Za dlugi identyfikator: \"%s\" ...", p);
               _SetError(pErrList, buf);
              }
            Clear();
            return -2; //za dlugi identyfikator
           }

         char* id = (char*)id_hg.Ptr();
         if( !id ) id = (char*)"";

         p = _NextChar(p + strlen(id) + ((ok==2)? 2: 0));

         if( *p == '(' ) // id jest funkcja
           {
            char* p1 = _FindKet(p+1);
            if(!p1)
              {
               if( pErrList )
                 {
                  char buf[64];
                  strcpy(buf, "Brak koncowego nawiasu ')' ...");
                  _SetError(pErrList, buf);
                 }
               Clear();
               return -1; //brak konczacego nawiasu
              }

            p++;
            while(1)
              {
               char* pc = _FindComma(p);
               char* p2 = (pc)? pc: p1;

               WZOR wz;
               char* pTemp = (char*)malloc((1+p2-p)*sizeof(char));
               memcpy(pTemp, p, (p2-p)*sizeof(char));
               pTemp[p2-p] = 0;
               int ret = wz.Scan(pTemp, pErrList);
               free(pTemp);
               if( ret < 0 ) { Clear(); return ret; }
               wz.oper = CurrOp;
               list.Append(&wz);
               wz.id_hg.Init();
               wz.list.Init();

               p = _NextChar(p2+1);
               if( !pc ) break;
              }
           }
        }
     }
  }

int ret = CutLogic();
if( ret == 1 )
  {
   ret = CutCompare();
   if( ret == -3 && pErrList )
     {
      char buf[128];
      strcpy(buf, "Za duzo operatorow porownania w wyrazeniu ...");
      _SetError(pErrList, buf);
     }
   if( ret < 0 ) { Clear(); return ret; }
  }

return 1;
}
//--------------------------

int WZOR::CutLogic()
{
int n = list.Nelem();
if( !n ) return 1;

int bLogic = 0;
int bOthers = 0;

WZOR* ptr = (WZOR*)list.Ptr();
for(int i=1; i<n; i++) // pomijamy pierwszy
  {
   int op = ptr[i].oper;
   if( _IsLogic(op) ) bLogic = 1;
   else if( op ) bOthers = 1;

   if( bLogic && bOthers ) break;
  }

if( !bLogic || !bOthers ) return 1;

VECT newlist;
newlist.SetSize(sizeof(WZOR));

int n1 = 0;
for(int i=1; i<=n; i++)
  {
   if( (i<n && _IsLogic(ptr[i].oper)) || i==n )
     {
      if( i > n1+1 )
        {
         WZOR wz;
         wz.oper = ptr[n1].oper;
         ptr[n1].oper = 1; // +
         wz.list.SetSize(sizeof(WZOR));
         for(int j=n1; j<i; j++) wz.list.Append((void*)(ptr+j));

         int ret = wz.CutCompare(); // b. wazne
         if( ret < 0 )
           {
            list.CopyFrom(newlist);
            return ret;
           }

         newlist.Append((void*)&wz);
         wz.id_hg.Init();
         wz.list.Init();
        }
      else newlist.Append((void*)(ptr+n1));
      n1 = i;
     }
  }

list.CopyFrom(newlist);
return 1;
}
//--------------------------

int WZOR::CutCompare()
{
int n = list.Nelem();
if( !n ) return 1;

int nCompare = 0;
int bOthers = 0;

WZOR* ptr = (WZOR*)list.Ptr();
for(int i=1; i<n; i++) // pomijamy pierwszy
  {
   int op = ptr[i].oper;
   if( _IsCompare(op) ) nCompare++;
   else if( op ) bOthers = 1;
  }

if( nCompare > 1 ) return -3; // za duzo operatorow porownania
if( !nCompare || !bOthers ) return 1;

VECT newlist;
newlist.SetSize(sizeof(WZOR));

int n1 = 0;
for(int i=1; i<=n; i++)
  {
   if( (i<n && _IsCompare(ptr[i].oper)) || i==n )
     {
      if( i > n1+1 )
        {
         WZOR wz;
         wz.oper = ptr[n1].oper;
         ptr[n1].oper = 1; // +
         wz.list.SetSize(sizeof(WZOR));
         for(int j=n1; j<i; j++) wz.list.Append((void*)(ptr+j));
         newlist.Append((void*)&wz);
         wz.id_hg.Init();
         wz.list.Init();
        }
      else newlist.Append((void*)(ptr+n1));
      n1 = i;
     }
  }

list.CopyFrom(newlist);
return 1;
}
//--------------------------

static int _IsNumber(char* id)
{
char* end = 0;
strtod(id, &end);
if( end && *end )
  {
   if( !_mycmp(id, (char*)"NULL") ) return 1; // NULL tez jest numeric i nie bedzie w ""
   return 0;
  }
else return 1; // jesli puste id to tez zwraca 1
}
//--------------------------

static void _RemoveBrackets(VECT* pList)
{
int nc = pList->Nelem();
char* p = (char*)pList->Ptr();
if( nc >= 4 && p[0] == '(' && p[1] == '(' && p[nc-1] == ')' && p[nc-2] == ')' )
  {
   char* p1 = _FindKet(p+2, nc-2);
   if( p1 == p+nc-2 )
     {
      pList->Remove(0, 1);
      int nc = pList->Nelem();
      pList->Remove(nc-1, 1);
     }
  }
}
//--------------------------

inline int _IsStrangeChar(char c)
{
if( c == ' ' || c == '+' || c == '-' || c == '*'
|| c == '/' || c == '\\' || c == '!' || c == '%'
|| c == '^' || c == '&' || c == '|' || c == '('
|| c == ')' || c == ':' || c == ';' || c == '\''
|| c == '\"' || c == '?' || c == ',' || c == '.'
|| c == '[' || c == ']' || c == '{' || c == '}'
|| c == '<' || c == '>' || c == '=' || c == '`' ) return 1;
else return 0;
}
//--------------------------

static int _HasStrangeChar(char* s)
{
if( !s ) return 0;

char* p = s;
while(*p)
  {
   if( _IsStrangeChar(*p) ) return 1;
   p++;
  }

return 0;
}
//--------------------------

static int _AddQuot(char* name)
{
if( !name ) return 0;
if( name[0] == 0 ) return 0;

if( _HasStrangeChar(name) ) return 1;

return 0;
}
//-----------------------------------

void WZOR::FormatHelp(VECT* pList)
{
if( !pList->Nelem() ) pList->SetSize(sizeof(char));

char opbuf[16] = "";
if( oper == 1 ) strcpy(opbuf, " + ");
else if( oper == 2 ) strcpy(opbuf, " - ");
else if( oper == 3 ) strcpy(opbuf, "*");
else if( oper == 4 ) strcpy(opbuf, "/");
else if( oper == 5 ) strcpy(opbuf, "^");

else if( oper == 101 ) strcpy(opbuf, " >= ");
else if( oper == 102 ) strcpy(opbuf, " <= ");
else if( oper == 103 ) strcpy(opbuf, " > ");
else if( oper == 104 ) strcpy(opbuf, " < ");
else if( oper == 105 || oper == 1051 ) strcpy(opbuf, " = ");
else if( oper == 106 ) strcpy(opbuf, " != ");
else if( oper == 107 ) strcpy(opbuf, " || ");
else if( oper == 108 || oper == 1081 ) strcpy(opbuf, " && ");

char* id = (char*)id_hg.Ptr();
if( !id ) id = (char*)"";

char buf[1024] = "";

int nc = pList->Nelem();
if( (!nc || ( nc && *((char*)pList->Ptr(nc-1)) == '(' )) && oper == 1 ) *opbuf = 0;
int n1 = strlen(opbuf);
int n2 = strlen(id);

int n = list.Nelem();

int bFun = (n2 && n);

if( n1 && n2 )
  {
   if( bFun || *id == '\'' || *id == '%' || _IsNumber(id) ) sprintf(buf, "%s%s", opbuf, id);
   else
     {
      if( _AddQuot(id) ) sprintf(buf, "%s\"%s\"", opbuf, id);
      else sprintf(buf, "%s%s", opbuf, id);
     }
  }
else if( n1 ) sprintf(buf, "%s", opbuf);
else if( n2 )
  {
   if( bFun || *id == '\'' || *id == '%' || _IsNumber(id) ) sprintf(buf, "%s", id);
   else
     {
      if( _AddQuot(id) ) sprintf(buf, "\"%s\"", id);
      else sprintf(buf, "%s", id);
     }
  }

int nn = strlen(buf);
if( nn ) pList->Append(buf, nn);

if( !n ) return;

strcpy(buf, "(");
pList->Append(buf, 1);

WZOR* ptr = (WZOR*)list.Ptr();
for(int i=0; i<n; i++)
  {
   if( n2 ) // jesli funkcja
     {
      VECT lst;
      ptr[i].FormatHelp(&lst);
      int nel = lst.Nelem();
      if( nel )
        {
         char* pc = (char*)lst.Ptr();
         pc = _NextChar(pc);
         if( *pc == '+' ) pc = _NextChar(pc+1);
         int diff = (int)(pc - (char*)lst.Ptr());
         if( diff < nel ) pList->Append(pc, nel-diff);
        }
     }
   else ptr[i].FormatHelp(pList);

   if( n2 && i<n-1 )
     {
      strcpy(buf, ", ");
      pList->Append(buf, 2);
     }
  }

nc = pList->Nelem();
if( nc && *((char*)pList->Ptr(nc-1)) == ' ' ) pList->Remove(nc-1, 1);

strcpy(buf, ")");
pList->Append(buf, 1);

_RemoveBrackets(pList);
}
//--------------------------

void WZOR::Format(VECT* pList)
{
FormatHelp(pList);

int nc = pList->Nelem();
char* p = (char*)pList->Ptr();
if( nc && p[0] == '(' && p[nc-1] == ')' )
  {
   char* p1 = _FindKet(p+1, nc-1);
   if( p1 == p+nc-1 )
     {
      pList->Remove(0, 1);
      int nc = pList->Nelem();
      pList->Remove(nc-1, 1);
     }
  }

char null = 0;
pList->Append(&null, 1);
}

