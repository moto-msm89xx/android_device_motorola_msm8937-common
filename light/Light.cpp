/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Copyright (C) 2019, Harshit Jain
 */

#define LOG_TAG "android.hardware.light@2.0-service.msm8937"

#include <log/log.h>
#include <fstream>
#include "Light.h"

namespace android {
namespace hardware {
namespace light {
namespace V2_0 {
namespace implementation {

#define LEDS                       "/sys/class/leds/"
#define LCD_LED                    LEDS "lcd-backlight/"
#define BRIGHTNESS                 "brightness"
#define WHITE                      LEDS "white/"

/*
 * Write value to path and close file.
 */
static void set(std::string path, std::string value) {
    std::ofstream file(path);
    /* Only write brightness value if stream is open, alive & well */
    if (file.is_open()) {
        file << value;
    } else {
        /* Fire a warning a bail out */
        ALOGE("failed to write %s to %s", value.c_str(), path.c_str());
        return;
    }
}

static void set(std::string path, int value) {
    set(path, std::to_string(value));
}

static inline bool isLit(const LightState& state) {
    return state.color & 0x00ffffff;
}

/*
 * Device specific methods
 */
static void Backlight(const LightState& state) {
    uint32_t brightness = state.color & 0xFF;
    set(LCD_LED BRIGHTNESS, brightness);
}

static inline void Led(const LightState& state, uint32_t pattern) {
    isLit(state) ? set(WHITE BRIGHTNESS, pattern) : set(WHITE BRIGHTNESS, 0);
}

static void Notification(const LightState& state) {
    /* Fast blink */
    Led(state, 1);
}

static void Attention(const LightState& state) {
    /* Slow blink */
    Led(state, 2);
}

static void ChargingNotification(const LightState& state) {
    /* Steady Led */
    Led(state, 3);
}

static std::map<Type, std::function<void(const LightState&)>> lights = {
    {Type::BACKLIGHT, Backlight},
    {Type::NOTIFICATIONS, Notification},
    {Type::BATTERY, ChargingNotification},
    {Type::ATTENTION, Attention},
};

Light::Light() {}

Return<Status> Light::setLight(Type type, const LightState& state) {
    auto it = lights.find(type);

    if (it == lights.end()) {
        return Status::LIGHT_NOT_SUPPORTED;
    }

    /*
     * Lock global mutex until light state is updated.
     */

    std::lock_guard<std::mutex> lock(globalLock);
    it->second(state);
    return Status::SUCCESS;
}

Return<void> Light::getSupportedTypes(getSupportedTypes_cb _hidl_cb) {
    std::vector<Type> types;

    for (auto const& light : lights) types.push_back(light.first);

    _hidl_cb(types);

    return Void();
}
/* Close all the namespaces */
}
}
}
}
}
