include $(CLEAR_VARS)
LOCAL_PATH 		:= $(ROOT_PATH)/lib/kaztimer
LOCAL_MODULE	:= kaztimer
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/kaztimer/*.c))
include $(BUILD_SHARED_LIBRARY) 
