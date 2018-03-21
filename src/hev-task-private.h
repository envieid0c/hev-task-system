/*
 ============================================================================
 Name        : hev-task-private.h
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description :
 ============================================================================
 */

#ifndef __HEV_TASK_PRIVATE_H__
#define __HEV_TASK_PRIVATE_H__

#include <setjmp.h>

#include "hev-task.h"
#include "hev-task-stack-allocator.h"

typedef struct _HevTaskSchedEntity HevTaskSchedEntity;

struct _HevTaskSchedEntity
{
	HevTask *task;
};

struct _HevTask
{
	void *stack_top;
	HevTaskEntry entry;
	void *data;

	HevTask *prev;
	HevTask *next;

	HevTaskSchedEntity sched_entity;

	void *stack;
	HevTaskStackPage **stack_pages;
	HevTaskStackPage *recently_stack_page;
	unsigned int shared_stack_index;

	int ref_count;
	int priority;
	int next_priority;
	HevTaskState state;

	jmp_buf context;
};

extern void hev_task_execute (HevTask *self, void *executer);

#endif /* __HEV_TASK_PRIVATE_H__ */

