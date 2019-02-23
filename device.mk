#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_o.mk)

# Get non-open-source specific aspects
$(call inherit-product-if-exists, vendor/motorola/hannah/hannah-vendor.mk)

# Boot animation
TARGET_SCREEN_HEIGHT := 1440
TARGET_SCREEN_WIDTH := 720

# Overlays
DEVICE_PACKAGE_OVERLAYS += \
    $(LOCAL_PATH)/overlay \
    $(LOCAL_PATH)/overlay-lineage

# Screen density
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxhdpi

# Camera
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/camera/mot_s5k4h7_hannah_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/mot_s5k4h7_hannah_chromatix.xml \
    $(LOCAL_PATH)/configs/camera/msm8937_mot_hannah_camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/msm8937_mot_hannah_camera.xml \
    $(LOCAL_PATH)/configs/camera/ov12a10_hannah_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/ov12a10_hannah_chromatix.xml \
    $(LOCAL_PATH)/configs/camera/ov5675_jeterht_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/ov5675_jeterht_chromatix.xml \
    $(LOCAL_PATH)/configs/camera/ov5675_jeterkc_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/ov5675_jeterkc_chromatix.xml

# Keylayouts
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keylayout/ft5x06_ts.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/ft5x06_ts.kl \
    $(LOCAL_PATH)/keylayout/gpio-keys.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/gpio-keys.kl \
    $(LOCAL_PATH)/keylayout/synaptics_dsx.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/synaptics_dsx.kl \
    $(LOCAL_PATH)/keylayout/synaptics_rmi4_i2c.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/synaptics_rmi4_i2c.kl \
    $(LOCAL_PATH)/keylayout/uinput-egis.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/uinput-egis.kl

# IDC
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/idc/uinput-egis.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/uinput-egis.idc
