#ifndef _FEICONSCROLLERMENU
#define _FEICONSCROLLERMENU

#include <types.h>

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct FEObject;
struct FEImage;
struct FEString;
struct FEGroup;
int FEngSNPrintf(char* dest, int size, const char* fmt, ...);

struct ScrollerDatumNode : public bTNode<ScrollerDatumNode> {
    char String[128];           // offset 0x8
    unsigned int LanguageHash;  // offset 0x88

    ScrollerDatumNode(const char* string, unsigned int hash) {
        FEngSNPrintf(String, 0x80, string);
        LanguageHash = hash;
    }
    virtual ~ScrollerDatumNode() {}
};

struct ScrollerSlotNode : public bTNode<ScrollerSlotNode> {
    FEObject* String; // offset 0x8

    ScrollerSlotNode(FEObject* string) {
        String = string;
    }
    virtual ~ScrollerSlotNode() {}
};

struct ScrollerDatum : public bTNode<ScrollerDatum> {
    bTList<ScrollerDatumNode> Strings; // offset 0x8
    bool bEnabled;                     // offset 0x10

    ScrollerDatum() {}
    ScrollerDatum(const char* string, unsigned int hash);
    virtual ~ScrollerDatum() {}
    void AddData(const char* string, unsigned int hash) {
        Strings.AddTail(new(__FILE__, __LINE__) ScrollerDatumNode(string, hash));
    }
    ScrollerDatumNode* Find(const char* to_find);
    ScrollerDatumNode* Find(unsigned int hash);
    void Printf();
    char* GetTopDatumModeString();
    void Enable() { bEnabled = true; }
    void Disable() { bEnabled = false; }
    bool IsEnabled() { return bEnabled; }
};

struct ScrollerSlot : public bTNode<ScrollerSlot> {
    bTList<ScrollerSlotNode> FEStrings; // offset 0x8
    FEObject* pBacking;                 // offset 0x10
    bVector2 vTopLeft;                  // offset 0x14
    bVector2 vSize;                     // offset 0x1C
    bool bEnabled;                      // offset 0x24

    ScrollerSlot() {}
    ScrollerSlot(FEObject* string);
    virtual ~ScrollerSlot() {}
    void AddData(FEObject* string) {
        FEStrings.AddTail(new(__FILE__, __LINE__) ScrollerSlotNode(string));
    }
    void SetBacking(FEObject* obj) { pBacking = obj; }
    void Highlight();
    void UnHighlight();
    void Enable() { bEnabled = true; }
    void Disable() { bEnabled = false; }
    void GetSize(bVector2& size) { size = vSize; }
    void GetTopLeft(bVector2& top_left) { top_left = vTopLeft; }
    bool IsEnabled() { return bEnabled; }
    void SetScript(unsigned int script_hash);
    void FindSize();
    void Show();
    void Hide();
    bool Find(FEObject* obj);
};

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
    virtual ~IconOption() {}
    virtual void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) = 0;

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

inline unsigned int IconOption::GetName() { return NameHash; }
inline void IconOption::SetReactImmediately(bool b) { bReactImmediately = b; }
inline bool IconOption::ReactsImmediately() { return bReactImmediately; }
inline bool IconOption::IsTutorialAvailable() { return bIsTutorialAvailable; }
inline const char* IconOption::GetTutorialMovieName() { return pTutorialMovieName; }

struct FEScrollyBookEnd : public IconOption {
    FEScrollyBookEnd(unsigned int tex_hash) : IconOption(tex_hash, 0, 0) {}
    void React(const char *pkg_name, unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) override {}
};

#endif
