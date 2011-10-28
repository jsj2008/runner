ROOT := $(call my-dir)
ROOT := $(shell [ -f /usr/bin/cygpath ] && cygpath --mixed "$(ROOT)" || echo "$(ROOT)")
include $(call all-subdir-makefiles)
