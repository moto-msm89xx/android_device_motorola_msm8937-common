/* Copyright (c) 2018-2020, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define LOG_TAG "LocSvc_LocationClientApi"

#include <sys/types.h>
#include <unistd.h>
#include <loc_cfg.h>
#include <LocationClientApiImpl.h>
#include <log_util.h>
#include <gps_extended_c.h>
#include <unistd.h>
#include <sstream>
#include <dlfcn.h>
#include <loc_misc_utils.h>

static uint32_t gDebug = 0;

static const loc_param_s_type gConfigTable[] =
{
    {"DEBUG_LEVEL", &gDebug, NULL, 'n'}
};


namespace location_client {

static uint32_t gfIdGenerator = LOCATION_CLIENT_SESSION_ID_INVALID;
mutex GeofenceImpl::mGfMutex;
uint32_t GeofenceImpl::nextId() {
    lock_guard<mutex> lock(mGfMutex);
    uint32_t id = ++gfIdGenerator;
    if (0xFFFFFFFF == id) {
        id = 0;
        gfIdGenerator = 0;
    }
    return id;
}

/******************************************************************************
Utilities
******************************************************************************/
static GnssMeasurementsDataFlagsMask parseMeasurementsDataMask(
    ::GnssMeasurementsDataFlagsMask in) {
    uint32_t out = 0;
    LOC_LOGd("Hal GnssMeasurementsDataFlagsMask =0x%x ", in);

    if (::GNSS_MEASUREMENTS_DATA_SV_ID_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_SV_ID_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_SV_TYPE_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_STATE_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_STATE_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_RECEIVED_SV_TIME_UNCERTAINTY_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_CARRIER_TO_NOISE_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_ADR_STATE_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_ADR_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_ADR_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_ADR_UNCERTAINTY_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_MULTIPATH_INDICATOR_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_AUTOMATIC_GAIN_CONTROL_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_FULL_ISB_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_FULL_ISB_UNCERTAINTY_BIT;
    }
    if (::GNSS_MEASUREMENTS_DATA_CYCLE_SLIP_COUNT_BIT & in) {
        out |= GNSS_MEASUREMENTS_DATA_CYCLE_SLIP_COUNT_BIT;
    }
    LOC_LOGd("LCA GnssMeasurementsDataFlagsMask =0x%x ", out);
    return static_cast<GnssMeasurementsDataFlagsMask>(out);
}

static LocationCapabilitiesMask parseCapabilitiesMask(::LocationCapabilitiesMask mask) {
    uint64_t capsMask = 0;

    LOC_LOGd("LocationCapabilitiesMask =0x%x ", mask);

    if (::LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT & mask) {
        capsMask |= LOCATION_CAPS_TIME_BASED_TRACKING_BIT;
    }
    if (::LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_TIME_BASED_BATCHING_BIT;
    }
    if (::LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_DISTANCE_BASED_TRACKING_BIT;
    }
    if (::LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_DISTANCE_BASED_BATCHING_BIT;
    }
    if (::LOCATION_CAPABILITIES_GEOFENCE_BIT & mask) {
        capsMask |=  LOCATION_CAPS_GEOFENCE_BIT;
    }
    if (::LOCATION_CAPABILITIES_OUTDOOR_TRIP_BATCHING_BIT & mask) {
        capsMask |=  LOCATION_CAPS_OUTDOOR_TRIP_BATCHING_BIT;
    }

    return static_cast<LocationCapabilitiesMask>(capsMask);
}

static uint16_t parseYearOfHw(::LocationCapabilitiesMask mask) {
    uint16_t yearOfHw = 2015;
    if (::LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT & mask) {
        yearOfHw++; // 2016
        if (::LOCATION_CAPABILITIES_DEBUG_NMEA_BIT & mask) {
            yearOfHw++; // 2017
            if (::LOCATION_CAPABILITIES_CONSTELLATION_ENABLEMENT_BIT & mask ||
                ::LOCATION_CAPABILITIES_AGPM_BIT & mask) {
                yearOfHw++; // 2018
                if (::LOCATION_CAPABILITIES_PRIVACY_BIT & mask) {
                    yearOfHw++; // 2019
                    if (::LOCATION_CAPABILITIES_MEASUREMENTS_CORRECTION_BIT & mask) {
                        yearOfHw++; // 2020
                    }
                }
            }
        }
    }
    return yearOfHw;
}

static void parseLocation(const ::Location &halLocation, Location& location) {
    uint32_t flags = 0;

    location.timestamp = halLocation.timestamp;
    location.latitude = halLocation.latitude;
    location.longitude = halLocation.longitude;
    location.altitude = halLocation.altitude;
    location.speed = halLocation.speed;
    location.bearing = halLocation.bearing;
    location.horizontalAccuracy = halLocation.accuracy;
    location.verticalAccuracy = halLocation.verticalAccuracy;
    location.speedAccuracy = halLocation.speedAccuracy;
    location.bearingAccuracy = halLocation.bearingAccuracy;

    if (0 != halLocation.timestamp) {
        flags |= LOCATION_HAS_TIMESTAMP_BIT;
    }
    if (::LOCATION_HAS_LAT_LONG_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_LAT_LONG_BIT;
    }
    if (::LOCATION_HAS_ALTITUDE_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_ALTITUDE_BIT;
    }
    if (::LOCATION_HAS_SPEED_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_SPEED_BIT;
    }
    if (::LOCATION_HAS_BEARING_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_BEARING_BIT;
    }
    if (::LOCATION_HAS_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_ACCURACY_BIT;
    }
    if (::LOCATION_HAS_VERTICAL_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_VERTICAL_ACCURACY_BIT;
    }
    if (::LOCATION_HAS_SPEED_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_SPEED_ACCURACY_BIT;
    }
    if (::LOCATION_HAS_BEARING_ACCURACY_BIT & halLocation.flags) {
        flags |= LOCATION_HAS_BEARING_ACCURACY_BIT;
    }
    location.flags = (LocationFlagsMask)flags;

    flags = 0;
    if (::LOCATION_TECHNOLOGY_GNSS_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_GNSS_BIT;
    }
    if (::LOCATION_TECHNOLOGY_CELL_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_CELL_BIT;
    }
    if (::LOCATION_TECHNOLOGY_WIFI_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_WIFI_BIT;
    }
    if (::LOCATION_TECHNOLOGY_SENSORS_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_SENSORS_BIT;
    }
    if (::LOCATION_TECHNOLOGY_REFERENCE_LOCATION_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_REFERENCE_LOCATION_BIT;
    }
    if (::LOCATION_TECHNOLOGY_INJECTED_COARSE_POSITION_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_INJECTED_COARSE_POSITION_BIT;
    }
    if (::LOCATION_TECHNOLOGY_AFLT_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_AFLT_BIT;
    }
    if (::LOCATION_TECHNOLOGY_HYBRID_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_HYBRID_BIT;
    }
    if (::LOCATION_TECHNOLOGY_PPE_BIT & halLocation.techMask) {
        flags |= LOCATION_TECHNOLOGY_PPE_BIT;
    }
    location.techMask = (LocationTechnologyMask)flags;
}

static Location parseLocation(const ::Location &halLocation) {
    Location location;
    parseLocation(halLocation, location);
    return location;
}

static GnssLocationSvUsedInPosition parseLocationSvUsedInPosition(
        const ::GnssLocationSvUsedInPosition &halSv) {

    GnssLocationSvUsedInPosition clientSv;
    clientSv.gpsSvUsedIdsMask = halSv.gpsSvUsedIdsMask;
    clientSv.gloSvUsedIdsMask = halSv.gloSvUsedIdsMask;
    clientSv.galSvUsedIdsMask = halSv.galSvUsedIdsMask;
    clientSv.bdsSvUsedIdsMask = halSv.bdsSvUsedIdsMask;
    clientSv.qzssSvUsedIdsMask = halSv.qzssSvUsedIdsMask;
    clientSv.navicSvUsedIdsMask = halSv.navicSvUsedIdsMask;
    return clientSv;
}

static GnssSignalTypeMask parseGnssSignalType(const ::GnssSignalTypeMask &halGnssSignalTypeMask) {
    uint32_t gnssSignalTypeMask = 0;
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GPS_L1CA) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L1CA_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GPS_L1C) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L1C_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GPS_L2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L2_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GPS_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GPS_L5_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GLONASS_G1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GLONASS_G1_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GLONASS_G2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GLONASS_G2_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GALILEO_E1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E1_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GALILEO_E5A) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E5A_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_GALILEO_E5B) {
        gnssSignalTypeMask |= GNSS_SIGNAL_GALILEO_E5B_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B1I) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1I_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B1C) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1C_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B2I) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2I_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B2AI) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2AI_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_QZSS_L1CA) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L1CA_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_QZSS_L1S) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L1S_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_QZSS_L2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L2_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_QZSS_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_QZSS_L5_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_SBAS_L1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_SBAS_L1_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_NAVIC_L5) {
        gnssSignalTypeMask |= GNSS_SIGNAL_NAVIC_L5_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B2AQ) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2AQ_BIT;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B1) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B1;
    }
    if (halGnssSignalTypeMask & ::GNSS_SIGNAL_BEIDOU_B2) {
        gnssSignalTypeMask |= GNSS_SIGNAL_BEIDOU_B2;
    }
    return (GnssSignalTypeMask)gnssSignalTypeMask;
}

static void parseGnssMeasUsageInfo(const ::GnssLocationInfoNotification &halLocationInfo,
        std::vector<GnssMeasUsageInfo>& clientMeasUsageInfo) {

    if (halLocationInfo.numOfMeasReceived) {

        for (int idx = 0; idx < halLocationInfo.numOfMeasReceived; idx++) {
            GnssMeasUsageInfo measUsageInfo;

            measUsageInfo.gnssSignalType = parseGnssSignalType(
                    halLocationInfo.measUsageInfo[idx].gnssSignalType);
            measUsageInfo.gnssConstellation = (Gnss_LocSvSystemEnumType)
                    halLocationInfo.measUsageInfo[idx].gnssConstellation;
            measUsageInfo.gnssSvId = halLocationInfo.measUsageInfo[idx].gnssSvId;
            clientMeasUsageInfo.push_back(measUsageInfo);
        }
    }
}

static GnssLocationPositionDynamics parseLocationPositionDynamics(
        const ::GnssLocationPositionDynamics &halPositionDynamics,
        const ::GnssLocationPositionDynamicsExt &halPositionDynamicsExt) {
    GnssLocationPositionDynamics positionDynamics = {};
    uint32_t bodyFrameDataMask = 0;

    if (::LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_BIT;
        positionDynamics.longAccel = halPositionDynamics.longAccel;
    }
    if (::LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_BIT;
        positionDynamics.latAccel = halPositionDynamics.latAccel;
    }
    if (::LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_BIT;
        positionDynamics.vertAccel = halPositionDynamics.vertAccel;
    }
    if (::LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LONG_ACCEL_UNC_BIT;
        positionDynamics.longAccelUnc = halPositionDynamics.longAccelUnc;
    }
    if (::LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_LAT_ACCEL_UNC_BIT;
        positionDynamics.latAccelUnc = halPositionDynamics.latAccelUnc;
    }
    if (::LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_VERT_ACCEL_UNC_BIT;
        positionDynamics.vertAccelUnc = halPositionDynamics.vertAccelUnc;
    }

    if (::LOCATION_NAV_DATA_HAS_ROLL_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_BIT;
        positionDynamics.roll = halPositionDynamicsExt.roll;
    }
    if (::LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_UNC_BIT;
        positionDynamics.rollUnc = halPositionDynamicsExt.rollUnc;
    }
    if (::LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_RATE_BIT;
        positionDynamics.rollRate = halPositionDynamicsExt.rollRate;
    }
    if (::LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_ROLL_RATE_UNC_BIT;
        positionDynamics.rollRateUnc = halPositionDynamicsExt.rollRateUnc;
    }

    if (::LOCATION_NAV_DATA_HAS_PITCH_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_BIT;
        positionDynamics.pitch = halPositionDynamics.pitch;
    }
    if (::LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_UNC_BIT;
        positionDynamics.pitchUnc = halPositionDynamics.pitchUnc;
    }
    if (::LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_RATE_BIT;
        positionDynamics.pitchRate = halPositionDynamicsExt.pitchRate;
    }
    if (::LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_PITCH_RATE_UNC_BIT;
        positionDynamics.pitchRateUnc = halPositionDynamicsExt.pitchRateUnc;
    }

    if (::LOCATION_NAV_DATA_HAS_YAW_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_BIT;
        positionDynamics.yaw = halPositionDynamicsExt.yaw;
    }
    if (::LOCATION_NAV_DATA_HAS_YAW_UNC_BIT & halPositionDynamicsExt.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_UNC_BIT;
        positionDynamics.yawUnc = halPositionDynamicsExt.yawUnc;
    }
    if (::LOCATION_NAV_DATA_HAS_YAW_RATE_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_BIT;
        positionDynamics.yawRate = halPositionDynamics.yawRate;
    }
    if (::LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT & halPositionDynamics.bodyFrameDataMask) {
        bodyFrameDataMask |= LOCATION_NAV_DATA_HAS_YAW_RATE_UNC_BIT;
        positionDynamics.yawRateUnc = halPositionDynamics.yawRateUnc;
    }
    positionDynamics.bodyFrameDataMask = (GnssLocationPosDataMask)bodyFrameDataMask;

    return positionDynamics;
}

