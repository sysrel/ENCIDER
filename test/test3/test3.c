#include <stdio.h>

#define ru1 3
#define ru2 4
#define ru3 5
#define ru4 6
#define ru5 7
#define ru6 8
#define ru7 9
#define ru8 10

struct secret_t {
  int data;
};

struct public_t {
   int data;
}; 

int loop(int ru, int base) {
    int result = base;
    for(int i=0; i<ru; i++)
       result = result * base;   
     return result;
}

int foo(struct public_t L, struct secret_t H) {
    if (H.data > 0) {
        if (L.data<1) {
           printf("%d\n", loop(ru1,2));
           return 1;         
        }
        else if (L.data<5) {
           printf("%d\n", loop(ru2,2));
           return 1; 
        }
        else {
           printf("%d\n", loop(ru3,2));
           return 1;
        }
    }
    else {
        if (L.data<0) {
           if (H.data > -10) {
              if (L.data == -1) {
                 printf("%d\n", loop(ru4,2));
                 return 1;
              }
              else { 
                 printf("%d\n", loop(ru5,2)); 
                 return 1; 
              }
           }
           else {
              if (L.data == -1) {
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
