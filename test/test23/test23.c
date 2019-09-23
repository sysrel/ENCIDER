#include <stdio.h>


int foo(int A, int *B, int C, int *D, int *G) {
  int *E;
  int *F = G;
  if (A + C > 0) {
     E = B;
     *G = C;
  }
  else {
     E = D;
  }
  *E = A;
  *F = 0; 
  return C;     
}
