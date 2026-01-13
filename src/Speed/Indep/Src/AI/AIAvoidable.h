#ifndef AI_AIAVOIDABLE_H
#define AI_AIAVOIDABLE_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Miscellaneous/SAP.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

DECLARE_VECTOR_TYPE(AIAvoidableNeighbors);

typedef UTL::Std::list<struct AIAvoidable *, _type_AIAvoidableNeighbors> AvoidableList;

// total size: 0x14
class AIAvoidable {
  public:
    // total size: 0x6C
    struct Grid : public SAP::Grid<AIAvoidable> {
        // void *operator new(unsigned int size, void *ptr) {}

        // void operator delete(void *mem, void *ptr) {}

        // void *operator new(unsigned int size) {}

        void operator delete(void *mem, size_t size) {
            if (mem) {
                gFastMem.Free(mem, size, nullptr);
            }
        }

        // void *operator new(unsigned int size, const char *name) {}

        // void operator delete(void *mem, const char *name) {}

        // void operator delete(void *mem, unsigned int size, const char *name) {}

        Grid(); // TODO delete
        // Grid(AIAvoidable &owner, const UMath::Vector3 &position, float radius) : SAP::Grid() {}

        ~Grid() {}
    };

    typedef UTL::Std::list<AIAvoidable *, _type_AIAvoidableNeighbors> Neighbors;

    static void OnOverLap(AIAvoidable &a0, AIAvoidable &a1, float dT);
    static void Validate(AIAvoidable *avoidable);
    static void DrawAll();
    static void UpdateAllAvoidables(float dT);

    AIAvoidable(UTL::COM::IUnknown *pUnkPersist);
    AIAvoidable *FindOverlap(UMath::Vector4 *normal) const;

    // Virtual functions
    virtual ~AIAvoidable();
    virtual bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep);

    // const Neighbors &GetAvoidableNeighbors() {}

    // void SetAvoidableObject(UTL::COM::IUnknown *pUnk) {}

    // bool QueryInterface(struct IBody **out) {}

    // bool QueryInterface(IVehicle **out) {}

    // bool QueryInterface(IRigidBody **out) {}

  private:
    static AvoidableList mAll;

    struct Grid *mGridNode;   // offset 0x0, size 0x4
    Neighbors mNeighbors;     // offset 0x4, size 0x8
    UTL::COM::IUnknown *mUnk; // offset 0xC, size 0x4
};

#endif
