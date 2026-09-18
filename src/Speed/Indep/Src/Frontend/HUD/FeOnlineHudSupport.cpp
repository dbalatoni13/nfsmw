#include "Speed/Indep/Src/Frontend/HUD/FeOnlineHudSupport.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

// Solo PS2. Leido en orig/SLES-53558-A124/NFS.ELF (0x002855d0, 0x44 B). Faltan el
// constructor (0x4C B) y Update (0x1B4 B): los cuerpos vacios de upstream/dev no
// son estas funciones.
void OnlineHUDSupport::DisplayGenericMessage(const char *message) {
    mIGenericMessage->RequestGenericMessage(message, false, FEHASH_FLASHER2, 0, 0, GenericMessage_Priority_1);
}
