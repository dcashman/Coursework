/*
 *
 * File: prefetcher.C
 * Author: Sat Garcia (sat@cs)
 * Description: This simple prefetcher waits until there is a D-cache miss then 
 * requests location (addr + 16), where addr is the address that just missed 
 * in the cache.
 *
 */

#include "prefetcher.h"
#include <stdio.h>

/* below structures are used as STATE */
static rpt_row_entry_t rpt_table[NUM_RPT_ENTRIES]; 
static int free_rpt_entries = NUM_RPT_ENTRIES;  //don't use random replacement if we haven't filled up yet

/* see if the pc is in the rpt_table.
   Return index if it is, -1 if not. */
static int checkInRPT(u_int32_t pc){
  int i;
  for(i = 0; i < NUM_RPT_ENTRIES; i++){
    if(rpt_table[i].pc == pc){
      return i;
    }
  }
  return -1;
}

Prefetcher::Prefetcher() { 
  int i;
  printf("PRE: creating prefetcher with value of (%d reqs per miss)\n", NUM_REQS_PER_MISS);
  printf("PRE: NUM ENTRIES IN TABLE: %d\n", NUM_RPT_ENTRIES);

  /* seed random number generator for replacement strat */
  srand(time(NULL));

  _ready = false; 
  /* clear to 0's the rpt table */
  for(i = 0; i < NUM_RPT_ENTRIES; i++){
    rpt_table[i].pc = 0;
    rpt_table[i].last_stride = 0;
    rpt_table[i].last_mem = 0;
  }

  
}

bool Prefetcher::hasRequest(u_int32_t cycle) { return _ready; }

Request Prefetcher::getRequest(u_int32_t cycle) { 
  return _nextReq; }

void Prefetcher::completeRequest(u_int32_t cycle) { 
  if(_req_left == 0){
    _ready = false; 
  }else{
    _req_left--;
    _nextReq.addr = _nextReq.addr + L2_BLOCK_SIZE;
  }
  
}

void Prefetcher::cpuRequest(Request req) { 
        int rpt_row_num, curr_stride;
	rpt_row_entry_t *curr_row;
 	if(!_ready && !req.HitL1) {
	       /* check entry in RPT table */
	  if((rpt_row_num = checkInRPT(req.pc)) >= 0){
	    //rpt_row_num = req.pc % NUM_RPT_ENTRIES; //OLD pre assoc
	       curr_row = &rpt_table[rpt_row_num];
	       //printf("PRE: rpt_row_num: %d, pc\t%x and curr_pc\t%x\n", rpt_row_num, req.pc, curr_row->pc);
	       //if(curr_row->pc == /*(short)*/req.pc){ //OLD pre assoc
		 //printf("DEBUG: req pc matches for the %dth time\n", counter++);
		 /* this entry is in the table */
		 if((curr_stride = req.addr - (curr_row->last_mem)) == curr_row->last_stride && curr_stride > WORTHWHILE_RPT){
		   /* if stride is the same as this one,
		      "punch-it" use it to prefetch */
		   printf("PRE: same stride found for address %x, with lastmem of %x and curr_req of %x, previous stride at %d\n",
		     req.pc, curr_row->last_mem, req.addr, curr_stride);
		   _nextReq.addr = req.addr + curr_stride;
		   _ready = true;
		   _req_left = NUM_REQS_PER_MISS - 1;  
		 }else{
		   /* update to new stride  and do standard prefetch*/
		   curr_row->last_stride = curr_stride; 
		      _nextReq.addr = req.addr + L2_BLOCK_SIZE;
		      _ready = true;
		      _req_left = NUM_REQS_PER_MISS - 1;   
		 } 
	  }else{
		 /* no pc, so add to assoc */
	    if(free_rpt_entries > 0){
	      rpt_row_num = NUM_RPT_ENTRIES - free_rpt_entries--;
	    }else{
	      rpt_row_num =rand() % NUM_RPT_ENTRIES;
	    }
	    rpt_table[rpt_row_num].last_stride = 0;
	    _nextReq.addr = req.addr + L2_BLOCK_SIZE;
	    _ready = true;
	    _req_left = NUM_REQS_PER_MISS - 1; 
	  }
	  rpt_table[rpt_row_num].pc = req.pc;
	  rpt_table[rpt_row_num].last_mem = req.addr;

		 } 

}
