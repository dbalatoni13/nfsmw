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

// total size: 0x40
class NisScene {
  public:
    static const int MaxSceneNameLength;

    unsigned int mSceneNameHash;   // offset 0x0, size 0x4
    char mSceneName[16];           // offset 0x4, size 0x10
    char *Description;             // offset 0x14, size 0x4
    int SceneType;                 // offset 0x18, size 0x4
    int HaveLayout;                // offset 0x1C, size 0x4
    int HaveCarAnimation;          // offset 0x20, size 0x4
    int NumberOfCars;              // offset 0x24, size 0x4
    int StartFrame;                // offset 0x28, size 0x4
    int VanishFrame;               // offset 0x2C, size 0x4
    char SeeulatorOverlayName[16]; // offset 0x30, size 0x10
};

// total size: 0x14
class CAnimEntityData : public bTNode<CAnimEntityData> {
  public:
    CAnimEntityData(unsigned int type, void *data, int size) : mType(type), mData(data), mSize(size) {}
    ~CAnimEntityData() {}
    int GetType() {
        return mType;
    }
    void *GetData() {
        return mData;
    }
    int GetSize() {
        return mSize;
    }

  private:
    int mType;   // offset 0x8, size 0x4
    void *mData; // offset 0xC, size 0x4
    int mSize;   // offset 0x10, size 0x4
};

// total size: 0x1C
class CAnimSceneData : public bTNode<CAnimSceneData> {
  public:
    bChunk *GetChunk() {
        return mChunk;
    }

    unsigned int GetAnimID() {
        return mNisScene->mSceneNameHash;
    }

    NisScene *GetSceneInfo() {
        return mNisScene;
    }

    bTList<CAnimEntityData> *GetAnimEntityDataList() {
        return &mAnimEntityDataList;
    }

    static CAnimSceneData *FindAnimSceneData(unsigned int anim_id);

    CAnimSceneData(bChunk *chunk);
    virtual ~CAnimSceneData();

    void EndianSwapHeaderData();
    void InitHeaderData(void *data, int size);
    void AddEntityData(void *data, int size);
    void RemoveAllEntityData();

  private:
    bChunk *mChunk;                              // offset 0x8, size 0x4
    NisScene *mNisScene;                         // offset 0xC, size 0x4
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

    bool Init();
    bool Purge();
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
    void ResetTime();
    void GetTime(float &time);
    void UpdateTime(float time_step);
    void JumpToEnd();
    void RenderEffects(eView *view, int is_reflection);
    bool Cue();
    bool Play();
    bool Stop();
    bool IsCued();
    bool IsStopped();
    bool IsPaused();
    IAnimEntity *GetAnimEntityWithModelName(const char *name);
    void AnimatedCars_Update(float time_step);

    bool IsPastThisTime(float time) {
        return mTimeElapsed > time;
    }
    bool IsFinished() {
        return mTimeElapsed >= mTimeTotalLength;
    }
    void SetCameraControl(bool enable) {
        mControllingCamera = enable;
    }
    bPList<IAnimEntity> *GetAnimEntityList() {
        return &mInstancedAnimEntityList;
    }
    void SetSceneRotationMatrix(bMatrix4 &scene_rotation_matrix) {
        mSceneRotationMatrix = scene_rotation_matrix;
    }
    void SetSceneTranslationMatrix(bMatrix4 &scene_translation_matrix) {
        mSceneTranslationMatrix = scene_translation_matrix;
    }
    void SetSceneTransformMatrix(bMatrix4 &scene_transform_matrix) {
        mSceneTransformMatrix = scene_transform_matrix;
    }
    const bMatrix4 &GetSceneTranslationMatrix() {
        return mSceneTranslationMatrix;
    }

    // Virtual overrides from ICEScene
    // TODO are all of these really overridden?
    virtual ~CAnimScene();
    unsigned int GetSceneHash() override;
    int GetCameraTrackNumber() override;
    void SetTime(float time) override;
    bool Pause() override;
    bool UnPause() override;
    bool IsPlaying() override;
    float GetTimeElapsed() override;
    float GetTimeStart() override;
    float GetTimeTotalLength() override;
    bool IsControllingCamera() override;
    bool IsCameraFixingElevation() override;
    const bMatrix4 &GetSceneRotationMatrix() override;
    const bMatrix4 &GetSceneTransformMatrix() override;

  private:
    static int mHandleCounter;

    int GenerateHandle();
    void ChangePlayStatus(ePlayStatus new_status);
    void AddProperty(eAnimProperty property_id, bool enabled);
    void RemoveProperties();
    CAnimProperty *FindProperty(eAnimProperty property_id);
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
    void AnimatedCars_Bind();
    void AnimatedCars_UnBind();
    void CreateAnimEntities();
    void ClearAnimEntities();

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
    CAnimMomentScene(unsigned int sceneHash, int camera_track_number, bMatrix4 &rot, bMatrix4 &transform)
        : mSceneHash(sceneHash),                     //
          mCamera_track_number(camera_track_number), //
          mTimeElapsed(0.0f),                        //
          mTotalTime(0.0f),                          //
          mSceneRotationMatrix(rot),                 //
          mSceneTransformMatrix(transform) {}

    virtual ~CAnimMomentScene();

    // TODO are all of these really overridden?
    unsigned int GetSceneHash() override {
        return mSceneHash;
    }
    int GetCameraTrackNumber() override {
        return mCamera_track_number;
    }
    bool IsControllingCamera() override {
        return true;
    }
    bool IsCameraFixingElevation() override {
        return false;
    }
    void SetTime(float time) override {
        mTimeElapsed = time;
    }
    bool Pause() override {
        return true;
    }
    bool UnPause() override {
        return true;
    }
    bool IsPlaying() override {
        return true;
    }
    void Update(float dT) {
        mTimeElapsed += dT;
    }
    bool IsFinished() {
        return mTimeElapsed >= mTotalTime;
    }
    float GetTimeStart() override {
        return 0.0f;
    }
    float GetTimeTotalLength() override {
        return mTotalTime;
    }
    float GetTimeElapsed() override {
        return mTimeElapsed;
    }
    const bMatrix4 &GetSceneRotationMatrix() override {
        return mSceneRotationMatrix;
    }
    const bMatrix4 &GetSceneTransformMatrix() override {
        return mSceneTransformMatrix;
    }

    unsigned int mSceneHash;        // offset 0x4, size 0x4
    int mCamera_track_number;       // offset 0x8, size 0x4
    float mTimeElapsed;             // offset 0xC, size 0x4
    float mTotalTime;               // offset 0x10, size 0x4
    bMatrix4 mSceneRotationMatrix;  // offset 0x14, size 0x40
    bMatrix4 mSceneTransformMatrix; // offset 0x54, size 0x40
};

#endif
