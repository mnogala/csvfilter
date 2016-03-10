
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_fastbuf.h"

#include <stdlib.h>
#include <string.h>

FASTBUF::FASTBUF(unsigned _item_size, unsigned _n_alloc)
{
ptr = 0;
n_total = 0;
n_added = 0;
item_size = _item_size;
if( item_size < 1 ) item_size = 1;
n_alloc = _n_alloc;
if( n_alloc < 1024 ) n_alloc = 1024;
}
//-----------------------

FASTBUF::~FASTBUF()
{
if(ptr) free(ptr);
}
//-----------------------

int FASTBUF::Add(void* pItem)
{
if( !pItem ) return 0;

if( !ptr )
  {
   n_added = 0;
   ptr = malloc(n_alloc*item_size);
   if( !ptr ) { n_total = 0; return 0; }
   n_total = n_alloc;
  }

if( n_added == n_total )
  {
   ptr = realloc(ptr, (n_total + n_alloc)*item_size);
   if( !ptr ) { n_total = 0; n_added = 0; return 0; }
   n_total += n_alloc;
  }

if( item_size == 1 ) *((char*)ptr + n_added) = *((char*)pItem);
else if( item_size == 2 ) *((short*)ptr + n_added) = *((short*)pItem);
else if( item_size == 4 ) *((int*)ptr + n_added) = *((int*)pItem);
else memcpy(((char*)ptr) + n_added*item_size, pItem, item_size);

n_added++;
return 1;
}

