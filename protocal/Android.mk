LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES:= \
    main.c \
    pro_timer.c \
    protocal.c \
    serial.c \
    util.c
    
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := dgv
include $(BUILD_EXECUTABLE)
