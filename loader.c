/*
 * Loader Implementation
 *
 * 2022, Operating Systems
 */

#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/stat.h>

#include "exec_parser.h"

#define page_size getpagesize()

static so_exec_t *exec;

static int fd;

static struct sigaction old_action; /* Default handler */

static void segv_handler(int signum, siginfo_t *info, void *context)
{
	int i, page_index, seg_index, page_to_read = 0, offset = 0;
	int *data, paging;
	so_seg_t segment;

	/* Checking if the signal is SIGSEGV */
	if (signum != SIGSEGV) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	/* Calculating the segment which caused SIGSEGV */
	for (i = 0; i < exec->segments_no; i++) {
		if (exec->segments[i].mem_size + exec->segments[i].vaddr > (uintptr_t)info->si_addr)
			break;
	}

	seg_index = i;
	segment = exec->segments[seg_index];

	/* If the address of the segment which caused SIGSEGV is not found -> default handler */
	if (seg_index == exec->segments_no) {
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	/* Calculating the page fault index */
	page_index = ((uintptr_t)info->si_addr - segment.vaddr) / page_size;

	data = (int *) segment.data;

	/* If the page is already mapped in memory -> default handler */
	if(data[page_index] == 1) { 
		old_action.sa_sigaction(signum, info, context);
		return;
	}

	/* If it is not mapped, we map it in memory using *data */
	data[page_index] = 1;

	paging = page_index * page_size;

	offset = segment.offset + paging;

	page_to_read = segment.file_size - paging;

	/* Mapping the page */
	void *addr = mmap ((void *)(segment.vaddr + paging), page_size, PROT_WRITE,
			MAP_ANONYMOUS | MAP_FIXED | MAP_SHARED, -1, 0);

	/* Checking if the the mapping has worked */
	if (!addr)
		return;
	
	/* Finding the amount of data to read from page in memory */
	if (page_to_read > 0)
		page_to_read = ((page_size < page_to_read) ? page_size : page_to_read);
	else
		page_to_read = 0;
		
	if (page_to_read != 0) {
		lseek(fd, offset, SEEK_SET);
		read(fd, addr, page_to_read);
	}
	
	/* Setting permissions */
	if (mprotect(addr, page_size, segment.perm) == -1)
		return;
}

int so_init_loader(void)
{
	int rc;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = segv_handler;
	sa.sa_flags = SA_SIGINFO;
	rc = sigaction(SIGSEGV, &sa, NULL);
	if (rc < 0) {
		perror("sigaction");
		return -1;
	}
	return 0;
}

int so_execute(char *path, char *argv[])
{
	int pages;
	fd = open(path, O_RDONLY);
	if (fd < 0)
		return -1;

	exec = so_parse_exec(path);
	if (!exec)
		return -1;
	
	/* Calculating no of pages for every segment and allocating data */
	for (int i = 0; i < exec->segments_no; i++) {
		pages = exec->segments[i].mem_size / page_size;
		if(!exec->segments[i].data)
			exec->segments[i].data = malloc(pages * sizeof(int));
	}

	so_start_exec(exec, argv);

	for (int i = 0; i < exec->segments_no; i++) 
		free(exec->segments[i].data);
	
	free(exec->segments);
	free(exec);
	free(path);

	close(fd);

	return -1;
}
