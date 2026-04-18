#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_NISStream.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Pathfinder.hpp"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/EAXSound/Stream/EAXS_StreamChannel.h"
#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

namespace Csis {
extern FunctionHandle gNIS_Select_StartHandle;
extern FunctionHandle gNIS_Select_BlacklistHandle;
extern FunctionHandle gNIS_Select_EndHandle;
extern InterfaceId NIS_Select_StartId;
extern InterfaceId NIS_Select_BlacklistId;
extern InterfaceId NIS_Select_EndId;
} // namespace Csis

namespace {

enum Type_NIS_Track {
    Type_NIS_Track_Track01 = 1,
    Type_NIS_Track_Track02 = 2,
    Type_NIS_Track_Track03 = 4,
    Type_NIS_Track_Track04 = 8,
    Type_NIS_Track_Track05 = 16,
    Type_NIS_Track_Track06 = 32,
    Type_NIS_Track_Track07 = 64,
    Type_NIS_Track_Track08 = 128,
    Type_NIS_Track_Track09 = 256,
    Type_NIS_Track_Track10 = 512,
    Type_NIS_Track_Track11 = 1024,
    Type_NIS_Track_Track12 = 2048,
    Type_NIS_Track_Track13 = 4096,
    Type_NIS_Track_Track00 = 8192,
};

enum Type_NIS_Scene_Start {
    Type_NIS_Scene_Start_IntroNis01 = 1,
    Type_NIS_Scene_Start_IntroNis02 = 2,
    Type_NIS_Scene_Start_IntroNis03 = 4,
    Type_NIS_Scene_Start_IntroNis04 = 8,
    Type_NIS_Scene_Start_IntroNis05 = 16,
    Type_NIS_Scene_Start_IntroNis06 = 32,
    Type_NIS_Scene_Start_IntroNis07 = 64,
    Type_NIS_Scene_Start_IntroNis08 = 128,
    Type_NIS_Scene_Start_IntroNis09 = 256,
    Type_NIS_Scene_Start_IntroNis10 = 512,
    Type_NIS_Scene_Start_IntroNis11 = 1024,
    Type_NIS_Scene_Start_dummy = 2048,
    Type_NIS_Scene_Start_flythrough = 4096,
    Type_NIS_Scene_Start_safehouse = 8192,
    Type_NIS_Scene_Start_dday = 16384,
    Type_NIS_Scene_Start_e3 = 32768,
    Type_NIS_Scene_Start_opm = 65536,
    Type_NIS_Scene_Start_toll = 131072,
    Type_NIS_Scene_Start_generic = 262144,
    Type_NIS_Scene_Start_ddayend = 524288,
};

enum Type_NIS_Blacklist {
    Type_NIS_Blacklist_Black01 = 1,
    Type_NIS_Blacklist_Black02 = 2,
    Type_NIS_Blacklist_Black03 = 4,
    Type_NIS_Blacklist_Black04 = 8,
    Type_NIS_Blacklist_Black05 = 16,
    Type_NIS_Blacklist_Black06 = 32,
    Type_NIS_Blacklist_Black07 = 64,
    Type_NIS_Blacklist_Black08 = 128,
    Type_NIS_Blacklist_Black09 = 256,
    Type_NIS_Blacklist_Black10 = 512,
    Type_NIS_Blacklist_Black11 = 1024,
    Type_NIS_Blacklist_Black12 = 2048,
    Type_NIS_Blacklist_Black13 = 4096,
    Type_NIS_Blacklist_Black14 = 8192,
    Type_NIS_Blacklist_Black15 = 16384,
    Type_NIS_Blacklist_Black16 = 32768,
    Type_NIS_Blacklist_Black06end = 65536,
    Type_NIS_Blacklist_Black07end = 131072,
    Type_NIS_Blacklist_generic = 262144,
};

enum Type_NIS_Scene_End {
    Type_NIS_Scene_End_Arrest_M01 = 1,
    Type_NIS_Scene_End_Arrest_M04 = 2,
    Type_NIS_Scene_End_Arrest_M06 = 4,
    Type_NIS_Scene_End_Arrest_M14 = 8,
    Type_NIS_Scene_End_Arrest_M16 = 16,
    Type_NIS_Scene_End_Arrest_M19 = 32,
    Type_NIS_Scene_End_Arrest_M23 = 64,
    Type_NIS_Scene_End_Arrest_F02 = 128,
    Type_NIS_Scene_End_Arrest_F06 = 256,
    Type_NIS_Scene_End_Arrest_F07 = 512,
    Type_NIS_Scene_End_Arrest_F14 = 1024,
    Type_NIS_Scene_End_Arrest_F23 = 2048,
    Type_NIS_Scene_End_E3 = 4096,
    Type_NIS_Scene_End_opm = 8192,
    Type_NIS_Scene_End_Arrest_F18 = 16384,
    Type_NIS_Scene_End_Arrest_M07 = 32768,
    Type_NIS_Scene_End_generic = 65536,
    Type_NIS_Scene_End_end01 = 131072,
    Type_NIS_Scene_End_end02 = 262144,
    Type_NIS_Scene_End_end03 = 524288,
    Type_NIS_Scene_End_end04 = 1048576,
};

enum Type_NIS_Section {
    Type_NIS_Section_Complete = 1,
    Type_NIS_Section_Start = 2,
    Type_NIS_Section_End = 4,
};

struct NIS_Select_StartStruct {
    Type_NIS_Scene_Start NIS_Scene_Start;
    Type_NIS_Track NIS_Track;
    Type_NIS_Section NIS_Section;
};

struct NIS_Select_BlacklistStruct {
    Type_NIS_Track NIS_Track;
    Type_NIS_Section NIS_Section;
    Type_NIS_Blacklist NIS_Blacklist;
};

struct NIS_Select_EndStruct {
    Type_NIS_Scene_End NIS_Scene_End;
    Type_NIS_Track NIS_Track;
    Type_NIS_Section NIS_Section;
};

static Csis::System::Result NIS_Select_Start(Type_NIS_Scene_Start NIS_Scene_Start, Type_NIS_Track NIS_Track,
                                             Type_NIS_Section NIS_Section) {
    NIS_Select_StartStruct data;
    data.NIS_Scene_Start = NIS_Scene_Start;
    data.NIS_Track = NIS_Track;
    data.NIS_Section = NIS_Section;
    return Csis::Function::Call(&Csis::gNIS_Select_StartHandle, &data);
}

static Csis::System::Result NIS_Select_Blacklist(Type_NIS_Track NIS_Track, Type_NIS_Section NIS_Section,
                                                 Type_NIS_Blacklist NIS_Blacklist) {
    NIS_Select_BlacklistStruct data;
    data.NIS_Track = NIS_Track;
    data.NIS_Section = NIS_Section;
    data.NIS_Blacklist = NIS_Blacklist;
    return Csis::Function::Call(&Csis::gNIS_Select_BlacklistHandle, &data);
}

static Csis::System::Result NIS_Select_End(Type_NIS_Scene_End NIS_Scene_End, Type_NIS_Track NIS_Track,
                                           Type_NIS_Section NIS_Section) {
    NIS_Select_EndStruct data;
    data.NIS_Scene_End = NIS_Scene_End;
    data.NIS_Track = NIS_Track;
    data.NIS_Section = NIS_Section;
    return Csis::Function::Call(&Csis::gNIS_Select_EndHandle, &data);
}

static Type_NIS_Track GetCSISTrack(int camera_track_number) {
    Type_NIS_Track csiscamtrack = Type_NIS_Track_Track00;

    if (camera_track_number == 6) {
        csiscamtrack = Type_NIS_Track_Track06;
    } else if (camera_track_number < 7) {
        if (camera_track_number == 2) {
            csiscamtrack = Type_NIS_Track_Track02;
        } else if (camera_track_number < 3) {
            if (camera_track_number != 0 && camera_track_number == 1) {
                csiscamtrack = Type_NIS_Track_Track01;
            }
        } else if (camera_track_number == 4) {
            csiscamtrack = Type_NIS_Track_Track04;
        } else if (camera_track_number < 5) {
            csiscamtrack = Type_NIS_Track_Track03;
        } else {
            csiscamtrack = Type_NIS_Track_Track05;
        }
    } else if (camera_track_number == 10) {
        csiscamtrack = Type_NIS_Track_Track10;
    } else if (camera_track_number < 11) {
        if (camera_track_number == 8) {
            csiscamtrack = Type_NIS_Track_Track08;
        } else if (camera_track_number < 9) {
            csiscamtrack = Type_NIS_Track_Track07;
        } else {
            csiscamtrack = Type_NIS_Track_Track09;
        }
    } else if (camera_track_number == 12) {
        csiscamtrack = Type_NIS_Track_Track12;
    } else if (camera_track_number < 12) {
        csiscamtrack = Type_NIS_Track_Track11;
    } else if (camera_track_number == 13) {
        csiscamtrack = Type_NIS_Track_Track13;
    }

    return csiscamtrack;
}

} // namespace

