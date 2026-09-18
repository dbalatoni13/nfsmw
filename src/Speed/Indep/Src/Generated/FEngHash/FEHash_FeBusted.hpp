#ifndef __FEHASH_FEBUSTED_HPP__
#define __FEHASH_FEBUSTED_HPP__

// Constantes de hash del frontend. El juego las pasa como numero, y la funcion
// que las produce esta en nuestro arbol: `FEHash` en FEng/FEList.cpp, la misma
// que `bStringHash` de bWare.
//
// Cada nombre de aqui REPRODUCE su hash, comprobado uno a uno. La lista de
// upstream trae 4 mas que no verifican con ninguna forma de su nombre y
// que por eso no estan: si el nombre no da el numero, no sabemos como se
// llamaba y ponerlo seria inventarlo.

#define __BACKING_GRP_1__               0x2f260aa6
#define __BAR_BACKING__                 0xa8f62e42
#define __BAR_LEFT_1__                  0xd38a10e
#define __BAR_MIDDLE_1__                0x583a0172
#define __BAR_RIGHT_1__                 0x8b8a2d21
#define __BLACK_BACKING1__              0x815557b
#define __BLACK_BACKING2__              0x815557c
#define __BLACK_BACKING3__              0x815557d
#define __BLACK_BACKING_TITLE__         0x3403c8ab
#define __BLACK_BACKING__               0xf0bad40a
#define __BOTTOM_DOTTED_LINE__          0xb8f3ee1e
#define __BUTTON_1__                    0xcda0a66b
#define __BUTTON_2_TEXT_SHADOW__        0x458b0555
#define __BUTTON_2__                    0xcda0a66c
#define __BUTTON_BASE1__                0x626c9346
#define __BUTTON_BASE3__                0x626c9348
#define __BUTTON_CONTINUE_TEXT_SHADOW__ 0x45fbf4e8
#define __BUTTON_CONTINUE_TEXT__        0x1a7d81c3
#define __BUTTON_GRAD_1__               0xbcd28ac8
#define __BUTTON_GRAD_2__               0xbcd28ac9
#define __BUTTON_SMALL_GROUP_POSITION__ 0x10ecfef3
#define __BUTTON_TEXT_1__               0xda473d2f
#define __BUTTON_TEXT_2__               0xda473d30
#define __BUTTON_TEXT_SHADOW__          0x6b82eca4
#define __CAN_PAY_FINES_GROUP__         0xacabb27a
#define __COLUMN_ONE_R1_SHADOW__        0x44a69155
#define __COLUMN_ONE_R1_TEXT__          0x886909b4
#define __COLUMN_ONE_R1__               0xe90ceed0
#define __COLUMN_ONE_R2_SHADOW__        0x30e86636
#define __COLUMN_ONE_R2_TEXT__          0x8abe3255
#define __COLUMN_ONE_R2__               0xe90ceed1
#define __COLUMN_ONE_R3_SHADOW__        0x1d2a3b17
#define __COLUMN_ONE_R3_TEXT__          0x8d135af6
#define __COLUMN_ONE_R3__               0xe90ceed2
#define __COLUMN_THREE_R1_SHADOW__      0xa269a10b
#define __COLUMN_THREE_R1_TEXT__        0x5b98b3ea
#define __COLUMN_THREE_R1__             0x4be5df46
#define __COLUMN_THREE_R2_SHADOW__      0x8eab75ec
#define __COLUMN_THREE_R2_TEXT__        0x5deddc8b
#define __COLUMN_THREE_R2__             0x4be5df47
#define __COLUMN_THREE_R3_SHADOW__      0x7aed4acd
#define __COLUMN_THREE_R3_TEXT__        0x6043052c
#define __COLUMN_THREE_R3__             0x4be5df48
#define __COLUMN_TWO_R1_SHADOW__        0xbfaf1dcd
#define __COLUMN_TWO_R1_TEXT__          0xd474182c
#define __COLUMN_TWO_R1__               0xf55f1248
#define __COLUMN_TWO_R2_SHADOW__        0xabf0f2ae
#define __COLUMN_TWO_R2_TEXT__          0xd6c940cd
#define __COLUMN_TWO_R2__               0xf55f1249
#define __CONTINUE_PRESSED__            0xee839b59
#define __DOTTED_LINE_TOP_LEFT__        0xf2d7f966
#define __DOTTED_LINE_TOP_RIGHT__       0x4e45edb9
#define __DOTTED_LINE_TOP__             0xc787ec5c
#define __GRADIENT_32X331__             0x8c4d9b40
#define __GRADIENT_32X332__             0x8c4d9b41
#define __GRADIENT_32X341__             0x8c4d9b61
#define __HELP_BUTTON__                 0x27f7503
#define __HIDE_MAIN__                   0x3809713d
#define __HIGHLIGHT_2_TEXT__            0x3a94dc8e
#define __IMPOUND_MESSAGE_MAIN__        0x8b9e2e83
#define __IMPOUND_MESSAGE_SHADOW__      0xfa5cf784
#define __IMPOUND_MESSAGE__             0x2d087bdf
#define __IMPOUND_TEXT_GRP__            0x4fb122e7
#define __LAPSE_TIME2__                 0x10a276f4
#define __LAPSE_TIME__                  0xb2eda682
#define __MAIN_BLACK_BACKING1__         0xb459b83f
#define __MAIN_BLACK_BACKING_LEFT1__    0x8cf555c9
#define __MAIN_BLACK_BACKING_LEFT__     0x42550298
#define __MAIN_BLACK_BACKING_RIGHT__    0x8d641d2b
#define __MAIN_BLACK_BACKING__          0x98dbee4e
#define __MAIN_CYAN_BACKING2__          0x9f8e7cce
#define __MAIN_CYAN_BACKING3__          0x9f8e7ccf
#define __MAIN_CYAN_BACKING4__          0x9f8e7cd0
#define __MAIN_CYAN_BACKING_LEFT2__     0x17012318
#define __MAIN_CYAN_BACKING_LEFT3__     0x17012319
#define __MAIN_CYAN_BACKING_LEFT4__     0x1701231a
#define __MAIN_NAV_BAR__                0xa9e12adc
#define __MAIN_PANEL_BOTTOM__           0x85a100c7
#define __MAIN_PANEL_MIDDLE__           0x9ed42221
#define __MAIN_PANEL_TOP__              0xec1da8e5
#define __MAIN_PANEL__                  0xfbd48f93
#define __MASTER_HIDE__                 0xc229b404
#define __NUMBER2__                     0x53d9eb7a
#define __NUMBER__                      0x6f25a248
#define __ONLINE_TIMER_GROUP__          0x7d913050
#define __PAYMENT_GROUP__               0x1b2891e9
#define __PAYMENT_STATS_GROUP__         0x6c6c2337
#define __PC_CONTINUE_HIGHLIGHT__       0xccfca20d
#define __PC_CONTINUE_TEXT_GROUP__      0x436be286
#define __PC_CONTINUE__                 0x17767276
#define __PLAYER_CASH_BACKING__         0xffaf17b8
#define __PLAYER_CASH_VALUE__           0xfa206a46
#define __PLAYER_CASH__                 0xeb3a688a
#define __PLAY_CARDS_VALUE__            0x5cdf1c9d
#define __POSITION_DATA__               0x2742206d
#define __POSITION_TITLE_SHADOW__       0xb14565ba
#define __POSITION_TITLE_TEXT__         0xfa0d80d9
#define __POSITION_TITLE__              0x10ac1ad5
#define __POSITION__                    0xe8434eb4
#define __SECONDARY_PANEL_BOTTOM__      0x5ddeb64a
#define __SECONDARY_PANEL_TOP__         0x977c9548
#define __SHAPE_INGAME_OUTLINE__        0x31ac9c7f
#define __SOUND_IMPOUND_STRIKE__        0x3c782474
#define __STAGES_TITLE__                0x2db98f47
#define __STAGE_DATA__                  0xd4c80c
#define __STAGE_GROUP__                 0x1bad531f
#define __STAGE_TITLE_GROUP__           0xaed5740
#define __STANDINGS_LEAD2__             0xd6603671
#define __STANDINGS_LEAD__              0x1602ea5f
#define __STAR_LOSE_INSIDE__            0x2197b5c6
#define __STAR_LOSE__                   0x979a7c4b
#define __STATS_GROUP__                 0x50b4751a
#define __TEXTGROUP_CARD__              0x7027946a
#define __TEXTGROUP_CASH__              0x7027948f
#define __TEXT_CAMO_GROUP__             0xaba7372f
#define __TIMER_GROUP__                 0xfa9c840c
#define __TIME_DATA__                   0x62a5f207
#define __TIME_TITLE_SHADOW__           0x4ac63154
#define __TIME_TITLE_TEXT__             0x8dd29df3
#define __TIME_TITLE__                  0xb88a1faf
#define __TRIANGLE_END2__               0x124a0edd
#define __TRIANGLE_END3__               0x124a0ede
#define __TRIANGLE_END4__               0x124a0edf
#define __TRIANGLE_END__                0x3e9d654b
#define __UNHIGHLIGHT_TEXT_2__          0x3a3fe391
#define __WINNER_NAME2__                0x4d105f24
#define __WINNER_NAME__                 0x482748b2

#endif
