# r64 -- `fe`: zFe, zMisc, zSpeech

**Aplicado**: una funcion cerrada (`Setup__Q26Speech13RoadblockFlow`, VEDA de la
r48, cuatro rondas) con una palanca NUEVA y general, mas 8 B de pool duplicado
menos en zMisc. **Ninguna unidad promociona**, y abajo esta medido por que: las
tres estan al 100 % de codigo y el unico frente que les queda es la COLOCACION
del bloque diferido.

Estado al cerrar la ronda (`fncmp`, con el `.o` sellado por tres compilaciones
del mismo sha1):

| unidad  | fncmp        | .o sha1                                    | distancia |
|---------|--------------|--------------------------------------------|-----------|
| zFe     | **0 de 921** | (sin tocar)                                | `dolwhere` 34.384 B |
| zMisc   | **0 de 450** | `7febf3ffb695c808f65048481dd011c513dc5419` | `linkdelta` rodata+560 data+32 |
| zSpeech | **0 de 703** | `6df17777281e5bac3ed62a81649aebf478c821df` | `dolwhere` 11.264 -> **11.248 B** |

`lcfix --check`: **85 pendientes, y NINGUNA es mia** (64 de zLua, 21 de zFe2).
Mis tres unidades no dejan deuda de `$LC`.

---

## 1. `Setup__Q26Speech13RoadblockFlow`: CERRADA. La veda no era de fuente

Cuatro rondas (r36d, r36f, r45, r48, r63) barrieron **15+ formas de fuente y 6
barreras** contra las 4 filas que quedaban, todas negativas. La causa real:

> El cruce `lha`/`lwz` del thunk de una llamada virtual lo decide el **nivel 5 de
> `rank_for_schedule`** (`haifa-sched.c:4158`, «mas dependientes gana»)
> **en `sched2`**, y el unico modo de moverlo es darle al `lwz` **otro
> dependiente de r0 dentro del mismo bloque basico**.

### Como se probo (banco de 20 lineas, 0,4 s por prueba)

`sprobe.py` sobre un `.cpp` minimo con la misma forma (herencia multiple -> delta
no nulo, `RBUpdate` delante, `PursuitApproaching` virtual) reproduce el sintoma
**exacto**, y `schedtrace.py` lo explica:

* en **`sched1`** las dos cadenas salen SIEMPRE `lha` -> `lwz`, en las dos
  variantes. Quien las cruza es **`sched2`**. Barrer formas contra «el empate de
  sched2» mide el orden **que dejo sched1** (ver `nfsmw-rank-for-schedule`).
* en `sched2` el nivel 2 (`INSN_REG_WEIGHT`) esta **apagado**
  (`if (!reload_completed && ...)`), asi que el desempate cae al nivel 5: numero
  de dependientes; y si empata, LUID mas bajo, que es `lha`.
* `Ready list (t = 7): 70 66` -> elige **66 = `lha`** cuando el bloque acaba en
  `blrl` + `b`; `Ready list (t = 7): 59 63` -> elige **63 = `lwz`** cuando el
  bloque SIGUE detras de la llamada. La unica diferencia entre los dos casos es
  que en el segundo hay **otra escritura de r0** detras (`lwz r0,0x24(r28)` del
  `mFlags &= ~REQ_SERVICE`), que le anade una dependencia de salida al `lwz`.

### La palanca: **LA COLA DUPLICADA**

`mFlags &= ~REQ_SERVICE;` estaba al final de la funcion, en el bloque de union al
que las ramas saltan con `b`. **Escribiendolo DENTRO de las dos ramas** (con
`return` detras) el `lwz r0,0x24(r28)` entra en el bloque de la llamada, sched2
ve el dependiente extra y emite `lwz`/`mtlr` delante de `lha`/`add` -- que es
exactamente lo que hace el objetivo. **El cross-jumping posterior a sched2 vuelve
a fundir las colas, asi que los bytes no cambian.**

Con eso: 588/596 B. Faltaban 8 B porque el cross-jumping fundia ADEMAS la cola
del `MiscSpeech::RBWarning()` de la rama de arriba (ahora la de abajo es mas
larga y el sufijo comun crece). Un `__asm__("# e")` detras de ese `RBWarning`
la separa: **596/596, `fncmp 0 de 703`**.

Efecto en el DOL: `dolwhere zSpeech` **11.264 -> 11.248 B**, los 16 B que la r63
habia predicho.

