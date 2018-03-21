/*
 ============================================================================
 Name        : hev-task-system-private.h
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description :
 ============================================================================
 */

#ifndef __HEV_TASK_SYSTEM_PRIVATE_H__
#define __HEV_TASK_SYSTEM_PRIVATE_H__

#include <setjmp.h>

#include "hev-task.h"
#include "hev-task-private.h"
#include "hev-task-system.h"
#include "hev-task-timer-manager.h"
#include "hev-task-stack-allocator.h"

#define HEV_TASK_RUN_SCHEDULER	HEV_TASK_YIELD_COUNT
#define PRIORITY_COUNT (HEV_TASK_PRIORITY_MAX - HEV_TASK_PRIORITY_MIN + 1)

typedef struct _HevTaskSystemContext HevTaskSystemContext;

struct _HevTaskSystemContext
{
	int epoll_fd;
	unsigned int total_task_count;
	unsigned int running_tasks_bitmap;

	HevTaskTimerManager *timer_manager;
	HevTaskStackAllocator *stack_allocator;

	void *shared_stack;
	unsigned int shared_stack_size;

	HevTask *new_task;
	HevTask *prev_task;
	HevTask *current_task;
	HevTask *running_tasks[PRIORITY_COUNT];
	HevTask *running_tasks_tail[PRIORITY_COUNT];

	jmp_buf kernel_context;
};

void hev_task_system_schedule (HevTaskYieldType type);
void hev_task_system_wakeup_task (HevTask *task);
void hev_task_system_run_new_task (HevTask *task);
void hev_task_system_kill_current_task (void);

HevTaskSystemContext * hev_task_system_get_context (void);

void * hev_task_system_get_shared_stack (void);
unsigned int hev_task_system_get_shared_stack_size (void);

#endif /* __HEV_TASK_SYSTEM_PRIVATE_H__ */

