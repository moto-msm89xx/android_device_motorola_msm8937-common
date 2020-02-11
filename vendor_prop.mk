# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    audio.deep_buffer.media=true \
    audio.dolby.ds2.enabled=true \
    audio.offload.min.duration.secs=60 \
    audio.offload.video=true \
    persist.speaker.prot.enable=false \
    persist.vendor.audio.dualmic.config=endfire \
    persist.vendor.audio.fluence.voicecall=true \
    persist.vendor.audio.fluence.voicecomm=true \
    persist.vendor.audio.fluence.voicerec=true \
    ro.config.max_starting_bg=8 \
    ro.config.vc_call_vol_steps=8 \
    ro.vendor.audio.sdk.fluencetype=fluence \
    ro.vendor.audio.sdk.ssr=false \
    vendor.audio_hal.period_size=240 \
    vendor.audio.hw.aac.encoder=false \
    vendor.audio.offload.gapless.enabled=true \
    vendor.audio.playback.mch.downsample=true \
    vendor.audio.offload.multiple.enabled=false \
    vendor.audio.offload.passthrough=false \
    vendor.audio.offload.track.enable=true \
    vendor.audio.parser.ip.buffer.size=262144 \
    vendor.audio.pp.asphere.enabled=false \
    vendor.audio.safx.pbe.enabled=true \
    vendor.tunnel.audio.encode=false \
    vendor.voice.conc.fallbackpath=deep-buffer \
    vendor.voice.path.for.pcm.voip=true

# Audio ACDB
PRODUCT_PROPERTY_OVERRIDES += \
    persist.audio.calfile0=/vendor/etc/acdbdata/Bluetooth_cal.acdb \
    persist.audio.calfile1=/vendor/etc/acdbdata/General_cal.acdb \
    persist.audio.calfile2=/vendor/etc/acdbdata/Global_cal.acdb \
    persist.audio.calfile3=/vendor/etc/acdbdata/Handset_cal.acdb \
    persist.audio.calfile4=/vendor/etc/acdbdata/Hdmi_cal.acdb \
    persist.audio.calfile5=/vendor/etc/acdbdata/Headset_cal.acdb \
    persist.audio.calfile6=/vendor/etc/acdbdata/Speaker_cal.acdb

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.hfp.client=1 \
    qcom.bt.le_dev_pwr_class=1 \
    ro.bluetooth.hfp.ver=1.6 \
    ro.qualcomm.bt.hci_transport=smd \
    vendor.bluetooth.soc=smd \
    vendor.qcom.bluetooth.soc=smd

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    camera.disable_zsl_mode=1 \
    persist.ts.rtmakeup=true \
    persist.vendor.camera.display.umax=1920x1080 \
    persist.vendor.camera.display.lmax=1280x720 \
    persist.vendor.qti.telephony.vt_cam_interface=1

# CNE
PRODUCT_PROPERTY_OVERRIDES += \
    persist.vendor.cne.feature=1 \
    persist.cne.logging.qxdm=3974 \
    persist.sys.cnd.iwlan=1

# core control
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.core_ctl_min_cpu=2 \
    ro.vendor.qti.core_ctl_max_cpu=4

# Dalvik
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapstartsize=16m \
    dalvik.vm.heapgrowthlimit=256m \
    dalvik.vm.heapsize=512m \
    dalvik.vm.heaptargetutilization=0.75 \
    dalvik.vm.heapminfree=4m \
    dalvik.vm.heapmaxfree=8m

# Debug
PRODUCT_PROPERTY_OVERRIDES += \
    persist.dbg.volte_avail_ovr=1 \
    persist.dbg.vt_avail_ovr=1 \
    persist.dbg.wfc_avail_ovr=1

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.display.cabl=0 \
    vendor.display.enable_default_color_mode=1

# DRM
PRODUCT_PROPERTY_OVERRIDES += \
    drm.service.enabled=true

# Fingerprint
PRODUCT_PROPERTY_OVERRIDES += \
    persist.qfp=false

# Fm
PRODUCT_PROPERTY_OVERRIDES += \
    ro.fm.transmitter=false \
    ro.vendor.fm.use_audio_session=true

# GPU
PRODUCT_PROPERTY_OVERRIDES += \
    debug.egl.hw=0 \
    debug.enable.sglscale=1 \
    debug.gralloc.gfx_ubwc_disable=0 \
    debug.sf.enable_hwc_vds=1 \
    debug.sf.hw=0 \
    debug.sf.latch_unsignaled=1 \
    debug.sf.recomputecrop=0 \
    dev.pm.dyn_samplingrate=1 \
    persist.demo.hdmirotationlock=false \
    persist.hwc.mdpcomp.enable=true \
    vendor.display.disable_rotator_split=1 \
    vendor.display.disable_skip_validate=1 \
    vendor.display.enable_default_color_mode=1 \
    vendor.display.perf_hint_window=50 \
    vendor.gralloc.enable_fb_ubwc=1