struct NISStringHashMapEntry {
    unsigned int Hash;
    unsigned int Flags;
    const char *Name;
};

extern int GetCsisEventIndex(unsigned int anim_id);
extern void SNDSYS_service();

bool SFXObj_NISStream::m_bNISButtonThroughAnimationReady;
bool SFXObj_NISStream::m_bNISAudioStreamReady;
bool SFXObj_NISStream::m_bNISButtonThroughReady;
bool SFXObj_NISStream::m_bIsButtonThrough;
int SFXObj_NISStream::m_mstimeelapsed;
int SFXObj_NISStream::m_mslengthofstream;
unsigned int g_laststartanimid;
bool g_bWasLastNISaStart;
SndBase::TypeInfo SFXObj_NISStream::s_TypeInfo = { 0, "SFXObj_NISStream", nullptr, SFXObj_NISStream::CreateObject };

NISStringHashMapEntry uNIS_STRINGHASHMAP[68];

SndBase::TypeInfo *SFXObj_NISStream::GetTypeInfo() const {
    return &s_TypeInfo;
}

const char *SFXObj_NISStream::GetTypeName() const {
    return s_TypeInfo.typeName;
}

SndBase *SFXObj_NISStream::CreateObject(unsigned int allocator) {
    if (!allocator) {
        return new (SFXObj_NISStream::GetStaticTypeInfo()->typeName, false) SFXObj_NISStream();
    }

    return new (SFXObj_NISStream::GetStaticTypeInfo()->typeName, true) SFXObj_NISStream();
}

