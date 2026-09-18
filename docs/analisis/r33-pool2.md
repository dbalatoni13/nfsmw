# Ronda 33 / pool2 — **ninguna de las cuatro da `DOL OK`**, y la razón no es la que decía el encargo

**Lo primero, que es lo que pide el encargo: `trypromo.py` da `DOL ROTO` en las
cuatro, y ninguna llega a `.rodata` byte a byte.** No hay paquete que proponer:
`configure.py`, `config/GOWE69/*` y `splits.txt` quedan **intactos** (md5
comprobado), y los cuatro `.cpp` de `SourceLists` quedan **byte a byte como
estaban** (md5 comprobado contra copia).

| unidad | `.rodata` antes | después (palanca §3) | `.data` | `.bss` | `trypromo.py` | sec9 del DOL |
|---|---|---|---|---|---|---|
| `zRender` | 2.240 / 1.032 (**+1.208**) | 2.008 (**+976**) | 172 = 172 | 232 / 8 (+224) | **DOL ROTO** | **−704 B** |
| `zMiscSmall` | 1.000 / 352 (**+648**) | 768 (**+416**) | 684 / 620 (+64) | 8 = 8 | **DOL ROTO** | **+192 B** |
| `zMisc` | 12.632 / 7.864 (**+4.768**) | 12.400 (**+4.536**) | 2.184 / 2.164 (+20) | 2.560 / 2.272 (+288) | **DOL ROTO** | **−2.208 B** |
| `zEcstasy` | 13.456 / 7.824 (**+5.632**) | 13.224 (**+5.400**) | 4.240 / 4.136 (+104) | 82.064 / 81.200 (+864) | **DOL ROTO** | **−704 B** |

`keepchk.py` sigue en **759 ok / 21 RANCIAS** (sin tocar `keep.lst`).

---

## 0. La frase que abre el informe: **la receta de la r32 no se puede aplicar aquí**

El encargo (y el §7 del informe de mi predecesor) dan por hecho que las cuatro
fallan «por lo mismo» que `zDynamics` y `zMission`: el literal del compilador
delante del bloque escrito a mano. **Lo he medido y no es así.** En `zDynamics`
el pool que emitía `cc1plus` **era** el del objetivo salvo el bloque bWare; en
estas cuatro el pool del compilador y el del objetivo **son conjuntos distintos
de cadenas, en orden distinto**, y ninguna colocación arregla eso.

Dos hechos medidos que cierran la vía del encargo:

### 0.1 Devolverle el pool al compilador NO cierra: falta pool, no sobra

Ensayo **c1** (`zMisc`): borrado el `asm()` de `.rodata` entero (3.705 líneas)
dejando **sólo** `lbl_803F4878` (92 B, el bloque bWare/STL) movido **antes de
los `#include`**.

| | `.rodata` | primer desajuste |
|---|---|---|
| base | 12.632 (objetivo 7.864, **+4.768**) | offset 0 |
| **c1** | **5.392 (objetivo 7.864, −2.472)** | **offset 96** |

Los **92 primeros bytes casan** (y los 4 siguientes por casualidad: nuestro
`""` de 1 B rellenado a 4 coincide con el `0.0f` del objetivo). Pero el
compilador **sólo genera 4.768 B de los 7.864** que tiene el objetivo: al
devolverle el pool nos quedamos **2.472 B cortos**. `zMisc` no tiene un pool que
sobra, tiene un pool que **no coincide**. c1 **revertido**.

Alineando además a mano (quitando el `""` y la `"GAMECUBE"` duplicada, 16 B) el
pool casa hasta el offset **140** y ahí el objetivo tiene `"QuickSpline"` y
nosotros flotantes: **187 rachas, 4.786 B distintos** en los 5.376 primeros.

### 0.2 Conservar el pool a mano tampoco cierra: `-strip-unused-data` deja basura

La otra mitad de la vía —dejar el bloque escrito a mano y que el pool del
compilador se lo lleve el estripado— **está cerrada por el enlazador**.
Medido en `zRender` (`--keep` con las tres etiquetas escritas a mano):

```
sin keep : sec9 −704 B   (se lleva tambien el pad_ de 1.024 B escrito a mano)
con keep : sec9 +320 B   (sobreviven 288 B de restos + $LC298 + relleno)
```

Los 288 B son exactamente lo que el brief §3 describe: **de cada símbolo
estripado quedan sus `size % 8` bytes finales, y además se conserva el relleno
de alineación entre símbolos**. Se lee en el enlace:

```
0000 63000000000000000000000070000000  c...........p...
0010 00000000650000000000000041727261  ....e.......Arra
0020 7900000065000000000000006f6e0000  y...e.......on..
0030 436c617373000000426c6f6200000000  Class...Blob....
```

