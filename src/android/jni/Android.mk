LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#OPENCV_CAMERA_MODULES:=off
#OPENCV_INSTALL_MODULES:=off
#OPENCV_LIB_TYPE:=SHARED
OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
include ../../sdk/native/jni/OpenCV-tegra3.mk

LOCAL_MODULE    := native_sample
LOCAL_SRC_FILES := jni_part.cpp \
                   src/descriptor_extractor.cpp \
                   src/elucid_binary_descriptor_extractor.cpp \
                   src/elucid_descriptor_extractor.cpp \
                   src/util.cpp
LOCAL_CFLAGS := -DCV_NEON=1
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_LDLIBS +=  -llog -ldl
LOCAL_ARM_NEON := true

include $(BUILD_SHARED_LIBRARY)
