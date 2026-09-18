# Dossier del frente — las 20 unidades, qué las bloquea y qué vale

**Todo lo de aquí está MEDIDO**, no supuesto. Lo que no se ha medido lleva `SIN MEDIR`.
El objetivo es que **una ronda no tenga que descubrir nada**: coge la fila y trabaja.

    linked 525 / 619, techo real 545.  Quedan 20 unidades.
    DOL 9619ba57c9919f95f7f2ac951a2166a3517f91e3, verificado al cerrar la r63.

> **El censo se mueve.** 616 → 617 al añadir `spchdata`, 617 → 619 al partirse dos
> comodines. La cifra honesta es el **conteo** de `linked` contra el **techo (545)**,
> no la fracción contra el censo.

---

## El cambio de naturaleza del frente

**Nueve de las veinte no tienen NI UNA función pendiente.** No les falta escribir nada:
las bloquea la colocación.

    zAI 0/1030 · zEAXSound 0/773 · zFe 0/921 · zFe2 0/1307 · zGameplay 0/768
    zLua 0/537 · zMisc 0/450 · zPhysics 0/718 · zPlatform 0/136

Y de las **26 funciones abiertas** que quedan, el perfil de sus 8.417 instrucciones es:

| | |
|---|---:|
| idénticas | **80,8 %** |
| difieren **sólo en el registro** | **16,0 %** |
| estructurales | 5,3 % |

**Seis no tienen ni una palabra estructural**: `HolePunchAvoidables`, `InitAtSegment`,
`DefragmentPool`, `HandleTriggers`, `CookValues` (y `ActualReadJoystickData`, ya cerrada).
Lo que queda es, en su gran mayoría, **el asignador de registros**.

---

## Las cinco más cerca

| unidad | queda | qué es exactamente |
|---|---:|---|
| **zEagl4Anim** | `.text +4` | **los +4 son TODO él**: la función `Initialize__Q25EAGL413DynamicLoader`, 2356 contra 2352. Sus saltos de delta bajaron de 47 a **4**. Su `.text` es PERMUTACIÓN PURA: 318 símbolos contra 318. |
| **zTrack** | 3.694 B | **UN solo bloque**: 348 B de cadenas que `cc1plus` interna duplicando el prefijo escrito a mano en `lbl_80408FB8`. El patrón (`_bwarePrefix + offset`) ya está resuelto tres veces en el árbol. |
| **zEcstasy** | 4.384 B | tres funciones. `UpdatePlatInfo` tiene la veda **rota por escrito desde la r56** (28 filas → 3) y nadie la recogió. |
| **zPlatform** | 6.699 B | **ORDEN PERFECTO** desde la r63 (134 descolocadas de 137 → 0). Lo que queda es TODO `.rodata` (la sección mide 6.712: coinciden en 13 B). |
| **zLua** | 9.152 B | sólo **3** descolocadas. |

---

## El resto, por distancia

| unidad | B | fn abiertas | descolocadas | nota |
|---|---:|---:|---:|---|
| zAI | 53.874 | 0 | 101 | bajó un **78 %** en la r63 (era 244.740) |
| zFe2 | 46.850 | 0 | 89 | |
| zEAXSound2 | 37.784 | 1 | — | `bss+32` |
| zGameplay | 35.438 | 0 | 35 | |
| zFe | 34.384 | 0 | 56 | |
| zEAXSound | 31.705 | 0 | — | orden de DATOS; su `.bss` se cerró en la r61 |
| zWorld | 24.884 | 4 | 53 | **~14.200 son UN frente**: el orden del pool de cadenas ($LC533 = 10.617 B) |
| zCamera | 19.529 | 3 | 31 | 12.117 del bloque diferido + 7.292 de `.rodata` |
| zSpeech | 11.264 | 1 | 19 | |
| zPhysics | SIN MEDIR | 0 | 149 | `reorden.py`: **cero palabras de contenido real** en 34.532 |
| zMisc | 648 | 0 | **0** | orden ya resuelto; pero la `.data` está REORDENADA (126 de 173) |
| zWorld2 | SIN MEDIR | 2 | — | `bss+64` |
| zPhysicsBehaviors | SIN MEDIR | 1 | — | `.text +4`, y es `UpdateLoaded` |
| steering | SIN MEDIR | 6 | — | `.text −8  bss+32`, Metrowerks |
| madidct | SIN MEDIR | 2 | — | Metrowerks |