### NEGATIVOS de esta ronda (13 formas, todas en el banco minimo, todas LHA-FIRST)

`__asm__("")`, `__asm__("# d")`, `__asm__ __volatile__("")`, clobber de `"r0"`,
clobber de `"lr"`, entrada `"r"(p)`, salida `"=r"(z)`, clobber de `"memory"`, dos
asm seguidos, y las cinco mismas **delante** de la llamada. Y de fuente:
`if (p != 0) {...}` en vez de `return`, `Disp *d = ai->GetDispatch();` en local, y
`goto` a la etiqueta comun.

**La razon por la que ninguna vale, y vale para todo el arbol**: un `asm` de cero
bytes **no le anade un DEPENDIENTE DE r0 al `lwz`**, y ese es el unico nivel que
mueve este empate. **La familia entera de «barrera / pin / barrera de ranura» es
ORTOGONAL a un empate del nivel 5.** Antes de gastar una ronda en barreras, mirar
`Ready list` en `schedtrace`: si las dos candidatas entran en el MISMO ciclo y el
empate es de nivel 5, hay que anadir un dependiente, no frenar nada.

### Se PROMOCIONA como palanca general

`fe_cola_duplicada`: *cuando el objetivo cruza dos cadenas al final de un bloque
que acaba en `b`, duplicar en las ramas la sentencia del bloque de union que
escribe el registro en disputa; el cross-jumping de despues de sched2 la vuelve a
fundir y los bytes no cambian.* Aplica a cualquier near-miss cuyo bloque acabe en
llamada + salto incondicional. Coste de la prueba: un banco de 20 lineas.

---

## 2. Lo que de verdad bloquea las tres: **el bloque diferido, y NADA MAS**

Medido sobre **DIRECCIONES ENLAZADAS**, no sobre el `.o` (los «descolocadas» de
`textorder` son fantasma en cuanto el enlace estripa algo):

| unidad  | simbolos objetivo / nuestros | solo nuestras | solo objetivo | descolocadas | saltos | ventana |
|---------|------------------------------|---------------|----------------|--------------|--------|---------|
| zFe     | 921 / 921                    | **0**         | **0**          | 227          | 56     | `8013ED00`..`80142AC0` |
| zSpeech | 790 / 790                    | **0**         | **0**          |  67          | 19     | `802B44E4`..`802B59B0` |
| zMisc   | 451 / 451                    | **0**         | **0**          | **0**        | **0**  | -- |

Es decir: en zFe y zSpeech **el enlace emite EXACTAMENTE los mismos simbolos que
el original, con el mismo tamano y el mismo codigo, y todo esta a +0 hasta que
empieza el bloque diferido de `finish_file`**. zFe: 157 kB de 174 perfectos.
zSpeech: 174 kB de 178 perfectos.

Los 163 (zSpeech) y 89 (zMisc) simbolos «SOLO NUESTRAS» que canta `textorder`
**no existen en el enlace**: el enlazador los tira. No son trabajo.

### zFe: la PRIMERA divergencia, y vale los 68 B del primer salto

```
   OBJETIVO                                        NUESTRO
   ...MemcardGetCurrentUIOperation__Fv             ...MemcardGetCurrentUIOperation__Fv
   220 __static_initialization_and_destruction_0   12  ClassKey__Q36Attrib3Gen8frontend   <-- sobra aqui
                                                   56  RaiseToPower__H1i10_i_i            <-- sobra aqui
                                                   220 __static_initialization_and_destruction_0
```

12 + 56 = **68 B**, que es exactamente el primer salto (`+0 -> +68`). El objetivo
pone `ClassKey__Q36Attrib3Gen8frontend` 6.380 B mas abajo (en `0x801405EC`, entre
`_._15uiRapSheetLogin` y `_._15RapSheetUSDatum`). **Ese es el sitio por donde
empezar la r65 en zFe**: el resto de los 56 saltos son en su mayoria bloques
enteros arrastrados por este.

### zSpeech: la permutacion, por GRUPOS (los 19 saltos son 13 grupos)

