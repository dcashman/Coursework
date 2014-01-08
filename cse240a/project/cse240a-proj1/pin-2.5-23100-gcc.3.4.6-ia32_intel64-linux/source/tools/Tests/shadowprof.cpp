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
// Shadow Profiler
//   Original Author:  Tipp J. Moseley
//   Ported to SuperPin by Steven Wallace

#include <map>
#include <iostream>

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <syscall.h>
#include <unistd.h>

//#include "tassert.h"
#include "pagetable.H"
//#include "path_profiler.H"
#include "pin.H"
//#include "shadow.H"
#include "portability.H"

using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Knobs ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

KNOB<double> KnobNumShadows(KNOB_MODE_WRITEONCE,
			    "pintool",
			    "load",
			    "1.0",
			    "number of active shadows");

static KNOB<string> KnobProfiler(KNOB_MODE_WRITEONCE, "pintool",
				 "prof", "", "profiler: path|val");

KNOB<UINT32> KnobSampleSize(KNOB_MODE_WRITEONCE, "pintool", "sample-size",
			    "10000000",
			    "number of BBLs for each shadow to run");

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Global Variables ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

typedef PageTable<bool, 16, 16, 1> BBLTABLE;
typedef BBLTABLE::Page_T BBLPAGE;

BBLTABLE BblTable;

static map<uint64_t, vector<uintptr_t> > Paths;
static map<uint64_t, uint32_t> PathCounts;

static const uint32_t BufferBits = 12;
static const uint32_t BufferSize = 1<<BufferBits;
static ADDRINT Buffer[BufferSize];
static uint32_t BufferIndex = 0;

static uintptr_t Path[4096];
static uint32_t PathIndex = 0;

UINT64 *sharedData;
UINT32 InsCount;
UINT32 InsSampleSize;
UINT32 ParentPid;

LOCALCONST UINT64 sliceLimit = 100000000;


///////////////////////////////////////////////////////////////////////////////
///////////////////////// Analysis ////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void
AddPath()
{
  uint64_t hash = PathIndex ^ ((uint64_t)PathIndex << 32);

  for (uint32_t i = 0; i < PathIndex; i++) {
    hash += Path[i] + (Path[i] << (16 * (i % 4)));

    BBLPAGE *page = BblTable.get(Path[i]);
    page->data[BBLPAGE::getOffset(Path[i])] = false;			
  }
  
  map<uint64_t, uint32_t>::iterator cIt;
  if ((cIt = PathCounts.find(hash)) == PathCounts.end()) {
    vector<uintptr_t> vec;
    for (uint32_t i = 0; i < PathIndex; i++) {
      vec.push_back(Path[i]);
    }
    Paths[hash] = vec;
    PathCounts[hash] = 1;
  } else {
    cIt->second++;
  }
}

static ADDRINT
A_Bbl_if(ADDRINT pc, UINT32 numIns)
{
  InsCount += numIns;
  Buffer[BufferIndex++] = pc;
  return BufferIndex >> BufferBits;
}

static void
A_Bbl_then()
{
  for (uint32_t i = 0; i < BufferIndex; i++) {
    ADDRINT pc = Buffer[i];
    BBLPAGE *page = BblTable.get(pc);

    bool isOnPath = page->data[BBLPAGE::getOffset(pc)];

    page->data[BBLPAGE::getOffset(pc)] = true;
    Path[PathIndex++] = pc;
    if (isOnPath) {
      AddPath();
      PathIndex = 0;
    }
  }
  BufferIndex = 0;

  //cout << "Dump Done\n";
  if (InsCount > InsSampleSize) {
    //cout << "Exiting!\n";
    BufferIndex = 0;
    //    ShadowFini(0, 0);
    // end slice prematurely
    SP_EndSlice();
  }
}

VOID ToolReset(UINT32 sliceNum)
{
    InsCount = 0;
    // data = 0;
}

VOID Fini(INT32 code, VOID *v)
{
    std::cerr << "Total Count: " << *sharedData << endl;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Fini Function ///////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static bool Finished = false;
VOID PATH_ProfilerMerge(INT32 sliceNum, VOID *v)
{
    std::cerr << "Slice[" << sliceNum << "]Count: " << InsCount;
    *sharedData += InsCount;
    std::cerr << " Merged Count: " << *sharedData << endl;

    //cout << "PROFILER: FINI\n";
    if (BufferIndex) {
        A_Bbl_then();
    }
  
    if (Finished) {
        return;
    }
    Finished = true;
    char buf[1024];

    sprintf(buf, "pathprof.out.%d.%d", ParentPid, getpid_portable());
    ofstream outfile(buf);
    
    map<uint64_t, uint32_t>::iterator pIt;
    
    for (pIt = PathCounts.begin(); pIt != PathCounts.end(); pIt++) {
        uint64_t hash = pIt->first;
        if (Paths.find(hash) != Paths.end()) {
            outfile << pIt->second;
            outfile << hex << ", " << hash;
            vector<uintptr_t>::iterator bIt;
            for (bIt = Paths[hash].begin(); bIt != Paths[hash].end(); bIt++) {
                outfile << ", " << *bIt;
            }
            outfile << dec;
            outfile << endl;
        }
    }
    //cout << "PROFILER: FINI FINI\n";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////// Instrumentation /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void
I_Trace(TRACE trace, void *v)
{
    //    printf("trace %x %d\n", TRACE_Address(trace), TRACE_Size(trace));
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        BBL_InsertIfCall(bbl, IPOINT_BEFORE,
                         AFUNPTR(A_Bbl_if),
                         IARG_ADDRINT, BBL_Address(bbl),
                         IARG_UINT32, BBL_NumIns(bbl),
                         IARG_END);
        
        BBL_InsertThenCall(bbl, IPOINT_BEFORE,
                           AFUNPTR(A_Bbl_then),
                           IARG_END);
    }
    //printf("trace done\n");
}


int main(INT32 argc, CHAR **argv)
{
    PIN_Init(argc, argv);

    InsSampleSize  = KnobSampleSize.Value();
    ParentPid = getpid_portable();

    /* SuperPin */
    SP_Init(ToolReset);
    sharedData = (UINT64 *)SP_CreateSharedArea(&InsCount, sizeof(InsCount), 0);
    SP_AddSliceEndFunction(PATH_ProfilerMerge, 0);
    /* END SuperPin */

    // Initialize the dummy page
    BBLPAGE *dummy = BblTable.getFast(0);
    for (uint32_t i = 0; i < BBLPAGE::PAGE_SIZE; i++)
    {
        dummy->data[BBLPAGE::getOffset(i)] = true;
    }

    TRACE_AddInstrumentFunction(I_Trace, 0);

    PIN_AddFiniFunction(Fini, 0);

    // Never returns
    PIN_StartProgram();
    
    return 0;
}
