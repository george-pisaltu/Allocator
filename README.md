# Allocator
Memory allocator in C
Commands:

INITIALIZE x  -  begins managing a chunk of x bytes

ALLOC x  -  allocates x bytes of the chunk
	    prints the start index of the data section

FREE x  -  frees a previously allocated block
	   x must be a number previously printed by ALLOC

DUMP  -  dumps the whole chunk of memory with all values, in hex

FILL I N X  - writes the value X in N consecutive bytes
	      starting with the byte on position I

SHOW FREE  -  prints the number of free blocks and the number
	      of free bytes

SHOW USAGE  -  prints the number of allocated blocks and the
	       number of used bytes along with the efficiency
	       and fragmentation of memory usage

SHOW ALLOCATIONS  -  prints all blocks, free or used, along with
		     the size of each block in bytes

FINALIZE  -  frees the whole chunk and terminates the program


Notes:
FILL and FREE assume the input is correct
FILL overwrites the old values at the memory location
DUMP prints 16 bytes in hex on each line, with an
	additional space in the middle of each line
