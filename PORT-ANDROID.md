# Port a Android — plan de arquitectura (nativo)

## Estrategia: "lo más nativo posible"
El código del juego (una vez decompilado) es C++ portable. El port sustituye SOLO la
capa de plataforma GameCube (que tenemos decompilada al 100% = superficie exacta):

| Capa GC (100% decomp) | Backend Android |
|---|---|
| GX (gráficos) | Vulkan: renderer GX→Vulkan (pipelines cacheados por estados GX) |
| AX (audio) | Oboe/AAudio (mezclador ya es software en el juego) |
| DVD/FST + ZZDATA | Storage Access / AAssetManager, packs mapeados en memoria |
| PAD/SI | InputDispatcher + capa táctil (virtual joystick) |
| OS/threads/ARAM | std::thread / preferencias |
| DSP middleware | No portar: el mezclado es CPU (snd lib ya es C portable) |

## Fases
0. **Sondeo NDK (ESTE commit)**: compilar el código independiente de plataforma
   (UMath/Foundation/CARP) con clang arm64-v8a vía CMake — valida que el árbol
   compila fuera del toolchain ProDG. `port/android/`
1. **Envoltorio de activos**: extractor ZZDATA → almacenamiento app + indexación.
2. **Backend AX→Oboe**: el mixer del juego es software; solo hay que entregar
   el buffer final. Punto de entrada natural: snd lib (C puro, portable).
3. **Backend GX→Vulkan**: la pieza grande. Estudiar GX vía los fuentes SDK
   decompilados (GXInit/GXDraw... = la spec completa). Empezar por el path de
   render del frontend (2D, FEng) antes del mundo 3D.
4. **Bucle principal + input**: loop propio con Choreographer, touch mapping.
5. Validación continua: ejecutar el DOL original en core de Dolphin (libretro)
   como referencia de comportamiento mientras el port nativo avanza.

## Reglas de oro
- Nada del árbol src/ se edita para el port: todo va con backends en port/.
- El toolchain GC sigue siendo el juez del matching (SHA del DOL).

## Fase 0.5 — primer frame nativo (EGL/GLES2)
SurfaceView + hilo nativo EGL. Clear pulsante naranja MW + triangulo GLES2.
Overlay TextView sigue mostrando `probe()` (UMath Length/Cross). Sin FEng ni loop del juego.
Build: `cd port/android` → `gradlew assembleDebug` (minSdk 28, landscape, arm64).

## Fase 1.0 — assets + stub FEng 2D
`AAssetManager` abre `assets/probe.txt`. `gx2d` dibuja quads en espacio 640x480 (barra HUD falsa).
Siguiente: texturas desde asset + comandos GX reales del FEng.

## Fase 1.1 — TGA desde AAssetManager
`logo.tga` (96x32) se copia, decodifica BGRA→RGBA y se sube a GLES. Badge arriba-izquierda. Prueba el camino asset→GPU del FEng.

## Fase 1.2 — AAudio + poly FEng
AAudio 48 kHz stereo entrega un beep (luego el mixer snd). `gx2d_fe_poly` usa el layout de `FERenderObject::AddPoly` (4 verts, UV, 0xAARRGGBB).

## Fase 1.3 — ePoly
`ePoly` (0x94, mismo layout que Ecstasy.hpp). `epoly_init` = ctor de EcstasyE. `epoly_add` = `FERenderObject::AddPoly`. `gx2d_epoly` lee Vertices/UVs/Colours. Panel + badge texturado + ePoly() crudo 64x64.

## Fase 1.4 — paquete FEng
Parser LE (PkHd/ObjL/FObj/ObjD/SA/PA). Carga `fng/PC_Loading.fng` extraido de `THINSCREEN_GLOBAL.BUN` (PC). Grupos + imagenes → ePoly. Alpha 0 se fuerza visible (sin scripts INIT).

## Fase 1.5 — TPK
`extract_tpk.py` lee `FrontB.lzc` / `GLOBALB.BUN` (chunks 0xB3300000, DXT1/3/5). TGA en `assets/tpk/`. `U2_loading_Circle` no esta en el TPK de Redux: se usa `CIRCLE_2`. Splash = `DEMO_SPLASH` 640x480.

## Fase 1.7 — toque
Tap cicla FEng: EA_TRAX, PressStart, PC_Loading, FadeScreen, DiscErrorPC, Loading. Texto con pixfont.

## Fase 1.6 — JDLZ + fuentes
`port_lz` = JDLZ (`FadeScreen.fng`, `DiscErrorPC.fng`). `font_mw_title` + atlas DXT5. Default `EA_TRAX.fng` (cadenas + TRAX_*).

## Fase 0 — avance y costuras restantes (verificado con NDK 28.2)
La sonda compila codigo real del decomp (UMath.cpp) con clang arm64-v8a. Costuras
identificadas, en orden:
1. `types.h`: rama `__ANDROID__` anadida (typedefs LP64) — HECHA.
2. `ConversionUtil.hpp`: `M_TWOPI` no existe en math.h de Android → definir en el
   shim o `-DM_TWOPI=6.28318530717958647692` (igual que hace el build GC).
3. `bMath.hpp`: `#error Choose a platform` → anadir rama `__ANDROID__` a su
   seleccion de plataforma (usar la ruta PS2/X360 como referencia de lo portable).
4. `dolphin/mtx.h` (via bMath desde CARP/UMath): shim de tipos ya creado en
   `port/android/app/src/main/cpp/platform/dolphin/types.h`; anadir mtx/vec shims
   con implementaciones C portables.
Comando de build (sin Android Studio):
  cmake -S port/android/app/src/main/cpp -B build/android-probe -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=$NDK/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28
