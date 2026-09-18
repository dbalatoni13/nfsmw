#ifndef __FEHASH_UI_DEBUGCARCUSTOMIZE_HPP__
#define __FEHASH_UI_DEBUGCARCUSTOMIZE_HPP__

// Constantes de hash del frontend. El juego las pasa como numero, y la funcion
// que las produce esta en nuestro arbol: `FEHash` en FEng/FEList.cpp, la misma
// que `bStringHash` de bWare.
//
// Cada nombre de aqui REPRODUCE su hash, comprobado uno a uno. La lista de
// upstream trae 0 mas que no verifican con ninguna forma de su nombre y
// que por eso no estan: si el nombre no da el numero, no sabemos como se
// llamaba y ponerlo seria inventarlo.

#define __CARNAME__            0xd6d32016
#define __CIRCLE__             0x54a68051
#define __CONDUITMDITC_TT21I__ 0x5b9d88b9
#define __DISABLE_INPUTS__     0x13fd3296
#define __ENABLE_INPUTS__      0x8cb81f09
#define __HELP_LABEL_1__       0x243f4b37
#define __HELP_LABEL_2__       0x243f4b38
#define __HELP_LABEL_3__       0x243f4b39
#define __HELP_LABEL_4__       0x243f4b3a
#define __HELP_LABEL_5__       0x243f4b3b
#define __HELP_LABEL_6__       0x243f4b3c
#define __HIDE_GROUP__         0x38d47305
#define __L2__                 0x5bd
#define __MODELHASH__          0x6a81554
#define __MODEL_NAME__         0xdd51c8b0
#define __OPT_1__              0x36db742
#define __OPT_2__              0x36db743
#define __OPT_5__              0x36db746
#define __PARTIDNAME__         0xeffe7224
#define __PARTNAME__           0xb1027477
#define __PART_ID__            0xbf8bff42
#define __PART_NAME__          0xd28b9316
#define __R3__                 0x684
#define __SCROLL_MASTER__      0x48f3d179
#define __SCROLL_SLAVE__       0x960d1728
#define __VAL_1__              0x3e40712
#define __VAL_2__              0x3e40713
#define __VAL_3__              0x3e40714

#endif
