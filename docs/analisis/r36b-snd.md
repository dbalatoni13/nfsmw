# Ronda 36b — `c36b-snd`: zEagl4Anim, zEAXSound, zEAXSound2, zSpeech

**Encargo verificado.** `triaje.py` sobre las cuatro unidades reproduce
exactamente el reparto del brief: **17 funciones, 11.736 B**.

    zEagl4Anim   4 funciones  5008 B
    zEAXSound    4 funciones  1916 B
    zEAXSound2   7 funciones  3900 B
    zSpeech      2 funciones   912 B

**Resultado: ninguna función cerrada.** Un solo cambio aplicado al árbol
(`NFSMixMapState.cpp`, 3 líneas: `CreateSubMixChannels` **98,69048 → 99,28571 %**,
de 16 a **9** filas de diff). Todo lo demás **revertido**; `git status` del
territorio enseña ese fichero y nada más. **Sin commits.**

Lo que traigo es **veinte medidas** —cuatro permutadores, catorce ensayos de
fuente y dos hallazgos de método— y, sobre todo, **el diagnóstico al byte de las
cinco funciones más cercanas**, que quedan localizadas a UNA o DOS instrucciones
concretas.

---

## 1. Lo aplicado: `NFSMixMapState::CreateSubMixChannels`, 98,690 → 99,286 %

Lo encontró el **permutador ciego** (221 variantes de un solo cambio,
`score 0,809524 → 0,892857` con un `cse_temp` sobre `numin`; la ronda 2, otras
227 variantes, no mejoró). Escrito en limpio:

```c
-            numin = *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(&pSubMixParms->MIXCHID) + 1);
+            int nextin = *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(&pSubMixParms->MIXCHID) + 1);
+            numin = nextin;
 
             this->m_SubMixChannelsAdded++;
-            pSubMixParms = …(reinterpret_cast<int *>(pSubMixParms) + numin) + sizeof(stSubMixChParams));
+            pSubMixParms = …(reinterpret_cast<int *>(pSubMixParms) + nextin) + sizeof(stSubMixChParams));
```

`triaje.py zEAXSound2` antes y después: las otras seis funciones **idénticas**,
mismo número de funciones por debajo del 100 %, y `lcfix.py zEAXSound2` dice
«todas las entradas @lc están al día».

Las **9 filas que quedan** son un solo desplazamiento: `offset` va en **r0** en
el original y en **r9** en el nuestro (y de rebote el `li 0` del
`m_SubMixChannelsAdded = 0` y el `lwzx r30` con los operandos al revés). Barrido
alrededor de eso, todo sobre esta base:

| # | qué | % |
|---|---|---|
| V1 | el `cse_temp` de arriba | **99,28571** (9 filas) |
| V2 | una sola `numin` (el `else` lee el campo en línea) | 96,64286 |
| W1 | `int offset = this->m_pMMStateHdr->OffsetSubMixData;` en la declaración | 99,28571 idéntico |
| W2 | `m_SubMixChannelsAdded = 0;` **antes** de leer `offset` | 96,96429 |
| W3 | `(n + …pSubMixChProcs)->pMixChData_S` (conmutar el `PLUS`) | 99,28571 **idéntico** |

**W3 vale para todo el proyecto**: conmutar la suma en la fuente **no** cambia el
orden de operandos que emite el `lwzx`. GCC 2.9 canonicaliza el `PLUS_EXPR` en el
árbol, así que el `lwzx r30, r11, r10` contra `lwzx r30, r10, r11` **no se ataca
desde la fuente**. (Eso cierra de paso la idea equivalente en
`DynamicLoader::Initialize`, §5.)

---

## 2. Dos hallazgos de método

### 2.1 `{}` en el volcado DWARF NO significa «el original no tiene locales»

`symbols/mw_dwarfdump.nothpp:488894` dice literalmente:

    void EAXAemsManager::ResolveCurrentDataMemory() {}

Nuestra versión tiene tres locales (`mgr`, `curLoad`, `MemLocation`). La lectura
obvia —«el original no las tiene, quítalas»— es **falsa y cuesta puntos**:

| # | qué | % |
|---|---|---|
| base | las tres locales | **90,23810** |
| R1 | **ninguna** local, `gAEMSMgr.m_pCurLoadSDLP->…` en todas partes | **mucho peor**: desaparece `addi r31, r9, gAEMSMgr@l`, pasa a `@ha/@l` por uso, +4 filas y el marco baja de 0x10 a 0x8 |
| R2 | sin `curLoad` (quedan `mgr` y `MemLocation`) | 90,23810 **idéntico** |
| R3 | temporal `void *pmem` delante de las dos llamadas a método | 90,23810 **idéntico** |

Ese `{}` quiere decir **«ninguna de sus locales tiene localización DWARF»**
—igual que `pHP` y `h` en `DynamicLoader::Initialize`, que salen con `-` en las
DOS columnas de `regmap`—. Una local plegada a una constante o a una dirección de
símbolo no aparece. **`regmap` sólo compara las locales que el volcado lista; las
que no lista no son evidencia de nada.**

### 2.2 El permutador no ve los inlines, y por eso `MsgPlayMiscSound` era imposible

`SFX_Common::MsgPlayMiscSound` (780 B, 99,40513 %) es el PERMUTADOR más limpio
del triaje: 28 diferencias y **todas** de registro. Le pasé el catálogo entero en
ciego (64 variantes de un solo cambio, 18,6 s cada una): **score 0,856410 antes y
después, ninguna mejora.**

El motivo está a la vista en el diff: **las 28 diferencias no están en el cuerpo
de `MsgPlayMiscSound`. Están en el constructor `Csis::FX_UVES`, que se expande
inline** (`MAIN_AEMS.h:1628`). El cuerpo son cuatro `new` y un `switch`; todo lo
demás lo pone el constructor, y **el permutador sólo permuta el cuerpo de la
función que le pasas**.

Regla: **antes de lanzar el permutador, mira si las filas que fallan llevan
símbolos de OTRA función.** Si el diff vive dentro de un inline, el permutador es
tiempo tirado.

Y lo he cerrado a mano, que es el resultado que de verdad importa aquí:

**X1** — invertir el constructor a `SetID(iD); SetVolume(volume);` (o sea,
deshacer el commit `5d1ac0cf`, que hizo lo contrario y valió 97,26 → 99,41):

- **arregla ENTERO el primer sitio de expansión** (`FX_UVES(0,0,0,0,0,0)`, del
  `case 1`): la primera divergencia se va de la fila ~31 a la **109**;
- **y rompe los otros dos** (`FX_UVES(2,…)` y `FX_UVES(1,…)`, `case 3`/`case 4`):
  de 28 filas a **30**, y el veredicto empeora de `regs=28` a
  `regs=26 falta=2 sobra=2`. Sin daño colateral en zEAXSound. **REVERTIDO.**

**Lo que eso demuestra**: el objetivo emite el MISMO constructor inline con
**dos órdenes de almacenamiento distintos** según el sitio —`stop, iD, volume, …`
donde `iD` vale 0 y `volume, iD, stop, …` donde vale 2 o 1—. Como el orden de
sentencias de la fuente es uno solo, **ninguna forma del constructor puede
satisfacer a los tres sitios**: lo decide el planificador con la presión de cada
sitio. `MsgPlayMiscSound` no se cierra por la fuente del constructor.

---

## 3. Las cinco funciones localizadas al byte

Estas cinco ya no necesitan lectura: sé qué instrucción falla y dónde.

### 3.1 `CARSFX_PreColWoosh::MsgBarrier` — 140 B, 94,28571 %, **UNA instrucción**

    obj:   addi r3, r10, 0x3c ; li r4, 0x1 ; stfs f13, 0x34(r10) ; fmr f2, f1
    nos:   addi r3, r10, 0x3c ; stfs f13, 0x34(r10) ; li r4, 0x1 ; fmr f2, f1

`li r4, 0x1` es el tercer argumento de
`WooshFadeOut.Initialize(1.0f, 1.0f, 1, LINEAR)` y el `stfs` es
`this->mDurationActive = 0.0f;`. Independientes: empate del planificador.

- **M1** intercambiar `bGoingToCollide = true;` y `mDurationActive = 0.0f;`:
  **77,14286 %** (MUCHO PEOR; se descoloca todo el bloque de argumentos).
- **Permutador ciego, 22 variantes de un cambio: ninguna mejora.**

