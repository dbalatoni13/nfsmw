# Ronda 28 — `linkonce`: la pista era real y es un CALLEJON SIN SALIDA

Encargo: los ~303.000 B de `.text` que nadie compara, la pregunta del
`.gnu.linkonce`, y la tabla de `-fno-implement-inlines` por biblioteca.

## 0. Las tres frases

1. **Sí, este `cc1plus` emite `.gnu.linkonce.t.*`. Lo que lo apaga es `-gdwarf+`**
   (cualquier `-g` de nivel ≥ 2). Bisecado sobre un caso mínimo: probando las
   banderas de `cflags_game` una a una, y luego **todas juntas menos `-gdwarf+`**,
   la única que lo cambia es `-gdwarf+`.
2. **Y da igual, porque el enlace ya hace lo mismo.** `ngcld -strip-unused-data`
   —que es la bandera real del enlace, `build.ninja:8`— **tira los cuerpos weak
   duplicados exactamente igual que `linkonce`**. Medido: los mismos dos objetos
   dan 252 B sin la bandera, **184 B con ella**, y **184 B** con `linkonce`.
3. **Por lo tanto los ~303.000 B NO bloquean la promoción, ni en biblioteca ni en
   SourceList.** Sustituyendo nuestro `zLua.o` (con +18.908 B) el `.text`
   enlazado sale en **3.804.440 B, el del original clavado**, y tres unidades dan
   **el mismo sha1 de DOL con y sin el exceso**. La premisa del encargo
   («en SourceLists SÍ bloquea») **es falsa, y está medida en el enlace.**

Lo que sí desplaza el DOL es **`.rodata` / `.data` / `.sdata`**, y ahí la
diferencia es sobre todo **negativa: nos faltan constantes** (§5).

---

## 1. La pregunta del `linkonce`, contestada con la prueba

### 1.1 El compilador SÍ las emite

Caso mínimo (`c28lo/t1.cpp`, plantilla `Vec<T>::reserve` instanciada
implícitamente), `ngccc.exe -O1 -S`:

```
 3:  .section  ".text"
 5:  .globl    use__FRt3Vec1Z13WCollisionTri
20:  .section  ".gnu.linkonce.t.reserve__t3Vec1Z13WCollisionTrii","ax"
22:  .weak     reserve__t3Vec1Z13WCollisionTrii
```

Y en el `.o`: 7 secciones, la 2 es
`.gnu.linkonce.t.reserve__t3Vec1Z13WCollisionTrii` (20 B), con el símbolo
**WEAK** (`bind 2`) apuntando a ella.

### 1.2 Lo que la apaga es `-gdwarf+`

Con los `cflags` reales de `zLua` la sección desaparece y el símbolo **sigue
siendo WEAK, pero cae en `.text` plano (shndx 1)**. Bisecado una a una:

| banderas | resultado |
|---|---|
| `-O1` | **linkonce** |
| `-O1 -G0` · `-mps-nodf` · `-x c++` · `-fno-static-dtors` · `-ffast-math` | **linkonce** |
| **todas las de `cflags_game` MENOS `-gdwarf+`** | **linkonce** |
| `-O1 -gdwarf+` | `.text` plano |
| `-O1 -g` · `-g2` · `-g3` · `-gdwarf -g2` · `-gdwarf-2` | `.text` plano |
| **`-g1`** | **linkonce** |

Es el conflicto clásico de GCC 2.x: DWARF-1 necesita un `.text` contiguo
(emite `text_begin`/`text_end` en `.debug_aranges`), así que con información de
depuración completa el compilador renuncia a las secciones one-only. A nivel 1
no hay rangos y vuelven.

### 1.3 Ninguna bandera la recupera

Barridas sobre `-gdwarf+`: `-fno-opt-dwarf`, `-fopt-dwarf` (son extensiones de
SN, están en el `--help` de `cc1plus`), `-fweak`, `-ffunction-sections`,
`-falt-external-templates` → **siguen en `.text` plano**.
`-fno-implicit-templates`, `-fexternal-templates` y `-frepo` sí cambian algo,
pero **no emitiendo la instanciación en absoluto** (`weak=0`, `linkonce=0`).

