/*
 ============================================================================
 Name        : hev-task-executer.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2017 everyone.
 Description :
 ============================================================================
 */

#include <assert.h>

#include "hev-task-executer.h"
#include "hev-task-system-private.h"

#define STACK_OVERFLOW_DETECTION_TAG	(0xdeadbeefu)

void
hev_task_executer (HevTask *task)
{
	if (setjmp (task->context) == 0)
		return;

#ifdef ENABLE_STACK_OVERFLOW_DETECTION
	*(unsigned int *) task->stack = STACK_OVERFLOW_DETECTION_TAG;
#endif

	task->entry (task->data);

#ifdef ENABLE_STACK_OVERFLOW_DETECTION
	assert (*(unsigned int *) task->stack == STACK_OVERFLOW_DETECTION_TAG);
#endif

	hev_task_system_kill_current_task ();
}

