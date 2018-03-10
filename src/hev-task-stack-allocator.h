/*
 ============================================================================
 Name        : hev-task-stack-allocator.h
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2018 everyone.
 Description :
 ============================================================================
 */

#ifndef __HEV_TASK_STACK_ALLOCATOR_H__
#define __HEV_TASK_STACK_ALLOCATOR_H__

typedef struct _HevTaskStackPage HevTaskStackPage;
typedef struct _HevTaskStackAllocator HevTaskStackAllocator;

HevTaskStackAllocator * hev_task_stack_allocator_new (void);
void hev_task_stack_allocator_destroy (HevTaskStackAllocator *self);

unsigned int hev_task_stack_allocator_get_page_size (HevTaskStackAllocator *self);

HevTaskStackPage * hev_task_stack_allocator_alloc (HevTaskStackAllocator *self);
void hev_task_stack_allocator_free (HevTaskStackAllocator *self, HevTaskStackPage *page);

int hev_task_stack_page_map (HevTaskStackPage *page, void *addr);
int hev_task_stack_page_unmap (HevTaskStackPage *page);

#endif /* __HEV_TASK_STACK_ALLOCATOR_H__ */

