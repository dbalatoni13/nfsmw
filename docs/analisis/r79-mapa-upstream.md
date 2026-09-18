# r79 — Mapa del trabajo que queda con upstream

Los 689 ficheros de contenido (tocados por ambos, sin `asm` por ninguna parte)

## Lo que NO funciona, medido

- **Sobrescribir con el suyo**: pierde lo nuestro (ICEManager.hpp: 40 declaraciones nuestras contra 13 suyas).
- **Fusion textual limpia**: no es semanticamente segura. Los 116 que fusionaban sin conflicto rompieron GameCube (`VU0_ACos` redefinida en UVectorMath.h). Las CABECERAS son las peligrosas.
- **Politica `--ours` a granel** sobre los 265 con conflicto: deja codigo inconsistente (en CarRender.cpp una variable queda declarada en un trozo rechazado). Resolver por hunks NO es seguro.
- **El objetivo `ok` no basta**: no compila las unidades NonMatching. Hay que correr `ninja` COMPLETO.

## Lo que SI funciona

- Fusion a tres bandas con los finales de linea normalizados, **solo en `.c`/`.cpp`**, y verificando con el build completo: 25 ficheros aplicados y el DOL exacto.

## La prioridad: donde upstream tiene contenido que NO tenemos

Indexados los 133.963 identificadores de nuestro arbol; upstream declara **267 nombres** que no existen en ninguna parte del nuestro, repartidos en **68 ficheros**:

| fichero | nombres | ejemplos |
|---|---:|---|
| `Speed/Indep/Src/EAXSound/EAXSOund.hpp` | 37 | eDEPTH_REVERB, eINVERTED_MIX_CTRL, eMISC_SOUNDS, eREVERB_TRIGGERS |
| `Speed/Indep/Src/Gameplay/GRaceStatus.h` | 34 | DetachCamera, EndForceStop, GetAiRanking, GetAveragePercentComplete |
| `Speed/Indep/Libs/snd/9/include/snd/sndo.h` | 24 | CapOutputSampleRate, CapVoices, GetHeap, GetHeapThreshold |
| `Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp` | 15 | BuildImageDisplayName, CheckForOldSaveFile, FakeLoad, FoundInBoot |
| `Speed/Indep/Src/Camera/ICE/ICEData.hpp` | 13 | FlushTrack, GetGroup, GetTrackList, ICESingleKey |
| `Speed/Indep/Src/Camera/ICE/ICEManager.hpp` | 12 | GetCurrentGroup, GetCurrentTrack, GetNumGroups, GetNumGroupsWithData |
| `Speed/Indep/Src/Input/ISteeringWheel.h` | 10 | ConvertAcceleratorPosition, ConvertBrakePosition, Deactivated, GetGameBrakerValue |
| `Speed/Indep/Src/EAXSound/SND_GEN/MAIN_AEMS.h` | 9 | CacheHandlesMAIN_AEMS, GetHYD_ID, GetHYD_PITCH_OFFSET, GetHYD_azimuth |
| `Speed/Indep/Src/EAXSound/EAXAemsManager.cpp` | 9 | CacheHandlesCOP_SIREN_AEMS, CacheHandlesENGINES_AEMS2, CacheHandlesENVIRO_AEMS, CacheHandlesFE_AEMS |
| `s/project.py` | 7 | BuildConfig, BuildConfigLink, BuildConfigModule, BuildConfigUnit |
| `Speed/Indep/Src/Gameplay/GManager.h` | 6 | ClearQueuedFreeRoamPursuit, Get24BitAttributeKey, Get32BitCollectionKey, GetFreeRoamFromSafeHouseStartMarker |
| `Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp` | 6 | CalculateMemorySize, PreAllocate, SetScrollMessages, UpdateArrowsMouse |
| `Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsScreen.hpp` | 5 | SetupDebug, SetupDebugAI, SetupDebugOnline, SetupDebugPhysics |
| `Speed/Indep/Src/Speech/MiscSpeech.h` | 3 | SPCH_Rules, Type_eta, Type_roadcondition_type |
| `Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp` | 3 | FEngIsVisible, FEngSetCenterY, FEngSetVisibility |
| `Speed/Indep/Src/Frontend/Database/FEDatabase.hpp` | 3 | CS_SpecialFlags, HasBeenAwardedCastrolGT, SetAwardedCastrolGT |
| `Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp` | 3 | IsInTunnel, IsReadyForSwitch, WasInTunnel |
| `Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp` | 3 | DoNothing, Speech_Done, ToggleSpeech |
| `Speed/Indep/Src/EAXSound/EAXSND8Wrapper.hpp` | 3 | DeleteStreamBuffer, HasAudioModeChanged, RestoreSoundDriver |
| `Speed/Indep/bWare/Inc/bWare.hpp` | 2 | bAssertFailMsg, bEndianSwap |

## Conflictos por fichero (los 270 `.cpp`)

| conflictos | ficheros |
|---:|---:|
| 1 | 45 |
| 2 | 20 |
| 3 | 28 |
| 4 | 21 |
| 5 | 19 |
| 6 | 14 |
| 7 | 17 |
| 8 | 10 |
| 9 | 2 |
| 10 | 4 |
| 11 | 7 |
| 12 | 6 |
| 13 | 3 |
| 14 | 6 |
| 15 | 6 |
| 16 | 4 |
| 17 | 3 |
| 18 | 4 |
| 19 | 2 |
| 20 | 4 |
| 21 | 8 |
| 22 | 2 |
| 23 | 2 |
| 24 | 1 |
| 26 | 2 |
| 27 | 2 |
| 29 | 1 |
| 30 | 3 |
| 31 | 1 |
| 32 | 1 |
| 33 | 1 |
| 34 | 1 |
| 35 | 3 |
| 36 | 1 |
| 37 | 1 |
| 41 | 1 |
| 42 | 1 |
| 45 | 1 |
| 47 | 2 |
| 50 | 2 |
| 52 | 1 |
| 55 | 1 |
| 57 | 1 |
| 75 | 1 |
| 77 | 1 |
| 80 | 1 |
| 87 | 1 |
| 127 | 1 |

Los de 1 conflicto que son SourceLists llevan nuestra documentacion contra su version simple: ahi gana la nuestra. Los demas hay que mirarlos uno a uno.
