# r55 — `zAI`: las dos ediciones del encargo, y tres más que salieron de ellas

Unidad: `Speed/Indep/SourceLists/zAI` (272.796 B de `.text`). Ficheros tocados: `AIGoal.cpp`,
`AIVehicle.cpp`, `AIMath.cpp`, `AISpawnManager.cpp`. **Ninguna cabecera.** Ninguna unidad ajena
reconstruida.

## Resumen en una línea

**El `.text` de `zAI` pasa de 126.180 B descolocados a 35.784: +90.396 B puestos en su sitio en
el ENLACE, del 53,7 % al 86,9 %, con CERO regresiones. Y el resultado estructural importa más
que la cifra: la mitad de parseo de la unidad —226.644 B, del primer símbolo al último de
`Gps.cpp`— está ahora ENTERA a desplazamiento +0. Todo lo que queda vive dentro de la cola de
`finish_file`.** No promociona: sigue `DOL ROTO`, y le faltan además `.rodata −240` y
`.data −384`.

Las dos ediciones del encargo valieron 83.444 B (prometía 83.340). Las otras tres las encontré
midiendo el residuo: son reordenaciones de fuente dentro de mi propia unidad y valen 6.952 B
más.

---

## 1. Las cinco ediciones

### (1) `AIGoal.cpp:305` — el destructor, fuera de la clase

```cpp
-    ~AIGoalStaticRoadBlock() override {}          // en clase = inline = a la cola
+    ~AIGoalStaticRoadBlock() override;
 };
 ...
 AIGoalStaticRoadBlock::AIGoalStaticRoadBlock(ISimable *isimable) : AIGoal(isimable) { ... }
+
+AIGoalStaticRoadBlock::~AIGoalStaticRoadBlock() {}
```

`_._21AIGoalStaticRoadBlock` pasa del offset `0x43880` (la cola, **+105.820**) a `0x29B84`,
pegado a su constructor y **al mismo desplazamiento que su vecindario**. Los otros destructores
de `AIGoal*` no se han tocado: ésos sí van a la cola.

### (2) `AIVehicle.cpp:2369,:2373` — `inline` en `WRoadLane::GetBits` / `GetBitsSigned`

| | antes | después | objetivo |
|---|---:|---:|---:|
| `GetBits__C9WRoadLaneii` | `0x215CC` (**−122.928**) | cola | `0x3F5FC` |
| `GetBitsSigned__C9WRoadLaneii` | `0x215E4` (−122.928) | cola | `0x3F614` |

Ensayo acotado a la unidad, como pedía el encargo: **la cabecera `WRoadElem.h` sigue sin
tocarse** (ver §5, sigue siendo la edición fiel y no la poseo).

### (3) `AISpawnManager.cpp` — dos cuerpos en el sitio equivocado

El objeto objetivo dice el orden que tenía el fuente original:

```
     OBJETIVO                             NOSOTROS (antes)
     __14AISpawnManagerff        92       __14AISpawnManagerff        92
     _._14AISpawnManager         52       _._14AISpawnManager         52
  -> GetBasePosition            136       RespawnAvailable           284
  -> GetBaseForwardVector       136       GetSpawnPointOnSegment     200
     RespawnAvailable           284       GetSpawnLocation           360
     GetSpawnPointOnSegment     200    -> GetBasePosition            136
     GetSpawnLocation           360    -> GetBaseForwardVector       136
     CheckSpawnPosition         660       CheckSpawnPosition         660
     RefreshSpawnData          1120       RefreshSpawnData          1120
```

Movidos los cuerpos de `GetBasePosition` y `GetBaseForwardVector` delante de
`RespawnAvailable`. Lo confirmaban ya las dos primeras transiciones del recorrido —
`GetBasePosition d=+844` con un tramo de 272 B y `RespawnAvailable d=−272` con un tramo de
844 B: el intercambio de dos bloques adyacentes, al byte.

### (4) `AIMath.cpp` — `PredictPosition` va la última

```
     OBJETIVO                             NOSOTROS (antes)
     AngleTo                    172       AngleTo                    172
     TimeToIntercept            216       PredictPosition            336
     TimeToImpactXZ             684       TimeToIntercept            216
     PredictPosition            336       SegmentSphereIntersect     404   <- (5)
                                          TimeToImpactXZ             684
```

