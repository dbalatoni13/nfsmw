# Ronda 36d — `c36d-fe`: frontend y sonido

**Tres funciones cerradas, +1.488 B de código casado, y una palanca NUEVA que
explica las tres y que vale para todo el árbol.**

    zEAXSound    EAXAemsManager::ResolveCurrentDataMemory   168 B   90,238 % -> 100 %
    zFe          MemcardCallbacks::FoundEntry               512 B   97,969 % -> 100 %
    zFeOverlay   CustomizeMain::NotificationMessage         808 B   97,896 % -> 100 %

Verificado con `pctsnap` símbolo a símbolo sobre las **7 unidades del territorio
(5.419 funciones)**: `EMPEORAN: ninguna`, y las únicas tres que se mueven son
esas. `linkdelta` deja **`.text +0`** en zFe, zFe2, zFeOverlay, zEAXSound y
zEAXSound2 (las cinco que toco); `lcfix.py --check` limpio; `undlist` sigue a
**0 símbolos por definir**. Tres ficheros tocados, **sin commit**:

    src/Speed/Indep/Src/EAXSound/EAXAemsManager.cpp
    src/Speed/Indep/Src/Frontend/MemoryCard/MemoryCardCallbacks.cpp
    src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.cpp

---

## 1. La palanca: **la barrera selectiva POR ARGUMENTO** (quinto uso)

`asm("" : "+r"(x))` tenía cuatro usos catalogados (adelanto de sched,
coalescing, hundimiento interbloque, plegado de CSE). Hay un **quinto**, y es el
que ha pagado esta ronda:

> **Una barrera selectiva sobre CADA argumento de una llamada fija el orden en
> que se emite la preparación de argumentos.** No emite un byte y no cambia el
> tamaño.

La **firma** es inconfundible en el diff y la vi en cinco funciones distintas del
territorio: *el `lis @ha` de una dirección (un literal, un objeto global, un
hash) sale una o dos ranuras antes o después de lo que debe, respecto al `lwz` /
`mr` de otro argumento de la misma llamada.*

    obj:   bl GetScreen / lwz r4,0(r31) / lis r5,LC@ha / mr r6 / mr r7 / addi r5,r5,LC@l / bl
    ours:  bl GetScreen / lis r5,LC@ha  / lwz r4,0(r31) / addi r5,r5,LC@l / mr r6 / mr r7 / bl

### 1.1 Lo primero que hay que saber: **la fuente NO lo mueve**

En `ResolveCurrentDataMemory` barrí **cinco formas de fuente** para las dos
llamadas afectadas y las **cinco dan el binario IDÉNTICO** (168 B, 6 diffs,
90,238 %):

| # | forma | % |
|---|---|---|
| base | `TheTrackStreamer.FreeUserMemory(curLoad->pmem)` | 90,23810 |
| W1 | `void *m = curLoad->pmem;` y pasar `m` | 90,23810 (idéntico) |
| W3 | `TrackStreamer *ts = &TheTrackStreamer; ts->…` | 90,23810 (idéntico) |
| W4 | lo mismo con referencia | 90,23810 (idéntico) |
| W6/W7 | las dos locales, en los dos órdenes | 90,23810 (idéntico) |
| W5 | leer `pmem` una vez antes del `switch` | 91,190 %, **164 B** (CSE se come una carga) |

O sea: **el orden lo decide `expand_call`, no el `;` del fuente**. Por eso
`r36c-fe` había dado `FoundEntry` por «empate de emisión» tras probar el literal
en local, `GetScreen()` en local, `info->mName` en local y **el permutador ciego
con las 114 variantes del catálogo**: ninguna de esas herramientas toca este eje.

### 1.2 Lo que sí lo mueve

    void *m = curLoad->pmem;
    __asm__("" : "+r"(m));          // <- la barrera, sobre el ARGUMENTO
    TheTrackStreamer.FreeUserMemory(m);

