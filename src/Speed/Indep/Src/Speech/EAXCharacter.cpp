#include "EAXCharacter.h"
#include "ScheduleSpeech.hpp"
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

template void Speech::Manager::ScheduleSpeech<Csis::AcknowledgeStruct>(
    Csis::AcknowledgeStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);
template void Speech::Manager::ScheduleSpeech<Csis::Interrupts_StaticInterruptStruct>(
    Csis::Interrupts_StaticInterruptStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);
template void Speech::Manager::ScheduleSpeech<Csis::Interrupts_InterruptRamStruct>(
    Csis::Interrupts_InterruptRamStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);
template void Speech::Manager::ScheduleSpeech<Csis::Interrupts_InterruptRamHighStruct>(
    Csis::Interrupts_InterruptRamHighStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);
template void Speech::Manager::ScheduleSpeech<Csis::Interrupts_InterruptStruct>(
    Csis::Interrupts_InterruptStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);
template void Speech::Manager::ScheduleSpeech<Csis::AnytimeEvents_DriverHistoryStruct>(
    Csis::AnytimeEvents_DriverHistoryStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);
template void Speech::Manager::ScheduleSpeech<Csis::AnytimeEvents_HeatJumpStruct>(
    Csis::AnytimeEvents_HeatJumpStruct &data,
    Csis::InterfaceId &iid,
    Csis::FunctionHandle &fh,
    EAXCharacter *actor);

extern void *NullPointer;

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
        SlotPool *pool = ai->GetActorPool();
        if (pool->NumAllocatedSlots != pool->TotalNumSlots) {
            p = pool->Malloc(1, 0);
        }
    }
    return p;
}

void EAXCharacter::operator delete(void *ptr) {
    SoundAI *ai = SoundAI::Get();
    if (ai) {
        SlotPool *pool = ai->GetActorPool();
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
    ScheduleSpeech(ack, Csis::AcknowledgeId, Csis::gAcknowledgeHandle, this);
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
    ScheduleSpeech(ack, Csis::AcknowledgeId, Csis::gAcknowledgeHandle, this);
}

void EAXCharacter::InterruptStatic() {
    Csis::Interrupts_StaticInterruptStruct data;
    ScheduleSpeech(data, Csis::Interrupts_StaticInterruptId, Csis::gInterrupts_StaticInterruptHandle, this);
}

void EAXCharacter::InterruptExpletive() {
    Csis::Interrupts_InterruptRamStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Interrupts_InterruptRamId, Csis::gInterrupts_InterruptRamHandle, this);
}

void EAXCharacter::InterruptViolent() {
    Csis::Interrupts_InterruptRamHighStruct data;
    data.speaker_id = mSpeakerID;
    ScheduleSpeech(data, Csis::Interrupts_InterruptRamHighId, Csis::gInterrupts_InterruptRamHighHandle, this);
}

void EAXCharacter::InterruptComposedLow() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = 1;
    ScheduleSpeech(data, Csis::Interrupts_InterruptId, Csis::gInterrupts_InterruptHandle, this);
}

void EAXCharacter::InterruptComposedHigh() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = mSpeakerID;
    data.intensity = 2;
    ScheduleSpeech(data, Csis::Interrupts_InterruptId, Csis::gInterrupts_InterruptHandle, this);
}

void EAXCharacter::DriverHistory() {
    Csis::AnytimeEvents_DriverHistoryStruct data;
    data.speaker_id = mSpeakerID;
    data.region = 2;
    ScheduleSpeech(data, Csis::AnytimeEvents_DriverHistoryId, Csis::gAnytimeEvents_DriverHistoryHandle, this);
}

void EAXCharacter::HeatJump(Csis::Type_heat_level heat) {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_HeatJumpStruct data;

    if (ai) {
        data.speaker_id = mSpeakerID;
        data.heat_level = heat;
        ScheduleSpeech(data, Csis::AnytimeEvents_HeatJumpId, Csis::gAnytimeEvents_HeatJumpHandle, this);
    }
}

void EAXCharacter::Update() {
    ISimable *simable = ISimable::FindInstance(GetHandle());
    IVehicle *vehicle = 0;
    SoundAI *ai;

    if (simable) {
        simable->QueryInterface(&vehicle);
    } else {
        *reinterpret_cast<unsigned int *>(&mActive) = 0;
        mHandle = 0;
    }
    ai = SoundAI::Get();
    if (ai && vehicle && *reinterpret_cast<unsigned int *>(&mActive)) {
        UMath::Vector3 pPos;
        UMath::Vector3 cPos;

        mPos = vehicle->GetPosition();
        pPos = ai->GetPlayerPos();
        cPos = mPos;
        mSpeed = MPS2MPH(vehicle->GetAbsoluteSpeed());
        mDistance = UMath::Distance(cPos, pPos);
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
