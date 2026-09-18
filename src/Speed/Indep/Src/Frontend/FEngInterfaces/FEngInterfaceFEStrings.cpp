#include "FEngInterfaceFEStrings.hpp"

#include "FEngInterface.hpp"
#include "FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

static char FEPrintf_Buffer[1024];

FEString *FEngFindString(const char *pkg_name, int name_hash) {
    FEObject *obj = FEngFindObject(pkg_name, name_hash);

    if (obj == nullptr || obj->Type != FE_String) {
        return nullptr;
    }
    return (FEString *) obj;
}

void FEngSetLanguageHash(FEString *text, unsigned int hash) {
    if (text != nullptr) {
        text->SetLabelHash(hash);
        text->Flags = (text->Flags & ~0x2) | 0x400000;
    }
}

// total size: 0x8
// r59 (feov): la clase vive aqui, no en la cabecera: `parseord zFe` da su puesto de
// COMPLETADO en 130 en el objetivo y en 8 con la clase en el .hpp (que arrastra
// uiMain.cpp por la primera linea de zFe.cpp).  El `Callback` va DENTRO para que el
// cuerpo se difiera a la cola de finish_file, que es donde el objetivo lo emite.
class FEngSetGroupLanguageHash : public FEObjectCallback {
  public:
    FEngSetGroupLanguageHash() {}
    virtual ~FEngSetGroupLanguageHash() {}
    virtual bool Callback(FEObject *pObj) {
        if (pObj->Type == FE_String) {
            FEngSetLanguageHash((FEString *) pObj, Hash);
        }
        return true;
    }

    u32 Hash; // offset 0x4, size 0x4
};

void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int language) {
    FEObject *obj = FEngFindObject(pkg_name, obj_hash);

    if (obj != nullptr) {
        if (obj->Type == FE_Group) {
            FEngSetGroupLanguageHash SetHash;

            SetHash.Hash = language;
            cFEng::Get()->FindPackage(pkg_name)->ForAllChildren((FEGroup *) obj, SetHash);
        } else if (obj->Type == FE_String) {
            FEngSetLanguageHash((FEString *) obj, language);
        }
    }
}

void FESetString(FEString *text, const short *string) {
    if (string != nullptr && text != nullptr) {
        text->SetString((short *) string);
        text->Flags |= FF_DirtyCode | 2;
    }
}

static int DoFEngPrintf(FEString *text, char *string, int len) {
    if (text != nullptr) {
        u32 length = text->string.mulBufferLength;

        if (length <= 1023) {
            string[length - 1] = '\0';
        }

        text->SetString(string);
        text->Flags |= FF_DirtyCode | 2;

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

// total size: 0x8
// r59 (feov): idem, puesto 131 del objetivo.  Tiene que ir DETRAS de DoFEngPrintf
// porque el cuerpo en clase lo llama, y esa es la razon por la que la clase no puede
// quedarse en la cabecera (DoFEngPrintf es `static` de este .cpp).
class FEngGroupFEPrintf : public FEObjectCallback {
  public:
    FEngGroupFEPrintf() {}
    virtual ~FEngGroupFEPrintf() {}
    virtual bool Callback(FEObject *pObj) {
        if (pObj->Type == FE_String) {
            DoFEngPrintf((FEString *) pObj, string, 0);
        }
        return true;
    }

    char *string; // offset 0x4, size 0x4
};

int FEPrintf(FEString *text, const char *fmt, ...) {
    va_list argList;

    va_start(argList, fmt);

    if (text != nullptr && text->GetString() != nullptr) {
        return DoFEngPrintf(text, fmt, argList);
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

        int nchars = bVSPrintf(FEPrintf_Buffer, fmt, arg_list);
        FEngGroupFEPrintf DoPrintf;

        DoPrintf.string = FEPrintf_Buffer;
        cFEng::Get()->FindPackage(pkg_name)->ForAllChildren((FEGroup *) obj, DoPrintf);
    } else if (obj->Type == FE_String) {
        DoFEngPrintf((FEString *) obj, fmt, argList);
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

        int nchars = bVSPrintf(FEPrintf_Buffer, fmt, arg_list);
        FEngGroupFEPrintf DoPrintf;

        DoPrintf.string = FEPrintf_Buffer;
        cFEng::Get()->FindPackage(pkg_name)->ForAllChildren((FEGroup *) obj, DoPrintf);
    } else if (obj->Type == FE_String) {
        DoFEngPrintf((FEString *) obj, fmt, argList);
    }
    return 0;
}

int FEngSNPrintf(char *buffer, int buf_size, const char *fmt, ...) {
    va_list arg_list;

    va_start(arg_list, fmt);

    int nchars = bVSPrintf(buffer, fmt, arg_list);

    return nchars;
}
