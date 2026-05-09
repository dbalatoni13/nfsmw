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
int FEngSNPrintf(char *dest, int size, const char *fmt, ...);

class ScrollerDatumNode : public bTNode<ScrollerDatumNode> {
  public:
    ScrollerDatumNode(const char *string, uint32 hash) {
        FEngSNPrintf(String, 0x80, string);
        LanguageHash = hash;
    }

    virtual ~ScrollerDatumNode() {}

    char String[128];    // offset 0x8
    uint32 LanguageHash; // offset 0x88
};

class ScrollerSlotNode : public bTNode<ScrollerSlotNode> {
  public:
    ScrollerSlotNode(FEObject *string) {
        String = string;
    }

    virtual ~ScrollerSlotNode() {}

    FEObject *String; // offset 0x8
};

class ScrollerDatum : public bTNode<ScrollerDatum> {
  public:
    ScrollerDatum() {}
    ScrollerDatum(const char *string, uint32 hash) {};
    virtual ~ScrollerDatum() {}

    void AddData(const char *string, uint32 hash) {
        Strings.AddTail(new (__FILE__, __LINE__) ScrollerDatumNode(string, hash));
    }
    char *GetTopDatumModeString() {};
    ScrollerDatumNode *Find(const char *to_find);
    ScrollerDatumNode *Find(uint32 hash);
    void Printf();
    void Enable() {
        bEnabled = true;
    }
    void Disable() {
        bEnabled = false;
    }
    bool IsEnabled() {
        return bEnabled;
    }

    bTList<ScrollerDatumNode> Strings; // offset 0x8
  private:
    bool bEnabled; // offset 0x10
};

class ScrollerSlot : public bTNode<ScrollerSlot> {
  public:
    ScrollerSlot() {}
    ScrollerSlot(FEObject *string) {};
    virtual ~ScrollerSlot() {}

    void AddData(FEObject *string) {
        FEStrings.AddTail(new (__FILE__, __LINE__) ScrollerSlotNode(string));
    }
    void SetBacking(FEObject *obj) {
        pBacking = obj;
    }
    void Hide();
    void Show();
    void Highlight() {};
    void UnHighlight() {};
    void Enable() {
        bEnabled = true;
    }
    void Disable() {
        bEnabled = false;
    }
    void SetScript(uint32 script_hash);
    void FindSize();
    void GetSize(bVector2 &size) {
        size = vSize;
    }
    void GetTopLeft(bVector2 &top_left) {
        top_left = vTopLeft;
    }
    bool IsEnabled() {
        return bEnabled;
    }

    bool Find(FEObject *obj);

    bTList<ScrollerSlotNode> FEStrings; // offset 0x8
    FEObject *pBacking;                 // offset 0x10
    bVector2 vTopLeft;                  // offset 0x14
    bVector2 vSize;                     // offset 0x1C
    bool bEnabled;                      // offset 0x24
};

// 0x5C
class IconOption : public bTNode<IconOption> {
  public:
    IconOption(uint32 tex_hash, uint32 name_hash, uint32 desc_hash);
    virtual ~IconOption() {}
    virtual void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) = 0;

    uint32 GetName() {
        return NameHash;
    };
    uint32 GetDesc() {
        return DescHash;
    };

    void StartScale(float scale_to, float duration);

    float GetScaleToPcnt() {
        return fScaleToPcnt;
    };
    float GetScaleStartSecs() {
        return fScaleStartSecs;
    };
    float GetScaleDurSecs() {
        return fScaleDurSecs;
    };
    float GetScaleAtStart() {
        return fScaleAtStart;
    };
    void SetScaleAtStart(float scale) {
        fScaleAtStart = scale;
    };
    bool IsAnimComplete() {
        return bAnimComplete;
    };
    void SetAnimComplete(bool b) {
        bAnimComplete = b;
    };
    bool ReactsImmediately() {
        return bReactImmediately;
    };
    bool IsLocked() {
        return Locked;
    };
    void SetLocked(bool b) {
        Locked = b;
    };
    void SetReactImmediately(bool b) {
        bReactImmediately = b;
    };
    bool IsTutorialAvailable() {
        return bIsTutorialAvailable;
    };
    const char *GetTutorialMovieName() {
        return pTutorialMovieName;
    };
    void SetTutorialMovieName(const char *name) {
        pTutorialMovieName = name;
    };

    void SetFEngObject(FEObject *obj);

    uint32 Item;          // 0x08
    FEObject *FEngObject; // 0x0C
    float XPos;           // 0x10
    float YPos;           // 0x14
    uint32 OriginalColor; // 0x18
    bool IsGreyOut;       // 0x1C
    bool IsFlashable;     // 0x20
    bool Locked;          // 0x24
    float OrigWidth;      // 0x28
    float OrigHeight;     // 0x2C

  private:
    uint32 NameHash;                // 0x30
    uint32 DescHash;                // 0x34
    float fScaleToPcnt;             // 0x38
    float fScaleStartSecs;          // 0x3C
    float fScaleDurSecs;            // 0x40
    float fScaleAtStart;            // 0x44
    bool bAnimComplete;             // 0x48
    bool bReactImmediately;         // 0x4C
    bool bIsTutorialAvailable;      // 0x50
    const char *pTutorialMovieName; // 0x54
};

class FEScrollyBookEnd : public IconOption {
  public:
    FEScrollyBookEnd(uint32 tex_hash) : IconOption(tex_hash, 0, 0) {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {}
};

#endif
