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
namespace INSTLIB 
{

/*! @defgroup ICOUNT
  Instrumentation for counting instruction execution
*/

/*! @ingroup ICOUNT
  The example below can be found in InstLibExamples/icount.cpp

  \include icount.cpp
*/
class ICOUNT 
{
  public:
    ICOUNT()
    {
        _maxThreads = ISIMPOINT_MAX_THREADS; 
        _active = FALSE;
        _counts = new UINT64[_maxThreads];
        memset(_counts, 0, sizeof(_counts[0]) * _maxThreads);
        _counts_withREP = new UINT64[_maxThreads];
        memset(_counts_withREP, 0, sizeof(_counts_withREP[0]) * _maxThreads);
    };
    
    /*! @ingroup ICOUNT
      @return Number of instructions executed
    */
    UINT64 Count(THREADID tid = 0) const
    {
        ASSERTX(tid < _maxThreads);
        return _counts[tid];
    }
    
    /*! @ingroup ICOUNT
      @return Number of instructions executed with REP repitions
    */
    UINT64 CountWithREP(THREADID tid = 0) const
    {
        ASSERTX(tid < _maxThreads);
        return _counts_withREP[tid] + _counts[tid];
    }

    /*! @ingroup ICOUNT
      Set the current count
    */
    VOID SetCount(UINT64 count, THREADID tid = 0)
    {
        ASSERTX(_active == TRUE);
        ASSERTX(tid < _maxThreads);
        _counts[tid] = count;
    }

    /*! @ingroup ICOUNT
      Activate the counter, must be called before PIN_StartProgram
    */
    VOID Activate()
    {
        ASSERTX(_active == FALSE);
        _active = TRUE;
        TRACE_AddInstrumentFunction(Trace, this);
    }

        

  private:
    static VOID Trace(TRACE trace, VOID * icount)
    {
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            BBL_InsertCall(bbl, IPOINT_ANYWHERE,
                           AFUNPTR(Advance),
                           IARG_FAST_ANALYSIS_CALL,
                           IARG_ADDRINT, icount, IARG_ADDRINT, ADDRINT(BBL_NumIns(bbl)), IARG_THREAD_ID, IARG_END);
            for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
            {
            // Count the number of times REP-prefixed instructions are executed.
            // We assume that these instructions always refer memory.
            // The count computation is done by dividing  the size of the 
            // total memory accessed ( obtained dynamically via
            // IARG_MEMORYWRITE(READ)_SIZE) by the size of the memory
            // accessed by the un-prefixed instruction.
                if( INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins) )
                {
                    BOOL write = INS_IsMemoryWrite(ins);
                    UINT32 size = write ? INS_MemoryWriteSize(ins) : INS_MemoryReadSize(ins);

                    if( size == VARIABLE_MEMORY_REFERENCE_SIZE )
                    {
                        UINT32 prim_size = write ? INS_MemoryWriteSizeWithoutPrefix(ins) : INS_MemoryReadSizeWithoutPrefix(ins);
                        if(write)
                            INS_InsertCall(ins,
                                           IPOINT_AFTER,
                                           AFUNPTR(Advance_withREP),
                                           IARG_FAST_ANALYSIS_CALL,
                                           IARG_ADDRINT, icount,
                                           IARG_INST_PTR,
                                           IARG_UINT32, prim_size,
                                           IARG_MEMORYWRITE_SIZE,
                                           IARG_THREAD_ID, 
                                           IARG_END);
                        else
                            INS_InsertCall(ins,
                                           IPOINT_AFTER,
                                           AFUNPTR(Advance_withREP),
                                           IARG_FAST_ANALYSIS_CALL,
                                           IARG_ADDRINT, icount,
                                           IARG_INST_PTR,
                                           IARG_UINT32, prim_size,
                                           IARG_MEMORYREAD_SIZE,
                                           IARG_THREAD_ID, 
                                           IARG_END);
                    }
                }
            }
        }
    }

    static VOID PIN_FAST_ANALYSIS_CALL Advance(ICOUNT * ic, ADDRINT c, THREADID tid)
    {
        // ASSERTX(tid < _maxThreads);
        ic->_counts[tid] += c;
    }

    static VOID PIN_FAST_ANALYSIS_CALL Advance_withREP(ICOUNT * ic, VOID * ip, UINT32 prim_size, UINT32 size, THREADID tid)
    {
        UINT32 count = size/prim_size;
        if(count)
        {
            ic->_counts_withREP[tid] += (count -1); 
            // -1 because Advance counted this once
        }
        else
        {
            // The repeat count is ZERO.
            // Should we subtract one to undo Advance()'s counting of
            // this instruction? 
        }
    }
    
    BOOL _active;
    UINT64 *_counts;
    UINT64 *_counts_withREP;
    UINT64 _maxThreads;
};


}
