LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := wrapper
LOCAL_CFLAGS := -Werror -O2
LOCAL_SRC_FILES := wrapper.c
LOCAL_STATIC_LIBRARIES := runner engine physics png bullet
LOCAL_C_INCLUDES := $(ROOT)/librunner

include $(BUILD_SHARED_LIBRARY)

