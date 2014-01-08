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
 * See "swizzleapp.c" for a description of this test.
 */

#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <set>
#include "pin.H"

#define MASK 0xC0000000
#define TAG  0xC0000000


static VOID InstrumentImage(IMG, VOID *);
static VOID Swizzle(ADDRINT *);
static VOID InstrumentTrace(TRACE, VOID *);
static VOID RewriteIns(INS);
static ADDRINT Unswizzle(ADDRINT);
static BOOL SegvHandler(THREADID, INT32, CONTEXT *, BOOL hndlr, void *);


set<ADDRINT> SwizzleRefs;

int main(int argc, char * argv[])
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    IMG_AddInstrumentFunction(InstrumentImage, 0);
    TRACE_AddInstrumentFunction(InstrumentTrace, 0);
    PIN_AddSignalInterceptFunction(SIGSEGV, SegvHandler, 0);

    // Never returns
    PIN_StartProgram();
    return 0;
}


static VOID InstrumentImage(IMG img, VOID *v)
{
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            if (RTN_Name(rtn) == "Allocate")
            {
                RTN_Open(rtn);
                RTN_InsertCall(rtn, IPOINT_AFTER, AFUNPTR(Swizzle),
                    IARG_FUNCRET_EXITPOINT_REFERENCE, IARG_END);
                RTN_Close(rtn);
            }
        }
    }
}


static VOID Swizzle(ADDRINT *val)
{
    if ((*val & MASK) != 0)
        cerr << "Invalid test" << endl;
    *val |= TAG;
}


static VOID InstrumentTrace(TRACE trace, VOID *v)
{
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            // If we see an instruction that needs rewriting, then rewrite all
            if (SwizzleRefs.find(INS_Address(ins)) != SwizzleRefs.end())
            {
                // If we suspect this instruction needs to be swizzled, generate safe, but slow code
                RewriteIns(ins);
            }
        }
    }
}


static VOID RewriteIns(INS ins)
{
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ, REG_INST_G0))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Unswizzle),
            IARG_MEMORYREAD_EA, IARG_RETURN_REGS, REG_INST_G0, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_WRITE, REG_INST_G1))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Unswizzle),
            IARG_MEMORYWRITE_EA, IARG_RETURN_REGS, REG_INST_G1, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ2, REG_INST_G2))
    {
        INS_InsertCall(ins, IPOINT_BEFORE, AFUNPTR(Unswizzle),
            IARG_MEMORYREAD2_EA, IARG_RETURN_REGS, REG_INST_G2, IARG_END);
    }
}


static ADDRINT Unswizzle(ADDRINT val)
{
    if ((val & MASK) == TAG)
        return val & ~MASK;
    return val;
}


static BOOL SegvHandler(THREADID tid, INT32 sig, CONTEXT *ctxt, BOOL hndlr, void *v)
{
    ADDRINT address = PIN_GetContextReg(ctxt, REG_INST_PTR);

    if (SwizzleRefs.find(address) != SwizzleRefs.end())
    {
        cerr << "Multiple faults at same instruction" << endl;
        exit(1);
    }

    SwizzleRefs.insert(address);
    CODECACHE_InvalidateRange(address, address);

    return false;
}
