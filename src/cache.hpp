#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <bitset>

using namespace std;

//------------------------------------//
//        Cache Configuration         //
//------------------------------------//
class CacheBase
{
public:
    virtual ~CacheBase() = default;
    // Perform a memory access through the cache interface for the address 'addr'
    // Return the access time for the memory operation
    virtual uint32_t cache_access(uint32_t addr) = 0;

    // Predict an address to prefetch on dcache with the information of last dcache access:
    // 'pc':     Program Counter of the instruction of last dcache access
    // 'addr':   Accessed Address of last dcache access
    // 'r_or_w': Read/Write of last dcache access
    virtual uint32_t cache_prefetch_addr(uint32_t pc, uint32_t addr, bool rw) = 0;

    // Perform a prefetch operation to I$ for the address 'addr'
    virtual void cache_prefetch(uint32_t addr, PrefetchPolicy policy) = 0;

    // 最多8路组相联的替换策略
    virtual uint8_t cache_replace(uint32_t addr, ReplacePolicy policy) = 0;

protected:
    uint32_t sets;
    uint32_t assoc;
    uint32_t blockSize;
    uint32_t hitTime;
    CacheType type;

    // statistics
    uint64_t refs;            // $ references
    uint64_t misses;          // $ misses
    uint64_t penalties;       // $ penalties
    uint64_t compulsory_miss; // Compulsory misses on all caches
    uint64_t other_miss;      // Other misses (Conflict / Capacity miss) on all caches
};


class L1_ICache : public CacheBase
{
public:
    L1_ICache(uint32_t sets, uint32_t assoc, uint32_t blockSize, uint32_t hitTime, CacheType type) :
    data(sets, vector<uint32_t>(assoc, 0)) {
        this->sets = sets;
        this->assoc = assoc;
        this->blockSize = blockSize;
        this->hitTime = hitTime;
        this->type = type;

        // statistics
        this->refs = 0;
        this->misses = 0;
        this->penalties = 0;
        this->compulsory_miss = 0;
        this->other_miss = 0;
    }
    ~L1_ICache() {
        for (int i = 0; i < sets; i++) {
            data[i].clear();
        }
        data.clear();
    }

    uint32_t cache_access(uint32_t addr) override {
        return 0;
    }
    uint32_t cache_prefetch_addr(uint32_t pc, uint32_t addr, bool rw) override {
        return 0;
    }
    void cache_prefetch(uint32_t addr, PrefetchPolicy policy) override {

    }
    uint8_t cache_replace(uint32_t addr, ReplacePolicy policy) override {
        return 0;
    }

private:
    vector<vector<uint32_t>> data;
};


enum class CacheType
{
    L1_ICACHE,
    L1_DCACHE,
    L2_CACHE
};

enum class ReplacePolicy
{
    LRU,
    FIFO,
    PLRU
};

enum class PrefetchPolicy
{
    NEXT_LINE,
    STRIDE,
    STREAM
};
