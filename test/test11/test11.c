#include <stdio.h>

#define ru1 3
#define ru2 4
#define ru3 5
#define ru4 6
#define ru5 7
#define ru6 8
#define ru7 9
#define ru8 10

int loop(int ru, int base) {
    int result = base;
    for(int i=0; i<ru; i++)
       result = result * base;   
     return result;
}

struct mytest {
  int nonsens;
  int L;
  int H;
};

int foo(struct mytest *t) {
    if (t->H > 0) {
        if (t->L<1) {
           printf("%d\n", loop(ru1,2));
           return 1;         
        }
        else if (t->L<5) {
           printf("%d\n", loop(ru2,2));
           return 1; 
        }
        else {
           printf("%d\n", loop(ru3,2));
           return 1;
        }
    }
    else {
        if (t->L<0) {
           if (t->H > -10) {
              if (t->L == -1) {
                 printf("%d\n", loop(ru4,2));
                 return 1;
              }
              else { 
                 printf("%d\n", loop(ru5,2)); 
                 return 1; 
              }
           }
           else {
              if (t->L == -1) {
                 printf("%d\n", loop(ru6,2));
                 return 1;
              }
              else {
                 printf("%d\n", loop(ru7,2));
                 return 1; 
              }
           }
        }
        else {
           printf("%d\n", loop(ru8,2));
           return 1;
        }
    } 
}
