LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
    bootable/recovery/edify/include \
    bootable/recovery/otautil/include
LOCAL_SRC_FILES := recovery_updater.cpp
LOCAL_MODULE := librecovery_updater_motorola_msm8937
include $(BUILD_STATIC_LIBRARY)
