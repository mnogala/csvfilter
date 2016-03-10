
/* Mariusz Nogala, mnogala@amu.edu.pl */

#ifndef MN_READER_H
#define MN_READER_H
/***********************************/

#include "mn_vect.h"
#include "mn_int64.h"

#include <stdio.h>

class READER
{
unsigned buf_size;
int b_create_map;
my_int64 n_rows;
FILE* p_file;
VECT hg_fname;
VECT hg_buf;
VECT hg_tmp;
VECT hg_map;

int ReadHelp(VECT* pMem, int* pWasNL);

public:
READER(const char* _fname, int _b_create_map=0, unsigned _buf_size = 32768);
~READER();

int IsOpened() { return (p_file != 0); }
int Read(VECT* pMem);
int ReadFirstLine(VECT* pMem);
void Reset();
my_int64 Rows() { return n_rows; };
unsigned MapNelem() { return hg_map.Nelem(); };
int SetMap(my_int64 row);
int GetMap(my_int64 row);
int Reopen();
};

/***********************************/
#endif // MN_READER_H

