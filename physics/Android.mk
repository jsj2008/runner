LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := physics
LOCAL_CFLAGS := -Werror -O2
LOCAL_SRC_FILES := bullet_wrapper.cpp
LOCAL_STATIC_LIBRARIES := bullet
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../engine
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)

