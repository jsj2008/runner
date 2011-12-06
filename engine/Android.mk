LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= engine
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_LDLIBS := -llog -landroid -lGLESv2
LOCAL_SRC_FILES	:= gui.c game.c world.c image.c gl_defs.c matrix.c vector.c quaternion.c frustum.c tex2d.c shader.c stream_android.c bbox.c resman.c material.c timestamp.c
LOCAL_STATIC_LIBRARIES := physics png bullet

include $(BUILD_STATIC_LIBRARY)