| # | qué | % | difs |
|---|---|---|---|
| base | — | 90,23810 | 6 |
| **R1** | **local + `asm("" : "+r"(m))`** | **95,11905** | **3** |
| R2 | R1 + clobber `"memory"` | 95,11905 | 3 |
| R3 | local + `asm("")` (barrera TOTAL) | 90,23810 | 6 (nada) |
| R4 | local + `asm("" : "=r"(m) : "0"(m))` | 95,11905 | 3 |
| R5 | `register void *m asm("r4")` (pin solo) | 90,23810 | 6 (nada) |
| R6 | pin + barrera | 95,11905 | 3 |
| **S2** | **R1 en los DOS sitios de la función** | **100** | **0** |

**La barrera total no sirve y el pin tampoco: sólo la selectiva.** Y hay que
ponerla en **todos** los sitios de la función, no en uno.

### 1.3 La regla fina que costó tres ensayos: **un `asm` por argumento**

En `FoundEntry` (`GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault)`)
la barrera sobre un solo argumento **empeora**; hacen falta las tres, **y en
`asm` separados**:

| # | qué | % | tam | difs |
|---|---|---|---|---|
| base | — | 97,96875 | 512 | 3 |
| F1 | `nm` + barrera, sin cachear `GetScreen()` | 95,77344 | **516** | 15 |
| F2 | ídem con `"=r"/"0"` | 95,77344 | 516 | 15 |
| F3 | ídem + clobber `"memory"` | 95,77344 | 516 | 15 |
| F4 | F1 sin la local muerta `sec` | 94,21094 | **508** | 18 |
| **F5** | **`scr = GetScreen()` + `nm` + barrera** | **98,43750** | 512 | 2 |
| G1 | F5 + barrera en `iGuessSize` | 98,43750 | 512 | 2 |
| G2 | F5 + barrera en `fDefault` | 98,28125 | 512 | 4 |
| **G3** | **F5 + barrera en `iGuessSize` Y en `fDefault`** | **100** | 512 | **0** |
| G4 | G3 con el literal `""` en una local | 98,43750 | 512 | 2 |
| G5 | G4 + barrera también al literal | 97,96875 | 512 | 3 |
| G6 | la barrera de `nm` repetida dos veces | 98,43750 | 512 | 2 |
| G7 | G3 con barrera también en `scr` | 98,43750 | 512 | 2 |
| H1 | G3 sin cachear `GetScreen()` | 95,30469 | **516** | 27 |
| **H2** | **G3 con UN solo `asm` de tres operandos** | **98,43750** | 512 | 2 |
| H3 | G3 sin la barrera de `nm` | 98,43750 | 512 | 2 |

**H2 es el dato de método**: `asm("" : "+r"(nm), "+r"(iGuessSize), "+r"(fDefault))`
**no equivale** a tres `asm` separados. Un `asm` es UNA insn en el DAG: da un
punto de orden, no tres. Las tres barreras hacen falta *y* el objeto (`scr`)
tiene que estar cacheado en una local, o la función engorda 4 B.

Receta final aplicada:

```c
UIMemcardBase *scr = GetScreen();
const char *nm = info->mName;
__asm__("" : "+r"(nm));
__asm__("" : "+r"(iGuessSize));
__asm__("" : "+r"(fDefault));
scr->AddItem(nm, "", iGuessSize, fDefault);
```

### 1.4 Y el aviso: quita el `lis` de en medio, **no** arregla el reparto

En `NotificationMessage` la barrera cerró las 4 filas del par `lis/ori` del hash
`0x6d5d86a1` pero dejó intactas las 5 del intercambio `r31`↔`r28`. Son dos
problemas distintos y hacen falta dos palancas (§2).

---

## 2. `NotificationMessage` (808 B, 97,896 % -> 100 %): dos palancas

**Palanca A — barrera por argumento** en la rama `else` del `case 0x911ab364`
(`cFEng::Get()->QueuePackageMessage(hash, GetPackageName(), nullptr)`):