Movido el cuerpo de `PredictPosition` al final del `namespace Math`.

### (5) `AIMath.cpp:79` — `AI::Math::SegmentSphereIntersect` era `inline`

El objetivo lo emite en `0x425C0` del objeto (enlazado `0x80045A60`), **dentro de la cola** y
cerca del final; nosotros lo teníamos en `0x362A4`, en medio de `AIMath.cpp`. Un cuerpo
no-inline no puede caer ahí. Y sus **404 B son exactamente** el `+404` con el que salía
descolocado **todo `Gps.cpp`** (4.196 B: `Construct__3Gps`, `__3Gps`, `_._3Gps`, `OnTask`,
`Update`, `Engage`, `Render`, `GPS_Disengage`, `GPS_Engage`, `GPS_IsEngaged`,
`RenderGpsArrows`).

Anteponerle `inline` lo saca de ahí y lo manda a la cola. **No cae en su sitio exacto** (queda
a `+5.348` del objetivo), pero eso ya es residuo de cola: lo que paga es haberlo quitado de en
medio de la zona de parseo.

Los tres ficheros de (3)(4)(5) los incluye **sólo** `zAI.cpp`; no son cabeceras y no los
comparte ninguna otra unidad (comprobado con `grep` sobre `src/Speed/*/SourceLists/*.cpp`).

## 2. La medida

### `fncmp` — ANTES y DESPUÉS, la red de seguridad

```
ANTES    0 de 1030 funciones con el CODIGO distinto -- 0 B
         337 mas solo con nombres de simbolo distintos (199388 B)
DESPUES  0 de 1030 funciones con el CODIGO distinto -- 0 B
         337 mas solo con nombres de simbolo distintos (199388 B)
```

**Cero regresiones**, en las tres pasadas intermedias y en la final. Ni una función que casaba
ha dejado de casar, y la cuenta de reubicaciones con nombre distinto es idéntica.

### El censo de desplazamiento **ENLAZADO**

`dolwhere` **no arranca en `zAI`**: aborta con `LAS SECCIONES NO COINCIDEN` porque `.rodata` y
`.data` difieren de tamaño, que es justo el caso de esta unidad. Así que medí a mano lo mismo
que hace `linkdelta`, pero símbolo a símbolo: enlace base (todos los `obj/`) contra enlace con
mi `.o` sustituido, y desplazamiento de cada uno de los 1.030 símbolos de `.text` del objetivo.

| estado | EN SITIO (+0) | FUERA | % | Δ |
|---|---:|---:|---:|---:|
| base (revertido y reconstruido para medirlo) | 146.616 B | **126.180 B** | 53,7 % | — |
| + (1) y (2) | 230.060 B | 42.736 B | 84,3 % | **+83.444** |
| + (3) y (4) | 231.392 B | 41.404 B | 84,8 % | +1.332 |
| + (5) | **237.012 B** | **35.784 B** | **86,9 %** | +5.620 |
| **total** | | | | **+90.396 B** |

El `126.180 B` de partida es, al byte, el que dejó medido `r55-med-zAI.md`: son la misma
medida. Y los 83.444 de las dos ediciones del encargo son sus 83.340 previstos más los 104 B
del destructor, que su cuenta no incluía.

### El resultado estructural: la zona de parseo está cerrada

`zAI` es **una sola TU** — `zAI.cpp` hace `#include` de los 35 `.cpp` —, así que hay **una
única cola de `finish_file`**, al final del todo. La frontera está en el offset `0x37554` del
objeto (enlazado `0x8003A9F4`), justo después de `RenderGpsArrows__FP5eView`, el último símbolo
de `Gps.cpp`.

```
   800034A0  d=+0  tramo 226644 B  __16AITrafficManagerGQ23Sim5Param   <- UN SOLO TRAMO
   8003A9F4  d=+440 ...                                               <- empieza la cola
```

**Los 226.644 B de la zona de parseo son ahora un único tramo a +0**, sin una sola transición.
Antes eran 226.644 B con siete transiciones dentro. Todos los 35.784 B que quedan, y las 140
transiciones, están **dentro de la cola**.

