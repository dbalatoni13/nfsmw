// Tabla de drivers de fichero de GameCube (DVD + HD).

namespace RealFile {
class DeviceDriver;
}

extern RealFile::DeviceDriver GcDvd_fdd;
extern RealFile::DeviceDriver GcHd_fdd;

RealFile::DeviceDriver *libdevice[2] = {
    &GcDvd_fdd,
    &GcHd_fdd,
};
