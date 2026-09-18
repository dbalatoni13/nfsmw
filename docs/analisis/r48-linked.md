# r48 — `linked`: las dos funciones caen, y el frente de simbolos cambia de sitio

Territorio: `SetScreenNames__13CustomizeMain` (zFeOverlay), `PATHI_nextnode__Fiii`
(`pathnode` + las 14 unidades de `path`), y el censo de simbolos de mas.

## Resultado

**Las DOS funciones del encargo cierran, las dos SIN un solo `asm`, y las dos
por FIDELIDAD**: el diagnostico de reparto/planificacion de la r47 era correcto
como descripcion pero la causa estaba en la fuente, y la delatan el DWARF y el
mapa de lineas del original.

| unidad | funcion | antes | despues |
|---|---|---|---|
| `Speed/Indep/SourceLists/zFeOverlay` | `SetScreenNames__13CustomizeMain` | 440/440 B, 6 filas | **100,0 %, 0 filas** |
| `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode` | `PATHI_nextnode__Fiii` | **300**/308 B | **308/308 B, 100,0 %** |

```
# SetScreenNames__13CustomizeMain  target=100.0%  ours=100.0%  size=440/440
# PATHI_nextnode__Fiii             target=100.0%  ours=100.0%  size=308/308
fncmp zFeOverlay   0 de 467 funciones con el CODIGO distinto -- 0 B   (antes 1)
fncmp pathnode     0 de  11 funciones con el CODIGO distinto -- 0 B   (antes 1)
las 14 unidades de `path`  0 de N distintas, TODAS          (antes 1 sola pendiente)
lcfix.py --check   todas las entradas @lc estan al dia
```

Se retiran **dos `asm` de deuda** que llevaban puestos: el
`__asm__("" : "+m"(g_pCustomizeSubPkg))` de `CarCustomize.cpp` y, con el, la
VEDA r46; y no hace falta ninguno en `pathnode`. **`pathi.h` NO se toca**: sale
del gate intacta (SHA `2a5b731052588823` antes y despues).

Ficheros modificados: **dos**, los dos de territorio y los dos LF puro (como
estaban):

- `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.cpp`
- `src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp`

**Ninguna de las dos unidades promociona todavia**, y el motivo ya no es codigo
(§3 y §4). Los `trypromo` estan citados con su SHA.

---

## 1. `SetScreenNames` — no era el planificador: faltaba el `this` muerto

El diagnostico de la r47 (6 filas; `high(g_pCustomizeSubPkg)` a r30 en vez de r3
y `high(g_pCustomizeSubTopPkg)` a r9 en vez de r30) era exacto, y el volcado lo
confirma entero. Lo que estaba mal era **de donde salia**.

**El DWARF del original** (`symbols/mw_dwarfdump.nothpp`, rango 0x803B9584):

```
void CustomizeMain::SetScreenNames() {
    // Range: 0x803B9594 -> 0x803B9594
    inline bool CarCustomizeManager::IsInBackRoom() {}
}
```

Una sola expansion inline, **`gCarCustomizeManager.IsInBackRoom()`**, y nosotros
llamabamos a la funcion libre `CustomizeIsInBackRoom()`. El aviso ya estaba
escrito en el propio fichero desde la r26 (linea 1950: «el original llama al
MIEMBRO gCarCustomizeManager.IsInBackRoom() … Corregido en los TRES sitios»)
pero `SetScreenNames` no era uno de esos tres.

El cambio es una linea:

```cpp
void CustomizeMain::SetScreenNames() {
    if (gCarCustomizeManager.IsInBackRoom()) {
```

Y con el, **sin ningun `asm`**, la funcion sale a 0 filas. El envoltorio miembro
mete una referencia MUERTA a `&gCarCustomizeManager` (el `this` que el inline no
usa) y eso basta para que `local_alloc` reparta
`high(g_pCustomizeSubPkg) -> r3`, `high(g_pCustomizeSubTopPkg) -> r30` y el
resto corrido (r29..r24), que es el reparto del objetivo. Coste en bytes: cero.

### El modelo, para que quede escrito (y porque explica los negativos)

Con `-dS -fsched-verbose-3` el bloque 2 (la rama `else`) se lee entero:

- **el reparto es un SINTOMA del ORDEN DE `sched1`, no de `sched2`.** El
  `local_alloc` trabaja sobre la salida de `sched1`; el orden final que se ve en
  el binario lo pone `sched2` DESPUES.