| # | qué | % | difs |
|---|---|---|---|
| base | — | 97,89604 | 9 |
| **C1** | barrera sobre `pkg = GetPackageName()` | **98,88614** | 7 |
| C2 | C1 + barrera sobre el hash | 97,84654 | 10 |
| C3 | `fe` local + barreras de `pkg` y hash | 97,89604 | 9 |
| C4 | barrera sólo sobre el hash | 97,72277 | 12 |
| C5 | `fe` local + barreras de los tres args | 96,60891 | 12 |
| C6 | `fe` local + barrera de `pkg` (sin barrera de `fe`) | 98,88614 | 7 |
| **C7** | **`fe` local + barrera de `fe` + barrera de `pkg`** | **99,87624** | **5** |
| C8 | C1 con la barrera de `pkg` repetida | 97,77228 | 12 |
| C9 | C1 + barrera sobre el `nullptr` | 97,59901 | 10 |

El destinatario importa: **la barrera va sobre `this` (`fe`) y sobre el
argumento que llega tarde (`pkg`), y NO sobre el hash** —el hash es justo lo que
queremos que se hunda, y atarlo lo fija donde está—.

**Palanca B — `n_refs` sobre el puntero al manager.** Quedaba el intercambio
`&gCarCustomizeManager` (r31 en el objetivo, r28 nuestro) contra el temporal de
`CustomizeIsInBackRoom()`. `r36c-fe` había medido que el **pin a r31
MISCOMPILA**. La palanca buena es la de `n_refs` de `nfsmw-barrera-selectiva`:

```c
CarCustomizeManager *mgrp = &gCarCustomizeManager;
__asm__("" : "+r"(mgrp));          // sube n_refs de 3 a 4: NO emite un byte
CarCustomizeManager &mgr = *mgrp;
```

| # | qué | % | difs |
|---|---|---|---|
| C7 | (referencia) | 99,87624 | 5 |
| **E1** | **C7 + `mgrp` con UN `asm`** | **100** | **0** |
| E2 | C7 + `mgrp` con dos `asm` | 100 | 0 |
| E3 | C7 + `mgrp` sin `asm` | 99,87624 | 5 |
| E6 | C7 + `mgrp` con tres `asm` | 99,85149 | 6 |
| E7 | C7 + usar `mgr` también en el `if` interior | 99,87624 | 5 |

La cuenta cuadra exacta con `allocno_compare`: `&gCarCustomizeManager` tenía
3 refs y rango 64..114; el temporal, 2 refs y rango 70..99. Con
`pri = floor_log2(n_refs)·n_refs/live_length`, 3 refs dan `1·3/49 = 0,061` y el
temporal `1·2/29 = 0,069` — pierde por poco. **Con la cuarta referencia
`floor_log2` salta de 1 a 2** y el numerador pasa de 3 a 8: `0,163`, gana, y sale
el reparto del objetivo. **Es el escalón de `floor_log2` lo que decide, no el
+1.** Por eso E2 (dos asm, 5 refs) también cierra y E6 (tres, 6 refs) ya rompe
otra cosa.

**Nota importante frente a r36c-fe**: el diagnóstico de esa ronda («al original
le sobra alguna referencia a `gCarCustomizeManager`») era **correcto**. Lo que
faltaba era la forma de añadir esa referencia sin emitir código; el pin no lo
era.

---

## 3. Lo que queda a un paso, con la receta medida (NO aplicado)

`matched_code` es todo-o-nada, así que dejar una función al 99,9 % no aporta un
byte y sí deja deuda de `asm`. Las dejo **revertidas** con la receta exacta para
que la ronda siguiente arranque desde ahí y no desde cero.

### 3.1 `CustomizeMain::SetScreenNames` (zFeOverlay, 440 B): 91,727 % -> **98,000 %**, 6 difs

**El `+m` sobre un global ordena SU store sin ser barrera total.** El objetivo
suelta el primer store de la rama `else` inmediatamente (`r36c-fe` §
«SetScreenNames»); una barrera de memoria sobre ese único global lo consigue:

```c
g_pCustomizeSubPkg = "CustomizeCategory.fng";
__asm__("" : "+m"(g_pCustomizeSubPkg));
g_pCustomizeSubTopPkg = "CustomizeGenericTop.fng";
```

