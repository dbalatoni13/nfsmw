#include "EAXCharacter.h"
#include "SoundAI.h"
#include "Speed/Indep/Src/EAXSound/Csis.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

namespace Csis {
struct AcknowledgeStruct {
    int speaker_id;
    int intensity;
    int yes_no;
};

struct Interrupts_StaticInterruptStruct {};

struct Interrupts_InterruptRamStruct {
    int speaker_id;
};

struct Interrupts_InterruptRamHighStruct {
    int speaker_id;
};

struct Interrupts_InterruptStruct {
    int speaker_id;
    int intensity;
};

struct AnytimeEvents_DriverHistoryStruct {
    int speaker_id;
    int region;
};

struct AnytimeEvents_HeatJumpStruct {
    int speaker_id;
    int heat_level;
};

extern InterfaceId AcknowledgeId;
extern InterfaceId Interrupts_StaticInterruptId;
extern InterfaceId Interrupts_InterruptRamId;
extern InterfaceId Interrupts_InterruptRamHighId;
extern InterfaceId Interrupts_InterruptId;
extern InterfaceId AnytimeEvents_DriverHistoryId;
extern InterfaceId AnytimeEvents_HeatJumpId;

extern FunctionHandle gAcknowledgeHandle;
extern FunctionHandle gInterrupts_StaticInterruptHandle;
extern FunctionHandle gInterrupts_InterruptRamHandle;
extern FunctionHandle gInterrupts_InterruptRamHighHandle;
extern FunctionHandle gInterrupts_InterruptHandle;
extern FunctionHandle gAnytimeEvents_DriverHistoryHandle;
extern FunctionHandle gAnytimeEvents_HeatJumpHandle;
}

