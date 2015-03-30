LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := Hello

### Add all source file names to be included in lib separated by a whitespace
LOCAL_SRC_FILES := Hello.c
LOCAL_LDLIBS    := -lm -llog -ljnigraphics
include $(BUILD_SHARED_LIBRARY)
