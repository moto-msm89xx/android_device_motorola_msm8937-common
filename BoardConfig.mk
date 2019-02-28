#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

DEVICE_PATH := device/motorola/hannah

# Assert
TARGET_OTA_ASSERT_DEVICE := hannah,hannah_t,ahannah,rhannah

# Bootloader
TARGET_BOOTLOADER_BOARD_NAME := MSM8937
TARGET_NO_BOOTLOADER := true

# Platform
TARGET_BOARD_PLATFORM := msm8937

# Architecture
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_VARIANT := cortex-a53

# Kernel
TARGET_KERNEL_CONFIG := hannah_defconfig
TARGET_KERNEL_SOURCE := kernel/motorola/msm8937

# Inherit from the proprietary version
-include vendor/motorola/hannah/BoardConfigVendor.mk
