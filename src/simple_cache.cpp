#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <bitset>
#include <tuple>
#include <bit>
#include <ranges>
#include <span>
#include <cassert>
using namespace std;

/**
 * trace format : # 0 7fffed80 1      0 for read, 1 for write, the last num means the number of instructions executed between the previous memory access and this on
 *
 *          X-Bits        Y-Bits        Z-Bits
 *  ****** TAG ******|**** SET ****|** OFFSET **|
 */

class CacheSim
{
public:
  CacheSim(std::string input, unsigned block_sz, unsigned asso, unsigned capacity, unsigned miss_penalty, unsigned dirty_wb_penalty)
      : block_size(block_sz), associativity(asso), capacity(capacity), miss_penalty(miss_penalty), dirty_wb_penalty(dirty_wb_penalty)
  {

    auto num_blocks = capacity / block_size;
    auto sets = capacity / (block_size * associativity);

    infile.open(input);

    tags.resize(num_blocks);
    dirty.resize(num_blocks);
    valid.resize(num_blocks);
    priority.resize(num_blocks);

    auto set_offset = std::popcount(block_size - 1); // bits of Z
    set_mask = sets - 1;
    auto set_bits = std::popcount(set_mask); // bits of Y
    tag_offset = set_bits + set_offset;
  }

  ~CacheSim()
  {
    infile.close();
    dump_stats();
  }

  void run()
  {
    string line;
    while (getline(infile, line))
    {
      auto [type, addr, insts] = parse_line(line);
      auto [hit, dirty_wb] = probe(type, addr);
      // Update the cache statistics
      update_statistics(insts, type, hit, dirty_wb);
    }
  }

  tuple<bool, uint64_t, int> parse_line(string access)
  {
    int type;
    uint64_t addr;
    int insts;
    sscanf(access.c_str(), "# %d %lx %d", &type, &addr, &insts);
    return std::make_tuple(!!type, addr, insts);
  }

  int get_set(uint64_t addr)
  {
    return (addr >> set_offset) & set_mask;
  }

  uint64_t get_tag(uint64_t addr)
  {
    return (addr >> tag_offset);
  }

  /*
   * @brief simulate the actual cache access
   */
  tuple<bool, bool> probe(bool type, uint64_t addr)
  {
    auto set = get_set(addr);
    auto tag = get_tag(addr);

    auto base = set * associativity;

    // current set responding to the access
    std::span<uint32_t>   local_tags{tags.data() + base, associativity};
    std::span<bitset<1>>  local_dirty{dirty.data() + base, associativity};
    std::span<bitset<1>>  local_valid{valid.data() + base, associativity};
    std::span<uint8_t>    local_priority{priority.data() + base, associativity};

        // Check each cache line in the set
    auto hit = false;
    int invalid_index = -1;
    int index;
    for (auto i = 0u; i < local_valid.size(); i++) {
      // Check if the block is invalid
      if (!local_valid[i][0]) {
        // Keep track of invalid entries in case we need them
        invalid_index = i;
        continue;
      }

      // Check if the tag matches
      if (tag != local_tags[i]) continue;

      // We found the line, so mark it as a hit
      hit = true;
      index = i;

      // Update dirty flag
      local_dirty[index] |= type;

      // Break out of the loop
      break;
    }

    // Find an element to replace if it wasn't a hit
    auto dirty_wb = false;
    if (!hit) {
      // First try and use an invalid line (if available)
      if (invalid_index >= 0) {
        index = invalid_index;
        local_valid[index] = 1;
      }
      // Otherwise, evict the lowest-priority cache block (largest value)
      else {
        auto max_element = std::ranges::max_element(local_priority);
        index = std::distance(begin(local_priority), max_element);
        dirty_wb = local_dirty[index][0];
      }

      // Update the tag and dirty state
      local_tags[index] = tag;
      local_dirty[index] = type;
    }

    // Update the priority
    // Go through each element
    // Increase the priority of all the blocks with a lower priority than the
    // one we are accessing
    // High priority -> Low priority = 0 -> associativity - 1
    std::transform(begin(local_priority), end(local_priority),
                   begin(local_priority), [&](int p) {
                     if (p <= local_priority[index] && p < associativity)
                       return p + 1;
                     else
                       return p;
                   });

    // Currently accessed block has the highest priority (0)
    local_priority[index] = 0;

    return {hit, dirty_wb};
  }