| # | qué | % | difs |
|---|---|---|---|
| base | — | 91,72727 | 23 |
| **D1** | **`+m` tras la PRIMERA asignación** | **98,00000** | **6** |
| D2 | D1 + clobber `"r0"` | 98,00000 | 6 |
| F1 | `+m` tras las dos primeras | 91,63636 | 26 |
| F2 | `+m` repetido tras la primera | 97,72727 | 10 |
| F3 | `+m` sólo tras la segunda | 98,00000 | 6 |
| F4 | D1 con clobber `"memory"` | 97,13636 | 22 |
| F5 | D1 con clobber `"r30"` | 97,45454 | 18 |
| F6 | D1 + `+m` tras la tercera | 91,63636 | 26 |

Contrasta con lo que anotó `r36c-fe` en el propio fuente: **la barrera TOTAL
`asm("")` ahí daba 94,500 %**. El `+m` selectivo da 98,000. Es la diferencia
entre ordenar *todos* los stores y ordenar *uno*.

Lo que queda son **6 filas de reparto**: el objetivo mete
`&g_pCustomizeSubPkg` en **r3** (volátil, muere en el store) y `&…SubTopPkg` en
r30; nosotros al revés (r30 y r9), y con eso el
`addi r3, r3, gCarCustomizeManager@l` se hunde tres ranuras. No hay nombre de
fuente al que pinchar: son las direcciones `@ha` de once globales.

### 3.2 `NFSMixMapState::CreateSubMixChannels` (zEAXSound2, 336 B): 99,286 % -> **99,881 %**, 1 dif

Tres pines, los tres señalados por `regmap` (`offset` r0 vs r9, `numin` r11 vs
r9, y una local `nextin` que sólo tenemos nosotros):

```c
register int offset asm("r0");                       // (1)
register int zeroAdded asm("r10") = 0;               // (2)  m_SubMixChannelsAdded = zeroAdded;
register int numCh asm("r0") = this->m_pSubChHdr->NumMixChannels;   // (3)
```

| # | qué | % | difs |
|---|---|---|---|
| base | — | 99,28571 | 9 |
| N1/N2 | quitar la local `nextin` que `regmap` marca como sólo nuestra | 98,69048 | 16 |
| **P1** | pin `offset` a r0 | **99,64286** | 5 |
| P2 | pin `numin` a r11 | 99,16666 | 11 |
| P3 | P1 + P2 | 99,52381 | 7 |
| P4 | pin `nextin` a r11 | 99,28571 | 9 (nada) |
| **P6** | P1 + pin del cero de `m_SubMixChannelsAdded` a r10 | **99,76190** | 3 |
| **Q2** | **P6 + pin de `NumMixChannels` a r0** | **99,88095** | **1** |
| Q4 | Q2 + puntero en local | 99,88095 | 2 |

La fila que queda es **el orden de operandos de un `lwzx`**:

    obj:   lwzx r30, r11, r10        (puntero, índice)
    ours:  lwzx r30, r10, r11        (índice, puntero)

sobre `(this->m_pFirstInstance->m_MixStateParams.pSubMixChProcs + n)->pMixChData_S`.
**Nueve formas de fuente dan exactamente el mismo `lwzx`**: `n + ptr`,
`ptr[n]`, `procs[n]` con local, `&ptr[n]`, `p += n`, `int idx = n`, barrera
sobre el puntero, `(char*)ptr + n*sizeof(...)` y `+ (unsigned)n`. El orden del
`PLUS` en el direccionamiento indexado **no se alcanza desde la fuente** — es la
misma familia que la veda de `(a+C)+b` de `r36c-fe` §1, pero al revés: allí la
fuente mandaba y aquí no.

---

## 4. Vedas nuevas, todas medidas

### `CARSFX_PreColWoosh::MsgBarrier` (zEAXSound2, 140 B, 2 difs) — **16 ensayos, ninguno mejora**

