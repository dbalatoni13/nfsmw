#include "SubTitle.hpp"
#include "MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int bSNPrintf(char *, int, const char *, ...);
extern void *bGetFile(const char *, int *, int);
extern void bFree(void *);
extern unsigned int bGetTicker();
extern float bGetTickerDifference(unsigned int, unsigned int);
extern bool IsMovieTimerPrintf;
extern FEString *FEngFindString(const char *, int);
extern FEObject *FEngFindObject(const char *, unsigned int);
extern int FEPrintf(FEString *, const char *, ...);
extern unsigned int FEngHashString(const char *, ...);
extern void FEngSetScript(FEObject *, unsigned int, bool);
extern void FEngSetLanguageHash(FEString *, unsigned int);
extern void FEngGetTopLeft(FEObject *, float &, float &);
extern void FEngSetTopLeft(FEObject *, float, float);
extern const char *GetLocalizedString(unsigned int);
extern int bStrCmp(const char *, const char *);
extern unsigned int bStringHash(const char *, int);
extern int DoesStringExist(unsigned int);

inline float FEngGetTopLeftX(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return x;
}

inline float FEngGetTopLeftY(FEObject *obj) {
    float x, y;
    FEngGetTopLeft(obj, x, y);
    return y;
}

inline void FEngSetTopLeftY(FEObject *obj, float y) {
    float x = FEngGetTopLeftX(obj);
    FEngSetTopLeft(obj, x, y);
}

SubTitler *SubTitler::gCurrentSubtitler_;

SubTitler::SubTitler() {
    next_ = 0;
    data_ = nullptr;
    str_ = nullptr;
    str2_ = nullptr;
    back_ = nullptr;
    gCurrentSubtitler_ = this;
    timeElapsed = 0.0f;
    lastTime = 0;
    mSubtitlePaused = false;
}

SubTitler::~SubTitler() {
    Unload();
    gCurrentSubtitler_ = nullptr;
}

bool SubTitler::ShouldShowSubTitles(const char *movie_name) {
    if (GetCurrentLanguage() != 0) {
        return true;
    }
    if (!mIsTutorial) {
        return false;
    }
    return true;
}

void SubTitler::BeginningMovie(const char *moviename, const char *packagename) {
    SetIsTutorialMovie(moviename);
    if (ShouldShowSubTitles(moviename)) {
        Load(moviename, packagename);
    }
}

void SubTitler::Load(const char *movieName, const char *packageName) {
    char filename[64];

    Unload();
    if (movieName != nullptr) {
        bSNPrintf(filename, 64, "SUBTITLES\\%s", movieName);
        data_ = static_cast<SubtitleInfo *>(bGetFile(filename, 0, 0));
        if (data_ != nullptr) {
            next_ = 0;
            timeElapsed = 0.0f;
            lastTime = 0;
            for (int i = 0; data_[i].startTime != 0xFFFF; i++) {
                bPlatEndianSwap(&data_[i].startTime);
                bPlatEndianSwap(&data_[i].stringHash);
            }
            str_ = FEngFindString(packageName, 0x599B8442);
            str2_ = FEngFindString(packageName, 0x2E8DA933);
            back_ = FEngFindObject(packageName, 0x8BD49BCC);
            FEPrintf(str_, "");
            FEngSetScript(str_, FEngHashString("SHOWSUBS"), true);
            FEPrintf(str2_, "");
            FEngSetScript(str2_, FEngHashString("SHOWSUBS"), true);
        }
    }
}

void SubTitler::Unload() {
    if (data_ != nullptr) {
        bFree(data_);
        data_ = nullptr;
    }
}

// NONMATCHING: regalloc - fmadds targets f1 directly instead of f0 + fmr
float SubTitler::GetElapsedTime() {
    unsigned int timenow;
    float thetime_ms;
    if (!mSubtitlePaused) {
        timenow = bGetTicker();
        thetime_ms = bGetTickerDifference(lastTime, timenow) * 0.001f + timeElapsed;
        lastTime = timenow;
        timeElapsed = thetime_ms;
    } else {
        lastTime = bGetTicker();
        thetime_ms = timeElapsed;
    }
    return thetime_ms;
}

void SubTitler::Update(unsigned int msg) {
    if (gMoviePlayer != nullptr) {
        int paused = gMoviePlayer->IsMoviePaused();
        if (paused)
            paused = 1;
        mSubtitlePaused = paused;
        if (msg == 0xC98356BA) {
            if (data_ != nullptr && lastTime != 0) {
                float timenow = GetElapsedTime();
                if (IsMovieTimerPrintf) {
                    Timer timer(static_cast<int>(timenow * 4000.0f + 0.5f));
                    char timer_str[100];
                    timer.PrintToString(timer_str, 0);
                }
                unsigned short delta = static_cast<unsigned short>(timenow * 10.0f);
                if (data_[next_].startTime <= delta) {
                    RefreshText();
                    next_++;
                }
            }
        } else if (msg == 0xC3960EB9) {
            Unload();
        }
    }
}

void SubTitler::Start() {
    lastTime = bGetTicker();
}

void SubTitler::NotifyFirstFrame() {
    if (gCurrentSubtitler_ != nullptr) {
        gCurrentSubtitler_->Start();
    }
}

void SubTitler::RefreshText() {
    if (!mIsTutorial) {
        if (data_[next_].stringHash != 0x1A20BA && bStrCmp("", GetLocalizedString(data_[next_].stringHash)) != 0) {
            FEngSetLanguageHash(str_, data_[next_].stringHash);
            FEngSetTopLeftY(back_, FEngGetTopLeftY(str_));
        } else {
            FEngSetTopLeftY(back_, 6000.0f);
            FEPrintf(str_, "");
        }
    } else {
        if (data_[next_].stringHash != 0x1A20BA) {
            FEngSetScript(str_, 0x16A259, true);
            FEngSetScript(str2_, 0x16A259, true);
            unsigned int text_hash = bStringHash("_A", data_[next_].stringHash);
            if (DoesStringExist(text_hash)) {
                FEngSetLanguageHash(str_, text_hash);
                FEngSetScript(str_, 0xBCBF0306, true);
            }
            text_hash = bStringHash("_B", data_[next_].stringHash);
            if (DoesStringExist(text_hash)) {
                FEngSetLanguageHash(str2_, text_hash);
                FEngSetScript(str2_, 0xBCBF0306, true);
            }
        } else {
            cFEng::Get()->QueuePackageMessage(0xDBDF2888, nullptr, nullptr);
        }
    }
}

void SubTitler::SetIsTutorialMovie(const char *movieName) {
    if (bStrCmp(movieName, "drag_tutorial") == 0 || bStrCmp(movieName, "speedtrap_tutorial") == 0 || bStrCmp(movieName, "tollbooth_tutorial") == 0 ||
        bStrCmp(movieName, "bounty_tutorial") == 0 || bStrCmp(movieName, "pursuit_tutorial") == 0) {
        mIsTutorial = true;
    } else {
        mIsTutorial = false;
    }
}
