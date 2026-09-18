#ifndef SCRIPTHASHES_HPP
#define SCRIPTHASHES_HPP

// Los mensajes de script del frontend. El juego los pasa como hash, y la
// funcion que los produce esta en nuestro arbol: `FEHash`, en
// FEng/FEList.cpp. Cada nombre de aqui REPRODUCE su hash -- comprobado uno
// a uno -- asi que no son etiquetas puestas a ojo.
//
// Seis se hashean con espacio y no con guion bajo ("SORT UP", "LEAVE ALL",
// "ENABLE INPUT", "NOT READY", "SORT DOWN", "BACK DELAY"); van marcados.
//
// La lista de upstream trae nueve mas que NO verifican con ninguna variante
// de su nombre (EXITCOMPLETE, ENTERCOMPLETE, INITCOMPLETE, HEADERTEXT,
// HEADERTEXT2, BACKBUTTON, SELECTBUTTON, LISTUP, LISTDOWN). Si el nombre no
// reproduce el hash no sabemos como se llamaba, asi que no estan.

#define FEHASH_15_IN              0x014035fb
#define FEHASH_15_OUT             0x2947101c
#define FEHASH_30_IN              0x0161a918
#define FEHASH_30_OUT             0x2d96e6d9
#define FEHASH_45_IN              0x01767f7e
#define FEHASH_45_OUT             0x304689ff
#define FEHASH_ACTIVATE           0x77031c70
#define FEHASH_ACTIVE             0x4f90cf9b
#define FEHASH_ACTIVE             0x4f90cf9b
#define FEHASH_AI_RACER           0x5f20a715
#define FEHASH_ALERT              0x026ded57
#define FEHASH_ANIMATE            0x5a8e4ebe
#define FEHASH_ANIMATETOTIME      0xb7001a70
#define FEHASH_ANIMATE_ADD        0x82a8b346
#define FEHASH_ANIMATE_LOSS       0xd7c5553e
#define FEHASH_ANIMATE_WON        0x82a91251
#define FEHASH_APPEAR             0x5079c8f8
#define FEHASH_APPEARINVISIBLE    0xd42290dd
#define FEHASH_BACK_DELAY         0xcb1aa27f  // "BACK DELAY"
#define FEHASH_BADCONNECTION      0x2d02fcd6
#define FEHASH_BK_UNLOCK          0x98257537
#define FEHASH_BLINK              0x0280164f
#define FEHASH_BLUE               0x00136707
#define FEHASH_BUTTON1            0xb8a7c6cc
#define FEHASH_BUTTON2            0xb8a7c6cd
#define FEHASH_BUTTON3            0xb8a7c6ce
#define FEHASH_CALL               0x0013c37b
#define FEHASH_CASTROL_UNLOCK     0x6521e5c2
#define FEHASH_CHECKED            0xe6361f46
#define FEHASH_CONTINUE           0xde02cf84
#define FEHASH_COPS               0x0013ff94
#define FEHASH_COP_REMOVED        0x579dbc92
#define FEHASH_CUSTOMIZE          0xa936c3a2
#define FEHASH_DECAL_COLOUR       0x9c11442b
#define FEHASH_DEMO_CHEAT         0xa6813b08
#define FEHASH_DIM                0x00009e99
#define FEHASH_DISABLE            0x36819d93
#define FEHASH_ENABLE             0x59a1f566
#define FEHASH_ENABLE_INPUT       0xc6341ff6  // "ENABLE INPUT"
#define FEHASH_FADEIN             0x5b0d9106
#define FEHASH_FADEOUT            0xbcbfcc87
#define FEHASH_FADEOUT_CENTER     0x4e6c8b47
#define FEHASH_FADEOUT_NORMAL     0x68cc526f
#define FEHASH_FADEOUT_WIDESCREEN 0x020f004f
#define FEHASH_FLASH              0x02c856ed
#define FEHASH_FLASHER1           0x9d73bc15
#define FEHASH_FLASHER2           0x9d73bc16
#define FEHASH_FLASHY             0x5bd334e6
#define FEHASH_FLIP_IN            0xd6c950a0
#define FEHASH_FLIP_OUT           0xaff37f61
#define FEHASH_FORWARD            0xde6eff34
#define FEHASH_GAME_ROOM          0x164d7635
#define FEHASH_GLOW               0x00162438
#define FEHASH_GO                 0x00000535
#define FEHASH_GOODCONNECTION     0xe8ee5b38
#define FEHASH_GREEN              0x02ddc8f0
#define FEHASH_GREEN_HIGHLIGHT    0x1835d907
#define FEHASH_GREY               0x00163c76
#define FEHASH_GREYED             0x5e973b1f
#define FEHASH_GREYEDOUT          0x8ba3b617
#define FEHASH_HIDE               0x0016a259
#define FEHASH_HIGHLIGHT          0x249db7b7
#define FEHASH_HIGHLIGHT2         0xb854aec9
#define FEHASH_ICON_PRESSED_1     0xe03a662d
#define FEHASH_ICON_PRESSED_2     0xe03a662e
#define FEHASH_ICON_PRESSED_3     0xe03a662f
#define FEHASH_ICON_PRESSED_4     0xe03a6630
#define FEHASH_ICON_PRESSED_5     0xe03a6631
#define FEHASH_IDLE               0x00171a7d
#define FEHASH_INACTIVE           0xfb12d252
#define FEHASH_INIT               0x001744b3
#define FEHASH_LEAVE              0x033113ac
#define FEHASH_LEAVE_ACCEPT       0x2e76edfb
#define FEHASH_LEAVE_ALL          0x97bdb465  // "LEAVE ALL"
#define FEHASH_LEAVE_BACK         0x93e8a57c
#define FEHASH_LEAVE_BACK         0x93e8a57c
#define FEHASH_LEFT               0x0018c32a
#define FEHASH_LEVEL_CHANGE       0x41e1fedc
#define FEHASH_LOADED             0x6a0831e8
#define FEHASH_LOCKED             0x6a096871
#define FEHASH_MENUFADEOUT        0x9b90037c
#define FEHASH_MOVE               0x00197c16
#define FEHASH_MOVE_NORMAL        0x47510b1e
#define FEHASH_MOVE_WIDESCREEN    0x908e787e
#define FEHASH_NORMAL             0x6ebbfb68
#define FEHASH_NOT_READY          0xf4e45f25  // "NOT READY"
#define FEHASH_NO_SKEW            0x472a1b35
#define FEHASH_OFF                0x0000ccfa
#define FEHASH_ON                 0x0000063c
#define FEHASH_ONE                0x0000ce01
#define FEHASH_OPTIONSICON        0x4ec2f614
#define FEHASH_PLAYER             0x7326e44c
#define FEHASH_PLAYER_HIGHLIGHT   0xec2dd763
#define FEHASH_PRESSED            0xe647b375
#define FEHASH_PULSE              0x03826a28
#define FEHASH_PULSEBLUE          0x75a0bdb0
#define FEHASH_PULSEGREEN         0x2a15f4b9
#define FEHASH_PULSERED           0xa6797f23
#define FEHASH_PULSEWHITE         0x2b3213e9
#define FEHASH_PULSEYELLOW        0x95ea1d84
#define FEHASH_PURSUIT_REP_ANIM   0x2ec08325
#define FEHASH_READY              0x039da474
#define FEHASH_RED                0x0000d99a
#define FEHASH_REDLINE            0x61d30442
#define FEHASH_REGCONNECTION      0x76d72a2d
#define FEHASH_REGULAR            0x620e4851
#define FEHASH_RENDER_ONLY        0x629becc0
#define FEHASH_RIGHT              0x039feffd
#define FEHASH_ROTATE             0x7811860e
#define FEHASH_ROTATE_IN          0x13f51124
#define FEHASH_ROTATE_OUT         0x92975065
#define FEHASH_SCALE              0x03aea527
#define FEHASH_SCALE_DOWN         0xc574b2be
#define FEHASH_SCALE_UP           0xe77f872b
#define FEHASH_SETPLAYERCOLOR     0x213d4fb7
#define FEHASH_SET_CURRENT        0xf9f5fcad
#define FEHASH_SHOW               0x001ca7c0
#define FEHASH_SKEW               0x001cb339
#define FEHASH_SORT_DOWN          0x172143bf  // "SORT DOWN"
#define FEHASH_SORT_UP            0xc721e3ec  // "SORT UP"
#define FEHASH_STATIC             0x7ab70d67
#define FEHASH_TEAM_TWO_COLOR     0x63f269dd
#define FEHASH_THREE              0x03c3c2f7
#define FEHASH_TIMEBONUS          0x609f6b15
#define FEHASH_TWO                0x0000e479
#define FEHASH_UNAVAILABLE        0x7f194ec3
#define FEHASH_UNCHECKED          0x77cdc4e9
#define FEHASH_UNDIM              0x03d8eabc
#define FEHASH_UNHIGHLIGHT        0x7ab5521a
#define FEHASH_UNKNOWN            0x5e40072f
#define FEHASH_UNLOCKED           0x28a165b4
#define FEHASH_WARNING            0xda600155
#define FEHASH_WHITE              0x03f9e820
#define FEHASH_WINGMAN_HIGHLIGHT  0x3be4d527
#define FEHASH_YELLOW             0x87ac789b
#define FEHASH_ZOOMIN             0x8ab83edb
#define FEHASH_ZOOMINBLUE         0x9847b3e3
#define FEHASH_ZOOMINGREEN        0xa19bb14c
#define FEHASH_ZOOMINORANGE       0xe7b913f7
#define FEHASH_ZOOMINRED          0x5230faf6
#define FEHASH_ZOOMINWHITE        0xa2b7d07c
#define FEHASH_ZOOMINYELLOW       0xfe276c77
#define FEHASH_ZOOMOUT            0xe1c034fc

#endif
