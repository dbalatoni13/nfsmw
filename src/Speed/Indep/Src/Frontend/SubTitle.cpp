#include "SubTitle.hpp"

#include <types.h>

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct FEString;
struct FEObject;

int GetCurrentLanguage();
void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
void FEngSetLanguageHash(FEString *text, unsigned int hash);
int FEPrintf(FEString *text, const char *fmt, ...);
FEString *FEngFindString(const char *pkg_name, int name_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
void FEngGetTopLeft(FEObject *object, float& x, float& y);
void FEngSetTopLeft(FEObject *object, float x, float y);
unsigned int FEngHashString(const char *str, ...);
bool DoesStringExist(unsigned int hash);
const char *GetLocalizedString(unsigned int hash);

SubTitler *SubTitler::gCurrentSubtitler_;

SubTitler::SubTitler() {
    next_ = 0;
    float zero = 0.0f;
    data_ = nullptr;
    str_ = nullptr;
    str2_ = nullptr;
    back_ = nullptr;
    gCurrentSubtitler_ = this;
    timeElapsed = zero;
    mSubtitlePaused = false;
    lastTime = 0;
}

SubTitler::~SubTitler() {
    Unload();
    gCurrentSubtitler_ = nullptr;
}

bool SubTitler::ShouldShowSubTitles(const char *movie_name) {
    int lang = GetCurrentLanguage();
    if (lang != 0 || mIsTutorial) {
        return true;
    }
    return false;
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
        bSNPrintf(filename, 0x40, "SUBTITLES\%s.sub", movieName);
        int size;
        data_ = static_cast<SubtitleInfo*>(bGetFile(filename, &size, 0));
        if (data_ != nullptr) {
            lastTime = 0;
            timeElapsed = 0.0f;
            next_ = 0;
            for (int i = 0; data_[i].startTime != static_cast<unsigned short>(0xFFFF); i++) {
                bEndianSwap16(&data_[i].startTime);
                bEndianSwap32(&data_[i].stringHash);
            }
            str_ = FEngFindString(packageName, 0x599B8442);
            str2_ = FEngFindString(packageName, 0x2E8DA933);
            back_ = FEngFindObject(packageName, 0x8BD49BCC);
            FEPrintf(str_, "");
            unsigned int hideHash = FEngHashString("HIDE");
            FEngSetScript(reinterpret_cast<FEObject*>(str_), hideHash, true);
            FEPrintf(str2_, "");
            hideHash = FEngHashString("HIDE");
            FEngSetScript(reinterpret_cast<FEObject*>(str2_), hideHash, true);
        }
    }
}

void SubTitler::Unload() {
    if (data_ != nullptr) {
        bFree(data_);
        data_ = nullptr;
    }
}

float SubTitler::GetElapsedTime() {
    unsigned int timenow;
    float thetime_ms;
    if (mSubtitlePaused == false) {
        timenow = bGetTicker();
        float diff = bGetTickerDifference(lastTime, timenow);
        lastTime = timenow;
        thetime_ms = timeElapsed + diff * 0.001f;
        timeElapsed = thetime_ms;
    } else {
        timenow = bGetTicker();
        lastTime = timenow;
        thetime_ms = timeElapsed;
    }
    return thetime_ms;
}

void SubTitler::Update(unsigned int msg) {
    if (gMoviePlayer != nullptr) {
        mSubtitlePaused = gMoviePlayer->IsMoviePaused();
        if (msg == 0xC9960EBA) {
            if (data_ != nullptr && lastTime != 0) {
                float timenow = GetElapsedTime();
                if (IsMovieTimerPrintf != 0) {
                    Timer timer;
                    char timer_str[100];
                    timer.SetTime(timenow);
                    timer.PrintToString(timer_str, 0);
                }
                unsigned short delta = static_cast<unsigned short>(
                    static_cast<int>(timenow * 1000.0f));
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
    if (mIsTutorial == false) {
        if (data_[next_].stringHash != 0x1A20BA) {
            const char *str = GetLocalizedString(data_[next_].stringHash);
            if (bStrCmp("", str) != 0) {
                FEngSetLanguageHash(str_, data_[next_].stringHash);
                float x1, y1;
                FEngGetTopLeft(reinterpret_cast<FEObject*>(str_), x1, y1);
                float x2, dummy;
                FEngGetTopLeft(back_, x2, dummy);
                FEngSetTopLeft(back_, x2, y1);
                return;
            }
        }
        float x3, dummy2;
        FEngGetTopLeft(back_, x3, dummy2);
        FEngSetTopLeft(back_, x3, 1000.0f);
        FEPrintf(str_, "");
    } else if (data_[next_].stringHash == 0x1A20BA) {
        cFEng::Get()->QueuePackageMessage(0xDBDF2888, nullptr, nullptr);
    } else {
        FEngSetScript(reinterpret_cast<FEObject*>(str_), 0x16A259, true);
        FEngSetScript(reinterpret_cast<FEObject*>(str2_), 0x16A259, true);
        unsigned int text_hash = bStringHash("1_", data_[next_].stringHash);
        if (DoesStringExist(text_hash)) {
            FEngSetLanguageHash(str_, text_hash);
            FEngSetScript(reinterpret_cast<FEObject*>(str_), 0xBCBF0306, true);
        }
        text_hash = bStringHash("2_", data_[next_].stringHash);
        if (DoesStringExist(text_hash)) {
            FEngSetLanguageHash(str2_, text_hash);
            FEngSetScript(reinterpret_cast<FEObject*>(str2_), 0xBCBF0306, true);
        }
    }
}

void SubTitler::SetIsTutorialMovie(const char *movieName) {
    if (bStrCmp(movieName, "NIS_tutorial_1") == 0 ||
        bStrCmp(movieName, "NIS_tutorial_2") == 0 ||
        bStrCmp(movieName, "NIS_tutorial_3") == 0 ||
        bStrCmp(movieName, "NIS_tutorial_4") == 0 ||
        bStrCmp(movieName, "NIS_tutorial_5") == 0) {
        mIsTutorial = true;
    } else {
        mIsTutorial = false;
    }
}