static LocationReliability parseLocationReliability(const ::LocationReliability &halReliability) {

    LocationReliability reliability;
    switch (halReliability) {
        case ::LOCATION_RELIABILITY_VERY_LOW:
            reliability = LOCATION_RELIABILITY_VERY_LOW;
            break;

        case ::LOCATION_RELIABILITY_LOW:
            reliability = LOCATION_RELIABILITY_LOW;
            break;

        case ::LOCATION_RELIABILITY_MEDIUM:
            reliability = LOCATION_RELIABILITY_MEDIUM;
            break;

        case ::LOCATION_RELIABILITY_HIGH:
            reliability = LOCATION_RELIABILITY_HIGH;
            break;

        default:
            reliability = LOCATION_RELIABILITY_NOT_SET;
            break;
    }

    return reliability;
}

static GnssSystemTimeStructType parseGnssTime(const ::GnssSystemTimeStructType &halGnssTime) {

    GnssSystemTimeStructType   gnssTime;
    memset(&gnssTime, 0, sizeof(gnssTime));
    uint32_t gnssTimeFlags = 0;

    if (::GNSS_SYSTEM_TIME_WEEK_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_VALID;
        gnssTime.systemWeek = halGnssTime.systemWeek;
    }
    if (::GNSS_SYSTEM_TIME_WEEK_MS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
        gnssTime.systemMsec = halGnssTime.systemMsec;
    }
    if (::GNSS_SYSTEM_CLK_TIME_BIAS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
        gnssTime.systemClkTimeBias = halGnssTime.systemClkTimeBias;
    }
    if (::GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
        gnssTime.systemClkTimeUncMs = halGnssTime.systemClkTimeUncMs;
    }
    if (::GNSS_SYSTEM_REF_FCOUNT_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_REF_FCOUNT_VALID;
        gnssTime.refFCount = halGnssTime.refFCount;
    }
    if (::GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
        gnssTime.numClockResets = halGnssTime.numClockResets;
    }

    gnssTime.validityMask = (GnssSystemTimeStructTypeFlags)gnssTimeFlags;

    return gnssTime;
}

static GnssGloTimeStructType parseGloTime(const ::GnssGloTimeStructType &halGloTime) {

    GnssGloTimeStructType   gloTime;
    memset(&gloTime, 0, sizeof(gloTime));
    uint32_t gloTimeFlags = 0;

    if (::GNSS_CLO_DAYS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_CLO_DAYS_VALID;
        gloTime.gloDays = halGloTime.gloDays;
    }
    if (::GNSS_GLO_MSEC_VALID  & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_MSEC_VALID ;
        gloTime.gloMsec = halGloTime.gloMsec;
    }
    if (::GNSS_GLO_CLK_TIME_BIAS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_VALID;
        gloTime.gloClkTimeBias = halGloTime.gloClkTimeBias;
    }
    if (::GNSS_GLO_CLK_TIME_BIAS_UNC_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
        gloTime.gloClkTimeUncMs = halGloTime.gloClkTimeUncMs;
    }
    if (::GNSS_GLO_REF_FCOUNT_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_REF_FCOUNT_VALID;
        gloTime.refFCount = halGloTime.refFCount;
    }
    if (::GNSS_GLO_NUM_CLOCK_RESETS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
        gloTime.numClockResets = halGloTime.numClockResets;
    }
    if (::GNSS_GLO_FOUR_YEAR_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_FOUR_YEAR_VALID;
        gloTime.gloFourYear = halGloTime.gloFourYear;
    }

    gloTime.validityMask = (GnssGloTimeStructTypeFlags)gloTimeFlags;

    return gloTime;
}

