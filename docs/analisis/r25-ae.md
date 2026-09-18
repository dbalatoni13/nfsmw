# Ronda 25 — zEagl4Anim + zEAXSound

Encargo: `EvalSQT…FnStatelessQ` (el «trabajo obvio»), `Initialize…DynamicLoader`,
y los muros de las dos unidades.

## 0. Línea base VERIFICADA

`build_direct.py zEagl4Anim zEAXSound` + `triage.py`, medidos por mí:

    2352 B  91.33673%  zEagl4Anim  Initialize__…DynamicLoader   faltan 14, sobran 5, 10 SUST
    2152 B  91.81599%  zEAXSound   Play__14cSampleWarpper       reorden local, dmax 7
    1480 B  99.45946%  zEagl4Anim  EvalSQT__…FnStatelessQ       faltan 0, sobran 2 · clrlwi-2
    1008 B  98.67063%  zEAXSound   SetupNextLoad                2 filas
     896 B  98.16964%  zEAXSound   UpdateParams__16SFXCTL_MasterVolf   49 registros
     736 B  99.86413%  zEAXSound   QueueFileLoad                5 registros   <-- el encargo dice 796 B
     720 B  97.36100%  zEagl4Anim  FindMatchTime                4 filas
     488 B  98.85200%  zEagl4Anim  Eval__…FnRawLinearChannel    22 registros
     456 B  93.50877%  zEagl4Anim  EvalState__…FnRawStateChan   faltan 2, sobran 2, 2 SUST

**Una discrepancia con el encargo**: `QueueFileLoad` mide **736 B**, no 796.
Todo lo demás coincide instrucción a instrucción.

`audit.py` al empezar: **zEagl4Anim 313 ok / 0 FALLA**, **zEAXSound 763 ok /
0 FALLA**. Ningún FALLA que confirmar con segunda pasada.

`EAXAemsManager.cpp` y `SFXCTL_MasterVol.cpp` aparecían modificados en el
`git status` del encargo: cuando empecé **ya estaban limpios** (`git diff` vacío,
`git status --porcelain` no los lista). No he tirado nada.

---

## 1. RESULTADO

| | antes | después |
|---|---|---|
| `Initialize__Q25EAGL413DynamicLoader…` (2.352 B) | 91,33673 % · 264 diffs · 579 ins | **93,88776 % · 227 diffs · 586 ins** (+2,551 pp) |
| todas las demás | — | sin tocar |

`measure.py --cmp` (base y medida seguidas, `build_direct.py` delante de cada
una): **+0 B, +0 funciones, 0 unidades cambian**.
`pctsnap.py --cmp`: **EMPEORAN: ninguna · MEJORAN: 1** (+2,551 pp).

`audit.py` al cerrar: **zEagl4Anim 313 ok / 0 FALLA**, **zEAXSound 763 ok /
0 FALLA**. `frozen.py`: `zEAXSound` **idéntico al congelado** (no la he tocado);
`zEagl4Anim` **re-congelado** (`bf7b70bb6d586e14`) porque es mía y la he cambiado.

