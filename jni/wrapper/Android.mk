LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= libwrapper
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_SRC_FILES	:= wrapper.c runner.c
LOCAL_LDLIBS		:= -llog -lz -lGLESv1_CM -lGLESv2 -landroid
LOCAL_STATIC_LIBRARIES	:= runner zip png bullet
LOCAL_C_INCLUDES	:= $(ROOT)/librunner

include $(BUILD_SHARED_LIBRARY)