- En el objetivo, `sched1` emite el primer `stw` **justo antes** de
  `lis r3,gCarCustomizeManager@ha`, con las ocho cadenas de cadena todavia
  vivas. Por eso `high(SubPkg)` muere antes de que nazca el `high(mgr)` y puede
  compartir **r3** con el, y `high(SubTopPkg)` --que si solapa-- se va a r30.
  Lo que se ve en el binario (el store en la posicion 4 del bloque) lo produce
  `sched2`, empujado por la **antidependencia sobre r3** que la propia asignacion
  crea.
- Las prioridades de `haifa-sched` en ese bloque son
  `lis_str = c+3`, `addi = lis_glob = 178(lis mgr) = c+2`, `stores = 185 = c+1`,
  con `c = INSN_PRIORITY(bl IsCareerMode)`. Como `178 = stores + 1` **por
  construccion** (`178 -> 185 -> call`), subir la prioridad del primer store
  arrastra su cadena entera (`addi1`, `lis_glob1`, `lis_str1`) y lo adelanta
  demasiado. Ese es el techo real de todas las palancas de barrera, y explica
  por que 18 formas de la r47 y las 18 mias fallaron.

### Medido y negativo, con la cifra (todas 440/440 B)

| # | forma | filas |
|---|---|---:|
| — | base r47 (`asm "+m"` detras de las ocho asignaciones) | 6 |
| B0 | quitar el `asm` | 33 |
| T4 | `*(char *volatile *)&g_pCustomizeSubPkg = …` (solo el 1.º) | 33 |
| B1 | `__asm__ __volatile__("")` en el sitio del `"+m"` (VEDA r46 remedida) | 33 |
| B8 | el `"+m"` DELANTE de las ocho asignaciones | 33 |
| T5 | los OCHO stores volatiles | 31 |
| V123 / V128 / V1234 | tres o cuatro stores volatiles | 31 |
| D3 | `IsInBackRoom` miembro + pareja volatil | 30 |
| V28 | volatiles el 2.º y el 8.º (sin el 1.º) | 29 |
| B5b | barrera de ranura ANTES del `"+m"` | 28 |
| A3 | un `asm` con `"+m"` sobre SubPkg y HudColorPkg | 26 |
| A4 | `"+m"` sobre SubPkg y SubTopPkg detras de la 1.ª asignacion | 23 |
| B5 | `"+m"` + `asm volatile("")` detras | 16 |
| B7 | pareja volatil + `asm volatile("")` | 16 |
| **T1/T2/V13/V14** | **`volatile` en el 1.º y en OTRO cualquiera** | **15** |
| D1 | `IsInBackRoom` miembro **+ el `asm "+m"` puesto** | 15 |
| **D2** | **`IsInBackRoom` miembro, SIN `asm`** | **0** |

Dos cosas utiles de ahi:

1. **`write_dependence_p` de `alias.c:1320` exige que las DOS `MEM` sean
   volatiles** (`if (MEM_VOLATILE_P (x) && MEM_VOLATILE_P (mem)) return 1;`).
   Por eso un solo store volatil no hace nada (33 filas) y una PAREJA si (15):
   es la forma mas barata que hay de encadenar dos stores sin emitir un byte y
   **sin `asm`**. La pareja ya ponia `high(SubPkg)` en **r3** (las filas 53 y 56
   desaparecian); lo que no arreglaba era `SubTopPkg`.
2. **`D1` demuestra que el `asm` y la correccion de fidelidad se pelean**: con
   los dos puestos salen 15 filas, con solo la correccion salen 0. Un `asm` de
   deuda puede TAPAR la causa real durante rondas.

---

## 2. `PATHI_nextnode` — el `if (node < 0)` explicito que faltaba

La palanca **no estaba en `pathi.h`**. La da `lmap.py` sobre el original:

```
8037C744  mr. r3, r3                     pathnode.cpp:351
8037C748  blt .L_8037C788
8037C74C  lwz r9, _4Path.pfstate@sda21   pathnode.cpp:352  pathi.h:799  pathi.h:800
8037C750  lwz r10, 0x18(r9)
8037C754  mr r6, r9
...
8037C780  cmpwi r7, 0x0                  pathi.h:809  pathnode.cpp:356
8037C788  li r3, -0x1                    pathnode.cpp:357
```

