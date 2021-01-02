#!/bin/bash
#
# Copyright (C) 2017-2020 The LineageOS Project
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

function blob_fixup() {
    case "${1}" in
        lib64/libwfdnative.so)
            "${PATCHELF}" --remove-needed android.hidl.base@1.0.so "${2}"
            ;;

        product/etc/permissions/vendor.qti.hardware.data.connection-V1.0-java.xml | product/etc/permissions/vendor.qti.hardware.data.connection-V1.1-java.xml)
            sed -i 's/xml version="2.0"/xml version="1.0"/' "${2}"
            ;;

        # memset shim
        vendor/bin/charge_only_mode)
            "${PATCHELF}" --add-needed libmemset_shim.so "${2}"
            ;;

        vendor/lib/hw/activity_recognition.msm8937.so | vendor/lib64/hw/activity_recognition.msm8937.so)
            "${PATCHELF}" --set-soname activity_recognition.msm8937.so "${2}"
            ;;

        vendor/lib/hw/camera.msm8937.so)
            "${PATCHELF}" --set-soname camera.msm8937.so "${2}"
            ;;

        vendor/lib64/hw/gatekeeper.msm8937.so)
            "${PATCHELF}" --set-soname gatekeeper.msm8937.so "${2}"
            ;;

        vendor/lib64/hw/keystore.msm8937.so)
            "${PATCHELF}" --set-soname keystore.msm8937.so "${2}"
            ;;

        vendor/lib/libactuator_dw9767_truly.so)
            "${PATCHELF}" --set-soname libactuator_dw9767_truly.so "${2}"
            ;;

        # Fix camera recording
        vendor/lib/libmmcamera2_pproc_modules.so)
            sed -i "s/ro.product.manufacturer/ro.product.nopefacturer/" "${2}"
            ;;

        vendor/lib/libmmcamera2_sensor_modules.so)
            sed -i 's|msm8953_mot_deen_camera.xml|msm8937_mot_camera_conf.xml|g' "${2}"
            ;;

        vendor/lib/libmot_gpu_mapper.so | vendor/lib/libmmcamera_vstab_module.so)
            sed -i "s/libgui/libwui/" "${2}"
            ;;

        vendor/lib64/libmdmcutback.so)
            sed -i "s|libqsap_sdk.so|libqsapshim.so|g" "${2}"
            ;;

        vendor/lib64/libril-qc-qmi-1.so)
            "${PATCHELF}" --add-needed "libcutils_shim.so" "${2}"
            ;;
    esac
}

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true

while [ "$1" != "" ]; do
    case $1 in
        -n | --no-cleanup )     CLEAN_VENDOR=false
                                ;;
        -s | --section )        shift
                                SECTION=$1
                                CLEAN_VENDOR=false
                                ;;
        * )                     SRC=$1
                                ;;
    esac
    shift
done

if [ -z "$SRC" ]; then
    SRC=adb
fi

# Initialize the helper
setup_vendor "${BOARD_COMMON}" "${VENDOR}" "${LINEAGE_ROOT}" true "${CLEAN_VENDOR}"

extract "${MY_DIR}/proprietary-files.txt" "${SRC}" "${SECTION}"

if [ -s "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt" ];then
    # Reinitialize the helper for device common
    source "${MY_DIR}/../${DEVICE_COMMON}/extract-files.sh"
    setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${LINEAGE_ROOT}" false "${CLEAN_VENDOR}"

    extract "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt" "${SRC}" "${SECTION}"
fi

if [ -s "${MY_DIR}/../${DEVICE}/proprietary-files.txt" ]; then
    # Reinitialize the helper for device
    source "${MY_DIR}/../${DEVICE}/extract-files.sh"
    setup_vendor "${DEVICE}" "${VENDOR}" "${LINEAGE_ROOT}" false "${CLEAN_VENDOR}"

    extract "${MY_DIR}/../${DEVICE_COMMON}/proprietary-files.txt" "${SRC}" "${SECTION}"
    extract "${MY_DIR}/../${DEVICE}/proprietary-files.txt" "${SRC}" "${SECTION}"
fi

"$MY_DIR"/setup-makefiles.sh
