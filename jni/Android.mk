ROOT := $(call my-dir)
ROOT := $(shell cygpath --mixed "$(ROOT)")
include $(call all-subdir-makefiles)