### Lo demás

```
linkdelta   Speed/Indep/SourceLists/zAI     .text +0   rodata-240 data-384   (igual que antes)
permorden   en su sitio 861 -> 866 de 1030;  ciclos 9 -> 8 (345,24,22,21,7,6,4,2)
trypromo    Speed/Indep/SourceLists/zAI     DOL ROTO (b7707add4c6d)
```

El `.o` se reconstruye **byte a byte idéntico** al recompilar sin cambiar nada (comprobado con
`cmp`), así que nadie me ha contaminado la medida mientras la tomaba.

## 3. Lo que le queda a `zAI`

### a) 35.784 B en la cola, 140 transiciones — el orden de instanciación

Los tramos gordos que quedan (todos posteriores a `0x8003A9F4`):

```
 -584  6576 B     -760  3244 B    +1212  3188 B    -828  2560 B    -408  1344 B
```

y una nube de `find__H2ZPP<T>Z…`, `clear__Q24_STLt10_List_base2Z<T>`,
`reserve__Q24_STLt6vector2Z<T>` y `Get__CQ26Attribt7TAttrib1Z<T>` con deltas de ±5.000 a
±18.000. **Su orden lo decide el primer uso dentro de una función**, y la cabeza de nuestra
cola no se parece a la del objetivo:

```
   OBJETIVO                              NOSOTROS
   Get__CQ26Attribt7TAttrib1ZQ25UMath…   ClassKey__Q36Attrib3Gen13pursuitlevels
   __lower_bound__H4ZPCUiZUi…            ClassKey__Q36Attrib3Gen17pursuitescalation
   __lower_bound__H4ZPCfZf…              ClassKey__Q36Attrib3Gen14trafficpattern
   clear__…ZP11IAttachable…              Get__CQ26Attribt7TAttrib1ZfUi
   find__H2ZPP7IPlayer…                  Get__CQ26Attribt7TAttrib1Z20Traffic…
```

No hay símbolo dominante. Es trabajo de tercera capa, unidad por primer uso.

### b) `.rodata −240`: es el orden del pool de literales, no contenido

`dolrod.py zAI .rodata` enseña `FALTA`/`SOBRA` alternándose **desde el byte 156** de la
ventana: los `$LC` están barajados, no desplazados. Censo de símbolos con nombre: de los 1.313
que «sólo tiene el objetivo», **todos** son `lbl_*`/`pad_*` del troceador o `base_pos.NNNNN`
(renombre del contador de declaración), y ninguno cambia de tamaño. **El único símbolo de
verdad de más es `_vt.14AIGoalPullOver`, 48 B**, una vtable que el objetivo no emite — huele a
`nfsmw-la-virtual-que-era-pura`. Frente propio, no barato.

### c) `.data −384`: **frente cerrado, no es trabajo**

El encargo lo señalaba como «lo único que sí es contenido». **Lo contradigo con una medida que
ya estaba en el árbol**: la nota de la r52 (`nfsmw-huecos-leer-el-dol`) midió justo esto en
siete unidades, `zAI` incluida — de 2.568 B de `.data` «ausente», 844 son renombres del
contador de declaración, 1.724 son nombres del troceador (`pad_`/`gap_`/`lbl_`) casi todos a
CERO, o sea **relleno de alineación que no se escribe, se consigue**, y **0 B son dato con
nombre sin escribir**. Los catorce bloques anónimos que lista `r55-med-zAI.md`
(`pad_06_80415180_data` 172 B, `lbl_80415438` 68 B, `gap_06_80415230_data` 44 B…) son de esa
familia. Y la regla que dejó esa nota vale aquí entera: **antes de diagnosticar `.data`, cierra
el `.rodata`** — parte del déficit desaparece sola, porque es la sombra de los punteros a
literales descolocados.

## 4. Sorpresas

1. **`dolwhere` no se puede usar en `zAI`.** Aborta antes de comparar nada
   (`LAS SECCIONES NO COINCIDEN`) precisamente en el caso para el que hace falta: una unidad
   con `.rodata`/`.data` de otro tamaño. `r55-med-zAI.md` lo citaba con cifras; hoy no arranca.
