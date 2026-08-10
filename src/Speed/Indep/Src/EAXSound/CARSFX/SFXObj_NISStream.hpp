#ifndef SFXOBJ_NISSTREAM_HPP
#define SFXOBJ_NISSTREAM_HPP

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"

// total size: 0x3C
// Decl: 7
class SFXObj_NISStream : public CARSFX {
  public:
    DECLARE_CREATABLE();
    SFXObj_NISStream();
    ~SFXObj_NISStream() override;

    // Overrides: SndBase
    int GetController(int Index) override {
        return -1;
    } // Decl: 16
    void AttachController(SFXCTL *psfxctl) override {} // Decl: 17
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override {} // Decl: 23

    void StartNIS();
    void NISActivityDone();
    bool IsNISStreamReady() {
        return this->m_bNISAudioStreamReady;
    } // Decl: 26
    static void PlayNISStream();
    static void PlayNISButtonThroughStream();
    void StartNISButtonThrough();

    void AnimationEnded(bool bunloaded);
    void StopStream();
    bool QueueNISStream(Attrib::Key anim_id, int camera_track_number, void (*setmstimecb)(uint32, int), bool bbuttonthrough);
    bool QueueNISStream(Attrib::Key anim_id, int camera_track_number, bool bbuttonthrough, bool btracktime);

    bool m_bNISAnimationReady;                     // offset 0x28, size 0x1, Decl: 27
    static bool m_bNISButtonThroughAnimationReady; // size: 0x1, address: 0x80418320
    static bool m_bNISAudioStreamReady;            // size: 0x1, address: 0x8041831C
    static bool m_bNISButtonThroughReady;          // size: 0x1, address: 0x80418324

    bool m_bBackupStreamCleared;  // offset 0x2C, size 0x1, Decl: 39
    bool m_bPlayingButtonThrough; // offset 0x30, size 0x1, Decl: 40

    static bool m_bIsButtonThrough; // size: 0x1, address: 0x80418328
    static int m_mstimeelapsed;     // size: 0x4, address: 0x8041832C
    static int m_mslengthofstream;  // size: 0x4, address: 0x80418330

    uint32 m_animid;                             // offset 0x34, size 0x4, Decl: 46
    void (*m_mselapsedtimecb)(Attrib::Key, int); // offset 0x38, size 0x4, Decl: 49
};

#endif
