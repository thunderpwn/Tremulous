LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libovrplatform

LOCAL_SRC_FILES := ../libs/$(TARGET_ARCH_ABI)/$(LOCAL_MODULE).so

# only export public headers
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../include

ifneq (,$(wildcard $(LOCAL_PATH)/$(LOCAL_SRC_FILES)))
  include $(PREBUILT_SHARED_LIBRARY)
endif
