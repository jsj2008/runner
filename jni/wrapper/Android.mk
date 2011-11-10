LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= libwrapper
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_SRC_FILES	:= wrapper.c runner.c
LOCAL_STATIC_LIBRARIES	:= runner png physics bullet

include $(BUILD_SHARED_LIBRARY)

