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


PRODUCT_SYSTEM_PROPERTY_BLACKLIST += \
    ro.product.model \
    ro.product.name

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="hannah_t-user 8.0.0 OCPS27.91-150-8 2 release-keys" \
    PRODUCT_NAME="hannah"

BUILD_FINGERPRINT := motorola/hannah_t/hannah:8.0.0/OCP27.91-150-8/2:user/release-keys

PRODUCT_GMS_CLIENTID_BASE := android-motorola