### 1.4 Pero la VERSIÓN del compilador sí

Mismo caso mínimo, mismo `-gdwarf+`:

| ProDG | sin `-g` | con `-gdwarf+` |
|---|---|---|
| 3.5 | linkonce | **linkonce** |
| 3.5b140 | linkonce | **linkonce** |
| 3.7 | linkonce | **linkonce** |
| 3.8.1 | linkonce | `.text` plano |
| **3.9.3 (el nuestro)** | linkonce | `.text` plano |

O sea: el comportamiento cambió **entre 3.7 y 3.8.1**. No propongo cambiar de
compilador —el proyecto está al 97,67 % con 3.9.3— pero queda dicho: **si alguna
vez hace falta, ProDG ≤3.7 emite `linkonce` con `-gdwarf+`.**

---

## 2. Por qué el `linkonce` NO es la palanca: `-strip-unused-data` ya lo hace

El enlace real (`build.ninja:8`) es
`ngcld -strip-unused-data -keep config\GOWE69\keep.lst -T ldscript.ld`.

Dos objetos que instancian la MISMA plantilla (`dupA.cpp` + `dupB.cpp`,
104 + 148 = **252 B** de código entre los dos):

| cómo | `.text` enlazado |
|---|---|
| `-gdwarf+` (weak en `.text` plano), `ngcld` **sin** `-strip-unused-data` | **252** — guarda las dos copias |
| `-gdwarf+`, `ngcld` **con** `-strip-unused-data` | **184** — **tira el duplicado** |
| sin `-g` (con `.gnu.linkonce.t.*`), `ngcld` sin la bandera | **184** — tira el duplicado |

**Las dos vías dan el mismo número.** El enlazador de SN ya deduplica por
símbolo weak cuando se le pide que quite lo que no se usa; el `linkonce` sólo
sería otra forma de decírselo.

### 2.1 Y comprobado sobre el enlace de verdad

`c28lo_promo2.py` sustituye un objeto extraído por el nuestro, enlaza el juego
entero y mide:

| unidad sustituida | exceso del `.o` | `.text` enlazado | vs original |
|---|---|---|---|
| `zLua` | **+18.908** | 3.804.440 | **+0** |
| `zAnim` | **+9.076** | 3.804.440 | **+0** |

Y el juez de verdad, el sha1 del DOL, **no se mueve al quitar el exceso**:

| unidad | `.o` base | `.o` con la bandera | sha1 del DOL |
|---|---|---|---|
| `zMiscSmall` | +3.996 B | +0 B (`-fno-implicit-templates`) | **`90bb55bd2030` los dos** |
| `zRender` | +4.028 B | +32 B (`-fno-implicit-templates`) | **`9b9aa1f0570b` los dos** |
| `zAttribSys` | +156 B | +0 B (`-fno-implement-inlines`) | **`83a4c41e8902` los dos** |

Quitar **3.996 B** de `.text` de `zMiscSmall` no cambia **ni un byte** del DOL.

---

## 3. El original tenía UNA copia de cada una, y por la misma vía que nosotros

Tres medidas, por si alguien quiere seguir el hilo:

1. **El `.text` del original no tiene cuerpos anónimos.** En los 31 rangos de
   SourceList, las **15.494** funciones del objeto extraído suman **exactamente**
   el tamaño de la sección: **hueco 0 B**, y **0** nombres inventados por el
   troceador (todos están en el `symtab` del ELF original). Si el original
   hubiera arrastrado duplicados weak sin deduplicar, se verían como bytes sin
   nombre.
2. **La copia superviviente está en la PRIMERA unidad del orden de enlace.** De
   los **323** símbolos que emitimos en 2+ SourceLists y que el original tiene
   una sola vez, en **303 (93,8 %)** el dueño es la primera unidad del orden de
   enlace entre las que lo emiten. El orden es alfabético y `zAI` va primero
   (`0x800034A0`). Los 20 fallos son casos en que **nuestro** `zAI` incluye de
   más (8 son de `Attrib::Collection`, que en el original son de `zAttribSys`).
