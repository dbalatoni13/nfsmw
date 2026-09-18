# r61 — mw: steering, criticalpath, madidct, inittmr

Cuatro unidades que **no compila GCC** (dos de MWCC, dos de ProDG fuera de las
SourceLists). Ninguna promociona esta ronda. Lo que sí sale:

1. **Un bloqueo de promoción nuevo, medido, y de una clase que ninguna
   herramienta del proyecto mira: la ALINEACIÓN DE SECCIÓN.** En `criticalpath`
   vale **13 B de DOL** y ya está arreglado. En `inittmr` y en `steering` el
   mismo defecto está presente (uno arreglado y neutro hoy, el otro medido y sin
   palanca).
2. **La FAMILIA C del informe r60b queda DECIDIDA**, y en el sentido que el
   encargo pedía: el operando de entrada **sí crea arco de dependencia en
   haifa** — `prio` 2 → 6 en las dos insn, con la traza delante — pero es
   **negativo** en las dos funciones donde lo he barrido (34 ensayos en
   `IdctRow`, 8 en `VP6`).
3. **`inittmr` tiene la promoción a tres líneas de configuración**, con la
   prueba completa: lo único que la separa de `DOL OK` es un rango de `.rodata`
   en `splits.txt` **más una entrada de `keep.lst`** sin la cual el rango no
   sirve de nada.
4. El **combo de steering** del decisor E está aplicado y sellado.
5. La receta de `IdctColumn` que el informe r60b llamaba «la más clara de todo
   el informe» **está MUERTA**, con objeto byte-idéntico como prueba.

DOL base al terminar: `9619ba57c9919f95f7f2ac951a2166a3517f91e3` **OK**.
`checksplits` LIMPIO, `prefijochk` LIMPIO. **Cero correcciones de `lcfix.py`
pendientes** (ninguna edición mueve un `$LC`: los cuatro objetos conservan
tamaño y contenido de `.rodata`/`.sdata`/`.sdata2`).

---

## 0. Estado de las cuatro unidades

| unidad | linkdelta ANTES | linkdelta DESPUÉS | trypromo ANTES | trypromo DESPUÉS |
|---|---|---|---|---|
| `Packages/vp6/.../criticalpath` | `.text +0  IGUAL` | `.text +0  IGUAL` | ROTO `df912aaf2ba1` | ROTO `0241e4bbeb7c` |
| `egami/.../madidct` | `.text -20  IGUAL` | `.text -20  IGUAL` | ROTO | ROTO `fec9e6117ef4` |
| `LibSN/steering` | `.text -8  bss+32` | `.text -8  bss+32` | ROTO | ROTO `2ee66b227885` |
| `Speed/.../inittmr` | `.text +0  rodata-144` | `.text +0  rodata-144` | ROTO | ROTO `3cbf803f9f36` |

`linkdelta` no se mueve en ninguna, y eso es correcto: **`linkdelta` compara
TAMAÑOS de sección y la alineación no la ve.** Lo que se movió en `criticalpath`
sólo lo ve `dolwhere` (176 B → **163 B**). Es una entrada nueva para
`docs/TRAMPAS.md`.

Sellos triples de los cuatro `.o` al cierre (regla 7):

```
criticalpath  f5200a220c0aef3e0ebafad992d85bd81fdd5b43   x3
madidct       99dd931e875f78690710f858b655350181300b66   x3
steering      1962b7d16adc91a21e4f8070dacc3113935a219e   x3
inittmr       0548e227f2bc47b4ee869c8e9ad212082aa9b322   x3
```

---

## 1. criticalpath — la alineación de `.sdata`, 13 B de DOL

### Lo que había

`criticalpath.c` dice desde la r48: «`.data` y `.sdata` ya eran idénticas … lo
ÚNICO que separa a esta unidad de promocionar son 12.040 B de `.text` y esta
función». `promote.py` lo confirmaba: **«LIMPIA: se puede marcar Matching»**. Y
sin embargo `dolwhere` daba **176 B distintos**, de los cuales **13 estaban
fuera de `VP6_PredictFilteredBlock`**:

```
80359CD3   1 B  ScalarReconIntra_GC  +0x3    obj f0   nue ec
8035A523   1 B  FilterBlock1dBil_GC  +0x3    obj f4   nue f0
8035A5AB   1 B  FilterBlock2dBil_GC  +0x7    obj f4   nue f0
804FF5AC  10 B  f128                 +0x0    obj 00000000 43000000 4280
                                              nue 43000000 42800000 0000
```