static GnssSystemTime parseSystemTime(const ::GnssSystemTime &halSystemTime) {

    GnssSystemTime systemTime;
    memset(&systemTime, 0x0, sizeof(GnssSystemTime));

    switch (halSystemTime.gnssSystemTimeSrc) {
        case ::GNSS_LOC_SV_SYSTEM_GPS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GPS;
           systemTime.u.gpsSystemTime = parseGnssTime(halSystemTime.u.gpsSystemTime);
           break;
        case ::GNSS_LOC_SV_SYSTEM_GALILEO:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GALILEO;
           systemTime.u.galSystemTime = parseGnssTime(halSystemTime.u.galSystemTime);
           break;
        case ::GNSS_LOC_SV_SYSTEM_SBAS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_SBAS;
           break;
        case ::GNSS_LOC_SV_SYSTEM_GLONASS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_GLONASS;
           systemTime.u.gloSystemTime = parseGloTime(halSystemTime.u.gloSystemTime);
           break;
        case ::GNSS_LOC_SV_SYSTEM_BDS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_BDS;
           systemTime.u.bdsSystemTime = parseGnssTime(halSystemTime.u.bdsSystemTime);
           break;
        case ::GNSS_LOC_SV_SYSTEM_QZSS:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_QZSS;
           systemTime.u.qzssSystemTime = parseGnssTime(halSystemTime.u.qzssSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_NAVIC:
           systemTime.gnssSystemTimeSrc = GNSS_LOC_SV_SYSTEM_NAVIC;
           systemTime.u.navicSystemTime = parseGnssTime(halSystemTime.u.navicSystemTime);
           break;
    }

    return systemTime;
}

static GnssLocation parseLocationInfo(const ::GnssLocationInfoNotification &halLocationInfo) {

    GnssLocation locationInfo;
    parseLocation(halLocationInfo.location, locationInfo);
    uint64_t flags = 0;

    if (::GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
    }
    if (::GNSS_LOCATION_INFO_DOP_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_DOP_BIT;
    }
    if (::GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_MAGNETIC_DEVIATION_BIT;
    }
    if (::GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_RELIABILITY_BIT;
    }
    if (::GNSS_LOCATION_INFO_VER_RELIABILITY_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_VER_RELIABILITY_BIT;
    }
    if (::GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MAJOR_BIT;
    }
    if (::GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_SEMI_MINOR_BIT;
    }
    if (::GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_HOR_ACCURACY_ELIP_AZIMUTH_BIT;
    }
    if (::GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_GNSS_SV_USED_DATA_BIT;
    }
    if (::GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NAV_SOLUTION_MASK_BIT;
    }
    flags |= GNSS_LOCATION_INFO_POS_TECH_MASK_BIT;
    if (::GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_SV_SOURCE_INFO_BIT;
    }
    if (::GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_POS_DYNAMICS_DATA_BIT;
    }
    if (::GNSS_LOCATION_INFO_EXT_DOP_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EXT_DOP_BIT;
    }
    if (::GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_ALTITUDE_MEAN_SEA_LEVEL_BIT;
    }
    if (::GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NORTH_STD_DEV_BIT;
    }
    if (::GNSS_LOCATION_INFO_EAST_STD_DEV_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EAST_STD_DEV_BIT;
    }
    if (::GNSS_LOCATION_INFO_NORTH_VEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NORTH_VEL_BIT;
    }
    if (::GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NORTH_VEL_UNC_BIT;
    }
    if (::GNSS_LOCATION_INFO_EAST_VEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EAST_VEL_BIT;
    }
    if (::GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_EAST_VEL_UNC_BIT;
    }
    if (::GNSS_LOCATION_INFO_UP_VEL_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_UP_VEL_BIT;
    }
    if (::GNSS_LOCATION_INFO_UP_VEL_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_UP_VEL_UNC_BIT;
    }
    if (::GNSS_LOCATION_INFO_LEAP_SECONDS_BIT & halLocationInfo.flags) {
       flags |= GNSS_LOCATION_INFO_LEAP_SECONDS_BIT;
    }
    if (::GNSS_LOCATION_INFO_TIME_UNC_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_TIME_UNC_BIT;
    }
    if (::GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_NUM_SV_USED_IN_POSITION_BIT;
    }
    if (::GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_CALIBRATION_CONFIDENCE_PERCENT_BIT;
        locationInfo.calibrationConfidencePercent = halLocationInfo.calibrationConfidence;
    }
    if (::GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_CALIBRATION_STATUS_BIT;
        locationInfo.calibrationStatus =
                (DrCalibrationStatusMask)halLocationInfo.calibrationStatus;
    }
    if (::GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_OUTPUT_ENG_TYPE_BIT;
    }
    if (::GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_OUTPUT_ENG_MASK_BIT;
    }

    if (::GNSS_LOCATION_INFO_CONFORMITY_INDEX_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_CONFORMITY_INDEX_BIT;
    }

    if (::GNSS_LOCATION_INFO_LLA_VRP_BASED_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_LLA_VRP_BASED_BIT;
    }

    if (::GNSS_LOCATION_INFO_ENU_VELOCITY_VRP_BASED_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_ENU_VELOCITY_VRP_BASED_BIT;
    }

    if (::GNSS_LOCATION_INFO_DR_SOLUTION_STATUS_MASK_BIT & halLocationInfo.flags) {
        flags |= GNSS_LOCATION_INFO_DR_SOLUTION_STATUS_MASK_BIT;
    }

    locationInfo.gnssInfoFlags = (GnssLocationInfoFlagMask)flags;
    locationInfo.altitudeMeanSeaLevel = halLocationInfo.altitudeMeanSeaLevel;
    locationInfo.pdop = halLocationInfo.pdop;
    locationInfo.hdop = halLocationInfo.hdop;
    locationInfo.vdop = halLocationInfo.vdop;
    locationInfo.gdop = halLocationInfo.gdop;
    locationInfo.tdop = halLocationInfo.tdop;
    locationInfo.magneticDeviation = halLocationInfo.magneticDeviation;
    locationInfo.horReliability = parseLocationReliability(halLocationInfo.horReliability);
    locationInfo.verReliability = parseLocationReliability(halLocationInfo.verReliability);
    locationInfo.horUncEllipseSemiMajor = halLocationInfo.horUncEllipseSemiMajor;
    locationInfo.horUncEllipseSemiMinor = halLocationInfo.horUncEllipseSemiMinor;
    locationInfo.horUncEllipseOrientAzimuth = halLocationInfo.horUncEllipseOrientAzimuth;
    locationInfo.northStdDeviation = halLocationInfo.northStdDeviation;
    locationInfo.eastStdDeviation = halLocationInfo.eastStdDeviation;
    locationInfo.northVelocity = halLocationInfo.northVelocity;
    locationInfo.eastVelocity = halLocationInfo.eastVelocity;
    locationInfo.upVelocity = halLocationInfo.upVelocity;
    locationInfo.northVelocityStdDeviation = halLocationInfo.northVelocityStdDeviation;
    locationInfo.eastVelocityStdDeviation = halLocationInfo.eastVelocityStdDeviation;
    locationInfo.upVelocityStdDeviation = halLocationInfo.upVelocityStdDeviation;
    locationInfo.numSvUsedInPosition = halLocationInfo.numSvUsedInPosition;
    locationInfo.svUsedInPosition =
            parseLocationSvUsedInPosition(halLocationInfo.svUsedInPosition);
    locationInfo.locOutputEngType =
            (LocOutputEngineType)halLocationInfo.locOutputEngType;
    locationInfo.locOutputEngMask =
            (PositioningEngineMask)halLocationInfo.locOutputEngMask;
    locationInfo.conformityIndex = halLocationInfo.conformityIndex;
    locationInfo.llaVRPBased.latitude = halLocationInfo.llaVRPBased.latitude;
    locationInfo.llaVRPBased.longitude = halLocationInfo.llaVRPBased.longitude;
    locationInfo.llaVRPBased.altitude = halLocationInfo.llaVRPBased.altitude;
    // copy VRP-based north, east, up velocity
    locationInfo.enuVelocityVRPBased[0] = halLocationInfo.enuVelocityVRPBased[0];
    locationInfo.enuVelocityVRPBased[1] = halLocationInfo.enuVelocityVRPBased[1];
    locationInfo.enuVelocityVRPBased[2] = halLocationInfo.enuVelocityVRPBased[2];
    parseGnssMeasUsageInfo(halLocationInfo, locationInfo.measUsageInfo);
    locationInfo.drSolutionStatusMask = (DrSolutionStatusMask) halLocationInfo.drSolutionStatusMask;

    flags = 0;
    if (::LOCATION_SBAS_CORRECTION_IONO_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_CORRECTION_IONO_BIT;
    }
    if (::LOCATION_SBAS_CORRECTION_FAST_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_CORRECTION_FAST_BIT;
    }
    if (::LOCATION_SBAS_CORRECTION_LONG_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_CORRECTION_LONG_BIT;
    }
    if (::LOCATION_SBAS_INTEGRITY_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_SBAS_INTEGRITY_BIT;
    }
    if (::LOCATION_NAV_CORRECTION_DGNSS_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_DGNSS_BIT;
    }
    if (::LOCATION_NAV_CORRECTION_RTK_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_RTK_BIT;
    }
    if (::LOCATION_NAV_CORRECTION_RTK_FIXED_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_RTK_FIXED_BIT;
    }
    if (::LOCATION_NAV_CORRECTION_PPP_BIT & halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_PPP_BIT;
    }
    if (::LOCATION_NAV_CORRECTION_ONLY_SBAS_CORRECTED_SV_USED_BIT &
            halLocationInfo.navSolutionMask) {
        flags |= LOCATION_NAV_CORRECTION_ONLY_SBAS_CORRECTED_SV_USED_BIT;
    }
    locationInfo.navSolutionMask = (GnssLocationNavSolutionMask)flags;

    locationInfo.posTechMask = locationInfo.techMask;
    locationInfo.bodyFrameData = parseLocationPositionDynamics(
            halLocationInfo.bodyFrameData, halLocationInfo.bodyFrameDataExt);
    locationInfo.gnssSystemTime = parseSystemTime(halLocationInfo.gnssSystemTime);
    locationInfo.leapSeconds = halLocationInfo.leapSeconds;
    locationInfo.timeUncMs = halLocationInfo.timeUncMs;

    return locationInfo;
}

static GnssSv parseGnssSv(const ::GnssSv &halGnssSv) {
    GnssSv gnssSv;

    gnssSv.svId = halGnssSv.svId;
    switch (halGnssSv.type) {
        case ::GNSS_SV_TYPE_GPS:
            gnssSv.type = GNSS_SV_TYPE_GPS;
            break;

        case ::GNSS_SV_TYPE_SBAS:
            gnssSv.type = GNSS_SV_TYPE_SBAS;
            break;

        case ::GNSS_SV_TYPE_GLONASS:
            gnssSv.type = GNSS_SV_TYPE_GLONASS;
            break;

        case ::GNSS_SV_TYPE_QZSS:
            gnssSv.type = GNSS_SV_TYPE_QZSS;
            break;

        case ::GNSS_SV_TYPE_BEIDOU:
            gnssSv.type = GNSS_SV_TYPE_BEIDOU;
            break;

        case ::GNSS_SV_TYPE_GALILEO:
            gnssSv.type = GNSS_SV_TYPE_GALILEO;
            break;

        case ::GNSS_SV_TYPE_NAVIC:
            gnssSv.type = GNSS_SV_TYPE_NAVIC;
            break;

        default:
            gnssSv.type = GNSS_SV_TYPE_UNKNOWN;
            break;
    }
    gnssSv.cN0Dbhz = halGnssSv.cN0Dbhz;
    gnssSv.elevation = halGnssSv.elevation;
    gnssSv.azimuth = halGnssSv.azimuth;
    gnssSv.basebandCarrierToNoiseDbHz = halGnssSv.basebandCarrierToNoiseDbHz;

    uint32_t gnssSvOptionsMask = 0;
    if (::GNSS_SV_OPTIONS_HAS_EPHEMER_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_EPHEMER_BIT;
    }
    if (::GNSS_SV_OPTIONS_HAS_ALMANAC_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_ALMANAC_BIT;
    }
    if (::GNSS_SV_OPTIONS_USED_IN_FIX_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_USED_IN_FIX_BIT;
    }
    if (::GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_CARRIER_FREQUENCY_BIT;
    }
    if (::GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT & halGnssSv.gnssSvOptionsMask) {
        gnssSvOptionsMask |= GNSS_SV_OPTIONS_HAS_GNSS_SIGNAL_TYPE_BIT;
    }
    gnssSv.gnssSvOptionsMask = (GnssSvOptionsMask)gnssSvOptionsMask;

    gnssSv.carrierFrequencyHz = halGnssSv.carrierFrequencyHz;
    gnssSv.gnssSignalTypeMask = parseGnssSignalType(halGnssSv.gnssSignalTypeMask);
    return gnssSv;
}

static GnssData parseGnssData(const ::GnssDataNotification &halGnssData) {

    GnssData gnssData;

    for (int sig = GNSS_LOC_SIGNAL_TYPE_GPS_L1CA;
         sig < GNSS_LOC_MAX_NUMBER_OF_SIGNAL_TYPES; sig++) {
        gnssData.gnssDataMask[sig] =
                (location_client::GnssDataMask) halGnssData.gnssDataMask[sig];
        gnssData.jammerInd[sig] = halGnssData.jammerInd[sig];
        gnssData.agc[sig] = halGnssData.agc[sig];
        if (0 != gnssData.gnssDataMask[sig]) {
            LOC_LOGv("gnssDataMask[%d]=0x%X", sig, gnssData.gnssDataMask[sig]);
            LOC_LOGv("jammerInd[%d]=%f", sig, gnssData.jammerInd[sig]);
            LOC_LOGv("agc[%d]=%f", sig, gnssData.agc[sig]);
        }
    }
    return gnssData;
}

static GnssMeasurements parseGnssMeasurements(const ::GnssMeasurementsNotification
            &halGnssMeasurements) {
    GnssMeasurements gnssMeasurements = {};

    for (int meas = 0; meas < halGnssMeasurements.count; meas++) {
        GnssMeasurementsData measurement;

        measurement.flags =
                parseMeasurementsDataMask(halGnssMeasurements.measurements[meas].flags);
        measurement.svId = halGnssMeasurements.measurements[meas].svId;
        measurement.svType =
                (location_client::GnssSvType)halGnssMeasurements.measurements[meas].svType;
        measurement.timeOffsetNs = halGnssMeasurements.measurements[meas].timeOffsetNs;
        measurement.stateMask = (GnssMeasurementsStateMask)
                halGnssMeasurements.measurements[meas].stateMask;
        measurement.receivedSvTimeNs = halGnssMeasurements.measurements[meas].receivedSvTimeNs;
        measurement.receivedSvTimeUncertaintyNs =
                halGnssMeasurements.measurements[meas].receivedSvTimeUncertaintyNs;
        measurement.carrierToNoiseDbHz =
                halGnssMeasurements.measurements[meas].carrierToNoiseDbHz;
        measurement.pseudorangeRateMps =
                halGnssMeasurements.measurements[meas].pseudorangeRateMps;
        measurement.pseudorangeRateUncertaintyMps =
                halGnssMeasurements.measurements[meas].pseudorangeRateUncertaintyMps;
        measurement.adrStateMask = (GnssMeasurementsAdrStateMask)
                halGnssMeasurements.measurements[meas].adrStateMask;
        measurement.adrMeters = halGnssMeasurements.measurements[meas].adrMeters;
        measurement.adrUncertaintyMeters =
                halGnssMeasurements.measurements[meas].adrUncertaintyMeters;
        measurement.carrierFrequencyHz =
                halGnssMeasurements.measurements[meas].carrierFrequencyHz;
        measurement.carrierCycles = halGnssMeasurements.measurements[meas].carrierCycles;
        measurement.carrierPhase = halGnssMeasurements.measurements[meas].carrierPhase;
        measurement.carrierPhaseUncertainty =
                halGnssMeasurements.measurements[meas].carrierPhaseUncertainty;
        measurement.multipathIndicator = (location_client::GnssMeasurementsMultipathIndicator)
                halGnssMeasurements.measurements[meas].multipathIndicator;
        measurement.signalToNoiseRatioDb =
                halGnssMeasurements.measurements[meas].signalToNoiseRatioDb;
        measurement.agcLevelDb = halGnssMeasurements.measurements[meas].agcLevelDb;
        measurement.basebandCarrierToNoiseDbHz =
                halGnssMeasurements.measurements[meas].basebandCarrierToNoiseDbHz;
        measurement.gnssSignalType =
                parseGnssSignalType(halGnssMeasurements.measurements[meas].gnssSignalType);
        measurement.interSignalBiasNs =
                halGnssMeasurements.measurements[meas].fullInterSignalBiasNs;
        measurement.interSignalBiasUncertaintyNs =
                halGnssMeasurements.measurements[meas].fullInterSignalBiasUncertaintyNs;
        measurement.cycleSlipCount = halGnssMeasurements.measurements[meas].cycleSlipCount;

        gnssMeasurements.measurements.push_back(measurement);
    }
    gnssMeasurements.clock.flags =
            (GnssMeasurementsClockFlagsMask) halGnssMeasurements.clock.flags;
    gnssMeasurements.clock.leapSecond = halGnssMeasurements.clock.leapSecond;
    gnssMeasurements.clock.timeNs = halGnssMeasurements.clock.timeNs;
    gnssMeasurements.clock.timeUncertaintyNs = halGnssMeasurements.clock.timeUncertaintyNs;
    gnssMeasurements.clock.fullBiasNs = halGnssMeasurements.clock.fullBiasNs;
    gnssMeasurements.clock.biasNs = halGnssMeasurements.clock.biasNs;
    gnssMeasurements.clock.biasUncertaintyNs = halGnssMeasurements.clock.biasUncertaintyNs;
    gnssMeasurements.clock.driftNsps = halGnssMeasurements.clock.driftNsps;
    gnssMeasurements.clock.driftUncertaintyNsps = halGnssMeasurements.clock.driftUncertaintyNsps;
    gnssMeasurements.clock.hwClockDiscontinuityCount =
            halGnssMeasurements.clock.hwClockDiscontinuityCount;

    return gnssMeasurements;
}

static LocationResponse parseLocationError(::LocationError error) {
    LocationResponse response;

    switch (error) {
        case ::LOCATION_ERROR_SUCCESS:
            response = LOCATION_RESPONSE_SUCCESS;
            break;
        case ::LOCATION_ERROR_NOT_SUPPORTED:
            response = LOCATION_RESPONSE_NOT_SUPPORTED;
            break;
        default:
            response = LOCATION_RESPONSE_UNKOWN_FAILURE;
            break;
    }

    return response;
}

static LocationSystemInfo parseLocationSystemInfo(
        const::LocationSystemInfo &halSystemInfo) {
    LocationSystemInfo systemInfo = {};

    systemInfo.systemInfoMask = (location_client::LocationSystemInfoMask)
            halSystemInfo.systemInfoMask;
    if (halSystemInfo.systemInfoMask & ::LOCATION_SYS_INFO_LEAP_SECOND) {
        systemInfo.leapSecondSysInfo.leapSecondInfoMask = (location_client::LeapSecondSysInfoMask)
                halSystemInfo.leapSecondSysInfo.leapSecondInfoMask;

        if (halSystemInfo.leapSecondSysInfo.leapSecondInfoMask &
                ::LEAP_SECOND_SYS_INFO_LEAP_SECOND_CHANGE_BIT) {
            LeapSecondChangeInfo &clientInfo =
                    systemInfo.leapSecondSysInfo.leapSecondChangeInfo;
            const::LeapSecondChangeInfo &halInfo =
                    halSystemInfo.leapSecondSysInfo.leapSecondChangeInfo;

            clientInfo.gpsTimestampLsChange = parseGnssTime(halInfo.gpsTimestampLsChange);
            clientInfo.leapSecondsBeforeChange = halInfo.leapSecondsBeforeChange;
            clientInfo.leapSecondsAfterChange = halInfo.leapSecondsAfterChange;
        }

        if (halSystemInfo.leapSecondSysInfo.leapSecondInfoMask &
            ::LEAP_SECOND_SYS_INFO_CURRENT_LEAP_SECONDS_BIT) {
            systemInfo.leapSecondSysInfo.leapSecondCurrent =
                    halSystemInfo.leapSecondSysInfo.leapSecondCurrent;
        }
    }

    return systemInfo;
}

/******************************************************************************
ILocIpcListener override
******************************************************************************/
class IpcListener : public ILocIpcListener {
    MsgTask& mMsgTask;
    LocationClientApiImpl& mApiImpl;
    const SockNode::Type mSockTpye;
public:
    inline IpcListener(LocationClientApiImpl& apiImpl, MsgTask& msgTask,
                       const SockNode::Type sockType) :
            mMsgTask(msgTask), mApiImpl(apiImpl), mSockTpye(sockType) {}
    virtual void onListenerReady() override;
    virtual void onReceive(const char* data, uint32_t length,
                           const LocIpcRecver* recver) override;
};

/******************************************************************************
LocIpcQrtrWatcher override
******************************************************************************/
class IpcQrtrWatcher : public LocIpcQrtrWatcher {
    const weak_ptr<IpcListener> mIpcListener;
    const weak_ptr<LocIpcSender> mIpcSender;
    LocIpcQrtrWatcher::ServiceStatus mKnownStatus;
public:
    inline IpcQrtrWatcher(shared_ptr<IpcListener>& listener, shared_ptr<LocIpcSender>& sender) :
            LocIpcQrtrWatcher({LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID}),
            mIpcListener(listener), mIpcSender(sender),
            mKnownStatus(LocIpcQrtrWatcher::ServiceStatus::DOWN) {
    }
    inline virtual void onServiceStatusChange(int serviceId, int instanceId,
            LocIpcQrtrWatcher::ServiceStatus status, const LocIpcSender& refSender) {
        if (LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID == serviceId &&
            LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID == instanceId) {
            if (mKnownStatus != status) {
                mKnownStatus = status;
                if (LocIpcQrtrWatcher::ServiceStatus::UP == status) {
                    LOC_LOGv("case LocIpcQrtrWatcher::ServiceStatus::UP");
                    auto sender = mIpcSender.lock();
                    if (nullptr != sender) {
                        sender->copyDestAddrFrom(refSender);
                    }
                    auto listener = mIpcListener.lock();
                    if (nullptr != listener) {
                        const LocAPIHalReadyIndMsg msg(SERVICE_NAME);
                        listener->onReceive((const char*)&msg, sizeof(msg), nullptr);
                    }
                }
            }
        }
    }
};

/******************************************************************************
LocationClientApiImpl
******************************************************************************/

uint32_t  LocationClientApiImpl::mClientIdGenerator = LOCATION_CLIENT_SESSION_ID_INVALID;

mutex LocationClientApiImpl::mMutex;

/******************************************************************************
LocationClientApiImpl - constructors
******************************************************************************/
LocationClientApiImpl::LocationClientApiImpl(CapabilitiesCb capabitiescb) :
        mSessionId(LOCATION_CLIENT_SESSION_ID_INVALID),
        mBatchingId(LOCATION_CLIENT_SESSION_ID_INVALID),
        mHalRegistered(false),
        mCallbacksMask(0),
        mCapsMask((LocationCapabilitiesMask)0),
        mYearOfHw(0),
        mLastAddedClientIds({}),
        mCapabilitiesCb(capabitiescb),
        mResponseCb(nullptr),
        mPositionSessionResponseCbPending(false),
        mLocationCb(nullptr),
        mGnssLocationCb(nullptr),
        mEngLocationsCb(nullptr),
        mGnssSvCb(nullptr),
        mGnssNmeaCb(nullptr),
        mGnssDataCb(nullptr),
        mGnssMeasurementsCb(nullptr),
        mGnssEnergyConsumedInfoCb(nullptr),
        mGnssEnergyConsumedResponseCb(nullptr),
        mLocationSysInfoCb(nullptr),
        mLocationSysInfoResponseCb(nullptr),
        mPingTestCb(nullptr),
        mMsgTask("ClientApiImpl"),
        mLogger()
{
    // read configuration file
    UTIL_READ_CONF(LOC_PATH_GPS_CONF, gConfigTable);
    LOC_LOGd("gDebug=%u", gDebug);

    // get pid to generate sokect name
    uint32_t pid = (uint32_t)getpid();

#ifdef FEATURE_EXTERNAL_AP
    // The instance id is composed from pid and client id.
    // We support up to 32 unique client api within one process.
    // Each client id is tracked via a bit in mClientIdGenerator,
    // which is 4 bytes now.
    lock_guard<mutex> lock(mMutex);
    unsigned int clientIdMask = 1;
    // find a bit in the mClientIdGenerator that is not yet used
    // and use that as client id
    // client id will be from 1 to 32, as client id will be used to
    // set session id and 0 is reserved for LOCATION_CLIENT_SESSION_ID_INVALID
    for (mClientId = 1; mClientId <= sizeof(mClientIdGenerator) * 8; mClientId++) {
        if ((mClientIdGenerator & (1UL << (mClientId-1))) == 0) {
            mClientIdGenerator |= (1UL << (mClientId-1));
            break;
        }
    }

    if (mClientId > sizeof(mClientIdGenerator) * 8) {
        LOC_LOGe("create Qsocket failed, already use up maximum of %d clients",
                 sizeof(mClientIdGenerator)*8);
        return;
    }

    SockNodeEap sock(LOCATION_CLIENT_API_QSOCKET_CLIENT_SERVICE_ID,
                     pid * 100 + mClientId);
    strlcpy(mSocketName, sock.getNodePathname().c_str(), sizeof(mSocketName));

    // establish an ipc sender to the hal daemon
    mIpcSender = LocIpc::getLocIpcQrtrSender(LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID,
                                     LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID);
    if (mIpcSender == nullptr) {
        LOC_LOGe("create sender socket failed for service id: %d instance id: %d",
                 LOCATION_CLIENT_API_QSOCKET_HALDAEMON_SERVICE_ID,
                 LOCATION_CLIENT_API_QSOCKET_HALDAEMON_INSTANCE_ID);
        return;
    }
    shared_ptr<IpcListener> listener(make_shared<IpcListener>(*this, mMsgTask, SockNode::Eap));
    unique_ptr<LocIpcRecver> recver = LocIpc::getLocIpcQrtrRecver(listener,
            sock.getId1(), sock.getId2(), make_shared<IpcQrtrWatcher>(listener, mIpcSender));
#else
    // get clientId
    lock_guard<mutex> lock(mMutex);
    mClientId = ++mClientIdGenerator;
    // make sure client ID is not equal to LOCATION_CLIENT_SESSION_ID_INVALID,
    // as session id will be assigned to client ID
    if (mClientId == LOCATION_CLIENT_SESSION_ID_INVALID) {
        mClientId = ++mClientIdGenerator;
    }

    SockNodeLocal sock(LOCATION_CLIENT_API, pid, mClientId);
    strlcpy(mSocketName, sock.getNodePathname().c_str(), sizeof(mSocketName));

    // establish an ipc sender to the hal daemon
    mIpcSender = LocIpc::getLocIpcLocalSender(SOCKET_TO_LOCATION_HAL_DAEMON);
    if (mIpcSender == nullptr) {
        LOC_LOGe("create sender socket failed %s", SOCKET_TO_LOCATION_HAL_DAEMON);
        return;
    }
    unique_ptr<LocIpcRecver> recver = LocIpc::getLocIpcLocalRecver(
            make_shared<IpcListener>(*this, mMsgTask, SockNode::Local), mSocketName);
#endif

    LOC_LOGd("listen on socket: %s", mSocketName);
    mIpc.startNonBlockingListening(recver);
}

LocationClientApiImpl::~LocationClientApiImpl() {
}

void LocationClientApiImpl::destroy() {

    struct DestroyReq : public LocMsg {
        DestroyReq(LocationClientApiImpl* apiImpl) :
                mApiImpl(apiImpl) {}
        virtual ~DestroyReq() {}
        void proc() const {
            // deregister
            if (mApiImpl->mHalRegistered && (nullptr != mApiImpl->mIpcSender)) {
                LocAPIClientDeregisterReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
                LOC_LOGd(">>> DeregisterReq rc=%d\n", rc);
                mApiImpl->mIpcSender = nullptr;
            }

#ifdef FEATURE_EXTERNAL_AP
            {
                // get clientId
                lock_guard<mutex> lock(mMutex);
                mApiImpl->mClientIdGenerator &= ~(1UL << mApiImpl->mClientId);
                LOC_LOGd("client id generarator 0x%x, id %d",
                         mApiImpl->mClientIdGenerator, mApiImpl->mClientId);
            }
#endif //FEATURE_EXTERNAL_AP
            delete mApiImpl;
        }
        LocationClientApiImpl* mApiImpl;
    };

    mMsgTask.sendMsg(new (nothrow) DestroyReq(this));
}

/******************************************************************************
LocationClientApiImpl - implementation
******************************************************************************/
void LocationClientApiImpl::updateCallbackFunctions(const ClientCallbacks& cbs,
                                                    ReportCbEnumType reportCbType) {

    struct UpdateCallbackFunctionsReq : public LocMsg {
        UpdateCallbackFunctionsReq(LocationClientApiImpl* apiImpl, const ClientCallbacks& cbs,
                                   ReportCbEnumType reportCbType) :
                mApiImpl(apiImpl), mCbs(cbs), mReportCbType(reportCbType) {}
        virtual ~UpdateCallbackFunctionsReq() {}
        void proc() const {
            // update callback functions
            mApiImpl->mResponseCb = mCbs.responsecb;
            mApiImpl->mCollectiveResCb = mCbs.collectivecb;
            mApiImpl->mLocationCb = mCbs.locationcb;
            mApiImpl->mBatchingCb = mCbs.batchingcb;
            mApiImpl->mGfBreachCb = mCbs.gfbreachcb;

            if (REPORT_CB_GNSS_INFO == mReportCbType) {
                mApiImpl->mGnssLocationCb     = mCbs.gnssreportcbs.gnssLocationCallback;
                mApiImpl->mGnssSvCb           = mCbs.gnssreportcbs.gnssSvCallback;
                mApiImpl->mGnssNmeaCb         = mCbs.gnssreportcbs.gnssNmeaCallback;
                mApiImpl->mGnssDataCb         = mCbs.gnssreportcbs.gnssDataCallback;
                mApiImpl->mGnssMeasurementsCb = mCbs.gnssreportcbs.gnssMeasurementsCallback;
            } else if (REPORT_CB_ENGINE_INFO == mReportCbType) {
                mApiImpl->mEngLocationsCb     = mCbs.engreportcbs.engLocationsCallback;
                mApiImpl->mGnssSvCb           = mCbs.engreportcbs.gnssSvCallback;
                mApiImpl->mGnssNmeaCb         = mCbs.engreportcbs.gnssNmeaCallback;
                mApiImpl->mGnssDataCb         = mCbs.engreportcbs.gnssDataCallback;
                mApiImpl->mGnssMeasurementsCb = mCbs.engreportcbs.gnssMeasurementsCallback;
            }
        }
        LocationClientApiImpl* mApiImpl;
        const ClientCallbacks mCbs;
        ReportCbEnumType mReportCbType;
    };
    mMsgTask.sendMsg(new (nothrow) UpdateCallbackFunctionsReq(this, cbs, reportCbType));
}

void LocationClientApiImpl::updateCallbacks(LocationCallbacks& callbacks) {

    struct UpdateCallbacksReq : public LocMsg {
        UpdateCallbacksReq(LocationClientApiImpl* apiImpl, const LocationCallbacks& callbacks) :
                mApiImpl(apiImpl), mCallBacks(callbacks) {}
        virtual ~UpdateCallbacksReq() {}
        void proc() const {
            // set up the flag to indicate that responseCb is pending
            mApiImpl->mPositionSessionResponseCbPending = true;

            //convert callbacks to callBacksMask
            LocationCallbacksMask callBacksMask = 0;
            if (mCallBacks.trackingCb) {
                callBacksMask |= E_LOC_CB_DISTANCE_BASED_TRACKING_BIT;
            }
            if (mCallBacks.gnssLocationInfoCb) {
                if (mApiImpl->mLocationCb) {
                    callBacksMask |= E_LOC_CB_SIMPLE_LOCATION_INFO_BIT;
                } else {
                    callBacksMask |= E_LOC_CB_GNSS_LOCATION_INFO_BIT;
                }
            }
            if (mCallBacks.engineLocationsInfoCb) {
                callBacksMask |= E_LOC_CB_ENGINE_LOCATIONS_INFO_BIT;
            }
            if (mCallBacks.gnssSvCb) {
                callBacksMask |= E_LOC_CB_GNSS_SV_BIT;
            }
            if (mCallBacks.gnssNmeaCb) {
                callBacksMask |= E_LOC_CB_GNSS_NMEA_BIT;
            }
            if (mCallBacks.gnssDataCb) {
                callBacksMask |= E_LOC_CB_GNSS_DATA_BIT;
            }
            if (mCallBacks.gnssMeasurementsCb) {
                callBacksMask |= E_LOC_CB_GNSS_MEAS_BIT;
            }
            // handle callbacks that are not related to a fix session
            if (mApiImpl->mLocationSysInfoCb) {
                callBacksMask |= E_LOC_CB_SYSTEM_INFO_BIT;
            }
            if (mCallBacks.batchingCb) {
                callBacksMask |= E_LOC_CB_BATCHING_BIT;
            }
            if (mCallBacks.batchingStatusCb) {
                callBacksMask |= E_LOC_CB_BATCHING_STATUS_BIT;
            }
            if (mCallBacks.geofenceBreachCb) {
                callBacksMask |= E_LOC_CB_GEOFENCE_BREACH_BIT;
            }

            // update callback only when changed
            if (mApiImpl->mCallbacksMask != callBacksMask) {
                mApiImpl->mCallbacksMask = callBacksMask;
                if (mApiImpl->mHalRegistered) {
                    LocAPIUpdateCallbacksReqMsg msg(mApiImpl->mSocketName,
                                                    mApiImpl->mCallbacksMask);
                    bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                                         sizeof(msg));
                    LOC_LOGd(">>> UpdateCallbacksReq callBacksMask=0x%x rc=%d",
                             mApiImpl->mCallbacksMask, rc);
                }
            } else {
                LOC_LOGd("No updateCallbacks because same callBacksMask 0x%x", callBacksMask);
            }
        }
        LocationClientApiImpl* mApiImpl;
        const LocationCallbacks mCallBacks;
    };
    mMsgTask.sendMsg(new (nothrow) UpdateCallbacksReq(this, callbacks));
}

