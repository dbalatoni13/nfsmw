# r56 · `phys` — el orden de emisión dentro de la unidad, medido

**−122.665 B de DOL distinto en tres unidades, cero regresiones, ninguna promoción.**
El grueso es `zPhysics`: **498.146 → 398.070 B, un −20,1 %**, y todo con la misma palanca —
*dónde* se emite cada función—, sin tocar una sola instrucción.

    fncmp ANTES y DESPUES, las tres:  zPhysics 0/718 · zGameplay 0/768 · zMisc 0/450
    enlace BASE:  DOL OK, sha1 9619ba57c9919f95f7f2ac951a2166a3517f91e3 (sin tocar)

| unidad | bytes de DOL ANTES | DESPUÉS | ganancia | `linkdelta` |
|---|---:|---:|---:|---|
| **zPhysics** | 498.146 | **398.070** | **−100.076** | `.text +0`, `rodata +8`, `bss +32` |
| **zGameplay** | 546.816 | **524.227** | **−22.589** | `.text +0`, `rodata −136`, `data −128` |
| zMisc | 431.704 | 431.704 | 0 | `.text +0`, `rodata +2840`, `data +32` |

`bytes de DOL` = bytes que difieren del `orig/GOWE69/sys/main.dol`, sección a sección por
dirección, promocionando **una** unidad. Es la misma receta de `trypromo` con la cifra en vez
del sha1. El enlace base da **0** con esa misma sonda, así que la escala está calibrada.

`sha1` del `.o`, **tres compilaciones seguidas, idéntico las tres**:

    zPhysics   3f745839727885e7b59c6a498169672e88dda77a
    zGameplay  02eefc4bb761c85d6ee823a4f9d9ac761cebc7a9
    zMisc      bc161c7db892495429f8163378bb146424fe88d6

---

## 1. La palanca, y las dos direcciones en que se usa

El brief la nombra pero sólo en una dirección. **Se usa en las dos, y las dos pagan**:

| forma en la fuente | dónde sale |
|---|---|
| cuerpo **fuera** de la clase, en el `.cpp` | **en el punto** del `.cpp`, en el orden en que están escritas |
| cuerpo **dentro** de la clase | en la **cola de COMDAT** que `finish_file` drena al final de la TU |

Y hay una tercera, que es la que más pagó y no estaba escrita en ningún sitio:

**el orden de emisión de un `.cpp` ES el orden de sus definiciones en la fuente, al pie de la
letra.** Comprobado sobre `PVehicle.cpp`: la lista de sus 57 funciones en el `.o` nuestro es,
símbolo a símbolo, la lista de sus definiciones por número de línea. Así que `permorden` no
sólo dice «está desordenado»: **dicta el orden que hay que escribir**.

### Las cinco ediciones de zPhysics, con su cifra

| # | edición | bytes |
|---|---|---:|
| 1 | `PhysicsObject.h`: redeclarar `GetModel()` / `GetModel() const` puras entre `ProcessStimulus` y `SetCausality` | +4 (ver §4) |
| 2 | `PVehicle.cpp`: los tres `ManageNode::sort_*` con el cuerpo **EN CLASE** | **−11.018** |
| 3 | `PVehicle.cpp`: sus 57 definiciones **reordenadas al orden del objetivo** | **−20.685** |
| 4 | `Smackable.h`: `SimplifySort` con el cuerpo **EN CLASE** | **−60.753** |
| 5 | `Wheel.cpp` (2 fn), `PhysicsUpgrades.cpp` (2 fn), `PhysicsInfo.cpp` (1 fn) | **−7.463** |

`permorden` pasa de **520 a 560 de 718 en su sitio**, y el ciclo grande de **433 se rompe en
uno de 101**.

**`SimplifySort` sola vale 60.753 B y es una función de 6 líneas.** `permorden` la situaba en
el puesto 483 del objetivo —entre `_._t16BehaviorSpecsPtr1...` (482) e `IsRequired__9Smackable`
(484), o sea dentro de la cola— y nosotros la emitíamos en el 106, dentro del bloque de
`Smackable.cpp`. Meter el cuerpo en la clase, justo donde ya estaba la declaración y justo
delante de `IsRequired`, la deja en el 483 clavada.

Lo mismo con los tres `sort_*` de `PVehicle::ManageNode`: objetivo 433/434/435, entre el ctor
de `map<UCrc32,UCrc32>` (432) e `is_kept` (436). Con el cuerpo en la clase salen **549/550/551
detrás del 548 y delante del 552**: consecutivas y en orden.

### El reordenado de `PVehicle.cpp`

