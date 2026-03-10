#ifndef _FEICONSCROLLERMENU
#define _FEICONSCROLLERMENU

#include <types.h>

#include "Speed/Indep/bWare/Inc/bList.hpp"

struct FEObject;

// 0x5C
struct IconOption : public bTNode<IconOption> {
    unsigned int Item;          // 0x08
    FEObject* FEngObject;       // 0x0C
    float XPos;                 // 0x10
    float YPos;                 // 0x14
    unsigned int OriginalColor; // 0x18
    bool IsGreyOut;             // 0x1C
    bool IsFlashable;           // 0x20
    bool Locked;                // 0x24
    float OrigWidth;            // 0x28
    float OrigHeight;           // 0x2C

private:
    unsigned int NameHash;          // 0x30
    unsigned int DescHash;          // 0x34
    float fScaleToPcnt;             // 0x38
    float fScaleStartSecs;          // 0x3C
    float fScaleDurSecs;            // 0x40
    float fScaleAtStart;            // 0x44
    bool bAnimComplete;             // 0x48
    bool bReactImmediately;         // 0x4C
    bool bIsTutorialAvailable;      // 0x50
    const char* pTutorialMovieName; // 0x54

public:
    IconOption(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash);
    virtual ~IconOption();
    virtual void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2);

    unsigned int GetName();
    unsigned int GetDesc();
    void StartScale(float scale_to, float duration);
    float GetScaleToPcnt();
    float GetScaleStartSecs();
    float GetScaleDurSecs();
    float GetScaleAtStart();
    void SetScaleAtStart(float scale);
    bool IsAnimComplete();
    void SetAnimComplete(bool b);
    bool ReactsImmediately();
    bool IsLocked();
    void SetLocked(bool b);
    void SetReactImmediately(bool b);
    bool IsTutorialAvailable();
    const char* GetTutorialMovieName();
    void SetTutorialMovieName(const char* name);
    void SetFEngObject(FEObject* obj);
};

#endif