uint32_t LocationClientApiImpl::startTracking(TrackingOptions& option) {

    struct StartTrackingReq : public LocMsg {
        StartTrackingReq(LocationClientApiImpl* apiImpl, TrackingOptions& option) :
                mApiImpl(apiImpl), mOption(option) {}
        virtual ~StartTrackingReq() {}
        void proc() const {
            // check if option is updated
            bool isOptionUpdated = false;

            if ((mApiImpl->mLocationOptions.minInterval != mOption.minInterval) ||
                (mApiImpl->mLocationOptions.minDistance != mOption.minDistance) ||
                (mApiImpl->mLocationOptions.locReqEngTypeMask != mOption.locReqEngTypeMask)) {
                isOptionUpdated = true;
            }

            if (!mApiImpl->mHalRegistered) {
                mApiImpl->mLocationOptions = mOption;
                // need to set session id so when hal is ready, the session can be resumed
                mApiImpl->mSessionId = mApiImpl->mClientId;
                LOC_LOGe(">>> startTracking - Not registered yet");
                return;
            }

            if (LOCATION_CLIENT_SESSION_ID_INVALID == mApiImpl->mSessionId) {
                mApiImpl->mLocationOptions = mOption;
                //start a new tracking session
                mApiImpl->mSessionId = mApiImpl->mClientId;

                if ((0 != mApiImpl->mLocationOptions.minInterval) ||
                    (0 != mApiImpl->mLocationOptions.minDistance)) {
                    LocAPIStartTrackingReqMsg msg(mApiImpl->mSocketName,
                                                  mApiImpl->mLocationOptions);
                    bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                                    sizeof(msg));
                    LOC_LOGd(">>> StartTrackingReq Interval=%d Distance=%d, locReqEngTypeMask=0x%x",
                             mApiImpl->mLocationOptions.minInterval,
                             mApiImpl->mLocationOptions.minDistance,
                             mApiImpl->mLocationOptions.locReqEngTypeMask);
                }
            } else if (isOptionUpdated) {
                //update a tracking session, mApiImpl->mLocationOptions
                //will be updated in updateTrackingOptionsSync
                mApiImpl->updateTrackingOptionsSync(
                        mApiImpl, const_cast<TrackingOptions&>(mOption));
            } else {
                LOC_LOGd(">>> StartTrackingReq - no change in option");
                mApiImpl->invokePositionSessionResponseCb(LOCATION_RESPONSE_SUCCESS);
            }
        }
        LocationClientApiImpl* mApiImpl;
        TrackingOptions mOption;
    };
    mMsgTask.sendMsg(new (nothrow) StartTrackingReq(this, option));
    return 0;
}