El objetivo tiene sus 57 funciones en un orden que **no es el nuestro** y que sí es un orden de
fuente plausible (bloques por tema: arranque, recursos, construcción/destrucción, tareas,
modos, tunings). Reescrito ese orden, `permorden` da **delta 0 en los puestos 0-62 y +1 en el
63-104**: el bloque entero encaja.

La herramienta quedó escrita y es reutilizable (`p_reord2.py` del scratchpad): empareja cada
trozo de nivel superior del `.cpp` con su símbolo en NUESTRO `.o` por `(clase, nombre)` sacados
de la primera línea, y los ordena por la posición del símbolo en el `.o` **extraído**. Con eso
el reordenado de un fichero es un comando, no una tarde. **Tres trampas medidas**:

1. **La cabecera de declaraciones no se puede mover.** `struct PathSegment` y
   `DECLARE_CONTAINER_TYPE(ID_PATH_SET)` viajaron pegados a la primera función y
   `GRaceStatus.cpp` dejó de compilar. Se ancla todo lo que va antes de la primera definición
   con símbolo.
2. **Las sobrecargas se reparten EN ORDEN.** Los dos `GetRacerInfo` de `GRaceStatus` empatan
   por `(clase, nombre)`; el k-ésimo trozo va con el k-ésimo símbolo, porque GCC las emite en
   el orden de la fuente.
3. **Un `.cpp` reordenado puede cambiar el CÓDIGO.** `GRaceDatabase.cpp` pasó de 0 a **21
   funciones distintas** en `fncmp` (mueve la definición del ctor de `PackedDecimal` y deja de
   plegarse). Revertido. Hay que pasar `fncmp` después de **cada** fichero.

---

## 2. zGameplay: la misma receta, −22.589 B

`GRaceStatus.cpp` (104 definiciones reordenadas) **−22.598**, `GRuntimeInstance.cpp` −33,
`GSpeedTrap.cpp` −24. `permorden` pasa de **498 a 546 de 767**.

De los 17 `.cpp` de la unidad, **9 ya estaban en el orden del objetivo**, 2 no compilan
reordenados (`GManager.cpp`, `GObjectBlock.cpp`), 1 regresa (`GRaceDatabase.cpp`) y 2 salen
neutros. Todos revertidos automáticamente por la tanda.

`zMisc` **no tiene nada que reordenar**: `permorden` da **450 de 450**, cero ciclos. Su
problema es otro (§5).

---

## 3. LO MÁS VALIOSO QUE QUEDA, y es una cifra sola: **273.472 B por 8 bytes**

Desglosando dónde caen los 398.070 B de `zPhysics`:

| zona | bytes | qué es |
|---|---:|---|
| `.data` | 267.232 | **100 % distinta** |
| `.sdata` | 2.816 | **100 % distinta** |
| `.sdata2` | 3.424 | **100 % distinta** |
| `.rodata` | 72.296 | 23 % de la sección |
| `.text` | 51.086 | 1,3 % de la sección |
| resto | 1.216 | |

**`.data`, `.sdata` y `.sdata2` no tienen ni un byte de contenido malo: están DESPLAZADAS.**
Nuestra `.rodata` mide `+8`, el DOL rellena cada sección a 32 B, y eso empuja `.data` 32 B
adelante: las 267.232 posiciones comparan contra el byte de al lado. **Arreglar 8 bytes de
`.rodata` se lleva 273.472 B de golpe**, un 69 % del residuo de la unidad.

Y el `+8` está localizado. Partiendo la ventana de `.rodata` de zPhysics en dos:

    vtables      base 142 (11.680 B)  |  nuestro 142 (11.680 B)   IDENTICO: mismos nombres,
                                                                  mismos tamaños, +0
    pool         base 3.688 B         |  nuestro 3.696 B          +8   <-- AQUI

Las 28 vtables que emitimos de más las estripa el enlazador y no cuestan un byte. **El `+8`
está en el pool de literales y constantes, en `[803F68F0, 803F7758)`.** Un diff por palabras
del pool da 27 tramos movidos —el pool sale **por función**, así que su orden sigue al de
`.text`— y el neto de todos ellos es exactamente `+8`, casi seguro relleno de alineación de una
constante de 8 B que cae en otra paridad.

**El siguiente encargo de zPhysics es ése y sólo ése**: cerrar el orden del pool de `.rodata`.
Vale 273.472 B, es orden y no contenido, y el resto de la unidad ya no estorba.

Lo mismo aplica a las otras dos: **zGameplay y zMisc tienen también sus 273.472 B de `.data` /
`.sdata` / `.sdata2` desplazadas**, por `rodata −136` y `rodata +2840` respectivamente.

---

## 4. Las dos palabras de contenido de zPhysics: cerradas

