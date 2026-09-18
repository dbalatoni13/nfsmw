# Accesores con cuerpo vacio en cabeceras, ordenados por llamantes

> **CORRECCION (2026-09-18). Este frente esta AGOTADO, y este documento lo
> sobrevalora mucho.** El conteo de llamantes de abajo es por NOMBRE, y el aviso
> que ya lleva («la mayoria de los nombres muy usados tienen varias sobrecargas y
> solo algunas estan vacias») resulta ser el efecto dominante, no una nota al pie.
>
> Medido contra las expansiones inline del DWARF del original, con
> `scripts/missinline.py rank` (que ahora clasifica `VACIO` y `VACIO_CTOR`):
>
> | | |
> |---|---:|
> | accesores con cuerpo `{}` en el arbol | 597 |
> | de esos, que el original EXPANDA inline | **225** |
> | de esos, constructores o destructores (vacio = CORRECTO: lo hacen la base y los miembros) | **222** |
> | accesores de verdad con cuerpo vacio que el original expande | **3** |
> | de esos 3, que bloqueen alguna funcion por debajo del 100 % | **0** |
>
> Prueba practica: rellene los diez de `FEng/FEList.h` siguiendo el patron exacto
> de sus hermanos ya rellenos en el mismo fichero, y el resultado fue **cero
> cambio en las cinco versiones**. La razon la da el propio volcado:
> `FEHashList::GetTableSize` y `FEHashNodePtr::GetNodePtr` **no aparecen en el
> DWARF del original, ni con candidatos** — el original no los expande nunca. Los
> 421 llamantes de `GetHead` son de `FEMinList::GetHead` y `FEList::GetHead`, que
> ya estaban rellenos.
>
> Los cambios se quedan porque son codigo mas veraz (un accesor que devuelve su
> miembro en vez de basura), pero **no son una via de subir el porcentaje**. La
> tabla de abajo se conserva como referencia de donde estan, no como orden de
> trabajo.


Generado con un barrido de `src/Speed/Indep/**.h[pp]` buscando definiciones
no-`void` cuyo cuerpo es `{}`, cruzado con el numero de sitios de llamada en
los `.cpp` del arbol.

**Total: 597 accesores vacios en 154 cabeceras; 110 de ellos tienen llamantes.**

Cada uno devuelve basura, asi que **ninguna funcion que lo llame puede casar**.
Es la causa de near-miss mas barata de arreglar que queda, y esta cruzada:
un solo accesor con 60 llamantes desbloquea funciones en varias unidades.

AVISO: la mayoria de los nombres muy usados (`bMin`, `bMax`, `bLength`, `bDot`)
tienen **varias sobrecargas** y solo algunas estan vacias — el conteo de
llamantes es del NOMBRE, no de la sobrecarga. Hay que mirar cual se llama.

