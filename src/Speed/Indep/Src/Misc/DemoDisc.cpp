#include "Speed/Indep/Src/Misc/DemoDisc.hpp"

// El orden de la .bss del original lo pone aqui: va justo antes de los
// estaticos de Rumble.cpp, y DemoDisc.cpp se incluye justo antes que Rumble.cpp.
static Timer last_any_joy(0);

// SuspendPlayTimer y ResumePlayTimer existieron (el DWARF los da con rango
// 0xFFFFFFFF y cuerpos que usan Timer), pero el enlazador los quito y su
// contenido no se puede leer: no se escriben.