int GetCsisEventIndex(unsigned int hashid) {
    for (int n = 0; n < 68; n++) {
        if (uNIS_STRINGHASHMAP[n].Hash == hashid) {
            return n;
        }
    }
    return -1;
}

#define INIT_NIS_ENTRY(index, flags, name) \
    uNIS_STRINGHASHMAP[index].Flags = flags; \
    uNIS_STRINGHASHMAP[index].Name = name; \
    uNIS_STRINGHASHMAP[index].Hash = bStringHash(name)

void GenerateNISAnimHashMap() {
    INIT_NIS_ENTRY(0x00, 0x40000, "GenericStart");
    INIT_NIS_ENTRY(0x01, 1, "IntroNis01");
    INIT_NIS_ENTRY(0x02, 2, "IntroNis02");
    INIT_NIS_ENTRY(0x03, 4, "IntroNis03");
    INIT_NIS_ENTRY(0x04, 8, "IntroNis04");
    INIT_NIS_ENTRY(0x05, 0x10, "IntroNis05");
    INIT_NIS_ENTRY(0x06, 0x20, "IntroNis06");
    INIT_NIS_ENTRY(0x07, 0x40, "IntroNis07");
    INIT_NIS_ENTRY(0x08, 0x80, "IntroNis08");
    INIT_NIS_ENTRY(0x09, 0x100, "IntroNis09");
    INIT_NIS_ENTRY(0x0A, 0x200, "IntroNis10");
    INIT_NIS_ENTRY(0x0B, 0x1000, "Flythrough");
    INIT_NIS_ENTRY(0x0C, 0x4000, "IntroNisDD");
    INIT_NIS_ENTRY(0x0D, 0x80000, "IntroNisDDEnd");
    INIT_NIS_ENTRY(0x0E, 0x20000, "IntroNisToll01");
    INIT_NIS_ENTRY(0x0F, 0x2000, "GarageEnter");
    INIT_NIS_ENTRY(0x10, 0x2000, "GarageExit");
    INIT_NIS_ENTRY(0x11, 0x8000, "OPMRivalIntro");
    INIT_NIS_ENTRY(0x12, 1, "IntroNisBL01");
    INIT_NIS_ENTRY(0x13, 2, "IntroNisBL02");
    INIT_NIS_ENTRY(0x14, 4, "IntroNisBL03");
    INIT_NIS_ENTRY(0x15, 8, "IntroNisBL04");
    INIT_NIS_ENTRY(0x16, 0x10, "IntroNisBL05");
    INIT_NIS_ENTRY(0x17, 0x20, "IntroNisBL06");
    INIT_NIS_ENTRY(0x18, 0x20, "IntroNisBL06End");
    INIT_NIS_ENTRY(0x19, 0x40, "IntroNisBL07");
    INIT_NIS_ENTRY(0x1A, 0x40, "IntroNisBL07End");
    INIT_NIS_ENTRY(0x1B, 0x80, "IntroNisBL08");
    INIT_NIS_ENTRY(0x1C, 0x100, "IntroNisBL09");
    INIT_NIS_ENTRY(0x1D, 0x200, "IntroNisBL10");
    INIT_NIS_ENTRY(0x1E, 0x400, "IntroNisBL11");
    INIT_NIS_ENTRY(0x1F, 0x800, "IntroNisBL12");
    INIT_NIS_ENTRY(0x20, 0x1000, "IntroNisBL13");
    INIT_NIS_ENTRY(0x21, 0x2000, "IntroNisBL14");
    INIT_NIS_ENTRY(0x22, 0x4000, "IntroNisBL15");
    INIT_NIS_ENTRY(0x23, 0x8000, "IntroNisBL16");
    INIT_NIS_ENTRY(0x24, 0x80, "ArrestF02");
    INIT_NIS_ENTRY(0x25, 0x100, "ArrestF06");
    INIT_NIS_ENTRY(0x26, 0x200, "ArrestF07");
    INIT_NIS_ENTRY(0x27, 0x400, "ArrestF14");
    INIT_NIS_ENTRY(0x28, 0x4000, "ArrestF18");
    INIT_NIS_ENTRY(0x29, 0x800, "ArrestF23");
    INIT_NIS_ENTRY(0x2A, 1, "ArrestM01");
    INIT_NIS_ENTRY(0x2B, 2, "ArrestM04");
    INIT_NIS_ENTRY(0x2C, 4, "ArrestM06");
    INIT_NIS_ENTRY(0x2D, 0x8000, "ArrestM07");
    INIT_NIS_ENTRY(0x2E, 8, "ArrestM14");
    INIT_NIS_ENTRY(0x2F, 0x10, "ArrestM16");
    INIT_NIS_ENTRY(0x30, 0x20, "ArrestM19");
    INIT_NIS_ENTRY(0x31, 0x40, "ArrestM23");
    INIT_NIS_ENTRY(0x32, 0x80, "ArrestF02b");
    INIT_NIS_ENTRY(0x33, 0x100, "ArrestF06b");
    INIT_NIS_ENTRY(0x34, 0x200, "ArrestF07b");
    INIT_NIS_ENTRY(0x35, 0x400, "ArrestF14b");
    INIT_NIS_ENTRY(0x36, 0x4000, "ArrestF18b");
    INIT_NIS_ENTRY(0x37, 0x800, "ArrestF23b");
    INIT_NIS_ENTRY(0x38, 1, "ArrestM01b");
    INIT_NIS_ENTRY(0x39, 2, "ArrestM04b");
    INIT_NIS_ENTRY(0x3A, 4, "ArrestM06b");
    INIT_NIS_ENTRY(0x3B, 0x8000, "ArrestM07b");
    INIT_NIS_ENTRY(0x3C, 8, "ArrestM14b");
    INIT_NIS_ENTRY(0x3D, 0x10, "ArrestM16b");
    INIT_NIS_ENTRY(0x3E, 0x20, "ArrestM19b");
    INIT_NIS_ENTRY(0x3F, 0x40, "ArrestM23b");
    INIT_NIS_ENTRY(0x40, 0x20000, "EndingNis01");
    INIT_NIS_ENTRY(0x41, 0x40000, "EndingNis02");
    INIT_NIS_ENTRY(0x42, 0x80000, "EndingNis03");
    INIT_NIS_ENTRY(0x43, 0x100000, "EndingNis04");
}

