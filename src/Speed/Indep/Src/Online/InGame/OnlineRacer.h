#ifndef ONLINE_INGAME_ONLINERACER_H
#define ONLINE_INGAME_ONLINERACER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// La capa online esta anulada en la version de GameCube: el volcado DWARF da
// `total size: 0x1` para esta clase, sin un solo miembro, y sus accesores
// devuelven constantes.

// total size: 0x1
struct OnlineRacer {
    char *GetPersona() {
        return NULL;
    }
};

#endif
