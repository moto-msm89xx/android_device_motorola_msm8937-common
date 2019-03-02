# Audio
PRODUCT_PROPERTY_OVERRIDES += \
    audio.deep_buffer.media=true \
    audio.dolby.ds2.enabled=true \
    audio.offload.min.duration.secs=60 \
    audio.offload.video=true \
    persist.vendor.audio.dualmic.config=endfire \
    persist.vendor.audio.fluence.voicecall=true \
    persist.vendor.audio.fluence.voicecomm=true \
    persist.vendor.audio.fluence.voicerec=true \
    persist.vendor.audio.fluence.speaker=true \
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

# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
    bluetooth.hfp.client=1 \
    qcom.bt.le_dev_pwr_class=1 \
    ro.bluetooth.hfp.ver=1.6 \
    ro.qualcomm.bt.hci_transport=smd \
    vendor.qcom.bluetooth.soc=smd

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
    camera.disable_zsl_mode=1 \
    camera.hal1.packagelist=com.skype.raider,com.google.android.talk \
    persist.vendor.camera.tof.direct=1 \
    persist.vendor.camera.display.umax=1920x1080 \
    persist.vendor.camera.display.lmax=1280x720

# Display
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196610 \
    ro.qualcomm.cabl=0 \
    ro.sf.lcd_density=320 \
    vendor.display.enable_default_color_mode=1

# Fm
PRODUCT_PROPERTY_OVERRIDES += \
    ro.fm.transmitter=false

# GPU
PRODUCT_PROPERTY_OVERRIDES += \
    debug.sf.enable_hwc_vds=1 \
    debug.sf.hw=1 \
    debug.sf.latch_unsignaled=1 \
    debug.egl.hw=1 \
    persist.hwc.mdpcomp.enable=true \
    debug.mdpcomp.logs=0 \
    dev.pm.dyn_samplingrate=1 \
    persist.demo.hdmirotationlock=false \
    debug.enable.sglscale=1 \
    sdm.debug.disable_skip_validate=1

# Media
PRODUCT_PROPERTY_OVERRIDES += \
    mm.enable.qcom_parser=4643 \
    vendor.vidc.enc.narrow.searchrange=1 \
    vendor.vidc.disable.split.mode=1

# RIL
PRODUCT_PROPERTY_OVERRIDES += \
    rild.libpath=/vendor/lib/libril-qc-qmi-1.so \
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
    persist.vendor.radio.data_con_rprt=1 \
    persist.vendor.radio.lte_vrte_ltd=1 \
    persist.vendor.radio.cs_srv_type=1
    persist.vendor.radio.sw_mbn_update=1 \
    persist.vendor.radio.app_mbn_path=/fsg \
    persist.vendor.radio.adb_log_on=0 \
    persist.radio.msgtunnel.start=true \
    persist.data.qmi.adb_logmask=0 \
    persist.data.netmgrd.qos.enable=true \
    persist.lte.pco_supported=true \
    telephony.lteOnCdmaDevice=1

# WIFI
PRODUCT_PROPERTY_OVERRIDES += \
    wifi.interface=wlan0