SFXObj_NISStream::SFXObj_NISStream()
    : CARSFX() {
    m_bNISAnimationReady = false;
    m_bNISButtonThroughAnimationReady = false;
    m_bNISAudioStreamReady = false;
    m_bNISButtonThroughReady = false;
    m_bBackupStreamCleared = true;
    m_mselapsedtimecb = 0;
}

SFXObj_NISStream::~SFXObj_NISStream() {
    g_pEAXSound->SetSFXBaseObject(0, eMM_MAIN, 5, 0);
}

void SFXObj_NISStream::InitSFX() {
    SndBase::InitSFX();
    GenerateNISAnimHashMap();
    m_bNISAnimationReady = false;
    m_bNISButtonThroughReady = false;
    m_bNISButtonThroughAnimationReady = false;
    m_bNISAudioStreamReady = false;
    m_bBackupStreamCleared = true;
    (void)g_pEAXSound->GetSndGameMode();
    g_pEAXSound->SetSFXBaseObject(this, eMM_MAIN, 5, 0);
}

void SFXObj_NISStream::Destroy() {}

bool SFXObj_NISStream::QueueNISStream(unsigned int anim_id, int camera_track_number,
                                      void (*setmstimecb)(unsigned int, int), bool bbuttonthrough) {
    m_mselapsedtimecb = setmstimecb;
    m_mstimeelapsed = 0;
    return QueueNISStream(anim_id, camera_track_number, bbuttonthrough, true);
}

