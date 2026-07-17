#ifndef ENGINES_AEMS2_NG_H
#define ENGINES_AEMS2_NG_H

#include <csis/csis.h>

namespace Csis {

extern InterfaceId CARId;      // size: 0x8, address: 0x80418818, Decl: 29
extern ClassHandle gCARHandle; // size: 0x8, address: 0x8045E7B4, Decl: 30

// total size: 0x68
// Decl: 32
typedef struct {
    int car_class;                      // offset 0x0, size 0x4
    int rPM;                            // offset 0x4, size 0x4
    int tRQ_ENG;                        // offset 0x8, size 0x4
    int tORQUE;                         // offset 0xC, size 0x4
    int vOL_ENG;                        // offset 0x10, size 0x4
    int vOL_EXH;                        // offset 0x14, size 0x4
    int tRQ_LFO_AMP;                    // offset 0x18, size 0x4
    int tRQ_LFO_FREQ;                   // offset 0x1C, size 0x4
    int rPMLFO_AMP;                     // offset 0x20, size 0x4
    int rPM_LFO_FREQ;                   // offset 0x24, size 0x4
    int vOL_LFO_AMP;                    // offset 0x28, size 0x4
    int vOL_LFO_FREQ;                   // offset 0x2C, size 0x4
    int sPU_or_EE;                      // offset 0x30, size 0x4
    int fX_DRY;                         // offset 0x34, size 0x4
    int fX_AMOUNT;                      // offset 0x38, size 0x4
    int cOMMON_PARAMETERS_AZIMUTH;      // offset 0x3C, size 0x4
    int cOMMON_PARAMETERS_PITCH_OFFSET; // offset 0x40, size 0x4
    int cOMMON_PARAMETERS_ROTATION;     // offset 0x44, size 0x4
    int tWEAKERS_XOVER_IDLE_2_LO;       // offset 0x48, size 0x4
    int tWEAKERS_XOVER_LO_2_MID;        // offset 0x4C, size 0x4
    int tWEAKERS_XOVER_MID_2_HI;        // offset 0x50, size 0x4
    int fILTERS_FILTER;                 // offset 0x54, size 0x4
    int fILTERS_FILTER_RND;             // offset 0x58, size 0x4
    int fILTERS_FILTER_Dist;            // offset 0x5C, size 0x4
    int fILTERS_FILTER_Trig;            // offset 0x60, size 0x4
    int mAX_RPM;                        // offset 0x64, size 0x4
} CARStruct;

extern InterfaceId CAR_SWTNId;      // size: 0x8, Decl: 498
extern ClassHandle gCAR_SWTNHandle; // size: 0x8, address: 0x8045E7BC, Decl: 499

// total size: 0x1C
// Decl: 501
typedef struct {
    int id;                             // offset 0x0, size 0x4, Decl: 503
    int car_class;                      // offset 0x4, size 0x4, Decl: 506
    int rPM;                            // offset 0x8, size 0x4, Decl: 509
    int vOL;                            // offset 0xC, size 0x4, Decl: 512
    int cOMMON_PARAMETERS_AZIMUTH;      // offset 0x10, size 0x4, Decl: 515
    int cOMMON_PARAMETERS_PITCH_OFFSET; // offset 0x14, size 0x4, Decl: 518
    int cOMMON_PARAMETERS_ROTATION;     // offset 0x18, size 0x4, Decl: 521
} CAR_SWTNStruct;

extern InterfaceId CAR_WHINEId;      // size: 0x8, Decl: 524
extern ClassHandle gCAR_WHINEHandle; // size: 0x8, address: 0x8045E7C4, Decl: 525

// Decl: 527
typedef struct {
    int car_class;                      // offset 0x0, size 0x4, Decl: 530
    int rPM;                            // offset 0x4, size 0x4, Decl: 533
    int vOL;                            // offset 0x8, size 0x4, Decl: 536
    int cOMMON_PARAMETERS_AZIMUTH;      // offset 0xC, size 0x4, Decl: 539
    int cOMMON_PARAMETERS_PITCH_OFFSET; // offset 0x10, size 0x4, Decl: 542
    int cOMMON_PARAMETERS_ROTATION;     // offset 0x14, size 0x4, Decl: 545
    int rEVERB_AND_FILTERS_LoPass;      // offset 0x18, size 0x4, Decl: 548
    int rEVERB_AND_FILTERS_Wet;         // offset 0x1C, size 0x4, Decl: 551
    int rEVERB_AND_FILTERS_Dry;         // offset 0x20, size 0x4, Decl: 554
} CAR_WHINEStruct;

extern InterfaceId CAR_TRANNYId;
extern ClassHandle gCAR_TRANNYHandle;

// total size: 0x24
// Decl: 560
typedef struct {
    int car_class;                 // offset 0x0, size 0x4
    int magnitude;                 // offset 0x4, size 0x4
    int vOL;                       // offset 0x8, size 0x4
    int aZIMUTH;                   // offset 0xC, size 0x4
    int pITCH_OFFSET;              // offset 0x10, size 0x4
    int rEVERB_AND_FILTERS_LoPass; // offset 0x14, size 0x4
    int rEVERB_AND_FILTERS_Wet;    // offset 0x18, size 0x4
    int rEVERB_AND_FILTERS_Dry;    // offset 0x1C, size 0x4
    int single_Shot;               // offset 0x20, size 0x4
} CAR_TRANNYStruct;

extern InterfaceId CAR_SputterId;
extern ClassHandle gCAR_SputterHandle;

// Decl: 593
typedef struct {
    int car_class;                      // offset 0x0, size 0x4, Decl: 596
    int car_id;                         // offset 0x4, size 0x4, Decl: 597
    int rPM;                            // offset 0x8, size 0x4, Decl: 600
    int vOL;                            // offset 0xC, size 0x4, Decl: 603
    int cOMMON_PARAMETERS_AZIMUTH;      // offset 0x10, size 0x4, Decl: 606
    int cOMMON_PARAMETERS_PITCH_OFFSET; // offset 0x14, size 0x4, Decl: 609
    int cOMMON_PARAMETERS_ROTATION;     // offset 0x18, size 0x4, Decl: 612
    int tORQUE;                         // offset 0x1C, size 0x4, Decl: 615
    int force_Trigger;                  // offset 0x20, size 0x4, Decl: 618
    int accel_true;                     // offset 0x24, size 0x4, Decl: 621
    int shifting_true;                  // offset 0x28, size 0x4, Decl: 624
} CAR_SputterStruct;

extern InterfaceId CAR_SputOutputId;      // size: 0x8, Decl: 627
extern ClassHandle gCAR_SputOutputHandle; // size: 0x8, address: 0x8045E7DC, Decl: 628

// total size: 0xC
// Decl: 630
typedef struct {
    int volume;    // offset 0x0, size 0x4
    int car_class; // offset 0x4, size 0x4
    int car_id;    // offset 0x8, size 0x4
} CAR_SputOutputStruct;

// Decl: 691
class CAR {
  public:
    void SetCar_class(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 99) {
            x = 99;
        }
        mData.car_class = x;
    }

