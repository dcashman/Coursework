//Groups: @ingroup\s+(API_REF|KNOBS|IMG_BASIC_API|INS_BASIC_API|INS_INST_API|INS_BASIC_API_GEN_IA32|INS_BASIC_API_IA32|INS_BASIC_API_IPF|INS_MOD_API_GEN_IA32|SEC_BASIC_API|RTN_BASIC_API|REG_BASIC_API|REG_CPU_GENERIC|REG_CPU_IPF|REG_CPU_IA32|TRACE_BASIC_API|BBL_BASIC_API|SYM_BASIC_API|MISC_PRINT|MISC_PARSE|KNOB_API|KNOB_BASIC|KNOB_PRINT|LOCK|PIN_CONTROL|PROTO_API|PIN_PROCESS_API|PIN_THREAD_API|DEBUG_API|TYPE_BASE|INSTLIB|ALARM|ROGUE|ROGUE_CFG|ROGUE_CFG_BASIC_API|ROGUE_EDG_BASIC_API|ROGUE_BBL_BASIC_API|ROGUE_INS_BASIC_API|ROGUE_TRACE_BASIC_API|ROGUE_CONTROL_BASIC_API|CODECACHE_API|CHILD_PROCESS_API|SUPERPIN_API|UTILS|IPF_UTILS_API|MISC|CONTEXT_API|CHECKPOINTING|PROTO|INST_ARGS|PIN_THREAD_PRIVATE|CHILD_PROCESS_INTERNAL|EDG_BASIC|BBL_BASIC|ROGUE_BASIC_API|CODECACHE_NYI_API|INS_BASIC_API_GEN_IPF|MESSAGE_TYPE|MESSAGE_BASIC|ERRFILE|MISC_BASIC|ITC_INST_API|CHECKPOINTING_PRIVATE|CONTEXT_API_UNDOC|UNDOCUMENTED_PIN_API|DEPRECATED_PIN_API|OPIN|TRACE_VERSIONS
/* PIN API */

/* THIS FILE IS AUTOMAGICALLY GENERATED - DO NOT CHANGE DIRECTLY*/


class LOGFILE
{
  public:
    virtual VOID Open(const CHAR *name) = 0;
    virtual BOOL IsOpen() const = 0;
    virtual VOID Close() = 0;
    virtual VOID Write(const CHAR* message, size_t length) = 0;
    VOID Write(const string& message);
    virtual ~LOGFILE() {}
};

                                                                  /* DO NOT EDIT */
enum LOGTYPE 
{
    LOGTYPE_CONSOLE,              //!< log only to the console
    LOGTYPE_LOGFILE,              //!< log only to the log file
    LOGTYPE_CONSOLE_AND_LOGFILE   //!< log to both the log file and console
};

                                                                  /* DO NOT EDIT */
typedef enum 
{
    PIN_ERR_NONE,                   // 0 args
    PIN_ERR_INTERNAL,               // 0 args
    PIN_ERR_ASSERT,                 // 0 args
    PIN_ERR_TOOL,                   // 0 args
    PIN_ERR_CMD_LINE,               // 0 args
    PIN_ERR_PARSING,                // 0 args
    PIN_ERR_NO_APP,                 // 0 args
    PIN_ERR_INVALID_PLOAD,          // 0 args
    PIN_ERR_FILE_NOT_FOUND,         // 1 arg: file name
    PIN_ERR_ELF_HEADER,             // 1 arg: file name
    PIN_ERR_NO_PIE,                 // 1 arg: file name
    PIN_ERR_NO_ENTRY_OFFSET,        // 1 arg: file name
    PIN_ERR_INVALID_ADDRESS,        // 1 arg: address
    PIN_ERR_INVALID_ADDRESS_RANGE,  // 2 args: low address, high address
    PIN_ERR_CANNOT_LOAD_TOOL,       // 1 arg: tool name
    PIN_ERR_WAIT_FOR_INJECTOR,      // 2 args: pid, errno
    PIN_ERR_BAD_OS_FILE,            // 0 args
    PIN_ERR_UNSUPPORTED_OS,         // 0 args
    PIN_ERR_BIN32,                  // 0 args
    PIN_ERR_BIN64,                  // 0 args
    PIN_ERR_CANNOT_RUN,             // 1 arg: errno
    PIN_ERR_ATTACH_PID,             // 1 arg: errno
    PIN_ERR_DETACH_PID,             // 1 arg: errno
    PIN_ERR_WAIT_PID,               // 1 arg: errno
    PIN_ERR_STEP_PID,               // 1 arg: errno
    PIN_ERR_WAIT_EXECV,             // 1 arg: errno
    PIN_ERR_APP_RESUME,             // 1 arg: errno
    PIN_ERR_NO_INSTRUMENT,          // 2 args: pid, errno
    PIN_ERR_CHILD_PTRACEME,         // 1 arg: errno
    PIN_ERR_CHILD_RESUME,           // 1 arg: errno
    PIN_ERR_RELINK_APP,             // 0 args
    PIN_ERR_KNOB_BAD,               // 1 arg: knob
    PIN_ERR_NO_RELOCS,              // 0 args
    PIN_ERR_VERSION_SKEW,           // 0 args
    PIN_ERR_MS_DOS,                 // 0 args
    PIN_ERR_LONG_PATH,              // 0 args
    PIN_ERR_LAST
} PIN_ERRTYPE;

                                                                  /* DO NOT EDIT */
