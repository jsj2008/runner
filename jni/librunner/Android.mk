LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= librunner
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_SRC_FILES	:= gui.c game.c world.c image.c common.c matrix.c vector.c quaternion.c frustum.c tex2d.c shader.c stream_android.c bbox.c resman.c material.c physworld.c
LOCAL_C_INCLUDES	:= $(ROOT)/libpng $(ROOT)/libbullet

include $(BUILD_STATIC_LIBRARY)
