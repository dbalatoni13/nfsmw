#include "Speed/Indep/Src/Frontend/HUD/FeAutoSaveIcon.hpp"

// Solo PS2. Leido en orig/SLES-53558-A124/NFS.ELF (0x00288128, 0xC B). Faltan el
// constructor (0xAC B) y Update (0xEC B): los cuerpos vacios de upstream/dev no
// son estas funciones.
void AutoSaveIcon::RequestAutoSaveIcon() {
    mIconRequested = true;
}
