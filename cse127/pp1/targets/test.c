#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "tmalloc.h"


int main(int argc, char *argv[])
{

  int i, b;
  b=28;
  i=0x800000c9;
  printf("Addition of 0x800000c9: %x\nMultiplication of 0x800000c9: %x\n", i+b, i*b);

  return 0;
}
