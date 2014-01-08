package nachos.vm;

import nachos.machine.*;
import nachos.threads.*;
import nachos.userprog.*;
import nachos.vm.*;
import java.util.*;

/**
 * A kernel that can support multiple demand-paging user processes.
 */
public class VMKernel extends UserKernel {
    /**
     * Allocate a new VM kernel.
     */
    public VMKernel() {
	super();
    }

    /**
     * Initialize this kernel.
     */
    public void initialize(String[] args) {
        //need to initialize the VMKernel slightly differently: need an inverted page table, a swap file, different locks, 

	super.initialize(args);  //bad parts from UserKernel?
        //initialize swap file
        swapFile=this.fileSystem.open(swapFileName, true);  //make sure this works DEBUG DAC
        numSwapPages=0; //there are no pages in swap at beginning
        numPhysPages = Machine.processor().getNumPhysPages();
        invertedPageTable = new invertedBucket[numPhysPages];
        initInvertedPageTable(numPhysPages); //initialize invertedPageTable 
        freeSwap=new PriorityQueue<Integer>();
        memLock= new Lock(); //create the memory lock
        pinneded= new Condition(memLock);  //memLock should work, DAC DEBUG
        //create all entries in TLB invalid?  (prob not necessary)

        //more...??? DAC 
        return;
    }

    /**
     * Test this kernel.
     */	
    public void selfTest() {
	super.selfTest();
    }

    /**
     * Start running user programs.
     */
    public void run() {
	super.run();
    }
    
    /**
     * Terminate this kernel. Never returns.
     */
    public void terminate() {
        this.fileSystem.remove(swapFileName);  //delete swap file  DAC DEBUG 
	super.terminate();

    }
    /**
     *initInvertedPageTable - creates all of the entries for the inverted page table
     *params:
     *  numPages - number of entries to put in inverted table
     *returns - N/A - void
     **/
    private void initInvertedPageTable(int numPages){
	for(int i=0;i<numPhysPages;i++){
	    invertedPageTable[i]=new invertedBucket(new TranslationEntry(), false, false, false, null); // DEBUG DAC  (might not need tEntry)
	}
    }
    /**
     * scanForFreePage - 
     * params: none
     * returns - ppn of available page, or -1 if all physical pages are being used
     */
    private int scanForFreePage() {
        //code
        //can probably just use the clockAlgorithm here, but have as a separate function to make conceptualization easier
        //scan invertedPageTable looking for page with valid == 0 (all should have valid set to 0 before being used)
        //use currentPage "pointer" as starting poing (same as clock alg, again can probably just use that)
        //return ppn of available page
        //else
        return -1;
    }


    /****************************************************************************************************
     *zero- fills the page in physical memory with zeros
     *params
     *  ppn - the page to zero out
     *return N/A
     *****************************************************************************************************/
    public static void zero(int ppn){
        //acquire pagelock, dipshit
        int physAddress = ppn*pageSize;
	byte[] memory = Machine.processor().getMemory();        
        byte zeero=0;
        Arrays.fill(memory,physAddress, physAddress+pageSize-1, zeero);  //fill a page with zeros  
        return;
    }


    /**
     * assignPage - changes values in invertedPageTable to assign to process with id pid
     * params:
     *   ppn - the page in invertedPageTable to modify
     *   VMProcess - the process to assign to that page
     *   vpn - the vpn in the process wit which this will be associated 
     * returns - n/a
     */
    private static void assignPage(int ppn, VMProcess process, int vpn) {
        //code
        //might need to call lock, but should already have lock in getAvailablePage
        invertedBucket bucky=invertedPageTable[ppn];
        bucky.tEntry.ppn=ppn;
        bucky.tEntry.vpn=vpn;
        bucky.tEntry.valid=true; //not necessary? DAC DEBUG 
        bucky.assigned=true;
        bucky.used=true; //do we want to do this? DAC DEBUG ???
        bucky.process=process;
        return;
    }

