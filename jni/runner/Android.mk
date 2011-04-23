LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE		:= librunner
LOCAL_CFLAGS		:= -Werror -O2
LOCAL_SRC_FILES	:= wrapper.c gl.c matrix.c vector.c camera.c tex2d.c model.c hlmdl.c runner.c shader.c stream.c obj.c bbox.c frustum.c octree.c
LOCAL_LDLIBS		:= -llog -lz -lGLESv1_CM -lGLESv2
LOCAL_STATIC_LIBRARIES	:= zip png
LOCAL_C_INCLUDES	:= $(LOCAL_PATH)/../libzip $(LOCAL_PATH)/../libpng

include $(BUILD_SHARED_LIBRARY)

