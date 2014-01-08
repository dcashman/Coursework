#include "syscall.h"
#include "stdio.h"
#include "stdlib.h"

#define BUFSIZE 1024

char buf[BUFSIZE];
char buff2[BUFSIZE];

int main(int argc, char** argv)
{
  int src, dst, amount;



  src = open("testsrc.txt");
  if (src==-1) {
    printf("Unable to open %s\n", argv[1]);
    return 1;
  }

  creat("testdest.txt");
  dst = open("testdest.txt");
  if (dst==-1) {
    printf("Unable to create %s\n", "testdest.txt");
    return 1;
  }

  while ((amount = read(src, buf, BUFSIZE))>0) {
    write(dst, buf, amount);
  }

  for(int i=0; i<BUFSIZE;i++){
    buff2[i]='a';
  }
  write(dst,buff2,BUFSIZE);
  

  close(src);
  close(dst);

  return 0;
}
