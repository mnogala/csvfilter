
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_CZAS_H
#define MN_CZAS_H
/***********************************/
#include <sys/timeb.h>

class CZAS
{
timeb t1;
timeb t2;

public:
void start() { ftime(&t1); }
void stop() { ftime(&t2); }
int msek() { return 1000*(t2.time - t1.time) + (t2.millitm - t1.millitm); }
float sek() { return 0.001*msek(); }
};

/***********************************/
#endif // MN_CZAS_H