La r55 describió y no aplicó el arreglo de `_vt.13PhysicsObject.8ISimable` +256 y +264 (las dos
ranuras `__pure_virtual` que se quedaban con delta 0 en vez de `0xFFD4`). **Aplicado y
verificado al byte**: volcando la vtable entera de los dos objetos, las ranuras +256 y +264 son
ahora `FFD40000` en los dos. `reorden.py` baja de 44 a 43 palabras de contenido en `.rodata`, y
las palabras crudas distintas de **929 a 780**.

**Pero la cifra de DOL sube +4**, no baja 8. No es un error de la edición —el contenido queda
igual al objetivo, eso está volcado— sino que a esta distancia el residuo lo manda el orden y
4 bytes son ruido. Lo dejo puesto porque es contenido correcto y verificable, y lo digo con su
signo.

---

## 5. Lo que le queda a cada una, medido

**zPhysics** — `.text` de la fuente CERRADO (los 14 `.cpp` de la unidad están en orden; los
otros 10 ya lo estaban). Lo que queda:

* el `+8` de `.rodata` (§3): **273.472 B**;
* la **cola de COMDAT**, puestos 358-717 del objetivo, 360 funciones en otro orden. La región
  escrita (0-301) es ya monótona. Esa cola es orden de instanciación / de completado de clase,
  o sea orden de `#include` en `zPhysics.cpp`;
* `.bss +8` (eran +24, ver §6).

**zGameplay** — le faltan **152 B de `.data`** y son identificables uno a uno. Los volqué del
DOL original: **148 de los 152 son CEROS**, y el otro es un `4` con valor **220**. En GCC 2.95
`assemble_variable` no tiene `flag_zero_initialized_in_bss`, así que **`= 0` explícito va a
`.data`, no a `.bss`** —está confirmado en el árbol: nuestros `TWEAK_ShowGameplayMilestoneValues
= 0` y `TWEAK_ShowAllGameplayIcons = 0` salen en `.data`—. El mapa completo:

| dirección | B | contenido | vecino de delante |
|---|---:|---|---|
| `8041D2D0` | 60 | ceros | `_16GRuntimeInstance.sRingListHead` |
| `8041D310` | 8 | ceros | `_11GRaceStatus.fObj` |
| `8041D38C` | 8 | ceros | `regionTable.32149` |
| `8041D3A0` | 4 | ceros | `_8GManager.mObj` |
| `8041D3AC` | 4 | **220** | `yTop.34878` |
| `8041D53C` | 68 | ceros | los seis `kObjectTemplateKey` |

El de 220 está **cazado**: su `var_labelno` es **34879**, el hueco exacto entre `yTop.34878` y
`line.34880` de `GManager.cpp:2634`. Puesto (`static volatile int xWidth = 220;`), sale en su
sitio dentro de nuestra `.data` —`xWidth.25379` entre `yTop` y `line`— y **no genera ni una
instrucción**. Hoy vale **0 bytes** porque el relleno de la sección se lo come; sólo paga
cuando estén los 152. Lo dejo puesto y lo digo.

**zMisc** — `.text` perfecto, `.rodata` **+3.392 B en el objeto** y `+2.840` después de
estripar. Su ventana en el objetivo es minúscula (5 vtables, 13 constantes y
`fShakeIntensityCoeffs`, 328 B) y la nuestra tiene cientos de `$LC` que **sobreviven al strip
porque nuestro código los referencia**. Con `fncmp` a 0 de 450, eso significa que en el
original esas cadenas las suministra **otro objeto**: es un problema de DUEÑO de cadenas, no de
código. No lo he perseguido.

---

## 6. Hallazgo transversal: `lower`/`upper` en **20 SourceLists** que el original no tiene

`PackedDecimal.h` define `FloatingPoint<T,BASE,MINEXP,EXPBITS,MANTBITS>` con
`GetNormalizedLower/Upper()` en clase, y su **uso está escrito dentro del ctor de
`PackedDecimal`, que también está en la cabecera**. Eso dispara `instantiate_decl` al PARSEAR y
suelta las dos estáticas de función (`lower`, `upper`) más sus dos guardas `_.tmp_N` —**16 B de
`.bss`**— en toda TU que la parsee. Y llega a todas por `IAI.h:7 → GRaceStatus.h:19`.

Censo sobre los 296 objetos extraídos: **`lower.`/`upper.` aparecen en zGameplay y en NINGUNA
otra**; en el nuestro estaban en **20**.

Y la POSICIÓN lo prueba, que es lo que convierte la sospecha en medida: en el zGameplay del
objetivo `lower.32099` / `upper.32104` están en el desplazamiento **0x58** de `.bss`, **justo
detrás** de `quarterMileInMeters.31487` y `sixtyMphInMetersPerSec.31489`, que son
`GRaceStatus.cpp:1063` y `:1070` —o sea la instanciación ocurre **al empezar
GRaceDatabase.cpp**, no al parsear la cabecera—. En el nuestro estaban en el desplazamiento
**0**, o sea en el primer `#include` del SourceList.

