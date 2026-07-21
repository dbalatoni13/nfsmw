#ifndef FENG_FESTRING_H
#define FENG_FESTRING_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEObject.h"
#include "FEWideString.h"

class FEString : public FEObject {
  private:
    char *pLabelName;
    uint32 LabelHash;

  public:
    FEWideString string;
    uint32 Format;
    int32 Leading;
    uint32 MaxWidth;

    void SetStringFromUTF8(char *pUTF8String);
};

#endif
