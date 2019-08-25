#include <stdio.h>
#include <string.h>

#define ru1 3
#define ru2 4
#define ru3 5
#define ru4 6
#define ru5 7
#define ru6 8
#define ru7 9
#define ru8 10

int _mm_fake1(int);
int _mm_fake2(int,int);
int _mm_fake3(int,int);
int _mm_fake4(int,int);
int _mm_fake5(int,int);

int loop(int ru, int base) {
    int result = base;
    for(int i=0; i<ru; i++)
       result = result * base;   
     return result;
}

int foo(int L, int H) {
    int H1 = _mm_fake1(H);
    int H2 = _mm_fake2(H,L);
    int H3 = _mm_fake3(H,L); 
    int H4 = _mm_fake4(L,H);
    int L5 = _mm_fake5(L,L);

    if (H1 > 0 || H2 > 0 || H3 > 0 || H4 > 0) {
       if (L5 > 0) {
          printf("%d\n", loop(ru1,2));
          memset(&H1, 0, sizeof(H1));
          H2 = 0;   
          printf("cleanued up %d%d\n", H1, H2);
          return 1;         
       }
       else {
          printf("%d\n", loop(ru2,2));
          memset(&H1, 0, sizeof(H1));
          H2 = 0;   
          printf("cleanued up %d%d\n", H1, H2);
          return 1; 
       }
    }
    else {
       if (L5 > 0) {
          printf("%d\n", loop(ru8,2));
          memset(&H1, 0, sizeof(H1));
          H2 = 0;   
          printf("cleanued up %d%d\n", H1, H2);
          return 1; 
       }
       else {
          printf("%d\n", loop(ru7,2));
          memset(&H1, 0, sizeof(H1));
          H2 = 0;   
          printf("cleanued up %d%d\n", H1, H2);
          return 1; 
       }
    }   
   
}