3. **`.weak` no sirve de discriminante**: el ELF original tiene **0 símbolos
   WEAK** de 26.262… y nuestro `main.elf` también tiene **0** de 40.519. `ngcld`
   normaliza weak→global al enlazar. Ese dato no dice nada.

O sea: el original hacía lo mismo que hacemos nosotros **más el
`-strip-unused-data` que ya tenemos**. No hay nada que arreglar aquí.

### 3.1 El `ldscript` no era la pista que parecía

`config/GOWE69/ldscript.ld` recoge `*(.gnu.linkonce.t*)`, `.r*` y `.d*`. Pero:
(a) esos comodines son el estándar de cualquier `ldscript` de GCC, y (b) medido
con tres objetos, `ngcld` coloca lo que recoja ese comodín **al FINAL de
`.text`**, mientras que en el original las instanciaciones están **dentro** del
bloque de su unidad (p. ej. `reserve__…WCollisionTri` en `0x8003B72C`, a
`0x3828C` del principio de `zAI`, entre otras instanciaciones y justo antes de
`_GLOBAL_.I.`). Es la posición del **bloque diferido de `finish_file`** de una
unidad normal, no la de una sección `linkonce` recolocada.

Y el aviso `L0019: Symbol 'eFrameCounter' multiply defined` **no es de
plantillas**: es una variable global normal definida dos veces. Con `linkonce`
saldría igual.

---

## 4. De qué son los ~303.000 B, contado de tres formas

Medido con `scripts/seccdiff.py` y `scripts/extrasym.py` (nuevos, §7).
`.text` + `.over` + `.gnu.linkonce.t*` de nuestro `.o` contra el extraído:

| familia | unidades | `.text` de más |
|---|---|---|
| **SourceLists (ProDG)** | 31 | **317.888 B** |
| bibliotecas **mwcc** (`dolphinsdk`) | 79 | 81.560 B |
| bibliotecas ProDG (middleware) | 17 | 2.136 B |
| **total** | **127** | **401.584 B** |

### 4.1 Por si el símbolo existe o no en el original

Sobre las SourceLists, símbolo a símbolo contra `orig/GOWE69/NFSMWRELEASE.ELF`:

| familia | símbolos | B | % |
|---|---|---|---|
| **A** — existe en el original, en el rango de **otra** unidad | 1.226 | **158.240** | 49,8 |
| **B** — **no existe en ninguna parte** del original | 2.758 | **159.684** | 50,2 |

La A son instanciaciones que el original emitió **una vez** y nosotros hasta
**24** (`reserve__…vector<WCollisionTri>` en 22 unidades, `_M_insert__…_Rb_tree`
en 23, `find__H2ZPP6IModel…` en 22). **`-strip-unused-data` las tira: son
inocuas.**

La B es lo único que es «código que el original no tiene». Y lo que pesa ahí no
son accesores: son **plantillas que el original inlinó siempre**. El caso claro
es `Attrib::TAttrib<T>::Get`: el original define **TRES** instanciaciones en todo
el juego (`bool`, `UMath::Vector4`, `GCollectionKey`, las tres dentro de `zAI`),
y nosotros emitimos **23 tipos distintos en hasta 23 unidades**, 80 B cada una.
Sólo eso son ~38.000 B.

### 4.2 Por clase de símbolo

| familia | clase | binding | símbolos | B |
|---|---|---|---|---|
| A | plantilla | WEAK | 808 | 138.640 |
| B | plantilla | WEAK | 626 | 92.844 |
| B | no plantilla | global | 2.041 | 57.100 |
| A | no plantilla | WEAK | 417 | 19.544 |
| B | plantilla | global | 45 | 6.188 |
| B | no plantilla | local | 40 | 3.372 |
| resto | | | 7 | 236 |

**El 79 % (251.132 B) son símbolos WEAK «one-only»** —instanciaciones de
plantilla e inlines emitidos fuera de línea—, que es exactamente lo que
`-strip-unused-data` deduplica.

### 4.3 Cuánto explica cada causa