void LocationClientApiImpl::stopTracking(uint32_t) {

    struct StopTrackingReq : public LocMsg {
        StopTrackingReq(LocationClientApiImpl* apiImpl) : mApiImpl(apiImpl) {}
        virtual ~StopTrackingReq() {}
        void proc() const {
            if (mApiImpl->mSessionId == mApiImpl->mClientId) {
                if (mApiImpl->mHalRegistered &&
                        ((mApiImpl->mLocationOptions.minInterval != 0) ||
                         (mApiImpl->mLocationOptions.minDistance != 0))) {
                    LocAPIStopTrackingReqMsg msg(mApiImpl->mSocketName);
                    bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                                        sizeof(msg));
                    LOC_LOGd(">>> StopTrackingReq rc=%d\n", rc);
                }

                mApiImpl->mLocationOptions.minInterval = 0;
                mApiImpl->mLocationOptions.minDistance = 0;
                mApiImpl->mCallbacksMask = 0;
                // handle callback that are not tied with fix session
                if (mApiImpl->mLocationSysInfoCb) {
                    mApiImpl->mCallbacksMask |= E_LOC_CB_SYSTEM_INFO_BIT;
                }
            }
            mApiImpl->mSessionId = LOCATION_CLIENT_SESSION_ID_INVALID;
        }
        LocationClientApiImpl* mApiImpl;
    };
    mMsgTask.sendMsg(new (nothrow) StopTrackingReq(this));
}

void LocationClientApiImpl::updateTrackingOptionsSync(
        LocationClientApiImpl* pImpl, TrackingOptions& option) {

    LOC_LOGd(">>> updateTrackingOptionsSync,sessionId=%d, "
             "new Interval=%d Distance=%d, current Interval=%d Distance=%d",
             pImpl->mSessionId, option.minInterval,
             option.minDistance, pImpl->mLocationOptions.minInterval,
             pImpl->mLocationOptions.minDistance);

    bool rc = true;
    // update option to passive listening where previous option
    // is not passive listening, in this case, we need to stop the session
    if (((option.minInterval == 0) && (option.minDistance == 0)) &&
            ((pImpl->mLocationOptions.minInterval != 0) ||
             (pImpl->mLocationOptions.minDistance != 0))) {
        LocAPIStopTrackingReqMsg msg(pImpl->mSocketName);
        rc = pImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                        sizeof(msg));
    } else if (((option.minInterval != 0) || (option.minDistance != 0)) &&
               ((pImpl->mLocationOptions.minInterval == 0) &&
                (pImpl->mLocationOptions.minDistance == 0))) {
        // update option from passive listening to none passive listening,
        // we need to start the session
        LocAPIStartTrackingReqMsg msg(pImpl->mSocketName, option);
        rc = pImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                        sizeof(msg));
        LOC_LOGd(">>> start tracking Interval=%d Distance=%d",
                 option.minInterval, option.minDistance);
    } else {
        LocAPIUpdateTrackingOptionsReqMsg msg(pImpl->mSocketName, option);
        bool rc = pImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
        LOC_LOGd(">>> updateTrackingOptionsSync Interval=%d Distance=%d, reqTypeMask=0x%x",
                option.minInterval, option.minDistance, option.locReqEngTypeMask);
    }

    pImpl->mLocationOptions = option;
}


//Batching
uint32_t LocationClientApiImpl::startBatching(BatchingOptions& batchOptions) {
    struct StartBatchingReq : public LocMsg {
        StartBatchingReq(LocationClientApiImpl *apiImpl, BatchingOptions& batchOptions) :
            mApiImpl(apiImpl), mBatchOptions(batchOptions) {}
        virtual ~StartBatchingReq() {}
        void proc() const {
            mApiImpl->mBatchingOptions = mBatchOptions;
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> startBatching - Not registered yet");
                return;
            }
            if (LOCATION_CLIENT_SESSION_ID_INVALID == mApiImpl->mSessionId) {
                //start a new batching session
                mApiImpl->mBatchingId = mApiImpl->mClientId;
                LocAPIStartBatchingReqMsg msg(mApiImpl->mSocketName,
                        mApiImpl->mBatchingOptions.minInterval,
                        mApiImpl->mBatchingOptions.minDistance,
                        mApiImpl->mBatchingOptions.batchingMode);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                        sizeof(msg));
                LOC_LOGd(">>> StartBatchingReq Interval=%d Distance=%d BatchingMode=%d",
                        mApiImpl->mBatchingOptions.minInterval,
                        mApiImpl->mBatchingOptions.minDistance,
                        mApiImpl->mBatchingOptions.batchingMode);
            } else {
                mApiImpl->updateBatchingOptions(mApiImpl->mBatchingId,
                        const_cast<BatchingOptions&>(mBatchOptions));
            }
        }
        LocationClientApiImpl *mApiImpl;
        BatchingOptions mBatchOptions;
    };
    mMsgTask.sendMsg(new (nothrow) StartBatchingReq(this, batchOptions));
    return 0;
}

void LocationClientApiImpl::stopBatching(uint32_t id) {
    struct StopBatchingReq : public LocMsg {
        StopBatchingReq(LocationClientApiImpl *apiImpl) :
            mApiImpl(apiImpl) {}
        virtual ~StopBatchingReq() {}
        void proc() const {
            if (mApiImpl->mBatchingId == mApiImpl->mClientId) {
                mApiImpl->mBatchingOptions = {};
                LocAPIStopBatchingReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                        sizeof(msg));
                LOC_LOGd(">>> StopBatchingReq rc=%d", rc);
            }
            mApiImpl->mBatchingId = LOCATION_CLIENT_SESSION_ID_INVALID;
        }
        LocationClientApiImpl *mApiImpl;
    };
    mMsgTask.sendMsg(new (nothrow) StopBatchingReq(this));
}