# Media
PRODUCT_PROPERTY_OVERRIDES += \
    vendor.mm.enable.qcom_parser=4643 \
    vendor.vidc.enc.narrow.searchrange=1 \
    vendor.vidc.enc.disable.pq=true \
    vendor.vidc.disable.split.mode=1 \
    vendor.video.disable.ubwc=1

# Memory optimizations
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.sys.fw.bservice_enable=true \
    ro.vendor.qti.sys.fw.bservice_limit=5 \
    ro.vendor.qti.sys.fw.bservice_age=5000

# Perf
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.extension_library=libqti-perfd-client.so \
    ro.vendor.qti.sys.fw.bg_apps_limit=60

# RIL
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libpath=/vendor/lib64/libril-qc-qmi-1.so \
    rild.libargs=-d /dev/smd0 \
    ril.subscription.types=NV,RUIM \
    DEVICE_PROVISIONED=1 \
    persist.vendor.radio.no_wait_for_card=1 \
    persist.vendor.radio.dfr_mode_set=1 \
    persist.vendor.radio.relay_oprt_change=1 \
    persist.vendor.radio.oem_ind_to_both=0 \
    persist.vendor.radio.qcril_uim_vcc_feature=1 \
    persist.vendor.radio.0x9e_not_callname=1 \
    persist.vendor.radio.mt_sms_ack=30 \
    persist.vendor.radio.force_get_pref=1 \
    persist.vendor.radio.is_wps_enabled=true \
    persist.vendor.radio.custom_ecc=1 \
    persist.vendor.radio.eri64_as_home=1 \
    persist.vendor.radio.add_power_save=1 \
    persist.vendor.radio.data_con_rprt=1 \
    persist.vendor.radio.lte_vrte_ltd=1 \
    persist.vendor.radio.cs_srv_type=1 \
    persist.vendor.radio.sw_mbn_update=1 \
    persist.vendor.radio.app_mbn_path=/fsg \
    persist.vendor.radio.adb_log_on=0 \
    persist.vendor.radio.apm_sim_not_pwdn=1 \
    persist.vendor.radio.jbims=1 \
    persist.vendor.ims.disableDebugLogs=0 \
    persist.vendor.ims.disableIMSLogs=0 \
    persist.vendor.ims.disableDebugDataPathLogs=0 \
    persist.vendor.ims.disableADBLogs=0 \
    persist.vendor.ims.vt.enableadb=1 \
    persist.vendor.ims.disableQXDMLogs=1 \
    persist.radio.msgtunnel.start=true \
    persist.radio.calls.on.ims=true \
    persist.radio.domain.ps=0 \
    persist.rmnet.mux=enabled \
    persist.radio.REVERSE_QMI=0 \
    persist.radio.VT_USE_MDM_TIME=0 \
    persist.vt.supported=1 \
    persist.data.qmi.adb_logmask=0 \
    persist.data.netmgrd.qos.enable=true \
    persist.data.iwlan.enable=true \
    telephony.lteOnCdmaDevice=1 \
    ro.build.vendorprefix=/vendor

#,Sensors
PRODUCT_PROPERTY_OVERRIDES += \
    persist.radio.sar_sensor=1 \
    ro.mot.sensors.glance_approach=false \
    ro.vendor.sensors.pmd=false \
    ro.vendor.sensors.rmd=false \
    ro.vendor.sdk.sensors.gestures=false \
    ro.vendor.sensors.facing=false \
    ro.vendor.sensors.scrn_ortn=false \
    ro.vendor.sensors.cmc=false \
    ro.vendor.sensors.pedometer=false

# TimeService
PRODUCT_PROPERTY_OVERRIDES += \
    persist.timed.enable=true

# Trim properties
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.sys.fw.use_trim_settings=true \
    ro.vendor.qti.sys.fw.empty_app_percent=50 \
    ro.vendor.qti.sys.fw.trim_empty_percent=100 \
    ro.vendor.qti.sys.fw.trim_cache_percent=100 \
    ro.vendor.qti.sys.fw.trim_enable_memory=2147483648

# USB
PRODUCT_PROPERTY_OVERRIDES += \
    ro.usb.bpt=2ec1 \
    ro.usb.bpt_adb=2ec5 \
    ro.usb.bpteth=2ec3 \
    ro.usb.bpteth_adb=2ec6 \
    ro.usb.mtp=2e82 \
    ro.usb.mtp_adb=2e76 \
    ro.usb.ptp=2e83 \
    ro.usb.ptp_adb=2e84

# WIFI
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0
