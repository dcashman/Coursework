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
// sse-unaligned-class.cpp

// This tool that provides a class that forces every SSE operation that
// reads or writes memory to work from an aligned buffer. There is one
// buffer for loads and one for stores, per thread.

// FIXME: 2007-05-09 This realigns ALL SSE operations that are not emulated
// already without checking for alignment.  I should make this check for
// misalignment.

#include <assert.h>
#include <stdio.h>
#include "pin.H"
#include "portability.H"
extern "C" {
#include "xed-interface.h"
}
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>



#if defined(__GNUC__) && !defined(_WIN32)
# include <stdint.h>
#endif

#if defined(_WIN32)
#  define uint8_t  unsigned __int8
#  define uint16_t unsigned __int16
#  define uint32_t unsigned __int32
#  define uint64_t unsigned __int64
#  define int8_t  __int8
#  define int16_t __int16
#  define int32_t __int32
#  define int64_t __int64
#  define uint_t unsigned int
#else
typedef unsigned int  uint_t;
#endif

#if defined(__GNUC__)
// NOTE: MAC XCODE2.4.1 gcc and Cgywin gcc 3.4.x only allow for 16b
// alignment!
# define SSE_ALIGN __attribute__ ((aligned(16)))
#else
# define SSE_ALIGN __declspec(align(16))
#endif

//////////////////////////////////////////////////////////////////////////
typedef uint8_t sse_aligned_buffer_t[16];

// key for accessing TLS storage in the threads. initialized once in main()


class thread_data_t
{
  public:
    thread_data_t() {}
    sse_aligned_buffer_t SSE_ALIGN read;
    sse_aligned_buffer_t SSE_ALIGN write;
};

//////////////////////////////////////////////////////////////////////////


class sse_aligner_t
{
  public:
    sse_aligner_t()
        : knob_output_file(KNOB_MODE_WRITEONCE,"pintool", "o", 
                           "sse-unaligned.out", "specify profile file name"),
          knob_pid(KNOB_MODE_WRITEONCE, "pintool", "i", 
                   "0", "append pid to output"),
          knob_stores(KNOB_MODE_WRITEONCE, "pintool", "align-sse-stores", 
                      "1", "align unaligned SSE stores"),
          knob_loads(KNOB_MODE_WRITEONCE, "pintool", "align-sse-loads", 
                     "1", "align unaligned SSE loads"),
          knob_verbose(KNOB_MODE_WRITEONCE, "pintool", "align-sse-verbose", 
                      "0", "make the sse aligner verbose")
    {
        num_threads = 0;
        active_threads = 0;
        out = 0;
        //NOTE: knob processing must happen in the activate() function.
    }

    ~sse_aligner_t()
    {
        if (out)
            out->close();
    }

    std::ofstream* out;
    KNOB<string> knob_output_file;
    KNOB<BOOL>   knob_pid;
    KNOB<BOOL>   knob_loads;
    KNOB<BOOL>   knob_stores;
    KNOB<BOOL>   knob_verbose;
    TLS_KEY tls_key;                     
    uint32_t num_threads;
    uint32_t active_threads;
    bool realign_stores;
    bool realign_loads;
    bool verbose;
    
    void activate()
    {
        //FIXME: only one aligner at a time -- not changing output file
        //name based on a static count of the number of sse_aligner_t
        //objects.

        if (knob_verbose)
        {
            string filename =  knob_output_file.Value();
            if( knob_pid )
                filename += "." + decstr( getpid_portable() );
            out = new std::ofstream(filename.c_str());
        }
        realign_stores = (knob_stores==1);
        realign_loads = (knob_loads==1);
        verbose = (knob_verbose==1);

        // obtain  a key for TLS storage
        tls_key = PIN_CreateThreadDataKey(0);

        PIN_AddThreadStartFunction(thread_start, this);
        PIN_AddThreadFiniFunction(thread_fini, this);

        TRACE_AddInstrumentFunction(instrument_trace, this);
        if (verbose)
            *out << "sse aligner activated" << endl;
    }



    thread_data_t* get_tls(THREADID tid)
    {
        thread_data_t* tdata = 
            static_cast<thread_data_t*>(PIN_GetThreadData(tls_key, tid));
        return tdata;
    }


    static void thread_start(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v)
    {
        sse_aligner_t *pthis = static_cast<sse_aligner_t *>(v);

        if (pthis->verbose)
            *(pthis->out) << "thead begin " << static_cast<uint32_t>(tid) << endl;
        // This function is locked no need for a Pin Lock here
        pthis->num_threads++;
        pthis->active_threads++;

        thread_data_t* tdata = new thread_data_t;
        // remember my pointer for later
        PIN_SetThreadData(pthis->tls_key, tdata, tid);
    }