### 3.2 `Speech::RoadblockFlow::Setup` — 596 B, 98,25503 %, **UNA instrucción**

    obj:   lwz r9,0(r31) ; lwz r0,0x2e4(r9) ; lha r3,0x2e0(r9) ; mtlr r0 ; add r3,r31,r3
    nos:   lwz r9,0(r31) ; lha r3,0x2e0(r9) ; lwz r0,0x2e4(r9) ; add r3,r31,r3 ; mtlr r0

Es el thunk de una llamada virtual (0x2e0 = delta, 0x2e4 = puntero). **El mismo
patrón ochenta filas antes (0x2c8/0x2cc, filas 54-59) casa exacto**, así que no
es la forma de la llamada: es la presión de registros del bloque. No tocada.

### 3.3 `EAXAemsManager::ResolveCurrentDataMemory` — 168 B, 90,23810 %, **DOS**

Las dos son el mismo patrón, un `lis` adelantado:

    obj:   lwz r4, 0x28(r9) ; lis r3, TheTrackStreamer@ha ; addi r3, r3, …@l ; bl
    nos:   lis r3, TheTrackStreamer@ha ; lwz r4, 0x28(r9) ; addi r3, r3, …@l ; bl

(ídem con `gAudioMemoryManager@ha`). Empate exacto de camino crítico:
`lwz`(latencia 2)+`bl` = `lis`+`addi`+`bl` = 3, y lo desempata el `INSN_LUID`, o
sea el orden de emisión de los argumentos de la llamada a método. Barridos R1, R2
y R3 de §2.1: uno mucho peor y dos idénticos.

### 3.4 `EvalSQT__…FnStatelessQ` — 1480 B, 99,45946 %, **DOS instrucciones**

Confirmo el diagnóstico de la r21 sin gastar ensayos: son los dos
`clrlwi r0, r0, 16` de las filas **54 y 161**, detrás de cada `subi r0, r5, 1`, y
vienen del `static_cast<unsigned short>(statelessQ->mNumKeys - 1)` de las líneas
74 y 132. `regmap` dice **IDÉNTICO** (26 locales, mismo árbol de bloques, mismo
reparto): **no hay una local detrás**.

**El frente está CERRADO y lo he respetado**: la r21 barrió 22 formas de esa
sentencia (todas peores o idénticas) y el `brief-r27` cerró el mecanismo
(«`combine` funde sólo si el pseudo es de un solo `set` y viene de memoria; el de
los guardias lo crea PRE con cinco»).

**Corrección al encargo**: lo señalaba como «el más limpio del triaje» y como el
patrón «código defensivo de más». **No lo es.** Las dos instrucciones que sobran
no son un `if` defensivo: son la máscara de un cast, y el cast es el óptimo local
medido —quitarlo hace que GCC extienda la ENTRADA en vez de la salida y cuesta
1,3 puntos—.

### 3.5 `FnRunBlender::FindMatchTime` — 720 B, 97,36111 %

`regmap`: **IDÉNTICO** (15 locales, mismo reparto). El objetivo carga el literal
doble `lbl_803D4730` (la constante 0x4330…8000… de la conversión int→double) en
las filas 26-32 y nosotros doce filas más tarde (38-39). Otro adelanto de sched
sin local detrás. No tocada.

---

## 4. `GenerateRoadNoise`: la veda que contradice al DWARF

`CARSFX_RoadNoise::GenerateRoadNoise` (1240 B, 97,11290 %) es el único de mi
reparto con diagnóstico estructural de `regmap`:

    slipBoost    (nada en el original)   f7 en el nuestro    <-- SOLO NUESTRA

El original tiene seis locales (`fRightVol`, `fLeftVol`, `fRightPitch`,
`fLeftPitch`, `speed`, `ftemp`) y **no tiene `slipBoost`**. La regla del proyecto
(«la local que roba el registro») dice que eso va primero. **Aquí es falso, y lo
he medido en las tres posiciones posibles de la sentencia**:

| # | qué | % | regmap |
|---|---|---|---|
| base | `float slipBoost = 0.1f;` entre las dos mitades del `bLength` | **97,11290** | ESTRUCTURA (1 local de más) |
| e1 | literal `0.1f` en el sitio de `slipBoost` | 96,60968 | **IDÉNTICO** |
| e2 | literal, y la sentencia **antes** del `bLength(izquierdo)` | 96,10968 | — |
| e3 | e2 + `ftemp = bLength(…) * 0.01f` fundido en una sentencia | 96,10968 | — |

