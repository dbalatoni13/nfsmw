# r57 · `phys8` — el `+8` de `.rodata` era **una `static` sin `inline`**, y el `+32` de `.bss` **dos globales robadas**

**zPhysics: 262.003 → 26.892 bytes de DOL distinto, un −89,7 %, y `linkdelta` pasa a
`.text +0 · IGUAL`: todas sus secciones miden ya EXACTAMENTE lo que el objetivo.**
Tres ediciones, ninguna toca una instruccion, `fncmp` 0/718 antes y despues.

    .o de zPhysics, TRES compilaciones seguidas: b88fe7fed0b96c9a02c24f21a54cc00a61118d50
    zGameplay 02eefc4bb761c85d6ee823a4f9d9ac761cebc7a9  (sin tocar, 0/768)
    zMisc     bc161c7db892495429f8163378bb146424fe88d6  (sin tocar, 0/450)
    checksplits LIMPIO · prefijochk LIMPIO · lcfix --check: 0 correcciones MIAS

---

## 0. La sonda, y por que mi cifra no es la de la r56

`p8_where.py` cuenta los bytes que difieren del `main.dol` original **emparejando las
secciones del DOL POR INDICE**, no por direccion. Hace falta: las secciones 2 y 3 del DOL
**se solapan 8 bytes** (`800034A0 + 3.804.448 = 803A41C0`, y la siguiente empieza en
`803A41B8`), y una sonda que busque «que seccion cubre esta direccion» se lleva 5 bytes
falsos **en el enlace BASE**. Con el emparejamiento por indice el enlace base da **0**, que
es lo que calibra la escala. Por eso mi 262.003 de partida no es el 398.070 de la r56: es
la misma unidad, medida sin ese sesgo.

---

## 1. La aritmetica que convierte 8 bytes en 235.000

`ldscript.ld` pone `. = ALIGN(32)` **delante de `.data`, de `.bss`, de `.sdata` y de
`.sdata2`**. Y `.rodata` empieza en `803C8C60`, que ya es multiplo de 32. Entonces:

    .data arranca en  803C8C60 + ALIGN32(tamano de .rodata)

El objetivo tiene `.rodata` = 312.608 B, **exactamente multiplo de 32**. O sea que estamos
pegados al borde: `+8` en `.rodata` se convierte en `+32` en la direccion de `.data`, y las
267.232 posiciones de `.data` pasan a compararse contra el byte de al lado.

**La consecuencia practica, y es lo que hay que llevarse de esta ronda: NO hay que acertar
el tamano al byte. Hay una VENTANA de 32.** Para zPhysics valia cualquier `.rodata` entre
**312.577 y 312.608**: sobraba entre 8 y 39 bytes. Lo mismo para `.bss`, donde la ventana
era «quitar entre 12 y 43».

Esto reordena el trabajo: una unidad con `rodata +8` no esta a un byte de precision, esta a
**un objeto muerto cualquiera de entre 8 y 39 bytes**.

---

## 2. Edicion 1 — `Ratio()` sin `inline`: **8 bytes que valian 226.385**

`PhysicsInfo.cpp:476` define

    static float Ratio(float a, float b) { return b > UMath::Epsilon ? a / b : 0.0f; }

y **nadie la llama** (un `grep` en todo el arbol solo encuentra la definicion). GCC 2.95
**la compila igual** —una `static` no-`inline` se emite aunque este muerta— y con ella su
pool: `$LC691` (`358637bd`) y `$LC692` (`00000000`), **4 bytes cada uno**.

Y aqui esta el mecanismo que no estaba escrito en ningun sitio del proyecto:

> **`-strip-unused-data` se lleva `size & ~7` de cada simbolo muerto.** De un literal de
> **4 bytes eso es CERO**: los 4 bytes se quedan enteros en el enlace. El literal muerto de
> 8 B cuesta 0; el de 4 B cuesta 4; el de 12 cuesta 4; el de 22 cuesta 6.

