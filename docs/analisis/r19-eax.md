# Ronda 19 — zEAXSound / zEAXSound2

Base `base_r19_eax.json`: 299.472/321.348 B = **93,19 %**.

`audit.py` sobre las dos unidades: **759 ok / 0 fallos** en zEAXSound y
**921 ok / 0 fallos** en zEAXSound2 (confirmado con segunda pasada). Las seis
correcciones de la 18 siguen en pie.

## Lista de trabajo (triaje r19, tras `build_direct.py`)

    7508 B  98,444%  ResolveCarBanks__18CSTATEMGR_CarState        falta 1, sobran 2 (1 SUST)  ble-1 bgt+1 b-1
     612 B  97,320%  Play__11EAXFrontEnd18eMenuSoundTriggers      falta 1, sobran 2 (1 SUST)  mr-1 mr.+1 cmpwi-1
     492 B  96,829%  Play__9EAXCommon18eMenuSoundTriggers         faltan 3, sobran 2 (1 SUST) mr-1 bne+1 beq-1 li+1
     392 B  87,673%  Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi  faltan 2, sobra 1 (5 SUST)  mr+1 addi-1 li+1

## `Play__16CARSFX_RoadNoise16FXROADNOISE_LOOPi` — 392 B objetivo / 388 B nuestro

Dirección `0x800D85D4`, 98 instrucciones objetivo contra 97 nuestras.
Llamantes: ninguna `bl` directa en las dos unidades (se llama por vtable /
desde `ProcessUpdate` del mismo TU — censar).

`plan.py` da el mapa de líneas del original y deja el else-branch así:

- `.cpp:681` → `g_pEAXSound->SetCsisName(this);`
- `.cpp:688` → `m_pRoadNoiseControl[side] = new Csis::FX_ROADNOISE(...)`
  (`addi r0,r30,0x40`, `add r26,r0,r28`, `stw r31,0x0(r26)`)
- `ENVIRO_AEMS.h:490-494` → el `GetRefCount()` inline, y **recarga**
  `m_pRoadNoiseControl[side]` con `lwzx r9, r25, r28`, donde **r25 = this+0x40
  guardado desde antes de la llamada** (`mr r25, r0`).

El bloque léxico DWARF sólo declara `int refcnt`: **la temporal `slot` de nuestra
fuente no existe en el original**. Por eso nosotros formamos `0x40+side*4` y
sumamos `this` (una sola dirección), y después **recomputamos** `this+0x40`
(`addi r11, r28, 0x40`, la instrucción que sobra) para el `GetRefCount`.

Las dos que faltan: el `mr r25, r0` (copia de la base a callee-saved) y el
**segundo cero** — el objetivo materializa `li r29,0` en `ENVIRO_AEMS.h:330`
(línea de declaración de `SetVolume`) y `li r0,0` en `h:382` (el `else if` de
`SetType`).

### Ensayos

- **c1 — quitar la temporal `slot`** (`m_pRoadNoiseControl[side] = new ...` directo):
  **87,673 % → 94,939 %**, 388 B. Casan las 45 primeras instrucciones enteras
  (antes fallaban desde la 4: el reparto de callee-saved era otro).
  El DWARF respalda el cambio: el bloque léxico sólo declara `int refcnt`.
  **APLICADO.**
- c2 `type` como `(Csis::FXROADNOISETypeType)0` — 94,939 %, idéntico byte a byte.
- c3 sin llaves internas — idéntico. c4 `int refcnt = 0;` antes — idéntico.
  c5 sin asignar el resultado — idéntico. **La forma del .cpp está agotada.**
- c6 temporal `pRN` para el `new` — 88,83 %, 376 B. Descartado.
- h1/h3/h5 (dos `if` sueltos / ternario / orden invertido) aplicados a los
  **setters 2..11** de `FX_ROADNOISE`: **byte a byte idénticos** a c1. Los
  setters con argumento constante se pliegan antes de generar RTL y su forma
  es invisible. Sólo `SetId` (argumento variable) mueve la aguja: s1 ternario
  90,07 % 392/392, s2 dos `if` 86,62 % 372, s3 orden invertido 93,53 % 392/392,
  s4 tres asignaciones al campo 88,58 % 400, s5 `else { if }` idéntico a c1.
  **Los dos que dan 392/392 exactos producen la rama en el orden contrario al
  objetivo** — la trampa del tamaño exacto, otra vez.

**Lo que queda y NO he probado**: la única diferencia real es que el objetivo
materializa el cero **dos veces** (`li r29,0` para volume+azimuth+`refCount`,
`li r0,0` para type/secondary/speed/HiPass/WetFX) y nosotros una sola vez. Es
un artefacto de CSE/asignador, no de semántica: los once campos reciben los
mismos valores en los mismos desplazamientos. Con el sexto temporal el objetivo
reparte r8/r9/r11/r10 donde nosotros r10/r11/r9/r0. `InitSFX` usa el **mismo
constructor con ocho ceros** y casa al 100 % con **un solo** registro cero, así
que el constructor de la cabecera es correcto. No he probado: pasar valores que
el clamp reduzca al mismo resultado (p. ej. negativos), ni el permutador.