    static void thread_fini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v)
    {
        sse_aligner_t *pthis = static_cast<sse_aligner_t *>(v);

        // This function is locked no need for a Pin Lock here
        pthis->active_threads--;
    }



    static void rewrite_instruction(INS ins, bool is_read, sse_aligner_t* pthis) 
    {
        // Avoid aligning trivially aligned stuff
        const xed_decoded_inst_t* xedd = INS_XedDec(ins);
        if (xed_decoded_inst_get_memory_operand_length(xedd,0) 
              > sizeof (sse_aligned_buffer_t))
        {
            return;
        }

        //fprintf(stderr,"Rewriting %p\n",(void*)INS_Address(ins));

        if (pthis->verbose)
            *(pthis->out) << "REWRITE: "
                          << std::setw(16)
                          << std::hex 
                          << INS_Address(ins) 
                          << std::dec 
                          << " "
                          << INS_Disassemble(ins) << std::endl;
    
        
        // Loads -- we change the load to use G0 as the base register and
        // then add a "before" function that sets G0 and copies the data to
        // an aligned bufffer.
        if (is_read && 
            INS_RewriteMemoryAddressingToBaseRegisterOnly(ins, MEMORY_TYPE_READ, REG_INST_G0))
        {
            if (pthis->verbose)
                *(pthis->out) << "REWRITING LOAD" << std::endl;
            INS_InsertCall(ins, IPOINT_BEFORE,
                           AFUNPTR(copy_to_aligned_load_buffer_and_return_pointer),
                           IARG_MEMORYREAD_EA,
                           IARG_MEMORYREAD_SIZE,
                           IARG_INST_PTR,
                           IARG_THREAD_ID,
                           IARG_PTR, pthis,
                           IARG_RETURN_REGS, REG_INST_G0, 
                           IARG_END);
        }

        // Stores -- we change the store to use G1 as a base register and
        // then add a "before" function to set G1 and an "after" function
        // that copies the data from the aligned buffer to where it was
        // supposed to go.
        if (!is_read && 
            INS_RewriteMemoryAddressingToBaseRegisterOnly(ins, MEMORY_TYPE_WRITE, REG_INST_G1))
        {
            if (pthis->verbose)
                *(pthis->out) << "REWRITING STORE" << std::endl;
            INS_InsertCall(ins, IPOINT_BEFORE,
                           AFUNPTR(return_pointer_to_aligned_store_buffer),
                           IARG_MEMORYWRITE_EA,
                           IARG_INST_PTR,
                           IARG_THREAD_ID,
                           IARG_PTR, pthis,
                           IARG_RETURN_REGS, REG_INST_G1,
                           IARG_END);
            INS_InsertCall(ins, IPOINT_AFTER,
                           AFUNPTR(copy_from_aligned_store_buffer),
                           IARG_MEMORYWRITE_EA,
                           IARG_MEMORYWRITE_SIZE,
                           IARG_INST_PTR,
                           IARG_THREAD_ID,
                           IARG_PTR, pthis,
                           IARG_END);
        }
    }


    
    static bool check_for_sse_memop(INS ins, bool& is_read, sse_aligner_t* pthis) 
    {
        // return true if the instruction is SSEx and reads/writes memory
        xed_extension_enum_t extension = static_cast<xed_extension_enum_t>(INS_Extension(ins));
        if (extension == XED_EXTENSION_SSE ||
            extension == XED_EXTENSION_SSE2 ||
            extension == XED_EXTENSION_SSE3 ||
            extension == XED_EXTENSION_SSSE3 ||
            extension == XED_EXTENSION_SSE4)
        {
            if (pthis->realign_loads && INS_IsMemoryRead(ins))
            {
                is_read = true;
                return true;
            }
            if (pthis->realign_stores && INS_IsMemoryWrite(ins)) 
            {
                is_read = false;
                return true;
            }
        }
        return false;
    }

    static void instrument_trace(TRACE trace, VOID *v)
    {
        sse_aligner_t* pthis = static_cast<sse_aligner_t*>(v);
        bool is_read = false;
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
            for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
                if (check_for_sse_memop(ins, is_read, pthis))
                    rewrite_instruction(ins, is_read, pthis);
    }




    static ADDRINT copy_to_aligned_load_buffer_and_return_pointer(ADDRINT load_addr, 
                                                                  ADDRINT byte_len,
                                                                  ADDRINT ip,
                                                                  THREADID tid,
                                                                  sse_aligner_t* pthis)
    {
        // return the address to use for the SSEx operation
        thread_data_t* tdata = pthis->get_tls(tid);
        memcpy(tdata->read, 
               reinterpret_cast<uint8_t*>(load_addr), 
               byte_len);
        return reinterpret_cast<ADDRINT>(tdata->read);
    }
    static ADDRINT return_pointer_to_aligned_store_buffer(ADDRINT store_addr, 
                                                          ADDRINT ip,
                                                          THREADID tid,
                                                          sse_aligner_t* pthis)
    {
        // return the address to use for the SSEx operation
        thread_data_t* tdata = pthis->get_tls(tid);
        return reinterpret_cast<ADDRINT>(tdata->write);
    }

    static void copy_from_aligned_store_buffer(ADDRINT store_addr, 
                                               ADDRINT byte_len,
                                               ADDRINT ip,
                                               THREADID tid,
                                               sse_aligner_t* pthis)
    {
        thread_data_t* tdata = pthis->get_tls(tid);
        memcpy(reinterpret_cast<uint8_t*>(store_addr), 
               tdata->write, 
               byte_len);
    }

}; // class


int  usage()
{
    cerr << "Usage: ..." << endl;
    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return 1;
}

int main(int argc, char * argv[])
{
    static sse_aligner_t  aligner; // must be before usage...

    PIN_InitSymbols();
    if (PIN_Init(argc, argv))
        return usage();

    aligner.activate();

    // Never returns
    PIN_StartProgram();
    
    return 0;
}