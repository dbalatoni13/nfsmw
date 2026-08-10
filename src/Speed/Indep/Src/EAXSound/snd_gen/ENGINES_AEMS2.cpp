#include "Speed/Indep/Src/EAXSound/SND_GEN/ENGINES_AEMS2.h"

namespace Csis {

InterfaceId Sputter_MessageId = {"Sputter_Message", 0x383F, 0x0783}; // size: 0x8, Decl: 12
FunctionHandle gSputter_MessageHandle;                               // size: 0x10, Decl: 13

InterfaceId CARId = {"CAR", 0x383F, 0x198B}; // size: 0x8, Decl: 15
ClassHandle gCARHandle;                      // size: 0x10, Decl: 16

InterfaceId CAR_SWTNId = {"CAR_SWT", 0x383F, 0x7859}; // size: 0x8, Decl: 24
ClassHandle gCAR_SWTNHandle;                          // size: 0x10, Decl: 25

InterfaceId CAR_WHINEId = {"CAR_WHINE", 0x383F, 0x603F}; // size: 0x8, Decl: 27
ClassHandle gCAR_WHINEHandle;                            // size: 0x10, Decl: 28

InterfaceId CAR_TRANNYId = {"CAR_TRANNY", 0x383F, 0x6F12}; // size: 0x8, Decl: 30
ClassHandle gCAR_TRANNYHandle;                             // size: 0x10, Decl: 31

InterfaceId CAR_SputterId = {"CAR_Sputter", 0x383F, 0x3686}; // size: 0x8, Decl: 33
ClassHandle gCAR_SputterHandle;                              // size: 0x10, Decl: 34

InterfaceId CAR_SputOutputId = {"CAR_SputOutput", 0x383F, 0x1901}; // size: 0x8, Decl: 36
ClassHandle gCAR_SputOutputHandle;                                 // size: 0x10, Decl: 37

}; // namespace Csis