**Fichero tocado, uno solo: `src/Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.cpp`**
(8 inserciones, 9 borrados). Ninguna cabecera. Sin commit.

    -        int j;
    -        for (j = 0; j < pHP->symbols_num; j++) {          (y los 5 usos de j)
    +        for (i = 0; i < pHP->symbols_num; i++) {          (y los 5 usos de i)
    -            if (sym->st_shndx > 0 && sym->st_shndx < pHP->e->e_shnum) {
    +            if (sym->st_shndx > 0 && sym->st_shndx < e->e_shnum) {
    -                    len = len - 1 - slen;
    +                    len -= slen + 1;

Las tres son **semánticamente idénticas** al código anterior (`e == pHP->e`, la
asignación está 60 líneas más arriba; `len-1-slen == len-(slen+1)`).

---

## 2. `Initialize__…DynamicLoader` — de qué salieron los 2,55 pp

Tres rondas la dieron por vedada por el árbol del `switch`. **La ganancia no
estaba ahí: estaba en el DWARF, y `regmap.py --ours` la enseña en una pasada.**

    fn   e      -   r1+0x94   <-- DISTINTO
    b1   sym    r4  r30       <-- DISTINTO
    b1   j      -   r29       <-- DISTINTO   (el original NO la tiene en registro)
    b1   hv         -         <-- SOLO NUESTRA

### 2.1 El contador del último bucle: el objetivo lo tiene EN PILA (+1,85 pp)

El objetivo abre el bucle de símbolos con `li r6,0 ; stw r6, 0x90(r1)` y lo
recarga **cinco veces** (`lwz r6,0x90(r1)` antes de la comparación, del
`slwi ·,2`, del `stwx` y del incremento). `0x90(r1)` es la ranura de `i`, la
variable de ámbito de función que usan los dos bucles anteriores. Nosotros
declarábamos un `int j` propio y GCC lo dejaba en **r29**.

Reusar `i` reproduce el vertido exacto: **91,337 → 93,189 %**, 585 ins (objetivo
588; nosotros teníamos 579).

**Deuda declarada**: el DWARF del original **sí lista una `j` en ese bloque**
(sin localización, o sea en pila y compartiendo ranura con `i`). Mi cambio la
elimina del árbol, así que es fiel al *código emitido* pero no al *DWARF*. La
forma fiel a las dos cosas sería conservar `int j` y que la presión la echase a
la pila — y esa presión no la tenemos (véase §2.4).

### 2.2 `e->e_shnum` en vez de `pHP->e->e_shnum` (+0,04 pp, y es del asm)

Fila 563 del diff: el objetivo hace `lwz r7, 0x94(r1)` (la local `e`) y nosotros
`lwz r9, 0x1c(r31)` (`pHP->e`). El original usa la local. Sola vale poco; con
§2.1 suma (93,189 → 93,633).

### 2.3 `len -= slen + 1;` (+0,25 pp)

Filas 438-441. El objetivo emite `subi r9,r26,1 ; … ; subf. r26,r27,r9`, o sea
`(len-1) - slen` en dos pasos; nuestro `len = len - 1 - slen;` compartía el
`slen+1` de la línea siguiente (`s += slen + 1`) y GCC lo fundía. Barrido de las
5 asociaciones:

    base   `len = len - 1 - slen;`      93,63265  585 ins  235 diffs
    g1     `len = len - slen - 1;`      93,39455  586      234
    h2     `len = (len - 1) - slen;`    93,63265  585      235   (idéntico a base)
    h3     `len--; len -= slen;`        93,81123  586      232
    h4     `len = len-1; len = len-slen;` 93,81123 586     232
    **g4   `len -= slen + 1;`**         **93,88776  586      227**  <- APLICADA

### 2.4 EL HALLAZGO GORDO: el pin de `pHP` **compila mal** (y es previo a mí)

`eagl4supportdlopen.cpp:410` lleva `register HashPointer *pHP asm("r31")`.
En el OBJETIVO, **r31 se escribe UNA sola vez en toda la función** (`mr r31,r3`,
fila 13): `pHP` vive en r31 de principio a fin. En el nuestro **r31 se escribe
cinco veces** (filas 15, 79, 179, 354 y una recarga `lwz 31,156(1)`), o sea GCC
mete ahí también el `e_shoff` byte-volteado y dos veces `sheader`.

Eso no es sólo reparto distinto: **produce código incorrecto**, y se ve al ojo en
el `case SHT_SYMTAB` (presente ya en el árbol ANTES de mi cambio):

    lwz 0,16(31)      ; sheader->sh_voffset   (r31 = sheader)
    stw 0,20(31)      ; pHP->symtab = ...     (r31 = pHP)      <-- el MISMO r31
    mr  9,0           ; sheader->sh_size ...  reusa lo recién escrito
    …
    stw 0,16(31)      ; pHP->symbols_num      (0x10 de pHP == 0x10 de sheader)

El objetivo usa **dos** registros (`lwz r0,0x10(r30)` / `stw r0,0x14(r31)` /
`lwz r9,0x14(r30)`). `pHP` (bloque de 0x430 B de `EAGL4Malloc`) y `sheader`
(dentro del ELF) no pueden ser la misma dirección: `pHP->symbols_num` se está
escribiendo encima de `sheader->sh_voffset`.

**Causa**: r31 es `FRAME_POINTER_REGNUM` en rs6000. Comprobado con medida: el
mismo pin sobre **r30, r29 o r28 NO tiene el fallo** — ahí `pHP` sale con **una
sola escritura**, igual que el objetivo, y el `lwz` del `sh_size` vuelve.

Alternativas medidas (sobre el árbol ya con §2.1-2.3):

    pin r31 (actual, INCORRECTO)   93,88776 %   227 diffs   31 filas estructurales
    pin r30                        88,94898 %   312 diffs   62 filas estructurales
    pin r29                        89,05102 %   302         62
    pin r28                        89,06802 %   303         62
    sin pin (`HashPointer *pHP`)   79,83843 %   441         (pHP se va a la pila)
    `register` sin asm             = sin pin

O sea: **el pin correcto cuesta ~5 pp y duplica las filas estructurales**, y
quitarlo cuesta 14 pp. **No lo he cambiado** — es una decisión que no me
corresponde tomar sola; lo dejo medido y documentado. Si se decide corregirlo,
`asm("r30")` es la opción barata y correcta.

Y es también la razón de fondo de las ~200 filas `DIFF_ARG_MISMATCH` que quedan:
el objetivo tiene **un registro menos** (r31 ocupado por `pHP` de verdad), y por
eso vierte `i*0x28` a **CTR** (`mtctr r8` / `mfctr r10`, filas 173 y 316) y
recarga `0x98(r1)` donde nosotros nos permitimos un registro.

### 2.5 Lo que queda en `Initialize` (227 diffs, 31 estructurales)

1. **filas 137-138** — el cross-jump de `ELFAddr`. El objetivo funde las dos
   copias de `return &mpData[offset];` porque escribe el resultado en **r30 en
   las dos** (r30 es además el `offset`, que muere ahí); nosotros en r9 y r6.
   Es consecuencia del reparto (§2.4), no de la fuente: `eagl4supportdlopen.h`
   ya tiene las dos sentencias idénticas.
2. **filas 173-184 y 314-322** — `mtctr`/`mfctr`: presión (§2.4).
3. **filas 352-366** — el árbol del `switch`. El objetivo emite
   `cmplwi r0,8 ; ble → subárbol izquierdo` y deja el **derecho** en línea;
   nosotros `bgt → derecho` y el izquierdo en línea, y por eso nos sobra un `b`
   (fila 366). En `stmt.c: emit_case_nodes` eso es la rama
   `node_is_bounded(node->left)`; con nuestra lista de 7 nodos
   (`[2] [3] [4-7] [8] [9] [L+5,L+6] [L+7,HI]`) `balance_case_nodes` deja la raíz
   en `[8]` y **ninguno de los dos subárboles queda acotado**, así que cae en el
   `else` (que es lo nuestro). Para que salga el del objetivo hace falta que el
   subárbol izquierdo no tenga hijo izquierdo. **No lo he tocado** (veda de la
   r20/r21: 88 árboles y 512 listas de `case`), pero ahora está dicho *qué*
   condición del compilador hay que romper, que antes no lo estaba.
4. **fila 381** — el `mr r9, r0` del §2.4.
5. **filas 433/435** — `&s[nameLength] + 1` contra `&s[nameLength + 1]`.
   Probado (`g2` y `h1`): **peor** (93,459 y 93,714 contra 93,888). Revertido.

Ensayos que NO entraron (todos medidos, base 91,33673 salvo aviso):

    e1  `e->e_shnum`                             91,37245
    e2  `i` en vez de `j`                        93,18877
    e3  e1+e2                                    93,63265
    k1  e1 + `len -= slen+1` (conserva `j`)      91,86735
    k2  e2 + `len -= slen+1`                     93,44388
    i1  `sheader` declarada dentro de los bucles 77,73470   MUCHO PEOR
    i2  `int symSize` leída antes del store      93,52211   PEOR que g4
    n1  `register … *sheader asm("r30")`         78,92517   MUCHO PEOR
    f1/f2/f3  sin pin / pin r30 / pin r14        79,84 / 88,46 / 87,84

---

## 3. `EvalSQT__…FnStatelessQ` (1.480 B) — el mecanismo, al insn, y sigue sin cerrar

El encargo pedía «crecer el caso mínimo hasta que `combine` se rompa».
**Lo he hecho al revés y ha salido antes: he ENCOGIDO el fichero real.**

### 3.1 Detector nuevo: contar máscaras, no porcentajes

El objetivo tiene **una** `rlwinm rX,rY,0,0xffff` en `EvalSQT` (la del
`floorTime >= mNumKeys` de la línea 64, que también usa la línea 66); nosotros
tenemos **tres**. Contar esa máscara sobre el `.s` es un detector exacto y no
depende del ruido del planificador, que es lo que había estado guiando las
rondas 21-24. Arnés: `c25ae_var.py` (compila una copia del `.cpp` en el
scratchpad con los cflags exactos, 10 s por variante, **sin tocar el árbol**);
verificado que reproduce el `.o` real al bit (99,45946 % / 1.488 B).

### 3.2 De los dos guardias, el que rompe es el de la línea 74

Recortando `EvalSQT` a su cabecera (127 instrucciones):

    los dos guardias (74 y 132)   mask=3   (1 legítima + 2 nuestras)
    sólo el guardia 132           mask=0   FUNDE
    sólo el guardia 74            mask=2   NO funde
    ninguno                       mask=0

### 3.3 La causa, en el volcado RTL: el pseudo de PRE tiene CINCO sets

`c25ae_rtl.sh` sobre el fichero sin cast deja los 16 volcados. En `.combine`:

    (insn 1431 (set (reg:SI 592)
        (plus:SI (subreg:SI (reg:HI 590) 0) (const_int -2))))   <- FUNDE
    (insn 152  (set (reg:SI 143) (zero_extend:SI (reg:HI 628))))<- NO funde

`reg:HI 590` es la carga de `mNumKeys` de la línea 96 (`mNumKeys - 2`), con
**un solo set, y de MEM**. `reg:HI 628` es el pseudo que crea **PRE** para los
dos guardias, y tiene **cinco sets**: tres `(mem:HI 0x14(...))` y **dos copias**
`(set (reg:HI 628) (reg:HI 119))` y `(set (reg:HI 628) (reg:HI 590))`.

> **Regla que deja la ronda: `combine` sólo funde `(plus (zero_extend (reg:HI P)) K)`
> en `(plus (subreg:SI P) K)` cuando P es de UN SOLO SET y ese set viene de
> memoria** (ahí `nonzero_bits` sabe, por `LOAD_EXTEND_OP == ZERO_EXTEND`, que
> los 16 altos son cero). Un pseudo con sets múltiples que llegan de otro
> registro lo bloquea. **Es lo que hay detrás de las 40 formas vedadas de la
> sentencia en las rondas 21, 22 y 24: la sentencia no era el problema.**

### 3.4 Y por eso mismo no se puede cerrar «por fuente» sin romper la línea 64

Todo lo que convierte el operando en un pseudo limpio hace fundir el guardia
**y además borra la máscara de la línea 64**, que el objetivo SÍ tiene:

    z3   `int numKeys` arriba del bloque, usada en 64/66/74      96,44325  369 ins  mask=1
    z20  `int numKeys` asignada en las DOS ramas                 96,91892  369      mask=1
    z25  igual con `unsigned short numKeys`                      96,91892  369      mask=1
    z28/z29  local en las dos ramas, 64/66 con el campo          96,91892  369      mask=1
    w6   el tratamiento completo (los dos guardias)              95,43243  367      mask=0
    base (con cast)                                              99,45946  372      mask=3
    OBJETIVO                                                              370      mask=1

O sea: **369 y mask=1 (borra la de la 64) o 372 y mask=3.** El objetivo, 370 con
mask=1, exige que la línea 64 NO sepa `nonzero_bits` y los guardias SÍ — y eso
sólo pasa si el operando de los guardias es un pseudo distinto y limpio, cosa
que PRE deshace en cuanto la carga es parcialmente redundante.

Otros ensayos, todos peores o iguales:

    z1  `floorKey + 1 >= mNumKeys`               99,24324  mask=3
    z2  `floorKey > mNumKeys - 2`                97,86216  mask=3
    z4  `int numKeys` justo delante del guardia  98,13243  mask=3
    z5  `(int)mNumKeys - 1`                      98,13243  mask=3
    z8  `floorKey = mNumKeys; floorKey -= 1;`    97,26757  mask=3
    z9  guardia fundido con `slerpReqd`          93,68649  mask=3
    z11 `(mNumKeys & 0xFFFF) - 1`                98,13243  mask=3
    z15 `int lastKey = mNumKeys - 1` izado       95,65946  mask=1  367 ins
    z16 `(unsigned short)mNumKeys - 1`           98,13243  mask=3
    z22 `int numKeys` DESPUÉS de la cadena de if 98,13243  mask=3
    w9  `if (floorTime >= mNumKeys)` primero     96,47298  mask=3
    w10 `||` + ternario                          94,56757  mask=3

**Banderas, sólo como diagnóstico** (nunca aplicadas): con el cast y sin él,
`-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`,
`-fno-rerun-cse-after-loop`, `-fno-expensive-optimizations`,
`-fno-schedule-insns`, `-fno-schedule-insns2` — **ninguna hace fundir**, y
`-fno-gcse` (que es la que mataría a PRE) es la peor de todas (91,6 / 92,2 %).
Eso descarta también que baste con quitar PRE.

**Veda ampliada**: la sentencia
`floorKey >= static_cast<unsigned short>(statelessQ->mNumKeys - 1)` suma ya
**~55 formas** entre las cuatro rondas. Lo que falta no es una forma de la
sentencia: es una fuente que le dé a los dos guardias un pseudo de un solo set
sin que la línea 64 pierda su `zero_extend`.

---

## 4. `UpdateParams__16SFXCTL_MasterVolf` (896 B, 98,170 %) — dos diferencias del DWARF, ninguna accionable

`dwbody.py` da diferencias estructurales de verdad, y de las gordas:

| | original | nuestro |
|---|---|---|
| locales de función | `fvol`, `RandarRange` (r9) | + **`__ct8` (r9)** temporal de constructor |
| dentro del bloque | `DemoDiscManager::IsActive()`, `DemoDiscManager::GetMasterVolumeScale()` | — |
| dentro del bloque | (ninguna) | **7 × `EAXSound::GetCurAudioSettings()`** |
| `fMasterVol` | f6 | f11 |

Pero el asm dice que **no valen bytes**: los dos `DemoDiscManager::*` del
original son expansiones de **rango cero** (los dos ficheros tienen 224
instrucciones y el diff no trae ni un INSERT ni un DELETE en esa zona) — es una
función de disco de demo que en retail pliega a nada. Y los siete
`GetCurAudioSettings` nuestros emiten **exactamente** el `lwz r9, 0x34(r7)` que
emite el objetivo: el original leía el campo sin pasar por el accesor (por eso no
deja registro de inline), pero el código es el mismo.

Lo que queda son **49 registros de coma flotante permutados** (`fMasterVol`
f6↔f11, el `1.0f` f9↔f12) repetidos en los seis `SetDMIX_Input`. **No la he
tocado.**

---

## 5. `Eval__…FnRawLinearChannel` (488 B) — cuidado, `regmap` está emparejando mal

`regmap.py zEagl4Anim "FnRawLinearChannel::Eval"` da un original con **cuatro
parámetros** (`Eval(float, float, float*, float /* f1 */)`) y **cero locales con
localización**, contra nuestros tres parámetros del símbolo
`Eval__Q29EAGL4Anim18FnRawLinearChannelffPf`. Es el caso que avisaba el encargo:
**homónimos**. No me he fiado y no he trabajado sobre ese volcado. Quien siga:
filtrar por `low_pc` antes de leer nada.

---

## 6. `QueueFileLoad` (736 B, 99,864 %) — cinco filas, y son un empate de `local-alloc`

Las cinco filas (67-75) son la copia de `stSndAssetQueue currequst = *i;`:

    70 lwz r11, 0x28(r8)   |  lwz r0, 0x28(r8)
    71 stw r11, 0x48(r1)   |  stw r0, 0x48(r1)
    72 cmpw r11, r27       |  cmpw r0, r27
    73 lwz r0,  0x2c(r8)   |  lwz r9, 0x2c(r8)
    74 stw r0,  0x4c(r1)   |  stw r9, 0x4c(r1)

El objetivo rota **r0 → r9 → r11 → r0** por las cuatro palabras de la copia;
nosotros **r0 → r9 → r0 → r9**. Mismo código, mismo tamaño, mismo orden.
`regmap.py` **no encuentra la función en el volcado original** (`no encuentro
"EAXAemsManager::QueueFileLoad"`), así que no hay contraste de locales.
No la he tocado. (Nota: en su cuerpo hay un `asm("" : : "m"(framePad[0]))`
inventado nuestro, de una ronda anterior; no lo he tocado ni contado.)

---

## 7. Herramientas dejadas en el scratchpad (prefijo `c25ae_`)

* **`c25ae_var.py <sym> <var.cpp>…`** — el caballo de batalla: compila **copias
  del `.cpp` que viven en el scratchpad** con los cflags exactos de zEagl4Anim,
  saca `.s` y `.o`, mide con objdiff y **cuenta las máscaras de 16 bits** del
  cuerpo del símbolo. No toca el árbol, así que es inmune a los otros agentes.
  10 s por variante. `DUMP=<tag>` vuelca las filas con diff.
* **`c25ae_var2.py`** — el mismo para `Initialize`, contando escrituras a r31.
* `c25ae_asm.sh` — compila un `.cpp` del scratchpad a `.s` con los cflags de la
  unidad (más `-I src/Speed/Indep/Src/EAGL4Anim` para que los includes relativos
  resuelvan).
* `c25ae_rtl.sh` — los 16 volcados por pase de un `.cpp` del scratchpad
  (receta de la r22, con los `-I/-D` de la unidad).
* `c25ae_fl.py` — barrido de banderas sobre un fichero DEL ÁRBOL, con parche
  temporal (`PATCH=fich|||viejo|||nuevo@@@…`) y restauración garantizada.
* `c25ae_sw.py`, `c25ae_rows.py`, `c25ae_full.py`, `c25ae_det.py` — copias
  renombradas de los arneses de r21/r22 más el detector de máscaras.
* `c25ae/` — `q0.cpp` (copia de `FnStatelessQ.cpp`), `q_nc.cpp`, `v_*.cpp` (los
  ~30 ensayos de §3), `dl0.cpp`/`dl_now.cpp` (`eagl4supportdlopen.cpp` antes y
  después), `d_*.cpp` (los ~20 de §2), y los listados `q_full.txt`,
  `dl_full.txt`, `e3_full.txt`. Los 16 volcados RTL de §3.3 los he **borrado**
  para no llenar el disco (quedan 13 GB): se rehacen con
  `sh c25ae_rtl.sh <SCR>/c25ae/q_nc.cpp <SCR>/c25ae/rtl_nc` en 20 s.
* `c25ae_{antes,post}.json`, `c25ae_pct_{antes,post}.json`.

---

## 8. Qué NO he probado

* **`Initialize`**: no he tocado el árbol del `switch` (§2.5.3) — sé qué
  condición de `emit_case_nodes` hay que romper, no he buscado la lista de
  `case` que la rompe. No he probado `permuter.py`. No he probado la referencia
  `HashPointer &h` que pide el DWARF (la r21 la midió neutra, no la he
  re-medido sobre la base nueva). No he intentado reconstruir el cross-jump de
  `ELFAddr` por fuente. **No he cambiado el pin de r31 a r30** pese a estar
  medido que r31 compila mal (§2.4): eso es una decisión de proyecto.
* **`EvalSQT…FnStatelessQ`**: no he instrumentado `try_combine`; no he probado a
  declarar `mNumKeys` como campo de bits en `StatelessQ.h` (cabecera compartida
  con `FnStatelessQ.cpp` y `StatelessQ.cpp`), que es la única vía que se me
  ocurre para cambiar el modo del pseudo sin tocar la sentencia; no he probado el
  permutador.
* **`EvalState…FnRawStateChan`** (456 B): **cero ensayos**. La r24 la dejó con
  37 formas vedadas y el diagnóstico de `gcse`/copy-props; no he vuelto.
* **`Play__14cSampleWarpper`** (2.152 B): cero ensayos; sigue el diagnóstico de
  la r24 (falta el registro salvado que mantiene vivo el `0` de `filter_HiPass`).
* **`FindMatchTime`, `SetupNextLoad`, `startnextrequest`, `DataLoadCB`,
  `ResolveCurrentDataMemory`, `MsgPlayMiscSound`, `Play__13cStichWrapper`,
  `UpdateRPM`**: no tocadas. `SetupNextLoad` es una permutación limpia
  (`this` r29→r27, r26→r31) y `regmap` **tampoco la encuentra** en el volcado
  original.
* **`UpdateParams`**: no he probado a añadir el guardia de `DemoDiscManager`
  (§4) para recuperar los dos inline de rango cero: es fidelidad de DWARF, no
  bytes, y `DemoDiscManager` habría que escribirlo.
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas.
* **El DOL**: no lo he construido (va en la verificación de la tanda).
* **`configure.py`, `config/GOWE69/*`, `splits.txt`**: intactos. Ningún
  ensamblador escrito. Ningún `#if defined(__ANDROID__)` tocado.