(`'c'` de `bad_alloc`, `'Array'` de `Attrib::Array`, `'Class'`, `'Blob'`…)
**Con 60 símbolos de pool a estripar no hay forma de que el resto sea cero**, y
cualquier resto desplaza el enlace. O el pool del compilador **es** el del
objetivo, o la unidad no enlaza.

---

## 1. Lo que sí bloquea: el conjunto de literales, y la mayor parte tiene nombre

`lcpool.py` cuenta bytes, no cadenas, y por eso el frente parecía de colocación.
Contando **cadena a cadena contra el ELF original** (`c33pool2_censo.py`, en el
scratchpad) sale la causa dominante, y es **compartida por las cuatro**: las
**etiquetas de asignador `Attrib::*`** que nuestras cabeceras internan en cada
TU.

Censo de copias en `orig/GOWE69/NFSMWRELEASE.ELF` (cadena + NUL):

| etiqueta | copias en el original | veredicto |
|---|---|---|
| `Attrib::Attribute`, `Instance`, `Definition`, `Class`, `Database`, `TypeDesc`, `RefSpec`, `Blob` | **27 cada una** | las interna **cada TU**: correctas |
| `Attrib::TAttrib` | 24 | plantilla, por instanciación |
| **`Attrib::CollectionHashMap`** | **0** | **no existe en el juego**: invención nuestra |
| `Attrib::TypeTable`, `Array`, `HashMapTable`, `HashMap`, `Collection`, `ClassPrivate`, `ClassTable`, `DatabasePrivate` | **1** | sólo viven donde se usan: **no se internan** |
| `Attrib::ExportManager`, `Attrib::Vault` | 3 | ídem |

Nosotros internamos **19** en cada TU (las 8 buenas + las 11 de la última
columna), y **en un orden distinto**:

```
nuestro : TypeDesc Database Attribute Instance Definition Class RefSpec Blob
objetivo: Attribute Instance Definition Class Database TypeDesc RefSpec Blob
```

Salen de `USE_ATTRIB_ALLOC(...)` y de los `operator new/delete` **en clase** de
`AttribSys.h`, `AttribLoadAndGo.h`, `Common/AttribHashMap.h` y
`Common/AttribPrivate.h`, y se internan **al parsear la cabecera**, así que el
orden es el orden de declaración de las clases. Las 11 sobrantes están en clases
cuyo `operator new` el original **no tenía en la cabecera**.

**24 de las 33 SourceLists construidas llevan hoy `Attrib::CollectionHashMap`.**

---

## 2. Los tres mecanismos de colocación de la r32 **no sirven en la cabeza del pool**

Medido con `-S` y contando dónde cae cada `$LC` respecto de la primera función:

| unidad | `$LC` emitidos **antes de la primera función** | después |
|---|---|---|
| `zMiscSmall` | **33 de 34** | 1 |
| `zMisc` | 13 | 355 |

En `zMiscSmall` (y en la cabeza de las otras tres) **el pool entero se emite al
parsear las cabeceras, antes de que exista una sola función**. Los tres mandos
de la r32 —`asm()` de ámbito de fichero entre dos funciones, estático de
plantilla tardío, `.previous`— actúan sobre el pool **por función**: **no pueden
reordenar nada de lo que se interna en el parseo**. La única palanca ahí es el
**orden de parseo**: qué cabecera se lee antes.

(En `zMisc` sí es cierto lo contrario: 355 de 368 entradas son pool por función y
ahí los mandos de la r32 valen. Su cabeza, no.)

---

## 3. La palanca medida: 11 etiquetas fuera y las 8 en el orden del objetivo

Probado **sin tocar el árbol**, con una copia sombra de las cuatro cabeceras de
AttribSys en el scratchpad y un `-I` por delante (`c33pool2_cc.py --pre`):

- **e1** — las 11 etiquetas sobrantes pasan a `NULL`.
- **e2** — e1 + una `inline` sin usar al principio de `AttribSys.h` que
  **interna las 8 en el orden del objetivo** antes de que las clases las usen
  (GCC 2.9 reutiliza el `$LC` de una cadena ya internada).

| unidad | `.rodata` base | e2 | ahorro | `.text` |
|---|---|---|---|---|
| `zRender` | 2.240 | 2.008 | **−232** | sin cambio |
| `zMiscSmall` | 1.000 | 768 | **−232** | sin cambio |
| `zMisc` | 12.632 | 12.400 | **−232** | sin cambio |
| `zEcstasy` | 13.456 | 13.224 | **−232** | sin cambio |
| `zDynamics` (ya promocionada) | 544 = 544 | **544 = 544** | 0 | sin cambio |

**−232 B por unidad, exacto en las cuatro**, `.text` intacto, y la unidad ya
promocionada **no se mueve**. La `inline` no se emite (no se usa), así que el
truco del orden **no cuesta un byte**.

