#include <stdio.h>

struct myst {
  int index;
  char c;
  int age;
};

int foo(int A, int *B, int C, int *D, int **F, struct myst *M) {
  int *E;
  if (A + C > 0)
     E = B;
  else 
     E = D;
  *E = A;
  int temp = **F;
  M->age = temp;
  return C;     
}