`fncmp` declaraba **exactas** esas tres funciones. Los tres bytes son la parte
baja de un desplazamiento `@sda21`, y los tres difieren **en 4**.

### La causa

Los dos objetos tienen `.sdata` de **8 B exactos** con el mismo contenido
(`f128` = 128.0f en +0, `f64` = 64.0f en +4). Lo que difiere es
**`sh_addralign`**:

```
build/GOWE69/obj/.../criticalpath.o   .sdata  size=8  align=8
build/GOWE69/src/.../criticalpath.o   .sdata  size=8  align=4     <-- nuestro
```

`f128` cae en `0x804FF5AC`, que **no es múltiplo de 8**. Con `align=4` el
enlazador nos ponía el par cuatro bytes antes que al original, y con él se
movían los tres `@sda21` de las funciones vecinas.

### El arreglo, aplicado

`src/Packages/vp6/1.0.6/source/decode/gc/criticalpath.c` (la declaración de
`f128`):

```c
static float f128 __attribute__((aligned(8))) = 128.0f;
```

Va en `f128` y no en `f64` porque `f128` está en el offset 0 de la sección: el
`.align 3` que emite **no mete relleno** (la sección sigue midiendo 8 B) y sólo
sube el `sh_addralign`.

**Medida**: `.sdata align 4 → 8`, tamaño 8 B clavado, `.text` sin tocar, `fncmp`
sigue en 1 de 21, y `dolwhere` **176 B → 163 B**. Las cuatro entradas de arriba
desaparecen del volcado y **todo lo que queda está dentro de
`VP6_PredictFilteredBlock`**. `.o` sellado 3× (`d5c1f48740af…` sin el
comentario, `f5200a220c0a…` con él).

### Por qué nadie lo había visto

`promote.py` y `linkdelta.py` comparan **tamaños** de sección. 8 == 8 en tamaño,
4 != 8 en alineación. Sólo `dolwhere` lo delata, y lo delata **imputándolo a un
símbolo de datos y a tres funciones que las demás herramientas dan por buenas** —
que es justo el caso de `memory/nfsmw-huecos-leer-el-dol`.

**PROPUESTA para `docs/TRAMPAS.md`** (entrada nueva):

> ### `promote.py` / `linkdelta.py` / `fncmp.py` — no miran `sh_addralign`
> Dan «LIMPIA» / «IGUAL» / «exacta» con la sección del tamaño y el contenido
> correctos pero con **otra alineación**, y entonces el enlazador la coloca en
> otra dirección y arrastra los `@sda21`/`@sda2` de sus vecinas.
> **Caso (r61, `criticalpath`)**: `.sdata` 8 B idénticos, `align` 4 contra 8,
> `promote.py` decía LIMPIA desde la r48 y costaba **13 B de DOL** — 10 en
> `f128` y 1 byte en cada una de tres funciones que `fncmp` daba por exactas.
> **Confirmación**: comparar `sh_addralign` de cada sección de los dos `.o`.
> **Barrido hecho en la r61**: de mis cuatro unidades, **tres** tenían el
> defecto — `criticalpath` `.sdata` 4/8, `inittmr` `.sdata` 4/8, `steering`
> `.bss` **8/4** (al revés). Merece un barrido de las 616.

---

## 2. criticalpath — `VP6_PredictFilteredBlock`, y la FAMILIA C decidida

Toda la deuda restante de la unidad (163 B de `dolwhere`, 740 B de `linked`)
está aquí. Nada se deja puesto: el tamaño ya es exacto y todas las palancas son
de cero bytes.

### PASO 0 — el texto de la r48 se lee mal, y esto lo corrige

La r48 escribió dos palancas. **La segunda no vale sola.** Medido, todo
740/740 B:

| variante | filas | sha1 del `.o` |
|---|---:|---|
| base (árbol, sin asm) | **40** | `d5c1f48740af` |
| L1 = `__asm__("" : "+r"(mVx));` tras las declaraciones | 29 | `9fbe9c74239c` |
| L2 = `rec` + barrera, **sola** | **40 — CERO EFECTO** | `ebe67b990e4d` |
| L1 + L2 | **25** | `ad60ebc7b419` |

