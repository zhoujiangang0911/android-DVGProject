LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
    hello.c \
    uart_test.c
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := helloworld
include $(BUILD_EXECUTABLE)
