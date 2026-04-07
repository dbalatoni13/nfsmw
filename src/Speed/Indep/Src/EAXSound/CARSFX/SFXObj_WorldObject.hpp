#ifndef EAXSOUND_CARSFX_SFXOBJ_WORLDOBJECT_H
#define EAXSOUND_CARSFX_SFXOBJ_WORLDOBJECT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum WORLDOBJECT_TYPES {
    WORLDOBJECT_FOUNTAIN = 4,
};

struct ISndAttachable : public UTL::Collections::Listable<ISndAttachable, 15> {
    ISndAttachable();
    virtual ~ISndAttachable() {}
    virtual const bVector3 *GetPosition() = 0;
    virtual int GetType() = 0;
};

struct WorldObject : public ISndAttachable {
    WorldObject(const bVector3 &_pos, WORLDOBJECT_TYPES _type)
        : m_3DPosition(_pos) //
        , Types(_type)
    {}

    const bVector3 *GetPosition() override { return &m_3DPosition; }
    int GetType() override { return Types; }

    bVector3 m_3DPosition;      // offset 0x8, size 0x10
    WORLDOBJECT_TYPES Types;    // offset 0x18, size 0x4
};

struct ENV_STATIC;
struct SFXCTL_3DObjPos;

struct SFXObj_WorldObject : public CARSFX {
  protected:
    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    SFXObj_WorldObject();
    ~SFXObj_WorldObject() override;
    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    void Destroy() override;
    void ProcessUpdate() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void InitSFX() override;
    void Detach() override;

  private:
    ENV_STATIC *m_pcsisSFX;         // offset 0x28, size 0x4
    bVector3 mObjPos;               // offset 0x2C, size 0x10
    SFXCTL_3DObjPos *m_p3DObjPos;   // offset 0x3C, size 0x4
};

#endif
