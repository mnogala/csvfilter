
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_FASTBUF_H
#define MN_FASTBUF_H
/***********************************/

class FASTBUF
{
void* ptr;
unsigned item_size;
unsigned n_alloc;
unsigned n_total;
unsigned n_added;

public:
FASTBUF(unsigned _item_size = 1, unsigned _n_alloc = 1024);
~FASTBUF();

int Add(void* pItem);
void Clear() { n_added = 0; }
unsigned Nelem() { return n_added; }

void* Ptr(int i=0)
  {
   if( n_added == 0 ) return 0;
   if(i==0) return ptr;
   return (i>0 && i<n_added)? (((char*)ptr) + i*item_size): 0;
  }
};

/***********************************/
#endif // MN_FASTBUF_H