**Quitar la local que el DWARF no tiene deja el diagnóstico perfecto y cuesta
medio punto.** Primer contraejemplo medido de esa regla que veo anotado: `regmap`
pasa a IDÉNTICO y el diff sube de 23 a 29 filas.

Lo que de verdad falla, localizado: los dos `fmadds` de `fRightVol`
(`fRightVol = fLeftVol + fLeftVol*ftemp` y `fRightVol += fRightVol*0.1f`) salen
en las filas 102 y 115 del objetivo y en las 114 y 134 nuestras — el objetivo los
adelanta por encima de la cadena `frsqrte` del segundo `bLength`. **REVERTIDO.**

---

## 5. `DynamicLoader::Initialize`: dos pares de registros ACOPLADOS

2352 B, 98,87585 %. `regmap` da el diagnóstico más rico de las cuatro unidades:

    t             original r29   nuestro r30    <-- INTERCAMBIADOS
    nameLength    original r30   nuestro r29
    sym           original r4    nuestro r30
    sheader / e / p    sin localización en el original, r30 / pila en el nuestro

- **Ensayo A**: `strcpy(&s[nameLength] + 1, typebuf)` en vez de
  `strcpy(&s[nameLength + 1], typebuf)` → **98,70238 %**.
  **Arregla el par de abajo**: las filas 422-428 pasan a casar exactas (`t`=r29,
  `nameLength`=r30, como el objetivo) y aparece el `add r29, r28, r30` +
  `addi r3, r29, 1` en el orden bueno. **Pero rompe el par de arriba**: `&s[2]` y
  `type_separator` se intercambian (r29/r30 → r30/r29) y salen cinco filas nuevas
  (409-416). De 22 filas a 24. **REVERTIDO.**
  **Los dos pares están acoplados**: darle a `nameLength` un uso más sube su
  prioridad de asignación por encima de la de `t` y arrastra al par anterior.
- **Ensayo B**: reordenar las declaraciones del bloque interno al orden del DWARF
  del original (`char *t; char typebuf[128]; char *type_separator = …;` en vez de
  `type_separator; typebuf; t`) → **98,87585 %, byte a byte idéntico**. GCC 2.9
  no usa el orden de declaración para desempatar aquí. **REVERTIDO.**

Lo que queda, y no he tocado por veda (vedas 4 y 5 de `r20-muros.md`): el árbol
del `switch` de `sh_type` (filas 349-362; el objetivo compara `r21`, `r22`, `9`,
`3`, `3`, `1` y nosotros `3`, `3`, `2`, `r21`, `r22`, `9`, con un `b` de más).

Y **la pista de `sym` queda descartada por W3**: el objetivo direcciona el array
como `(i*16) + symtab` y nosotros como `symtab + (i*16)` (cuatro filas `lwzx`,
`add`, `stwx`, `lwzx` con los operandos al revés), pero conmutar la suma en la
fuente **no cambia nada** —medido en `CreateSubMixChannels`, §1—.

---

## 6. Permutadores: cuatro pasadas, un premio

| función | unidad | modo | variantes | resultado |
|---|---|---|---|---|
| `NFSMixMapState::CreateSubMixChannels` | zEAXSound2 | **ciego** | 221 + 227 | **0,809524 → 0,892857** (`cse_temp:numin`) → **APLICADO**, 98,690 → 99,286 % |
| `SFX_Common::MsgPlayMiscSound` | zEAXSound2 | **ciego**, catálogo entero | 64 | ninguna mejora (score 0,856410) |
| `CARSFX_Turbo::ProcessUpdate` | zEAXSound2 | guiado (`TmpBlowoffVol`) | 60 | ninguna mejora (score 0,964286) |
| `CARSFX_PreColWoosh::MsgBarrier` | zEAXSound2 | **ciego** | 22 | ninguna mejora (score 0,971429) |
| `startnextrequest` | zEAXSound | guiado (3 registros) | 70 + 68 | **falso positivo**: score 0,797753 → 0,866667 pero **90 instrucciones contra 89** y `opcodes` 0,9775 → 0,9444 |

`regmap` decía **IDÉNTICO** en `MsgPlayMiscSound`, `MsgBarrier` y
`CreateSubMixChannels`, así que fui en ciego con el catálogo entero (regla de
`nfsmw-permutador-guiado`); en `ProcessUpdate` y `startnextrequest` decía REPARTO
y fui guiado. **Y la de premio fue precisamente una de las ciegas.**

