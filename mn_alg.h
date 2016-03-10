
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_ALG_H
#define MN_ALG_H

#include "mn_int64.h"
#include "mn_vect.h"
#include <string.h>

#define DEF_ID_MAXLEN 64

#define DEF_MATHLIB_ERROR -200
#define DEF_NULL_ERROR -300
//***********************************

class WZOR
{
public:
my_int64     val;
int          narg;
short        bnull;
short        oper; // 1 +, 2 -, 3 *, 4 /, 5 ^

// scala do obliczen opartych na INT64
// Wartosci: 1, 2, 3, ... - val bedzie __int64 o skali odpowiednio 0, 1, 2, ... (trzeba odjac 1)
//           0 (- val bedzie double)
short        numericscale;

VECT         id_hg;
VECT         list;

int     SetVal(VECT* pValList);
//void    DisplayHelp(VECT* pList);
void    FormatHelp(VECT* pList);

int     CutLogic();
int     CutCompare();

public:
WZOR();
int     GetScale() { return numericscale; };
void    Clear();
int     Scan(char* Buf, VECT* pErrList=0);
int     Calculate(my_int64* pResult, VECT* pValList, VECT* pErrList, int bTest);

void    Format(VECT* pList);
void    CopyFrom(WZOR&);
};
//***********************************

struct IDVAL_STRUCT
{
char  id[DEF_ID_MAXLEN];
my_int64 val;
int      bnull;
int      scale;

IDVAL_STRUCT() { memset(this, 0, sizeof(IDVAL_STRUCT)); }
IDVAL_STRUCT(char* _id) { strcpy(id, _id); val = 0; bnull = 1; scale = 0; } // null
IDVAL_STRUCT(char* _id, double* pval) { strcpy(id, _id); memcpy(&val, pval, sizeof(val)); bnull = 0; scale = 0; }
IDVAL_STRUCT(char* _id, my_int64 x, int _scale) { strcpy(id, _id); val = x; bnull = 0; scale = _scale; }
};

//***********************************
#endif // MN_ALG_H

