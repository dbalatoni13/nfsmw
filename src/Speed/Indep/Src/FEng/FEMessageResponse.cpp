#include "FEMessageResponse.h"
#include "FEngStandard.h"
#include "ObjectPool.h"

FEResponse::~FEResponse() {
    ReleaseParam();
}

FEResponse &FEResponse::operator=(FEResponse &rhs) {
    ReleaseParam();
    ResponseID = rhs.ResponseID;
    if (FEResponse::HasString(ResponseID)) {
        SetParam(reinterpret_cast<const char *>(rhs.ResponseParam));
    } else {
        ResponseParam = rhs.ResponseParam;
    }
    ResponseTarget = rhs.ResponseTarget;
    return *this;
}

void FEResponse::SetParam(const char *pString) {
    ReleaseParam();
    if (pString) {
        u32 Len = FEngStrLen(pString);
        char *pPathCopy = FNEW char[Len + 1];
        FEngStrCpy(pPathCopy, pString);
        ResponseParam = reinterpret_cast<u32>(pPathCopy);
    }
}

void FEResponse::ReleaseParam() {
    if (HasString() && ResponseParam) {
        delete[] reinterpret_cast<char *>(ResponseParam);
    }
    ResponseParam = 0;
}

ObjectPool<FEMessageResponse, 64> FEMessageResponse::NodePool;

FEMessageResponse::~FEMessageResponse() {
    PurgeResponses();
}

void *FEMessageResponse::operator new(size_t) {
    FEMessageResponse *pNode = NodePool.AllocSingle();
    pNode->Init();
    return pNode;
}

void FEMessageResponse::operator delete(void *pNode) {
    FEMessageResponse *pDeleteNode = static_cast<FEMessageResponse *>(pNode);
    pDeleteNode->~FEMessageResponse();
    NodePool.FreeSingle(pDeleteNode);
}

void FEMessageResponse::PurgeResponses() {
    delete[] pResponseList;
    pResponseList = nullptr;
    Count = 0;
}

void FEMessageResponse::SetCount(u32 NewCount) {
    if (NewCount != Count) {
        if (NewCount == 0) {
            PurgeResponses();
        } else {
            FEResponse *pNewList = FNEW FEResponse[NewCount];
            u32 copyCount = Count;
            if (copyCount > NewCount) {
                copyCount = NewCount;
            }
            u32 i = 0;
            while (i < copyCount) {
                pNewList[i] = pResponseList[i];
                i++;
            }
            delete[] pResponseList;
            pResponseList = pNewList;
            Count = NewCount;
        }
    }
}

u32 FEMessageResponse::FindResponse(u32 ResponseID) const {
    for (u32 i = 0; i < Count; i++) {
        if (pResponseList[i].ResponseID == ResponseID) {
            return i;
        }
    }
    return 0xFFFFFFFF;
}

/* CERRADA CONTRA EL DWARF, 144 B. El volcado del original da la estructura
 * entera y no se parecia a ninguna de las tres formas que se habian medido
 * (`goto` 74,9%, `while` con la comprobacion al final 72,2%, directa 62,6%):
 *
 *     unsigned long Nest;    // r10
 *     unsigned long Result;  // r4   <- COMPARTE r4 CON Index
 *
 * O sea: DOS locales a nivel de funcion, ninguna para Count ni para
 * pResponseList, ni etiqueta ni bloque anonimo. `Result` es el indice que
 * corre --por eso comparte registro con el parametro-- y `Nest` es el
 * contador de anidamiento, que es lo que nosotros llamabamos `Result`.
 *
 * Con `count` como local se llevaba r3 el parametro y el `return Count` de
 * salida costaba `bne`+`mr`+`blr` en vez del `beqlr` del objetivo. */
u32 FEMessageResponse::FindConditionBranchTarget(u32 Index) const {
    u32 Nest;
    u32 Result;

    if (Index == this->Count - 1) {
        return this->Count;
    }
    Nest = 1;
    Result = Index;
    do {
        Result++;
        switch (this->pResponseList[Result].ResponseID) {
            case MR_IfScriptEquals:
            case MR_IfScriptNotEquals:
                Nest++;
                break;
            case MR_Else:
                if (Nest == 1) {
                    Nest = 0;
                }
                break;
            case MR_EndIf:
                Nest--;
                break;
        }
    } while (Result < this->Count && Nest != 0);
    return Result;
}