typedef BOOL (*MESSAGE_CALLBACK)(const string &message,
                                        PIN_ERRTYPE type, va_list ap);

                                                                  /* DO NOT EDIT */
class MESSAGE_TYPE
{    
  private:
    STATIC VOID (*_panic_callback) ();
    STATIC string *_log_fname;
    STATIC string *_image_name;
    STATIC MESSAGE_TYPE *_list;
    STATIC LOGFILE   *_logstream;
    STATIC MESSAGE_CALLBACK _message_callback;
    
  private:
    const string _name;
    const string _prefix;
    const string _comment;

    BOOL _terminate;
    BOOL _trace;
    BOOL _enabled;
    BOOL _resource;
    LOGTYPE _log;
    // RM: see remarks in constructor
    BOOL _initted;

    MESSAGE_TYPE   *_next;

    
  public:
    STATIC VOID RegisterPanicCallBack( VOID (*cb) () );
    STATIC VOID RegisterLogFile(const string& name );
    STATIC string GetLogFileName();
    STATIC VOID RegisterImageName(const string& name );
    STATIC MESSAGE_TYPE * Find(const string& name);
    STATIC BOOL Disable(const string& name);
    STATIC BOOL Enable(const string& name);
    STATIC UINT32 Number();
    STATIC string StringLongAll(); 
    STATIC BOOL IsTerminating();
    STATIC VOID PrintMessageAndExit(const char *message, const char *message2,
                                    PIN_ERRTYPE errType = PIN_ERR_NONE, ...);
//    STATIC BOOL WriteUserErrorFile( const string& message, PIN_ERRTYPE errType, va_list va);
//    STATIC VOID RegisterErrorFile( const string& name );
    STATIC VOID RegisterMessageCallBack(MESSAGE_CALLBACK fun);
    STATIC MESSAGE_CALLBACK MessageCallBack();
    STATIC BOOL ErrorFileSpecified();
    
  private:
    STATIC VOID DumpTrace();
    
  public:
    MESSAGE_TYPE( string name, string prefix, BOOL terminate, BOOL trace, BOOL enabled, BOOL resource, 
                  LOGTYPE log, string comment);
    
    VOID Message(const string& message, BOOL printPrefix = TRUE,
                                        PIN_ERRTYPE errType = PIN_ERR_NONE, ...);

    BOOL on() const {return _enabled;}
    VOID disable() {_enabled=false;}
    VOID enable() {_enabled=true;}

    int Compare(const MESSAGE_TYPE & k2) const;

};

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeWinError;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeError;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeWarning;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeConsole;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeConsoleNoPrefix;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeAssert;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypePhase;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeKnown;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeInfo;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeDebug;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeOpportunity;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeStats;

                                                                  /* DO NOT EDIT */
extern MESSAGE_TYPE MessageTypeLog;

                                                                  /* DO NOT EDIT */
extern const string Line1;

                                                                  /* DO NOT EDIT */
extern UINT64 MilliSecondsElapsed();

                                                                  /* DO NOT EDIT */
extern VOID BreakMe();

                                                                  /* DO NOT EDIT */
/* DO NOT EDIT */

#define QMESSAGE(t,m)             do{ if(t.on()) t.Message(m); }while(0)
#define QMESSAGE0(t,m,p,e)        do{ if(t.on()) t.Message(m,p,e); }while(0)
#define QMESSAGE1(t,m,p,e,a1)     do{ if(t.on()) t.Message(m,p,e,a1); }while(0)
#define QMESSAGE2(t,m,p,e,a1,a2)  do{ if(t.on()) t.Message(m,p,e,a1,a2); }while(0)
#define QMESSAGEP(t,m,p)          do{ if(t.on()) t.Message(m,p); }while(0)
#define QMESSAGE_LIMIT(t,m,max)   do{ static int qqq = 0; if( qqq++ >= max) break; if(t.on()) t.Message(m);}while(0)


