
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_DEFS_H
#define MN_DEFS_H
/***********************************/

#ifdef linux
  #include <pthread.h>
  typedef long long my_int64;
  typedef pthread_mutex_t my_mutex;
  typedef pthread_t my_thread;
#else
  #include <windows.h>
  #include <process.h>
  typedef __int64 my_int64;
  typedef CRITICAL_SECTION my_mutex;
  typedef int my_thread;
#endif
//------------------------

#ifdef __cplusplus
extern "C" {
#endif

my_int64 my_atoi64(const char* s);
void my_mutex_init(my_mutex* pMtx);
void my_mutex_exit(my_mutex* pMtx);
void my_lock(my_mutex* pMtx);
void my_unlock(my_mutex* pMtx);
void my_thread_start(my_thread* pthr, void* (*fun)(void*), void* param);

#ifdef __cplusplus
}
#endif

/***********************************/
#endif // MN_DEFS_H

