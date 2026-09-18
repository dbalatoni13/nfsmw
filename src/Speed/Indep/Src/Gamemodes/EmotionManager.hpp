#ifndef EMOTIONMANAGER_HPP
#define EMOTIONMANAGER_HPP

// EmotionalSummary -----------------------------------------------------

struct EmotionalSummary {
    enum eType {
        Embarrassed = 0,
        Anxious = 1,
        Smug = 2,
        Ecstatic = 3,
        MaxTypes = 4,
    };

    EmotionalSummary();
    ~EmotionalSummary();

    eType GetEmotion() const;
    float GetIntensity() const;

    eType mEmotion;                // 0x00
    eType mEmotionTarget;          // 0x04
    float mEmotionIntensity;       // 0x08
    float mEmotionIntensityTarget; // 0x0C
};

// EmotionalResponse ----------------------------------------------------

struct EmotionalResponse {
    enum ePhase {
        Created = 0,
        Initial = 1,
        RampUp = 2,
        Peak = 3,
        RampDown = 4,
        Final = 5,
        MaxPhases = 6,
    };

    enum eTrigger {
        PursuitBegins = 0,
        PursuitAddsCar = 1,
        PursuitAddsHeli = 2,
        PursuitAddsRoadblock = 3,
        PursuitEnds = 4,
        Arrested = 5,
        MaxTypes = 6,
    };

    EmotionalResponse(eTrigger trigger);
    ~EmotionalResponse();

    eTrigger GetTrigger() const;
    ePhase GetPhase() const;
    float GetIntensity() const;

    eTrigger mTrigger;    // 0x00
    ePhase mPhase;        // 0x04
    float mPeakIntensity; // 0x08
    float mIntensity;     // 0x0C
    float mAge;           // 0x10
};

extern const char *aEmotionalSummaryTypeStrings[EmotionalSummary::MaxTypes];
extern const char *aEmotionalResponseTriggerStrings[EmotionalResponse::MaxTypes];
extern const char *aEmotionalResponsePhaseStrings[EmotionalResponse::MaxPhases];

const char *GetEmotionalSummaryString(EmotionalSummary::eType type);
const char *GetEmotionalResponseString(EmotionalResponse::eTrigger trigger);
const char *GetEmotionalResponsePhaseString(EmotionalResponse::ePhase phase);

// EmotionManager -------------------------------------------------------

struct Car;

struct EmotionManager {
    enum eEmotionalContext {
        OverallPursuitDanger = 0,
        CarPursuitDanger = 1,
        HeliPursuitDanger = 2,
        RoadblockDanger = 3,
        RaceSuccess = 4,
        MaxContexts = 5,
    };

    EmotionManager();
    virtual ~EmotionManager();

    void Update(Car *car);
    const EmotionalSummary &GetSummary();
    float GetEmotionalEvaluation(eEmotionalContext context);
    void SendResponseEvent(EmotionalResponse::eTrigger trigger, float intensity);

    EmotionalSummary mSummary; // 0x00
};

#endif
