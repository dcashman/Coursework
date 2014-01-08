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
 *  Record the memory read and written by variable size instructions
 */

#include <stdio.h>
#include "pin.H"

UINT8 oldVal = 0;
UINT8 * ea;

// Change the first byte of string
VOID Change(UINT8 * addr)
{
    ea = addr;
    
    oldVal = *ea;
    *ea = '1';
}

// Restore first byte of string
VOID Restore()
{
    *ea = oldVal;
}

VOID RecordMemory(VOID * ip, ADDRINT testNum, char * type, UINT8 * addr, INT32 size)
{
    ea = addr;
    
    printf("Test %d ", testNum);
    printf("%s size %d: ",type,size);
    for (INT32 i = 0; i < size; i++)
    {
        printf("%c", addr[i]);
    }
    
    printf("\n");
}

// Is called for every instruction and instruments reads and writes
VOID Trace(TRACE trace, VOID *v)
{
    // Ignore everything that is not in the main function
    RTN rtn = TRACE_Rtn(trace);
    if (!RTN_Valid(rtn))
        return;
    
    if (RTN_Name(rtn) != "main")
        return;
    
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            
            if (INS_IsMemoryRead(ins)
                && VARIABLE_MEMORY_REFERENCE_SIZE == INS_MemoryReadSize(ins))
            {
                INS_InsertCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)Change,
                    IARG_MEMORYREAD_EA,
                    IARG_END);

                INS_InsertCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemory,
                    IARG_INST_PTR,
                    IARG_REG_VALUE,
                    REG_EDX,
                    IARG_PTR,
                    "read1",
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_END);
                
                INS_InsertCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)Restore,
                    IARG_END);

                INS_InsertCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemory,
                    IARG_INST_PTR,
                    IARG_REG_VALUE,
                    REG_EDX,
                    IARG_PTR,
                    "read1",
                    IARG_MEMORYREAD_EA,
                    IARG_MEMORYREAD_SIZE,
                    IARG_END);
            }

            if (INS_HasMemoryRead2(ins)
                && VARIABLE_MEMORY_REFERENCE_SIZE == INS_MemoryReadSize(ins))
            {
                INS_InsertCall(
                    ins, IPOINT_BEFORE, (AFUNPTR)RecordMemory,
                    IARG_INST_PTR,
                    IARG_REG_VALUE,
                    REG_EDX,
                    IARG_PTR,
                    "read2",
                    IARG_MEMORYREAD2_EA,
                    IARG_MEMORYREAD_SIZE,
                    // IARG_CONTEXT requires a bridge and tests liveness of
                    // helper virtual registers
                    IARG_CONTEXT,
                    IARG_END);
            }

            if (INS_IsMemoryWrite(ins)
                && VARIABLE_MEMORY_REFERENCE_SIZE == INS_MemoryWriteSize(ins))
            {
                INS_InsertCall(
                    ins, IPOINT_AFTER, (AFUNPTR)RecordMemory,
                    IARG_INST_PTR,
                    IARG_REG_VALUE,
                    REG_EDX,
                    IARG_PTR,
                    "write",
                    IARG_MEMORYWRITE_EA,
                    IARG_MEMORYWRITE_SIZE,
                    IARG_END);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);
    PIN_InitSymbols();

    TRACE_AddInstrumentFunction(Trace, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
