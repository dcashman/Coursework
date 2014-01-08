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
#include <vector>

#define ATOMIC_DEC_W(loc)                         \
    __asm__ __volatile__ ("lock decl (%0)\n\t"    \
                          :                       \
                          : "r"(loc) : "memory"   \
    );

/*! @defgroup MT_ALARM_ICOUNT
  @ingroup ALARM
  Signal an alarm when the specified number of instructions has been executed

  The example below can be found in InstLibExamples/alarm_icount.cpp

  \include alarm_icount.cpp

*/

/*! @ingroup MT_ALARM_ICOUNT
*/
class MT_ALARM_ICOUNT
{
  public:
    MT_ALARM_ICOUNT(BOOL passContext=FALSE, THREADID tid=0, UINT64 max_threads=PIN_MAX_THREADS)
        :
          _active(FALSE),
          _passContext(passContext),
          _maxThreads(max_threads)
    {
        _count = new INT64[_maxThreads];
        _orig_count = new INT64[_maxThreads];
        _armed = new BOOL[_maxThreads];
        _rearm = new BOOL[_maxThreads];
        _always_armed = new BOOL[_maxThreads];
        _tid = new BOOL[_maxThreads];
        _handler = new ALARM_HANDLER[_maxThreads];
        for ( UINT32 i = 0; i < _maxThreads; i++ )
        {
            _count[i] = 0;
            _tid[i] = false;
            _armed[i] = false;
        }
    }
    virtual ~MT_ALARM_ICOUNT()
    {
    }
    
    /*! @ingroup MT_ALARM_ICOUNT
      @return Remaining count for this alarm
    */
    INT64 Count(THREADID tid=0) const
    {
        ASSERTX(_active == TRUE);
        return _count[tid];
    }

    /*! @ingroup MT_ALARM_ICOUNT
      Turn on an alarm. You should call Activate for this alarm before PIN_StartProgram
      @param count Number of times to execution address before alarm fires
      @param handler Call this function when alarm fires
      @param val Pass this value to the handler when the alarm fires
    */
    VOID SetAlarm(INT64 count, ALARM_HANDLER handler, VOID * val, THREADID tid=0, BOOL rearm=FALSE, BOOL always_armed=FALSE)
    {
        ASSERTX(_active == TRUE);
        if ( tid != ALL_THREADS )
        {
            _count[tid] = count;
            _tid[tid] = true;
            _orig_count[tid] = count;
            _armed[tid] = TRUE;
            _rearm[tid] = rearm;
            _always_armed[tid] = always_armed;
            _handler[tid] = handler;            
        }
        else
        {
            for ( UINT32 i = 0; i < _maxThreads; i++ )
            {
                _count[i] = count;
                _tid[i] = true;
                _orig_count[i] = count;
                _armed[i] = TRUE;
                _rearm[i] = rearm;
                _always_armed[i] = always_armed;
                _handler[i] = handler;
            }
        }
        _val = val;
    }

    /*! @ingroup MT_ALARM_ICOUNT
      This function initializes an alarm and must be called before @ref PIN_StartProgram.
      It does turn the alarm on. Use SetAlarm.
    */
    VOID Activate()
    {
        ASSERTX(_active == FALSE);
        _active = TRUE;
        TRACE_AddInstrumentFunction(Trace, this);
    }

    /*! @ingroup MT_ALARM_ICOUNT
      This function re-enables the alarm following a DeActivate(); 
      must have called Activate() before.
    */
    VOID ReActivate()
    {
        _active = TRUE;
    }

    /*! @ingroup MT_ALARM_ICOUNT
      De-activate the counter, should call PIN_RemoveInstrumentation()
      after de-activating
    */
    VOID DeActivate()
    {
        _active = FALSE;
    }
        
    /*! @ingroup MT_ALARM_ICOUNT
      Is this MT_ALARM_ICOUNT active?
    */
    BOOL Active() const { return _active; }

