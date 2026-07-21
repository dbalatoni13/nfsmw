//
//
//
//
//
//
//
//
//
//
//
#ifndef SNDBASE_H
#define SNDBASE_H

#include "Dynamic_Mixer/NFSMixerDefines.hpp"
#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXCarState.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/EAXSound/UG/SndDataParams.hpp"

#define SFT_ALLOCTYPE(AllocID) (AllocID << 24)     // Decl: 36
#define SFT_OBJIDX(ObjectIndex) (ObjectIndex << 4) // Decl: 37
#define SFT_GRPID(GroupID) (GroupID << 16)         // Decl: 38

#define MASK_OBJIDX(ObjectID) ((ObjectID & 0xFFF0) >> 4)         // Decl: 40
#define MASK_GRPID(ObjectID) ((ObjectID & 0xFF0000) >> 16)       // Decl: 41
#define MASK_ALLOCTYPE(ObjectID) ((ObjectID & 0x0F000000) >> 24) // Decl: 42
static const int MAX_NUMBER_OF_IO_CONNECTIONS = 16;              // size: 0x4, Decl: 43

class EAXCar;

// total size: 0x24
// Decl: 75
class SndBase : public AudioMemBase {
  public:
    // total size: 0x10
    struct TypeInfo {
        int ObjectID;                           // offset 0x0, size 0x4
        char *typeName;                         // offset 0x4, size 0x4
        TypeInfo *baseTypeInfo;                 // offset 0x8, size 0x4
        SndBase *(*createObject)(unsigned int); // offset 0xC, size 0x4

        SndBase *CreateObject(unsigned int allocator) {}
    };

    SndBase();
    ~SndBase() override;
    virtual TypeInfo *GetTypeInfo() const;
    virtual const char *GetTypeName() const;
    static SndBase::TypeInfo *GetStaticTypeInfo();

    int GetObjectIndex() {
        return MASK_OBJIDX(GetTypeInfo()->ObjectID);
    }

    int GetGroupID() {
        return MASK_GRPID(this->objectID);
    }

    virtual int GetController(int Index) {
        return -1;
    }
    virtual void AttachController(struct SFXCTL *psfxctl) {}
    virtual void SetupSFX(CSTATE_Base *_StateBase);
    virtual void SetupLoadData() {
        InitSFX();
    }
    void LoadAsset(Attrib::StringKey filename, eSNDDATAPATH path, eSNDDATATYPE datatype, eBANK_SLOT_TYPE SlotType, bool LoadToTop);
    void LoadAsset(stSndAssetQueue &queueitem, eBANK_SLOT_TYPE SlotType);
    virtual void InitSFX() {
        if (this->m_pInputBlock != nullptr) {
            int *pctl = &this->m_pInputBlock[MAX_NUMBER_OF_IO_CONNECTIONS - 1];
            *pctl = 1;
        }
    }
    virtual void Destroy() {}
    virtual void UpdateParams(float t) {}
    virtual void ProcessUpdate() {}
    virtual void Detach() {}
    virtual void UpdateMixerOutputs() {}

    bool IsEnabled() {
        return this->m_bIsEnabled;
    }

    void Enable() {
        this->m_bIsEnabled = true;
    }

    void Disable() {
        this->m_bIsEnabled = false;
    }

    void SetDMIX_Input(int index, int value) {
        this->m_pOutPutBlock[index] = value;
    }

    int GetDMIX_InputValue(int index) {
        return m_pOutPutBlock[index];
    }

    int GetDMixOutput(int idx, DMX_PRESET_TYPE etype);

    CSTATE_Base *GetStateBase() {
        return this->m_pStateBase;
    }

    EAX_CarState *GetPhysCar() {
        return this->m_pStateBase->GetPhysCar();
    }

    SndBase *m_pNextSFX; // offset 0x4, size 0x4, Decl: 236

    static float m_fRunningTime; // Decl: 296
    static float m_fDeltaTime;   // Decl: 297

  private:
    bool m_bIsEnabled;   // offset 0x8, size 0x1, Decl: 243
    int *m_pOutPutBlock; // offset 0xC, size 0x4
    int *m_pInputBlock;  // offset 0x10, size 0x4

  protected:
    static TypeInfo s_TypeInfo; // Decl: 302

    CSTATE_Base *m_pStateBase;       // offset 0x14, size 0x4, Decl: 304
    EAXCar *m_pEAXCar;               // offset 0x18, size 0x4
    unsigned short m_refCount;       // offset 0x1C, size 0x2
    unsigned short m_allocatorIndex; // offset 0x1E, size 0x2
    int objectID;                    // offset 0x20, size 0x4
};

#define TYPEINFO(classType) classType::GetStaticTypeInfo() // Decl: 421
// Decl: 429
#define DECLARE_TYPEINFO()                                                                                                                           \
  protected:                                                                                                                                         \
    static SndBase::TypeInfo s_TypeInfo;                                                                                                             \
                                                                                                                                                     \
  public:                                                                                                                                            \
    SndBase::TypeInfo *GetTypeInfo() const override;                                                                                                 \
    const char *GetTypeName() const override;                                                                                                        \
    static SndBase::TypeInfo *GetStaticTypeInfo() {                                                                                                  \
        return &s_TypeInfo;                                                                                                                          \
    }
#define DECLARE_CREATABLE() DECLARE_TYPEINFO() static SndBase *CreateObject(uint32 allocator); // Decl: 442

// Decl: 457
#define TYPEINFO_IMPLEMENT(theObjectID, theClass, baseClass, pCreateFunc)                                                                            \
    SndBase::TypeInfo theClass::s_TypeInfo = {(int)theObjectID, #theClass, &baseClass::s_TypeInfo, pCreateFunc};                                     \
    SndBase::TypeInfo *theClass::GetTypeInfo() const {                                                                                               \
        return &s_TypeInfo;                                                                                                                          \
    }                                                                                                                                                \
    const char *theClass::GetTypeName() const {                                                                                                      \
        return s_TypeInfo.typeName;                                                                                                                  \
    }
#define DEFINE_TYPEINFO(theClass, baseClass) TYPEINFO_IMPLEMENT(-1, theClass, baseClass, NULL) // Decl: 472
// Decl: 487
#define DEFINE_CREATABLE(theObjectID, theClass, baseClass)                                                                                           \
    TYPEINFO_IMPLEMENT(theObjectID, theClass, baseClass, theClass::CreateObject) SndBase *theClass::CreateObject(uint32 allocator) {                 \
        if (allocator == DEFAULT_ALLOCATOR) {                                                                                                        \
            return new (GetStaticTypeInfo()->typeName, false) theClass;                                                                              \
        } else                                                                                                                                       \
            return new (GetStaticTypeInfo()->typeName, true) theClass;                                                                               \
    }
#define GETSMOOTHVAL(SmoothFactor) ((int)(32767.0f * 30.0f / (float)SmoothFactor) & 0x7fff) // Decl: 498

#endif
