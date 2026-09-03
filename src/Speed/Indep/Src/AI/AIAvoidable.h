#ifndef AI_AVOIDABLE_H
#define AI_AVOIDABLE_H

#include "Speed/Indep/Libs/Support/Miscellaneous/SAP.h"
#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

DECLARE_CONTAINER_TYPE(AIAvoidableNeighbors);

typedef UTL::Std::list<struct AIAvoidable *, _type_AIAvoidableNeighbors> AvoidableList;

// total size: 0x14
class ALIGN_16 AIAvoidable {
  public:
    // total size: 0x6C
    struct Grid : public SAP::Grid<AIAvoidable> {
        USE_FASTALLOC(AIAvoidable::Grid);

        Grid(AIAvoidable &owner, const UMath::Vector3 &position, float radius) : SAP::Grid<AIAvoidable>(owner, position, radius) {}
    };

    AIAvoidable *FindOverlap(UMath::Vector4 *normal) const;

    typedef UTL::Std::list<AIAvoidable *, _type_AIAvoidableNeighbors> Neighbors;

    const Neighbors &GetAvoidableNeighbors() {
        return this->mNeighbors;
    }

    static void UpdateAllAvoidables(float dT);
    static void DrawAll();

    template <typename T> bool QueryInterface(T **out) {
        if (this->mUnk) {
            return this->mUnk->QueryInterface(out);
        }
        *out = nullptr;
        return false;
    }

  protected:
    AIAvoidable(UTL::COM::IUnknown *pUnkPersist);

    virtual ~AIAvoidable();
    virtual bool OnUpdateAvoidable(UMath::Vector3 &pos, float &sweep) = 0;

    // void SetAvoidableObject(UTL::COM::IUnknown *pUnk) {}

  private:
    static void OnOverLap(AIAvoidable &a0, AIAvoidable &a1, float dT);
    static void Validate(AIAvoidable *avoidable);

    Grid *mGridNode;          // offset 0x0, size 0x4
    Neighbors mNeighbors;     // offset 0x4, size 0x8
    UTL::COM::IUnknown *mUnk; // offset 0xC, size 0x4

    static AvoidableList mAll;
};

#endif
