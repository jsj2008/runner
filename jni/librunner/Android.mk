LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= librunner
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_SRC_FILES	:= game.c world.c image.c common.c matrix.c vector.c quaternion.c frustum.c tex2d.c shader.c stream_zip.c bbox.c resman.c material.c physworld.c
LOCAL_C_INCLUDES	:= $(LOCAL_PATH)/../libzip $(LOCAL_PATH)/../libpng $(LOCAL_PATH)/../libbullet

include $(BUILD_STATIC_LIBRARY)
