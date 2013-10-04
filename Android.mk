include $(CLEAR_VARS)
LOCAL_PATH 		:= $(ROOT_PATH)
LOCAL_MODULE	:= kaztimer
LOCAL_SRC_FILES := $(wildcard $(LOCAL_PATH)/kaztimer/*.c)
include $(BUILD_SHARED_LIBRARY) 