    /**
     * addToSwap - adds page from invertedPageTable to swap file for storage
     * params:
     *   ppn - the page in invertedPageTable to copy to swap file
     * returns - the page in swap file where it was saved
     */
    public static int addToSwap(int ppn) {
        //get next free page from freeSwap.  (consider case when there is no page in freeSwap as well, need to get new page from swap file)
        //maybe use a lock?
        int spn;
        if(freeSwap.size()==0){
	    spn=numSwapPages;
            numSwapPages++;
        }else{
            spn=freeSwap.poll();   //Integer object to 
        }
        int physicalAddr = ppn*pageSize;  //base of physical page
        int swapAddr = spn*pageSize;  //base of swap page
	byte[] memory = Machine.processor().getMemory();
        invertedPageTable[ppn].pinned=true;            
	memLock.release();
        swapFile.write(swapAddr,memory,physicalAddr,pageSize);  //DAC DEBUG
        memLock.acquire();
        invertedPageTable[ppn].pinned=false;
        pinneded.wakeAll();                 
         
        //return the page in swap that it has been saved to
        return spn;
    }
    /**
     * loadFromSwap - adds page from swap to page in inverted page table. Frees that page in swap
     * params:
     *   spn - the page in the swap file to grab
     *   ppn - the page in invertedPageTable to copy to swap file
     * returns - N/A
     */
    public static void loadFromSwap(int spn, int ppn) {
        //get next free page from freeSwap.  (consider case when there is no page in freeSwap as well, need to get new page from swap file)
        //maybe use a lock?
        int physicalAddr = ppn*pageSize;  //base of physical page
        int swapAddr = spn*pageSize;  //base of swap page
	byte[] memory = Machine.processor().getMemory();
        invertedPageTable[ppn].pinned=true;
        memLock.release();
        swapFile.read(swapAddr,memory,physicalAddr,pageSize);  //DAC DEBUG
        memLock.acquire();
        invertedPageTable[ppn].pinned=false;
        pinneded.wakeAll();                 
        //add this page to the freeSwap
        //zero out the page???
        freeSwap.add(spn);
        return;
    }
     /**
     * evictPage - kicks a page in physical memory out and notifies the process that owns it
     * params:  
     *   ppn - ppn to evict
     * returns: N/A
     */
    private static void evictPage(int ppn) {
        int vpn = invertedPageTable[ppn].tEntry.vpn;    
        VMProcess process = invertedPageTable[ppn].process;
	    //call savePage() to save physical page to swap if necessary and notify process that its page is invalid 'n' stuff
	process.savePage(vpn);  
            //call zeros() to zero out page?  (maybe not necessary, don't worry about for now)
	//zeros(ppn);
        return;  //placeholder
    }
     /**
     * clockAlgorithm - determines which page should be evicted, if no pages can be evicted (all pinned) put caller to sleep.
     * params:  none
     * returns: ppn of page chosen to be evicted
     */
    private static int clockAlgorithm() {
        //set starting point to currentPage
        int total=0;
        boolean notPinned=false;
        do{
            currentPage=currentPage%numPhysPages;  //going around modulo
            if(invertedPageTable[currentPage].assigned==false){
                currentPage++;
                return currentPage-1;   //make sure this works in java? DAC DEBUG
            }
            if(invertedPageTable[currentPage].pinned==true){    //it is pinned, mark it as unused and move on
                invertedPageTable[currentPage].used=false;
                int vpn=invertedPageTable[currentPage].tEntry.vpn;  //get the vpn that is currently assigned to this physical page
                VMProcess process=invertedPageTable[currentPage].process; //get the process that owns this physical page
                process.setUsed(vpn,false);  //your bit is no longer used
                currentPage++;
                total++;
            }else if(invertedPageTable[currentPage].used==false){   //not pinned and not used, gimme gimme
                currentPage++;
                return currentPage-1; 
            }else{                    //not pinned, but used, mark as unused - record that there is a non-pinned page
                notPinned=true;   //we have at least one non-pinned page
                invertedPageTable[currentPage].used=false;  //mark as unused
                int vpn=invertedPageTable[currentPage].tEntry.vpn;  //get the vpn that is currently assigned to this physical page
                VMProcess process=invertedPageTable[currentPage].process; //get the process that owns this physical page
                process.setUsed(vpn,false);  //your bit is no longer used
                currentPage++;   //move to next page
                total++;         //increment total
            }
            if(total>=numPhysPages && !notPinned){
                //put to sleep DAC DEBUG !!!
                System.out.print("\nGOING TO SLEEP IN CLOCK\n");
                pinneded.sleep();  //put to sleep
                //after waking up, reset total to 0;
                total=0;
                System.out.print("\nWOKE UP IN CLOCK\n");
            }
	}while(true);

    }

    /****************************************************************************************************************************************
     *getAvailablePage() - returns a page in physical memory which is available for use by a process.  Called when a process needs a page due
     *                      to an "invalid" bit set in their pageTable.  If there aren't any available pages, calls evictPage() to free one
                            and returns it.
     *params:
     *  VMProcess process - the process calling this function (the process asking for the page)
     * int vpn - the virtual page that this will be assigned to
     *returns - the next available page
     *****************************************************************************************************************************************/
    public static int getAvailablePage(VMProcess process, int vpn){
        int availablePage=-1; //the ppn we're after
        availablePage=clockAlgorithm();  //get the page we're going to give to the user
        if(invertedPageTable[availablePage].assigned==true){   //the page already has  an owner
	    evictPage(availablePage);   //take it away from someone
        }
        assignPage(availablePage,process, vpn);  //assign the page to the right process
	return availablePage;
    }
        

    // dummy variables to make javac smarter
    private static VMProcess dummy1 = null;

    private static final char dbgVM = 'v';
    private static final char w = 'w';
    private static final char trace = 'x';    
    private static final char lock = 'y';
    private static final char mem = 'z';

    /********************************************************************************************************************************************
     * DAN AND PINKY'S ADDED VARIABLES
     *******************************************************************************************************************************************/
    private static OpenFile swapFile;   //The swap file  
    /*might need to make "buckets" so we can keep track of PID, but for now will use ppn for pid since it doesn't seem to have a purpose if we index into this table by ppn  */
    private static final String swapFileName="swapswapswap.swp"; //name of swap file
    public static int numSwapPages;
    private static int pageSize=Processor.pageSize;
    public static invertedBucket[] invertedPageTable;   //make this private and adjust accordingly
    public static int numPhysPages;
    public static Lock memLock;         //lock accessed for each memory maintenance stuff (pinning, get page, etc)
    private static int currentPage=0;  //the page currently pointed to by the clock algorithm
    /*heap which returns lowest-valued free page in swap file, some more design needed here for managing memory, removing blocks, etc.  Should ask how dynamic freeing of the memory needs to be. (do we do as malloc, etc) */
    public static PriorityQueue<Integer> freeSwap; //holds freed pages
    public static Condition pinneded;  //with two eds in case I initialized another one already
    /*****************************************************************************************************
     * VMTest stuff
     *****************************************************************************************************/
    public static int memLockOwner=-1;    
}