## `ResolveCarBanks__18CSTATEMGR_CarState` — 7.508 B objetivo / 7.512 nuestro

`0x800D0F8C..0x800D2CE0`, 1.877 instrucciones objetivo / 1.878 nuestras.
Un solo llamante `bl`: `EAXSound.cpp:1066`.
Delta normalizado: `ble-1, bgt+1, b-1` — **una sola instrucción de más**.

### Lo que el DWARF fija (y no hay que volver a discutir)

`plan.py --dwarf-only` da la anidación exacta y **casa con nuestra fuente**:
bloque `int n` de BeginRule3 `0x20BC-0x247C`, `int m` `0x20CC-0x2474`,
`HighPriority r1+0x98 / LowerPriority r1+0xB0` `0x20E4-0x2454`; BeginRule4
`int n` `0x247C-0x2770`, `wantstoload r1+0x98 / mapping r1+0x138 / first`
`0x2488-0x2770`, `int m` `0x24D0-0x2664`, `isloaded r1+0xC8` `0x24FC-0x2650`.
**BeginRule3 y BeginRule4 son hermanos** del bloque
`if (CopsCanBeInGame && ...)` (`0x17F8-0x20BC`) — no están dentro.
Los dos `if (DEBUG_PRINT_CAR_BANK_RESOLVE)` **sí reservan pila** en el original
(0xE0/0xF8 y 0xB0/0xC8), o sea que su RTL existe antes de plegarse.

### La única diferencia, exacta

La cola compartida de 9 instrucciones
(`mr r3,r20 / b / mr r3,r31 / li / bl ~Instance / mr r3,r17 / li / bl / b LoadRemaining`)
está **colocada en sitios distintos**:

- objetivo: `0x2098-0x20B8`, es decir **al final del bloque
  `if (EnginesThatCanUpgradeToV8.size() != 0)`** (`0x1F0C-0x20BC`), justo detrás
  del `b BeginRule2` y justo delante de la etiqueta de BeginRule3.
- nuestro: `+0x17B8`, **al final del bloque `if (n >= 0)` de BeginRule4**, entre
  el `b BeginRule4` y el destructor de salida normal.

Y de ahí sale la instrucción de más: con la cola fuera de en medio, el objetivo
puede fundir el último `<destruye r17>; b BeginRule4` con el idéntico del bucle
interno (`0x2634-0x264C`) e invertir la rama → `bgt 0x2644` (1+3 = 4
instrucciones). Nosotros fundimos **en el sentido contrario** (el `b` interno de
`0x1690` salta a `+0x17a8`, la copia tardía sobrevive), así que nos queda
`ble` + 3 + `b` = 5. Es la dirección del *cross-jump* de `jump.c`: sobrevive el
lado de la ETIQUETA, y a nosotros nos toca al revés.

**Marca que lo delata**: la `b` final de la cola lleva línea 582 (la etiqueta
`LoadRemainingEngines`) en el objetivo — el fixup se resolvió en la etiqueta —
y línea 302 en el nuestro (el `}` de BeginRule4) — se resolvió al cerrar ese
bloque.

### Ensayos de la 19 (todos medidos, ninguno mueve nada)

Todos dan **exactamente 98,44433 %, 7.512 B, 58 diffs** — byte a byte iguales:

- `a1` BeginRule3 con `for (int n = ...)` en vez de `{ int n; for(;;) }`.
- `b1` último `if` sin llaves · `b2` con `else` explícito · `b3` con `!(<=)` ·
  `b4` `>= 5` · `b5` `if (<=4) goto LoadRemainingEngines; goto BeginRule4;`.
- `e2` bloque `{ }` extra englobando BeginRule3+BeginRule4.
- `e3` el bucle de `LoadRemainingEngines` dentro de su propio bloque.
- `e1` BeginRule4 sin bloque exterior: **no compila**.

Sumados a los seis del informe anterior (`c0/c1/c2/c5/c6/c9`, mismo SHA256), son
**19 formas de fuente con salida idéntica**. La forma del `if`/`goto`/llaves y el
anidamiento de bloques **no son la palanca**.

**Lo que sigue sin probar**: reordenar las declaraciones `Attrib::Gen::engineaudio`
dentro de los bucles; dar cuerpo real a los dos `if (DEBUG_PRINT_CAR_BANK_RESOLVE)`
(el original reserva pila para ellos, luego su RTL pesa); y meter un
`goto LoadRemainingEngines` adicional en el bloque
`if (EnginesThatCanUpgradeToV8...)` para que el hueco de fixup caiga ahí.