---

## Los dos mecanismos de ORDEN, y son distintos

**1. El orden de PARSEO manda en el código normal.** Tau de Kendall contra el objetivo:
**+0,953 / +0,998 / +0,989 / +0,938 / +1,000** (zPhysics, zWorld, zCamera, zWorld2, zMisc)
contra **+0,672 / +0,529 / +0,541 / +0,418 / +0,469** del grafo de llamadas. El mecanismo:
**460 de las 718 funciones de zPhysics (64,1 %) no tienen ni una `R_PPC_REL24`** — 407 sólo
las referencia una vtable y 53 nada en absoluto. Un grafo no puede ordenar lo que nadie
llama. `parseord.py` lo mide preprocesando con `-E`, **en segundos y sin compilar**.

**2. Pero los saltos que QUEDAN están en el BLOQUE DIFERIDO de `finish_file`.**
En zPhysics los rangos 0..358 de código normal ya estaban a +0 y los 169 saltos estaban
TODOS en el diferido. Y en zCamera los 19 saltos cerrados **no los movió ningún `#include`**.
Ahí mandan otras cosas, todas medidas:

- `walk_globals` saca las **vtables al REVÉS** del orden de completado de clases, y vale
  **por bloques**: una unidad puede tener dos, las dos pasadas de `finish_file`.
- `instantiate_decl` (`cp/pt.c:9439-9461`) instancia en el punto de parseo **sólo si el
  primer uso está DENTRO de una función**. **Cuerpo en clase = va a la cola; fuera = sale
  donde se parsea.**
- La cola de inlines sale en orden de **parseo de la definición**, no de primera petición.

---

## Las familias de las funciones abiertas

| familia | estado | qué es |
|---|---|---|
| **A** — `qty_const` de cse2 | **VIVA** | el objetivo mantiene un `(high <lit>)` vivo en un preservado y nosotros lo rematerializamos. Decidida con una línea de `-dG`: `COPY-PROP: Replacing reg 625 in insn 1210 with reg 753`. Miembros: `RenderFlaresOnCar`, `UpdateWheelY`, `UpdateLoaded`. |
| **B** — empate de `allocno_compare` | abierta | `pri = floor_log2(n_refs)*n_refs/live_length`. `DefragmentPool` es el miembro más barato: **el único con criterio de descarte SIN compilar el diff** (si el `.greg` no mueve una columna `live`, se tira). |
| **C** — palanca de enteros de la r53 | abierta | entrada `"r"(x)` en un `asm` que YA exista: sube `n_refs` sin gastar ranura. **Es posterior a casi todas las vedas del dossier**: hasta la r52 el catálogo sólo tenía palancas de coma flotante. |
| **D** — la base envenenada | **MUERTA** | los andamios NO envenenan la base: la SOSTIENEN. Quitarlos da 23 filas contra 14 y 60 contra 28, y los cuatro números reproducen la tabla de la r53 sin desviación. **Los ~200 negativos históricos siguen valiendo.** |
| **E** — Metrowerks | **VIVA** | `steering` y `madidct`. Sin DWARF y sin ayuda de Ghidra. Lo probado al 100 % en `steering.c`: la ranura explícita (`:798-808`) y el pragma acotado por función (`:649/670`, `:955/1007`). |
| **F** — orden del pool | **VIVA** | la semilla de la r59 desacopla el orden del pool del orden del reparto. Deja sin valor el rechazo de la r36f en `UpdatePlatInfo` («reordena el pool»). |