Un solo intercambio: el objetivo emite `li r4,0x1` (3.er argumento de
`WooshFadeOut.Initialize(1.0f,1.0f,1,LINEAR)`) **antes** del
`stfs f13, 0x34(r10)` de la sentencia anterior (`mDurationActive = 0.0f`), y
nosotros después.

    V1 stfs primero 77,143 %  |  V2 stfs tras la llamada 79,600 % y +16 B
    V3 local para el 1 94,286 (idéntico) | V4 pin r4 91,829 | V5/V6 pin+barrera 91,829
    V7 barrera sin pin, delante del store 90,857 | V8 barrera de ranura `+m` 76,971
    V9 local float cero 94,286 (idéntico)
    M1 barrera en `len` 90,857 | M2 `len`+`curve` 85,143 | M3 los cuatro args 85,143
    M4 objeto en local + `len` 90,857 | M5 objeto + los tres 85,143
    M6 objeto en local solo 94,286 (idéntico) | M7 barrera en `curve` 90,686

**Aquí la barrera por argumento NO vale**, y el motivo es el límite general de la
palanca: *una barrera sólo puede AÑADIR restricciones*. El objetivo **iza** el
`li r4` por encima de un store que viene de una sentencia anterior; para eso no
hay barrera. (En `FoundEntry` funcionaba porque lo que había que ordenar estaba
**dentro** de la misma llamada.)

### `Speech::RoadblockFlow::Setup` (zSpeech, 596 B, 3 difs) — barrera inerte

Las tres filas son el orden interno de las seis instrucciones que expande **una
sola llamada virtual** (`primary->PursuitApproaching()`): el objetivo carga el
puntero de función (`lwz 0x2e4`) antes del delta del thunk (`lha 0x2e0`), y
nosotros al revés. `asm("" : "+r"(primary))` delante, detrás, y `asm("")` total:
**binario idéntico** (98,255 %, 596 B, 3 difs) en los tres. Copiar `primary` a
otra local + barrera: 98,349 % pero **600 B**. Confirma la nota de
`nfsmw-barrera-selectiva` («`Setup`: no hay sitio»).

### `IconScroller::IconScroller` (zFe2, 384 B, 11 difs) — el `+m` lo destroza

Las 11 filas son la posición de **dos** `stfs f0` dentro de una tanda de trece:
`fCurFadeTime` (0xF8) va la **primera** en el objetivo y la penúltima en el
nuestro; `fCurrentAddPos` (0xF4) va la **última** en el objetivo y la primera en
el nuestro. Verificado que los desplazamientos de la cabecera son correctos.

    I1 `+m` tras `fCurFadeTime`                87,854 %  (33 difs)
    I2 `+m` tras las dos                       86,490 %  (28)
    I3 `+m` ANTES de `fCurrentAddPos`          97,771 %  (idéntico a base)
    I4 `+m` con clobber r0                     86,490 %  (37)

Suma a lo de `r36c-fe` (tres permutaciones de sentencias = misma emisión byte a
byte) y a la nota de memoria («`IconScroller`: la barrera rompe el CSE del
cero»). **La tanda de trece `stfs f0` es intocable desde el fuente.**

### `UIMemcardKeyboard::UIMemcardKeyboard` (zFe, 172 B, 19 difs) — cuatro formas, binario idéntico

Aunque el diff **tiene** la firma del §1 (`lwz r4, gButtonIDs@l` contra
`lwz r3, 0xc(r30)` cruzados), las cuatro variantes dan **exactamente el mismo
objeto** (172 B, 19 difs, 92,977 %): barrera sobre los `gButtonIDs[i]`, barrera
sobre paquete e id, barrera con los dos en el otro orden, y puntero a
`gButtonIDs`. El problema de fondo es el intercambio `this` r29↔r30, y encima el
objetivo **reutiliza el registro del `lis @ha` para el `addi @l`**
(`lis r30 / lwz r4,@l(r30) / addi r30,r30,@l`) mientras nosotros gastamos un r9
aparte. Es coalescing del asignador, no orden de argumentos.

### `FnStatelessQ::EvalSQT` (zEagl4Anim, 1480 B, 2 difs) — la veda de la r21 aguanta

