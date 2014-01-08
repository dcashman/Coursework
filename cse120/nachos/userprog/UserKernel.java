package nachos.userprog;

import nachos.machine.*;
import nachos.threads.*;
import nachos.userprog.*;
import java.util.LinkedList;  //AFTER MERGE

/**
 * A kernel that can support multiple user processes.
 */
public class UserKernel extends ThreadedKernel {
    /**
     * Allocate a new user kernel.
     */
    public UserKernel() {
	super();
    }

    /**
     * Initialize this kernel. Creates a synchronized console and sets the
     * processor's exception handler.
     */
    public void initialize(String[] args) {
	super.initialize(args);

	console = new SynchConsole(Machine.console());
        pageLock=new Lock();  
        pid_lock=new Lock();  
        numProcessLock=new Lock();
        processWait = new Condition(pageLock);
	initFreePages();
	Machine.processor().setExceptionHandler(new Runnable() {
		public void run() { exceptionHandler(); }
	    });
    }
    private void initFreePages(){    
	for(int i=0; i< Machine.processor().getNumPhysPages(); i++){    //potential issue with .processor()?  
            freePagesPool.add(i);                                       
        }
        return;
    }
    /**
     * Test the console device.
     */	
    public void selfTest() {
	super.selfTest();

	System.out.println("Testing the console device. Typed characters");
	System.out.println("will be echoed until q is typed.");

	char c;

	do {
	    c = (char) console.readByte(true);
	    console.writeByte(c);
	}
	while (c != 'q');

	System.out.println("");
    }

    /**
     * Returns the current process.
     *
     * @return	the current process, or <tt>null</tt> if no process is current.
     */
    public static UserProcess currentProcess() {
	if (!(KThread.currentThread() instanceof UThread))
	    return null;
	
	return ((UThread) KThread.currentThread()).process;
    }

    /**
     * The exception handler. This handler is called by the processor whenever
     * a user instruction causes a processor exception.
     *
     * <p>
     * When the exception handler is invoked, interrupts are enabled, and the
     * processor's cause register contains an integer identifying the cause of
     * the exception (see the <tt>exceptionZZZ</tt> constants in the
     * <tt>Processor</tt> class). If the exception involves a bad virtual
     * address (e.g. page fault, TLB miss, read-only, bus error, or address
     * error), the processor's BadVAddr register identifies the virtual address
     * that caused the exception.
     */
    public void exceptionHandler() {
	Lib.assertTrue(KThread.currentThread() instanceof UThread);

	UserProcess process = ((UThread) KThread.currentThread()).process;
	int cause = Machine.processor().readRegister(Processor.regCause);
	process.handleException(cause);
    }

    /**
     * Start running user programs, by creating a process and running a shell
     * program in it. The name of the shell program it must run is returned by
     * <tt>Machine.getShellProgramName()</tt>.
     *
     * @see	nachos.machine.Machine#getShellProgramName
     */
    public void run() {
	super.run();

	UserProcess process = UserProcess.newUserProcess();
        process.isRoot=true;      //all subsequent processes are called by this process or one of its children
	
	String shellProgram = Machine.getShellProgramName();	
		Lib.assertTrue(process.execute(shellProgram, new String[] { }));                       
		//Lib.assertTrue(process.execute("echo.coff", new String[] {"b","o","o","p"}));                       


	KThread.currentThread().finish();
    }

    /**
     * Terminate this kernel. Never returns.
     */
    public void terminate() {
	super.terminate();
    }
    public static void assignPID(UserProcess process){  //AFTER MERGE2 
	pid_lock.acquire();
        pidLockOwner=currentProcess().pid;
        process.setPID(next_pid);
        next_pid++;
        pid_lock.release();
        return;
    }

    /** Globally accessible reference to the synchronized console. */
    public static SynchConsole console;

    // dummy variables to make javac smarter
    private static Coff dummy1 = null;
    /****************************************************************************************************************************************************
     * DAN AND PINKY ADDED
     ******************************************************************************************************************************************************/
    //FOLLOWING AFTER MERGE
    public static Lock pageLock;       //guards free pages pool1g
    public static Condition processWait;
    public static Lock pid_lock;       //guards next_pid
    public static int next_pid=0;     //pid to be assigned to next process
    public static Lock numProcessLock;
    public static int numActiveProcesses=0;   //active processes
    public static LinkedList<Integer> freePagesPool = new LinkedList<Integer>();  
    //DEBUG
    public static int numProcessLockOwner=-1;
    public static int pidLockOwner=-1;
}
