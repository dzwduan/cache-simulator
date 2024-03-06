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
//        Cache Configuration         //
//------------------------------------//

uint32_t icacheSets;      // Number of sets in the I$
uint32_t icacheAssoc;     // Associativity of the I$
uint32_t icacheBlocksize; // Blocksize of the I$
uint32_t icacheHitTime;   // Hit Time of the I$

uint32_t dcacheSets;      // Number of sets in the D$
uint32_t dcacheAssoc;     // Associativity of the D$
uint32_t dcacheBlocksize; // Blocksize of the D$
uint32_t dcacheHitTime;   // Hit Time of the D$

uint32_t l2cacheSets;      // Number of sets in the L2$
uint32_t l2cacheAssoc;     // Associativity of the L2$
uint32_t l2cacheBlocksize; // Blocksize of the L2$
uint32_t l2cacheHitTime;   // Hit Time of the L2$
uint32_t inclusive;        // Indicates if the L2 is inclusive

bool prefetch; // Indicate if prefetching is enabled

uint32_t memspeed; // Latency of Main Memory

//------------------------------------//
//          Cache Statistics          //
//------------------------------------//

uint64_t icacheRefs;      // I$ references
uint64_t icacheMisses;    // I$ misses
uint64_t icachePenalties; // I$ penalties

uint64_t dcacheRefs;      // D$ references
uint64_t dcacheMisses;    // D$ misses
uint64_t dcachePenalties; // D$ penalties

uint64_t l2cacheRefs;      // L2$ references
uint64_t l2cacheMisses;    // L2$ misses
uint64_t l2cachePenalties; // L2$ penalties

uint64_t compulsory_miss; // Compulsory misses on all caches
uint64_t other_miss;      // Other misses (Conflict / Capacity miss) on all caches

//------------------------------------//
//        Cache Data Structures       //
//------------------------------------//
class metaData
{
public:
  uint32_t tag;
  bool valid;
  bool dirty;

  metaData()
  {
    tag = 0;
    valid = 0;
    dirty = 0;
  }
};

class Cache
{
public:
  uint32_t sets;
  uint32_t assoc;
  uint32_t blocksize;
  uint32_t hitTime;
  uint32_t missTime;
  uint32_t memspeed;
  uint32_t refs;
  uint32_t misses;
  uint32_t penalties;
  uint32_t compulsory_miss;
  uint32_t other_miss;
  class metaData *meta;
  uint32_t *data;
  bool prefetch;
  string name;

public:
  Cache(string _name, uint32_t _sets, uint32_t _assoc, uint32_t _blocksize, uint32_t _hitTime, uint32_t _missTime, uint32_t _memspeed, bool _prefetch)
  {
    name = _name;
    sets = _sets;
    assoc = _assoc;
    blocksize = _blocksize;
    hitTime = _hitTime;
    missTime = _missTime;
    memspeed = _memspeed;
    refs = 0;
    misses = 0;
    penalties = 0;
    compulsory_miss = 0;
    other_miss = 0;
    prefetch = _prefetch;
    meta = new metaData[sets * assoc];
    data = new uint32_t[sets * assoc * blocksize];

    meta->dirty = false;
    meta->tag = 0;
    meta->valid = false;
    data = 0;
  }

  ~Cache()
  {
    delete[] meta;
    delete[] data;
  }

  uint32_t get_tag(uint32_t addr)
  {
    return addr >> (uint32_t)(log2(blocksize) + log2(sets));
  }

  uint32_t get_index(uint32_t addr)
  {
    return (addr >> (uint32_t)log2(blocksize)) & (sets - 1);
  }

  uint32_t get_offset(uint32_t addr)
  {
    return addr & (blocksize - 1);
  }

  bool get_valid(uint32_t index, uint32_t way)
  {
    return meta[index * assoc + way].valid;
  }

  bool get_way(uint32_t index, uint32_t tag)
  {
    for (uint32_t way = 0; way < assoc; way++)
    {
      if (meta[index * assoc + way].tag == tag && meta[index * assoc + way].valid)
      {
        return way;
      }
    }
    return -1;
  }

  bool enable_prefetch()
  {
    return prefetch;
  }

  void update_status() {

  }

  uint32_t replace(ReplacePolicy policy, uint32_t addr) {

  }
};

class VictimCache {

};

//------------------------------------//
//          Cache Functions           //
//------------------------------------//

Cache *l1_icache;
Cache *l1_dcache;
Cache *l2_cache;

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

  l1_icache = new Cache("L1 ICache", icacheSets, icacheAssoc, icacheBlocksize, icacheHitTime, memspeed, memspeed, prefetch);
  l1_dcache = new Cache("L1 DCache", dcacheSets, dcacheAssoc, dcacheBlocksize, dcacheHitTime, memspeed, memspeed, prefetch);
  l2_cache  = new Cache("L2 Cache", l2cacheSets, l2cacheAssoc, l2cacheBlocksize, l2cacheHitTime, memspeed, memspeed, prefetch);
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
  //
  // TODO: Implement I$
  // TODO: how to get compulsory misses?
  //
  l1_icache->refs++;

  if (!l1_icache->enable_prefetch()) {
    uint32_t index  = l1_icache->get_index(addr);
    uint32_t tag    = l1_icache->get_tag(addr);
    uint32_t offset = l1_icache->get_offset(addr);
    uint32_t way    = l1_icache->get_way(index, tag);   

    if (way == -1)
    {
      l1_icache->misses++;
      l1_icache->missTime = l2cache_access(addr);
      l1_icache->penalties += l1_icache->missTime;
      l1_icache->other_miss++;
      l1_icache->meta[index * icacheAssoc].tag = tag;
      l1_icache->meta[index * icacheAssoc].valid = true;
      l1_icache->data[index * icacheAssoc * icacheBlocksize] = addr;
      return l1_icache->missTime;
    }
    else
    {
      l1_icache->refs++;
      return l1_icache->hitTime;
    }
  }

    return l1_icache->penalties * (l1_icache->misses/l1_icache->refs) + l1_icache->hitTime ;
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
  if (!l1_dcache->enable_prefetch()) {

  }
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
uint32_t icache_replace(uint32_t addr)
{

}

// Replace the cache line in the D$ for the address 'addr'
uint32_t dcache_replace(uint32_t addr)
{

}