La cuenta cierra al byte y es reproducible: nuestro `.rodata` de objeto mide 17.872 B, los
36 simbolos muertos suman 1.829 B y su residuo `size&7` suma 29, y
`17.872 − (1.829 − 29) = 16.072`, que es exactamente lo que zPhysics aporta al enlace. El
objetivo aporta **16.064**.

`Ratio__Fff` **no existe en el `.o` extraido**, asi que los 8 B son deuda pura nuestra.
Con `static inline` la funcion muerta no se compila y los dos literales no nacen.

| | antes | despues |
|---|---|---|
| `linkdelta` | `.text +0  rodata+8  bss+32` | `.text +0  bss+32` |
| bytes de DOL | **262.003** | **35.618** |

**−226.385 B por una palabra clave.**

## 3. Edicion 2 — `Tweak_UseTweakerTunings` y `Tweak_TuningAero`: el `+32` de `.bss`

`PVehicle.cpp:48-49` las **definia**:

    bool  Tweak_UseTweakerTunings;
    float Tweak_TuningAero;

El ELF original las tiene en `8045ED9C` y `8045EDA0`, o sea **dentro del rango `.bss` de
zEcstasy**, no de zPhysics. Definirlas aqui metia 8 B que el objetivo no tiene.

Lo bonito es **como se localiza sin adivinar**: alineando por nombre los simbolos de `.bss`
de los dos objetos, **los 69 comunes estan en el MISMO desplazamiento salvo uno**, y el
delta `+8` aparece de golpe justo delante de `gap_07_80484E68_bss`, que es el ultimo. Un
solo dato senala al culpable: lo que va justo antes.

    T  +04C88   8 TheStockCars          N  +04C88   8 TheStockCars
    T  +04C90  80 gap_07_80484E68_bss   N  +04C90   4 Tweak_UseTweakerTunings
                                        N  +04C94   4 Tweak_TuningAero
                                        N  +04C98  80 gap_07_80484E68_bss

Declaradas `extern`, las suministra `zMiscSmall.o` (ya promocionada, las define con un
`asm()`), el enlace no queda con ningun UND y nuestro `.bss` pasa de 19.688 a **19.680**,
que es el del objetivo.

| | antes | despues |
|---|---|---|
| `linkdelta` | `.text +0  bss+32` | `.text +0  **IGUAL**` |
| bytes de DOL | 35.618 | **26.907** |
| `.sdata` / `.sdata2` | 2.091 / 2.171 | **0 / 0** |
| `.data` | 133.175 → 32 → | **18** |

## 4. Edicion 3 — el `asm()` de `lbl_8041F00C` a `VehicleSystem.cpp`

El comentario ya lo decia desde la r33 («en el objetivo va DENTRO de VehicleSystem.cpp,
entre `ENABLE_ROLL_STOPS_THRESHOLD` y `PAD_DEAD_ZONE`») y nadie lo habia movido: escrito en
`zPhysics.cpp` **detras** del `#include`, el bloque caia detras de las dos globales.
**Un `asm()` de fichero se emite DONDE ESTA ESCRITO**, asi que se mueve dentro del `.cpp`,
entre las dos definiciones. `.data` baja de 18 a **7** bytes distintos y el DOL a **26.892**.

---

## 5. Lo que le queda a zPhysics, y es UNA sola cosa

| seccion | bytes distintos |
|---|---:|
| `.text` (`800034A0`) | 20.191 |
| `.rodata` (`803C8C60`) | 6.692 |
| `803A41B8` | 2 |
| `.data` | 7 |
| `.sdata`, `.sdata2`, `.ctors`, `.dtors`, `.init` | **0** |

**Dentro de la ventana de `.text` de zPhysics hay 20.090 de esos bytes y son 3.150 rangos
de EXACTAMENTE 2**: las mitades de 16 bits (`@ha`/`@l`) de `lis`/`addi`/`lwz` que apuntan
al pool. Los 101 restantes de la seccion son referencias al pool desde otras unidades. O
sea que **el `.text`, la `.rodata` y los 7 B de `.data` (punteros dentro de `put_maps`) son
el MISMO problema: el ORDEN del pool de literales.** **26.890 de los 26.892 bytes cuelgan
de el**; los 2 sueltos estan en la seccion `803A41B8`.

