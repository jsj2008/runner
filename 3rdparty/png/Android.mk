LOCAL_PATH:=$(call my-dir)

include $(CLEAR_VARS)

MY_PNG_SOURCES := libpng-1.5.6

LOCAL_MODULE := png
LOCAL_CFLAGS := -Werror -O2
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(MY_PNG_SOURCES)
LOCAL_EXPORT_LDLIBS := -lz
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES) $(LOCAL_PATH)
LOCAL_SRC_FILES := \
   $(MY_PNG_SOURCES)/pngtest.c \
   $(MY_PNG_SOURCES)/pngvalid.c \
   $(MY_PNG_SOURCES)/pngwtran.c \
   $(MY_PNG_SOURCES)/pngerror.c \
   $(MY_PNG_SOURCES)/pngtrans.c \
   $(MY_PNG_SOURCES)/pngrio.c \
   $(MY_PNG_SOURCES)/pngmem.c \
   $(MY_PNG_SOURCES)/pngset.c \
   $(MY_PNG_SOURCES)/pngpread.c \
   $(MY_PNG_SOURCES)/pngrtran.c \
   $(MY_PNG_SOURCES)/pngwrite.c \
   $(MY_PNG_SOURCES)/pngwio.c \
   $(MY_PNG_SOURCES)/pngrutil.c \
   $(MY_PNG_SOURCES)/png.c \
   $(MY_PNG_SOURCES)/pngget.c \
   $(MY_PNG_SOURCES)/pngread.c \
   $(MY_PNG_SOURCES)/pngwutil.c

include $(BUILD_STATIC_LIBRARY)
