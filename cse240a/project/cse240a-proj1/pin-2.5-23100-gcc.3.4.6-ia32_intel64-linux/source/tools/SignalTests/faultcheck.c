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
/*
 * This test tries to raise each type of synchronous signal and prints
 * out various state information about the fault context.  The test can
 * be run under Pin to test whether the fields have the same values
 * when run natively.
 */

#include <signal.h>
#include <stdio.h>
#include <setjmp.h>


static sigjmp_buf JumpBuffer;
static unsigned int TestNumber = 0;

static void Handle(int, siginfo_t *, void *);
extern int DoTest(unsigned int);
extern void PrintSignalContext(int, const siginfo_t *, void *);


int main()
{
    struct sigaction sigact;

    sigact.sa_sigaction = Handle;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_SIGINFO;
    if (sigaction(SIGSEGV, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGSEGV\n");
        return 1;
    }
    if (sigaction(SIGBUS, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGBUS\n");
        return 1;
    }
    if (sigaction(SIGILL, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGILL\n");
        return 1;
    }
    if (sigaction(SIGFPE, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGSEGV\n");
        return 1;
    }
    if (sigaction(SIGTRAP, &sigact, 0) == -1)
    {
        fprintf(stderr, "Unable handle SIGTRAP\n");
        return 1;
    }

    sigsetjmp(JumpBuffer, 1);

    for (;;)
    {
        printf("Starting test %d ...\n", TestNumber);
        if (!DoTest(TestNumber))
        {
            printf("Last test\n");
            break;
        }
        printf("Failed to raise signal\n");
        TestNumber++;
    }

    return 0;
}


static void Handle(int sig, siginfo_t* info, void* ctxt)
{
    PrintSignalContext(sig, info, ctxt);
    TestNumber++;
    siglongjmp(JumpBuffer, 1);
}
