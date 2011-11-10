ROOT := $(call my-dir)
ROOT := $(shell [ -f /usr/bin/cygpath ] && cygpath --mixed "$(ROOT)" || echo "$(ROOT)")
include $(call all-subdir-makefiles)
include $(ROOT)/../../../3rdparty/bullet/Android.mk
include $(ROOT)/../../../3rdparty/png/Android.mk
include $(ROOT)/../../../physics/Android.mk
include $(ROOT)/../../../engine/Android.mk
include $(ROOT)/../../shared/Android.mk

