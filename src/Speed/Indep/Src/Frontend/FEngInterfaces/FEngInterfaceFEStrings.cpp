#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include <stdarg.h>

static char FEPrintf_Buffer[1024];

// File: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp
// total size: 0x8
// Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp:24
class FEngSetGroupLanguageHash : public FEObjectCallback {
  public:
    u32 Hash; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp:26

    bool Callback(struct FEObject *pObj) override { // Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp:28
        if (pObj->Type == FE_String) {
            FEngSetLanguageHash(static_cast<FEString *>(pObj), Hash);
        }
        return true;
    }
};

// total size: 0x8
// Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp:205
class FEngGroupFEPrintf : public FEObjectCallback {
  public:
    char *string; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp:207

    bool Callback(struct FEObject *pObj) override { // Decl: speed/indep/src/frontend/fenginterfaces/FEngInterfaceFEStrings.cpp:209
        if (pObj->Type == FE_String) {
            DoFEngPrintf(static_cast<FEString *>(pObj), string, 0);
        }
        return true;
    }
};

FEString *FEngFindString(const char *pkg_name, int name_hash) {
    FEObject *obj = FEngFindObject(pkg_name, name_hash);
    if (obj == nullptr || obj->Type != FE_String) {
        return nullptr;
    }
    return static_cast<FEString *>(obj);
}

void FEngSetLanguageHash(FEString *pkg_name, uint32 hash) {
    if (pkg_name != nullptr) {
        pkg_name->SetLabelHash(hash);
    }
}

void FEngSetLanguageHash(const char *pkg_name, uint32 obj_hash, uint32 language) {
    FEObject *obj = FEngFindObject(pkg_name, obj_hash);
    if (obj != nullptr) {
        if (obj->Type == FE_Group) {
            FEngSetGroupLanguageHash SetHash;
            SetHash.Hash = language;
            FEPackage *pkg = cFEng::Get()->FindPackage(pkg_name);
            pkg->ForAllChildren(static_cast<FEGroup *>(obj), SetHash);
        } else if (obj->Type == FE_String) {
            FEngSetLanguageHash(static_cast<FEString *>(obj), language);
        }
    }
}

void FESetString(FEString *text, const short *string) {
    if (string != nullptr && text != nullptr) {
        text->SetString(const_cast<short *>(string));
        text->Flags |= FF_DirtyCode | 0x2;
    }
}

static int DoFEngPrintf(FEString *text, char *string, int len) {
    if (text != nullptr) {
        int length = text->string.mulBufferLength;
        if (static_cast<unsigned long>(length) < 0x400) {
            string[length - 1] = 0;
        }
        text->SetString(string);
        text->Flags |= FF_DirtyCode | 0x2;
        return len;
    }
    return 0;
}

static int DoFEngPrintf(FEString *text, const char *fmt, va_list argList) {
    if (text != nullptr) {
        int nchars = bVSPrintf(FEPrintf_Buffer, fmt, argList);
        return DoFEngPrintf(text, FEPrintf_Buffer, nchars);
    }
    return 0;
}

int FEPrintf(FEString *text, const char *fmt, ...) {
    va_list argList;
    va_start(argList, fmt);
    if (text != nullptr) {
        if (text->string != nullptr) {
            return DoFEngPrintf(text, fmt, argList);
        }
    }
    return 0;
}

int FEPrintf(const char *pkg_name, int object_hash, const char *fmt, ...) {
    va_list argList;
    va_start(argList, fmt);

    FEObject *obj = FEngFindObject(pkg_name, object_hash);
    if (obj == nullptr) {
        return -1;
    }

    if (obj->Type == FE_Group) {
        va_list arg_list;
        va_start(arg_list, fmt);
        bVSPrintf(FEPrintf_Buffer, fmt, arg_list);
        va_end(arg_list);
        FEngGroupFEPrintf DoPrintf;
        DoPrintf.string = FEPrintf_Buffer;
        FEPackage *pkg = cFEng::Get()->FindPackage(pkg_name);
        pkg->ForAllChildren(static_cast<FEGroup *>(obj), DoPrintf);
    } else if (obj->Type == FE_String) {
        DoFEngPrintf(static_cast<FEString *>(obj), fmt, argList);
    }

    return 0;
}

int FEPrintf(const char *pkg_name, FEObject *obj, const char *fmt, ...) {
    va_list argList;
    va_start(argList, fmt);

    if (obj == nullptr) {
        return -1;
    }

    if (obj->Type == FE_Group) {
        va_list arg_list;
        va_start(arg_list, fmt);
        bVSPrintf(FEPrintf_Buffer, fmt, arg_list);
        va_end(arg_list);
        FEngGroupFEPrintf DoPrintf;
        DoPrintf.string = FEPrintf_Buffer;
        FEPackage *pkg = cFEng::Get()->FindPackage(pkg_name);
        pkg->ForAllChildren(static_cast<FEGroup *>(obj), DoPrintf);
    } else if (obj->Type == FE_String) {
        DoFEngPrintf(static_cast<FEString *>(obj), fmt, argList);
    }

    return 0;
}

int FEngSNPrintf(char *buffer, int buf_size, const char *fmt, ...) {
    va_list arg_list;
    va_start(arg_list, fmt);
    int nchars = bVSPrintf(buffer, fmt, arg_list);
    va_end(arg_list);
    return nchars;
}
