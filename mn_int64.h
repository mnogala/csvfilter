
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_INT64_H
#define MN_INT64_H
/***********************************/

#include "mn_defs.h"

int _CheckInt64Len(char* s);
my_int64 _Int64Rescale(my_int64 x, int OldScale, int NewScale, int Option); // Option = (0-normalnie, 1-w gore, 2- wdol)
int _StringToInt64(char* s, int scale, my_int64* px);
double _Int64ToDouble(my_int64 x, int scale);
my_int64 _DoubleToInt64(double x, int scale);

/***********************************/
#endif // MN_INT64_H