extern void ScheduleSpeech_Acknowledge(Csis::AcknowledgeStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis17AcknowledgeStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_StaticInterrupt(Csis::Interrupts_StaticInterruptStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis32Interrupts_StaticInterruptStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_InterruptRam(Csis::Interrupts_InterruptRamStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis29Interrupts_InterruptRamStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_InterruptRamHigh(Csis::Interrupts_InterruptRamHighStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33Interrupts_InterruptRamHighStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_Interrupts_Interrupt(Csis::Interrupts_InterruptStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis26Interrupts_InterruptStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_DriverHistory(Csis::AnytimeEvents_DriverHistoryStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis33AnytimeEvents_DriverHistoryStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");
extern void ScheduleSpeech_AnytimeEvents_HeatJump(Csis::AnytimeEvents_HeatJumpStruct &data, Csis::InterfaceId &iid, Csis::FunctionHandle &fh, EAXCharacter *actor) asm("ScheduleSpeech__H1ZQ24Csis28AnytimeEvents_HeatJumpStruct_Q26Speech7ManagerRX01RQ24Csis11InterfaceIdRQ24Csis14FunctionHandleP12EAXCharacter_v");

extern void *NullPointer;
extern float lbl_80407430;

EAXCharacter::EAXCharacter(int sID, HSIMABLE wID, int bID, int cID)
    : mCallsign(bID, cID) {
    float fVar1 = UMath::Vector3::kZero.z;
    float fVar2 = UMath::Vector3::kZero.y;

    mSpeakerID = sID;
    mHandle = wID;
    mPos.x = UMath::Vector3::kZero.x;
    mPos.z = fVar1;
    mPos.y = fVar2;
    mDistance = 0.0f;
    mHealth = 1.0f;
    *reinterpret_cast<unsigned int *>(&mSuspectLOS) = 0;
    mSpeed = 0.0f;
    *reinterpret_cast<unsigned int *>(&mDestroyed) = 0;
    *reinterpret_cast<unsigned int *>(&mActive) = 0;
}

EAXCharacter::~EAXCharacter() {}

void *EAXCharacter::operator new(unsigned int) {
    SoundAI *ai = SoundAI::Get();
    void *p = NullPointer;
    if (ai) {
        SlotPool *pool = *reinterpret_cast<SlotPool **>(reinterpret_cast<char *>(ai) + 0x240);
        if (pool->NumAllocatedSlots != pool->TotalNumSlots) {
            p = pool->Malloc(1, 0);
        }
    }
    return p;
}

void EAXCharacter::operator delete(void *ptr) {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        SlotPool *pool = *reinterpret_cast<SlotPool **>(reinterpret_cast<char *>(ai) + 0x240);
        pool->Free(ptr);
    }
}

void EAXCharacter::Ack() {
    Csis::AcknowledgeStruct ack;
    ack.yes_no = 1;
    ack.speaker_id = mSpeakerID;
    if (*reinterpret_cast<unsigned int *>(&mDestroyed) != 0) {
        ack.yes_no = 2;
    }
    ack.intensity = 1;
    ScheduleSpeech_Acknowledge(ack, Csis::AcknowledgeId, Csis::gAcknowledgeHandle, this);
}

void EAXCharacter::Deny() {
    SoundAI *ai = SoundAI::Get();
    Csis::AcknowledgeStruct ack;
    int intensity = 1;

    ack.yes_no = 1;
    ack.speaker_id = mSpeakerID;
    if (*reinterpret_cast<unsigned int *>(&mDestroyed) != 0) {
        ack.yes_no = 2;
    }
    if (!ai->IsHighIntensity()) {
        intensity = 2;
    }
    ack.intensity = intensity;
    ScheduleSpeech_Acknowledge(ack, Csis::AcknowledgeId, Csis::gAcknowledgeHandle, this);
}

void EAXCharacter::InterruptStatic() {
    Csis::Interrupts_StaticInterruptStruct data;
    ScheduleSpeech_Interrupts_StaticInterrupt(data, Csis::Interrupts_StaticInterruptId, Csis::gInterrupts_StaticInterruptHandle, this);
}

void EAXCharacter::InterruptExpletive() {
    Csis::Interrupts_InterruptRamStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Interrupts_InterruptRam(data, Csis::Interrupts_InterruptRamId, Csis::gInterrupts_InterruptRamHandle, this);
}

void EAXCharacter::InterruptViolent() {
    Csis::Interrupts_InterruptRamHighStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech_Interrupts_InterruptRamHigh(data, Csis::Interrupts_InterruptRamHighId, Csis::gInterrupts_InterruptRamHighHandle, this);
}

void EAXCharacter::InterruptComposedLow() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = 1;
    ScheduleSpeech_Interrupts_Interrupt(data, Csis::Interrupts_InterruptId, Csis::gInterrupts_InterruptHandle, this);
}

void EAXCharacter::InterruptComposedHigh() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = 2;
    ScheduleSpeech_Interrupts_Interrupt(data, Csis::Interrupts_InterruptId, Csis::gInterrupts_InterruptHandle, this);
}

void EAXCharacter::DriverHistory() {
    Csis::AnytimeEvents_DriverHistoryStruct data;
    data.speaker_id = mSpeakerID;
    data.region = 2;
    ScheduleSpeech_AnytimeEvents_DriverHistory(data, Csis::AnytimeEvents_DriverHistoryId, Csis::gAnytimeEvents_DriverHistoryHandle, this);
}

void EAXCharacter::HeatJump(Csis::Type_heat_level heat) {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_HeatJumpStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.heat_level = heat;
        ScheduleSpeech_AnytimeEvents_HeatJump(data, Csis::AnytimeEvents_HeatJumpId, Csis::gAnytimeEvents_HeatJumpHandle, this);
    }
}

void EAXCharacter::Update() {
    ISimable *simable = ISimable::FindInstance(GetHandle());
    IVehicle *vehicle = 0;
    SoundAI *ai;

    if (!simable) {
        *reinterpret_cast<unsigned int *>(&mActive) = 0;
        mHandle = 0;
    } else {
        simable->QueryInterface(&vehicle);
    }
    ai = SoundAI::Get();
    if (ai && vehicle && *reinterpret_cast<unsigned int *>(&mActive)) {
        UMath::Vector3 pPos;
        UMath::Vector3 cPos;
        UMath::Vector3 temp;
        const UMath::Vector3 &pos = vehicle->GetPosition();

        mPos.x = pos.x;
        mPos.y = pos.y;
        mPos.z = pos.z;
        pPos = *reinterpret_cast<UMath::Vector3 *>(reinterpret_cast<char *>(ai) + 0x114);
        cPos = mPos;
        mSpeed = vehicle->GetAbsoluteSpeed() * lbl_80407430;
        VU0_v3sub(cPos, pPos, temp);
        mDistance = VU0_sqrt(VU0_v3lengthsquare(temp));
    }
}

void EAXCharacter::Reset() {
    float fVar = 0.0f;
    float fVar1 = UMath::Vector3::kZero.z;
    float fVar2 = UMath::Vector3::kZero.y;

    *reinterpret_cast<unsigned int *>(&mActive) = 0;
    *reinterpret_cast<unsigned int *>(&mDestroyed) = 0;
    mHealth = fVar;
    mDistance = fVar;
    mPos.x = UMath::Vector3::kZero.x;
    mPos.z = fVar1;
    mPos.y = fVar2;
    *reinterpret_cast<unsigned int *>(&mSuspectLOS) = 0;
    mSpeed = fVar;
}