Y lo que importa más que los bytes: **el bloque de las 8 etiquetas queda byte a
byte igual al del objetivo** (148 B en las dos, mismos rellenos). En `zEcstasy`
eso se traduce en lo siguiente, que es el mejor resultado de la ronda:

> Con e2, y quitando además la `"GAMECUBE"` y el `"bad_alloc"` que el objetivo
> **no tiene** en esta unidad, **los 152 primeros bytes de la `.rodata` de
> `zEcstasy` casan byte a byte con el objetivo** (el `""` de 4 B más las 8
> etiquetas). El primer desajuste pasa del offset 0 al **152**.

**NO he aplicado nada de esto al árbol**: `AttribSys.h` la incluyen 24 de las 33
SourceLists y hay otros cuatro agentes midiendo. Es un frente para una tanda
propia, con A/B por objetos sobre las 24.

---

## 4. La lista de la compra, unidad a unidad

Lo que el compilador debe **dejar de emitir** y lo que debe **empezar a emitir**,
en el orden del objetivo. (Todo esto es fuente, no colocación.)

### `zEcstasy` — la más cerca, y aun así le faltan 5 funciones
Objetivo: `""`, 8 etiquetas, `Attrib::Gen::gameplay`, `Attrib::TAttrib`,
`Attrib::Gen::milestonetypes`, `"16.1.0"`, `"16.2.1"`, `"1.2.3"`, `"1.8.1"`,
`"19.8.31"`, `Attrib::Gen::simsurface`, `ePolySlotPool`, …
Nosotros: `""`, `GAMECUBE`, `bad_alloc`, 8 etiquetas, `STL`,
`Attrib::Gen::pvehicle`, `TAttrib`, `EventSequencerSystems`,
`Attrib::Gen::simsurface`, …
- **sobra**: `GAMECUBE`, `bad_alloc`, `STL` (el objetivo **no las tiene**: 0
  copias en su `.rodata`) y `EventSequencerSystems`;
- **falta**: `Attrib::Gen::gameplay`, `Attrib::Gen::milestonetypes` y las cinco
  cadenas de versión;
- tras el pool quedan **5.876 B de código en 5 funciones**:
  `epCalculateLocalDirectionalPOS16` 2.072 B (93,31 %),
  `UpdatePlatInfo__27eLightMaterialPlatInterface` 2.044 B (98,66 %),
  `GenerateHorizonFogDisplayList` 796 B (98,99 %),
  `Render__13EmitterSystemP5eView` 696 B (98,07 %, `faltan 2, sobran 0`),
  `eProject__FfffPA3_fPfN44` 268 B (85,82 %).
  **Aunque el pool se cerrara, `zEcstasy` no enlaza sin esas cinco.**

### `zMisc`
Cabeza ya resuelta (§0.1: 92 B exactos con `lbl_803F4878` antes de los
`#include`). Falta:
- **sobra**: el `""` y la `"GAMECUBE"` duplicada (16 B, y descuadran todo lo que
  viene detrás);
- **falta**: las cadenas de `QuickSpline.cpp` (`"QuickSpline"`, `"SplineDebug"`,
  `"SplineControlPoint"`, `"ControlPointNumber"`…) y ~2.470 B más de pool que
  nuestro árbol no genera;
- además `.bss` +288 B y `.data` +20 B.

### `zMiscSmall`
Su `.rodata` objetivo (352 B) está **entera** escrita a mano y es byte a byte
correcta. Le falta al compilador, en este orden: `GAMECUBE` (la tiene, mal
colocada), la ruta `bware.hpp`, `%f,%f,%f`, `%f,%f,%f,%f`, `STL`, y las cadenas
de versión `"D1003676"`, `"191527 191537 191573 191602..."`, `"10/19/05"` —que en
el original salen del fichero generado que define `BuildVersionChangelistName` y
**nosotros no tenemos**—, luego las 8 etiquetas, y luego `"bmwm3gtre46"`,
`"911turbo"`, `"10.10.235.145"`.
Le **sobran** `""`, `Attrib::Gen::pvehicle`, `TAttrib`,
`EventSequencerSystems`, `simsurface`, `chassis`, `engine`, `induction`, `nos`,
`tires`, `transmission` (≈228 B). Y `.data` +64 B.

### `zRender`
El caso peor, y el que menos paga (308 B de `linked`). Su `.rodata` objetivo
(1.032 B, todo escrito a mano) trae **cosas que el árbol no puede generar**:
- el bloque de 25 flotantes de conversión de color (0.299, 0.587, 0.114 …): en
  nuestro árbol **`ColourConvertXBoxToPS2` es un cascarón de 32 B**;
