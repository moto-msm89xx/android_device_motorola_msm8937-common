#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

$(call inherit-product, device/motorola/hannah/device.mk)

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_hannah
PRODUCT_DEVICE := hannah
PRODUCT_BRAND := motorola
PRODUCT_MODEL := moto e5 plus
PRODUCT_MANUFACTURER := motorola

BUILD_FINGERPRINT := motorola/hannah_t/hannah:8.0.0/OCPS27.91-150-4/4:user/release-keys

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRODUCT_NAME="hannah"

PRODUCT_GMS_CLIENTID_BASE := android-motorola