2. **`permorden` no mide esta clase de trabajo.** Con las dos ediciones del encargo dio
   **861 de 1030 antes y 861 después** mientras los bytes en su sitio subían 83.444: cuenta
   SÍMBOLOS, y una subsecuencia creciente es ciega al tamaño. Quien juzgue una edición de orden
   por `permorden` la dará por inútil.
3. **El censo por offsets del OBJETO engaña, y por mucho.** Decía +26.988 B donde el enlace
   dice +83.444. La diferencia son los ~114 símbolos que sólo emitimos nosotros: en el objeto
   empujan a todo el mundo y en el enlace `-strip-unused-data` se los lleva. **Hay que medir
   sobre el ENLACE**, como avisa `linkdelta` en su propio docstring.
4. **`zAI` es UNA sola TU y su cola empieza en `0x8003A9F4`, no en `0x800388C0`.**
   `r55-med-zAI.md` daba por «cola: churn» todo lo posterior a `0x800388C0`, y ahí todavía
   quedaban tres ficheros de parseo (`AISpawnManager.cpp`, `AIMath.cpp`, `Gps.cpp`). De esa
   confusión salieron los 6.952 B de las ediciones (3)(4)(5), que estaban a la vista y nadie
   había mirado. **Antes de dar un residuo por difuso, hay que localizar la frontera de la
   cola**: es el último símbolo del último `.cpp` de la lista de includes.
5. **Las tres ediciones nuevas las llegué a escribir, revertir y reescribir.** A mitad de la
   verificación el entorno dejó de permitirme ejecutar `build_direct.py` durante un rato; sin
   `fncmp` de después no hay red de seguridad, así que las revertí con `git checkout` antes que
   dejar media edición sin medir (regla 10). Cuando volvió, las reapliqué y las medí. Merece la
   pena decirlo porque la alternativa —dejarlas puestas y anotarlas como «propuesta»— habría
   bajado unidades que no son mías sin que nadie lo viera hasta la ronda siguiente.

## 5. Siguiente paso, en orden de rentabilidad

1. **`WRoadElem.h:97-98` — la edición fiel de `GetBits`/`GetBitsSigned`, y NO la poseo.** El
   sitio del original es el cuerpo dentro de la clase, donde ya hay un `// TODO inline`. Mi
   `inline` en `AIVehicle.cpp` los mete en la cola —que era el objetivo— pero los deja a
   `+14.680` del sitio exacto. Vale medir si la cabecera los coloca mejor; **es cabecera
   compartida y hay que dársela a quien la posea**. Coste: una compilación.
2. **El orden de la cola** (35.784 B): trabajo de primer uso sobre `find<T>`, `clear<T>`,
   `reserve<T>` y `TAttrib<T>::Get`. Sin dominante; es la tercera capa.
3. **`.rodata`**: quitar `_vt.14AIGoalPullOver` (48 B) y mirar si a `AIGoalPullOver` le falta
   una virtual no-pura sin definir; después, el orden del pool con `lcfix.py`/`lcpool.py`.
4. **`.data`: no tocar** hasta que `.rodata` esté cerrado. Ver §3c.

## 6. Propuesta de herramienta (no aplicada, no está en `scripts/`)

El censo de desplazamiento enlazado por símbolo —el de la tabla de §2— no existe en el árbol:
`linkdelta` da secciones, `permorden` da símbolos sin peso y `dolwhere` no arranca cuando las
secciones difieren, que es justo cuando hace falta. Lo tuve como sonda en el scratchpad. Si al
jefe le parece, es candidato a `scripts/` **como herramienta general, no como sonda de un solo
uso**: enlaza dos veces y agrupa por desplazamiento y por tramos, y encuentra la frontera de la
cola sola.

Las tres sondas que usé quedan en el scratchpad de mi sesión, por si se quieren mirar antes de
decidir (no las he metido en `scripts/`):

    …/scratchpad/zai_linkcenso.py   censo de desplazamiento ENLAZADO por simbolo + transiciones
    …/scratchpad/zai_censo.py       lo mismo pero por offsets del objeto (INFLA: ver Sorpresa 3)
    …/scratchpad/zai_seq.py         las dos secuencias de simbolos lado a lado en una ventana
