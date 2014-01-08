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
//
//  ORIGINAL_AUTHOR: Steven Wallace
//
//  This tool uses the SuperPin technology to count the number of
//       instructions executed and prints them out for the user
//       (For an overview of SuperPin see our CGO'07 paper and/or
//       the README file in this directory)
//  Sample usage:
//    pin -sp 1 -t icount1sp -- hello_static

#include <stdio.h>
#include "pin.H"
#include <iostream>

UINT64 icount = 0;
UINT64 *sharedData;

VOID docount() { 
    icount++; 
}
    
VOID ToolReset(UINT32 sliceNum)
{
    std::cerr << "Tool reset called for slice[" << sliceNum << "] ";
    icount = 0;
}

VOID Instruction(INS ins, VOID *v)
{
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);
}

VOID Merge(INT32 sliceNum, VOID *v)
{
    std::cerr << "Slice[" << sliceNum << "]Count: " << icount;
    *sharedData += icount;
    std::cerr << " Merged Count: " << *sharedData << endl;
}

VOID Fini(INT32 code, VOID *v)
{
    std::cerr << "Total Count: " << *sharedData << endl;
}

int main(int argc, char * argv[])
{
    PIN_Init(argc, argv);

    /* BEGIN SuperPin */
    SP_Init(ToolReset);
    sharedData = (UINT64 *)SP_CreateSharedArea(&icount, sizeof(icount), 0);
    SP_AddSliceEndFunction(Merge, 0);
    /* END SuperPin */

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