| llamantes | accesor | declaracion |
|---:|---|---|
| 516 | `GetNext` | `Speed/Indep/Src/FEng/FEList.h:206` — `FEHashNode *GetNext() const {}` |
| 480 | `GetPosition` | `Speed/Indep/bWare/Inc/SpeedScript.hpp:70` — `int GetPosition() {}` |
| 421 | `GetHead` | `Speed/Indep/Src/FEng/FEList.h:250` — `FEHashNode *GetHead() const {}` |
| 420 | `bPlatEndianSwap` | `Speed/Indep/bWare/Inc/bWare.hpp:204` — `inline void bPlatEndianSwap(uint8 *value) {}` |
| 420 | `bPlatEndianSwap` | `Speed/Indep/bWare/Inc/bWare.hpp:206` — `inline void bPlatEndianSwap(int8 *value) {}` |
| 295 | `Init` | `Speed/Indep/Src/EAGL4Anim/FnAnimFactory.h:31` — `bool Init(AnimMemoryMap *anim) const {}` |
| 295 | `Init` | `Speed/Indep/Src/Sim/SimProfile.h:22` — `inline void Init() {}` |
| 222 | `GetData` | `Speed/Indep/bWare/Inc/bFunk.hpp:30` — `uint8 *GetData() {}` |
| 222 | `GetData` | `Speed/Indep/bWare/Inc/bFunk.hpp:32` — `uint8 *GetData(int pos) {}` |
| 184 | `Destroy` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:16` — `static inline void Destroy(struct Shape *shape) {}` |
| 106 | `GetName` | `Speed/Indep/bWare/Inc/SpeedScript.hpp:62` — `char *GetName() {}` |
| 106 | `GetName` | `Speed/Indep/Src/World/CarInfo.hpp:131` — `char *GetName() {}` |
| 106 | `GetName` | `Speed/Indep/Src/World/CarInfo.hpp:731` — `const char *GetName() {}` |
| 106 | `GetName` | `Speed/Indep/Src/World/RaceParameters.hpp:145` — `char *GetName() {}` |
| 105 | `Create` | `Speed/Indep/Src/EAGL4Anim/FnAnimFactory.h:35` — `FnAnim *Create(AnimTypeId animTypeId) const {}` |
| 105 | `Create` | `Speed/Indep/Src/EAGL4Anim/FnAnimFactory.h:37` — `FnAnimMemoryMap *Create(AnimMemoryMap *anim) const {}` |
| 101 | `Length` | `Speed/Indep/Src/FEng/FEMath.h:250` — `f32 Length() const {}` |
| 99 | `GetType` | `Speed/Indep/bWare/Inc/bFunk.hpp:24` — `int GetType() {}` |
| 95 | `Release` | `Speed/Indep/Src/Sim/SimProfile.h:20` — `inline void Release(HSIMPROFILE) {}` |
| 89 | `GetTime` | `Speed/Indep/Src/Misc/Profiler.hpp:17` — `float GetTime() {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/CompoundChannel.h:44` — `int GetSize() const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/CsisEventChannel.h:52` — `int GetSize() const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/DeltaCompressedData.h:49` — `int GetSize(int numFrames) const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/PhaseChan.h:62` — `int GetSize() const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/PosePalette.h:72` — `int GetSize() const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/RawPoseChannel.h:84` — `int GetSize() const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/ScratchBuffer.h:39` — `unsigned int GetSize() const {}` |
| 88 | `GetSize` | `Speed/Indep/Src/EAGL4Anim/SkeletonData.h:38` — `int GetSize() const {}` |
| 86 | `bMax` | `Speed/Indep/bWare/Inc/bMath.hpp:582` — `inline bVector3 *bMax(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {}` |
| 86 | `bMax` | `Speed/Indep/bWare/Inc/bMath.hpp:888` — `inline bVector4 *bMax(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {}` |
| 70 | `bMin` | `Speed/Indep/bWare/Inc/bMath.hpp:580` — `inline bVector3 *bMin(bVector3 *dest, const bVector3 *v1, const bVector3 *v2) {}` |
| 70 | `bMin` | `Speed/Indep/bWare/Inc/bMath.hpp:886` — `inline bVector4 *bMin(bVector4 *dest, const bVector4 *v1, const bVector4 *v2) {}` |
| 67 | `IsEnabled` | `Speed/Indep/Src/World/DebugWorld.h:11` — `static bool IsEnabled() {}` |
| 64 | `bLength` | `Speed/Indep/bWare/Inc/bMath.hpp:910` — `inline float bLength(const bVector4 &v) {}` |
| 58 | `bDot` | `Speed/Indep/bWare/Inc/bMath.hpp:906` — `inline float bDot(const bVector4 &v1, const bVector4 &v2) {}` |
| 56 | `GetString` | `Speed/Indep/bWare/Inc/Strings.hpp:56` — `const char *GetString(int index) {}` |
| 56 | `GetString` | `Speed/Indep/Src/FEng/FEListBox.h:87` — `FEWideString GetString() const {}` |
| 55 | `GetNumBones` | `Speed/Indep/Src/EAGL4Anim/FnRawPoseChannel.h:61` — `int GetNumBones() const {}` |
| 55 | `GetNumBones` | `Speed/Indep/Src/EAGL4Anim/RawPoseChannel.h:88` — `int GetNumBones() const {}` |
| 47 | `GetPrev` | `Speed/Indep/Src/FEng/FEList.h:208` — `FEHashNode *GetPrev() const {}` |
| 45 | `GetTail` | `Speed/Indep/Src/FEng/FEList.h:252` — `FEHashNode *GetTail() const {}` |
| 45 | `IsLoaded` | `Speed/Indep/Src/Ecstasy/Texture.hpp:207` — `int IsLoaded() {}` |
| 44 | `GetDimension` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:99` — `inline int GetDimension() const {}` |
| 43 | `IsPlayer` | `Speed/Indep/Src/World/CarInfo.hpp:735` — `bool IsPlayer() {}` |
| 39 | `GetLength` | `Speed/Indep/Src/Animation/WorldAnimCtrl.hpp:149` — `float GetLength() {}` |
| 39 | `GetLength` | `Speed/Indep/Src/Misc/QuickSpline.hpp:114` — `float GetLength() {}` |
| 38 | `SetState` | `Speed/Indep/Src/Frontend/RealFontOld.hpp:84` — `bool SetState(FontState state, int value) {}` |
| 32 | `Shutdown` | `Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.h:42` — `static void Shutdown() {}` |
| 32 | `Shutdown` | `Speed/Indep/Src/Sim/SimProfile.h:24` — `inline void Shutdown() {}` |
| 29 | `Param` | `Speed/Indep/Src/Sim/SimTypes.h:185` — `template <typename T> Param(UCrc32 name, const T *addr) : mType(addr->TypeName()), mName(n` |
| 26 | `Service` | `Speed/Indep/bWare/Inc/bFunk.hpp:85` — `virtual int Service() {}` |
| 25 | `FEVector2` | `Speed/Indep/Src/FEng/FEGenericVal.h:17` — `operator FEVector2() const {}` |
| 24 | `GetAcceleration` | `Speed/Indep/Src/EAXSound/EAXCarState.hpp:169` — `const bVector3 *GetAcceleration() {}` |
| 22 | `Push` | `Speed/Indep/Src/Main/ScratchPtr.h:67` — `template <typename T> void ScratchPtr<T>::Push(void *workspace) {}` |
| 21 | `Begin` | `Speed/Indep/Src/Sim/SimProfile.h:26` — `inline void Begin() {}` |
| 20 | `GetHeight` | `Speed/Indep/Src/Frontend/RealFontOld.hpp:100` — `int GetHeight() const {}` |
| 20 | `GetHeight` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:27` — `inline int GetHeight() const {}` |
| 20 | `GetHeight` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:385` — `inline unsigned int GetHeight() {}` |
| 17 | `Register` | `Speed/Indep/Src/Interfaces/IAttributeable.h:11` — `static void Register(IAttributeable *who, Attrib::Key class_key) {}` |
| 16 | `GetWidth` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:25` — `inline int GetWidth() const {}` |
| 16 | `GetWidth` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:383` — `inline unsigned int GetWidth() {}` |
| 15 | `GetPoint` | `Speed/Indep/Src/Misc/QuickSpline.hpp:90` — `float GetPoint(float param, int deriv) {}` |
| 15 | `GetPoint` | `Speed/Indep/Src/Misc/QuickSpline.hpp:92` — `bVector2 *GetPoint(bVector2 *point, float param, int deriv) {}` |
| 15 | `GetPoint` | `Speed/Indep/Src/Misc/QuickSpline.hpp:94` — `bVector3 *GetPoint(bVector3 *point, float param, int deriv) {}` |
| 14 | `GetCarType` | `Speed/Indep/Src/EAXSound/EAXCarState.hpp:76` — `CarType GetCarType() {}` |
| 14 | `GetCarType` | `Speed/Indep/Src/World/CarInfo.hpp:734` — `CarType GetCarType() {}` |
| 14 | `GetNumFrames` | `Speed/Indep/Src/Animation/AnimCtrl.hpp:100` — `int GetNumFrames() {}` |
| 14 | `GetNumFrames` | `Speed/Indep/Src/Animation/WorldAnimCtrl.hpp:151` — `int GetNumFrames() {}` |
| 14 | `GetNumFrames` | `Speed/Indep/Src/EAGL4Anim/FnRawPoseChannel.h:59` — `int GetNumFrames() const {}` |
| 13 | `Cleanup` | `Speed/Indep/Src/EAGL4Anim/MemoryPoolManager.h:48` — `static void Cleanup() {}` |
| 12 | `GetDelta` | `Speed/Indep/Src/EAGL4Anim/DeltaQFast.h:141` — `DeltaQFastDelta *GetDelta(unsigned char *binData, int deltaIdx) {}` |
| 12 | `GetDistance` | `Speed/Indep/Src/Misc/QuickSpline.hpp:112` — `float GetDistance(float param) {}` |
| 12 | `GetShiftUpRPM` | `Speed/Indep/Src/EAXSound/EAXCarState.hpp:75` — `float GetShiftUpRPM(Gear Gear) {}` |
| 12 | `InitAnimMemoryMap` | `Speed/Indep/Src/EAGL4Anim/AnimMemoryMap.h:37` — `static void InitAnimMemoryMap() {}` |
| 12 | `IsSplitScreen` | `Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp:53` — `bool IsSplitScreen() {}` |
| 12 | `Size` | `Speed/Indep/Libs/Support/Miscellaneous/CARP.h:144` — `unsigned int Size() const {}` |
| 10 | `SetWidth` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:55` — `inline void SetWidth(int width) {}` |
| 9 | `GetPlayerCar` | `Speed/Indep/Src/World/World.hpp:54` — `Car *GetPlayerCar(int index) {}` |
| 8 | `GetCar` | `Speed/Indep/Src/World/World.hpp:52` — `Car *GetCar(int index) {}` |
| 8 | `GetRideInfo` | `Speed/Indep/Src/World/CarInfo.hpp:733` — `RideInfo *GetRideInfo() {}` |
| 8 | `bAngToRad` | `Speed/Indep/bWare/Inc/bMath.hpp:227` — `inline float bAngToRad(short angle) {}` |
| 8 | `espEmptyLayer` | `Speed/Indep/bWare/Inc/Espresso.hpp:21` — `inline void espEmptyLayer(const char *layername) {}` |
| 7 | `GetShiftDownRPM` | `Speed/Indep/Src/EAXSound/EAXCarState.hpp:74` — `float GetShiftDownRPM(Gear Gear) {}` |
| 7 | `UnRegister` | `Speed/Indep/Src/Interfaces/IAttributeable.h:13` — `static void UnRegister(IAttributeable *who) {}` |
| 7 | `bAngToDeg` | `Speed/Indep/bWare/Inc/bMath.hpp:229` — `inline float bAngToDeg(short angle) {}` |
| 7 | `bEqual` | `Speed/Indep/bWare/Inc/bMath.hpp:908` — `inline int bEqual(const bVector4 &v1, const bVector4 &v2, float epsilon) {}` |
| 6 | `FindSystem` | `Speed/Indep/Libs/Support/Miscellaneous/CARP.h:260` — `const EventSeqSystem *FindSystem(unsigned int ident) const {}` |
| 6 | `GetAttribute` | `Speed/Indep/Src/World/CarInfo.hpp:174` — `CarPartAttribute *GetAttribute() {}` |
| 6 | `GetNumElements` | `Speed/Indep/Src/FEng/FEList.h:248` — `u32 GetNumElements() const {}` |
| 6 | `GetSecondDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:104` — `float GetSecondDerivative(float param) {}` |
| 6 | `GetSecondDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:106` — `bVector2 *GetSecondDerivative(bVector2 *deriv, float param) {}` |
| 6 | `GetSecondDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:108` — `bVector3 *GetSecondDerivative(bVector3 *deriv, float param) {}` |
| 6 | `GetSecondDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:110` — `bVector4 *GetSecondDerivative(bVector4 *deriv, float param) {}` |
| 6 | `VU0_v4scalexyz` | `Speed/Indep/Libs/Support/Utility/UVectorMath.hpp:83` — `inline void VU0_v4scalexyz(const UMath::Vector4 &a, const float scaleby, UMath::Vector4 &r` |
| 5 | `FindNode` | `Speed/Indep/Src/FEng/FEList.h:147` — `FENode *FindNode(u32 ordinalnumber) const {}` |
| 4 | `Debug` | `Speed/Indep/Src/AI/AIDebug.h:9` — `inline void Debug() {}` |
| 4 | `FEColor` | `Speed/Indep/Src/FEng/FEGenericVal.h:20` — `operator FEColor() const {}` |
| 4 | `GetDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:96` — `float GetDerivative(float param) {}` |
| 4 | `GetDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:98` — `bVector2 *GetDerivative(bVector2 *deriv, float param) {}` |
| 4 | `GetDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:100` — `bVector3 *GetDerivative(bVector3 *deriv, float param) {}` |
| 4 | `GetDerivative` | `Speed/Indep/Src/Misc/QuickSpline.hpp:102` — `bVector4 *GetDerivative(bVector4 *deriv, float param) {}` |
| 4 | `GetStartTime` | `Speed/Indep/Src/EAGL4Anim/FnCycle.h:50` — `float GetStartTime() const {}` |
| 4 | `IsInited` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:120` — `inline bool IsInited() {}` |
| 4 | `espCreateObjectAsync` | `Speed/Indep/bWare/Inc/Espresso.hpp:30` — `inline void espCreateObjectAsync(const char *layername, const char *type, FloatVector *pos` |
| 4 | `espSetAttributeString` | `Speed/Indep/bWare/Inc/Espresso.hpp:34` — `inline void espSetAttributeString(unsigned int hobj, const char *attributename, const char` |
| 4 | `float` | `Speed/Indep/Src/FEng/FEGenericVal.h:15` — `operator float() const {}` |
| 3 | `GetKeyNumber` | `Speed/Indep/Src/FEng/FEKeyTrack.h:88` — `FEKeyNode *GetKeyNumber(u32 Number) {}` |
| 3 | `GetNumCars` | `Speed/Indep/Src/World/World.hpp:44` — `int GetNumCars() {}` |
| 3 | `GetTimeScale` | `Speed/Indep/Src/World/World.hpp:73` — `float GetTimeScale() {}` |
| 3 | `SetHeight` | `Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp:57` — `inline void SetHeight(int height) {}` |
| 3 | `ValidatePtr` | `Speed/Indep/Libs/Support/Utility/UCOM.h:146` — `inline void ValidatePtr(const IUnknown *pUnk) {}` |
| 3 | `espSetObjectPosition` | `Speed/Indep/bWare/Inc/Espresso.hpp:32` — `inline void espSetObjectPosition(unsigned int hobj, FloatVector *pvector) {}` |
| 3 | `espSetUserMeshFace` | `Speed/Indep/bWare/Inc/Espresso.hpp:40` — `inline void espSetUserMeshFace(unsigned int hobj, int face_num, FloatVector *vertices) {}` |
| 2 | `GetAnim` | `Speed/Indep/Src/EAGL4Anim/FnEventBlender.h:49` — `FnAnim *GetAnim(int i) {}` |
| 2 | `GetAnim` | `Speed/Indep/Src/EAGL4Anim/FnPoseBlender.h:47` — `FnAnim *GetAnim(int i) {}` |
| 2 | `GetDataSize` | `Speed/Indep/bWare/Inc/bFunk.hpp:28` — `int GetDataSize() {}` |
| 2 | `GetElapsedTime` | `Speed/Indep/Src/World/World.hpp:67` — `float GetElapsedTime(int32 start_time, int32 end_time) {}` |
| 2 | `GetElapsedTime` | `Speed/Indep/Src/World/World.hpp:69` — `float GetElapsedTime(int32 start_time) {}` |
| 2 | `GetNumEvents` | `Speed/Indep/Src/EAGL4Anim/FnCsisEventChannel.h:51` — `int GetNumEvents() const {}` |
| 2 | `GetNumEvents` | `Speed/Indep/Src/EAGL4Anim/FnRawEventChannel.h:52` — `int GetNumEvents() const {}` |
