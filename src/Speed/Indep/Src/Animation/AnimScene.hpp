#ifndef ANIMATION_ANIMSCENE_H
#define ANIMATION_ANIMSCENE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AnimCtrl.hpp"
#include "AnimEntity.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEAnimScene.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

enum eAnimProperty {
    eAnimProp_ControlRaceCountdown = 0,
    eAnimProp_ControlRaceCars = 1,
    eAnimProp_UnBindRaceCars = 2,
    eAnimProp_MaxAnimProperty = 3,
};

// total size: 0x14
class CAnimEntityData : public bTNode<CAnimEntityData> {
  private:
    int mType;   // offset 0x8, size 0x4
    void *mData; // offset 0xC, size 0x4
    int mSize;   // offset 0x10, size 0x4
};

// total size: 0x1C
class CAnimSceneData : public bTNode<CAnimSceneData> {
  public:
    CAnimSceneData(bChunk *chunk);
    ~CAnimSceneData();

    static CAnimSceneData *FindAnimSceneData(unsigned int scene_name_hash);

    void EndianSwapHeaderData();
    void InitHeaderData(void *data, int size);
    void AddEntityData(void *data, int size);

    int GetSceneInfo();

  private:
    bChunk *mChunk;                              // offset 0x8, size 0x4
    struct NisScene *mNisScene;                  // offset 0xC, size 0x4
    bTList<CAnimEntityData> mAnimEntityDataList; // offset 0x10, size 0x8
};

CAnimSceneData *CreateAnimSceneData(bChunk *nested_chunk, bChunk *sub_chunk);
int LoaderAnimSceneData(bChunk *chunk);
int UnloaderAnimSceneData(bChunk *chunk);

// total size: 0x14
class CAnimProperty : public bTNode<CAnimProperty> {
  public:
    CAnimProperty(eAnimProperty type, bool enabled);
    virtual ~CAnimProperty();
    eAnimProperty GetType();
    void SetEnabled(bool enabled);
    bool IsEnabled();

  private:
    eAnimProperty mType; // offset 0x8, size 0x4
    int mEnabled;        // offset 0xC, size 0x4
};

// total size: 0x110
class CAnimScene : public ICEScene, public bTNode<CAnimScene> {
  public:
    enum ePlayStatus {
        Stopped = 0,
        Paused = 1,
        Playing = 2,
        MaxPlayStatus = 3,
    };

    CAnimScene(CAnimSceneData *anim_scene_data, int camera_track_number, int anim_candidate_type, int anim_candidate_index);

    int GenerateHandle();
    int GetHandle();

    unsigned int GetAnimID();

    int GetSceneType();

    void GetSceneName(char *ret_name);

    const char *GetAnimDescription();

    bool SetPropertyEnabled(eAnimProperty property_id, bool enable);

    bool IsPropertyEnabled(eAnimProperty property_id);

    bool IsBoundToGame();

    bool BindToGame();

    bool UnBindToGame();

    void ChangePlayStatus(ePlayStatus new_status);

    bool Cue();

    bool Play();

    bool Stop();

    bool IsCued();

    bool IsStopped();

    bool IsPaused();

    void ResetTime();

    void JumpToEnd();

    void SetTime(float time);

    void GetTime(float &time);

    void UpdateTime(float time_step);

    void RenderEffects(eView *view, int is_reflection);

    void AddProperty(eAnimProperty property_id, bool enabled);

    void RemoveProperties();

    CAnimProperty *FindProperty(eAnimProperty property_id);

    bool Init();

    bool Purge();

    void ForceCarToAnimCarPosition(struct Car *car, int car_num);

    void ForcePlayerToAnimCarPosition(int player_num, int car_num);

    void ClearCarAnimStates();

    void InitCarAnimStatesFromStartingPositions();

    void InitCarAnimStatesFromNIS();

    int FindCurrentWorldCarIndex(struct Car *car);