Entre `nodeinfo = PATHI_getnode(node)` (351) y el `if (!nodeinfo)` (356) el
original tiene **CUATRO lineas que nosotros no teniamos**, y la 352 deja una
nota justo encima de la carga de `pfstate`. Con el desplazamiento de lineas
medido (nuestra 344 ↔ su 349, nuestra 353 ↔ su 363) el hueco es exactamente ese.

La forma que lo reproduce es un **`if (node < 0) return -1;` explicito delante
de la llamada**, redundante con el chequeo del propio inline:

```cpp
    int nextnode = -1;

    if (node < 0)
        return -1;

    nodeinfo = PATHI_getnode(node);
```

**308/308 B, 0 filas, cero `asm`, y `pathi.h` sin tocar.** El jump-threading
funde ese `return -1` con el de la linea 357 (por eso no cuesta un byte), pero
cambia el grafo lo justo para que:

- **gcse deja UNA sola carga de `_4Path.pfstate`** en el RTL (`.flow`: de 2 a 1)
  y `pre_insert_copies` emite el `mr r6,r9` del objetivo, en vez de que
  `update_equiv_regs` (`local-alloc.c`, `REG_N_REFS == 2 && REG_BASIC_BLOCK < 0`)
  rematerialice la segunda;
- con ese valor vivo de mas la funcion necesita **12 registros en vez de 11**, y
  `forreal` --que es el allocno de menos prioridad (2 refs / 34 insns, el ULTIMO
  de los 28 en el `.greg`)-- sube de **r12 a r31**: aparecen el `stw r31`/`lwz r31`
  y el marco pasa de 0x8 a 0x10. Los 8 B que faltaban.

**Corrige el diagnostico de la r47**: la copia `mr r6,r9` y el marco de 8 B no
eran dos problemas, eran el mismo, y la palanca no estaba en la cabecera
compartida.

### Medido y negativo, con la cifra

| # | forma | B |
|---|---|---:|
| — | base | 300/308 |
| h1 | `PATHFINDERSTATE *pfs = Path::pfstate;` al principio de `PATHI_getnode` | 296 (la carga se iza por encima del `blt`) |
| h2 | `PATHI_getnode` con dos `if` separados | 300 |
| h3 | `pfstate_agg[0]` dentro de `PATHI_getnode` | 300 |
| h4 | local `pfs` DETRAS del chequeo `< 0` | 300 |
| c1 | `track` antes de `branches` (como el original) | 300 |
| c2 | `pfstate_agg[0]->track` en el llamante | 300 |
| c3 | local `PATHTRACK **tracks` | 300 |
| c4/c5 | local `pfs` delante del `if (!nodeinfo)` | 296 |
| h4+c6, h4+c7, h4+c1 | combinaciones | 300 |
| h4+c4 | **aparece el `mr r6,r9`** pero con un camino de mas | 312 |
| **c8/c9** | **`if (node < 0) return -1;` explicito** | **308, 0 filas** |

`c9` (c8 + `track` antes de `branches`) da **el mismo objeto** que `c8`: el
orden de esas dos sentencias no aporta, aunque el original lo tenga al reves.

**Gate cumplido**: las **14** unidades que incluyen `pathi.h` medidas antes y
despues (`scratchpad/r48_linked/path_before.txt`), **cero funciones distintas en
las catorce**. `pathi.h` no se ha modificado.

---

## 3. Lo que bloquea ahora a `zFeOverlay` (141.224 B de `linked`)

`fncmp` = **0 de 467**. Y aun asi:

```
promote.py Speed/Indep/SourceLists/zFeOverlay
    - .over mide 148408 B y el extraido 141224 B
    - .rodata 8776 / 8472 ; .data 924 / 940 ; .bss 3144 / 7216
    - exportamos 106 simbolo(s) de mas
trypromo.py Speed/Indep/SourceLists/zFeOverlay   ->  DOL ROTO (d2e208d4dd88)
```

Es el mismo frente que tiene parada a `zEAXSound`. Ver §5: **medido, se queda en
424 B**.

## 4. Lo que bloquea a `pathnode` (4.204 B de `linked`)

El `.text` ya es **4.204 / 4.204 exacto**. Lo unico que sobra son **32 B de
`.rodata`**:

