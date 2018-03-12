/*
 ============================================================================
 Name        : hev-task-stack-fault-handler.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2018 everyone.
 Description :
 ============================================================================
 */

#include <signal.h>

#ifdef ENABLE_PTHREAD
# include <pthread.h>
#endif

#include "hev-task-stack-fault-handler.h"
#include "hev-task-stack-allocator.h"
#include "hev-task-private.h"
#include "hev-task-system-private.h"
#include "hev-memory-allocator.h"

typedef struct _HevTaskStackFaultHandlerStackInfo HevTaskStackFaultHandlerStackInfo;

struct _HevTaskStackFaultHandlerStackInfo
{
	stack_t new;
	stack_t old;
};

static void signal_handler (int signo, siginfo_t *info, void *context);

static int initialized;
static void (*default_signal_handler) (int, siginfo_t *, void *);

#ifdef ENABLE_PTHREAD
static pthread_key_t key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void pthread_key_creator (void);
#else
HevTaskStackFaultHandlerStackInfo *stack_info;
#endif

int
hev_task_stack_fault_handler_init (void)
{
	HevTaskStackFaultHandlerStackInfo *si;
	struct sigaction sa, osa;
	int retval = -1;

	si = hev_malloc (sizeof (HevTaskStackFaultHandlerStackInfo));
	if (!si)
		goto quit;

	si->new.ss_sp = hev_malloc (SIGSTKSZ);
	if (!si->new.ss_sp)
		goto quit;

	si->new.ss_size = SIGSTKSZ;
	si->new.ss_flags = 0;
	if (sigaltstack (&si->new, &si->old) == -1) {
		hev_free (si->new.ss_sp);
		goto quit;
	}

#ifdef ENABLE_PTHREAD
	pthread_once (&key_once, pthread_key_creator);
	pthread_setspecific (key, si);
	pthread_mutex_lock (&mutex);
#else
	stack_info = si;
#endif

	if (initialized) {
		retval = 0;
		goto quit;
	}

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO | SA_ONSTACK;
	sa.sa_sigaction = signal_handler;

	if (sigaction (SIGSEGV, &sa, &osa) == -1)
		goto quit;
	default_signal_handler = osa.sa_sigaction;

	retval = 0;
	initialized = 1;

quit:
#ifdef ENABLE_PTHREAD
	pthread_mutex_unlock (&mutex);
#endif

	return retval;
}

void
hev_task_stack_fault_handler_fini (void)
{
	HevTaskStackFaultHandlerStackInfo *si;

#ifdef ENABLE_PTHREAD
	si = pthread_getspecific (key);
#else
	si = stack_info;
#endif

	if (sigaltstack (&si->old, NULL) != -1)
		hev_free (si->new.ss_sp);

	hev_free (si);
}

#ifdef ENABLE_PTHREAD
static void
pthread_key_creator (void)
{
	pthread_key_create (&key, NULL);
}
#endif

static void
signal_handler (int signo, siginfo_t *info, void *context)
{
	HevTaskSystemContext *ctx;
	HevTaskStackPage *page;
	void *page_addr;
	void *stack_top;
	unsigned int page_index, page_size;
	uintptr_t page_mask;

	ctx = hev_task_system_get_context ();
	stack_top = ctx->stack + ctx->stack_size;

	if ((info->si_addr < ctx->stack) || (info->si_addr >= stack_top))
		default_signal_handler (signo, info, context);

	page_size = hev_task_stack_allocator_get_page_size (ctx->stack_allocator);
	page_mask = (uintptr_t) page_size - 1;
	page_addr = (void *) ((uintptr_t) info->si_addr & ~page_mask);
	page_index = (page_addr - ctx->stack) / page_size;
	page = ctx->current_task->stack_pages[page_index];

	if (!page) {
		page = hev_task_stack_allocator_alloc (ctx->stack_allocator);
		ctx->current_task->stack_pages[page_index] = page;
	}

	ctx->current_task->recently_stack_page = page;
	hev_task_stack_page_map (page, page_addr);
}