- `SceneryOverrideConn` y `Pkt_SceneryOverride_Open`: nuestra `RenderConn.cpp`
  **no tiene esa conexión**;
- las cuatro cadenas de traza (`"SmackableRender_Service()"`, …).
Y `.bss` +224 B (28 pares `hash./_.tmp_` de más).

---

## 5. Vedas nuevas, todas medidas

1. **`-fno-implement-inlines` no quita ni un byte de pool** (`zRender`:
   `.rodata` 2.240 antes y después). Las etiquetas se internan al parsear la
   cabecera, no al emitir la función.
2. **`-fno-implicit-templates` tampoco.** Quita 3.996 B de `.text`
   (`zRender` 4.336 → 340) pero **la `.rodata` no se mueve**: las cadenas del
   cuerpo de la plantilla ya están internadas aunque el cuerpo no se emita.
3. **`-strip-unused-data` no puede limpiar un pool ajeno**: deja `size % 8` por
   símbolo **más el relleno de alineación**. Medido: 288 B de restos en
   `zRender` con 60 símbolos estripados. Corolario: **«conservar el bloque a
   mano y que el compilador se estripe» no es una vía**, en ninguna unidad.
4. **Los tres mandos de colocación de la r32 no alcanzan la cabeza del pool**:
   en `zMiscSmall` 33 de 34 entradas se emiten antes de la primera función
   (§2). Lo único que las ordena es el **orden de `#include`**.
5. **`lcpool.py` mide bytes, no cadenas, y por eso este frente parecía de
   colocación.** El diagnóstico bueno es el censo contra el ELF original: una
   cadena con 0 copias es invención nuestra, con 1-3 no se interna, con ~27 sí.
6. **`extern const float lbl_…` sigue sin escalar** (veda r32 §8, confirmada):
   aquí ni siquiera hay etiquetas `lbl_` que referenciar, el pool del objetivo
   es anónimo (`pad_`/`$LC<direccion decimal>` los pone el troceador).

---

## 6. Lo que yo recomendaría para la r34

1. **Una tanda para la cabecera `AttribSys`** (las cuatro: `AttribSys.h`,
   `AttribLoadAndGo.h`, `Common/AttribHashMap.h`, `Common/AttribPrivate.h`):
   sacar de la clase los 11 `operator new/delete` que el original no interna y
   dejar las 8 en el orden `Attribute, Instance, Definition, Class, Database,
   TypeDesc, RefSpec, Blob`. Medido: **−232 B por unidad en 24 unidades**,
   `.text` intacto, `zDynamics` sin moverse, y el bloque de 148 B **byte a byte**
   con el objetivo. Es la única palanca que toca a las cuatro a la vez, y la
   sombra del scratchpad (`c33pool2_shadow`) sirve de banco de pruebas.
2. **`zEcstasy` antes que `zMisc`**: es la que más paga (145.884 B), la que ya
   casa en los 152 primeros bytes y la que tiene la lista de la compra más
   corta. Pero **hay que cerrar sus 5 funciones (5.876 B) igualmente**, así que
   conviene emparejar el agente de pool con el de código.
3. **`zRender` y `zMiscSmall` NO son «el caso limpio»**: son las dos que
   necesitan fuente que no existe en el árbol (el fichero de versión de
   `BuildVersionChangelistName`, `ColourConvertXBoxToPS2` entera, la conexión
   `SceneryOverride`). 308 B y 768 B de `linked`: son las últimas de la cola.

---

## 7. Estado del árbol y herramientas

- **Cero ficheros del árbol modificados por mí.** Los cuatro `.cpp` de
  `SourceLists` tienen el md5 de partida (copia en
  `…/scratchpad/c33pool2_bak/*.cpp.orig`); `configure.py`, `config/GOWE69/*` y
  `splits.txt` no se han tocado (`keep.lst` sólo se ha leído; las pruebas con
  `--keep` van sobre copia del scratchpad).
- Las cuatro unidades quedan **reconstruidas desde la fuente restaurada**
  (`build_direct.py`, 4 ok).
- **Ni una instrucción de ensamblador escrita.**
- Guiones nuevos en el scratchpad, prefijo `c33pool2_`:
  `_lay.py` (mapa de una sección: símbolo, offset, tamaño y contenido),
  `_pad.py` (desglosa un blob en cadenas y flotantes),
  `_ref.py` (quién referencia un símbolo),
  `_censo.py` (**el que cambió el diagnóstico**: cada cadena `$LC` nuestra con
  sus copias en el ELF original),
  `_cut.py` (deja en el `asm()` de `.rodata` sólo los símbolos que le digas),
  `_shadow/` (copia sombra de AttribSys para medir sin tocar el árbol),
  y `_cc.py` con `--pre` para meter un `-I` **delante** de los cflags.
