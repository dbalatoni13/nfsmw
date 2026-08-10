#ifndef STATEMGR_CARSTATE_HPP
#define STATEMGR_CARSTATE_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"

#define MAX_NUM_AI_ENGINES_LOADED 6   // Decl: 13
#define MAX_NUM_AI_ENGINES_ATTACHED 4 // Decl: 14

#define MAX_NUM_CARS 24 // Decl: 23

// total size: 0x8
// Decl: 30
struct EngineMappingPair {
    EngineMappingPair()
        : Start(0),    //
          Finish(0) {} // Decl: 31

    // Decl: 37
    bool operator==(const EngineMappingPair &cmp) const {
        if (cmp.Finish != this->Finish) {
            return false;
        }

        return cmp.Start == this->Start;
    }

    Attrib::Key Start;  // offset 0x0, size 0x4, Decl: 47
    Attrib::Key Finish; // offset 0x4, size 0x4, Decl: 48
};

// total size: 0x24
// Decl: 52
class CSTATEMGR_CarState : public CSTATEMGR_Base {
  public:
    CSTATEMGR_CarState();
    ~CSTATEMGR_CarState() override;

    // Overrides: CSTATEMGR_Base
    void UpdateParams(float t) override;

    static void ResetCarBanks();

    static void ResolveCarBanks();

    static void DestroyCar(EAX_CarState *eax_car);

    static void AddMapping(Attrib::Key key1, Attrib::Key key2);

    static void SpewEngineMapping();

    Sound::Context m_CarContext; // offset 0x1C, size 0x4, Decl: 59
    float m_fConnectDistance;    // offset 0x20, size 0x4

    static bool CopsCanBeInGame; // size: 0x1, address: 0x80417F54

    // Decl: 74
    struct EngToCarStruct {
        EngToCarStruct()
            : EngineKey(0),    //
              pCar(nullptr) {} // Decl: 75

        Attrib::Key EngineKey; // offset 0x0, size 0x4, Decl: 82
        EAX_CarState *pCar;    // offset 0x4, size 0x4, Decl: 83
    };

    typedef UTL::FixedVector<unsigned int, 8> EngineList;
    typedef UTL::FixedVector<EngineMappingPair, MAX_NUM_CARS> MappingList;
    typedef UTL::FixedVector<CSTATEMGR_CarState::EngToCarStruct, MAX_NUM_CARS> EngToCarMapList;

    static MappingList FinalMapping;     // size: 0xD0, address: 0x8045E114
    static EngineList FinalEngines;      // size: 0x30, address: 0x8045E1E4
    static EngineList FinalCopV8Engines; // size: 0x30, address: 0x8045E214

    static EngToCarMapList EngineToCarMapping; // size: 0xD0, address: 0x8045E244
};

#endif
