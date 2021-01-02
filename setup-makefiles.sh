#!/bin/bash
#
# Copyright (C) 2017-2018 The LineageOS Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

set -e

# Override anything that may come from the calling environment
BOARD=msm8937

INITIAL_COPYRIGHT_YEAR=2019

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

LINEAGE_ROOT="${MY_DIR}/../../.."

HELPER="${LINEAGE_ROOT}/vendor/lineage/build/tools/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Store device common variable for later usage
DEVICE_COMMON_ORIGINAL="${DEVICE_COMMON}"
DEVICE_COMMON="${BOARD_COMMON}"

# Initialize the helper
setup_vendor "${BOARD_COMMON}" "${VENDOR}" "${LINEAGE_ROOT}" true

# Copyright headers and common guards
write_headers "${BOARD}" "TARGET_BOARD_PLATFORM"

write_makefiles "${MY_DIR}/common-proprietary-files.txt"

write_footers

# Restore device common variable to original one
DEVICE_COMMON="${DEVICE_COMMON_ORIGINAL}"

if [ -s "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt" ]; then
    # Reinitialize the helper for device common
    setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${LINEAGE_ROOT}" true

    # Copyright headers and guards
    write_headers "${DEVICE_COMMON_GUARDS}"

    write_makefiles "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt"

    write_footers
fi

if [ -s "${MY_DIR}/../${DEVICE}/proprietary-files.txt" ]; then
    # Reinitialize the helper for device
    INITIAL_COPYRIGHT_YEAR="$DEVICE_BRINGUP_YEAR"
    setup_vendor "${DEVICE}" "${VENDOR}" "${LINEAGE_ROOT}" false

    # Copyright headers and guards
    write_headers

    write_makefiles "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt"
    write_makefiles "${MY_DIR}/../${DEVICE}/proprietary-files.txt"

    write_footers
fi