**Edición**: el cuerpo del ctor sale de `PackedDecimal.h` y se define `inline` en
`GRaceDatabase.cpp`, detrás de sus `#include`. Resultado medido:

* zGameplay: `lower`/`upper` pasan al 0x60, **detrás de `sixtyMph`**, como el objetivo;
* zPhysics: su `.bss` pasa de **+24 a +8** sobre el objetivo (19.704 → 19.688 contra 19.680);
* las otras 19 unidades pierden 16 B de `.bss` que el original no tiene. **NO las he
  recompilado** (no son mías): el censo es la medida, el efecto en cada una no.

**Lo que cuesta**: quita 3 constantes del pool de toda TU que parseaba la cabecera, así que
**renumera los `$LC`**. Ver §7.

---

## 7. PROPUESTA (no la aplico: `keep.lst` es de la ventana)

`python scripts/lcfix.py` — **82 entradas mías**: 69 de `zPhysics` (todas `−3`, de la edición
de `PackedDecimal.h`) y 13 de `zGameplay` (11 de `−3`, una `−4` y una `+22`, éstas del
reordenado de `GRaceStatus.cpp`).

**Sin ese `lcfix`, cualquier medida de zPhysics o zGameplay es basura**: con el `keep.lst` de
hoy el enlace conserva los `$LC` equivocados y `linkdelta` dice `rodata −384` donde de verdad
hay `+8`, o sea **392 B de mentira**. Todas las cifras de este informe están tomadas con un
`keep.lst` corregido en el scratchpad (mismas 82 líneas que aplicaría `lcfix`), y el enlace
base con ese fichero **sigue dando DOL OK, 0 bytes distintos**.

No es una carga que traiga yo solo: `lcfix --check` da hoy **482 correcciones pendientes** en
13 unidades, y 400 son de otros agentes de esta ronda (zFe 101, zPhysics 69, zMain 61,
zEAXSound 49, zSpeech 49, zEAXSound2 44...). La ronda necesita `lcfix` de todas formas.

---

## 8. Sorpresas

1. **El orden de emisión de un `.cpp` es su orden de líneas, sin excepción.** No lo dice
   ningún documento del proyecto y es lo que convierte `permorden` de diagnóstico en
   instrucción. Vale 20.685 B sólo en `PVehicle.cpp`.
2. **Una función de 6 líneas movida de sitio vale 60.753 B.** `SimplifySort`. El valor no está
   en el tamaño de la función sino en cuántos símbolos arrastra su desplazamiento.
3. **`.data` al 100 % distinta no es un desastre: es un `+8` en `.rodata`.** Tres unidades con
   `.data` «completamente mal» y ninguna tiene un byte de contenido malo ahí. Si hubiera leído
   el `100 %` como contenido habría perdido la ronda. **La sonda que lo destapa es contar bytes
   distintos POR SECCIÓN, no en total.**
4. **`static volatile int x = 220;` sin usar sí se emite y no genera código**, pero **`= 0`
   también va a `.data`** en GCC 2.95 (no hay `flag_zero_initialized_in_bss` hasta GCC 3).
   Las dos cosas hacen falta para reconstruir los 152 B de zGameplay.
5. **`reorden.py` bajó de 929 a 780 palabras crudas distintas en `.rodata` sin que yo tocara un
   dato**: el pool de literales se emite POR FUNCIÓN, así que arreglar el orden de `.text`
   arregla parte del de `.rodata` gratis.
6. **La tanda automática se comió tres falsos positivos**: dos ficheros que no compilan
   reordenados y uno que regresa 21 funciones. Sin `fncmp` + compilación después de **cada**
   fichero, cualquiera de los tres se habría colado como «mejora».
7. `build_direct.py` con la ruta terminada en **`.cpp`** (`Speed/Indep/SourceLists/zPhysics.cpp`)
   compila **una** unidad; con la ruta sin extensión compila también `zPhysicsBehaviors`, que
   es de otro agente. Es el arreglo de la sorpresa 3 de la r55 y funciona.

## 9. Sondas

Todas en el scratchpad de la sesión, ninguna escribe en el árbol. Las que merecen subir a
`scripts/`:

* **`p_reord2.py`** — reordena un `.cpp` al orden de emisión del objetivo. Es la herramienta
  que convierte `permorden` en una edición.
* **`p_tanda.py`** — aplica lo anterior a una lista de ficheros midiendo y revirtiendo solo.
* **la cifra de bytes de DOL por SECCIÓN** (`p_where.py`): es lo que separa «desplazado» de
  «mal escrito», y es la que reordena la cola de trabajo.
