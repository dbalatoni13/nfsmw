#ifndef EAXSOUND_SND_GEN_ENGINES_AEMS2_H
#define EAXSOUND_SND_GEN_ENGINES_AEMS2_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/Csis.hpp"

namespace Csis {
extern ClassHandle gCARHandle;
extern InterfaceId CARId;
extern ClassHandle gCAR_TRANNYHandle;
extern InterfaceId CAR_TRANNYId;
} // namespace Csis

struct CARStruct {
    int car_class;                       // offset 0x0, size 0x4
    int rPM;                             // offset 0x4, size 0x4
    int tRQ_ENG;                         // offset 0x8, size 0x4
    int tORQUE;                          // offset 0xC, size 0x4
    int vOL_ENG;                         // offset 0x10, size 0x4
    int vOL_EXH;                         // offset 0x14, size 0x4
    int tRQ_LFO_AMP;                     // offset 0x18, size 0x4
    int tRQ_LFO_FREQ;                    // offset 0x1C, size 0x4
    int rPMLFO_AMP;                      // offset 0x20, size 0x4
    int rPM_LFO_FREQ;                    // offset 0x24, size 0x4
    int vOL_LFO_AMP;                     // offset 0x28, size 0x4
    int vOL_LFO_FREQ;                    // offset 0x2C, size 0x4
    int sPU_or_EE;                       // offset 0x30, size 0x4
    int fX_DRY;                          // offset 0x34, size 0x4
    int fX_AMOUNT;                       // offset 0x38, size 0x4
    int cOMMON_PARAMETERS_AZIMUTH;       // offset 0x3C, size 0x4
    int cOMMON_PARAMETERS_PITCH_OFFSET;  // offset 0x40, size 0x4
    int cOMMON_PARAMETERS_ROTATION;      // offset 0x44, size 0x4
    int tWEAKERS_XOVER_IDLE_2_LO;        // offset 0x48, size 0x4
    int tWEAKERS_XOVER_LO_2_MID;         // offset 0x4C, size 0x4
    int tWEAKERS_XOVER_MID_2_HI;         // offset 0x50, size 0x4
    int fILTERS_FILTER;                  // offset 0x54, size 0x4
    int fILTERS_FILTER_RND;              // offset 0x58, size 0x4
    int fILTERS_FILTER_Dist;             // offset 0x5C, size 0x4
    int fILTERS_FILTER_Trig;             // offset 0x60, size 0x4
    int mAX_RPM;                         // offset 0x64, size 0x4
};

class CAR {
  public:
    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    ~CAR() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void SetVOL_ENG(int x) {
        if (x < -0x7FFF) {
            x = -0x7FFF;
        } else if (x > 0) {
            x = 0;
        }
        mData.vOL_ENG = x;
    }

    void SetVOL_EXH(int x) {
        if (x < -0x7FFF) {
            x = -0x7FFF;
        } else if (x > 0) {
            x = 0;
        }
        mData.vOL_EXH = x;
    }

    void SetMAX_RPM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.mAX_RPM = x;
    }

    void SetRPM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 10000) {
            x = 10000;
        }
        mData.rPM = x;
    }

    void SetCOMMON_PARAMETERS_PITCH_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        mData.cOMMON_PARAMETERS_PITCH_OFFSET = x;
    }

    void SetTORQUE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.tORQUE = x;
    }

    void SetCOMMON_PARAMETERS_ROTATION(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.cOMMON_PARAMETERS_ROTATION = x;
    }

    void SetCOMMON_PARAMETERS_AZIMUTH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        mData.cOMMON_PARAMETERS_AZIMUTH = x;
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    Csis::Class *mpClass;  // offset 0x0, size 0x4
    CARStruct mData;       // offset 0x4, size 0x68
};

struct CAR_TRANNYStruct {
    int car_class;                      // offset 0x0, size 0x4
    int magnitude;                      // offset 0x4, size 0x4
    int vOL;                            // offset 0x8, size 0x4
    int aZIMUTH;                        // offset 0xC, size 0x4
    int pITCH_OFFSET;                   // offset 0x10, size 0x4
    int rEVERB_AND_FILTERS_LoPass;      // offset 0x14, size 0x4
    int rEVERB_AND_FILTERS_Wet;         // offset 0x18, size 0x4
    int rEVERB_AND_FILTERS_Dry;         // offset 0x1C, size 0x4
    int single_Shot;                    // offset 0x20, size 0x4
};

class CAR_TRANNY {
  public:
    static void *operator new(unsigned int size) {
        return Csis::System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        Csis::System::Free(ptr);
    }

    ~CAR_TRANNY() {
        if (mpClass) {
            mpClass->Release();
        }
    }

    void SetMagnitude(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 10000) {
            x = 10000;
        }
        mData.magnitude = x;
    }

    void SetVOL(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x8013) {
            x = 0x8013;
        }
        mData.vOL = x;
    }

    void SetAZIMUTH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        mData.aZIMUTH = x;
    }

    void SetPITCH_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        mData.pITCH_OFFSET = x;
    }

    void SetREVERB_AND_FILTERS_LoPass(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 25000) {
            x = 25000;
        }
        mData.rEVERB_AND_FILTERS_LoPass = x;
    }

    void SetREVERB_AND_FILTERS_Wet(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.rEVERB_AND_FILTERS_Wet = x;
    }

    void SetREVERB_AND_FILTERS_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.rEVERB_AND_FILTERS_Dry = x;
    }

    void SetSingle_Shot(int x) {
        mData.single_Shot = x;
    }

    void CommitMemberData() {
        if (mpClass) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    Csis::Class *mpClass;       // offset 0x0, size 0x4
    CAR_TRANNYStruct mData;     // offset 0x4, size 0x24
};

#endif
