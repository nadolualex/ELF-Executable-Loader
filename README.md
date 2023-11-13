# ELF Executable Loader

## Overview
This repository contains a C implementation of an ELF Executable Loader as part of the Operating Systems homework (Homework no. 1). The loader is designed to handle SIGSEGV signals and implement demand paging for ELF executables.

## Implementation Details
The primary focus of the implementation is on the `segv_handler` function, which serves as the signal handler for SIGSEGV. Here's an overview of the key steps:

1. **Signal Handling:** Check if the `signum` parameter of the handler is not SIGSEGV and treat it with the default one using `struct sigaction`.

2. **Identifying Faulty Segment:** Determine the segment that caused SIGSEGV by extracting the address at which the fault occurred (`info->si_addr`).

3. **Handling Page Faults:**
    - Case 1: If the address of the faulty segment is not found or if it's a non-permitted memory access, use the default handler.
    - Case 2: If the page address is not mapped in memory, map it using the `mmap` function.

4. **Data Storage:** Use the `*data` field from the `so_seg_t` structure to create an array. Mapped pages are filled with 1, and unmapped pages with 0.

5. **File Handling:** For unmapped pages, open the file and calculate the amount of data to read from the page. For the `.bss` segment, fill the rest of the page with 0 if `mem_size` is larger than `file_size`.

6. **Mapping Pages:** Map the page using `mmap`, and add an extra field for mapping using `MAP_ANONYMOUS`. Initialize the contents with zero and set permissions for the mapped ones.

7. **Memory Allocation:** Allocate memory for the `*data` array in the execution function of the binary to prevent potential loss.

## Reflection
While the provided homework statement and the associated picture were helpful in understanding page_index and the segment causing SIGSEGV, additional insights were gained through online resources, particularly YouTube. The ability to search and access information online, especially from the programming community, greatly facilitated the understanding of low-level implementation details. The contributions from individuals sharing knowledge on platforms like YouTube were instrumental in grasping the concepts related to signal handling and implementation of handlers in a C environment.
