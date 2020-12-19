#!/bin/bash
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

set -e

# Override anything that may come from the calling environment
BOARD=msm8937

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

ANDROID_ROOT="${MY_DIR}/../../.."

HELPER="${ANDROID_ROOT}/tools/extract-utils/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Store device common variable for later usage
DEVICE_COMMON_ORIGINAL="${DEVICE_COMMON}"
DEVICE_COMMON="${BOARD_COMMON}"

# Initialize the helper
setup_vendor "${BOARD_COMMON}" "${VENDOR}" "${ANDROID_ROOT}" true

# Warning headers and guards
write_headers "${BOARD}" "TARGET_BOARD_PLATFORM"

# The standard common blobs
write_makefiles "${MY_DIR}/proprietary-files.txt" true

# Finish
write_footers

# Restore device common variable to original one
DEVICE_COMMON="${DEVICE_COMMON_ORIGINAL}"

if [ -s "${MY_DIR}/../$DEVICE_COMMON/proprietary-files.txt" ]; then
    # Reinitialize the helper for device common
    setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${ANDROID_ROOT}" true

    # Warning headers and guards
    write_headers "${DEVICE_COMMON_GUARDS}"

    # The standard device common blobs
    write_makefiles "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt" true

    # Finish
    write_footers
fi

if [ -s "${MY_DIR}/../${DEVICE}/proprietary-files.txt" ]; then
    # Reinitialize the helper for device
    setup_vendor "${DEVICE}" "${VENDOR}" "${ANDROID_ROOT}" false

    # Warning headers and guards
    write_headers

    # The standard device blobs
    write_makefiles "${MY_DIR}/../${DEVICE}/proprietary-files.txt" true

    # Finish
    write_footers
fi
