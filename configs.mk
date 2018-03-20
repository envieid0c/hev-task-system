# Configs

ENABLE_PTHREAD := 1
ENABLE_STACK_OVERFLOW_DETECTION := 1
ENABLE_MEMALLOC_SLICE := 1

CONFIG_MEMALLOC_SLICE_ALIGN := 64
CONFIG_MEMALLOC_SLICE_MAX_SIZE := 0x100000
CONFIG_MEMALLOC_SLICE_MAX_COUNT := 1000

CONFIG_TASK_TIMER_MAX_COUNT := 100

CONFIG_TASK_SHARED_STACK_SIZE := 0x800000
CONFIG_TASK_SHARED_STACK_COUNT := 8


CONFIG_CFLAGS :=

ifeq ($(ENABLE_PTHREAD),1)
	CONFIG_CFLAGS+=-DENABLE_PTHREAD
endif

ifeq ($(ENABLE_STACK_OVERFLOW_DETECTION),1)
	CONFIG_CFLAGS+=-DENABLE_STACK_OVERFLOW_DETECTION
endif

ifeq ($(ENABLE_MEMALLOC_SLICE),1)
	CONFIG_CFLAGS+=-DENABLE_MEMALLOC_SLICE
endif

CONFIG_CFLAGS+=-DCONFIG_MEMALLOC_SLICE_ALIGN=$(CONFIG_MEMALLOC_SLICE_ALIGN)
CONFIG_CFLAGS+=-DCONFIG_MEMALLOC_SLICE_MAX_SIZE=$(CONFIG_MEMALLOC_SLICE_MAX_SIZE)
CONFIG_CFLAGS+=-DCONFIG_MEMALLOC_SLICE_MAX_COUNT=$(CONFIG_MEMALLOC_SLICE_MAX_COUNT)
CONFIG_CFLAGS+=-DCONFIG_TASK_TIMER_MAX_COUNT=$(CONFIG_TASK_TIMER_MAX_COUNT)
CONFIG_CFLAGS+=-DCONFIG_TASK_SHARED_STACK_SIZE=$(CONFIG_TASK_SHARED_STACK_SIZE)
CONFIG_CFLAGS+=-DCONFIG_TASK_SHARED_STACK_COUNT=$(CONFIG_TASK_SHARED_STACK_COUNT)
