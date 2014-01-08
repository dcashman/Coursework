/*BEGIN_LEGAL 
Intel Open Source License 

Copyright (c) 2002-2007 Intel Corporation 
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
 *  This file contains an ISA-portable PIN tool for tracing memory accesses.
 */

#include <stdio.h>
#include "pin.H"
#include "instlib.H"


FILE * trace;

INSTLIB::ICOUNT icount;
UINT32 icount_lastmem;
UINT32 sincelast;

UINT32 memrefs; // number of memory accesses so far
#define MAX_REFS 2000000 // we want to limit the size of the log files so we will limit trace files to 2M lines

INSTLIB::CONTROL control;
bool doinst;

// update lastmem info and write to file (load)
VOID RecordMemRead(VOID * ip, VOID * addr)
{
	if(icount.CountWithREP() == icount_lastmem) sincelast = 0;
	else {
		sincelast = icount.CountWithREP() - icount_lastmem - 1;
		icount_lastmem = icount.CountWithREP();
	}

    fprintf(trace,"l %p %p %u\n", ip, addr, sincelast);

	++memrefs;

	if(memrefs > MAX_REFS) {
		cout << "Maximum number of memops reached. Stopping instrumentation... " << endl;
    	fclose(trace);
		PIN_Detach();
	}
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr)
{
	if(icount.CountWithREP() == icount_lastmem) sincelast = 0;
	else {
		sincelast = icount.CountWithREP() - icount_lastmem - 1;
		icount_lastmem = icount.CountWithREP();
	}

    fprintf(trace,"s %p %p %u\n", ip, addr, sincelast);

	++memrefs;

	if(memrefs > MAX_REFS) {
		cout << "Maximum number of memops reached. Stopping instrumentation... " << endl;
    	fclose(trace);
		PIN_Detach();
	}
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
	if(doinst) {

		// instruments loads using a predicated call, i.e.
		// the call happens iff the load will be actually executed
		// (this does not matter for ia32 but arm and ipf have predicated instructions)
		if (INS_IsMemoryRead(ins))
		{
			INS_InsertPredicatedCall(
				ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
				IARG_INST_PTR,
				IARG_MEMORYREAD_EA,
				IARG_END);
		}

		// instruments stores using a predicated call, i.e.
		// the call happens iff the store will be actually executed
		if (INS_IsMemoryWrite(ins))
		{
			INS_InsertPredicatedCall(
				ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
				IARG_INST_PTR,
				IARG_MEMORYWRITE_EA,
				IARG_END);
		}
	}
}

VOID Handler(INSTLIB::CONTROL_EVENT ev, VOID * v, CONTEXT * ctxt, VOID * ip, THREADID tid)
{
    switch(ev)
    {
      case INSTLIB::CONTROL_START:
        cout << "Starting instrumentation..." << endl;
		doinst = true;
		icount_lastmem = icount.CountWithREP();
        break;

      case INSTLIB::CONTROL_STOP:
        cout << "Stopping instrumentation..." << endl;
		doinst = false;
        break;

      default:
        ASSERTX(false);
        break;
    }
}

VOID Fini(INT32 code, VOID *v)
{
    fclose(trace);
}


int main(int argc, char *argv[])
{
    PIN_Init(argc, argv);

    trace = fopen("mem.trace", "w");

    // Activate instruction counter
    icount.Activate();

	icount_lastmem = 1;

	memrefs = 1;

    // Activate alarm, must be done before PIN_StartProgram
    control.CheckKnobs(Handler, 0);

	doinst = false;

    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