  private:
    static VOID Trace(TRACE trace, VOID * icount)
    {
        MT_ALARM_ICOUNT * ic = static_cast<MT_ALARM_ICOUNT *>(icount);
        if (!ic->_active) return;
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            if (ic->_passContext)
            {
                INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE,
                               AFUNPTR(Advance),
                               IARG_ADDRINT, icount, 
                               IARG_UINT32, BBL_NumIns(bbl), 
                               IARG_CONTEXT, 
                               IARG_INST_PTR,
                               IARG_THREAD_ID, IARG_END);
            }
            else 
            {
                INS_InsertCall(BBL_InsHead(bbl), IPOINT_BEFORE,
                               AFUNPTR(Advance), 
                               IARG_ADDRINT, icount, 
                               IARG_UINT32, BBL_NumIns(bbl),
                               IARG_ADDRINT, static_cast<ADDRINT>(0), // pass a null instead
                               IARG_INST_PTR, IARG_THREAD_ID,
                               IARG_END);
            }
        }
    }
    
    static VOID Advance(MT_ALARM_ICOUNT * ic, INT32 c, CONTEXT * ctxt, VOID * ip, THREADID tid)
    {
        if (!ic->_armed[tid])
            return;

        if (!ic->_tid[tid])
            return;

        BOOL fire = ic->_always_armed[tid];
        if (!fire)  {
            ic->_count[tid] -= c;
            if (ic->_count[tid] <= 0)
                fire = TRUE;
        }

        if (fire) {
            if (ic->_rearm[tid])
                ic->_count[tid] = ic->_orig_count[tid];
            else 
                ic->_armed[tid] = FALSE;
            ic->_handler[tid](ic->_val, ctxt, ip, tid);
        }
    }

  protected:    
    INT64 *_count;
    INT64 *_orig_count;
    VOID * _val;
    ALARM_HANDLER *_handler;
    BOOL _active;
    BOOL *_armed;
    BOOL *_rearm;
    BOOL *_always_armed;
    BOOL _passContext;
    UINT64 _maxThreads;
    BOOL *_tid;
};

/*! @defgroup MT_ALARM_MCOUNT
  @ingroup ALARM
  Signal an alarm when the specified number of memory instructions has been executed

  The example below can be found in InstLibExamples/alarm_icount.cpp

  \include alarm_icount.cpp

*/
class MT_ALARM_MCOUNT: public MT_ALARM_ICOUNT
{
  public:
    ~MT_ALARM_MCOUNT()
    {
    }
    virtual VOID Activate()
    {
        ASSERTX(_active == FALSE);
        _active = TRUE;
        TRACE_AddInstrumentFunction(AlarmMCountInstrumentTrace, this);
    }
    
  private:
    static VOID AlarmMCountInstrumentTrace(TRACE trace, VOID * icount)
    {
        MT_ALARM_MCOUNT * ic = static_cast<MT_ALARM_MCOUNT *>(icount);
        if (!ic->_active) return;
        for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
        {
            for ( INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
            {
                if ( INS_IsMemoryRead(ins) || INS_IsMemoryWrite(ins) )
                {
                    if (ic->_passContext)
                    {
                        INS_InsertCall(ins, IPOINT_BEFORE,
                                       AFUNPTR(AlarmMCountAdvance),
                                       IARG_ADDRINT, icount, 
                                       IARG_CONTEXT, 
                                       IARG_INST_PTR,
                                       IARG_THREAD_ID, IARG_END);
                    }
                    else 
                    {
                        INS_InsertCall(ins, IPOINT_BEFORE,
                                       AFUNPTR(AlarmMCountAdvance), 
                                       IARG_ADDRINT, icount, 
                                       IARG_ADDRINT, static_cast<ADDRINT>(0), // pass a null instead
                                       IARG_INST_PTR, IARG_THREAD_ID,
                                       IARG_END);
                    }
                }
            }
        }
    }
    static VOID AlarmMCountAdvance(MT_ALARM_MCOUNT * ic, CONTEXT * ctxt, VOID * ip, THREADID tid)
    {
        if (!ic->_armed[tid])
            return;

        if (!ic->_tid[tid])
            return;

        BOOL fire = ic->_always_armed[tid];
        if (!fire)  {
            ic->_count[tid] --;
            if (ic->_count[tid] <= 0)
                fire = TRUE;
        }

        if (fire) {
            if (ic->_rearm[tid])
                ic->_count[tid] = ic->_orig_count[tid];
            else 
                ic->_armed[tid] = FALSE;
            ic->_handler[tid](ic->_val, ctxt, ip, tid);
        }
    }
};

class PINPOINT_ENTRY
{
  public:
    PINPOINT_ENTRY(THREADID t, UINT64 start, UINT64 stop)
        :
        _tid(t),
        _start(start),
        _stop(stop),
        active(false)
    {
    }
    THREADID GetTid()
    {
        return _tid;
    }
    UINT64 GetStart()
    {
        return _start;
    }
    UINT64 GetStop()
    {
        return _stop;
    }
    BOOL IsActive()
    {
        return active;
    }
    VOID SetActive()
    {
        active = true;
    }
    friend ostream & operator<<(ostream & o, PINPOINT_ENTRY & entry)
    {
        o << "thread " << entry._tid
          << " start: " << entry._start
          << " stop: " << entry._stop << endl;
        
        return o;
    }
    