Y el orden esta acotado. Comparando por relocaciones quien es dueno de cada trozo de pool
en los dos objetos: **140 duenos en el objetivo, 168 en el nuestro, 120 comunes, y 116 de
esos 120 en su sitio.** Los desajustes son cuatro funciones (`GetPercent` / `SetLevel` /
`MatchPerformance` de `Physics::Upgrades`, y `OnBehaviorChange__13PhysicsObject`) **mas el
sitio de las cadenas MUERTAS**, que es lo que de verdad manda: la primera diferencia esta
en `803F6A5C`, donde el objetivo tiene los dos flotantes de
`GetObjectMatrix__C16SmackableTrigger` (`3F800000 00000000`) **delante** del bloque de
`Attrib::Gen::*` y nosotros los ponemos 508 B **detras**.

`p8_poolord.py` (scratchpad) es la herramienta: da la secuencia de duenos de pool de los dos
objetos y sus desajustes.

---

## 6. Extrapolacion: las dos palancas en las 23 SourceLists

**No doy esto por frente sin contarlo** (`nfsmw-extrapolar-frentes`). Censo con
`p8_frente.py`, sin compilar nada:

| unidad | A) residuo de literal muerto | B) datos globales de mas |
|---|---:|---|
| zMain | 150 B | – |
| zBWare | 100 B | – |
| zLua | 89 B | – |
| zPhysicsBehaviors | 84 B | 4 B |
| zSpeech | 56 B | 4 B |
| zFe2 | 49 B | – |
| zCamera | 48 B | – |
| zEagl4Anim | 40 B | – |
| zGameplay | 27 B | – |
| zEAXSound | 26 B | 8 B |
| zMisc | 24 B | 12 B |
| zSim | 21 B | – |
| zEAXSound2 | 15 B | 96 B |
| zWorld | 14 B | 544 B |
| zAnim | 12 B | 8 B |
| zEcstasy | 4 B | 297 B |
| zFe | 3 B | – |
| zWorld2 | 3 B | 16 B |
| zAI | – | 8 B |
| zGameModes | – | 64 B |
| zMiscSmall | – | 64 B |
| zFeOverlay | – | 8 B |
| zPlatform | – | 1 B |
| **TOTAL** | **776 B** | **1.134 B** |

* **A** = literales `$LC` con `size & 7 != 0` cuyo unico dueno en `.text` es una funcion que
  el `.o` extraido NO tiene. Es una lista de **candidatos**, no un cobro: hay que
  comprobar uno por uno que el objetivo no los tenga (en zPhysics, ya con `linkdelta` a
  `IGUAL`, la sonda sigue senalando 11 B: son falsos positivos que el objetivo tambien
  tiene).
* **B** = simbolos de datos **globales** que definimos y el `.o` extraido no, y que en el
  ELF original viven **fuera** de la ventana de la unidad. `zWorld` con 544 B y `zEcstasy`
  con 297 son los dos gordos, y los dos son de otros agentes.

Lo que hace que esto valga la pena no son los bytes: es que **cada unidad que pase a
`IGUAL` se lleva de golpe el desplazamiento de `.data`, `.sdata` y `.sdata2`**. En zPhysics
eso fueron 235.111 B por 20 bytes de datos.

---

## 7. Regresiones

**Ninguna, y esta comprobado por construccion**: los cuatro ficheros tocados
(`PhysicsInfo.cpp`, `Common/PVehicle.cpp`, `Common/VehicleSystem.cpp`, `zPhysics.cpp`)
**solo los incluye `zPhysics.cpp`** (`grep` sobre `SourceLists/`). **No he tocado ninguna
cabecera.** `zGameplay` y `zMisc` recompiladas al terminar con el `sha1` de partida y
`fncmp` 0/768 y 0/450.