```
promote.py  ->  - secciones que emitimos de mas: .rodata(32B)
trypromo.py ->  DOL ROTO (748afb27cd23)
```

Contenido: `$LC0..$LC3` = `4330000000000000`, `4330000080000000` y **las dos
repetidas** (los sesgos de conversion entero↔flotante de `PATHI_calcwaitbeat`).
El original tiene **solo dos**, en `lbl_80413A48` y `lbl_80413A50` (se ven en
`lmap` de `PATHI_calcwaitbeat`, 8037C26C y 8037C284). O sea **dos cosas**:

1. emitimos 4 constantes donde el original tiene 2 (no deduplicamos: `$LC0==$LC2`
   y `$LC1==$LC3`), y
2. el rango `0x80413A48..0x80413AE0` **no esta asignado a nadie en
   `config/GOWE69/splits.txt`** --`pathbank` acaba en 0x80413A48 y el siguiente
   `.rodata` empieza en 0x80413AE0--, asi que vive en un comodin `auto_*`.

Con la Regla A de `ventana-pendiente.md` (§1) el hueco de detras es de 136 B, asi
que reclamar solo nuestros 32 B **romperia** el DOL. Es trabajo de coordinacion
sobre `splits.txt`, no de codigo. **No lo he tocado.**

`pathbank` (la otra `NonMatching` de `path`) esta igual de cerca y por lo mismo:
`fncmp` 0 de 5, y `promote` dice `.rodata 164 / 172` y cinco simbolos
`lbl_8041399C…` que se lleva un `auto_*`.

---

## 5. El censo de `extrasym`, y el frente que cambia de sitio

### El censo pedido

`python scripts/extrasym.py` / `--lib`, arbol entero:

| | simbolos | B |
|---|---:|---:|
| **A)** existe en el ELF original, en el rango de OTRA unidad | 1.040 | 124.276 |
| **B)** NO existe en ninguna parte del original | 2.786 | 163.096 |
| A) bibliotecas | 13 | 608 |
| B) bibliotecas | 648 | 81.476 |

Y las dos unidades del encargo:

| unidad | sobran | B | A | B(A) | B | B(B) |
|---|---:|---:|---:|---:|---:|---:|
| `zEAXSound` | **224** | **22.480** | 31 | 4.960 | **193** | **17.520** |
| `zFeOverlay` | 96 | 7.184 | 38 | 4.276 | 58 | 2.908 |

(Los 229 del brief son de `promote.py`, que cuenta tambien simbolos de datos;
`extrasym` cuenta funciones. Los 22.480 B cuadran **exactamente** con los
173.572 − 151.092 de `.text`, y los 7.184 con los 148.408 − 141.224 de `.over`.)

**Respuesta al encargo: la mayoria es de la familia B** --193 de 224 simbolos y
17.520 de 22.480 B en `zEAXSound` (78 %)--. En `zFeOverlay` esta mas repartido
(40 % de los bytes en B).

### Pero el eje que importa es OTRO, y no estaba medido: **son HUERFANOS**

Contando cuantos de esos simbolos los referencia alguna reubicacion de **nuestro
propio objeto**:

| unidad | sobran | referenciados | **HUERFANOS** |
|---|---:|---:|---:|
| `zFeOverlay` | 96 / 7.184 B | 19 / 660 B | **77 / 6.524 B (90,8 %)** |
| `zEAXSound` | 224 / 22.480 B | 22 / 2.180 B | **202 / 20.300 B (90,3 %)** |

Y **nuestros objetos no tienen ni una seccion `.gnu.linkonce`**: todo va en un
`.text`/`.over` monolitico. Eso cierra el diagnostico de `-strip-unused` de
`ventana-pendiente.md` §2 **por el otro lado**: el enlazador no puede quitar una
funcion suelta de dentro de una seccion, solo objetos enteros --que es
exactamente por que `-strip-unused` rompe el DOL sobre el enlace intacto--.

**La palanca no es de enlace: es de COMPILACION.** Medido sobre `zFeOverlay`
compilando la SourceList real con banderas extra (`scratchpad/r48_linked/flagtest.py`):

| banderas | `.text` | sobran | faltan | funciones con CODIGO distinto |
|---|---:|---:|---:|---:|
| base | 148.408 | 96 / 7.184 B | 0 | 0 |
| `-fno-implicit-templates` | 142.528 | 49 / 1.304 B | 0 | **0** |
| `-fno-implicit-templates -fno-implement-inlines` | **141.648** | 18 / 520 B | 3 | **0** |