| causa | B de los 317.888 de SourceLists |
|---|---|
| duplicados de plantilla que el original tiene UNA vez (familia A) | 158.240 (49,8 %) |
| plantillas/inlines que el original **no tiene** (familia B, WEAK) | 92.844 + 6.188 = 99.032 (31,2 %) |
| globales no-plantilla que el original no tiene (accesores, funciones muertas) | 57.100 (18,0 %) |
| locales | 3.372 (1,1 %) |
| **de los cuales `-fno-implement-inlines` quita** | **43.488 (13,7 %)**, rompiendo 32 funciones |
| **de los cuales `-fno-implicit-templates` quita** | **346.516 — se pasa en 28.620**, rompiendo 1.207 |

---

## 5. Lo que SÍ bloquea la promoción: `.rodata` y `.data`

Con `zLua` sustituido, el ELF enlazado sale:

```
seccion      original    nuestro    delta
.text         3804440    3804440       +0
.rodata        312588     310608    -1980   <-- 
.data          267228     266748     -480   <--
.sdata           2796       2816      +20
.ctors            196        224      +28
.over          149912     149920       +8
```

`.rodata` **1.980 B más corta** desplaza todo lo que va detrás, y por eso el
4,03 % de los bytes de `.text` salen distintos **sin que `.text` cambie de
tamaño**: son las reubicaciones `lis/addi` apuntando a direcciones corridas. Los
primeros tramos malos están en `__16AITrafficManager…`, que es de `zAI` y ni
siquiera hemos tocado.

**Las 32 SourceLists tienen las tres secciones de datos descuadradas**, y casi
siempre por defecto (`zFe` −2.568 `.rodata` / −1.548 `.data`, `zFe2` −4.792,
`zWorld` −3.952, `zGameplay` −2.368, `zFoundation` −1.320…). **Ese es el frente,
no `.text`.** El detalle por unidad sale con `python scripts/seccdiff.py --sl --all`.

---

## 6. La tabla de `-fno-implement-inlines`

A/B sobre **unidades enteras** con sus `cflags` reales de `build.ninja`,
compilando a scratchpad (no toca `build/`). 424 unidades ProDG.

| familia | unidades | `.text` de más | la bandera quita | % |
|---|---|---|---|---|
| **SourceLists** | 33 | 317.904 | **43.488** | 13,7 |
| **bibliotecas ProDG** | 391 | 2.328 | **1.616** | 69,4 |
| **bibliotecas mwcc** (`dolphinsdk`, 79 unidades, 81.560 B) | — | — | **NO APLICA** | — |

`-fno-implement-inlines` es una bandera de GCC: las 79 unidades de `dolphinsdk`
son Metrowerks y **no la tienen**. Su exceso (81.560 B) es además el inocuo por
construcción del que ya hablaba la r27.

### 6.1 Por SourceList

| unidad | objetivo | base | +bandera | quita | rompe |
|---|---|---|---|---|---|
| zEagl4Anim | 113.016 | +6.916 | **−156** | 7.072 | 5 |
| zFe | 174.200 | +14.572 | +8.128 | 6.444 | 1 |
| zFe2 | 250.732 | +13.388 | +9.976 | 3.412 | 0 |
| zGameplay | 141.472 | +13.616 | +11.000 | 2.616 | 2 |
| zCamera | 125.008 | +12.508 | +9.900 | 2.608 | 0 |
| zPhysicsBehaviors | 236.176 | +14.996 | +12.560 | 2.436 | 7 |
| zAI | 272.796 | +7.128 | +4.696 | 2.432 | 2 |
| zEAXSound | 151.092 | +24.080 | +21.728 | 2.352 | 2 |
| **zFEng** | 71.460 | +2.296 | **+28** | 2.268 | 1 |
| zSpeech | 178.888 | +12.140 | +10.340 | 1.800 | 0 |
| zMain | 159.776 | +20.188 | +18.460 | 1.728 | 1 |
| zEAXSound2 | 170.256 | +13.752 | +12.332 | 1.420 | 0 |
| zWorld2 | 139.388 | +10.404 | +8.996 | 1.408 | 1 |
| zSim | 96.400 | +18.312 | +16.940 | 1.372 | 3 |
| zPhysics | 146.124 | +43.960 | +42.676 | 1.284 | 0 |
| zWorld | 160.956 | +11.660 | +10.592 | 1.068 | 3 |
| zFeOverlay | 141.224 | +8.656 | +7.776 | 880 | 3 |
| zAnim | 42.292 | +9.076 | +8.652 | 424 | 1 |
| **zAttribSys** | 48.776 | +156 | **+0** | 156 | 0 |
| zMisc | 78.008 | +8.360 | +8.208 | 152 | 0 |
| zFoundation | 36.000 | +348 | +280 | 68 | 0 |
| zBWare | 35.772 | +2.032 | +1.972 | 60 | 0 |
| zEcstasy | 145.884 | +8.956 | +8.928 | 28 | 0 |
| zDebug, zDynamics, zGameModes, **zLua**, **zMiscSmall**, zMission, zOnline, zPlatform, **zRender**, zTrack | | | sin cambio | 0 | 0 |

