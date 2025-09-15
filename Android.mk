# Android.mk for Konda
# Build configuration for Android NDK

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := Konda
LOCAL_SRC_FILES := \
    AudioWorkstation/Source/WorkstationProcessor.cpp \
    AudioWorkstation/Source/WorkstationEditor.cpp \
    Source/SineWaveVoice.cpp \
    Source/SineWaveSound.cpp

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/AudioWorkstation/Source \
    $(LOCAL_PATH)/Source \
    $(HOME)/JUCE/modules

LOCAL_CPPFLAGS := -std=c++17 -frtti -fexceptions
LOCAL_LDLIBS := -llog -landroid -lOpenSLES

include $(BUILD_SHARED_LIBRARY)