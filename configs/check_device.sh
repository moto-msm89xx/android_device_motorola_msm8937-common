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

rm /vendor/bin/hw/android.hardware.camera.provider@2.4-service
rm /vendor/etc/init/hw/android.hardware.camera.provider@2.4-service.rc