Dos avisos de herramienta:

1. **`permuter.py zEAXSound "cStichWrapper::Play"` resuelve la sobrecarga
   equivocada** —coge `Play(int,int,int)`, de 13 instrucciones, y anuncia
   `score=1.000000`—. La que falta es `Play(const SND_Params*)`, 384 B al
   99,688 %. Hay que pasarle `--symbol Play__13cStichWrapperPC10SND_Params`.
2. **El `score` del permutador premia filas que casan aunque el tamaño crezca.**
   En `startnextrequest` el «ganador» mete una instrucción de más (+4 B sobre un
   objetivo de 356 B): imposible que cierre. **Hay que mirar `insn` y `opcodes`,
   no sólo `score`.**

---

## 7. `startnextrequest`: el original es `static` y el nuestro no

356 B, 94,30337 %. El volcado (`symbols/mw_dwarfdump.nothpp:499994`) dice:

    static void startnextrequest(struct STREAMHEADERtag * strm /* r31 */, int priority /* r27 */)

y `regmap` lo canta: `original: static void …` / `nuestro: void …`. Las ocho
llamadas están todas en `stream.cpp`, así que la firma del original es
alcanzable.

- **S1** `static` en la declaración (línea 169) y en la definición (línea 465):
  **94,30337 %, mismo diff exacto**. En C++ el mangling de GCC 2.9 no cambia
  (`startnextrequest__FP15STREAMHEADERtagi` sigue igual) y el reparto tampoco.
  **Neutro.** Lo dejo anotado: es una corrección estructural gratis, pero cambia
  el binding del símbolo de GLOBAL a LOCAL y no me toca a mí verificar el DOL.
  **REVERTIDO.**
- **S2** orden de declaración según el DWARF (`req; lockstate; nopendingrequest`
  en vez de `req; nopendingrequest; lockstate`): **neutro**. **REVERTIDO.**

Lo que queda es una rotación limpia de tres registros
(`priority` r27→r28, `req` r28→r29, `nopendingrequest` r30→r27) más dos empates
de sched, y el permutador guiado no la mueve (§6).

---

## 8. `SetupNextLoad` (1008 B, 98,67063 %): sin DWARF y con el reparto rotado

**No está en el volcado del original** —ni la definición ni una entrada vacía;
hay un hueco entre `DataLoadCB` (488881) y `ResolveCurrentDataMemory`
(488894)—, así que `regmap` no puede decir nada. El diff es una rotación de todo
el marco:

    this            objetivo r29   nuestro r27
    contador        objetivo r26   nuestro r31
    contador+1      objetivo r28   nuestro r29
    contador*4      objetivo r27   nuestro r28

más el `lis r26, gFastMem@ha` dos ranuras adelantado. El cuerpo fuente son trece
líneas: los 1008 B los ponen `RemoveBankListing` y
`mWaitForResolve.DeleteRefToAsset` expandidos inline, así que —igual que en
`MsgPlayMiscSound`, §2.2— **permutar el cuerpo de `SetupNextLoad` no toca ninguna
de las instrucciones que fallan.** No tocada.

---

## 9. Lo que de verdad bloquea a estas cuatro unidades: los DATOS

Ninguna de mis unidades promociona aunque cierre todas sus funciones.
`datacmp.py`:

| unidad | .data | .rodata | .bss |
|---|---|---|---|
| zEagl4Anim | 1207/1344 (**89,8 %**) | 5391/7760 (**69,5 %**) | −148 |
| zEAXSound | 883/1908 (**46,3 %**) | 5616/12824 (**43,8 %**) | +296 |
| zEAXSound2 | 5573/10236 (**54,4 %**) | 10209/18056 (**56,5 %**) | −48 |
| zSpeech | 807/1496 (**53,9 %**) | 5593/12712 (**44,0 %**) | +244 |

La primera diferencia de `.data` está en +0x4 (zEAXSound), +0x9 (zEAXSound2) y
+0xC (zSpeech) — casi al principio. `datadiff.py zSpeech` enseña que es
composición: **sobran** cadenas (`GAMECUBE`, `Attrib::Gen::emitterdata`,
`Attrib::Gen::emittergroup`, `Attrib::Gen::gameplay`) y **faltan** otras
(`gameplay`, `Attrib::Gen::…`); zSpeech tiene además **+1008 B de `.data`**. Eso
es orden/contenido de SourceList, no la fuente de estas diecisiete funciones.