Objetivo: **141.224**. Es decir: **de 7.184 B de exceso a 424 B, sin que cambie
ni una instruccion de ninguna de las 467 funciones**. Los 3 que faltarian con la
segunda bandera son `TextureLoadedCallbackAccessor__14CustomizeParts`,
`TexturePackLoadedCallbackAccessor__14CustomizeParts` y
`TextureLoadedCallbackAccessor__24QRCarSelectBustedManager` (32 B cada uno), y
los 18 que sobran son cuerpos fuera de linea de inlines de clase
(`_._8FEWidget`, `SetPosX__8FEWidget`, `SetTime__5Timer`…) --el frente de
`nfsmw-en-clase-es-inline.md` / `nfsmw-decl-comdat-inline.md`, ya en tamano de
un solo agente--.

En `zEAXSound` la misma bandera **no vale tal cual**: `-fno-implicit-templates`
deja 158 / 14.496 B pero **pierde 68 simbolos**, y con las dos 40 / 12.196 B
perdiendo 70. Ahi habria que instanciar explicitamente esos 68-70 antes.

**Es una bandera POR UNIDAD, no global.** El test que decide si es segura es
`faltan == 0` en `flagtest.py`.

---

## 6. Propuestas a coordinacion (no he tocado nada de esto)

1. **`configure.py`, `cflags_frontend` (o el bloque que compile `zFeOverlay`):
   anadir `-fno-implicit-templates`.** Medido: `.text` 148.408 → 142.528, cero
   funciones distintas, cero simbolos perdidos. Es el paso seguro. Anadir ademas
   `-fno-implement-inlines` deja 141.648 (424 B del objetivo) pero pide resolver
   los 3 accesores de arriba, asi que va detras.
2. **Pasar `flagtest.py` a las demas unidades al 100 % que no promocionan.** El
   criterio es `faltan == 0`. Es el frente de `linked` mas grande que queda:
   287.372 B de simbolos de mas en las SourceLists, de los que ~90 % son
   huerfanos.
3. **`splits.txt`: el rango `.rodata 0x80413A48..0x80413AE0`** (152 B, hoy en un
   comodin `auto_*`). Con el asignado a `pathnode` --y respetando la Regla A, o
   sea reclamandolo ENTERO, no solo nuestros 32 B-- `pathnode` promociona:
   4.204 B de `linked`. `pathbank` esta en el mismo caso (`lbl_8041399C…`).
4. **`pathnode`: deduplicar `$LC0..$LC3`.** Emitimos cuatro constantes DFmode
   donde el original tiene dos. No cambia `.text` (las reubicaciones se
   normalizan) pero si el tamano de `.rodata`, que es lo que bloquea.
5. **Fuera de territorio, gratis**: el aviso de la linea 1950 de
   `CarCustomize.cpp` («el original llama al MIEMBRO
   `gCarCustomizeManager.IsInBackRoom()`») **vale para mas sitios de los tres que
   se corrigieron en la r26**. En `CarCustomize.cpp` quedan **30** llamadas a
   `CustomizeIsInBackRoom()` libres; en `SetScreenNames` esa sola linea valia 440 B
   y toda la unidad. Merece un barrido con el DWARF delante, unidad por unidad.

---

## 7. Lo que se lleva la ronda (para el brief)

1. **Antes de pelear un reparto, mira si al DWARF le sobra o le falta una
   EXPANSION INLINE.** `SetScreenNames` llevaba tres rondas y ~40 formas de
   fuente como problema de `sched1`/`local_alloc`, y era una llamada escrita a la
   funcion libre en vez de al envoltorio miembro. El `this` muerto de un inline
   que no usa `this` **no emite un byte y cambia el reparto entero**.
2. **Un `asm` de deuda puede TAPAR la causa real.** Con el `"+m"` puesto, la
   correccion de fidelidad da 15 filas; sin el, 0. Cuando una funcion lleva un
   `asm` que la deja «casi», conviene remedir la hipotesis nueva **tambien sin
   el**.
