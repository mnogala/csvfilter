
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_VECT_H
#define MN_VECT_H
/***********************************/

class VECT
{
void*    ptr;
unsigned nbytes;
unsigned size;

public:

VECT();
~VECT();

void  Init();

int   Alloc(int nelem, int _size = 1);
int   ReAlloc(int newnelem);
void  Free();
int   Insert(int pos, void* buf, int ile);
int   Append(void* buf, int ile = 1);
int   Remove(int pos, int ile);

int   SetSize(int newsize);

int   Nelem();
unsigned int Size() { return size; }
unsigned int Bytes() { return nbytes; }

void  CopyFrom(VECT& hg);

void* Ptr(int i=0)
  {
   if( nbytes == 0 ) return 0;
   if(i==0) return ptr;
   unsigned int ofs = i*size;
   return (i>0 && ofs<nbytes)? ( ((char*)ptr) + ofs ): 0;
  }
};
/***********************************/
#endif // MN_VECT_H

