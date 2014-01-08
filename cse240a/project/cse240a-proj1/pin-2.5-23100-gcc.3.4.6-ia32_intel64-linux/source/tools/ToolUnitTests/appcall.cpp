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
  Test calling an application function from instrumentation.  At the entry
  point to main, we insert a call to an analysis function that calls main
  natively.  The output should show that main was called twice
*/
#include <iostream>
#include "pin.H"

using namespace std;

#if defined(TARGET_MAC)
#define MAINNAME "_main"
#else
#define MAINNAME "main"
#endif

AFUNPTR mainFun = 0;

VOID callmain(INT32 code, VOID *)
{
    mainFun();
}

VOID ImageLoad(IMG img, VOID * v)
{
    IMG_TYPE imgType = IMG_Type(img);
    if(imgType ==  IMG_TYPE_STATIC || imgType == IMG_TYPE_SHARED)
    {
        RTN mainRtn = RTN_FindByName(img, MAINNAME);
        if (RTN_Valid(mainRtn))
        {
            mainFun = RTN_Funptr(mainRtn);

            RTN_Open(mainRtn);
            RTN_InsertCall(mainRtn, IPOINT_BEFORE, AFUNPTR(callmain), IARG_END);
            RTN_Close(mainRtn);
        }
    }
}

int main(INT32 argc, CHAR **argv)
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    
    IMG_AddInstrumentFunction(ImageLoad, 0);
    
    // Never returns
    PIN_StartProgram();
    
    return 0;
}