bool SFXObj_NISStream::QueueNISStream(unsigned int anim_id, int camera_track_number, bool bbuttonthrough,
                                      bool btracktime) {
    Speech::Module *nismgr;
    Type_NIS_Track csiscamtrack;
    int CSISindex = 0;
    int id = 0;
    SFXObj_Pathfinder *ppf = nullptr;

    m_bNISAnimationReady = false;
    m_animid = anim_id;

    if (!btracktime) {
        m_mselapsedtimecb = 0;
    }

    nismgr = Speech::Manager::GetSpeechModule(0);
    m_bIsButtonThrough = bbuttonthrough;
    if (bbuttonthrough == true || nismgr->GetStreamChannel()->IsPlaying()) {
        nismgr = Speech::Manager::GetSpeechModule(0);
        nismgr->PurgeSpeech();
    }

    csiscamtrack = Type_NIS_Track_Track00;
    switch (camera_track_number) {
    case 0:
        csiscamtrack = Type_NIS_Track_Track00;
        break;
    case 1:
        csiscamtrack = Type_NIS_Track_Track01;
        break;
    case 2:
        csiscamtrack = Type_NIS_Track_Track02;
        break;
    case 3:
        csiscamtrack = Type_NIS_Track_Track03;
        break;
    case 4:
        csiscamtrack = Type_NIS_Track_Track04;
        break;
    case 5:
        csiscamtrack = Type_NIS_Track_Track05;
        break;
    case 6:
        csiscamtrack = Type_NIS_Track_Track06;
        break;
    case 7:
        csiscamtrack = Type_NIS_Track_Track07;
        break;
    case 8:
        csiscamtrack = Type_NIS_Track_Track08;
        break;
    case 9:
        csiscamtrack = Type_NIS_Track_Track09;
        break;
    case 10:
        csiscamtrack = Type_NIS_Track_Track10;
        break;
    case 11:
        csiscamtrack = Type_NIS_Track_Track11;
        break;
    case 12:
        csiscamtrack = Type_NIS_Track_Track12;
        break;
    case 13:
        csiscamtrack = Type_NIS_Track_Track13;
        break;
    default:
        break;
    }

    CSISindex = GetCsisEventIndex(anim_id);
    if (CSISindex == -1) {
        m_bNISAudioStreamReady = true;
        SetSoundControlState(false, SNDSTATE_NIS_STORY, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_INTRO, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_321, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_BLK, "Clear NIS");
        SetSoundControlState(false, SNDSTATE_NIS_ARREST, "Clear NIS");
        return false;
    }

    if (CSISindex != 0) {
        csiscamtrack = Type_NIS_Track_Track00;
    }

    ppf = static_cast<SFXObj_Pathfinder *>(g_pEAXSound->GetSFXBase_Object(0x40010010));
    if (CSISindex < 0x12) {
        g_bWasLastNISaStart = true;
        g_laststartanimid = anim_id;
        if (bbuttonthrough == true) {
            SetSoundControlState(false, SNDSTATE_NIS_321, "NIS 321");

            {
                Csis::System::Result result;
                NIS_Select_StartStruct data;

                data.NIS_Scene_Start = static_cast<Type_NIS_Scene_Start>(uNIS_STRINGHASHMAP[CSISindex].Flags);
                data.NIS_Track = csiscamtrack;
                data.NIS_Section = Type_NIS_Section_End;
                result = Csis::Function::Call(&Csis::gNIS_Select_StartHandle, &data);
                if (result < Csis::System::kResult_Ok) {
                    Csis::gNIS_Select_StartHandle.Set(&Csis::NIS_Select_StartId);
                    Csis::Function::Call(&Csis::gNIS_Select_StartHandle, &data);
                }
            }

            id = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);
            SetDMIX_Input(6, 0);
            if (ppf) {
                ppf->SetNISPlaying(false);
            }
        } else {
            {
                Csis::System::Result result;
                NIS_Select_StartStruct data;

                data.NIS_Scene_Start = static_cast<Type_NIS_Scene_Start>(uNIS_STRINGHASHMAP[CSISindex].Flags);
                data.NIS_Track = csiscamtrack;
                data.NIS_Section = Type_NIS_Section_Complete;
                result = Csis::Function::Call(&Csis::gNIS_Select_StartHandle, &data);
                if (result < Csis::System::kResult_Ok) {
                    Csis::gNIS_Select_StartHandle.Set(&Csis::NIS_Select_StartId);
                    Csis::Function::Call(&Csis::gNIS_Select_StartHandle, &data);
                }
            }

            id = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);
            SetDMIX_Input(6, 0x7FFF);
            if (ppf) {
                ppf->SetNISPlaying(true);
            }
        }
    } else if (CSISindex < 0x24) {
        if (CSISindex == 0x18) {
            bbuttonthrough = true;
            CSISindex = 0x17;
        } else if (CSISindex == 0x1A) {
            bbuttonthrough = true;
            CSISindex = 0x19;
        }

        g_bWasLastNISaStart = true;
        g_laststartanimid = anim_id;
        if (bbuttonthrough == true) {
            SetSoundControlState(false, SNDSTATE_NIS_321, "NIS 321");

            {
                Csis::System::Result result;
                NIS_Select_BlacklistStruct data;

                data.NIS_Track = csiscamtrack;
                data.NIS_Section = Type_NIS_Section_End;
                data.NIS_Blacklist = static_cast<Type_NIS_Blacklist>(uNIS_STRINGHASHMAP[CSISindex].Flags);
                result = Csis::Function::Call(&Csis::gNIS_Select_BlacklistHandle, &data);
                if (result < Csis::System::kResult_Ok) {
                    Csis::gNIS_Select_BlacklistHandle.Set(&Csis::NIS_Select_BlacklistId);
                    Csis::Function::Call(&Csis::gNIS_Select_BlacklistHandle, &data);
                }
            }

            id = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);
            SetDMIX_Input(6, 0);
            if (ppf) {
                ppf->SetNISPlaying(false);
            }
        } else {
            {
                Csis::System::Result result;
                NIS_Select_BlacklistStruct data;

                data.NIS_Track = csiscamtrack;
                data.NIS_Section = Type_NIS_Section_Complete;
                data.NIS_Blacklist = static_cast<Type_NIS_Blacklist>(uNIS_STRINGHASHMAP[CSISindex].Flags);
                result = Csis::Function::Call(&Csis::gNIS_Select_BlacklistHandle, &data);
                if (result < Csis::System::kResult_Ok) {
                    Csis::gNIS_Select_BlacklistHandle.Set(&Csis::NIS_Select_BlacklistId);
                    Csis::Function::Call(&Csis::gNIS_Select_BlacklistHandle, &data);
                }
            }

            id = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_RACE_START, &SFXObj_NISStream::PlayNISStream, false);
            SetDMIX_Input(6, 0x7FFF);
            if (ppf) {
                ppf->SetNISPlaying(true);
            }
        }
    } else {
        g_bWasLastNISaStart = false;

        {
            Csis::System::Result result;
            NIS_Select_EndStruct data;

            data.NIS_Scene_End = static_cast<Type_NIS_Scene_End>(uNIS_STRINGHASHMAP[CSISindex].Flags);
            data.NIS_Track = Type_NIS_Track_Track00;
            data.NIS_Section = Type_NIS_Section_Complete;
            result = Csis::Function::Call(&Csis::gNIS_Select_EndHandle, &data);
            if (result < Csis::System::kResult_Ok) {
                Csis::gNIS_Select_EndHandle.Set(&Csis::NIS_Select_EndId);
                Csis::Function::Call(&Csis::gNIS_Select_EndHandle, &data);
            }
        }

        id = Speech::Manager::GetSpeechModule(0)->QueStream(STRM_NIS_BUSTED, &SFXObj_NISStream::PlayNISStream, false);
        SetDMIX_Input(7, 0x7FFF);
        if (ppf) {
            ppf->SetNISPlaying(true);
        }
    }

    if (id == 0) {
        m_bNISAudioStreamReady = true;
    }

    return id;
}