Sobran exactamente dos `clrlwi r0, r0, 16` (8 B: 1488 contra 1480), los dos del
`static_cast<unsigned short>` de
`if (floorKey >= static_cast<unsigned short>(statelessQ->mNumKeys - 1))`
(líneas 74 y 132). **Quitar el cast no quita la instrucción: la MUEVE.** Sin
cast GCC emite `clrlwi r9, r5, 16` **antes** del `subi` en vez de después, y el
tamaño sigue siendo 1488 en las siete variantes:

    base (con el cast)              99,45946 %  1488 B   2 difs
    Z1 sin cast (1.er sitio)        98,13243 %  1488     12
    Z2 sin cast (los dos)           98,05135 %  1488     17
    Z3 `static_cast<int>(mNumKeys)-1`  98,05135 %  1488   17
    Y1 local `unsigned short nk`    98,05135 %  1488     17
    Y3 `floorKey + 1 >= mNumKeys`   97,31081 %  **1492** 36
    Y4 cast sólo al campo           98,05135 %  1488     17

El objetivo hace `subi r0, r5, 1` **sin extensión ninguna**, con `r5` viniendo de
la unión de dos `lhz` en bloques distintos. Para que GCC 2.9 no extienda ahí,
`r5` tendría que ser un pseudo SI cuyo `nonzero_bits` conociera — y en la unión de
dos bloques no lo conoce. **No se alcanza desde la fuente con las formas
probadas.** La base con el cast sigue siendo el óptimo.

### `GinsuSynthData::BindToData` (zEAXSound2, 340 B objetivo / **344 nuestros**)

`regmap` señala ámbitos: el original declara `period` en `b0/b0` (un bloque
anónimo dentro del cuerpo del bucle) e `i` en `b0` (el `for`), y nosotros `i` a
nivel de función. Arreglar el árbol de ámbitos **no basta**:

    base                                   95,88236 %  344 B  12 difs
    B1 `for (int i = 0; …)`                95,87059 %  344    12
    B2 B1 + llaves extra en el cuerpo      95,87059 %  344    12
    B3 sólo las llaves                     95,88236 %  344    12
    B4 B2 con el `i++` en el cuerpo        95,87059 %  344    12

Los 4 B de más son un `mr r8, r5` que el objetivo no tiene. Queda para quien
retome: el diagnóstico de ámbitos de `regmap` es correcto pero no es la causa.

### `cStichWrapper::Play` (zEAXSound, 384 B, 4 difs)

`regmap` dice que `scale` es una local que **sólo tenemos nosotros**. Es el sexto
contraejemplo del proyecto a «quitar la local de más»: quitarla cuesta 4 B y
11 difs.

    base (con `int scale = 0x7FFF;`)   99,68750 %  384 B   4 difs
    Q1 sin `scale`                     97,44791 %  **388**  15
    Q2 sin `scale` ni `StitchSpecificVol` 97,44791 %  388   15
    Q3 `StitchSpecificVol` con inicializador 97,44791 %  388 15
    Q4 `scale` como `const int`        97,44791 %  388     15

Lo que queda son 4 filas de intercambio limpio r30↔r31 entre `i*4` y
`&this->ActiveSamplesRefs`, dos temporales **sin nombre en el fuente**: no hay
dónde pinchar ni a quién subirle `n_refs`.

### Diagnósticos nuevos que dejo escritos (no barridos)

- **`Speech::Manager::LoadSpeechBank`** (zSpeech, 316 B objetivo / **312
  nuestros**): el objetivo tiene **dos** `mr` de la base de `index->item`
  (`mr r11,r4` antes del bucle y `mr r4,r11` dentro) donde nosotros tenemos uno.
  Es la firma de `GetElapsedTime` (r36c): dos pseudos donde tenemos uno, con
  copias de ida y vuelta a través del arco de retroceso del bucle.
