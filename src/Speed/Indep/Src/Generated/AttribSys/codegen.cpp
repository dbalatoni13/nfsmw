#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include "Speed/Indep/Src/AI/aireflectedtypes.h"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_EnumAttributes.hpp"
#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"

namespace Attrib {

const void *DefaultDataArea(std::size_t bytes) {
    static char gDefaultDataArea[2048];

    return gDefaultDataArea;
}

//
// Manejadores de tipo generados: uno por tipo de usuario reflejado.
//

class AICollisionReactionRecord_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        AICollisionReactionRecord *tObj = new AICollisionReactionRecord(*(const AICollisionReactionRecord *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((AICollisionReactionRecord *) obj)->Reaction.Clean();
    }

    void Release(void *obj) {
        ((AICollisionReactionRecord *) obj)->Reaction.Clean();
    }
};

class Attrib_RefSpec_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        RefSpec *tObj = new RefSpec(*(const RefSpec *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((RefSpec *) obj)->Clean();
    }

    void Release(void *obj) {
        ((RefSpec *) obj)->Clean();
    }
};

class CollisionStream_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        CollisionStream *tObj = new CollisionStream(*(const CollisionStream *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((CollisionStream *) obj)->StreamMoment.Clean();
    }

    void Release(void *obj) {
        ((CollisionStream *) obj)->StreamMoment.Clean();
    }
};

class EffectLinkageRecord_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        EffectLinkageRecord *tObj = new EffectLinkageRecord(*(const EffectLinkageRecord *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((EffectLinkageRecord *) obj)->mSurface.Clean();
        ((EffectLinkageRecord *) obj)->mEffect.Clean();
    }

    void Release(void *obj) {
        ((EffectLinkageRecord *) obj)->mSurface.Clean();
        ((EffectLinkageRecord *) obj)->mEffect.Clean();
    }
};

class TireEffectRecord_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        TireEffectRecord *tObj = new TireEffectRecord(*(const TireEffectRecord *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((TireEffectRecord *) obj)->mEmitter.Clean();
    }

    void Release(void *obj) {
        ((TireEffectRecord *) obj)->mEmitter.Clean();
    }
};

class TrafficPatternRecord_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        TrafficPatternRecord *tObj = new TrafficPatternRecord(*(const TrafficPatternRecord *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((TrafficPatternRecord *) obj)->Vehicle.Clean();
    }

    void Release(void *obj) {
        ((TrafficPatternRecord *) obj)->Vehicle.Clean();
    }
};

class UpgradeSpecs_TypeHandler : public ITypeHandler {
  public:
    void *Retain(void *obj) {
        return obj;
    }

    void *Clone(void *obj) {
        UpgradeSpecs *tObj = new UpgradeSpecs(*(const UpgradeSpecs *) obj);

        return tObj;
    }

    void Clean(void *obj) {
        ((UpgradeSpecs *) obj)->Item.Clean();
    }

    void Release(void *obj) {
        ((UpgradeSpecs *) obj)->Item.Clean();
    }
};

static AICollisionReactionRecord_TypeHandler AICollisionReactionRecord_singleton;
static Attrib_RefSpec_TypeHandler Attrib_RefSpec_singleton;
static CollisionStream_TypeHandler CollisionStream_singleton;
static EffectLinkageRecord_TypeHandler EffectLinkageRecord_singleton;
static TireEffectRecord_TypeHandler TireEffectRecord_singleton;
static TrafficPatternRecord_TypeHandler TrafficPatternRecord_singleton;
static UpgradeSpecs_TypeHandler UpgradeSpecs_singleton;

static unsigned int kTypeHandlerCount = 7;
static unsigned int kTypeHandlerIds[8] = {
    0x2B936EB7u, 0xAA229CD7u, 0x341F03A0u, 0x600994C4u, 0x681D219Cu, 0x5FDE6463u, 0x57D382C9u, 0,
};
static ITypeHandler *kTypeHandlers[8] = {
    &Attrib_RefSpec_singleton,      //
    &AICollisionReactionRecord_singleton,
    &EffectLinkageRecord_singleton, //
    &TrafficPatternRecord_singleton,
    &TireEffectRecord_singleton,    //
    &UpgradeSpecs_singleton,        //
    &CollisionStream_singleton,     //
    0,
};

ITypeHandler *TypeDesc::Lookup(Type t) {
    unsigned int index = 0;

    // Sin local `count`: el global se lee en los tres sitios. El `lwz` de
    // dentro del bucle lo iza el LICM al preencabezado, y por eso el objetivo
    // trae el `mr r8,r9` que un `count` local no puede producir.
    if (index >= kTypeHandlerCount) {
        goto fail;
    }

    while (kTypeHandlerIds[index] != t) {
        unsigned int id = kTypeHandlerIds[index];

        // Dos locales: `(id < t) + 1` en una sola expresion se reasocia y sale
        // `2*index+1` primero (`addi` + `subfc`); con `less` y `branch`
        // separados sale el `subfic r9,r0,1` + `add` del objetivo.
        unsigned int less = id < t;
        unsigned int branch = less + 1;

        index = 2 * index + branch;

        if (index >= kTypeHandlerCount) {
            goto fail;
        }
    }

    if (index >= kTypeHandlerCount) {
        goto fail;
    }
    return kTypeHandlers[index];

fail:
    return 0;
}

Type TypeDesc::NameToType(const char *name) {
    return StringToKey(name);
}

} // namespace Attrib
