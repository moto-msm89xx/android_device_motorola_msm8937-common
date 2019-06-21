#!/sbin/sh

sku=`getprop ro.boot.hardware.sku`

if [ "$sku" = "XT1924-1" ] || [ "$sku" = "XT1924-3" ] || [ "$sku" = "XT1924-4" ] || [ "$sku" = "XT1924-5" ]; then
    mv /vendor/etc/audio_platform_info_ahannah.xml /vendor/etc/audio_platform_info.xml
    mv /vendor/etc/mixer_paths_ahannah.xml /vendor/etc/mixer_paths.xml
    mv /vendor/etc/sensors/sensor_def_qcomdev_ahannah.conf /vendor/etc/sensors/sensor_def_qcomdev.conf
    rm -rf /vendor/app/LineageActions
    rm /vendor/etc/permissions/android.hardware.sensor.compass.xml
    rm /vendor/etc/permissions/android.hardware.sensor.gyroscope.xml
else
    rm -rf /system/priv-app/MotoDoze
    rm /vendor/etc/audio_platform_info_ahannah.xml
    rm /vendor/etc/mixer_paths_ahannah.xml
    rm /vendor/etc/sensors/sensor_def_qcomdev_ahannah.conf
fi

    # Replace EGL for rhannah variants
if [ "$sku" = "XT1924-1" ] || [ "$sku" = "XT1924-4" ] || [ "$sku" = "XT1924-5" ]; then
    mv /vendor/lib/egl/libEGL_adreno_8917.so /vendor/lib/egl/libEGL_adreno.so
    mv /vendor/lib/egl/libGLESv1_CM_adreno_8917.so /vendor/lib/egl/libGLESv1_CM_adreno.so
    mv /vendor/lib/egl/libGLESv2_adreno_8917.so /vendor/lib/egl/libGLESv2_adreno.so
    mv /vendor/lib64/egl/libEGL_adreno_8917.so /vendor/lib64/egl/libEGL_adreno.so
    mv /vendor/lib64/egl/libGLESv1_CM_adreno_8917.so /vendor/lib64/egl/libGLESv1_CM_adreno.so
    mv /vendor/lib64/egl/libGLESv2_adreno_8917.so /vendor/lib64/egl/libGLESv2_adreno.so
else
    rm /vendor/lib/egl/libEGL_adreno_8917.so
    rm /vendor/lib/egl/libGLESv1_CM_adreno_8917.so
    rm /vendor/lib/egl/libGLESv2_adreno_8917.so
    rm /vendor/lib64/egl/libEGL_adreno_8917.so
    rm /vendor/lib64/egl/libGLESv1_CM_adreno_8917.so
    rm /vendor/lib64/egl/libGLESv2_adreno_8917.so
fi