void LocationClientApiImpl::updateBatchingOptions(uint32_t id, BatchingOptions& batchOptions) {
    struct UpdateBatchingOptionsReq : public LocMsg {
        UpdateBatchingOptionsReq(LocationClientApiImpl* apiImpl, BatchingOptions& batchOptions) :
            mApiImpl(apiImpl), mBatchOptions(batchOptions) {}
        virtual ~UpdateBatchingOptionsReq() {}
        void proc() const {
            mApiImpl->mBatchingOptions = mBatchOptions;
            LocAPIUpdateBatchingOptionsReqMsg msg(mApiImpl->mSocketName,
                    mApiImpl->mBatchingOptions.minInterval,
                    mApiImpl->mBatchingOptions.minDistance,
                    mApiImpl->mBatchingOptions.batchingMode);
            bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                    sizeof(msg));
            LOC_LOGd(">>> StartBatchingReq Interval=%d Distance=%d BatchingMode=%d",
                    mApiImpl->mBatchingOptions.minInterval,
                    mApiImpl->mBatchingOptions.minDistance,
                    mApiImpl->mBatchingOptions.batchingMode);
        }
        LocationClientApiImpl *mApiImpl;
        BatchingOptions mBatchOptions;
    };

    if ((mBatchingOptions.minInterval != batchOptions.minInterval) ||
            (mBatchingOptions.minDistance != batchOptions.minDistance) ||
            mBatchingOptions.batchingMode != batchOptions.batchingMode) {
        mMsgTask.sendMsg(new (nothrow) UpdateBatchingOptionsReq(this, batchOptions));
    } else {
        LOC_LOGd("No UpdateBatchingOptions because same Interval=%d Distance=%d, BatchingMode=%d",
                batchOptions.minInterval, batchOptions.minDistance, batchOptions.batchingMode);
    }
}

void LocationClientApiImpl::getBatchedLocations(uint32_t id, size_t count) {}

bool LocationClientApiImpl::checkGeofenceMap(size_t count, uint32_t* ids) {
    for (int i=0; i<count; ++i) {
        auto got = mGeofenceMap.find(ids[i]);
        if (got == mGeofenceMap.end()) {
            return false;
        }
    }
    return true;
}
void LocationClientApiImpl::addGeofenceMap(uint32_t id, Geofence& geofence) {
    lock_guard<mutex> lock(mMutex);
    mGeofenceMap.insert(make_pair(id, geofence));
}
void LocationClientApiImpl::eraseGeofenceMap(size_t count, uint32_t* ids) {
    lock_guard<mutex> lock(mMutex);
    for (int i=0; i<count; ++i) {
        mGeofenceMap.erase(ids[i]);
    }
}

uint32_t* LocationClientApiImpl::addGeofences(size_t count, GeofenceOption* options,
        GeofenceInfo* infos) {
    struct AddGeofencesReq : public LocMsg {
        AddGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, GeofenceOption* gfOptions,
                GeofenceInfo* gfInfos, std::vector<uint32_t> clientIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfOptions(gfOptions), mGfInfos(gfInfos),
                mClientIds(clientIds) {}
        virtual ~AddGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> addGeofences - Not registered yet");
                return;
            }

            if (mGfCount > 0) {
                //Add geofences, serialize geofence msg payload into ipc message payload
                size_t msglen = sizeof(LocAPIAddGeofencesReqMsg) +
                        sizeof(GeofencePayload) * (mGfCount - 1);
                uint8_t *msg = new(std::nothrow) uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                memset(msg, 0, msglen);
                LocAPIAddGeofencesReqMsg *pMsg = reinterpret_cast<LocAPIAddGeofencesReqMsg*>(msg);
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                pMsg->msgVersion = LOCATION_REMOTE_API_MSG_VERSION;
                pMsg->msgId = E_LOCAPI_ADD_GEOFENCES_MSG_ID;
                pMsg->msgVersion = LOCATION_REMOTE_API_MSG_VERSION;
                pMsg->geofences.count = mGfCount;
                for (int i=0; i<mGfCount; ++i) {
                    (*(pMsg->geofences.gfPayload + i)).gfClientId = mClientIds[i];
                    (*(pMsg->geofences.gfPayload + i)).gfOption = mGfOptions[i];
                    (*(pMsg->geofences.gfPayload + i)).gfInfo = mGfInfos[i];
                }
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> AddGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfOptions);
                free(mGfInfos);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        GeofenceOption* mGfOptions;
        GeofenceInfo* mGfInfos;
        std::vector<uint32_t>  mClientIds;
    };
    mMsgTask.sendMsg(new (nothrow) AddGeofencesReq(this, count, options, infos,
            mLastAddedClientIds));
    return nullptr;
}

void LocationClientApiImpl::removeGeofences(size_t count, uint32_t* ids) {
    struct RemoveGeofencesReq : public LocMsg {
        RemoveGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds) {}
        virtual ~RemoveGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> removeGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Remove geofences
                size_t msglen = sizeof(LocAPIRemoveGeofencesReqMsg) +
                        sizeof(uint32_t) * (mGfCount - 1);
                uint8_t* msg = new uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                LocAPIRemoveGeofencesReqMsg* pMsg = (LocAPIRemoveGeofencesReqMsg*)msg;
                pMsg->gfClientIds.count = mGfCount;
                memcpy(pMsg->gfClientIds.gfIds, mGfIds, sizeof(uint32_t) * mGfCount);
                pMsg->msgVersion = LOCATION_REMOTE_API_MSG_VERSION;
                pMsg->msgId = E_LOCAPI_REMOVE_GEOFENCES_MSG_ID;
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> RemoveGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
    };
    mMsgTask.sendMsg(new (nothrow) RemoveGeofencesReq(this, count, ids));
}

void LocationClientApiImpl::modifyGeofences(
        size_t count, uint32_t* ids, GeofenceOption* options) {
    struct ModifyGeofencesReq : public LocMsg {
        ModifyGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds,
                GeofenceOption* gfOptions) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds), mGfOptions(gfOptions) {}
        virtual ~ModifyGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> modifyGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Modify geofences
                size_t msglen = sizeof(LocAPIModifyGeofencesReqMsg) +
                        sizeof(GeofencePayload) * (mGfCount-1);
                uint8_t *msg = new(std::nothrow) uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                memset(msg, 0, msglen);
                LocAPIModifyGeofencesReqMsg *pMsg =
                        reinterpret_cast<LocAPIModifyGeofencesReqMsg*>(msg);
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                pMsg->msgVersion = LOCATION_REMOTE_API_MSG_VERSION;
                pMsg->msgId = E_LOCAPI_MODIFY_GEOFENCES_MSG_ID;
                pMsg->geofences.count = mGfCount;
                for (int i=0; i<mGfCount; ++i) {
                    (*(pMsg->geofences.gfPayload + i)).gfClientId = mGfIds[i];
                    (*(pMsg->geofences.gfPayload + i)).gfOption = mGfOptions[i];
                }
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> ModifyGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
                free(mGfOptions);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
        GeofenceOption* mGfOptions;
    };
    mMsgTask.sendMsg(new (nothrow) ModifyGeofencesReq(this, count, ids, options));
}

void LocationClientApiImpl::pauseGeofences(size_t count, uint32_t* ids) {
    struct PauseGeofencesReq : public LocMsg {
        PauseGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds) {}
        virtual ~PauseGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> pauseGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Pause geofences
                size_t msglen = sizeof(LocAPIPauseGeofencesReqMsg) +
                        sizeof(uint32_t) * (mGfCount - 1);
                uint8_t* msg = new uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                LocAPIPauseGeofencesReqMsg* pMsg = (LocAPIPauseGeofencesReqMsg*)msg;
                pMsg->gfClientIds.count = mGfCount;
                memcpy(&(pMsg->gfClientIds.gfIds[0]), mGfIds, sizeof(uint32_t) * mGfCount);
                pMsg->msgVersion = LOCATION_REMOTE_API_MSG_VERSION;
                pMsg->msgId = E_LOCAPI_PAUSE_GEOFENCES_MSG_ID;
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> PauseGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
    };
    mMsgTask.sendMsg(new (nothrow) PauseGeofencesReq(this, count, ids));
}

void LocationClientApiImpl::resumeGeofences(size_t count, uint32_t* ids) {
    struct ResumeGeofencesReq : public LocMsg {
        ResumeGeofencesReq(LocationClientApiImpl* apiImpl, uint32_t count, uint32_t* gfIds) :
                mApiImpl(apiImpl), mGfCount(count), mGfIds(gfIds) {}
        virtual ~ResumeGeofencesReq() {}
        void proc() const {
            if (!mApiImpl->mHalRegistered) {
                LOC_LOGe(">>> resumeGeofences - Not registered yet");
                return;
            }
            if (mGfCount > 0) {
                //Resume geofences
                size_t msglen = sizeof(LocAPIResumeGeofencesReqMsg) +
                    sizeof(uint32_t) * (mGfCount - 1);
                uint8_t* msg = new uint8_t[msglen];
                if (nullptr == msg) {
                    return;
                }
                LocAPIResumeGeofencesReqMsg* pMsg = (LocAPIResumeGeofencesReqMsg*)msg;
                pMsg->gfClientIds.count = mGfCount;
                memcpy(pMsg->gfClientIds.gfIds, mGfIds, sizeof(uint32_t) * mGfCount);
                pMsg->msgVersion = LOCATION_REMOTE_API_MSG_VERSION;
                pMsg->msgId = E_LOCAPI_RESUME_GEOFENCES_MSG_ID;
                strlcpy(pMsg->mSocketName, mApiImpl->mSocketName, MAX_SOCKET_PATHNAME_LENGTH);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(msg),
                        msglen);
                LOC_LOGd(">>> ResumeGeofencesReq count=%zu", mGfCount);
                delete[] msg;
                free(mGfIds);
            }
        }
        LocationClientApiImpl *mApiImpl;
        uint32_t mGfCount;
        uint32_t* mGfIds;
    };
    mMsgTask.sendMsg(new (nothrow) ResumeGeofencesReq(this, count, ids));
}

void LocationClientApiImpl::updateNetworkAvailability(bool available) {

    struct UpdateNetworkAvailabilityReq : public LocMsg {
        UpdateNetworkAvailabilityReq(const LocationClientApiImpl* apiImpl, bool available) :
                mApiImpl(apiImpl), mAvailable(available) {}
        virtual ~UpdateNetworkAvailabilityReq() {}
        void proc() const {
            LocAPIUpdateNetworkAvailabilityReqMsg msg(mApiImpl->mSocketName,
                                                      mAvailable);
            bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                                 sizeof(msg));
            LOC_LOGd(">>> UpdateNetworkAvailabilityReq available=%d ", mAvailable);
        }
        const LocationClientApiImpl* mApiImpl;
        const bool mAvailable;
    };
    mMsgTask.sendMsg(new (nothrow) UpdateNetworkAvailabilityReq(this, available));
}

void LocationClientApiImpl::getGnssEnergyConsumed(
        GnssEnergyConsumedCb gnssEnergyConsumedCallback,
        ResponseCb responseCallback) {

    struct GetGnssEnergyConsumedReq : public LocMsg {
        GetGnssEnergyConsumedReq(LocationClientApiImpl *apiImpl,
                                 GnssEnergyConsumedCb gnssEnergyConsumedCb,
                                 ResponseCb responseCb) :
        mApiImpl(apiImpl),
        mGnssEnergyConsumedCb(gnssEnergyConsumedCb),
        mResponseCb(responseCb) {}

        virtual ~GetGnssEnergyConsumedReq() {}
        void proc() const {
            mApiImpl->mGnssEnergyConsumedInfoCb = mGnssEnergyConsumedCb;
            mApiImpl->mGnssEnergyConsumedResponseCb = mResponseCb;

            // send msg to the hal daemon
            if (nullptr != mApiImpl->mGnssEnergyConsumedInfoCb) {
                LocAPIGetGnssEnergyConsumedReqMsg msg(mApiImpl->mSocketName);
                bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                           sizeof(msg));
                if (mResponseCb) {
                    if (true == rc) {
                        mResponseCb(LOCATION_RESPONSE_SUCCESS);
                    } else {
                        mResponseCb(LOCATION_RESPONSE_UNKOWN_FAILURE);
                    }
                }
            }
        }

        LocationClientApiImpl *mApiImpl;
        GnssEnergyConsumedCb   mGnssEnergyConsumedCb;
        ResponseCb             mResponseCb;
    };

    LOC_LOGd(">>> getGnssEnergyConsumed ");
    mMsgTask.sendMsg(new (nothrow)GetGnssEnergyConsumedReq(
            this, gnssEnergyConsumedCallback, responseCallback));
}