**Lo importante es la última columna.** Medido con
`c28lo_regres.py`, que compara el cuerpo de cada función **contra el objeto
extraído neutralizando los campos que parchea el enlazador** (los 26 bits de
`bl`/`b` y los 16 bits de las instrucciones con reubicación; sin eso salen 1.095
falsos positivos):

> **`-fno-implement-inlines` ROMPE 32 funciones que hoy casan y ARREGLA 0.**

Y esas 32 son **exactamente** los símbolos que pasan a referencia indefinida
—coincide al dígito con la columna `undef` del barrido—: son cuerpos que el
objetivo **sí tiene** y que hoy emitimos porque están en clase. Todo lo demás
sale **byte a byte idéntico**: la bandera no perturba la generación de código.

Los 32, por unidad: zPhysicsBehaviors 7 (`Construct__10EffectsCar…`),
zEagl4Anim 5, zSim 3, zWorld 3, zFeOverlay 3, zAI 2, zEAXSound 2, zGameplay 2,
zFe 1, zFEng 1 (`GetPCellData__9FEListBoxUlUl`, el que ya avisaba la r27),
zMain 1, zAnim 1, zWorld2 1.

### 6.2 Las 13 bibliotecas ProDG donde hace algo

| quita | base | +bandera | unidad |
|---|---|---|---|
| 260 | +448 | +188 | `rcmp/dev/source/decoder/cmn/rcmp_mad_codec` |
| 260 | +312 | +52 | `rcmp/dev/source/decoder/cmn/rcmp_vp6_codec` |
| 192 | +192 | **+0** | `mcard/3.04.01-layer2/source/lib/gc/gc_driver` |
| 144 | +200 | +56 | `realcore/6.24.00/source/input/cmn/interface` |
| 144 | +144 | **+0** | `mcard/3.04.01-layer2/source/lib/cmn/interfaceimp` |
| 140 | +340 | +200 | `mcard/3.04.01-layer2/source/lib/gc/gc_interface` |
| 92 | +92 | **+0** | `realcore/6.24.00/source/input/gc/gc_device` |
| 92 | +88 | −4 | `realcore/6.24.00/source/file/cmn/filesys` |
| 80 | +56 | −24 | `realcore/6.24.00/source/file/gc/dvd_device` |
| 76 | +76 | **+0** | `rcmp/dev/source/decoder/cmn/rcmpbase` |
| 68 | +68 | **+0** | `realcore/6.24.00/source/file/gc/hd_device` |
| 48 | +48 | **+0** | `path/5.01.04/source/cmn/pathreal` |
| 20 | +72 | +52 | `mcard/3.04.01-layer2/source/lib/gc/gc_blockcalculator` |

Las otras **378** bibliotecas ProDG: **+0 B**, la bandera no cambia nada.

### 6.3 Propuesta para `configure.py` — NO la aplico, y NO la recomiendo

**Mi recomendación es NO poner `-fno-implement-inlines` en ninguna parte**, y el
motivo es el §2: los bytes que quita **no llegan al DOL**, así que no compra
ninguna promoción, y a cambio **pierde 32 funciones de crédito legítimo**
(existen en el objetivo). El balance es negativo.

