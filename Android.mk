LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= kaztimer
LOCAL_CFLAGS    += "-std=c++11"
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/kaztimer/*.cpp)
LOCAL_SRC_FILES := $(LOCAL_SRC_FILES:$(LOCAL_PATH)/%=%)
include $(BUILD_SHARED_LIBRARY)