  void update_statistics(int insts, bool type, bool hit, bool dirty_wb)
  {
    mem_refs_++;
    writes_ += type;
    misses_ += !hit;
    dirty_wb_ += dirty_wb;
    inst_nums_ += insts;
  }

  // Dump the statistics from simulation
  void dump_stats()
  {
    // Print the cache settings
    std::cout << "CACHE SETTINGS\n";
    std::cout << "       Cache Size (Bytes): " << capacity << '\n';
    std::cout << "           Associativity : " << associativity << '\n';
    std::cout << "       Block Size (Bytes): " << block_size << '\n';
    std::cout << "    Miss Penalty (Cycles): " << miss_penalty << '\n';
    std::cout << "Dirty WB Penalty (Cycles): " << dirty_wb_penalty << '\n';
    std::cout << '\n';

    // Print the access breakdown
    std::cout << "CACHE ACCESS STATS\n";
    std::cout << "TOTAL ACCESSES: " << mem_refs_ << '\n';
    std::cout << "         READS: " << mem_refs_ - writes_ << '\n';
    std::cout << "        WRITES: " << writes_ << '\n';
    std::cout << '\n';

    // Print the miss-rate breakdown
    std::cout << "CACHE MISS-RATE STATS\n";
    double miss_rate = (double)misses_ / (double)mem_refs_ * 100.0;
    auto hits = mem_refs_ - misses_;
    std::cout << "     MISS-RATE: " << miss_rate << "%" << '\n';
    std::cout << "        MISSES: " << misses_ << '\n';
    std::cout << "          HITS: " << hits << '\n';
    std::cout << '\n';

    // Print the instruction breakdown
    std::cout << "CACHE IPC STATS\n";
    auto cycles = miss_penalty * misses_;
    cycles += dirty_wb_penalty * dirty_wb_;
    cycles += inst_nums_;
    double ipc = (double)inst_nums_ / (double)cycles;
    std::cout << "           IPC: " << ipc << '\n';
    std::cout << "  INSTRUCTIONS: " << inst_nums_ << '\n';
    std::cout << "        CYCLES: " << cycles << '\n';
    std::cout << "      DIRTY WB: " << dirty_wb_ << '\n';
  }

private:
  std::ifstream infile;
  // configurable parameters
  unsigned block_size;
  unsigned associativity;
  unsigned capacity;
  unsigned miss_penalty;
  unsigned dirty_wb_penalty;
  // offset and mask
  unsigned set_offset;
  unsigned tag_offset;
  unsigned set_mask;
  // status
  vector<uint32_t> tags;
  vector<bitset<1>> dirty; // because span can't be used with bool, so use bitset
  vector<bitset<1>> valid;
  vector<uint8_t> priority;
  // statistics info
  uint64_t writes_ = 0;
  uint64_t mem_refs_ = 0;
  uint64_t misses_ = 0;
  uint64_t dirty_wb_ = 0;
  uint64_t inst_nums_ = 0;
};

int main(int argc, char *argv[])
{

  // Hard coded cache settings
  unsigned block_size = 1 << 7;
  unsigned associativity = 1 << 2;
  unsigned capacity = 1 << 15;
  unsigned miss_penalty = 30;
  unsigned dirty_wb_penalty = 5;

  // Create our simulator
  CacheSim simulator(argv[1], block_size, associativity, capacity,
                     miss_penalty, dirty_wb_penalty);
  simulator.run();

  return 0;
}

// refer https://coffeebeforearch.github.io/2020/12/16/cache-simulator.html