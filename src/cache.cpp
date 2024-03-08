//========================================================//
//  cache.cpp                                               //
//  Source file for the Cache Simulator                   //
//                                                        //
//  Implement the I-cache, D-Cache and L2-cache as        //
//  described in the README                               //
//========================================================//

#include "cache.hpp"
#include <string>
#include <math.h>
using namespace std;





//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//




//------------------------------------//
//          Cache Functions           //
//------------------------------------//



// Initialize the Cache Hierarchy
//
void init_cache()
{
  // Initialize cache stats
  icacheRefs = 0;
  icacheMisses = 0;
  icachePenalties = 0;
  dcacheRefs = 0;
  dcacheMisses = 0;
  dcachePenalties = 0;
  l2cacheRefs = 0;
  l2cacheMisses = 0;
  l2cachePenalties = 0;

  compulsory_miss = 0;
  other_miss = 0;
  prefetch = false;

}

// Clean Up the Cache Hierarchy
//
void clean_cache()
{
}

// Perform a memory access through the icache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
icache_access(uint32_t addr)
{

}

// Perform a memory access through the dcache interface for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
dcache_access(uint32_t addr)
{
  //
  // TODO: Implement D$
  //

  return memspeed;
}

// Perform a memory access to the l2cache for the address 'addr'
// Return the access time for the memory operation
//
uint32_t
l2cache_access(uint32_t addr)
{
  //
  // TODO: Implement L2$
  //
  return memspeed;
}

// Predict an address to prefetch on icache with the information of last icache access:
// 'pc':     Program Counter of the instruction of last icache access
// 'addr':   Accessed Address of last icache access
// 'r_or_w': Read/Write of last icache access
// 'policy': Prefetch Policy
uint32_t
icache_prefetch_addr(uint32_t pc, uint32_t addr, char r_or_w, PrefetchPolicy policy)
{
  if (policy == PrefetchPolicy::NEXT_LINE)
    return addr + icacheBlocksize; // Next line prefetching
  //
  // TODO: Implement a better prefetching strategy
  //
}

// Predict an address to prefetch on dcache with the information of last dcache access:
// 'pc':     Program Counter of the instruction of last dcache access
// 'addr':   Accessed Address of last dcache access
// 'r_or_w': Read/Write of last dcache access
// 'policy': Prefetch Policy
uint32_t
dcache_prefetch_addr(uint32_t pc, uint32_t addr, char r_or_w, PrefetchPolicy policy)
{
  return addr + dcacheBlocksize; // Next line prefetching
  //
  // TODO: Implement a better prefetching strategy
  //
}

// Perform a prefetch operation to I$ for the address 'addr'
void icache_prefetch(uint32_t addr)
{
  //
  // TODO: Implement I$ prefetch operation
  //
}

// Perform a prefetch operation to D$ for the address 'addr'
void dcache_prefetch(uint32_t addr)
{
  //
  // TODO: Implement D$ prefetch operation
  //
}

// Replace the cache line in the I$ for the address 'addr'
uint8_t icache_replace(uint32_t addr)
{
  //
  // TODO: Implement I$ replacement
  //
}

// Replace the cache line in the D$ for the address 'addr'
uint8_t dcache_replace(uint32_t addr)
{
  //
  // TODO: Implement D$ replacement
  //
}