void SFXObj_NISStream::StopStream() {
    Speech::Manager::GetSpeechModule(0)->PurgeSpeech();
}

void SFXObj_NISStream::NISActivityDone() {
    int id;
    SFXObj_Pathfinder *ppf;

    SetDMIX_Input(6, 0);
    SetDMIX_Input(7, 0);
    m_mselapsedtimecb = 0;
    m_mstimeelapsed = -1;
    m_mslengthofstream = -1;

    ppf = static_cast<SFXObj_Pathfinder *>(g_pEAXSound->GetSFXBase_Object(0x40010010));
    if (ppf) {
        ppf->SetNISPlaying(false);
    }
}

void SFXObj_NISStream::StartNIS() {
    {
        bool bresult = m_bNISAudioStreamReady;

        m_bNISAnimationReady = true;
        if (bresult) {
            Speech::Manager::GetSpeechModule(0)->PlayStream(2);
            SNDSYS_service();
            m_bNISAudioStreamReady = false;
        }
    }
}

void SFXObj_NISStream::UpdateParams(float t) {
    if (!m_bNISAnimationReady || !m_bNISAudioStreamReady) {
        if (!m_bNISButtonThroughAnimationReady || !m_bNISButtonThroughReady) {
            if (m_mselapsedtimecb) {
                Speech::Module *nismgr = Speech::Manager::GetSpeechModule(0);
                EAXS_StreamChannel *pch = nismgr->GetStreamChannel();

                if (m_mslengthofstream == 0) {
                    m_mslengthofstream = pch->GetTimeRemaining();
                }

                m_mstimeelapsed = pch->GetCurrentTime();
                m_mselapsedtimecb(m_animid, m_mstimeelapsed);
            }
        } else {
            bool bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(7);

            if (bresult) {
                m_bNISButtonThroughReady = false;
                m_bNISButtonThroughAnimationReady = false;
            }
        }
    } else {
        bool bresult = Speech::Manager::GetSpeechModule(0)->PlayStream(2);
        Speech::Module *nismgr = Speech::Manager::GetSpeechModule(0);
        EAXS_StreamChannel *pch = nismgr->GetStreamChannel();

        if (m_mslengthofstream == 0) {
            SNDSYS_service();
            m_mslengthofstream = pch->GetTimeRemaining();
        }

        if (bresult) {
            m_bNISAnimationReady = false;
            m_bNISAudioStreamReady = false;
            m_bBackupStreamCleared = false;
        }
    }
}

void SFXObj_NISStream::PlayNISStream() {
    Speech::Module *nismgr;
    EAXS_StreamChannel *pch;

    m_bNISAudioStreamReady = true;
    m_bNISButtonThroughReady = false;
    m_mstimeelapsed = 0;
    m_mslengthofstream = 0;
    nismgr = Speech::Manager::GetSpeechModule(0);
    pch = nismgr->GetStreamChannel();
    m_mslengthofstream = pch->GetTimeRemaining();

    if (m_bIsButtonThrough) {
        nismgr = Speech::Manager::GetSpeechModule(0);
        nismgr->PlayStream(2);
    }
}

#undef INIT_NIS_ENTRY
