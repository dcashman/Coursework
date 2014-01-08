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
#include <sys/types.h>
#include <unistd.h>


#include "pin.H"

FILE * out;
PIN_LOCK lock;
pid_t parent_pid;

VOID BeforeFork(THREADID threadid, const CONTEXT* ctxt, VOID * arg)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "BeforeFork\n");
    fflush(out);
    ReleaseLock(&lock);
    parent_pid = PIN_GetPid();
}
VOID AfterForkInParent(THREADID threadid, const CONTEXT* ctxt, VOID * arg)
{
    GetLock(&lock, threadid+1);
    fprintf(out, "AfterForkInParent\n");
    fflush(out);
    ReleaseLock(&lock);
    if (PIN_GetPid() != parent_pid)
    {
    	fprintf(out, "PIN_GetPid() fails in parent process\n");
    }
    else
    {
    	fprintf(out, "PIN_GetPid() is correct in parent process\n");
    }    
}
VOID AfterForkInChild(THREADID threadid, const CONTEXT* ctxt, VOID * arg)
{
    // After the fork, there is only one thread in the child process.  It's possible
    // that a different thread in the parent held this lock when the fork() happened.
    // Since that thread does not exist in the child, it will never release the lock.
    // Compensate by re-initializing the lock here in the child.

    InitLock(&lock);
    GetLock(&lock, threadid+1);
    FILE *child_out = fopen("forkcallback.child.out", "w");
    fprintf(child_out, "AfterForkInChild\n");
    ReleaseLock(&lock);
    if ((PIN_GetPid() == parent_pid) || (getppid() != parent_pid))
    {
    	fprintf(child_out, "PIN_GetPid() fails in child process\n");
    }
    else
    {
    	fprintf(child_out, "PIN_GetPid() is correct in child process\n");
    }    
    fclose(child_out);
}


VOID Fini(INT32 code, VOID *v)
{
    fclose(out);
}

int main(INT32 argc, CHAR **argv)
{
	BOOL success;
    InitLock(&lock);
    
    out = fopen("forkcallback.out", "w");
    
    PIN_Init(argc, argv);
    
    success = PIN_AddForkFunction(FPOINT_BEFORE, BeforeFork, 0);
	ASSERTX(success);
    success = PIN_AddForkFunction(FPOINT_AFTER_IN_PARENT, AfterForkInParent, 0);
	ASSERTX(success);
    success = PIN_AddForkFunction(FPOINT_AFTER_IN_CHILD, AfterForkInChild, 0);
	ASSERTX(success);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
