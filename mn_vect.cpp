
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_vect.h"

#include <stdlib.h>
#include <string.h>

//*****************************************************************

VECT::VECT()
{
ptr = 0;
nbytes = 0L;
size = 0;
}
//-----------------------------------------------------------------

void VECT::Init()
{
ptr = 0;
nbytes = 0L;
size = 0;
}
//-----------------------------------------------------------------

VECT::~VECT()
{
Free();
}
//-----------------------------------------------------------------

int VECT::Alloc(int nelem, int _size)
{
Free();

size = _size;
if( !nelem || !size ) return 1;

int nb = nelem*size;
ptr = malloc(nb);
if( !ptr ) return 0;

nbytes = nb;
memset(ptr, 0, nbytes);
return 1;
}
//-----------------------------------------------------------------

int VECT::ReAlloc(int newnelem)
{
if( newnelem <= 0 && nbytes <= 0 ) { ptr = 0; return 1; }

if( newnelem <= 0 || size <= 0 )
  {
   if(ptr) free(ptr);
   ptr = 0;
   nbytes = 0L;
   return 1;
  }

void* p;
if( !ptr ) p = malloc(newnelem*size);
else p = realloc(ptr, newnelem*size);
if( !p && newnelem != 0L ) return 0;

ptr = p;
unsigned int oldnbytes = nbytes;
nbytes = newnelem*size;

if(nbytes>oldnbytes) memset(((char*)ptr)+oldnbytes, 0, nbytes-oldnbytes);
return 1;
}
//-----------------------------------------------------------------

void VECT::Free()
{
if(ptr) free(ptr);
ptr = 0;
nbytes = 0L;
size = 0;
}
//-----------------------------------------------------------------

int VECT::Insert(int pos, void* buf, int ile)
{
if( !buf ) return 0;
if ( pos*size > nbytes || ile < 0L ) return 0;

if( !ptr && !size ) return 0;

if( ile == 0L ) return 1;
// if empty
if( ptr == 0 && size != 0 && pos == 0L )
  {
   if( !Alloc( ile, size ) ) return 0;
   memcpy( ptr, buf, ile*size );
   return 1;
  }

unsigned int oldnbytes = nbytes;
if( !ReAlloc( Nelem() + ile ) ) return 0;

if( pos*size < oldnbytes ) memmove( ((char*)ptr) + (pos+ile)*size, ((char*)ptr) + pos*size, oldnbytes-pos*size );
memcpy( ((char*)ptr) + pos*size, buf, ile*size );

return 1;
}
//-----------------------------------------------------------------

int VECT::Append(void* buf, int ile)
{
if( !buf ) return 0;
return Insert(Nelem(), buf, ile);
}
//-----------------------------------------------------------------

int VECT::Remove(int pos, int ile)
{
if( ptr == 0 || pos >= Nelem() ) return 0;
if( ile == 0L ) return 1;

if( pos+ile < Nelem() ) memmove( ((char*)ptr) + pos*size, ((char*)ptr) + (pos + ile)*size, nbytes - (pos + ile)*size );

if( !ReAlloc(Nelem() - ile) ) return 0;
return 1;
}
//-----------------------------------------------------------------

int VECT::SetSize(int newsize)
{
int oldsize = size;
size = newsize;
return oldsize;
}
//-----------------------------------------------------------------

int VECT::Nelem()
{
if( !nbytes || !size ) return 0L;
return nbytes/size;
}
//-----------------------------------------------------------------

void VECT::CopyFrom(VECT& hg)
{
int nel = hg.Nelem();
if(!nel)
  {
   Free();
   size = hg.Size();
   return;
  }
int s = hg.Size();
if(!s)
  {
   Free();
   size = 0;
   return;
  }

Alloc(nel, s);
memcpy(ptr, hg.Ptr(), nbytes);
}

