#include <stdio.h>


int foo(int A, int *B, int C, int *D) {
  int *E;
  if (A + C > 0)
     E = B;
  else 
     E = D;
  *E = A;
  return C;     
}