Los 40, los 29 y los 25 son los tres números de la r48 exactos, así que la base
no se ha movido desde entonces.

### La pregunta de la familia C, contestada con la traza

El informe r60b dice: «`VP6` … su observable (`prio` 2→3 en la traza) contesta
si un operando de entrada crea arco de dependencia en haifa — un dato que
ninguna otra función del dossier mide». Y la veda de la r49 decía: «Para eso su
`INSN_PRIORITY` tendría que ser mayor que 2. **Y no puede serlo**».

**Puede serlo.** `schedtrace.py criticalpath VP6_PredictFilteredBlock -dS
-fsched-verbose-5`, sobre la base L1+L2. Las dos `andsi3` de `ModX` y `ModY` son
las insn **109** y **114** (las 129 y 134 de la r49; renumeradas por los asm), y
la insn del `asm` de `rec` es la **143**:

```
BASE                                     CON  : "r"(ModX), "r"(ModY)
;;  109  121  0  1   2  1  iu2 : 168     ;;  109  121  0  1   6  1  iu2 : 168 143
;;  114  121  0  1   2  1  iu2 : 168     ;;  114  121  0  1   6  1  iu2 : 168 143
;;  143   -1  0  1   5  1  none: 168 147 ;;  143   -1  0  3   5  1  none: 168 147
```

- `prio` **2 → 6** en las dos.
- La lista de dependientes de cada una gana el **143**.
- El `dep` del asm sube de **1 a 3**: dos arcos entrantes nuevos.
- **740/740 B**: no gasta ranura (la insn del asm sale con `code -1` y unidad
  `none`).

**Consecuencia para el dossier**: la familia C (11.024 B) tiene su mecanismo
**probado**, y la veda de la r49 sobre `VP6` queda **rota**. Quien la cite
tendrá que explicar esta traza.

### Pero aquí es negativo, con cifra

Todo 740/740 B salvo donde se indica; base = 25 filas:

| variante | filas |
|---|---:|
| `rec` + `"r"(ModX), "r"(ModY)` | 44 |
| `rec` + `"r"(ModX)` | 43 |
| `rec` + `"r"(ModY)` | 26 |
| asm inout sobre `TempPtr2` + `"r"(ModX),"r"(ModY)` | 46 · **736 B** |
| ídem sólo ModX / sólo ModY | 45 / 44 · **736 B** |
| asm inout sobre `Stride` + `"r"(ModX)` / `"r"(ModY)` / los dos | 32 / 29 / 32 |
| `rec:ModY` + `tp:ModX` | 46 · 736 B |
| `rec:ModX` + `tp:ModY` | 57 · 736 B |

Subir la prioridad saca los dos `and` de las ranuras 51/52 pero **no los deja en
las 32/40 del objetivo: los pasa de largo.**

### El diagnóstico corregido — la mitad es REPARTO, y está escrito en el asm

La r48 decía «lo que queda son DOS cosas y las dos son sched1». Leyendo el diff
de la base de 25 filas, **no**:

```
        OBJETIVO                    NOSOTROS
26      lhax r0, r6, r11            lhax r25, r6, r9      <-- mVy va a r25
28      lhax r10, r7, r11           lwz  r8, 0x88(r31)
31      -                           lhax r10, r7, r9
32      and  r25, r0, r3   (ModY)   and  r0, r3, r0
34      lwz  r8, 0x88(r31)          -
40      and  r28, r10, r3  (ModX)   and  r11, r3, r11
41      add  r10, r10, r11          add  r11, r10, r11    <-- no mata mVx
51      -                           and  r28, r10, r3     (ModX)
52      -                           and  r25, r25, r3     (ModY)
```

- **ModY**: nosotros cargamos `mVy` **directamente en r25**, que es el registro
  fijado a `ModY` (`register int ModY asm("r25")`). El `and r25,r25,r3` no puede
  salir hasta que `mVy` esté muerta, y por eso cae en la 52. El objetivo tiene
  `mVy` en **r0** y hace el `and` en la 32.
- **ModX**: el objetivo emite `add r10,r10,r11`, que **mata** `mVx`, así que su
  `and r28,r10,r3` **tiene** que ir antes — ranura 40. Nosotros emitimos
  `add r11,r10,r11`, `mVx` sobrevive, y el `and` se hunde a la 51.

