# Cache Simulator


## Prefetch design
### Instruction prefetch

- [ ] Next-Line Prefetching
- [ ] Fetch-Directed Prefetching
- [ ] Discontinuity Prefetching
- [ ] Prescient Fetch
- [ ] Temporal Instruction Fetch Streaming
- [ ] Return-Address Stack-Directed Instruction Prefetching
- [ ] Proactive Instruction Fetch

### Data prefetch

- [ ] Stride Prefetcher
- [ ] Stream Prefetcher

...


## Testing
Once you have created the binary, you can run it with the following command:
`bunzip2 -kc trace.bz2 | ./cache <options>`
The options are as follows:
```
  --help                     Print this message
  --icache=sets:assoc:blocksize:hit    I-cache Parameters
  --dcache=sets:assoc:blocksize:hit    D-cache Parameters
  --l2cache=sets:assoc:blocksize:hit   L2-cache Parameters
  --inclusive                Makes L2-cache be inclusive
  --prefetch                 Enable Prefetching
  --memspeed=latency         Latency to Main Memory
```

The testing can be done using the traces given to you in the repository. There are 2 correct outputs given for 2 configurations as follows:
1. **MIPS R10K** - [Reference Manual](https://ieeexplore.ieee.org/abstract/document/491460?casa_token=xRyemPMXCU4AAAAA:qMm86PcKveY_y6TAegQChllzSccO4b6ILZRKKEeO_ml4HjQfav6hBbHDJeHR0TeXZCUPyjOpFQ):
   * I$: 32KB, 2-way, 2 cycles hit latency
   * D$: 32KB, 4-way, 2 cycles hit latency
   * L2: 128KB, 8-way, off-chip, 50 cycles hit latency
   * 128B block size
   * `./cache --icache=128:2:128:2 --dcache=64:4:128:2 --l2cache=128:8:128:50 --memspeed=100`
2. **Alpha A21264** - [Reference Manual](https://course.ece.cmu.edu/~ece447/s15/lib/exe/fetch.php?media=21264hrm.pdf):
   * I$: 64KB, 2-way, 2 cycles hit latency
   * D$: 64KB, 4-way, 2 cycles hit latency
   * L2: 8MB, 8-way, off-chip, 50 cycles hit latency
   * 64B block size
   * `./cache --icache=512:2:64:2 --dcache=256:4:64:2 --l2cache=16384:8:64:50 --memspeed=100`

3. **Open Xiangshan** - [Reference Manual](https://xiangshan-doc.readthedocs.io/zh_CN/latest/): 
   * TODO



## Refer
1. CSE240A infrastructure
1. [CacheMemory](https://yuhaozhu.com/CacheMemory.pdf)
1. [A Primer on Memory Consistency and Cache Coherence](https://course.ece.cmu.edu/~ece847c/S15/lib/exe/fetch.php?media=part2_2_sorin12.pdf)
1. [A Primer on Hardware Prefetching](https://picture.iczhiku.com/resource/eetop/WHiEgafHDzheKCNv.pdf)
1. [The 3rd Data Prefetching Championship](https://dpc3.compas.cs.stonybrook.edu/)
1. Computer Architecture: A Quantitative Approach (Sixth Edition) ch2