- **`SFX_Common::MsgPlayMiscSound`** (780 B) y **`CARSFX_RoadNoise::Play`**
  (392 B) tienen **la misma raíz**: la expansión del constructor `FX_UVES`. Los
  dos difieren en el orden de los seis `stw` y en qué registro lleva el cero.
  Cerrar uno cierra la mitad del otro; y `Play` además está **4 B corta**
  (388/392), o sea que el problema no es sólo el orden.
- **`CARSFX_Turbo::ProcessUpdate`** (672 B, 8 difs) tiene la firma del §1
  (`lis r9, LC539@ha` dos ranuras adelantado) **pero también** un `mr. r7, r3`
  que llega nueve ranuras tarde: hay que arreglar el `mr.` primero.

---

## 5. Método: cuatro cosas que cuestan una tarde

1. **`mn_repro` necesita un PRE por unidad, y faltaban dos.** Los apunto aquí
   porque no estaban en ningún informe:

       zEAXSound     PRE = los .cpp anteriores de la SourceList que definan lo
                     que use el tuyo. Para EAXAemsManager.cpp:
                       #include ".../EAXSound/AudioMemoryManager.cpp"
                       #include ".../EAXSound/EAXSound.cpp"
                     (11 s por compilación)
       zSpeech       #define ZMAIN_MESSAGES_LUA_INLINE
                     #define ATTRIB_NO_INLINE_CLASSKEY
                     + los 16 #include de cabecera de zSpeech.cpp (líneas 38-53).
                     Para RoadblockFlow.cpp hacen falta ADEMAS los .cpp de las
                     líneas 55-94 de la SourceList. (23 s por compilación)

2. **Una sustitución de texto sin ancla se aplica en el sitio equivocado.** Mi
   arnés hacía `txt.replace(old, new, 1)` y
   `CarCustomizeManager &mgr = gCarCustomizeManager;` aparece **varias veces** en
   `CarCustomize.cpp`: cinco ensayos enteros de la tanda `E*` salieron con
   «`mgrp' undeclared» porque el cambio cayó en otra función. Se ve como un error
   de compilación, no como un falso negativo — pero cuesta una tanda. **Ancla
   siempre con dos o tres líneas de contexto.**

3. **El triaje engaña después de cerrar una función.** `triaje.py` sigue diciendo
   `90,238 %` para `ResolveCurrentDataMemory` porque el `%` sale del
   `report.json` rancio; lo fresco es la columna `difs` (que ya dice 0). La
   medida buena es `pctsnap --cmp` contra la instantánea de antes.

4. **La instantánea de «antes» hay que tomarla ANTES de compilar, no antes de
   editar.** Los `.o` de `build/GOWE69/src` son el estado anterior aunque el
   fuente ya esté tocado; eso permite editar, mirar, y sólo entonces decidir si
   se toma la referencia.

---

## 6. Lo que hay que llevarse a la memoria del proyecto

**`nfsmw-barrera-selectiva` gana un quinto uso y una regla de aplicación:**

> **Uso 5 — orden de preparación de argumentos.** Firma: el `lis @ha` de una
> dirección (literal, global o hash) sale una o dos ranuras fuera de sitio
> respecto al `lwz`/`mr` de otro argumento de la MISMA llamada. Cura: una local
> por argumento y **un `asm("" : "+r"(x))` propio para cada uno** (un solo `asm`
> con N operandos NO vale: es una insn, es un punto de orden). El objeto
> (`this`) también cuenta como argumento y suele necesitar su local.
> **Antes de gastar ensayos en formas de fuente**: cinco formas distintas dieron
> el binario idéntico, porque el orden lo fija `expand_call`.
> **Límite:** no vale si lo que hay que mover viene de OTRA sentencia (la
> barrera sólo añade restricciones, no iza) — medido en `MsgBarrier` con 16
> ensayos.

**Y el escalón de `floor_log2` en `n_refs`:** el asm no volátil no sube la
prioridad «un poco», la sube **a saltos**. Con 3 refs `floor_log2` vale 1 y con
4 vale 2: el numerador de `allocno_compare` pasa de 3 a 8. Por eso una barrera
cierra `NotificationMessage` y tres la rompen. **Cuenta las refs antes de elegir
cuántas poner.**
