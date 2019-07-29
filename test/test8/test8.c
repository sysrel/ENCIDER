#include <stdio.h>

int table[128];

int foo(int L, int H) {
    if (H >=0 && H < 128) {
       if (L > 10)  
           return table[H];
       else return table[L+H];
    }
    else if (L > 0) {
       for(int i=0; i < 128; i++)
          table[i] = 2*i;
    }
    return table[127]; 
}