```
OBJETIVO                                   NUESTRO
G1  _._SpeechSampleVec .. _._observations  G6  EAXCharacter (19 accesores)
G2  Module   (GetNumBanks..IsStreamQueued) G2  Module
G3  GameSpeech (GetCSIptr..IsDataLoaded)   G10 _._VecHashMap644, RebuildTable, _._SpchSampleMap
G4  SpeechFlow (GetState/Reset/IsBusy)     G1a _._SchedSpchEvents, _._SpeechSampleVec
G5  ClassKey speechtune                    G3  GameSpeech
G6  EAXCharacter                           G9  SED_NISSFX
G7  EAXCop                                 G5  ClassKey speechtune
G8  IsHeli__13EAXAirSupport                G4  SpeechFlow
G9  SED_NISSFX                             G7  EAXCop
G10 _._VecHashMap644, _._SpchSampleMap,    G8  EAXAirSupport
    RebuildTable                           G1b _._observations, _._copList, _._VoiceUsage
G11 Call__Hermes...  (13)   == IGUAL ==    G11 Call__Hermes... (13)      == IGUAL ==
G12 _._SpeechHashIDMap, _._EventHistory,   G12 _._SampleReqList primero, luego los otros tres
    _._SPCHEventList, _._SampleReqList
G13 AllocVectorSpace...     == IGUAL ==    G13 AllocVectorSpace...       == IGUAL ==
```

Dos observaciones utiles para quien lo coja: **G11 y G13 ya casan enteros** (los
13 `Call__Hermes` y los 10 de `FixedVector`), y **los cinco destructores de G1
salen PARTIDOS en dos trozos** (dos en el puesto 4 y tres en el puesto 11): eso
no es un desplazamiento, es que su primer uso esta en dos sitios distintos del TU.

---

## 3. zMisc: por que NO puede promocionar, con el mecanismo medido

zMisc tiene el `.text` **PERFECTO** (0 descolocadas de 451, 0 saltos) y el codigo
al 100 %. Lo unico que le separa del DOL es `rodata+560` y `data+32`, y la `.data`
va detras: su delta base es **+576**, o sea el corrimiento que le impone la
`.rodata`. **La `.rodata` es el problema entero.**

### Donde estan los 560 B, al byte

`dolrod zMisc .rodata` da: **los primeros 7.355 B casan EXACTOS**, y luego

```
SOBRA nue 803F64F3   616 B     <-- el pool VIVO que emite cc1plus
FALTA obj 803F65AB    64 B
```

Ese bloque de 616 B es el pool `$LC153..$LC516` de la unidad: los literales que
cc1plus interna para **~20 funciones VIVAS** (`InitBasisMatricies`, `main`,
`InitializeEverything`, `Main_AnimateFrame`, `MaybeCameraShake`,
`PrepareRealTimestep`, `AdvanceWorldTime`, `GetDebugRealTime`...).

**Y el objetivo tambien los tiene** -- pero UNA sola vez, y en el sitio natural.
Comprobado a mano: los 44 B de `$LC153..$LC165` (`0, -1, 2, -5, 3, 1, 4, -3, 0.5,
9, -9`) estan en el objetivo en `0x803F48D4`, o sea **el byte 92 de su
`.rodata`**, justo detras del prefijo de bWare. Lo mismo con `3983126f`,
`4ca4cb80`, `47a8c000` y la cadena `"WorldTime"` de `Timer.cpp`, que el objetivo
tiene en `0x803F62B0..0x803F6360`.

O sea: **el bloque escrito a mano de zMisc (450 simbolos, ~7,8 kB) reproduce el
pool ENTERO del objetivo, y cc1plus emite ADEMAS su propia copia viva de 616 B al
final.** Eso son los 560 B netos.

### Las tres salidas, y por que dos estan cerradas

1. **`ZMISC_POOL(off)`** (la palanca de la r61: `_bwarePrefix + off`, ya usada
   para 150 cadenas). **Funciona, y la he usado**: `"MainLoop()"` ->
   `ZMISC_POOL(0x830)` en `Main.cpp:386`. `dupstr` 12 cadenas/126 B ->
   **11 cadenas/115 B**, el bloque sobrante 624 -> **616 B**, `fncmp` sigue en
   **0 de 450**.
   **Pero NO llega a cero**: solo vale para CADENAS, y de los 616 B **112 B son
   los 14 `4330000080000000`** (la constante magica de `int -> float`), que no
   nace de ningun literal de fuente y no se puede redirigir. **Suelo duro: 112 B.**
2. **`extern const float lbl_XXXX`**: mismo suelo de 112 B, y ademas
   `pool2lit.py` lo tiene documentado como **cambia el reparto de registros**.
