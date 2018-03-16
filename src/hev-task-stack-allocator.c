/*
 ============================================================================
 Name        : hev-task-stack-allocator.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2018 everyone.
 Description :
 ============================================================================
 */

#define _GNU_SOURCE
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "hev-task-stack-allocator.h"
#include "hev-memory-allocator.h"

struct _HevTaskStackPage
{
	HevTaskStackPage *next;

	off_t offset;

	void *map_addr;
	HevTaskStackAllocator *owner;
};

struct _HevTaskStackAllocator
{
	int mem_fd;
	unsigned int page_size;
	unsigned int page_shift;

	off_t allocated_offset;
	HevTaskStackPage *free_pages;
};

#ifdef HEV_TASK_USE_SYS_MEMFD
# include <sys/memfd.h>
#else
# include <sys/syscall.h>

# define MFD_CLOEXEC 1U

static int
memfd_create (const char *name, unsigned int flags)
{
	return syscall (__NR_memfd_create, name, flags);
}
#endif

static int
memfd_tmpfile (const char *path)
{
	return open (path, O_TMPFILE | O_RDWR | O_CLOEXEC, S_IRUSR | S_IWUSR);
}

HevTaskStackAllocator *
hev_task_stack_allocator_new (void)
{
	HevTaskStackAllocator *self;
	unsigned int page_size;

	self = hev_malloc0 (sizeof (HevTaskStackAllocator));
	if (!self)
		return NULL;

	self->mem_fd = memfd_create ("shared-stack", MFD_CLOEXEC);
	if (-1 == self->mem_fd) {
		self->mem_fd = memfd_tmpfile ("/dev/shm");
		if (-1 == self->mem_fd) {
			self->mem_fd = memfd_tmpfile ("/tmp");
			if (-1 == self->mem_fd) {
				self->mem_fd = memfd_tmpfile (".");
				if (-1 == self->mem_fd) {
					hev_free (self);
					return NULL;
				}
			}
		}
	}

	self->page_size = sysconf (_SC_PAGESIZE);

	for (page_size=self->page_size; page_size>1; self->page_shift++)
		page_size >>= 1;

	return self;
}

void
hev_task_stack_allocator_destroy (HevTaskStackAllocator *self)
{
	HevTaskStackPage *iter = self->free_pages;

	while (iter) {
		HevTaskStackPage *next = iter->next;
		hev_free (iter);
		iter = next;
	}

	close (self->mem_fd);
	hev_free (self);
}

unsigned int
hev_task_stack_allocator_get_page_size (HevTaskStackAllocator *self)
{
	return self->page_size;
}

unsigned int
hev_task_stack_allocator_get_page_shift (HevTaskStackAllocator *self)
{
	return self->page_shift;
}

HevTaskStackPage *
hev_task_stack_allocator_alloc (HevTaskStackAllocator *self)
{
	HevTaskStackPage *page;
	off_t next_offset;

	if (self->free_pages) {
		page = self->free_pages;
		self->free_pages = page->next;

		return page;
	}

	page = hev_malloc (sizeof (HevTaskStackPage));
	if (!page)
		return NULL;

	page->owner = self;
	page->map_addr = NULL;

	next_offset = self->allocated_offset + self->page_size;
	if (ftruncate (self->mem_fd, next_offset) == -1) {
		hev_free (page);
		return NULL;
	}

	page->offset = self->allocated_offset;
	self->allocated_offset = next_offset;

	return page;
}

void
hev_task_stack_allocator_free (HevTaskStackAllocator *self, HevTaskStackPage *page)
{
	page->next = self->free_pages;
	self->free_pages = page;
}

int
hev_task_stack_page_map (HevTaskStackPage *page, void *addr)
{
	void *maddr;

	maddr = mmap (addr, page->owner->page_size, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_FIXED, page->owner->mem_fd, page->offset);
	if (maddr != addr)
		return -1;

	page->map_addr = addr;

	return 0;
}

int
hev_task_stack_page_unmap (HevTaskStackPage *page)
{
	return munmap (page->map_addr, page->owner->page_size);
}

int
hev_task_stack_page_remap (HevTaskStackPage *page)
{
	void *maddr;

	if (!page->map_addr)
		return -1;

	maddr = mmap (page->map_addr, page->owner->page_size, PROT_READ | PROT_WRITE,
				MAP_SHARED | MAP_FIXED, page->owner->mem_fd, page->offset);
	if (maddr != page->map_addr)
		return -1;

	return 0;
}

