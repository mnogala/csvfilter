
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_defs.h"
#include <stdlib.h>
/***********************************/

extern "C"
my_int64 my_atoi64(const char* s)
{
#ifdef linux
return atoll(s);
#else
return _atoi64(s);
#endif
}
//------------------------

extern "C"
void my_mutex_init(my_mutex* pMtx)
{
#ifdef linux
pthread_mutex_init(pMtx, 0);
#else
InitializeCriticalSection(pMtx);
#endif
}
//------------------------

extern "C"
void my_mutex_exit(my_mutex* pMtx)
{
#ifdef linux
pthread_mutex_destroy(pMtx);
#else
DeleteCriticalSection(pMtx);
#endif
}
//------------------------

extern "C"
void my_lock(my_mutex* pMtx)
{
#ifdef linux
pthread_mutex_lock(pMtx);
#else
EnterCriticalSection(pMtx);
#endif
}
//------------------------

extern "C"
void my_unlock(my_mutex* pMtx)
{
#ifdef linux
pthread_mutex_unlock(pMtx);
#else
LeaveCriticalSection(pMtx);
#endif
}
//------------------------

extern "C"
void my_thread_start(my_thread* pthr, void* (*fun)(void*), void* param)
{
#ifdef linux
pthread_create(pthr, 0, fun, param);
#else
_beginthread((void (*)(void*))fun, 0, param);
#endif
}