  private:
    THREADID _tid;
    UINT64 _start;
    UINT64 _stop;
    BOOL active;
};

class MT_PINPOINT_ENTRY
{
  public:
    MT_PINPOINT_ENTRY(UINT32 pp_num, UINT32 nthreads, UINT64 ticount)
        :
        _pp_num(pp_num),
        _num_threads(nthreads),
        _total_icount(ticount)
    {
        done = false;
    }
    VOID AddPinPointEntry(PINPOINT_ENTRY *pp_entry)
    {
        _pp_entries.push_back(pp_entry);
    }
    VOID Check()
    {
        ASSERTX(_num_threads == _pp_entries.size());
    }
    UINT32 GetPPNum()
    {
        return _pp_num;
    }
    UINT32 GetThreadCount()
    {
        return _num_threads;
    }
    UINT64 GetStartICount(THREADID tid)
    {
        for ( UINT32 i = 0; i < _pp_entries.size(); i++ )
        {
            if ( _pp_entries[i]->GetTid() == tid )
            {
                return _pp_entries[i]->GetStart();
            }
        }
        return 0;
    }
    INT64 GetLength(THREADID tid)
    {
        for ( UINT32 i = 0; i < _pp_entries.size(); i++ )
        {
            if ( _pp_entries[i]->GetTid() == tid )
            {
                return _pp_entries[i]->GetStop()-_pp_entries[i]->GetStart();
            }
        }
        return 0;
    }
    PINPOINT_ENTRY * FindByTid(THREADID tid)
    {
        for ( UINT32 i = 0; i < _pp_entries.size(); i++ )
        {
            PINPOINT_ENTRY *pp_entry = _pp_entries[i];
            if ( pp_entry->GetTid() == tid )
            {
                return pp_entry;
            }
        }
        return NULL;
    }
    VOID Activate(MT_PINPOINT_ENTRY * prev, MT_ALARM_MCOUNT &alarm_mcount,
                  CONTROL_HANDLER & controlHandler, VOID * controlVal, CONTROL_EVENT ev,
                  THREADID tid)
    {
        _controlHandler = controlHandler;
        _controlVal = controlVal;
        for ( UINT32 i = 0; i < _pp_entries.size(); i++ )
        {
            PINPOINT_ENTRY *pp_entry = _pp_entries[i];
            ASSERTX(pp_entry);
            
            if (tid == ALL_THREADS || tid == pp_entry->GetTid() )
            {
                if ( ev == CONTROL_START )
                {
                    //cerr << "Activating START for thread " << pp_entry->GetTid() << " "
                    //<< pp_entry->GetStart() << endl;
                    if ( !pp_entry->IsActive() )
                    {
                        PINPOINT_ENTRY * prev_pp_entry = NULL;
                        if ( prev )
                        {
                            prev_pp_entry = prev->FindByTid(tid);
                        }
                        INT32 length = pp_entry->GetStart();
                        // if there was a previous pinpoint, we need to
                        // subtract its stop ctoun from the current start
                        // count
                        if ( prev_pp_entry )
                        {
                            //cerr << "curr " << pp_entry->GetStart()
                            //     << " prev " << prev_pp_entry->GetStop() << endl;
                            length -= prev_pp_entry->GetStop();
                        }
                        alarm_mcount.SetAlarm(length, Start, this, pp_entry->GetTid());
                        pp_entry->SetActive();
                    }
                }
                else if ( ev == CONTROL_STOP )
                {
                    //cerr << "Activating STOP for thread " << tid << pp_entry->GetStop() << endl;
                    INT32 length = pp_entry->GetStop()-pp_entry->GetStart();
                    ASSERTX(length>=0);
                    if (length>0)
                    {
                        alarm_mcount.SetAlarm(length, Stop, this, pp_entry->GetTid());
                    }
                    else
                    {
                        // FIXME: Save the start event ctxt and ip and pass
                        // it here
                        Stop(this, NULL, NULL, tid);
                        ASSERTX(0);
                    }
                }
                else
                {
                    ASSERTX(0);
                }
            }
        }
    }
    VOID SetDone()
    {
        done = true;
    }
    BOOL IsDone()
    {
        return done;
    }
    BOOL HasTid(THREADID tid)
    {
        for ( UINT32 i = 0; i < _pp_entries.size(); i++ )
        {
            if ( tid == _pp_entries[i]->GetTid() )
            {
                return true;
            }
        }
        return false;
    }
    friend ostream & operator<<(ostream & o, MT_PINPOINT_ENTRY & entry)
    {
        o << "pinpoint: " << entry._pp_num
          << " numthreads " << entry._num_threads
          << " total_icount: " << entry._total_icount << endl;
        
        for ( UINT32 i = 0; i < entry._pp_entries.size(); i++ )
        {
            o << *entry._pp_entries[i];
        }
        return o;
    }
        