Si aun así se quiere (por higiene del censo de secciones), el único sitio donde
sale gratis es donde `rompe = 0` **y** quita algo: `zFe2` (3.412 B), `zCamera`
(2.608 B), `zSpeech` (1.800 B), `zEAXSound2` (1.420 B), `zPhysics` (1.284 B),
`zAttribSys` (156 B, y deja la unidad en **+0 exacto**), `zMisc`, `zFoundation`,
`zBWare`, `zEcstasy`, y las 6 bibliotecas que quedan en **+0**. Suman **~11.000 B
sin romper nada**. En las 13 unidades restantes hay que **sacar antes el cuerpo
de esas 32 funciones fuera de su clase**, y entonces la bandera es neutra.

Forma exacta si se decide: `extra_cflags=["-fno-implement-inlines"]` en la
entrada de cada unidad, no en `cflags_game` (afectaría a las 33).

---

## 6-bis. `-fno-implicit-templates`: VETADA, y con la cifra

Es la bandera que apaga las instanciaciones implícitas, o sea la que ataca
directamente el 79 % de los bytes. Barrida igual, sobre las 33 SourceLists:

**Quita 346.516 B de los 317.896** — o sea **se pasa en 28.620 B**, porque quita
también las instanciaciones que la unidad **sí posee**. Deja 14 unidades en
negativo (`zGameplay` −19.004, `zSpeech` −18.120, `zAI` −16.304…), y aciertos
exactos aislados: **`zMiscSmall` +0 clavado con 0 indefinidos**, `zRender` +32,
`zSim` +904, `zPhysicsBehaviors` +1.128, `zCamera` +1.360.

Y el precio, con `c28lo_regres.py`:

> **`-fno-implicit-templates` ROMPE 1.207 funciones que hoy casan y ARREGLA 0.**

`zPhysics` pierde 248, `zSpeech` 195, `zAI` 119, `zLua` 114, `zGameplay` 98.
**No se pone en ninguna parte.** Las únicas 9 unidades donde rompe 0 son las que
no tienen instanciaciones propias (`zMiscSmall`, `zRender`, `zBWare`,
`zEagl4Anim`, `zFeOverlay`, `zDebug`, `zGameModes`, `zMission`, `zOnline`), y en
`zMiscSmall`/`zRender` **ya está comprobado que el DOL no cambia** (§2.1): sería
maquillaje del censo por 8.024 B, sin ganar una promoción.

Comparación de las dos banderas, con la misma medida:

| bandera | quita | rompe | arregla |
|---|---|---|---|
| `-fno-implement-inlines` | 43.488 B | **32** | 0 |
| `-fno-implicit-templates` | 346.516 B | **1.207** | 0 |

---

## 7. Herramientas

**Subidas a `scripts/`, con la distinción biblioteca/SourceList incorporada:**

| script | qué hace |
|---|---|
| **`scripts/seccdiff.py`** | el tamaño de **cada sección** contra el objeto extraído, todas las unidades, **separando SourceList de biblioteca**. `--sl` / `--lib` / `--all`. Suma `.over` y `.gnu.linkonce.t*` al `.text`, que si no `zFeOverlay` y `zOnline` salen mal |
| **`scripts/extrasym.py`** | **qué** símbolos sobran, partidos en **familia A** (existe en el original en otra unidad → inocuo) y **familia B** (no existe en ninguna parte → código que el original no tiene). `-v` lista los gordos con su binding |

Los dos son autocontenidos (llevan su lector ELF), y `extrasym.py` necesita
`orig/GOWE69/NFSMWRELEASE.ELF`, que es lo que permite separar las dos familias.

**En el scratchpad (prefijo `c28lo_`), por si hacen falta:**

