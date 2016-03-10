
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_CSV_H
#define MN_CSV_H

#include "mn_defs.h"
/***********************************/

#ifdef __cplusplus
extern "C"
#endif

int csvfilter(const char* InputName, const char* ResultName,
const char* Filter, unsigned nThreads, int bOverWriteResult, int bTest,
my_int64* p_nInputRows, my_int64* p_nResultRows,
char* ErrorBuf, unsigned ErrorBufSize,
char* AstBuf, unsigned AstBufSize);

/***********************************/
#endif // MN_CSV_H

