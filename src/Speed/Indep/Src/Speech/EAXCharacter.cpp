#include "Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp"
#include "Speed/Indep/Src/Speech/EAXCharacter.h"
#include "Speed/Indep/Src/Speech/SoundAI.h"

EAXCharacter::EAXCharacter(int sID, HSIMABLE wID, int bID, int cID)
    : mSpeakerID(sID),             //
      mHandle(wID),                //
      mCallsign(bID, cID),         //
      mPos(UMath::Vector3::kZero), //
      mSpeed(0.0f),                //
      mDistance(0.0f),             //
      mHealth(1.0f),               //
      mDestroyed(false),           //
      mActive(false),              //
      mSuspectLOS(false) {}

void *EAXCharacter::operator new(size_t obj_size) {
    SoundAI *ai = SoundAI::Get();
    if (ai == nullptr || ai->GetActorPool()->IsFull()) {
        return NullPointer;
    }
    return ai->GetActorPool()->Malloc(1, nullptr);
}

void EAXCharacter::operator delete(void *ptr) {
    SoundAI *ai = SoundAI::Get();
    if (ai != nullptr) {
        ai->GetActorPool()->Free(ptr);
    }
}

void EAXCharacter::Reset() {
    this->mActive = false;
    this->mDestroyed = false;
    this->mHealth = 0.0f;
    this->mDistance = 0.0f;
    this->mPos = UMath::Vector3::kZero;
    this->mSpeed = 0.0f;
    this->mSuspectLOS = false;
}

EAXCharacter::~EAXCharacter() {}

void EAXCharacter::Update() {
    ISimable *simable = ISimable::FindInstance(this->GetHandle());
    IVehicle *vehicle = nullptr;
    SoundAI *ai;

    if (simable != nullptr) {
        simable->QueryInterface(&vehicle);
    } else {
        this->mActive = false;
        this->mHandle = nullptr;
    }
    ai = SoundAI::Get();
    if (ai != nullptr && vehicle != nullptr && this->mActive) {
        UMath::Vector3 pPos;
        UMath::Vector3 cPos;

        this->mPos = vehicle->GetPosition();
        pPos = ai->GetPlayerPos();
        cPos = this->mPos;
        this->mSpeed = MPS2MPH(vehicle->GetAbsoluteSpeed());
        this->mDistance = UMath::Distance(cPos, pPos);
    }
}

void EAXCharacter::Ack() {
    SoundAI *ai = SoundAI::Get();
    Csis::AcknowledgeStruct ack;
    ack.speaker_id = this->mSpeakerID;
    ack.yes_no = this->mDestroyed ? Csis::Type_yes_no_No_False : Csis::Type_yes_no_Yes_True;
    ack.intensity = Csis::Type_intensity_Normal;
    Speech::Manager::ScheduleSpeech(ack, Csis::AcknowledgeId, Csis::gAcknowledgeHandle, this);
}

void EAXCharacter::Deny() {
    SoundAI *ai = SoundAI::Get();
    Csis::AcknowledgeStruct ack;

    ack.speaker_id = this->mSpeakerID;
    ack.yes_no = this->mDestroyed ? Csis::Type_yes_no_No_False : Csis::Type_yes_no_Yes_True;
    ack.intensity = ai->IsHighIntensity() ? Csis::Type_intensity_Normal : Csis::Type_intensity_High;
    Speech::Manager::ScheduleSpeech(ack, Csis::AcknowledgeId, Csis::gAcknowledgeHandle, this);
}

void EAXCharacter::InterruptStatic() {
    Csis::Interrupts_StaticInterruptStruct data;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_StaticInterruptId, Csis::gInterrupts_StaticInterruptHandle, this);
}

void EAXCharacter::InterruptExpletive() {
    Csis::Interrupts_InterruptRamStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptRamId, Csis::gInterrupts_InterruptRamHandle, this);
}

void EAXCharacter::InterruptViolent() {
    Csis::Interrupts_InterruptRamHighStruct data;
    data.speaker_id = this->mSpeakerID;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptRamHighId, Csis::gInterrupts_InterruptRamHighHandle, this);
}

void EAXCharacter::InterruptComposedLow() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = Csis::Type_intensity_Normal;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptId, Csis::gInterrupts_InterruptHandle, this);
}

void EAXCharacter::InterruptComposedHigh() {
    Csis::Interrupts_InterruptStruct data;
    data.speaker_id = this->mSpeakerID;
    data.intensity = Csis::Type_intensity_High;
    Speech::Manager::ScheduleSpeech(data, Csis::Interrupts_InterruptId, Csis::gInterrupts_InterruptHandle, this);
}

void EAXCharacter::DriverHistory() {
    Csis::AnytimeEvents_DriverHistoryStruct data;
    data.speaker_id = this->mSpeakerID;
    data.region = Csis::Type_region_Coastal;
    Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_DriverHistoryId, Csis::gAnytimeEvents_DriverHistoryHandle, this);
}

void EAXCharacter::HeatJump(Csis::Type_heat_level heat) {
    SoundAI *ai = SoundAI::Get();
    Csis::AnytimeEvents_HeatJumpStruct data;

    if (ai != nullptr) {
        data.speaker_id = this->mSpeakerID;
        data.heat_level = heat;
        Speech::Manager::ScheduleSpeech(data, Csis::AnytimeEvents_HeatJumpId, Csis::gAnytimeEvents_HeatJumpHandle, this);
    }
}
