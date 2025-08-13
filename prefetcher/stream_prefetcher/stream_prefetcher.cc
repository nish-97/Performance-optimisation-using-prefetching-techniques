#include <iostream>
#include <array>
#include <map>

#include "cache.h"

#include "meamviz.h"
extern MeamViz logg;


constexpr int PREFETCH_DEGREE = 2;
constexpr int STREAM_DEPTH = 64;
constexpr int PREFETCH_DISTANCE = 10;
constexpr int FIXED_DISTANCE = 5;


struct tracker_entry {
//uint64_t last_cl_addr = 0; // the last address accessed in the stream
int stream_state = 0; 
uint64_t stream_start_addr = 0; 
uint64_t stream_end_addr = 0; 
uint64_t last_used_cycle = 0;
};

struct lookahead_entry {
  uint64_t address = 0;
  int degree = 0; // degree remaining
  int dir =0;
};

//int last_filled_entry= 0;
std::map<CACHE*, lookahead_entry> lookahead;
std::map<CACHE*, std::array<tracker_entry, STREAM_DEPTH>> stream_trackers;


void CACHE::prefetcher_initialize() { std::cout << NAME << " Stream prefetcher" << std::endl;
}

void CACHE::prefetcher_cycle_operate() {
// If a stream is active
//cout<<"1"<<endl;
if (auto [cl_addr, degree,dir] = lookahead[this]; degree > 0){
  //cout<<dir<<endl;
auto pf_addr = ((cl_addr + dir*1) << LOG2_BLOCK_SIZE);
cl_addr=cl_addr<<LOG2_BLOCK_SIZE;
// Check if the next step would run off the page, stop
if (virtual_prefetch || (pf_addr >> LOG2_PAGE_SIZE) == (cl_addr >> LOG2_PAGE_SIZE)) {
// Check the MSHR occupancy to decide if we're going to prefetch to this level or not
//cout<<pf_addr<<endl;
bool success = prefetch_line(0, 0, pf_addr, (get_occupancy(0, pf_addr) < get_size(0, pf_addr) / 2), 0);
if (success) {
  logg.logCachePrefetchEvent(current_cycle, NAME, (pf_addr >> LOG2_BLOCK_SIZE));
lookahead[this] = {pf_addr>>LOG2_BLOCK_SIZE,degree-1,dir};
//cout<<"1"<<endl;
        }
// If we fail, try again next cycle
          }
 else {
lookahead[this] = {};
    }
  }


}


void printTrackerEntry(const tracker_entry & entry,uint64_t claddr) {
  std::cout << "{" << std::endl;
  std::cout << entry.stream_state << std::endl;
  std::cout << entry.stream_start_addr << std::endl;
  std::cout << entry.stream_end_addr << std::endl;
  std::cout << entry.last_used_cycle << std::endl;
  std::cout << claddr << std::endl;
  std::cout << "}" << std::endl;
}