Medido contra eso, y todo **peor** (740 B salvo nota):

| variante | filas |
|---|---:|
| pin de `mVy` a r0 / r9 / r11 / r8 / r10 | 52 / 53 / 54 / 54 / 55 |
| asm sobre `mVy` tras las declaraciones | 55 · 736 B |
| asm sobre `mVx`+`mVy` tras las declaraciones (y al revés) | 41 · 736 B |
| asm sobre `mVx` **detrás** de las asignaciones de ModX/ModY | **25** (`47e1584b3b5c`) |
| asm sobre `mVy` detrás / los dos detrás | 62 / 49 |
| L1 + asm sobre `mVy` detrás / `mVx` detrás / los dos | 60 / 52 / 38 |
| L1 repetido ×2 / ×3 / ×4 | 26 / 53 / 54 |

Es el caso de `memory/nfsmw-si-el-pin-empeora`: **la primera diferencia que NO
es de registro es la ranura 28**, y es un empate de la unidad `lsu` — insn 91
(el `lhax` de `mVx`) tiene `prio 8` y la insn 149 (la carga de
`FrameReconStride`) tiene `prio 9`. Hay que subir 91 a ≥10 o bajar 149 a ≤7 **sin
mover nada más**. Apilar asm sobre `mVx` sube 91 de uno en uno (es lo que hacen
L1×2 y L1×3) pero rompe el resto.

Todo esto queda escrito en el comentario de la función (`previo.py
VP6_PredictFilteredBlock` lo encuentra).

---

## 3. madidct — la receta de `IdctColumn` del r60b está MUERTA

Base reproducida sin desviación (los tres números de la r48):

```
IdctRow      496 B  137 filas  46,372093 %   mtctr 1/0  mfctr 2/0  mr 10/8
IdctColumn   632 B  152 filas  50,449368 %   mr 12/10   lwz 18/20
```

### (1) `int s4 = src[4];` — NEGATIVO DURO

El informe r60b la llamaba «la más clara de todo el informe»: «Observable: el
`lwz r11, 0x8(r1)` de nuestro índice 16 desaparece; histograma `lwz` 20→18 y
`mr` 10→12; **632 B clavados**».

Medido: **636 B (+4)**, `lwz` **sigue en 20**, `mr` **sigue en 10**. Y las
**cuatro** variantes de sustitución (sólo la cadena de `or`, or+t6, or+t7,
or+t6+t7) dan el **MISMO OBJETO byte a byte**, sha1 `0fce85bd5749`: el front-end
ya hacía CSE de `src[4]` y la local **no crea un punto de definición nuevo**. Es
el mismo mecanismo que el negativo de `float clampSlip = 0.15f;` en
`GenerateRoadNoise`. No repetir.

### (2) Lo que SÍ toca el defecto: el PIN sobre esa local

`register int s4 asm("rN") = src[4];`

**Primera vez que el histograma de `IdctColumn` se arregla**: `mr` **12/12**
(era 12/10) y `lwz` **18/17** (era 18/20). Pero la función cae a **624 B**: el
pin se lleva el derrame entero y el objetivo conserva uno. Nueve registros:

| pin | B | filas |
|---|---:|---:|
| r10 | 624 | **151** (mejor) |
| r7 / r9 / r3 | 624 | 157 |
| r8 | 624 | 161 |
| r5 | 624 | 164 |
| r6 | 636 | 186 |
| r11 | 640 | 183 |
| r4 | 660 | 186 |

Con el pin puesto, sustituir o no `t6`/`t7` es indiferente (mismo sha1).
`int s4` + `__asm__("" : "+r"(s4))` sin pin: 640 B / 162 filas.

**Para el siguiente**: la base de ataque de `IdctColumn` ya no es la de 632 B con
`mr 12/10`; es la de **624 B con `mr 12/12`**, y lo que hay que hacer es
**devolver 2 instrucciones**, no volver a la base vieja.

### (3) `IdctRow` — la familia C barrida entera, 34 ensayos

PASO 0 (receta r46) reproducido exacto: **516/516 B, 135 filas, 48,24031 %**,
`mtctr 1/1 mfctr 2/1 mr 10/8 lwz 9/10 mflr 2/3 mtlr 2/3`, `.o` `e13e926331d4`.
`IdctColumn` no se mueve (632/632, 152 filas).

