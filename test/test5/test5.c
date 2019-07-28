#include <stdio.h>

#define ru1 3
#define ru2 4
#define ru3 5
#define ru4 6
#define ru5 7
#define ru6 8
#define ru7 9
#define ru8 10

struct data_t {
  int value;
};

struct secret_t {
  struct data_t *data;
};

struct public_t {
   struct data_t *data;
}; 

int loop(int ru, int base) {
    int result = base;
    for(int i=0; i<ru; i++)
       result = result * base;   
     return result;
}

int foo(struct public_t L, struct secret_t H) {
    if (H.data->value > 0) {
        if (L.data->value<1) {
           printf("%d\n", loop(ru1,2));
           return 1;         
        }
        else if (L.data->value<5) {
           printf("%d\n", loop(ru2,2));
           return 1; 
        }
        else {
           printf("%d\n", loop(ru3,2));
           return 1;
        }
    }
    else {
        if (L.data->value<0) {
           if (H.data->value > -10) {
              if (L.data->value == -1) {
                 printf("%d\n", loop(ru4,2));
                 return 1;
              }
              else { 
                 printf("%d\n", loop(ru5,2)); 
                 return 1; 
              }
           }
           else {
              if (L.data->value == -1) {
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