#define ASSERTQ(message) QMESSAGE0(MessageTypeAssert, message, TRUE, LEVEL_BASE::PIN_ERR_ASSERT)

#if defined(PIN_ASSERT_NO_FILE_NAME)
# define PIN_ASSERT_FILE ""
#else
# define PIN_ASSERT_FILE __FILE__
#endif

#define ASSERT(condition,message)   \
    do{ if(!(condition)) ASSERTQ(string(PIN_ASSERT_FILE) + ":" + __FUNCTION__  + ":" \
                                 + decstr(static_cast<LEVEL_BASE::UINT32>(__LINE__)) + ": " + message);} while(0)

#define ASSERTX(condition) ASSERT(condition,"assertion failed: " #condition  "\n")

#define ASSERTSLOW(condition,message)   \
  do{ if(KnobSlowAsserts && !(condition)) \
          ASSERTQ(string(PIN_ASSERT_FILE) + ":" + __FUNCTION__  + ":" +  \
                  decstr(static_cast<LEVEL_BASE::UINT32>(__LINE__)) + ": " + message);} while(0)

#define ASSERTXSLOW(condition) ASSERTSLOW(condition,"assertion failed: " #condition  "\n")

#define NYI()  ASSERT(0, "Not Yet Implemented\n")

#define PIN_ERROR(message)                    QMESSAGE0(MessageTypeError, message, TRUE, PIN_ERR_INTERNAL)
#define PIN_ERROR0(message, type)             QMESSAGE0(MessageTypeError, message, TRUE, type)
#define PIN_ERROR1(message, type, arg1)       QMESSAGE1(MessageTypeError, message, TRUE, type, arg1)
#define PIN_ERROR2(message, type, arg1, arg2) QMESSAGE2(MessageTypeError, message, TRUE, type, arg1, arg2)

// WIN_ERROR does not exit (_terminate=FALSE). winloader needs to kill the application
// process before it terminates, otherwise the application process will remain
// 'floating around' (created and suspended).  These macros are supported on non-windows
// systems as well.

#define WIN_ERROR(message)                    QMESSAGE0(MessageTypeWinError, message, TRUE, PIN_ERR_INTERNAL)
#define WIN_ERROR0(message, type)             QMESSAGE0(MessageTypeWinError, message, TRUE, type)
#define WIN_ERROR1(message, type, arg1)       QMESSAGE1(MessageTypeWinError, message, TRUE, type, arg1)
#define WIN_ERROR2(message, type, arg1, arg2) QMESSAGE2(MessageTypeWinError, message, TRUE, type, arg1, arg2)

/* 
 for calling even before the MessageType* constructors may have executed
 */
#define PIN_FATAL_ERROR(message) { PIN_ERROR(message); MESSAGE_TYPE::PrintMessageAndExit(message, "E: ");}

/*
 * The special message dumper. It copies the message to the log file,
 * sends it to stderr and exits pin with -1.
 * @param - message of type const char* to dump
 */
#define PIN_OUT_OF_MEMORY(message) MESSAGE_TYPE::PrintMessageAndExit(message, "Pin is out of memory: ")

#define CONSOLE(message) QMESSAGE(MessageTypeConsole, message)

#define CONSOLE_NOPREFIX(message) QMESSAGE(MessageTypeConsoleNoPrefix, message)

#define PHASE(message) QMESSAGE( MessageTypePhase, message)

#define WARNING(message) QMESSAGE( MessageTypeWarning,message)
#define WARNING_LIMIT(message,n) QMESSAGE_LIMIT( MessageTypeWarning, message, n)

#define KNOWN(message) QMESSAGE( MessageTypeKnown, message)
#define KNOWN_LIMIT(message,n) QMESSAGE_LIMIT( MessageTypeKnown, message, n)

#define LOG(message) QMESSAGE(MessageTypeLog, message)

#define INFO(message) QMESSAGE(MessageTypeInfo,message)
#define INFO_LIMIT(message,n) QMESSAGE_LIMIT(MessageTypeInfo, message, n)

#define OPPORTUNITY(message) QMESSAGE(MessageTypeOpportunity,message)
#define OPPORTUNITY_LIMIT(message,n) QMESSAGE_LIMIT(MessageTypeOpportunity,message, n)
#define PIN_DEBUG(message) QMESSAGE(MessageTypeDebug, message)
#define STATS(message) QMESSAGE(MessageTypeStats, message)