    void SetCarAnimationPositions();

    void CreateCarAnimationControllers();

    void ClearCarAnimationControllers();

    void AnimatedCars_SetMainAndWheels(int current_car, CAnimCtrl *main_anim_ctrl, float time_step);

    void AnimatedCars_ResetToBeginning();

    void AnimatedCars_ClearLastPose();

    void AnimatedCars_SetTime(float time);

    void AnimatedCars_Update(float time_step);

    void AnimatedCars_Bind();

    void AnimatedCars_UnBind();

    IAnimEntity *GetAnimEntityWithModelName(const char *name);

    void CreateAnimEntities();

    void ClearAnimEntities();

    // Virtual functions
    // ICEScene
    // TODO are all of these really overridden?
    virtual ~CAnimScene();
    unsigned int GetSceneHash() override;
    int GetCameraTrackNumber() override;
    void SetTime() override;
    bool Pause() override;
    bool UnPause() override;
    bool IsPlaying() override;
    float GetTimeElapsed() override;
    float GetTimeStart() override;
    float GetTimeTotalLength() override;
    bool IsControllingCamera() override;
    bool IsCameraFixingElevation() override;
    bMatrix4 &GetSceneRotationMatrix() override;
    bMatrix4 &GetSceneTransformMatrix() override;

  private:
    int mHandle;                                  // offset 0xC, size 0x4
    CAnimSceneData *mAnimSceneData;               // offset 0x10, size 0x4
    bPList<IAnimEntity> mInstancedAnimEntityList; // offset 0x14, size 0x8
    ePlayStatus mPlayStatus;                      // offset 0x1C, size 0x4
    float mTimeElapsed;                           // offset 0x20, size 0x4
    float mTimeDelta;                             // offset 0x24, size 0x4
    float mTimeStart;                             // offset 0x28, size 0x4
    float mTimeTotalLength;                       // offset 0x2C, size 0x4
    bTList<CAnimProperty> mAnimPropertyList;      // offset 0x30, size 0x8
    bool mIsBoundToGame;                          // offset 0x38, size 0x1
    int mCameraTrackNumber;                       // offset 0x3C, size 0x4
    bool mControllingCamera;                      // offset 0x40, size 0x1
    bMatrix4 mSceneRotationMatrix;                // offset 0x44, size 0x40
    bMatrix4 mSceneTranslationMatrix;             // offset 0x84, size 0x40
    bMatrix4 mSceneTransformMatrix;               // offset 0xC4, size 0x40
    SpaceNode *mSpaceNode;                        // offset 0x104, size 0x4
    int mAnimCandidateType;                       // offset 0x108, size 0x4
    int mAnimCandidateIndex;                      // offset 0x10C, size 0x4
};

// total size: 0x94
class CAnimMomentScene : public ICEScene {
  public:
    // Virtual functions
    virtual ~CAnimMomentScene();

    // ICEScene
    // TODO are all of these really overridden?
    unsigned int GetSceneHash() override;
    int GetCameraTrackNumber() override;
    bool IsControllingCamera() override;
    bool IsCameraFixingElevation() override;
    void SetTime() override;
    bool Pause() override;
    bool UnPause() override;
    bool IsPlaying() override;
    float GetTimeStart() override;
    float GetTimeTotalLength() override;
    float GetTimeElapsed() override;
    bMatrix4 &GetSceneRotationMatrix() override;
    bMatrix4 &GetSceneTransformMatrix() override;

  private:
    unsigned int mSceneHash;        // offset 0x4, size 0x4
    int mCamera_track_number;       // offset 0x8, size 0x4
    float mTimeElapsed;             // offset 0xC, size 0x4
    float mTotalTime;               // offset 0x10, size 0x4
    bMatrix4 mSceneRotationMatrix;  // offset 0x14, size 0x40
    bMatrix4 mSceneTransformMatrix; // offset 0x54, size 0x40
};

#endif
