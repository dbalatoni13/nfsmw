#ifndef STATEMGR_ENVIRO_HPP
#define STATEMGR_ENVIRO_HPP // Decl: 2

#include "Speed/Indep/Src/EAXSound/States/Managers/STATEMGR_Base.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Interfaces/ISndAttachable.hpp"

#define MAX_NUM_WORLD_OBJS 5 // Decl: 12

enum WORLDOBJECT_TYPES {
    WORLDOBJECT_FOUNTAIN = 4,
};

// total size: 0x1C
struct WorldObject : ISndAttachable {
    bVector3 m_3DPosition;   // offset 0x8, size 0x10
    WORLDOBJECT_TYPES Types; // offset 0x18, size 0x4

    WorldObject(const bVector3 &_pos, WORLDOBJECT_TYPES _type) {
        this->m_3DPosition = _pos;
        this->Types = _type;
    }

    // Overrides: ISndAttachable
    const bVector3 *GetPosition() override {
        return &this->m_3DPosition;
    }

    // Overrides: ISndAttachable
    int GetType() override {
        return this->Types;
    }
};

// total size: 0x48
// Decl: 34
class CSTATE_WorldObject : public CSTATE_Base {
  public:
    DECLARE_STATETYPE();

    CSTATE_WorldObject();
    ~CSTATE_WorldObject() override;

    // Overrides: CSTATE_Base
    void Attach(void *pAttachment) override;
    bool Detach() override;

    ISndAttachable *mSndAttachment; // offset 0x44, size 0x4, Decl: 45
};

// total size: 0x24
// Decl: 53
class CSTATEMGR_Enviro : public CSTATEMGR_Base {
  public:
    CSTATEMGR_Enviro();
    ~CSTATEMGR_Enviro() override;

    void RegisterWorldObjects();
    void AddWorldObject(float x, float y, float z, WORLDOBJECT_TYPES type);

    // Overrides: CSTATEMGR_Base
    void EnterWorld(eSndGameMode esgm) override;
    void UpdateParams(float t) override;

    UTL::Std::list<WorldObject *, _type_list> m_WorldObjects; // offset 0x1C, size 0x8
};

#endif