## `Play__11EAXFrontEnd18eMenuSoundTriggers` — 612 B objetivo / 616 nuestro

`0x800AD3AC..0x800AD610`. Dos bloques gemelos: `new PlayFrontEndSample_RS`
(rama `then`) y `new PlayFrontEndSample` (rama `else`). Las 54 primeras
instrucciones casan; quedan 14 diffs.

### Hallazgo grande: el DWARF desmiente dos locales nuestras

El volcado (`symbols/mw_dwarfdump.nothpp`, línea 489338) dice literalmente:

    virtual int EAXFrontEnd::Play(enum eMenuSoundTriggers etrigger /* r30 */) {
        int nvol;    // r28
        int testID;  // r9

**Sólo dos locales.** No existen ni `adjusted` ni `pitch`, y el `testID` del
original es **`etrigger + 23`** (el `addi r9, r30, 0x17` de la línea 250), no
`(int)etrigger`. El `id` que recibe el constructor es **`etrigger`** (`mr r0, r30`).
Nuestro `adjusted` ES su `testID` (mismo registro r9) y nuestro `testID` es una
copia que se pliega, así que **el reparto de registros de la línea base ya es el
del objetivo**; `pitch`, en cambio, es un invento nuestro.

Transcribir el DWARF al pie de la letra (`testID = etrigger + 23`, `id = etrigger`,
sin `pitch`) da **el tamaño exacto, 612 B, pero 28 diffs y 92,05 %**: al pasar
`etrigger` al constructor, `etrigger` y `testID` viven a la vez, GCC gasta dos
callee-saved (r27 para etrigger, r30 para testID) y **sube el `addi` por encima
de las dos llamadas**, mientras el objetivo lo deja detrás y le da r9. Medido con
`k1` (declarar `testID` a media función), `k2` (+`pitch`), `m1`/`m2`
(`testID = etrigger; testID += 23;`): los cuatro, 612 B y 28 diffs.

### Lo que queda (sobre la línea base, 616 B / 14 diffs)

- **Bloque 2** (`PlayFrontEndSample`): el objetivo funde `mr. r11, r28` y compara
  **la copia** (`cmpwi r11, 0x7fff`); nosotros hacemos `mr`+`cmpwi r28,0` y
  comparamos **el original**, lo que obliga a guardar `mData.volume` dos veces.
  Ésa es la instrucción de más. Nótese que **el `SetId` del mismo bloque sí funde
  en ambos** (`mr. r0, r30`) y que en el bloque 1 **ninguno de los dos funde**:
  es estado del asignador, no forma de fuente.
- **Bloque 1**: r10/r11 y r11/r0 permutados, más el `stw ...,0xc` adelantado.
  Mismo número de instrucciones.

### Ensayos (todos medidos)

`g1/g2/g3` `SetVolume` de `PlayFrontEndSample` con local `volume` (las tres
variantes de qué se compara): **byte a byte iguales** — la propagación de copias
las deshace. `g4` asignar al campo y luego acotarlo: 20 diffs, peor.
`v1` `pitch` tras el `delete` · `v2` `pitch` local del bloque · `v3` `getref` en
la declaración: idénticos. `z1` quitar `pitch` manteniendo `adjusted`: 616 B,
**17 diffs** (rompe también el `SetVolume` del bloque 1). `n1` ramas giradas:
58 %, catastrófico. `y1..y4` formas de la expresión `testID`: 28 diffs.

## `Play__9EAXCommon18eMenuSoundTriggers` — 492 B objetivo / 488 nuestro

El DWARF **confirma nuestra estructura**: única local `int nvol` (r30), bloque
anónimo con `int CurSliderVol` (r0) y los inlines `bClamp/bMax/bMin`.

La diferencia, exacta: el objetivo carga el puntero en **r3**
(`lwz r3, 0x24(r28)`) y lo conserva hasta el `bl GetDMixOutput`; nosotros lo
cargamos en r9 y hacemos `mr r3, r9`. Y es que **nosotros izamos `li r3, 0x0`**
(el `return 0` de la cuarta guarda) por encima del `cmpwi`, ocupando r3; el
objetivo iza en su lugar `li r3, -0x1` (el `return -1` de `Debug_Common_FE_OFF`)
y encadena ese `bne` directamente al epílogo. Es decir: **de los cuatro retornos
tempranos el objetivo hilvana el segundo y nosotros el cuarto.**

Ensayos: `p1` con llaves · `p2` los dos primeros con `||` · `p3` comparaciones
explícitas contra 0/nullptr · `p4` `nvol = 0; return nvol;`: **los cuatro byte a
byte idénticos**. Con las 8 formas del informe anterior son **12**. Muro de
asignador confirmado.
