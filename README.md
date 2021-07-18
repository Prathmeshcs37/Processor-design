# Processor-design
Finished design for a pipelined processor in C++. Runs a cache based memory system 
and a 2-way pipeline processor issuing two instructions at a time.

Consider a scalar pipelined processor that has a 256B instruction cache (I$) and a 256B data cache (D$),
both having a read port and a write port each and both are direct-mapped caches (the block size is 4B).
Assume that both instruction and data caches are perfect, meaning there won’t be any cache misses in
these caches. Assume that the processor has a register file (RF) with sixteen, 8-bit registers, named R0,
..., R15. Note that R0 always stores the value “0”. The register file has two read ports and a write port.