| script | qué hace |
|---|---|
| `c28lo_ab.py` | A/B de una bandera sobre unidades **enteras** con sus `cflags` reales de `build.ninja`, compilando **a scratchpad**: no pisa `build/` ni a otros agentes. `--sl`, o nombres de unidad |
| **`c28lo_regres.py`** | **la medida que decide una bandera**: cuántas funciones que HOY casan deja de casar, comparando contra el objeto extraído **con los campos de reubicación neutralizados**. Sin eso da 1.095 falsos positivos en vez de 32 |
| `c28lo_promo.py` / `c28lo_promo2.py` | como `trypromo.py` pero enlazando un `.o` **del scratchpad**; el `2` añade el tamaño de cada sección del ELF enlazado |
| `c28lo_dolwhy.py` | qué se mueve en el ELF enlazado al promocionar una unidad, con el símbolo del original dueño de cada tramo malo |
| `c28lo_firstwins.py` | contrasta la hipótesis «gana el primero del orden de enlace» |
| `c28lo_kind.py` / `c28lo_top.py` / `c28lo_where.py` | clasifican los símbolos de más por familia, clase y binding |
| `c28lo/` | los casos mínimos: `t1.cpp` (plantilla), `dupA/dupB.cpp` + `mini.ld`/`nolo.ld` (deduplicación en el enlace), `z0/z1/z2.cpp` (colocación) |

### 7.1 Trampas nuevas, medidas

- **El `md5`/los bytes crudos NO valen para comparar funciones entre dos
  compilaciones.** Los campos de `bl`/`b` y de las reubicaciones los rellena el
  enlazador; comparándolos en crudo, `-fno-implement-inlines` parecía tocar
  **1.095** funciones y toca **32**. `c28lo_regres.py` los neutraliza.
- **`seccdiff.py` tiene que sumar `.over` al `.text`**: `zFeOverlay` y `zOnline`
  renombran la sección y si no salen con decenas de miles de bytes de fantasma.
- **El orden de enlace es alfabético por SourceList** (`zAI` primero en
  `0x800034A0`, `zWorld2` último en `.text`, y `zOnline`/`zFeOverlay` aparte en
  `.over`). Calcularlo como «la dirección más baja de cualquier símbolo del
  objeto» **da mal**: hay 12 nombres repetidos en el ELF original
  (`__static_initialization_and_destruction_0` ×47, `__tcf_0` ×7, `OnReset` ×5…)
  y colapsan todas las unidades al mismo valor. Hay que filtrar por nombre único.
- **El ELF original tiene 0 símbolos WEAK, y el nuestro también.** `ngcld`
  normaliza weak→global al enlazar: ese contraste no discrimina nada.

---

## 8. Lo que NO he probado

1. **Aplicar nada.** No he tocado `configure.py`, `config/GOWE69/*` ni
   `splits.txt`, ni he cambiado una línea de fuente. Todo el A/B es a scratchpad.
   **Sin commit.**
2. **El frente de verdad: `.rodata`/`.data`.** Lo he localizado y cuantificado
   (§5) pero no he arreglado ni un byte. Es lo que yo atacaría a continuación, y
   `c28lo_dolwhy.py` ya dice, para una unidad dada, qué tramos se mueven y de
   quién son.
4. **Las 32 funciones que rompe `-fno-implement-inlines`.** Tengo la lista por
   unidad; no he sacado ningún cuerpo de su clase ni he medido cuánto crece
   `.text` al hacerlo (la r27 dejó esa misma pregunta abierta para
   `FEListBox::GetPCellData`).
5. **Cambiar de compilador a ProDG ≤3.7** para recuperar `linkonce`. Está
   medido que lo emite (§1.4) y **no lo he intentado**: cambiaría la generación
   de código de las 18.317 funciones que ya casan. Lo dejo escrito sólo como
   dato.
6. **Los 81.560 B de las 79 unidades mwcc.** No he buscado su equivalente de
   Metrowerks (`-inline off`, `-func_align`…) porque la r27 ya probó que ahí el
   exceso es inocuo y `mtx.c` promociona con +5.376 B.
7. **Reducir la familia B a mano.** Sé que `Attrib::TAttrib<T>::Get` explica
   ~38.000 B y que el original sólo define tres instanciaciones, pero no he
   buscado la forma de fuente que las hace desaparecer sin perder el `bl` de los
   llamantes (el comentario de `AttribSys.h:682` explica por qué está definida
   fuera de la clase a propósito).
