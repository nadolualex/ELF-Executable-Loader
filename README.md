# ELF-Executable-Loader

NADOLU ROBERT ALEXANDRU, 324CB
=============== E.L.F. Executable Loader File C Implementation ========================

  Operation Systems Homework no. 1

First step was to check if signum parameter of the handler is not SIGSEGV and treat it with the default one (struct sigaction). 
I didn't consider writing extra functions, as the handler could've been fully implemented just in the segv_handler function. 
Next, I found out which was the segment that caused SIGSEGV by getting the address at which fault ocurred  (info->si_addr). 
The statement presents three big cases to implement demand paging method when catching page faults:
  1) the address of the segment which caused SIGSEGV is not found -> default handler
  2) the page is already mapped in memory and it is a non-permitted memory access -> default handler
  3) the page address is not mapped in memory -> I start mapping it with mmap function

I easily treat the first two cases in my code so there is little to talk about there, but for the third case I use the *data 
field from the so_seg_t structure to create an array, as we are required to. I fill the mapped pages with 1 and the unmapped 
pages with 0. For every *data element stored with 1, the default handler is called because it is an invalid acces to memory. 
For the unmapped pages, I start by opening the file ( in the so_execute function before the executable file starts ) and 
calculating the amount of data to read from the page. For the .bss segment, in which the mem_size is larger than file_size, 
I fill the rest of the page with 0. Then I map the page, and also I add an extra field in the mapping function ( MAP_ANONYMOUS )
which initializes its contents with zero and the I set permissions for the mapped ones. The memory allocation for the *data array 
is made in the execution function of the binary to prevent it from being lost (in case the allocation was made in the handler
function). 
	Overall, the picture in the homework statement helped me to figure out how to determine the page_index, the segment which 
caused SIGSEGV but nothing of this statement compares to YOUTUBE. Searching went pretty well and it helped me to understand what the 
handler actually is and how it should be implemented, indians are almighty when it comes to such low-level stuff. 