---

## Los andamios: 687, y son deuda de FUENTE

368 barreras `__asm__("")`, 241 pines `register T x asm("frN")`, 78 `asm { }` de MWCC, en
186 ficheros. **Ninguno lo escribió nadie en EA**: cada uno es un sitio donde no encontramos
la forma correcta. Que el DOL salga idéntico los hace *matching*, **no decompilación**.

Lo medido, y hay que leerlo entero para no sacar la conclusión equivocada:

- quitarlos **a secas** empeora (`eProject` 14 → 23 filas, `UpdatePlatInfo` 28 → 60) y
  devuelve **cero bytes**: el tamaño es idéntico con y sin ellos;
- **696 de los 727** viven en código que ya casa al 100 %, y `criticalpath` cerró al 100 %
  **con once dentro**;
- **pero nadie ha medido si existe una forma de fuente que los haga innecesarios.** Ésa es
  otra pregunta y sigue abierta.

Y hay un caso que demuestra que la pregunta tiene sentido: en `UpdatePlatInfo`, `regmap`
muestra que el original mete **DOS locales en `f5`** con vidas disjuntas, y un `register asm`
**no puede compartir registro por construcción**. Ese pin no es sólo infiel: **bloquea** el
reparto correcto, y mantiene la función en 28 filas cuando la base buena son 60.

**El método**: `regmap.py` clasifica en IDÉNTICO / PERMUTACIÓN / REPARTO / **ESTRUCTURA**.
Sólo las ESTRUCTURA son deuda de fuente recuperable — «eso va PRIMERO: los registros se
recolocan solos». Y el test es local y exacto: **el sha1 del `.o` idéntico antes y después**,
o se revierte. En unidades ya promocionadas eso no necesita enlazar nada.

**268 de los 687 viven en unidades ya promocionadas**, que no toca ningún agente: es donde
el experimento es limpio. Y las tres más densas son libc (`sf_log10.c` 22, `ef_pow.c` 16,
`e_pow.c` 15), **cuyo fuente original es público** — son fdlibm de Sun.

---

## Ghidra: integrado, y qué aporta

Sleigh Gekko (**8.627 de 8.627** paired-singles decodificadas) y `ghidra-dwarf1` parcheada
(12 líneas: los seis atributos de extensión de GCC caen fuera de `AT_lo_user..AT_hi_user` y
hacen que `decode()` lance). **13.824 funciones con firma real**, 4.076 structs, 2.872 enums.
Las 28 abiertas decompiladas al **100 %** en `docs/analisis/r63-ghidra-27-decomp.c`.

**Aporta**: entender la función con tipos reales (`this->mFlags`, no `param_1+0x24`), los
grafos de llamada de las 28, y la prueba de que **el mapa de funciones está completo**
(25.645 símbolos del ELF contra nuestras 18.432, sólo 116 sin correspondencia y los 116
explicados). Esa pregunta queda cerrada.

**No aporta**: la forma del original — **normaliza la semántica**. En `IdctColumn` imprime
siete `&&` y el ELF tiene un `or.` de siete términos con UN salto: nuestra fuente ya era la
buena. Siete de ocho «hallazgos de forma» cayeron al contrastarlos contra el objeto, y el
conteo de llamadas sale **28 de 28 igual**. Tampoco el reparto de registros, que es lo que
queda: para eso mandan los volcados RTL `.greg`/`.lreg`.

Y no ayuda con el orden: tau del grafo de llamadas +0,42/+0,67 contra +0,95/+1,00 del parseo.

---

*Estado al cerrar la r63. Compañeros: `TRAMPAS.md` (29 entradas, dónde miente cada
herramienta), `HERRAMIENTAS.md`, `r60b-auditoria-vedas.md` (las 28 funciones fechadas contra
la cronología de palancas), `r63-ghidra-veredicto.md`.*