uint32_t CACHE::prefetcher_cache_operate(uint64_t addr, uint64_t ip, uint8_t cache_hit, uint8_t type, uint32_t metadata_in) {
logg.logCacheOperateEvent(
  current_cycle,
  NAME,
  (addr >> LOG2_BLOCK_SIZE), ip, cache_hit, type
);

uint64_t cl_addr = addr >> LOG2_BLOCK_SIZE;

auto set_begin = stream_trackers[this].begin();
// auto set_end = std::next(set_begin, last_filled_entry+1);
auto set_end = stream_trackers[this].end();

int stream_state = 0;
uint64_t start_addr=0,end_addr=0,curr=current_cycle;
  // find the current ip within the set
  // auto found_pos = std::find_if(set_begin, set_end, [cl_addr](tracker_entry x) { return ((x.stream_state == 1) && ((cl_addr > x.stream_end_addr ) && (cl_addr > x.stream_start_addr )  && ((cl_addr - x.stream_start_addr) <= (PREFETCH_DISTANCE)))); });
  // auto found_neg = std::find_if(set_begin, set_end, [cl_addr](tracker_entry x) { return ((x.stream_state == 0) && ((cl_addr < x.stream_end_addr ) && (cl_addr < x.stream_start_addr ) && ((x.stream_start_addr - cl_addr) <= (PREFETCH_DISTANCE)))); });
auto found_pos = std::find_if(set_begin, set_end, [cl_addr](tracker_entry x) { 
   // printTrackerEntry(x);
  return ((x.stream_state == 1) && (cl_addr > x.stream_start_addr ) && (cl_addr < x.stream_end_addr )); });

auto found_neg = std::find_if(set_begin, set_end, [cl_addr](tracker_entry x) { 
  return ((x.stream_state == 2) && (cl_addr > x.stream_end_addr ) && (cl_addr < x.stream_start_addr )); });
  
auto found_one_miss = std::find_if(set_begin, set_end, [cl_addr](tracker_entry x) { 
    //printTrackerEntry(x);
  return ((x.stream_state == 3 || x.stream_state == 4)  && (x.stream_end_addr != x.stream_start_addr) && ((cl_addr>x.stream_end_addr && cl_addr>x.stream_start_addr && x.stream_state == 3 && ((cl_addr-x.stream_start_addr) < PREFETCH_DISTANCE)) || (cl_addr<x.stream_end_addr && cl_addr<x.stream_start_addr && x.stream_state == 4 && ((x.stream_start_addr-cl_addr) < PREFETCH_DISTANCE)))); 
    });

auto found_stream_start = std::find_if(set_begin, set_end, [cl_addr,curr](tracker_entry x) { 
  return ((x.stream_state == 0)  && (x.stream_end_addr == x.stream_start_addr) && (curr != x.last_used_cycle) &&  (cl_addr != x.stream_start_addr) && (x.stream_end_addr != 0)  && (((cl_addr - x.stream_start_addr) < ((uint64_t)PREFETCH_DISTANCE)) || ((x.stream_start_addr - cl_addr) < ((uint64_t)PREFETCH_DISTANCE)))); });
  //cout<<found_neg->stream_state<<endl;
  
  
  if(found_pos != set_end){ 
    logg.generalEvent(current_cycle, NAME, (addr >> LOG2_BLOCK_SIZE),"POSITIVE");
    //cout<<"if1";
    //cout<<(found_pos->stream_end_addr)<<endl;
    //end_addr = found_pos->stream_end_addr;
    //end_addr=cl_addr;
    lookahead[this]={((found_pos->stream_end_addr)), PREFETCH_DEGREE,1};
    // found_pos->stream_end_addr = cl_addr;
  //   if(((found_pos->stream_end_addr + (uint64_t)PREFETCH_DEGREE) - found_pos->stream_start_addr) < ((uint64_t)PREFETCH_DISTANCE))
  //       found_pos->stream_end_addr+= (uint64_t)PREFETCH_DEGREE;
  //   else{
  //     found_pos->stream_start_addr+= (uint64_t)PREFETCH_DEGREE;
  //     found_pos->stream_end_addr+= (uint64_t)PREFETCH_DEGREE;   
  //  }

   if(((found_pos->stream_end_addr + (uint64_t)PREFETCH_DEGREE) - found_pos->stream_start_addr) < ((uint64_t)PREFETCH_DISTANCE))
        found_pos->stream_end_addr+= (uint64_t)PREFETCH_DEGREE;
    else{
      found_pos->stream_start_addr+= (uint64_t)PREFETCH_DEGREE;
      found_pos->stream_end_addr+= (uint64_t)PREFETCH_DEGREE;   
   }
   //printTrackerEntry(*found_pos);
   *found_pos={found_pos->stream_state,found_pos->stream_start_addr,found_pos->stream_end_addr,current_cycle };
  }

  else if(found_neg != set_end){
    logg.generalEvent(current_cycle, NAME, (addr >> LOG2_BLOCK_SIZE),"NEGATIVE");
      end_addr = found_neg->stream_end_addr ;
     //cout<<"if2";
      lookahead[this]={((found_neg->stream_end_addr)), PREFETCH_DEGREE,-1};
    // found_pos->stream_end_addr = cl_addr;
    if((found_neg->stream_start_addr - (found_neg->stream_end_addr - (uint64_t)PREFETCH_DEGREE)) < (uint64_t)PREFETCH_DISTANCE)
        found_neg->stream_end_addr-=(uint64_t)PREFETCH_DEGREE;   
    else{
      found_neg->stream_start_addr-=(uint64_t)PREFETCH_DEGREE;
      found_neg->stream_end_addr-=(uint64_t)PREFETCH_DEGREE;   
    }
   //printTrackerEntry(*found_neg);
    *found_neg={found_neg->stream_state,found_neg->stream_start_addr, found_neg->stream_end_addr,current_cycle };
  }

  else if(found_one_miss != set_end){
    //cout<<"if3"<<endl;
  if(cl_addr<found_one_miss->stream_end_addr){
    //cout<<"i am inside this if31";
    found_one_miss->stream_state=2;
    found_one_miss->stream_end_addr=cl_addr - FIXED_DISTANCE;
    }
  else {
    //cout<<"i am inside this if32";
    found_one_miss->stream_state=1;
    found_one_miss->stream_end_addr=cl_addr + FIXED_DISTANCE;
    }
    //printTrackerEntry(*found_one_miss,cl_addr);
  *found_one_miss={found_one_miss->stream_state,found_one_miss->stream_start_addr, found_one_miss->stream_end_addr ,current_cycle};
  } 
  
  else if(found_stream_start != set_end){
    //cout<<"if4"<<endl;
    if(cl_addr > found_stream_start->stream_start_addr){
      stream_state=3;
      }  
    else if(cl_addr < found_stream_start->stream_start_addr){
      stream_state=4;
      }
     found_stream_start->stream_end_addr = cl_addr;    
    *found_stream_start={stream_state,found_stream_start->stream_start_addr,found_stream_start->stream_end_addr,current_cycle };
    //printTrackerEntry(*found_stream_start,cl_addr);
  }

  else{  
    //cout<<"if5"<<endl;
    // int count = std::distance(set_begin,set_end);
    // cout<<count<<endl;
    stream_state=0;
    //if(last_filled_entry == STREAM_DEPTH + 1) {   
    auto found = std::min_element(set_begin, set_end, [](tracker_entry x, tracker_entry y) { return x.last_used_cycle < y.last_used_cycle; });   
    *found={stream_state,cl_addr,cl_addr,current_cycle};
    //printTrackerEntry(*found,cl_addr);
  }

return metadata_in;
}

uint32_t CACHE::prefetcher_cache_fill(uint64_t addr, uint32_t set, uint32_t way, uint8_t prefetch, uint64_t evicted_addr, uint32_t metadata_in) {
return metadata_in;
}

void CACHE::prefetcher_final_stats() {}