`lcfix.py --check` deja **CERO correcciones mias**. La cifra global se mueve mientras
corren los demas agentes (la vi en 128 y al cerrar en **83**), y en las dos lecturas el
100 % era de `zSpeech` y `zAI`, no de mis tres unidades. Mis ediciones no mueven ni un `$LC` de los 96 que
`keep.lst` fija para zPhysics (comprobado: `lcfix --check` daba limpio justo despues de
cada una de las tres).

---

## 8. Propuestas (no las aplico)

1. **`Tweak_TuningAero` / `Tweak_UseTweakerTunings` son de zEcstasy**, `8045ED9C` y
   `8045EDA0`, en ese orden (el `float` primero). Hoy las define `zMiscSmall.cpp` con un
   `asm()` en **`.data`** (`0x8041EFDC`), que es la seccion y la direccion equivocadas —le
   sirve a zMiscSmall para rellenar 8 B de su `.data`, pero deja los `@ha`/`@l` de zPhysics
   apuntando a otro sitio—. Cuando zEcstasy las emita en su `.bss`, zPhysics se lleva unos
   cuantos bytes mas de `.text` gratis. **No es mio: es de `ecs` y de `miscsmall`.**
2. **zGameplay necesita `@lc` en `keep.lst` para 26 cadenas** que `deadstr.py` da como
   presentes en el objetivo y que perdemos al enlazar (**432 B**). Hoy `keep.lst` tiene 35
   entradas `$LC` de zGameplay y 96 de zPhysics; zMisc no tiene ninguna. **Pero no basta**:
   zGameplay esta en `rodata −136`, asi que anadir 432 lo pondria en `+296`. Le sobran ~296 B
   de otra cosa y hay que encontrarlos antes. Es trabajo de ventana (`keep.lst` es entrada
   del enlace).
3. **`p8_where.py` (bytes de DOL por seccion, emparejando POR INDICE) merece subir a
   `scripts/`**, junto con `p8_poolord.py` (duenos del pool) y `p8_frente.py` (el censo de
   §6). La primera es la que reordena la cola de trabajo; sin el emparejamiento por indice
   miente 5 bytes en el enlace base.

## 9. Sorpresas

1. **La ventana de 32 bytes.** Llevabamos leyendo «`rodata +8`» como «hay que acertar 8».
   Es «sobran entre 8 y 39». Cambia por completo que ediciones merece la pena probar.
2. **Una `static` sin `inline` que nadie llama cuesta `size & 7` por cada literal de su
   pool, y para un flotante de 4 B eso es el literal ENTERO.** Es el complemento del
   «primer de pool»: aquella nota dice que una `static inline` muerta con `return "literal"`
   mueve una cadena **por cero bytes**; ahora se por que —una cadena de 8 B tiene residuo
   0— y se que con 4 o con 12 **no es gratis**.
3. **El `+8` de `.bss` se localiza en una linea**: alineando por nombre los simbolos de
   `.bss`, 69 de 69 estan en el mismo desplazamiento hasta que uno salta. No hace falta
   volcar nada.
4. **`.data` al 100 % distinta seguia sin ser contenido**, ni siquiera despues de la r56:
   de los 133.175 B que quedaban solo **18** eran contenido, y de esos 11 eran un `asm()`
   escrito tres lineas mas abajo de donde toca.
5. **3.150 diferencias de `.text` de exactamente 2 bytes** no son 3.150 problemas: son un
   solo problema —el orden del pool— visto a traves de los inmediatos de 16 bits. **La
   distribucion de TAMANOS de los rangos de diferencia dice de que familia es el fallo**
   antes de mirar una sola instruccion.
6. **zMisc sigue sin merecer la pena** y ahora con una cifra mas: `rodata +2840` obliga a
   quitar entre 2.840 y 2.871 B, y sus dos palancas de esta ronda suman 36. El brief tenia
   razon.
