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
#define _GNU_SOURCE
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/mman.h>

static void DoIntX(int);

extern void DoUD2();
extern void DoIdiv0();
extern void DoSqrtNeg1();
extern void DoSSEDiv0();
extern void DoSSEPrec();
extern void DoInt3();
extern void DoIntO();
extern void DoBound();


int DoTest(unsigned int tnum)
{
    unsigned int base;

    switch (tnum)
    {
    case 0:
        printf("  SEGV\n");
        *(int *)0x9 = 0;
        return 1;
    case 1:
        printf("  UD2\n");
        DoUD2();
        return 1;
    case 2:
        printf("  idiv zero\n");
        DoIdiv0();
        return 1;
    case 3:
        printf("  sqrt -1\n");
        DoSqrtNeg1(); return 1;
    case 4:
        printf("  SSE div0\n");
        DoSSEDiv0();
        return 1;
    case 5:
        printf("  SSE prec\n");
        DoSSEPrec();
        return 1;
    case 6:
        printf("  INT3\n");
        DoInt3();
        return 1;
    case 7:
        printf("  INTO\n");
        DoIntO();
        return 1;
    case 8:
        printf("  Bound\n");
        DoBound();
        return 1;
    }

    base = 9;
    if (tnum - base < 256)
    {
        DoIntX(tnum - base);
        return 1;
    }

    return 0;
}

static void DoIntX(int num)
{
    void (*fn)(void);
    int trap;
    int i;

    static unsigned init = 0;

#   define NUMBER_INSTRUCTIONS 8
    static unsigned char code[NUMBER_INSTRUCTIONS*256];

    if (!init)
    {
        /*
         * Make sure the 'code' array is executable.
         */
        unsigned long addrLo;
        unsigned long addrHi;
        size_t pageSize;
        unsigned long pageMask;
        size_t sizeCode;

        addrLo = (unsigned long)&code[0];
        addrHi = (unsigned long)&code[NUMBER_INSTRUCTIONS*256];
        pageSize = getpagesize();
        pageMask = ~(pageSize-1);

        addrLo = addrLo & pageMask;
        sizeCode = addrHi - addrLo;
        sizeCode = (sizeCode + pageSize) & pageMask;
        mprotect((void *)addrLo, sizeCode, (PROT_READ | PROT_WRITE | PROT_EXEC));

        /*
         * Generate a series of functions which execute all possible 'int x' instructions.
         * Each function sets %eax to -1 first because some 'int x' instructions perform system
         * calls.  Setting %eax to -1 ensures that the system call is invalid.
         */
        for (i = 0, trap=0;  i < 256*NUMBER_INSTRUCTIONS;  i+=NUMBER_INSTRUCTIONS, trap++)
        {
            int c = 0;

            code[i + (c++)] = 0xb8;   /* mov $0xffffffff, %eax */
            code[i + (c++)] = 0xff;
            code[i + (c++)] = 0xff;
            code[i + (c++)] = 0xff;
            code[i + (c++)] = 0xff;
            code[i + (c++)] = 0xcd;   /* int <trap> */
            code[i + (c++)] = trap;
            code[i + (c++)] = 0xc3;   /* ret */

            assert(c == NUMBER_INSTRUCTIONS);
        }
        init = 1;
    }

    printf("  INT %u   (at %p)\n", num, &code[num*NUMBER_INSTRUCTIONS + 5]);

    /*
     * Some kernels do not handle the 2-byte 'int 3' instruction correctly.
     * See mantis #666 for details.
     */
    if (num == 3)
        return;

    fn = (void (*)(void))&code[num*NUMBER_INSTRUCTIONS];
    fn();
}

void PrintSignalContext(int sig, const siginfo_t *info, void *vctxt)
{
    ucontext_t *ctxt = vctxt;
    printf("  Signal %d, pc=0x%lx, si_addr=0x%lx, si_code=%d, si_errno=%d, trap_no=%ld\n", sig,
        (unsigned long)ctxt->uc_mcontext.gregs[REG_EIP],
        (unsigned long)info->si_addr,
        (int)info->si_code,
        (int)info->si_errno,
        (long int)ctxt->uc_mcontext.gregs[REG_TRAPNO]);
}
