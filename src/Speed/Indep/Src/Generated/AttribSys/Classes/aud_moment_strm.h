#ifndef ATTRIBSYS_CLASSES_AUD_MOMENT_STRM_H
#define ATTRIBSYS_CLASSES_AUD_MOMENT_STRM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Csis {

enum Type_SoundFX_Type {
    Type_SoundFX_Type_Pursbreaker = 1,
    Type_SoundFX_Type_Crash = 2,
    Type_SoundFX_Type_Thunder = 4,
    Type_SoundFX_Type_engine = 8,
    Type_SoundFX_Type_Car = 16,
    Type_SoundFX_Type_GamePlayMoment = 32,
    Type_SoundFX_Type_jump = 64,
    Type_SoundFX_Type_game = 128,
};

enum Type_SoundFX_Param {
    Type_SoundFX_Param_distant = 1,
    Type_SoundFX_Param_sweetener = 2,
    Type_SoundFX_Param_blow = 4,
    Type_SoundFX_Param_total = 8,
    Type_SoundFX_Param_expl_gas_station = 16,
    Type_SoundFX_Param_truck_jackknife = 32,
    Type_SoundFX_Param_boatfall = 64,
    Type_SoundFX_Param_trck_log_dump = 128,
    Type_SoundFX_Param_stripmall = 256,
    Type_SoundFX_Param_scaffolding_big = 512,
    Type_SoundFX_Param_drive_in = 1024,
    Type_SoundFX_Param_water_tower = 2048,
    Type_SoundFX_Param_porch_collapse = 4096,
    Type_SoundFX_Param_comm_tower = 8192,
    Type_SoundFX_Param_fish_market_sign = 16384,
    Type_SoundFX_Param_patio_collapse = 32768,
    Type_SoundFX_Param_start_music = 65536,
    Type_SoundFX_Param_end_music = 131072,
    Type_SoundFX_Param_start_effects = 262144,
    Type_SoundFX_Param_end_effects = 524288,
    Type_SoundFX_Param_cam = 1048576,
    Type_SoundFX_Param_break = 2097152,
    Type_SoundFX_Param_amphitheatre = 4194304,
    Type_SoundFX_Param_gate_signage = 8388608,
    Type_SoundFX_Param_trailer_park = 16777216,
    Type_SoundFX_Param_torus_shop = 33554432,
    Type_SoundFX_Param_gazebo = 67108864,
    Type_SoundFX_Param_uves = 134217728,
    Type_SoundFX_Param_zoom = 268435456,
    Type_SoundFX_Param_camera = 536870912,
    Type_SoundFX_Param_police_tower = 1073741824,
};

} // namespace Csis

enum eVOL_MOMENT {
    eVOL_MOMENT_JACKNIFE = 1,
    eVOL_MOMENT_COL_SWT = 2,
    eVOL_MOMENT_GASPUMP = 3,
    eVOL_MOMENT_BLOWNENGINE = 4,
    eVOL_MOMENT_PURSUIT_BREAKER = 5,
    eVOL_MOMENT_TOTALLED_ENGINE = 6,
    eVOL_MOMENT_JUMPCAM = 7,
    eVOL_MOMENT_THUNDER = 8,
    eVOL_MOMENT_GAMEPLAY = 9,
    eVOL_MOMENT_FLY_IN_CAM = 10,
    eVOL_MOMENT_SPOT_COP = 11,
    eVOL_MOMENT_PHOTOFINISH = 12,
};

namespace Attrib {
namespace Gen {

struct aud_moment_strm : Instance {
    struct _LayoutStruct {
        Csis::Type_SoundFX_Type stream; // offset 0x0, size 0x4
        Csis::Type_SoundFX_Param param; // offset 0x4, size 0x4
        eVOL_MOMENT VolSlot;            // offset 0x8, size 0x4
        signed char strmpriority;       // offset 0xc, size 0x1
    };

    void *operator new(size_t bytes) {
        return Attrib::Alloc(bytes, "aud_moment_strm");
    }

    void operator delete(void *ptr, size_t bytes) {
        Attrib::Free(ptr, bytes, "aud_moment_strm");
    }

    aud_moment_strm(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
        : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aud_moment_strm(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aud_moment_strm(const aud_moment_strm &src) : Instance(src) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    aud_moment_strm(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
        SetDefaultLayout(sizeof(_LayoutStruct));
    }

    ~aud_moment_strm() {}

    void Change(const Collection *c) {
        Instance::Change(c);
    }

    void Change(Key collectionkey) {
        Change(FindCollection(ClassKey(), collectionkey));
    }

    void Change(const RefSpec &refspec) {
        Instance::Change(refspec);
    }

    static Key ClassKey() {
        return 0xd2410816;
    }

    const bool &CanInterupt() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xb34fc11d, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const bool &IsPositioned() const {
        const bool *resultptr = reinterpret_cast<const bool *>(GetAttributePointer(0xc4e7f7c2, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const bool *>(DefaultDataArea(sizeof(bool)));
        }
        return *resultptr;
    }

    const Csis::Type_SoundFX_Type &stream() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->stream;
    }

    const Csis::Type_SoundFX_Param &param() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->param;
    }

    const eVOL_MOMENT &VolSlot() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->VolSlot;
    }

    const signed char &strmpriority() const {
        return reinterpret_cast<_LayoutStruct *>(GetLayoutPointer())->strmpriority;
    }
};

} // namespace Gen
} // namespace Attrib

#endif
