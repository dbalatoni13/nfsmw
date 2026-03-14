#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"

extern int bStrLen(const unsigned short *s);

void FEngSNMakeHidden(char *outBuffer, int out_buf_size, const char *strInput) {
    int nLen = bStrLen(strInput);
    int i = 0;
    if (i < nLen && i != out_buf_size - 1) {
        do {
            outBuffer[i] = '*';
            i++;
            if (i >= nLen) break;
        } while (i != out_buf_size - 1);
    }
    outBuffer[i] = '\0';
}

void FEngSNMakeHidden(char *outBuffer, int out_buf_size, unsigned short *strInput) {
    int nLen = bStrLen(strInput);
    int i = 0;
    if (i < nLen && i != out_buf_size - 1) {
        do {
            outBuffer[i] = '*';
            i++;
            if (i >= nLen) break;
        } while (i != out_buf_size - 1);
    }
    outBuffer[i] = '\0';
}

int FEngMapJoyParamToJoyport(int feng_param) {
    if (feng_param & 1) return 0;
    if (feng_param & 2) return 1;
    if (feng_param & 4) return 2;
    if (feng_param & 8) return 3;
    return -1;
}

int FEngMapJoyportToJoyParam(int joyport) {
    if (joyport == 0) return 1;
    if (joyport == 1) return 2;
    if (joyport == 2) return 4;
    if (joyport == 3) return 8;
    return 0;
}

void FEngTickSinglePackage(const char *pkg_name, unsigned int ticks) {
    FEPackage *single_package = cFEng::Get()->FindPackage(pkg_name);
    if (single_package) {
        single_package->SetTickIncrement(ticks);
        FEObject *pObject = single_package->GetFirstObject();
        while (pObject) {
            single_package->UpdateObject(pObject, ticks);
            pObject = pObject->GetNext();
        }
    }
}