void LocationClientApiImpl::updateLocationSystemInfoListener(
    LocationSystemInfoCb locSystemInfoCallback,
    ResponseCb responseCallback) {

    struct UpdateLocationSystemInfoListenerReq : public LocMsg {
        UpdateLocationSystemInfoListenerReq(LocationClientApiImpl *apiImpl,
                                       LocationSystemInfoCb sysInfoCb,
                                       ResponseCb responseCb) :
        mApiImpl(apiImpl),
        mLocSysInfoCb(sysInfoCb),
        mResponseCb(responseCb) {}

        virtual ~UpdateLocationSystemInfoListenerReq() {}
        void proc() const {
            bool needIpc = false;
            LocationCallbacksMask callbackMaskCopy = mApiImpl->mCallbacksMask;
            // send msg to the hal daemon if the registration changes
            if ((nullptr != mLocSysInfoCb) &&
                (nullptr == mApiImpl->mLocationSysInfoCb)) {
                // client registers for system info, set up the bit
                mApiImpl->mCallbacksMask |= E_LOC_CB_SYSTEM_INFO_BIT;
                needIpc = true;
            } else if ((nullptr == mLocSysInfoCb) &&
                       (nullptr != mApiImpl->mLocationSysInfoCb)) {
                // system info is no longer needed, clear the bit
                mApiImpl->mCallbacksMask &= ~E_LOC_CB_SYSTEM_INFO_BIT;
                needIpc = true;
            }

            // save the new callback
            mApiImpl->mLocationSysInfoCb = mLocSysInfoCb;
            mApiImpl->mLocationSysInfoResponseCb = mResponseCb;

            // inform hal daemon of updated callback only when changed
            if (needIpc == true) {
                if (mApiImpl->mHalRegistered) {
                    LocAPIUpdateCallbacksReqMsg msg(mApiImpl->mSocketName,
                                                    mApiImpl->mCallbacksMask);
                    bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg),
                                                         sizeof(msg));
                    LOC_LOGd(">>> UpdateCallbacksReq new callBacksMask=0x%x, "
                             "old mask =0x%x, rc=%d",
                             mApiImpl->mCallbacksMask, callbackMaskCopy, rc);
                    if (mResponseCb) {
                        if (true == rc) {
                            mResponseCb(LOCATION_RESPONSE_SUCCESS);
                        } else {
                            mResponseCb(LOCATION_RESPONSE_UNKOWN_FAILURE);
                        }
                    }
                }
            } else {
                if (mResponseCb) {
                    mResponseCb(LOCATION_RESPONSE_SUCCESS);
                }
                LOC_LOGd("No updateCallbacks because same callback");
            }
        }

        LocationClientApiImpl *mApiImpl;
        LocationSystemInfoCb   mLocSysInfoCb;
        ResponseCb             mResponseCb;
    };

    LOC_LOGd(">>> updateLocationSystemInfoListener ");
    mMsgTask.sendMsg(new (nothrow)UpdateLocationSystemInfoListenerReq(
            this, locSystemInfoCallback, responseCallback));
}

