#include <stdio.h>
#include <stdlib.h>
#include "klee/klee.h"

struct data {
  char *buf;
};

struct data2 {
  char buf2;
};

struct data2 d2;

struct device {
   int devId;
   //char *name;
   char name[24];
   struct data *f2;
};

struct usbtv {
   struct device *dev;
};

extern int foo(char *);

void api1(struct device *dev) {
   if (dev == 0)
      return;
   else {
      if (dev->devId > 5) {
         if (dev->f2->buf[0] == 'C')
            printf(">5 data C\n");
         if (dev->name[0] == 'A') {
            printf(">5 A\n");
         }
         else printf(">5 not A\n");
      }
      else {
         char y;
         int x = foo(&y);
         if (y == 'G')
            printf("d2->buf G\n");
         else printf("d2->buf not G\n");
         if (x > 7)
            printf(">7 X\n");
         else printf("<=7 X\n");
         if (dev->name[1] == 'B') {
            printf("<=5 B\n");
         } 
         else printf("<=5 not B\n");
      } 
   }
}

int main(int argc, char **argv) {

  printf("sizeof struct data %lu\n", sizeof(struct data));
  struct device locdev;
  //locdev.name = malloc(8);
  klee_make_symbolic(&locdev,sizeof(locdev),"locdev");
  api1(&locdev);
  return 0;  
}
