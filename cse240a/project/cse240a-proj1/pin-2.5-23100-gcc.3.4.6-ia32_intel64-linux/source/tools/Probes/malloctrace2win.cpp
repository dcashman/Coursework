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

/* ===================================================================== */
/*
  @ORIGINAL_AUTHOR: Robert Cohn
  @MAJOR_REWRITE: Gail Lyons
*/

/* ===================================================================== */
/*! @file
  Similar to Probes/malloctrace.C, but puts replacement functions in the
  application namespace. Works in probed and jitted mode.
 */

#include "pin.H"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
namespace WIND
{
    #include <windows.h>
}

using namespace std;


/* ===================================================================== */
/* Commandline Switches */
/* ===================================================================== */

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "malloctrace2.outfile", "specify trace file name");

/* ===================================================================== */

INT32 Usage()
{
    cerr <<
        "This pin tool inserts a user-written version of malloc() and free() into the application.\n"
        "\n";
    cerr << KNOB_BASE::StringKnobSummary();
    cerr << endl;
    cerr.flush();
    return -1;
}

/* ===================================================================== */
/* Global Variables */
/* ===================================================================== */

ofstream TraceFile;

static void * ( *fp_mallocWrapper )(size_t);
static void ( *fp_freeWrapper )(void *);
static WIND::HMODULE (APIENTRY *fp_loadLibrary )(WIND::LPCTSTR);

static WIND::FARPROC (APIENTRY *fp_getProcAddress )( WIND::HMODULE, WIND::LPCTSTR );

static WIND::HMODULE mallocTraceHandle = 0;


RTN freeRtn;
RTN mallocRtn;

ADDRINT mainImgEntry = 0;

BOOL replaced = FALSE;
BOOL kernel_found = FALSE;
BOOL crtl_found = FALSE;

/* ===================================================================== */

// Look for routines that we want to probe

VOID ReplaceRtns()
{
    // inject mallocwrappers.dll into application by executing application
    // LoadLibrary.
    mallocTraceHandle = (*fp_loadLibrary)(TEXT("mallocwrapperswin.dll"));
    ASSERTX(mallocTraceHandle);
    
    // Get function pointers for the wrappers
    fp_mallocWrapper = (void * (*)(size_t))
                       (*fp_getProcAddress)(mallocTraceHandle, TEXT("mallocWrapper"));
    fp_freeWrapper = (void (*)(void *))
                     (*fp_getProcAddress)(mallocTraceHandle, TEXT("freeWrapper"));
    ASSERTX(fp_mallocWrapper && fp_freeWrapper);
    
    AFUNPTR mallocimpl =
        PIN_IsProbeMode() ?
        RTN_ReplaceProbed(mallocRtn, AFUNPTR(fp_mallocWrapper)) :
        RTN_Replace(mallocRtn, AFUNPTR(fp_mallocWrapper));
    
    
    // tell mallocwrappers.dll how to call original code
    ASSERTX(mallocTraceHandle);
    AFUNPTR * mallocptr = (AFUNPTR *)(*fp_getProcAddress)(mallocTraceHandle, 
                                                          TEXT("fp_mallocFun"));
    ASSERTX(mallocptr);
    *mallocptr = mallocimpl;
    
    AFUNPTR freeimpl =
        PIN_IsProbeMode() ?
        RTN_ReplaceProbed(freeRtn, AFUNPTR(fp_freeWrapper)) :
        RTN_Replace(freeRtn, AFUNPTR(fp_freeWrapper));
    
    AFUNPTR * freeptr = (AFUNPTR *)(*fp_getProcAddress)(mallocTraceHandle, 
                                                        TEXT("fp_freeFun"));
    ASSERTX(freeptr);
    *freeptr = freeimpl;
}

/* ===================================================================== */
// Called every time a new image is loaded
// Look for routines that we want to probe
//
VOID ImageLoad(IMG img, VOID *v)
{
    TraceFile << "Processing " << IMG_Name(img) << endl;
    TraceFile.flush();
    
    if ( strstr(IMG_Name(img).c_str(), "kernel32")) 
    {
        TraceFile << "Found " << IMG_Name(img) << endl;
        TraceFile.flush();

        kernel_found = TRUE;
        
        // Get the function pointer for the application LoadLibrary().
        RTN llRtn = RTN_FindByName(img, "LoadLibraryA");
        ASSERTX(RTN_Valid(llRtn));
        fp_loadLibrary = (WIND::HMODULE (APIENTRY *)(WIND::LPCTSTR))(RTN_Funptr(llRtn));
        
        // Get the function pointer for the application GetProcAddress().
        RTN gpaRtn = RTN_FindByName(img, "GetProcAddress");
        ASSERTX(RTN_Valid(gpaRtn));
        fp_getProcAddress = (WIND::FARPROC (APIENTRY *)(WIND::HMODULE, WIND::LPCTSTR))(RTN_Funptr(gpaRtn));
    }

    if ( strstr(IMG_Name(img).c_str(), "MSVCR80" ) || strstr(IMG_Name(img).c_str(), "MSVCR90" )) 
    {
        TraceFile << "Found " << IMG_Name(img) << endl;
        TraceFile.flush();
        
        crtl_found = TRUE;
        
        // Replace malloc and free in application libc with wrappers 
        // in mallocwrappers.dll.
        mallocRtn = RTN_FindByName(img, "malloc");
        ASSERTX(RTN_Valid(mallocRtn));
        
        freeRtn = RTN_FindByName(img, "free");
        ASSERTX(RTN_Valid(freeRtn));
    }

    if ( ! replaced && kernel_found && crtl_found ) 
    {
        replaced = TRUE;
        ReplaceRtns();
    }
}

/* ===================================================================== */

int main(int argc, CHAR *argv[])
{
    PIN_InitSymbols();
    
    if( PIN_Init(argc,argv) )
    {
        return Usage();
    }
    
    TraceFile.open(KnobOutputFile.Value().c_str());
    TraceFile << hex;
    TraceFile.setf(ios::showbase);
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    if (PIN_IsProbeMode()) {
        PIN_StartProgramProbed();
    } else {
        PIN_StartProgram();
    }
    
    return 0;
}

/* ===================================================================== */
/* eof */
/* ===================================================================== */