**El observable que pedía el encargo — `mfctr` de 1 a 2 — SÍ se alcanza.**
`__asm__("" : "+c"(src6) : "r"(t4))` da `mtctr 1/2 mfctr 2/2 mr 10/10 lwz 9/9`:
**las tres entradas que faltaban cuadran de golpe**. Pero cuesta **536 B (+20)**
porque aparecen dos vueltas de más por LR (`mflr 2/4`, `mtlr 2/4`), y ninguna
combinación las quita conservando el `mfctr`.

Barrido completo (B / filas / `mfctr` nuestro):

```
"+c" "r"(t1),(t2),(t5),(t9) .. OBJETO IDENTICO a la base (516/135/1)
"+c" "r"(t3) 516/134/1   "+c" "r"(t6) 516/142/1   "+c" "r"(t7) 516/139/1
"+c" "r"(t8) 516/140/1   "+c" "r"(t4) 536/145/2   <-- el unico con mfctr 2
"+l" "r"(t1) 520/144/1   "+l" "r"(t2) 524/147/1   "+l" "r"(t3) 524/160/1
"+l" "r"(t4) 528/158/1   "+l" "r"(t5) 552/162/2   "+l" "r"(t6) 520/149/1
"+l" "r"(t7) 532/140/3   "+l" "r"(t8) 516/134/1   "+l" "r"(src6) 532/159/1
c:t4+l:t1 532/142/3   c:t4+l:t3 516/157/1   c:t4+l:t5 540/143/3
c:t4+l:t6 524/157/1   c:t4+l:t8 536/147/2   c:t4+l:src6 536/145/2
c:t4,t3 sin "+l" 544/143/3     c:t4+l:t3,t8 516/157/1
c:t4,t8+l:t3 516/157/1         c:t4,t5+l:t3 516/157/1
c:t3+l:t8 516/133/1  <-- MEJOR RECUENTO DE FILAS JAMAS MEDIDO AQUI
c:t5+l:t8 516/134/1
quitar el asm "+l" y dejar solo c:t4 --> 492 B, mflr 0 / mtlr 0
```

Dos cosas que valen para el siguiente:

- **`c:t4 + l:t3`** (516 B) es el único estado en que `mflr`/`mtlr`/`lwz` cuadran
  a la vez (2/2, 2/2, 9/9). Lo que le falta es `mfctr` y `mr` — el reverso
  exacto de `c:t4` solo. **Nadie ha probado a mezclar los dos estados por otra
  vía.**
- **`c:t3 + l:t8`** es la mejor base de filas (**133**) al tamaño exacto, y
  sustituye a la receta de r46 como PASO 0 recomendado.

Determinismo comprobado: dos ejecuciones del barrido dan los mismos sha1
(`c:t4+l:t3` = `720fe472f59b` las dos veces, `l:t8` = `a29ae1ef323b`).

**Nada se deja puesto** (cero bytes). Todo en el comentario: `previo.py IdctRow`.

---

## 4. steering — combo aplicado, y un segundo bloqueo medido

### El botín del decisor E, APLICADO

`scratchpad/steering61/combo_crlf.c` copiado sobre `src/LibSN/steering.c`.
Reproduce el sello del decisor **exactamente**: `.o` sha1
`1962b7d16adc91a21e4f8070dacc3113935a219e`, tres compilaciones.

```
ANTES                       DESPUES
Effect_Init  26 insn        Effect_Init  21 insn
CookValues   13 insn        CookValues    9 insn
resto identico              resto identico
6 de 36                     6 de 36
```

Ninguna vecina se mueve: `SimThread_Step` 920/924, `HandleTriggers` 42,
`SimThread_Init` 8, `Effect_PerformEnvelope` 192/196, y las 30 exactas siguen
exactas. Es trabajo de dos rondas anteriores que estaba fuera del árbol.

### Lo segundo: `bss+32` es ALINEACIÓN, y MWCC no la suelta

`linkdelta` da `.text -8, bss+32`. El `-8` son los dos tamaños cortos conocidos
(`SimThread_Step` −4 y `Effect_PerformEnvelope` −4). El `bss+32` es de
alineación, y va **al revés** que en criticalpath:

```
obj  .bss  size=40  align=4      (su direccion, 0x804B9FFC, no es multiplo de 8)
src  .bss  size=40  align=8      <-- nuestro
```

