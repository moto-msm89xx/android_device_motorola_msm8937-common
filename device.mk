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

# Properties
-include $(LOCAL_PATH)/vendor_prop.mk

# Screen density
PRODUCT_AAPT_CONFIG := normal
PRODUCT_AAPT_PREF_CONFIG := xxhdpi

# Audio
PRODUCT_COPY_FILES += \
    frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
    frameworks/av/services/audiopolicy/config/default_volume_tables.xml:$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
    frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
    frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml \
    $(LOCAL_PATH)/audio/audio_effects.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_effects.conf \
    $(LOCAL_PATH)/audio/audio_ext_spkr.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_ext_spkr.conf \
    $(LOCAL_PATH)/audio/audio_output_policy.conf:$(TARGET_COPY_OUT_VENDOR)/etc/audio_output_policy.conf \
    $(LOCAL_PATH)/audio/audio_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_platform_info.xml \
    $(LOCAL_PATH)/audio/audio_policy_configuration.xml:$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_configuration.xml \
    $(LOCAL_PATH)/audio/mixer_paths.xml:$(TARGET_COPY_OUT_VENDOR)/etc/mixer_paths.xml \
    $(LOCAL_PATH)/audio/sound_trigger_mixer_paths.xml:$(TARGET_COPY_OUT_VENDOR)/etc/sound_trigger_mixer_paths.xml \
    $(LOCAL_PATH)/audio/sound_trigger_platform_info.xml:$(TARGET_COPY_OUT_VENDOR)/etc/sound_trigger_platform_info.xml

# Bluetooth
PRODUCT_PACKAGES += \
    libbt-vendor

PRODUCT_PACKAGES += \
    android.hardware.bluetooth@1.0-service

# Camera
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/camera/mot_s5k4h7_hannah_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/mot_s5k4h7_hannah_chromatix.xml \
    $(LOCAL_PATH)/configs/camera/msm8937_mot_hannah_camera.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/msm8937_mot_hannah_camera.xml \
    $(LOCAL_PATH)/configs/camera/ov12a10_hannah_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/ov12a10_hannah_chromatix.xml \
    $(LOCAL_PATH)/configs/camera/ov5675_jeterht_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/ov5675_jeterht_chromatix.xml \
    $(LOCAL_PATH)/configs/camera/ov5675_jeterkc_chromatix.xml:$(TARGET_COPY_OUT_VENDOR)/etc/camera/ov5675_jeterkc_chromatix.xml

# IRQ balance
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/msm_irqbalance.conf:$(TARGET_COPY_OUT_VENDOR)/etc/msm_irqbalance.conf

# IRSC
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/sec_config:$(TARGET_COPY_OUT_VENDOR)/etc/sec_config

# Keylayouts
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keylayout/ft5x06_ts.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/ft5x06_ts.kl \
    $(LOCAL_PATH)/keylayout/gpio-keys.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/gpio-keys.kl \
    $(LOCAL_PATH)/keylayout/synaptics_dsx.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/synaptics_dsx.kl \
    $(LOCAL_PATH)/keylayout/synaptics_rmi4_i2c.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/synaptics_rmi4_i2c.kl \
    $(LOCAL_PATH)/keylayout/uinput-egis.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/uinput-egis.kl

# IDC
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/idc/uinput-egis.idc:$(TARGET_COPY_OUT_VENDOR)/idc/uinput-egis.idc

# Media
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    $(LOCAL_PATH)/configs/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml \
    $(LOCAL_PATH)/configs/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles.xml

# Ramdisk
PRODUCT_PACKAGES += \
    fstab.qcom \
    init.mmi.common.rc \
    init.mmi.nonab.rc \
    init.mmi.overlay.rc \
    init.mmi.rc \
    init.mmi.sensor.rc \
    init.mmi.usb.rc \
    init.mmi.volte.rc \
    init.oem.rc \
    init.qcom.rc \
    init.target.rc \
    ueventd.qcom.rc

PRODUCT_PACKAGES += \
    init.mmi.boot.sh \
    init.mmi.laser.sh \
    init.mmi.touch.sh \
    init.mmi.usb.sh \
    init.oem.hw.sh \
    init.qcom.early_boot.sh \
    init.qcom.post_boot.sh \
    init.qcom.sensors.sh \
    init.qcom.sh \
    init.qcom.syspart_fixup.sh

# Sensors
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/sensors/hals.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/hals.conf \
    $(LOCAL_PATH)/configs/sensors/sensor_def_qcomdev.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/sensor_def_qcomdev.conf
