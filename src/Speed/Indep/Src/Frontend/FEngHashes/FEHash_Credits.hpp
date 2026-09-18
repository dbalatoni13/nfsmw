#ifndef __FEHASH_CREDITS_HPP__
#define __FEHASH_CREDITS_HPP__

// Constantes de hash del frontend. El juego las pasa como numero, y la funcion
// que las produce esta en nuestro arbol: `FEHash` en FEng/FEList.cpp, la misma
// que `bStringHash` de bWare.
//
// Cada nombre de aqui REPRODUCE su hash, comprobado uno a uno. La lista de
// upstream trae 0 mas que no verifican con ninguna forma de su nombre y
// que por eso no estan: si el nombre no da el numero, no sabemos como se
// llamaba y ponerlo seria inventarlo.

#define __BACK_SHADOW__            0x43319475
#define __CREDITSAREA__            0xe6093486
#define __CREDIT_AT_TOP__          0x29161540
#define __CREDIT_NEXT__            0xe6e946b8
#define __ENDGAME__                0x8e197c10
#define __HEADER_COLOR__           0x9735e5a6
#define __HELP_BAR_GROUP__         0xb04b66c8
#define __START_CREDITS__          0x8766253a
#define __START_END_GAME_CREDITS__ 0xf653f1e9
#define __UISND_OPTION_MENU_EXIT__ 0x4b680587

#endif