Es un solo objeto, `static s32 ia[LG_FILTER_TAPS]`.

**MEDIDO Y NEGATIVO**: `__attribute__((aligned(4)))` sobre ese array **no hace
nada** — MWCC lo acepta sin aviso y sigue sacando `.bss` con `align 8`; el `.o`
entero no cambia. Quien vuelva tiene que buscar un `#pragma` de MWCC o un cambio
de tipo, **no un atributo de GCC**. Queda anotado junto al array.

**Aviso de alcance**: `build.ninja:4716` marca `LibSN/steering.c` como
`linked False`. Nada de esto mueve el DOL hoy; son 2.568 B que sólo cobran
cuando las seis funciones cierren a la vez **y** se resuelva el `bss+32`.

---

## 5. inittmr — la promoción está a tres líneas de configuración

`fncmp` da **0 de 4 funciones con el código distinto**. `linkdelta` da
`.text +0` con **`rodata-144` como único delta**. No le falta código.

### Qué pasa hoy, medido

`splits.txt` no le da `.rodata` a `inittmr.cpp`, así que el objeto **extraído**
no tiene sección `.rodata` y en su lugar **referencia tres símbolos externos**:

```
.rela.text de build/GOWE69/obj/.../inittmr.o
   off=004e  lbl_80413EC0  addend=0
   off=0056  lbl_80413EC8  addend=0
   off=005a  lbl_80413EC0  addend=0
   off=005e  lbl_80413EB8  addend=0
   off=006a  lbl_80413EC8  addend=0
   off=007e  lbl_80413EB8  addend=0
```

Seis reubicaciones, **los tres símbolos, addend 0 siempre** — nunca un
desplazamiento interior. Los define el comodín
`build/GOWE69/obj/auto_05_80413E30_rodata.o`, y **un barrido de las 616 entradas
del enlace dice que `inittmr` es su ÚNICO referenciador en todo el proyecto**.

Nuestro objeto emite su propio pool (`$LC0`, `$LC1`, `$LC2`: 24 B, `align 8`) y
deja de referenciarlos, así que `-strip-unused-data` se lleva los
**8 + 8 + 152 = 168 B** y nosotros ponemos **24**: `168 − 24 = 144`, que es
exactamente el `rodata-144`.

**Control que lo confirma**: enlazando con un `keep.lst` al que se le añade
`lbl_80413EC8`, el delta pasa de **−144 a +8** (vuelven los 152 B del símbolo).

### Qué hay de verdad en 0x80413EB8 (leído del ELF original)

```
80413EB8  43300000 00000000     <- $LC0 nuestro, byte a byte
80413EC0  43300000 80000000     <- $LC1 nuestro, byte a byte
80413EC8  3f800000              <- $LC2 nuestro (1.0f)
80413ECC  00000000              <- relleno (nuestra .rodata mide 24 B, align 8)
80413ED0  ... 144 B de "Assert: %s:%i (%s)" repetido, que NO son de aqui
```

Es un pool de constantes de GCC completo y contiguo — los dos sesgos
`int→double` y el `1.0f`, **en el mismo orden en que los emite nuestro
compilador** — pegado detrás de la última `.rodata` de la unidad anterior
(`_vt.20GcHdFileDeviceDriver`, `0x80413E38 + 0x80 = 0x80413EB8`) y delante de un
bloque de cadenas que esta unidad no tiene.

**El orden de enlace lo respalda**: `dvd_device` (488) < `auto_05_80413E30`
(489) < `hd_device` (490) < **`inittmr` (491)** < `signals` (492).

**Sobre la prueba de los `STT_FILE` que pedía el encargo**: no la hay *dentro*
del rango, y hay que decirlo — son entradas anónimas de pool y el ELF original
**no tiene ni un símbolo local** entre `0x80413EB8` y `0x80413F60` (volcado de
los 3.845 locales con su `STT_FILE`). Los `STT_FILE` sí acotan **por fuera**: el
siguiente local con fichero es `_9RealShape.gTexelTypeToBpp`
(`FILE=shpcreate.cpp`) y está en `0x80414010`. La prueba fuerte es la de arriba:
contenido byte a byte, tamaño de sección (24 B = `0x80413ED0 − 0x80413EB8`),
alineación 8, orden de enlace, y **referenciador único con addend 0**.

