#ifndef __FEHASH_FEARRAYSCROLLER_HPP__
#define __FEHASH_FEARRAYSCROLLER_HPP__

// Constantes de hash del frontend. El juego las pasa como numero, y la funcion
// que las produce esta en nuestro arbol: `FEHash` en FEng/FEList.cpp, la misma
// que `bStringHash` de bWare.
//
// Cada nombre de aqui REPRODUCE su hash, comprobado uno a uno. La lista de
// upstream trae 16 mas que no verifican con ninguna forma de su nombre y
// que por eso no estan: si el nombre no da el numero, no sabemos como se
// llamaba y ponerlo seria inventarlo.

#define __APPEAR_CURSOR__      0x7e30c1b6
#define __BASEPOLY_12__        0x75c56f60
#define __BASESTRIPE_HEADER__  0xece41bd9
#define __BASESTRIPE_HELPBAR__ 0x8a352a6e
#define __CHECK_10__           0xacfa745d
#define __CHECK_11__           0xacfa745e
#define __CHECK_12__           0xacfa745f
#define __CHECK_1__            0xe636228d
#define __CHECK_2__            0xe636228e
#define __CHECK_3__            0xe636228f
#define __CHECK_4__            0xe6362290
#define __CHECK_5__            0xe6362291
#define __CHECK_6__            0xe6362292
#define __CHECK_7__            0xe6362293
#define __CHECK_8__            0xe6362294
#define __CHECK_9__            0xe6362295
#define __CURSOR_GROUP_123__   0x67d61a1e
#define __DESCRIPTION_BG__     0x8e27daeb
#define __EVENT_DESCRIPTION__  0x7b230d64
#define __EVENT_HEADER__       0x790ce49
#define __EVENT_ICON__         0xa018de49
#define __HIDE_HELP_GROUP__    0x267ec36d
#define __INNER_ARROW_DOWN__   0xdc442e3c
#define __INNER_ARROW_UP__     0x3c9e0b29
#define __OUTER_ARROW_DOWN__   0xfff86b8f
#define __OUTER_ARROW_UP__     0xf16cd7bc
#define __PC_STRIPE_BACK1__    0x6f77f88b
#define __POS13__              0x37f3935
#define __POS14__              0x37f3936
#define __POS15__              0x37f3937
#define __QUICK_RACE_MODE__    0x6471d6bc
#define __SHOW_CHECK__         0xedf59f5e
#define __TUT_GROUP__          0x389a5389

#endif