/******************************************************************************
LocationClientApiImpl - LocIpc onReceive handler
******************************************************************************/
void LocationClientApiImpl::capabilitesCallback(ELocMsgID msgId, const void* msgData) {

    mHalRegistered = true;
    const LocAPICapabilitiesIndMsg* pCapabilitiesIndMsg =
            (LocAPICapabilitiesIndMsg*)(msgData);
    mCapsMask = parseCapabilitiesMask(pCapabilitiesIndMsg->capabilitiesMask);

    if (mCapabilitiesCb) {
        mCapabilitiesCb(mCapsMask);
    }

    mYearOfHw = parseYearOfHw(pCapabilitiesIndMsg->capabilitiesMask);

    // send updatecallback request
    if (0 != mCallbacksMask) {
        LocAPIUpdateCallbacksReqMsg msg(mSocketName, mCallbacksMask);
        bool rc = sendMessage(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
        LOC_LOGd(">>> UpdateCallbacksReq callBacksMask=0x%x rc=%d", mCallbacksMask, rc);
    }

    LOC_LOGe(">>> session id %d, cap mask 0x%x", mSessionId, mCapsMask);
    if (mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID)  {
        // force mSessionId to invalid so startTracking will start the sesssion
        // if hal deamon crashes and restarts in the middle of a session
        mSessionId = LOCATION_CLIENT_SESSION_ID_INVALID;
        TrackingOptions trackOption;
        trackOption.setLocationOptions(mLocationOptions);
        (void)startTracking(trackOption);
    }
}

void LocationClientApiImpl::pingTest(PingTestCb pingTestCallback) {

    mPingTestCb = pingTestCallback;

    struct PingTestReq : public LocMsg {
        PingTestReq(LocationClientApiImpl *apiImpl) : mApiImpl(apiImpl){}
        virtual ~PingTestReq() {}
        void proc() const {
            LocAPIPingTestReqMsg msg(mApiImpl->mSocketName);
            bool rc = mApiImpl->sendMessage(reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
        }
        LocationClientApiImpl *mApiImpl;
    };
    mMsgTask.sendMsg(new (nothrow) PingTestReq(this));
    return;
}

void LocationClientApiImpl::invokePositionSessionResponseCb(LocationResponse responseCode) {
    if (mPositionSessionResponseCbPending) {
        if (nullptr != mResponseCb) {
            mResponseCb(responseCode);
        }
        mPositionSessionResponseCbPending = false;
    }
}

/******************************************************************************
LocationClientApiImpl -ILocIpcListener
******************************************************************************/
void IpcListener::onListenerReady() {
    struct ClientRegisterReq : public LocMsg {
        ClientRegisterReq(LocationClientApiImpl& apiImpl) : mApiImpl(apiImpl) {}
        void proc() const {
            LocAPIClientRegisterReqMsg msg(mApiImpl.mSocketName, LOCATION_CLIENT_API);
            bool rc = mApiImpl.sendMessage(reinterpret_cast<uint8_t *>(&msg), sizeof(msg));
            LOC_LOGd(">>> onListenerReady::ClientRegisterReqMsg rc=%d", rc);
        }
        LocationClientApiImpl& mApiImpl;
    };
    if (SockNode::Local == mSockTpye) {
        if (0 != chown(mApiImpl.mSocketName, getuid(), GID_LOCCLIENT)) {
            LOC_LOGe("chown to group locclient failed %s", strerror(errno));
        }
    }
    mMsgTask.sendMsg(new (nothrow) ClientRegisterReq(mApiImpl));
}

void IpcListener::onReceive(const char* data, uint32_t length,
                            const LocIpcRecver* recver) {
    struct OnReceiveHandler : public LocMsg {
        OnReceiveHandler(LocationClientApiImpl& apiImpl, IpcListener& listener,
                         const char* data, uint32_t length) :
                mApiImpl(apiImpl), mListener(listener), mMsgData(data, length) {}

        virtual ~OnReceiveHandler() {}
        void proc() const {
            LocAPIMsgHeader *pMsg = (LocAPIMsgHeader *)(mMsgData.data());

            // throw away message that does not come from location hal daemon
            if (false == pMsg->isValidServerMsg(mMsgData.length())) {
                return;
            }

            switch (pMsg->msgId) {
            case E_LOCAPI_CAPABILILTIES_MSG_ID:
            {
                LOC_LOGd("<<< capabilities indication");
                if (sizeof(LocAPICapabilitiesIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                mApiImpl.capabilitesCallback(pMsg->msgId, (void*)pMsg);
                break;
            }

            case E_LOCAPI_HAL_READY_MSG_ID:
            {
                LOC_LOGd("<<< HAL ready");
                if (sizeof(LocAPIHalReadyIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }

                // location hal deamon has restarted, need to set this
                // flag to false to prevent messages to be sent to hal
                // before registeration completes
                mApiImpl.mHalRegistered = false;
                mListener.onListenerReady();
                break;
            }

            case E_LOCAPI_START_TRACKING_MSG_ID:
            case E_LOCAPI_UPDATE_TRACKING_OPTIONS_MSG_ID:
            case E_LOCAPI_STOP_TRACKING_MSG_ID:
            case E_LOCAPI_START_BATCHING_MSG_ID:
            case E_LOCAPI_STOP_BATCHING_MSG_ID:
            case E_LOCAPI_UPDATE_BATCHING_OPTIONS_MSG_ID:
            {
                LOC_LOGd("<<< response message %d\n", pMsg->msgId);
                if (sizeof(LocAPIGenericRespMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                if (pMsg->msgId != E_LOCAPI_STOP_TRACKING_MSG_ID) {
                    const LocAPIGenericRespMsg* pRespMsg = (LocAPIGenericRespMsg*)(pMsg);
                    LocationResponse response = parseLocationError(pRespMsg->err);
                    mApiImpl.invokePositionSessionResponseCb(response);
                }
                break;
            }

            case E_LOCAPI_ADD_GEOFENCES_MSG_ID:
            case E_LOCAPI_REMOVE_GEOFENCES_MSG_ID:
            case E_LOCAPI_MODIFY_GEOFENCES_MSG_ID:
            case E_LOCAPI_PAUSE_GEOFENCES_MSG_ID:
            case E_LOCAPI_RESUME_GEOFENCES_MSG_ID:
            {
                LOC_LOGd("<<< collective response message, msgId = %d", pMsg->msgId);
                const LocAPICollectiveRespMsg* pRespMsg = (LocAPICollectiveRespMsg*)(pMsg);
                std::vector<pair<Geofence, LocationResponse>> responses{};
                for (int i=0; i<pRespMsg->collectiveRes.count; ++i) {
                    responses.push_back(make_pair(
                            mApiImpl.mGeofenceMap.at(
                            (*(pRespMsg->collectiveRes.resp + i)).clientId),
                            parseLocationError(
                            (*(pRespMsg->collectiveRes.resp + i)).error)));
                    if (LOCATION_ERROR_SUCCESS !=
                            (*(pRespMsg->collectiveRes.resp + i)).error ||
                            E_LOCAPI_REMOVE_GEOFENCES_MSG_ID == pMsg->msgId) {
                        mApiImpl.eraseGeofenceMap(1, const_cast<uint32_t*>(
                                &((*(pRespMsg->collectiveRes.resp + i)).clientId)));
                    }
                }
                if (mApiImpl.mCollectiveResCb) {
                    mApiImpl.mCollectiveResCb(responses);
                }
                break;
            }

            // async indication messages
            case E_LOCAPI_LOCATION_MSG_ID:
            {
                LOC_LOGd("<<< message = location");
                if (sizeof(LocAPILocationIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                LocationCallbacksMask tempMask =
                        (E_LOC_CB_DISTANCE_BASED_TRACKING_BIT | E_LOC_CB_SIMPLE_LOCATION_INFO_BIT);
                if ((mApiImpl.mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID) &&
                        (mApiImpl.mCallbacksMask & tempMask)) {
                    const LocAPILocationIndMsg* pLocationIndMsg = (LocAPILocationIndMsg*)(pMsg);
                    Location location = parseLocation(pLocationIndMsg->locationNotification);
                    if (mApiImpl.mLocationCb) {
                        mApiImpl.mLocationCb(location);
                    }
                    // copy location info over to gnsslocaiton so we can use existing routine
                    // to log the packet
                    GnssLocation gnssLocation = {};
                    gnssLocation.flags              = location.flags;
                    gnssLocation.timestamp          = location.timestamp;
                    gnssLocation.latitude           = location.latitude;
                    gnssLocation.longitude          = location.longitude;
                    gnssLocation.altitude           = location.altitude;
                    gnssLocation.speed              = location.speed;
                    gnssLocation.bearing            = location.bearing;
                    gnssLocation.horizontalAccuracy = location.horizontalAccuracy;
                    gnssLocation.verticalAccuracy   = location.verticalAccuracy;
                    gnssLocation.speedAccuracy      = location.speedAccuracy;
                    gnssLocation.bearingAccuracy    = location.bearingAccuracy;
                    gnssLocation.techMask           = location.techMask;

                    mApiImpl.mLogger.log(gnssLocation);
                }
                break;
            }

            case E_LOCAPI_BATCHING_MSG_ID:
            {
                LOC_LOGd("<<< message = batching");
                if (mApiImpl.mCallbacksMask & E_LOC_CB_BATCHING_BIT) {
                    const LocAPIBatchingIndMsg* pBatchingIndMsg =
                            (LocAPIBatchingIndMsg*)(pMsg);
                    std::vector<Location> locationVector;
                    BatchingStatus status = BATCHING_STATUS_INACTIVE;
                    if (BATCHING_STATUS_POSITION_AVAILABE ==
                            pBatchingIndMsg->batchNotification.status) {
                        for (int i=0; i<pBatchingIndMsg->batchNotification.count; ++i) {
                            locationVector.push_back(parseLocation(
                                        *(pBatchingIndMsg->batchNotification.location +
                                        i)));
                        }
                        status = BATCHING_STATUS_ACTIVE;
                    } else if (BATCHING_STATUS_TRIP_COMPLETED ==
                            pBatchingIndMsg->batchNotification.status) {
                        mApiImpl.stopBatching(0);
                        status = BATCHING_STATUS_DONE;
                    } else {
                        LOC_LOGe("invalid Batching Status!");
                        break;
                    }

                    if (mApiImpl.mBatchingCb) {
                        mApiImpl.mBatchingCb(locationVector, status);
                    }
                }
                break;
            }

            case E_LOCAPI_GEOFENCE_BREACH_MSG_ID:
            {
                LOC_LOGd("<<< message = geofence breach");
                if (mApiImpl.mCallbacksMask & E_LOC_CB_GEOFENCE_BREACH_BIT) {
                    const LocAPIGeofenceBreachIndMsg* pGfBreachIndMsg =
                        (LocAPIGeofenceBreachIndMsg*)(pMsg);
                    std::vector<Geofence> geofences;
                    for (int i=0; i<pGfBreachIndMsg->gfBreachNotification.count;
                         ++i) {
                        geofences.push_back(mApiImpl.mGeofenceMap.at(
                                                *(pGfBreachIndMsg->gfBreachNotification.id + i)));
                    }
                    if (mApiImpl.mGfBreachCb) {
                        mApiImpl.mGfBreachCb(geofences,
                                             parseLocation(
                                                 pGfBreachIndMsg->gfBreachNotification.location),
                                             GeofenceBreachTypeMask(
                                                 pGfBreachIndMsg->gfBreachNotification.type),
                                             pGfBreachIndMsg->gfBreachNotification.timestamp);
                    }
                }
                break;
            }

            case E_LOCAPI_LOCATION_INFO_MSG_ID:
            {
                LOC_LOGd("<<< message = location info");
                if (sizeof(LocAPILocationInfoIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                if ((mApiImpl.mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID) &&
                        (mApiImpl.mCallbacksMask & E_LOC_CB_GNSS_LOCATION_INFO_BIT)) {
                    const LocAPILocationInfoIndMsg* pLocationInfoIndMsg =
                        (LocAPILocationInfoIndMsg*)(pMsg);
                    GnssLocation gnssLocation =
                        parseLocationInfo(pLocationInfoIndMsg->gnssLocationInfoNotification);

                    if (mApiImpl.mGnssLocationCb) {
                        mApiImpl.mGnssLocationCb(gnssLocation);
                    }
                    mApiImpl.mLogger.log(gnssLocation);
                }
                break;
            }
            case E_LOCAPI_ENGINE_LOCATIONS_INFO_MSG_ID:
            {
                LOC_LOGd("<<< message = engine location info\n");

                if ((mApiImpl.mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID) &&
                        (mApiImpl.mCallbacksMask & E_LOC_CB_ENGINE_LOCATIONS_INFO_BIT)) {
                    const LocAPIEngineLocationsInfoIndMsg* pEngLocationsInfoIndMsg =
                        (LocAPIEngineLocationsInfoIndMsg*)(pMsg);

                    if (pEngLocationsInfoIndMsg->getMsgSize() != mMsgData.length()) {
                        LOC_LOGw("payload size does not match for message with id: %d",
                        pMsg->msgId);
                    }

                    std::vector<GnssLocation> engLocationsVector;
                    for (int i=0; i< pEngLocationsInfoIndMsg->count; i++) {
                        GnssLocation gnssLocation =
                            parseLocationInfo(pEngLocationsInfoIndMsg->engineLocationsInfo[i]);
                        engLocationsVector.push_back(gnssLocation);
                        mApiImpl.mLogger.log(gnssLocation);
                    }

                    if (mApiImpl.mEngLocationsCb) {
                        mApiImpl.mEngLocationsCb(engLocationsVector);
                    }
                }
                break;
            }

            case E_LOCAPI_SATELLITE_VEHICLE_MSG_ID:
            {
                LOC_LOGd("<<< message = sv");
                if (sizeof(LocAPISatelliteVehicleIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                if (mApiImpl.mCallbacksMask & E_LOC_CB_GNSS_SV_BIT) {
                    const LocAPISatelliteVehicleIndMsg* pSvIndMsg =
                        (LocAPISatelliteVehicleIndMsg*)(pMsg);
                    std::vector<GnssSv> gnssSvsVector;
                    for (int i=0; i< pSvIndMsg->gnssSvNotification.count; i++) {
                        GnssSv gnssSv;
                        gnssSv = parseGnssSv(pSvIndMsg->gnssSvNotification.gnssSvs[i]);
                        gnssSvsVector.push_back(gnssSv);
                    }
                    if (mApiImpl.mGnssSvCb) {
                        mApiImpl.mGnssSvCb(gnssSvsVector);
                    }
                    mApiImpl.mLogger.log(gnssSvsVector);
                }
                break;
            }

            case E_LOCAPI_NMEA_MSG_ID:
            {
                if ((mApiImpl.mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID) &&
                        (mApiImpl.mCallbacksMask & E_LOC_CB_GNSS_NMEA_BIT) &&
                         mApiImpl.mGnssNmeaCb) {
                    // nmea is variable length, can not be checked
                    const LocAPINmeaIndMsg* pNmeaIndMsg = (LocAPINmeaIndMsg*)(pMsg);
                    uint64_t timestamp = pNmeaIndMsg->gnssNmeaNotification.timestamp;
                    std::string nmea(pNmeaIndMsg->gnssNmeaNotification.nmea,
                                     pNmeaIndMsg->gnssNmeaNotification.length);
                    LOC_LOGd("<<< message = nmea[%s]", nmea.c_str());
                    std::stringstream ss(nmea);
                    std::string each;
                    while(std::getline(ss, each, '\n')) {
                        each += '\n';
                        mApiImpl.mGnssNmeaCb(timestamp, each);
                    }
                    mApiImpl.mLogger.log(timestamp,
                            pNmeaIndMsg->gnssNmeaNotification.length,
                            pNmeaIndMsg->gnssNmeaNotification.nmea);
                }
                break;
            }

            case E_LOCAPI_DATA_MSG_ID:
            {
                LOC_LOGd("<<< message = data");
                if (sizeof(LocAPIDataIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                if ((mApiImpl.mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID) &&
                        (mApiImpl.mCallbacksMask & E_LOC_CB_GNSS_DATA_BIT)) {
                    const LocAPIDataIndMsg* pDataIndMsg = (LocAPIDataIndMsg*)(pMsg);
                    GnssData gnssData =
                        parseGnssData(pDataIndMsg->gnssDataNotification);
                    if (mApiImpl.mGnssDataCb) {
                        mApiImpl.mGnssDataCb(gnssData);
                    }
                }
                break;
            }

            case E_LOCAPI_MEAS_MSG_ID:
            {
                LOC_LOGd("<<< message = measurements");
                if (sizeof(LocAPIMeasIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                        pMsg->msgId);
                }
                if ((mApiImpl.mSessionId != LOCATION_CLIENT_SESSION_ID_INVALID) &&
                    (mApiImpl.mCallbacksMask & E_LOC_CB_GNSS_MEAS_BIT)) {
                    const LocAPIMeasIndMsg* pMeasIndMsg = (LocAPIMeasIndMsg*)(pMsg);
                    GnssMeasurements gnssMeasurements =
                        parseGnssMeasurements(pMeasIndMsg->gnssMeasurementsNotification);
                    if (mApiImpl.mGnssMeasurementsCb) {
                        mApiImpl.mGnssMeasurementsCb(gnssMeasurements);
                    }
                    mApiImpl.mLogger.log(gnssMeasurements);
                }
                break;
            }

            case E_LOCAPI_GET_GNSS_ENGERY_CONSUMED_MSG_ID:
            {
                LOC_LOGd("<<< message = GNSS power consumption\n");
                if (sizeof(LocAPIGnssEnergyConsumedIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                LocAPIGnssEnergyConsumedIndMsg* pEnergyMsg =
                    (LocAPIGnssEnergyConsumedIndMsg*) pMsg;
                uint64_t energyNumber =
                    pEnergyMsg->totalGnssEnergyConsumedSinceFirstBoot;
                uint32_t flags = 0;
                if (energyNumber != 0xffffffffffffffff) {
                    flags = ENERGY_CONSUMED_SINCE_FIRST_BOOT_BIT;
                }
                GnssEnergyConsumedInfo energyConsumedInfo = {};
                energyConsumedInfo.flags =
                    (location_client::GnssEnergyConsumedInfoMask) flags;
                energyConsumedInfo.totalEnergyConsumedSinceFirstBoot = energyNumber;
                if (flags == 0 && mApiImpl.mGnssEnergyConsumedResponseCb) {
                    mApiImpl.mGnssEnergyConsumedResponseCb(
                        LOCATION_RESPONSE_UNKOWN_FAILURE);
                } else if (mApiImpl.mGnssEnergyConsumedInfoCb){
                    mApiImpl.mGnssEnergyConsumedInfoCb(energyConsumedInfo);
                }
                break;
            }

            case E_LOCAPI_LOCATION_SYSTEM_INFO_MSG_ID:
            {
                LOC_LOGd("<<< message = location system info");
                if (sizeof(LocAPILocationSystemInfoIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                if (mApiImpl.mCallbacksMask & E_LOC_CB_SYSTEM_INFO_BIT) {
                    const LocAPILocationSystemInfoIndMsg * pDataIndMsg =
                            (LocAPILocationSystemInfoIndMsg*)(pMsg);
                    LocationSystemInfo locationSystemInfo =
                            parseLocationSystemInfo(pDataIndMsg->locationSystemInfo);
                    if (mApiImpl.mLocationSysInfoCb) {
                        mApiImpl.mLocationSysInfoCb(locationSystemInfo);
                    }
                }
                break;
            }

            case E_LOCAPI_PINGTEST_MSG_ID:
            {
                LOC_LOGd("<<< ping message %d", pMsg->msgId);
                if (sizeof(LocAPIPingTestIndMsg) != mMsgData.length()) {
                    LOC_LOGw("payload size does not match for message with id: %d",
                             pMsg->msgId);
                }
                const LocAPIPingTestIndMsg* pIndMsg = (LocAPIPingTestIndMsg*)(pMsg);
                if (mApiImpl.mPingTestCb) {
                    uint32_t response = pIndMsg->data[0];
                    mApiImpl.mPingTestCb(response);
                }
                break;
            }

            default:
            {
                LOC_LOGe("<<< unknown message %d\n", pMsg->msgId);
                break;
            }
            }
        }
        LocationClientApiImpl& mApiImpl;
        IpcListener& mListener;
        const string mMsgData;
    };
    mMsgTask.sendMsg(new (nothrow) OnReceiveHandler(mApiImpl, *this, data, length));
}

/******************************************************************************
LocationClientApiImpl - Not implemented overrides
******************************************************************************/

void LocationClientApiImpl::gnssNiResponse(uint32_t id, GnssNiResponse response) {
}

void LocationClientApiImpl::updateTrackingOptions(uint32_t id, TrackingOptions& options) {
}
} // namespace location_client
