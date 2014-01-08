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
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include "pin.H"

UINT64 icount = 0;

#ifdef TARGET_WINDOWS
const ADDRINT TargetPrefix =  0x1000000;
const ADDRINT SwizzlePrefix = 0x1100000;
const ADDRINT PrefixMask =    0xfff00000;
#else
const ADDRINT TargetPrefix =  0xb0000000;
const ADDRINT SwizzlePrefix = 0xb1000000;
const ADDRINT PrefixMask =    0xff000000;
#endif

LOCALFUN ADDRINT Prefix(ADDRINT val)
{
    return val & PrefixMask;
}

LOCALFUN BOOL TargetSpace(ADDRINT val)
{
    return Prefix(val) == TargetPrefix;
}

LOCALFUN BOOL SwizzleSpace(ADDRINT val)
{
    return Prefix(val) == SwizzlePrefix;
}



LOCALFUN ADDRINT Unswizzle(ADDRINT val)
{
    fprintf(stderr, "Unswizzling\n");
    fflush (stderr);

    assert(SwizzleSpace(val));
    return (val & ~ SwizzlePrefix) | TargetPrefix;

}



LOCALFUN ADDRINT Swizzle(ADDRINT val)
{
    fprintf(stderr, "Swizzling\n");
    fflush (stderr);

    assert(TargetSpace(val));

    return (val & ~ TargetPrefix) | SwizzlePrefix;
}

    

ADDRINT ProcessAddress(ADDRINT val, VOID *ip)
{
    if (TargetSpace(val))
        fprintf(stderr, "Unexpected reference to target space: %p at ip %p\n", (void*)val, ip);
    
    if (SwizzleSpace(val))
    {
        return Unswizzle (val);
    }
    return val;
}

VOID SwizzleArg(ADDRINT * arg)
{
    ASSERTX(SwizzleSpace(*arg));
    *arg = ProcessAddress(*arg, 0);
}

// When an image is loaded, check for a MyAlloc function
VOID Image(IMG img, VOID *v)
{
    //fprintf(stderr, "Loading %s\n",IMG_name(img));
    
    for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
    {
        //fprintf(stderr, "  sec %s\n", SEC_name(sec).c_str());
        for (RTN rtn = SEC_RtnHead(sec); RTN_Valid(rtn); rtn = RTN_Next(rtn))
        {
            //fprintf(stderr, "    rtn %s\n", RTN_name(rtn).c_str());
            // Swizzle the return value of MyAlloc
            
            if (RTN_Name(rtn) == "_MyAlloc" || RTN_Name(rtn) == "MyAlloc")
            {
                RTN_Open(rtn);
                
                //fprintf(stderr, "Adding Swizzle to %s\n", "MyAlloc");
				//fflush (stderr);
                RTN_InsertCall(rtn, IPOINT_AFTER, AFUNPTR(Swizzle), IARG_REG_VALUE, REG_GAX, IARG_RETURN_REGS, REG_GAX, IARG_END);
                RTN_Close(rtn);
            }

            if (RTN_Name(rtn) == "_MyFree" || RTN_Name(rtn) == "MyFree")
            {
                RTN_Open(rtn);

                //fprintf(stderr, "Adding SwizzleArg to %s\n", "MyFree");
				//fflush (stderr);
                RTN_InsertCall(rtn, IPOINT_BEFORE, AFUNPTR(SwizzleArg), IARG_FUNCARG_ENTRYPOINT_REFERENCE, 0, IARG_END);
                RTN_Close(rtn);
            }
        }
    }
}

VOID RewriteIns(INS ins)
{
    //fprintf(stderr,"Rewriting %p\n",(void*)INS_Address(ins));
    
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ, REG_INST_G0))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYREAD_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, REG_INST_G0, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_WRITE, REG_INST_G1))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYWRITE_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, REG_INST_G1, IARG_END);
    }
    if (INS_ChangeMemOpToBaseRegisterAddressMode(ins, MEMORY_TYPE_READ2, REG_INST_G2))
    {
        INS_InsertCall(ins, IPOINT_BEFORE,
                       AFUNPTR(ProcessAddress),
                       IARG_MEMORYREAD2_EA,
                       IARG_INST_PTR,
                       IARG_RETURN_REGS, REG_INST_G2, IARG_END);
    }
}

set<ADDRINT> SwizzleRefs;

BOOL SegvHandler(THREADID tid, INT32 sig, CONTEXT *ctxt, BOOL hndlr, void *)
{
    ADDRINT address = PIN_GetContextReg(ctxt, REG_INST_PTR);

    //fprintf(stderr, "Fault at %p\n",(void*)address);
    
    if (SwizzleRefs.find(address) != SwizzleRefs.end())
    {
        return true;
    }
    
    // The next time we see this address, it requires swizzling
    SwizzleRefs.insert(address);
    
    // Invalidate this instruction in code cache so it will be reinstrumented
    CODECACHE_InvalidateRange(address, address + 20);

    // returning from the signal handler will re-execute the instruction
    // this time it will be swizzled
    return false;
}

    
VOID Trace(TRACE trace, VOID *v)
{
    BOOL rewrite = false;
    
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            // If we see an instruction that needs rewriting, then rewrite all
            if (SwizzleRefs.find(INS_Address(ins)) != SwizzleRefs.end())
                rewrite = true;
        
            if (rewrite)
            {
                // If we suspect this instruction needs to be swizzled, generate safe, but slow code
                RewriteIns(ins);
            }
        }
    }
}

int main(int argc, char * argv[])
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    TRACE_AddInstrumentFunction(Trace, 0);
    IMG_AddInstrumentFunction(Image, 0);
    
    if (!PIN_AddSignalInterceptFunction(SIGSEGV, SegvHandler, 0))
	{
		fprintf (stderr, "PIN_AddSignalInterceptFunction failed\n");
		exit (1);
	}

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