### PROPUESTA — las líneas exactas (regla 4: propongo, no aplico)

**1) `config/GOWE69/splits.txt`**, en el bloque de
`Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp:`, entre `.text`
y `.bss` (respetando el TAB inicial):

```
	.rodata     start:0x80413EB8 end:0x80413ED0
```

**2) `config/GOWE69/symbols.txt`** — la línea 33094 tiene hoy `size:0x98`
(152 B), que se traga 144 B de cadenas ajenas. Sustituir:

```
lbl_80413EC8 = .rodata:0x80413EC8; // type:object size:0x98 align:4 data:float
```
por
```
lbl_80413EC8 = .rodata:0x80413EC8; // type:object size:0x4 data:float
lbl_80413ED0 = .rodata:0x80413ED0; // type:object size:0x90 data:string
```

**3) `config/GOWE69/keep.lst`** — añadir (el fichero es **CRLF**, ver
`memory/crlf-grep-miente`):

```
lbl_80413ED0
```

**Sin la 3) no funciona**, y es el caso de `memory/nfsmw-rango-no-basta`: esos
144 B **no los referencia nadie** en el enlace — hoy sobreviven sólo por ir
dentro de `lbl_80413EC8`, que referenciamos nosotros —, así que al partir el
rango `-strip-unused-data` se los lleva y el `.rodata` vuelve a salir −144.

**Control de éxito**, en este orden:
`linkdelta.py --all Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr`
tiene que dar `.text +0   IGUAL`, y entonces `trypromo.py` sobre esa unidad
**DOL OK**.

**Riesgo que hay que mirar al re-extraer**: el comodín queda partido en dos
(`0x80413E30..0x80413EB8`, 136 B, y `0x80413ED0..0x80414398`, 1.224 B) y el
segundo trozo tiene que quedar **detrás** de `inittmr` en la lista de enlace.
Hoy `auto_05_80413E30` está en el índice 489 y `inittmr` en el 491, así que hay
sitio, pero eso lo decide el ordenador de `configure.py` y **no lo he podido
comprobar sin re-extraer**. Es la comprobación que falta.

*(Nota: hay dos objetos huérfanos en disco, `auto_05_804130A0_rodata.o` y
`auto_05_80413958_rodata.o`, que también definen los tres símbolos. **No están
en la lista de enlace** — son restos de un `configure` anterior. No estorban,
pero conviene saberlo antes de asustarse con un `grep`.)*

### Lo aplicado en inittmr

`.align 3` en el primer `asm` de `.sdata`:

```c
asm(".section \".sdata\"\n .align 3\n .byte 0\n");
```

El objeto extraído trae `.sdata` con `sh_addralign 8` y el nuestro lo emitía con
4. **HOY ES NEUTRO** (`0x804FF628` ya es múltiplo de 8, `linkdelta` no se mueve
y `fncmp` sigue en 0 de 4), pero es exactamente la diferencia que en
`criticalpath` costaba 13 B de DOL. `.sdata` pasa a `8 B / align 8`, idéntico al
extraído. Si el jefe prefiere el diff mínimo, se puede quitar sin perder nada
medible hoy.

---

## 6. Lo que hay que llevarse de aquí

1. **Barrer `sh_addralign` en las 616 unidades.** Tres de mis cuatro lo tenían
   mal y ninguna herramienta del proyecto lo mira. Coste: un script de 20
   líneas. En `criticalpath` valía 13 B y llevaba **trece rondas** escondido
   detrás de un «LIMPIA».
2. **La familia C existe y está probada** (`prio` 2→6, arco nuevo, cero bytes),
   **y es negativa en sus dos miembros baratos.** Los 11.024 B de la familia no
   se cierran por ahí; el mecanismo sirve para otra cosa.
3. **`inittmr` es la promoción más barata que le queda al proyecto**: tres
   líneas de configuración y una re-extracción. Ya no le falta código.
4. **Dos recetas del informe r60b hay que tacharlas**: `int s4 = src[4];` de
   `IdctColumn` (objeto byte-idéntico) y la lectura de la r48 que decía que la
   palanca `rec` de `VP6` vale sola (no vale: 40 filas, cero efecto).

**Cero correcciones de `lcfix.py` pendientes. Cero entradas venenosas de
`keep.lst` detectadas.** Las tres propuestas de configuración están en §5 y
ninguna se ha aplicado.