3. **La unica salida completa: INTERCALAR el bloque a mano entre los `#include`.**
   Medido con `ngccc -S` en un fichero de 5 lineas: cc1plus emite el pool de cada
   funcion **DELANTE de la funcion**, y un `asm()` de fichero sale **donde esta
   escrito**, asi que el orden de `.rodata` es
   `pool(f1) | asm1 | pool(f2) | asm2 | pool(f3)`. Partiendo el bloque de 450
   simbolos en trozos y metiendo cada trozo entre los `#include` que le tocan, el
   pool vivo de cc1plus cae en su sitio y se pueden BORRAR los bytes escritos a
   mano que duplica.

### El NEGATIVO que hay que anotar de la salida 3

**No basta con partir por `#include`.** Entre `$LC165` y `$LC177` (los dos del
MISMO fichero, `QuickSpline.cpp`) el objetivo tiene 48 B: la cadena
`"QuickSpline"` y dos constantes. **`"QuickSpline"` NO EXISTE en todo nuestro
arbol** (`grep -rn '"QuickSpline"' src/` -> cero fuera del bloque a mano): el
`QuickSpline.cpp` original tenia ese literal y el nuestro lo perdio. Mientras no
se reponga, el corte por `#include` deja los dos grupos pegados y el pool sale
48 B corrido. **La salida 3 es una ronda entera y empieza por reponer los
literales que faltan, no por mover asm.**

---

## 4. PROPUESTAS (no aplicadas: tocan cabeceras COMPARTIDAS)

Las 5 cadenas duplicadas que quedan en zMisc y que **si** existen en el objetivo
(offset leido del DOL, `off = direccion - 0x803F4878`):

| cadena                  | B  | direccion objetivo | `ZMISC_POOL(off)` | nace en |
|-------------------------|----|--------------------|-------------------|---------|
| `DebugVehicleSelection` | 22 | `0x803F4E0C`       | `0x594`           | `src/Speed/Indep/Src/World/DebugVehicleSelection.h:41` (`return "DebugVehicleSelection";`) y `.cpp:21` |
| `Pkt_Heli_Open`         | 14 | `0x803F4E78`       | `0x600`           | `SoundConn.h` |
| `HeliSoundConn`         | 14 | `0x803F4F68`       | `0x6F0`           | `SoundConn.h` |
| `Pkt_Car_Open`          | 13 | `0x803F4E48`       | `0x5D0`           | `SoundConn.h` |
| `CarSoundConn`          | 13 | `0x803F4F58`       | `0x6E0`           | `SoundConn.h` |

Receta, identica a la de la r61: guardar el literal en la cabecera con
`#ifndef ZMISC_POOL` / `#define ZMISC_POOL(off) (s)` y usar `ZMISC_POOL(off)` en
el punto de uso, para que las demas unidades no cambien.

**AVISO MEDIDO, y cuesta una compilacion averiguarlo**: el offset tiene que estar
**alineado a 4**. `"frontend"` esta en `0x803F5269` (offset `0x9F1`, sin alinear)
y `ZMISC_POOL(0x9F1)` **rompe `LoadFrontEndVault__Fb`** (2 insn y 5 reubicaciones
de mas: GCC no puede plegar el `@l` sin alinear en la `la`). Revertido. Las cinco
de la tabla si estan alineadas. Para las no alineadas haria falta un `.globl`
nuevo dentro del bloque a mano, y eso cambia el estripado -- **no probado**.

Sin proponer, por si acaso: `"%s - %s"` (`bFile.cpp:767`), `"TODO2"`
(`ResourceLoader.cpp:712`) y `"TODO"` **no existen en el `.rodata` del objetivo**
(buscados byte a byte en la ventana `803F4878..803F6730`). No se pueden redirigir
a ningun sitio; o el original llamaba con otra cadena, o esas tres llamadas viven
en las 89 funciones que emitimos de mas y el enlace tira. 19 B.

---

## 5. Ficheros tocados

* `src/Speed/Indep/Src/Speech/RoadblockFlow.cpp` -- la cola duplicada + el
  `__asm__("# e")` + 36 lineas de comentario con el diagnostico, el efecto en el
  DOL y los 13 negativos (`previo.py Setup` lo encuentra).
* `src/Speed/Indep/Src/Misc/Main.cpp:386` -- `"MainLoop()"` -> `ZMISC_POOL(0x830)`.

Nada de `configure.py`, `config/GOWE69/*`, `splits.txt` ni `symbols.txt`. Ningun
commit. Ninguna cabecera compartida tocada.
