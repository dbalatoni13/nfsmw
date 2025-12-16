#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

static Attrib::Class *TheSurfaceClass = nullptr;

const Attrib::Collection *SimSurface::mUnknown = nullptr;
const Attrib::Collection *SimSurface::mNullSpec = nullptr;

const Attrib::Collection *SimSurface::Lookup(const UCrc32 &namehash) {
    const Attrib::Collection *collection = TheSurfaceClass->GetCollection(namehash.GetValue());
    if (collection) {
        return collection;
    }
    return mUnknown;
}

SimSurface::SimSurface(const Attrib::Key &key) : Attrib::Gen::simsurface(key, 0, nullptr) {
    if (!IsValid()) {
        Change(mNullSpec);
    }
}

SimSurface::SimSurface(const Attrib::Collection *spec) : Attrib::Gen::simsurface(spec, 0, nullptr) {
    if (!IsValid()) {
        Change(mNullSpec);
    }
}

// UNSOLVED, confusing
SimSurface SimSurface::GetParentSurface() const {
    const Attrib::Collection *collection;
}

void SimSurface::UpdateSystem() {}

void SimSurface::InitSystem() {
    // simsurface, ?
    mNullSpec = Attrib::FindCollection(0xfb111fef, 0x90a4d09a);
    SimSurface *null_surface = const_cast<SimSurface *>(&kNull);
    null_surface->Change(mNullSpec);
    mUnknown = Attrib::FindCollection(0xfb111fef, Attrib::StringToKey("unknown"));
    TheSurfaceClass = Attrib::Database::Get().GetClass(0xfb111fef);
}
