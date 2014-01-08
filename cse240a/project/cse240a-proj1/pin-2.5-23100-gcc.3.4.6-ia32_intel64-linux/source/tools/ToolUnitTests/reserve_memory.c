/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2008 Intel Corporation 
All rights reserved. 
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.
 
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[])
{
   FILE *f = fopen(argv[1], "r");
   int access_flag = atoi(argv[2]);

   if ( !f )
   {
       fprintf(stderr, "cannot open file %s\n", argv[1]);
       return 0; 
   }

   long int low = 0, high = 0, size = 0;
   int tid;
   char desc[64]; 
   char * ptr = (char *)NULL;
   int i;

   while (!feof(f))
   {
      fscanf(f, "%lx %lx %s %d",  &low, &high, desc, &tid);
      /*fprintf(stdout, "%lx %lx %s %d\n", low, high, desc, tid);*/
      if ( feof(f) )
          break;

      size = high - low;
      fprintf(stdout, "%lx\n", size);
      assert(size>0);

      for ( i = 0; i < size; i += getpagesize() )
      {
          ptr = (char *)(low + i);
          if ( access_flag )
              *ptr = 0;
          fprintf(stdout, "%lx\n", (low+i)); 
      }
   }
   fclose(f);
   return 0;
}
