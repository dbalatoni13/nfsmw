#include "uiOptionsScreen.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionWidgets.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EUnPause.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"

FEImage* FEngFindImage(const char* pkg_name, int name_hash);
void FEngSetTextureHash(FEImage* image, unsigned int texture_hash);
int FEngSNPrintf(char* buffer, int buf_size, const char* fmt, ...);
const char* GetLocalizedString(unsigned int hash);

extern EAXSound* g_pEAXSound;

enum eDialogTitle {};
enum eDialogFirstButtons {};

struct DialogInterface {
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, const char* fmt, ...);
};

inline void FEngSetTextureHash(const char* pkg_name, unsigned int obj_hash,
                               unsigned int texture_hash) {
    FEngSetTextureHash(FEngFindImage(pkg_name, obj_hash), texture_hash);
}