  private:
    UINT32 _pp_num;
    UINT32 _num_threads;
    UINT64 _total_icount;
    vector<PINPOINT_ENTRY *>_pp_entries;
    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
    BOOL done;

    static VOID Start(VOID * val, CONTEXT * ctxt, VOID * ip, THREADID tid)
    {
        MT_PINPOINT_ENTRY * cl = static_cast<MT_PINPOINT_ENTRY *>(val);

        cl->_controlHandler(CONTROL_START, cl->_controlVal, ctxt, ip, tid);
    }            
    static VOID Stop(VOID * val, CONTEXT * ctxt, VOID * ip, THREADID tid)
    {
        MT_PINPOINT_ENTRY * cl = static_cast<MT_PINPOINT_ENTRY *>(val);

        cl->_controlHandler(CONTROL_STOP, cl->_controlVal, ctxt, ip, tid);
    }        
};
    
class MT_PINPOINT_MANAGER
{
  public:
    MT_PINPOINT_MANAGER(string fn, CONTROL_HANDLER controlHandler, VOID * controlVal)
        :
        _filename(fn),
        _controlHandler(controlHandler),
        _controlVal(controlVal)
    {
        ReadMtPPFile(_filename);
        for ( UINT32 i = 0; i < PIN_MAX_THREADS; i++ )
        {
            _curr_pp_index[i] = -1;
            _prev_pp_index[i] = -1;            
        }
        _mcount_alarm.Activate();
        _curr_global_index = 0;
        _prev_global_index = 0;
        _handshake_barrier = _pinpoint_barrier = _pinpoints[_curr_global_index]->GetThreadCount()-1;
        _wait_for_master_thread = 1;        
        ActivatePinPointEvent(CONTROL_START);
    }
    UINT32 CurrentPpNum()
    {
        return _pinpoints[_curr_global_index]->GetPPNum();
    }    
    UINT32 CurrentPpNum(THREADID tid)
    {
        INT32 index = _curr_pp_index[tid];
        return _pinpoints[index]->GetPPNum();
    }
    UINT64 CurrentPpStartIcount(THREADID tid)
    {
        INT32 index = _curr_pp_index[tid];        
        return _pinpoints[index]->GetStartICount(tid);
    }
    INT64 CurrentPpLength(THREADID tid)
    {
        INT32 index = _curr_pp_index[tid];        
        return _pinpoints[index]->GetLength(tid);
    }
    VOID YieldThread()
    {
#if defined(TARGET_LINUX)
        sched_yield();
#endif
#if defined(TARGET_WINDOWS)
        ASSERTX(0);
#endif
#if defined(TARGET_MAC)
        ASSERTX(0);
#endif
    }
    VOID AtomicDecrement(INT32 * counter)
    {
#if defined(TARGET_IPF) || defined(TARGET_WINDOWS) || defined(TARGET_MAC)
        ASSERTX(0);
#else
        ATOMIC_DEC_W(counter);
#endif
    }
    VOID Synchronize(CONTROL_EVENT ev, THREADID  tid)
    {
        // here we want to spin if:
        // 1. The current pinpoint number for this thread is different
        //    from the global current pinpoints number being processed
        // 2. If I am the master thread (in which case 1 above should never
        //    happen because that's thread 0) and not all the threads in the
        //    current poinpoints have generated their START event yet.
        if ( tid == 0 )
        {
            do
            {
                YieldThread();
            } while ( _pinpoint_barrier > 0);
            //cerr << tid << " past pinpoints_barrier " << endl;

            _handshake_barrier = _pinpoints[_curr_global_index]->GetThreadCount()-1;

            // At this point, all threads have received their events.
            // before letting other threads proceed, we need to activate
            // the next pinpoint
            if ( ev == CONTROL_START )
            {
                _pinpoint_barrier = _pinpoints[_curr_global_index]->GetThreadCount()-1;
            }
            else if ( ev == CONTROL_STOP )
            {
                _pinpoints[_curr_global_index]->SetDone();
                _prev_global_index = _curr_global_index;
                if ( static_cast<UINT32>(_curr_global_index) < _pinpoints.size()-1 )
                {
                    _curr_global_index ++;
                    _pinpoint_barrier = _pinpoints[_curr_global_index]->GetThreadCount()-1;
                    //cerr << "next pp: " << _pinpoints[_curr_global_index]->GetPPNum() << endl;
                    ActivatePinPointEvent(CONTROL_START);
                }
            }

            // let all threads make progress now
            _wait_for_master_thread = 0;
            do
            {
                YieldThread();
            } while ( _handshake_barrier > 0);
            //cerr << tid << " past handshake_barrier " << endl;

            _wait_for_master_thread = 1;                    
        }
        else
        {
            INT32 thread_pp_index = _curr_pp_index[tid];
            while ( _pinpoints[thread_pp_index]->GetPPNum() !=
                    _pinpoints[_curr_global_index]->GetPPNum() )
            {
                YieldThread();
            }
            AtomicDecrement(&_pinpoint_barrier);
            do 
            {
                YieldThread();
            } while ( _wait_for_master_thread );
            AtomicDecrement(&_handshake_barrier);
        }
    }
    VOID HandleStartEvent(THREADID tid)
    {
        Synchronize(CONTROL_START, tid);
        ActivatePinPointEvent(CONTROL_STOP, tid);
    }
    VOID HandleStopEvent(THREADID tid)
    {
        Synchronize(CONTROL_STOP, tid);
    }
    VOID Event(CONTROL_EVENT ev, THREADID tid)
    {
        switch(ev)
        {
          case CONTROL_START:
            HandleStartEvent(tid);
            break;

          case CONTROL_STOP:
            HandleStopEvent(tid);            
            break;
            
          default:
            ASSERTX(false);
        }        
    }
    friend ostream & operator<<(ostream & o, MT_PINPOINT_MANAGER & entry)
    {
        for ( UINT32 i = 0; i < entry._pinpoints.size(); i++ )
        {
            o << *entry._pinpoints[i];
        }
        return o;
    }
    
    
  private:
    string _filename;    
    vector<MT_PINPOINT_ENTRY *>_pinpoints;
    MT_ALARM_MCOUNT _mcount_alarm;
    INT32 _prev_pp_index[PIN_MAX_THREADS];    
    INT32 _curr_pp_index[PIN_MAX_THREADS];
    INT32 _curr_global_index, _prev_global_index;
    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
    INT32 _pinpoint_barrier, _handshake_barrier;
    INT32 _wait_for_master_thread;

