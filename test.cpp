
/* Mariusz Nogala, mnogala@amu.edu.pl */

#include "mn_csv.h"
#include "mn_czas.h"

#include <iostream>

/***********************/

int main()
{
const char* FnameIn = "input.csv";
const char* FnameOut = "result.csv";
const char* Filter = "((k1 > 600 and k2 = 446) or k3 = 999) or (k5 = 2*k6 + 1 and k8 >= k9)";

unsigned nThreads = 2;
int bOverWriteResult = 1;
int bTest = 1;
my_int64 nInputRows, nResultRows;
char ErrorBuf[512];
char AstBuf[512];

CZAS t;
t.start();
int RetValue = csvfilter(FnameIn, FnameOut, Filter, nThreads, bOverWriteResult, bTest, &nInputRows, &nResultRows, ErrorBuf, sizeof(ErrorBuf), AstBuf, sizeof(AstBuf));
t.stop();

if( RetValue < 0 )
  {
   std::cout<< "Komunikat bledu:\n" << ErrorBuf << "\n";
  }
else
  {
   std::cout << "Liczba wierszy zrodlowych: " << nInputRows << "\n"
   << "Liczba wierszy wynikowych: " << nResultRows << "\n"
   << "Czas przetwarzania: " << t.sek() << " sekund\n";
  }

if( AstBuf[0] ) std::cout << "\nAST:\n" << AstBuf <<"\n";

return 0;
}

