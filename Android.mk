# Copyright (C) 2017 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := libhev-task-system
LOCAL_SRC_FILES := \
	src/hev-memory-allocator.c \
	src/hev-memory-allocator-slice.c \
	src/hev-task.c \
	src/hev-task-poll.c \
	src/hev-task-execute.S \
	src/hev-task-executer.c \
	src/hev-task-system.c \
	src/hev-task-system-schedule.c \
	src/hev-task-stack-allocator.c \
	src/hev-task-stack-fault-handler.c \
	src/hev-task-timer-manager.c
include $(LOCAL_PATH)/configs.mk
LOCAL_CFLAGS += $(CONFIG_CFLAGS)
ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
LOCAL_CFLAGS += -mfpu=neon
endif
include $(BUILD_STATIC_LIBRARY)