    MT_PINPOINT_ENTRY * FindPPIndex(THREADID tid)
    {
        BOOL found = false;
        // Find next not done index for each thread
        for ( UINT32 i = _curr_global_index; i < _pinpoints.size(); i++ )
        {
            if ( _pinpoints[i]->HasTid(tid) || !_pinpoints[i]->IsDone() )
            {
                if ( _curr_pp_index[tid] != -1 )
                {
                    _prev_pp_index[tid] = _curr_pp_index[tid];
                }
                _curr_pp_index[tid] = i;
                found = true;
                break;
            }
        }
        if ( found )
        {
            return _pinpoints[_curr_pp_index[tid]];            
        }
        else
        {
            return NULL;
        }
    }    
    VOID ActivatePinPointEvent(CONTROL_EVENT ev, THREADID tid=static_cast<THREADID>(ALL_THREADS))
    {
        MT_PINPOINT_ENTRY * curr_pp = NULL;
        MT_PINPOINT_ENTRY * prev = NULL;        
        if ( tid == ALL_THREADS )
        {
            for ( THREADID t = 0; t < PIN_MAX_THREADS; t++ )
            {
                curr_pp = FindPPIndex(t);
                if ( curr_pp )
                {
                    // get a pointer to the previous pinpoint
                    if ( _prev_pp_index[t] != -1 )
                    {
                        prev = _pinpoints[_prev_pp_index[t]];
                    }
                    // check pp_index here
                    curr_pp->Activate(prev,
                                      _mcount_alarm,
                                      _controlHandler,
                                      _controlVal, ev, t);
                }
            }
        }
        else
        {
            curr_pp = FindPPIndex(tid);
            // get a pointer to the previous pinpoint
            if ( _prev_pp_index[tid] != -1 )
            {
                prev = _pinpoints[_prev_pp_index[tid]];
            }            
            if ( curr_pp )
            {
                curr_pp->Activate(prev,
                                  _mcount_alarm,
                                  _controlHandler,
                                  _controlVal, ev, tid);
            }            
        }
    }
    VOID ReadMtPPFile(string filename)
    {
        UINT32 lineNum = 0;
        ifstream pp(filename.c_str());
        if (!pp)
        {
            cerr << "Could not open pinpoint file " <<
                filename.c_str() << endl;
            exit(-1);
        }

        MT_PINPOINT_ENTRY *mt_pp_entry = 0;
        
        while(true)
        {
            istringstream line( ReadLine(pp, lineNum) );

            if( pp.eof() )
            {
                break;
            }

            string command;
            line >> command;

            if ("pinpoint" == command)
            {
                UINT32 ppnum;
                UINT32 numthreads;
                UINT64 total_icount;
                
                line >> ppnum;
                line >> numthreads;
                line >> total_icount;
                mt_pp_entry = new MT_PINPOINT_ENTRY(ppnum, numthreads, total_icount);
            }
            else if ("thread" == command)
            {
                UINT64 threadnum;
                UINT64 start;
                UINT64 stop;
                PINPOINT_ENTRY *pp_entry = 0;
                    
                line >> threadnum;
                line >> start;
                line >> stop;
                pp_entry = new PINPOINT_ENTRY(threadnum, start, stop);
                ASSERTX(mt_pp_entry);
                mt_pp_entry->AddPinPointEntry(pp_entry);
            }
            else if ("endpinpoint" == command)
            {
                ASSERTX(mt_pp_entry);
                _pinpoints.push_back(mt_pp_entry);
                mt_pp_entry = 0;
            }
            else
            {
                PpFileError(lineNum);
                cerr << "at token \"" << command << "\"" << endl;
                exit(1);
            }
        }
    }
    static string ReadLine(istream& inputFile, UINT32 &lineNum)
    {
        CHAR buf[2048];
        do
        {
            inputFile.getline(buf, sizeof(buf), '\n');
            lineNum++;
        }
        while( !inputFile.eof() && (buf[0] == '#' || buf[0] == 0 ));

        // workaround for some library problems
        return string(buf);
    }
    static VOID PpFileError(INT32 line)
    {
        cerr << "Error in pinpoint file at line " << line << ": ";
    }    
};

class CONTROL_MT_PINPOINT
{
  public:
    CONTROL_MT_PINPOINT(BOOL passContext=false, const string & prefix = "")
        : _passContext(passContext),
          _ppFileKnob(KNOB_MODE_WRITEONCE,
                      "pintool",
                      "mt_ppfile",
                      "",
                      "MtPinPoint file",
                      prefix)
    {
        _maxThreads = 20; // ISIMPOINT_MAX_THREADS
        _manager = 0;
    }
    VOID Event(CONTROL_EVENT ev, THREADID tid)
    {
        if ( _manager )
        {
            _manager->Event(ev, tid);
        }
    }
    /*! @ingroup CONTROLLER_MT_PINPOINT
      Activate the controller if the -mt_ppfile knob is provided
      @return 1 if controller can start an interval, otherwise 0
    */
    INT32 CheckKnobs(CONTROL_HANDLER ch, VOID * val, ofstream * outstreamp)
    {
        if (strcmp(_ppFileKnob.Value().c_str(),"") == 0)
        {
            return 0;
        }
        _active = true;
        _outstreamp = outstreamp;

        ASSERTX(!_manager);
        _manager = new MT_PINPOINT_MANAGER(_ppFileKnob.Value(), ch, val);

        if ( _outstreamp )
        {
            *_outstreamp << *_manager;
        }
        _controlHandler = ch;
        _controlVal = val;
        return 1;
    }
    bool IsActive() const { return _active; }
    UINT32 CurrentPp(THREADID tid) const
    {
        return _manager->CurrentPpNum(tid);
    }
    UINT64 CurrentPpStartIcount(THREADID tid) const
    {
        return _manager->CurrentPpStartIcount(tid);
    }
    INT64 CurrentPpLength(THREADID tid) const
    {
        return _manager->CurrentPpLength(tid);
    }        

  private:
    BOOL _passContext;    
    KNOB<string> _ppFileKnob;
    CONTROL_HANDLER _controlHandler;
    VOID * _controlVal;
    bool _active;
    UINT64 _maxThreads;
    ofstream * _outstreamp;
    MT_PINPOINT_MANAGER * _manager;
};
