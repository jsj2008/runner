LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= librunner
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_SRC_FILES	:= gl.c matrix.c vector.c quaternion.c camera.c tex2d.c model.c hlmdl.c shader.c stream_zip.c obj.c bbox.c frustum.c octree.c resman.c material.c entity.c physworld.c
LOCAL_C_INCLUDES	:= $(LOCAL_PATH)/../libzip $(LOCAL_PATH)/../libpng $(LOCAL_PATH)/../libbullet

include $(BUILD_STATIC_LIBRARY)