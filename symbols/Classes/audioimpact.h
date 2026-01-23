#ifndef ATTRIBSYS_CLASSES_AUDIOIMPACT_H
#define ATTRIBSYS_CLASSES_AUDIOIMPACT_H


#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <cstddef>

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

namespace Attrib {
namespace Gen {

struct audioimpact : Instance {
void *operator new(size_t bytes) {
    return Attrib::Alloc(bytes, "audioimpact");
}
            
void operator delete(void *ptr, size_t bytes) {
    Attrib::Free(ptr, bytes, "audioimpact");
}

audioimpact(Key collectionKey, unsigned int msgPort, UTL::COM::IUnknown *owner)
    : Instance(FindCollection(ClassKey(), collectionKey), msgPort, owner) {
    
}

audioimpact(const Collection *collection, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(collection, msgPort, owner) {
    
}

audioimpact(const audioimpact &src) : Instance(src) {
    
}

audioimpact(const RefSpec &refspec, unsigned int msgPort, UTL::COM::IUnknown *owner) : Instance(refspec, msgPort, owner) {
    
}

~audioimpact() {}

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
    return 0xfbffb107;
}

const Attrib::StringKey &DESCRIPTION(unsigned int index) const {
        const Attrib::StringKey *resultptr = reinterpret_cast<const Attrib::StringKey *>(GetAttributePointer(0x09925106, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const Attrib::StringKey *>(DefaultDataArea(sizeof(Attrib::StringKey)));
        }
        return *resultptr;
    }
        
unsigned int Num_DESCRIPTION() const {
            return Get(0x09925106).GetLength();
        }

const CollisionStream &StreamSweetner(unsigned int index) const {
        const CollisionStream *resultptr = reinterpret_cast<const CollisionStream *>(GetAttributePointer(0xa311c644, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const CollisionStream *>(DefaultDataArea(sizeof(CollisionStream)));
        }
        return *resultptr;
    }
        
unsigned int Num_StreamSweetner() const {
            return Get(0xa311c644).GetLength();
        }

const STICH_COLLISION_TYPE &STITCH_LEVEL_0(unsigned int index) const {
        const STICH_COLLISION_TYPE *resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(GetAttributePointer(0xc15856df, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(DefaultDataArea(sizeof(STICH_COLLISION_TYPE)));
        }
        return *resultptr;
    }
        
unsigned int Num_STITCH_LEVEL_0() const {
            return Get(0xc15856df).GetLength();
        }

const STICH_COLLISION_TYPE &STITCH_LEVEL_2(unsigned int index) const {
        const STICH_COLLISION_TYPE *resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(GetAttributePointer(0xc9218f8c, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(DefaultDataArea(sizeof(STICH_COLLISION_TYPE)));
        }
        return *resultptr;
    }
        
unsigned int Num_STITCH_LEVEL_2() const {
            return Get(0xc9218f8c).GetLength();
        }

const STICH_COLLISION_TYPE &STITCH_LEVEL_1(unsigned int index) const {
        const STICH_COLLISION_TYPE *resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(GetAttributePointer(0xdadb5580, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(DefaultDataArea(sizeof(STICH_COLLISION_TYPE)));
        }
        return *resultptr;
    }
        
unsigned int Num_STITCH_LEVEL_1() const {
            return Get(0xdadb5580).GetLength();
        }

const STICH_COLLISION_TYPE &STITCH_LEVEL_3(unsigned int index) const {
        const STICH_COLLISION_TYPE *resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(GetAttributePointer(0xefbca3c9, index));
        if (!resultptr) {
            resultptr = reinterpret_cast<const STICH_COLLISION_TYPE *>(DefaultDataArea(sizeof(STICH_COLLISION_TYPE)));
        }
        return *resultptr;
    }
        
unsigned int Num_STITCH_LEVEL_3() const {
            return Get(0xefbca3c9).GetLength();
        }

const StitchCollisionVol &Volumes() const {
        const StitchCollisionVol *resultptr = reinterpret_cast<const StitchCollisionVol *>(GetAttributePointer(0xfcc8e754, 0));
        if (!resultptr) {
            resultptr = reinterpret_cast<const StitchCollisionVol *>(DefaultDataArea(sizeof(StitchCollisionVol)));
        }
        return *resultptr;
    }
        
};

} // namespace Gen
} // namespace Attrib

#endif