3. **La pareja de `MEM` volatiles es una barrera de cero bytes y sin `asm`.**
   `alias.c:1320` (`write_dependence_p`) devuelve 1 **solo si las DOS** son
   volatiles: `*(T *volatile *)&g1 = v;` en dos sentencias las encadena. En
   `SetScreenNames` valia 33 → 15 filas. Un solo store volatil no hace nada.
4. **`sched1` es el que decide el reparto; `sched2` solo mueve lo ya repartido.**
   El orden que se ve en el binario NO es el que vio `local_alloc`. En
   `SetScreenNames` el store «adelantado» del objetivo lo pone `sched2`, empujado
   por una antidependencia sobre r3 que **nace del reparto**, no al reves.
5. **`178 = stores + 1` por construccion.** Cuando la cadena que compite con un
   store es la del argumento de una llamada del mismo bloque
   (`lis` → `addi` → `bl`), su prioridad es siempre la del store mas uno, y
   subir la del store arrastra su cadena entera. Es un techo, no una veda: se
   rompe por fuera (fidelidad), no con barreras.
6. **Un `return` redundante puede valer 8 B de marco.** En `PATHI_nextnode` un
   `if (node < 0) return -1;` que el jump-threading funde con otro `return -1`
   --cero bytes propios-- cambia el grafo lo justo para que gcse deje UNA carga
   con su `mr`, y la presion de mas empuja el parametro de menos prioridad a un
   preservado. Lo delata el HUECO DE LINEAS del mapa del original.
7. **El frente de `-strip-unused` no es de enlace.** 90 % de los simbolos que
   sobran son HUERFANOS (nadie los referencia ni en nuestro propio objeto) y no
   hay ni una `.gnu.linkonce`: el enlazador no puede quitarlos aunque quiera.
   `-fno-implicit-templates` por unidad se lleva el 82 % del exceso de
   `zFeOverlay` sin cambiar una instruccion.

---

## 8. Verificacion

```
build_direct.py zFeOverlay + las 14 unidades de path      todas ok, 0 fallidas
fndiff  SetScreenNames__13CustomizeMain   100.0 %  440/440
fndiff  PATHI_nextnode__Fiii              100.0 %  308/308
fncmp   zFeOverlay   0 de 467 distintas   (antes 1)
fncmp   pathnode     0 de  11 distintas   (antes 1)
fncmp   las otras 13 unidades de path     0 distintas, sin cambio
lcfix.py --check    todas las entradas @lc estan al dia
git diff --check    limpio
trypromo zFeOverlay  DOL ROTO (d2e208d4dd88)   <- simbolos, no codigo (§3/§5)
trypromo pathnode    DOL ROTO (748afb27cd23)   <- .rodata sin reclamar (§4)
```

Ninguna funcion empeora en ninguna de las 15 unidades. No se ha lanzado ningun
`ninja` ni `configure.py`; no hay commits ni `git add`; no se ha tocado
`configure.py`, `config/GOWE69/*` ni `splits.txt`.

Finales de linea comprobados: `CarCustomize.cpp` (5.104 lineas) y
`pathnode.cpp` (676) son **LF puro**, como estaban. `pathi.h` intacta
(sha256 `2a5b731052588823` antes y despues). No queda ningun fichero `__r48*`
bajo `src/`.

## 9. Artefactos

`scratchpad/r48_linked/`:

- `m.py` — arnes de SourceList sombra (de la r47, con `fncmp` como modulo);
- `mini.py` / `batch.py` — repro minima de `CarCustomize.cpp` (12 s) y batidor en
  paralelo (6 variantes en 21 s), con refs/len/prioridad y el reparto final;
- `schedv.py` + `lst.py` — `cc1plus -dS -fsched-verbose-3` recortado a la funcion
  y listado compacto del `.s` con la linea de fuente al lado;
- `p.py` — arnes de `pathnode`/`pathi.h`: instala la variante, compila las 14
  unidades, mide, **restaura y verifica el SHA** (1,8 s la vuelta completa);
- `pgen.py` / `gen.py` — generadores de variantes;
- `flagtest.py` — compila una SourceList con banderas extra y saca secciones,
  simbolos de mas y simbolos que FALTAN (el criterio de seguridad);
- `extrasym_sl.txt` / `extrasym_lib.txt` — el censo entero;
- `path_before.txt` — `fncmp` de las 14 unidades de `path` antes de tocar nada;
- `base/` — copias de entrada de `CarCustomize.cpp`, `pathi.h` y `pathnode.cpp`.