**No he tocado `splits.txt` ni `configure.py`.** Tampoco me he topado con el cruce
zEAXSound/zEAXSound2 que avisaba el encargo (todo mi trabajo fue en fuente).

Aparte, objdiff marca al 66,667 % los ~230 símbolos `_4Csis.<nombre>Id` de 8 B
(`InterfaceId = {"cadena", crcSistema, crcInterfaz}`) de zSpeech y zEAXSound2, y
al 85,714 % los `s_TypeInfo`/`s_StateInfo` de 16 B. Como el primer campo es un
puntero a rodata anónima, **antes de dar eso por trabajo hay que comprobarlo con
`ppc.calculatePoolRelocations=false`** (nota `nfsmw-reubicaciones-fantasma`): muy
probablemente sea el comparador y no el dato.

---

## 10. Vedas que dejo medidas (no las repitáis)

1. `GenerateRoadNoise`: **quitar la local `slipBoost`** que el DWARF no tiene, en
   las tres posiciones posibles de la sentencia (97,11 → 96,61 / 96,11 / 96,11).
2. `GenerateRoadNoise`: fundir `ftemp = bLength(…); ftemp = ftemp * 0.01f;` en una
   sola sentencia (96,11).
3. `DynamicLoader::Initialize`: `&s[nameLength] + 1` (98,88 → 98,70; arregla un
   par de registros y rompe el anterior — **están acoplados**).
4. `DynamicLoader::Initialize`: orden de declaración `t; typebuf; type_separator`
   según el DWARF (idéntico).
5. `MsgBarrier`: intercambiar `bGoingToCollide` y `mDurationActive` (94,29 →
   77,14).
6. `ResolveCurrentDataMemory`: sin ninguna local (mucho peor), sin `curLoad`
   (idéntico), con temporal `void *pmem` (idéntico).
7. `startnextrequest`: `static` (neutro) y orden de declaración según el DWARF
   (neutro).
8. `FX_UVES`: `SetID` antes de `SetVolume` (deshacer `5d1ac0cf`) — arregla el
   primer sitio de expansión y rompe los otros dos, 28 → 30 filas. **Y demuestra
   que ninguna forma del constructor sirve para los tres sitios.**
9. `CreateSubMixChannels`: una sola `numin` (96,64), `m_SubMixChannelsAdded = 0`
   delante (96,96), `offset` inicializado en la declaración (idéntico).
10. **Conmutar un `PLUS` en la fuente NO cambia el orden de operandos del `lwzx`**
    (`(n + p)` contra `(p + n)`: byte a byte idéntico).
11. Permutador de **un solo cambio, catálogo completo**, agotado sin mejora en
    `MsgPlayMiscSound` (64), `ProcessUpdate` (60), `MsgBarrier` (22) y
    `startnextrequest` (138, y el «ganador» crece 4 B).

## 11. Lo que dejo servido para el siguiente

- **`CreateSubMixChannels`** está a **9 filas**, y las nueve son el mismo
  desplazamiento (`offset` r0 contra r9). Merece un permutador con `--seed` desde
  el cuerpo nuevo, o `--depth 2`.
- **`MsgPlayMiscSound` y `SetupNextLoad`**: el diff vive en un inline
  (`Csis::FX_UVES::FX_UVES` y `RemoveBankListing`/`DeleteRefToAsset`). Permutar el
  inline, no la función — y en el primero el orden de sentencias ya está
  descartado (§2.2).
- **`Initialize`**: sólo queda el árbol del `switch` (veda de la r20). El resto
  del diff está explicado.
- **`MsgBarrier` (140 B) y `RoadblockFlow::Setup` (596 B)**: una instrucción cada
  una, empate de `sched`, catálogo de un cambio agotado en la primera. Son los
  dos mejores candidatos del reparto para probar profundidad 2.
- **`FindMatchTime`, `EvalState__…FnRawStateChan`, `Play__…CARSFX_RoadNoise`,
  `BindToData`, `LoadSpeechBank`, `Play__13cStichWrapper`**: cero ensayos míos
  esta ronda.
