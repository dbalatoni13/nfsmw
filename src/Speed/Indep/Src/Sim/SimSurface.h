#ifndef SIM_SIMSURFACE_H
#define SIM_SIMSURFACE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCrc.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/simsurface.h"

class SimSurface : public Attrib::Gen::simsurface {
  public:
    static const Attrib::Collection *Lookup(const UCrc32 &);
    static void InitSystem();
    static void UpdateSystem();

    static const SimSurface kNull;

    SimSurface(const Attrib::Collection *);
    SimSurface(const char *name);
    SimSurface(const Attrib::Key &key);
    SimSurface(const Attrib::RefSpec &spec);
    bool IsTypeOf(const SimSurface &surface) const;
    SimSurface GetParentSurface() const;

    SimSurface(const SimSurface &from) : Attrib::Gen::simsurface(from.GetConstCollection(), 0, nullptr) {}

    SimSurface() : Attrib::Gen::simsurface(GetConstCollection(), 0, nullptr) {
        // TODO
        // Change()
    }

    ~SimSurface() {
        // TODO
    }

    const SimSurface &operator=(const SimSurface &from) {
        Change(from.GetConstCollection());
    }

    void DebugOverride() {}

  private:
    void Init(const SimSurface &from);

    static const Attrib::Class *mClass;
    static const Attrib::Collection *mUnknown;
    static const Attrib::Collection *mNullSpec;
    static const Attrib::Collection *mOverride;
};

inline bool operator!=(const SimSurface &surfaceA, const SimSurface &surfaceB) {
    return surfaceA.GetConstCollection() != surfaceB.GetConstCollection();
}

#endif
