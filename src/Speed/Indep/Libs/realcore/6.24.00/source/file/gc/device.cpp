#include "dvd_device.h"
#include "hd_device.h"

extern GcDvdFileDeviceDriver GcDvd_fdd;
extern GcHdFileDeviceDriver GcHd_fdd;

static const int NUM_LIB_DEVICES = 2;

RealFile::DeviceDriver *libdevice[NUM_LIB_DEVICES] = {
    &GcDvd_fdd,
    &GcHd_fdd,
};