    void SetRPM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 10000) {
            x = 10000;
        }
        mData.rPM = x;
    }

    void SetTRQ_ENG(int x) {
        mData.tRQ_ENG = x;
    }

    void SetTORQUE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 1024) {
            x = 1024;
        }
        mData.tORQUE = x;
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

    void SetTRQ_LFO_AMP(int x) {
        mData.tRQ_LFO_AMP = x;
    }

    void SetTRQ_LFO_FREQ(int x) {
        mData.tRQ_LFO_FREQ = x;
    }

    void SetRPMLFO_AMP(int x) {
        mData.rPMLFO_AMP = x;
    }

    void SetRPM_LFO_FREQ(int x) {
        mData.rPM_LFO_FREQ = x;
    }

    void SetVOL_LFO_AMP(int x) {
        mData.vOL_LFO_AMP = x;
    }

    void SetVOL_LFO_FREQ(int x) {
        mData.vOL_LFO_FREQ = x;
    }

    void SetSPU_or_EE(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 2) {
            x = 2;
        }
        mData.sPU_or_EE = x;
    }

    void SetFX_DRY(int x) {
        mData.fX_DRY = x;
    }

    void SetFX_AMOUNT(int x) {
        mData.fX_AMOUNT = x;
    }

    void SetCOMMON_PARAMETERS_AZIMUTH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x10000) {
            x = 0x10000;
        }
        mData.cOMMON_PARAMETERS_AZIMUTH = x;
    }

    void SetCOMMON_PARAMETERS_PITCH_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 0x3FFF) {
            x = 0x3FFF;
        }
        mData.cOMMON_PARAMETERS_PITCH_OFFSET = x;
    }

    void SetCOMMON_PARAMETERS_ROTATION(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x400) {
            x = 0x400;
        }
        mData.cOMMON_PARAMETERS_ROTATION = x;
    }

    void SetTWEAKERS_XOVER_IDLE_2_LO(int x) {
        mData.tWEAKERS_XOVER_IDLE_2_LO = x;
    }

    void SetTWEAKERS_XOVER_LO_2_MID(int x) {
        mData.tWEAKERS_XOVER_LO_2_MID = x;
    }

    void SetTWEAKERS_XOVER_MID_2_HI(int x) {
        mData.tWEAKERS_XOVER_MID_2_HI = x;
    }

    void SetFILTERS_FILTER(int x) {
        mData.fILTERS_FILTER = x;
    }

    void SetFILTERS_FILTER_RND(int x) {
        mData.fILTERS_FILTER_RND = x;
    }

    void SetFILTERS_FILTER_Dist(int x) {
        mData.fILTERS_FILTER_Dist = x;
    }

    void SetFILTERS_FILTER_Trig(int x) {
        mData.fILTERS_FILTER_Trig = x;
    }

    void SetMAX_RPM(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 0x7FFF) {
            x = 0x7FFF;
        }
        mData.mAX_RPM = x;
    }

    int GetRefCount() {
        int refCount = 0;

        if (mpClass != nullptr) {
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    static void operator delete(void *ptr) {
        System::Free(ptr);
    }

    CAR(int car_class, int rPM, int tRQ_ENG, int tORQUE, int vOL_ENG, int vOL_EXH, int tRQ_LFO_AMP, int tRQ_LFO_FREQ, int rPMLFO_AMP,
        int rPM_LFO_FREQ, int vOL_LFO_AMP, int vOL_LFO_FREQ, int sPU_or_EE, int fX_DRY, int fX_AMOUNT, int cOMMON_PARAMETERS_AZIMUTH,
        int cOMMON_PARAMETERS_PITCH_OFFSET, int cOMMON_PARAMETERS_ROTATION, int tWEAKERS_XOVER_IDLE_2_LO, int tWEAKERS_XOVER_LO_2_MID,
        int tWEAKERS_XOVER_MID_2_HI, int fILTERS_FILTER, int fILTERS_FILTER_RND, int fILTERS_FILTER_Dist, int fILTERS_FILTER_Trig, int mAX_RPM) {
        SetCar_class(car_class);
        SetRPM(rPM);
        SetTRQ_ENG(tRQ_ENG);
        SetTORQUE(tORQUE);
        SetVOL_ENG(vOL_ENG);
        SetVOL_EXH(vOL_EXH);
        SetTRQ_LFO_AMP(tRQ_LFO_AMP);
        SetTRQ_LFO_FREQ(tRQ_LFO_FREQ);
        SetRPMLFO_AMP(rPMLFO_AMP);
        SetRPM_LFO_FREQ(rPM_LFO_FREQ);
        SetVOL_LFO_AMP(vOL_LFO_AMP);
        SetVOL_LFO_FREQ(vOL_LFO_FREQ);
        SetSPU_or_EE(sPU_or_EE);
        SetFX_DRY(fX_DRY);
        SetFX_AMOUNT(fX_AMOUNT);
        SetCOMMON_PARAMETERS_AZIMUTH(cOMMON_PARAMETERS_AZIMUTH);
        SetCOMMON_PARAMETERS_PITCH_OFFSET(cOMMON_PARAMETERS_PITCH_OFFSET);
        SetCOMMON_PARAMETERS_ROTATION(cOMMON_PARAMETERS_ROTATION);
        SetTWEAKERS_XOVER_IDLE_2_LO(tWEAKERS_XOVER_IDLE_2_LO);
        SetTWEAKERS_XOVER_LO_2_MID(tWEAKERS_XOVER_LO_2_MID);
        SetTWEAKERS_XOVER_MID_2_HI(tWEAKERS_XOVER_MID_2_HI);
        SetFILTERS_FILTER(fILTERS_FILTER);
        SetFILTERS_FILTER_RND(fILTERS_FILTER_RND);
        SetFILTERS_FILTER_Dist(fILTERS_FILTER_Dist);
        SetFILTERS_FILTER_Trig(fILTERS_FILTER_Trig);
        SetMAX_RPM(mAX_RPM);

        Result result = Result(Class::CreateInstance(&gCARHandle, &mData, &mpClass));
        if (result < 0) {
            gCARHandle.Set(&CARId);
            Class::CreateInstance(&gCARHandle, &mData, &mpClass);
        }
    }

    ~CAR() {
        if (mpClass != nullptr) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass != nullptr) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    CAR();
    CAR &operator=(const CAR &);

    Class *mpClass;  // offset 0x0, size 0x4
    CARStruct mData; // offset 0x4, size 0x68
};

// total size: 0x28
// Decl: 4685
class CAR_TRANNY {
  public:
    void SetCar_class(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 60) {
            x = 60;
        }
        mData.car_class = x;
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
        } else if (x > 32787) {
            x = 32787;
        }
        mData.vOL = x;
    }

    void SetAZIMUTH(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 65536) {
            x = 65536;
        }
        mData.aZIMUTH = x;
    }

    void SetPITCH_OFFSET(int x) {
        if (x < -0x3FFF) {
            x = -0x3FFF;
        } else if (x > 16383) {
            x = 16383;
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
        } else if (x > 32767) {
            x = 32767;
        }
        mData.rEVERB_AND_FILTERS_Wet = x;
    }

    void SetREVERB_AND_FILTERS_Dry(int x) {
        if (x < 0) {
            x = 0;
        } else if (x > 32767) {
            x = 32767;
        }
        mData.rEVERB_AND_FILTERS_Dry = x;
    }

    void SetSingle_Shot(int x) {
        mData.single_Shot = x;
    }

    int GetRefCount() {
        int refCount = 0;

        if (mpClass != nullptr) {
            mpClass->GetRefCount(&refCount);
        }

        return refCount;
    }

    void *operator new(size_t size) {
        return System::Alloc(size);
    }

    void operator delete(void *ptr) {
        System::Free(ptr);
    }

    CAR_TRANNY(int car_class, int magnitude, int vOL, int aZIMUTH, int pITCH_OFFSET, int rEVERB_AND_FILTERS_LoPass, int rEVERB_AND_FILTERS_Wet,
               int rEVERB_AND_FILTERS_Dry, int single_Shot) {
        SetCar_class(car_class);
        SetMagnitude(magnitude);
        SetVOL(vOL);
        SetAZIMUTH(aZIMUTH);
        SetPITCH_OFFSET(pITCH_OFFSET);
        SetREVERB_AND_FILTERS_LoPass(rEVERB_AND_FILTERS_LoPass);
        SetREVERB_AND_FILTERS_Wet(rEVERB_AND_FILTERS_Wet);
        SetREVERB_AND_FILTERS_Dry(rEVERB_AND_FILTERS_Dry);
        SetSingle_Shot(single_Shot);

        Result result = Class::CreateInstance(&gCAR_TRANNYHandle, &mData, &mpClass);
        if (result < 0) {
            gCAR_TRANNYHandle.Set(&CAR_TRANNYId);
            Class::CreateInstance(&gCAR_TRANNYHandle, &mData, &mpClass);
        }
    }

    ~CAR_TRANNY() {
        if (mpClass != nullptr) {
            mpClass->Release();
        }
    }

    void CommitMemberData() {
        if (mpClass != nullptr) {
            mpClass->SetMemberData(&mData);
        }
    }

  private:
    CAR_TRANNY();
    CAR_TRANNY &operator=(const CAR_TRANNY &);

    Csis::Class *mpClass;   // offset 0x0, size 0x4
    CAR_TRANNYStruct mData; // offset 0x4, size 0x24
};

}; // namespace Csis

#endif
