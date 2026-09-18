# Ronda 29 — `.rodata`: el frente existe, pero es 94 % DATO MUERTO

Encargo: los **32.472 B de `.rodata` que faltan** en las 32 SourceLists.
Resultado: **11.264 B cerrados** (32.472 → **21.208**), con `.text` **byte a
byte idéntico** en las 32 unidades; y la naturaleza del resto, medida.

---

## 0. Las cinco frases

1. **De lo que el objetivo tiene en `.rodata` y nosotros no, el 94 % NO LO
   REFERENCIA NADIE en todo el juego** (51.064 B muertos contra 3.319 vivos).
   No se deduce del código: hay que escribirlo como literal muerto.
2. **Y el 82 % de ese dato muerto son CADENAS** (44.768 B en 1.327 distintas),
   de las cuales **22.949 B se repiten en 5 o más unidades**: son literales de
   CABECERA, uno por TU. Eso los hace arreglables **en un solo sitio**.
3. **La forma de fuente que interna un literal muerto sin emitir código está
   medida** (banco de 16 variantes): `static inline`/`inline`/miembro inline que
   devuelve la cadena, `const char *n = "..."` sin usar, y **`if (CONSTANTE
   FALSA) f("...")`**. El **ternario `cond ? "..." : NULL` NO la interna**
   (`fold` lo pliega antes de `output_constant_def`), y **una constante de punto
   flotante muerta NO se puede internar de ninguna forma**.
4. **La causa raíz de 11.264 de esos bytes era un `#ifdef MILESTONE_OPT` en
   `AttribSys.h`** que tira el nombre de clase a la basura, más 59 cabeceras
   generadas que pasan el nombre corto (`"acceltrans"`) donde el original tiene
   el cualificado (`"Attrib::Gen::acceltrans"`, ×20 TU en el ELF). Corregido:
   **+11.264 B de `.rodata` correcta y CERO instrucciones cambiadas.**
5. **Ninguna de las 32 SourceLists puede promocionar hoy**, y está medido con el
   enlace real: **15 no enlazan** y **17 dan DOL ROTO**. `zFoundation`, el «caso
   limpio» del encargo, es de las que **no enlazan**: referencia 18 constantes
   `lbl_*` que no emite y que hoy le presta su propio objeto extraído.

---

## 1. La medida que cambia el frente: VIVO contra MUERTO

`dtk` pone un símbolo (`lbl_XXXXXXXX`, o el nombre real) en **cada dirección a
la que apunta algo en TODO el juego**, y `pad_`/`gap_` en lo que no referencia
nadie. Con eso se parte la `.rodata` extraída en piezas vivas y muertas.

Y el reparto es válido porque **cada constante que el código LEE tiene su propia
reubicación**: GCC 2.95 con `-fforce-addr` emite `lis rX,LC@ha` + `lfs f,LC@l(rX)`
por constante, no una base con desplazamientos. Comprobado en `rsincos`,
`BuildRotate` y `Evaluate__11UBezierLite`.

    32 SourceLists, .rodata que el objetivo tiene y nosotros NO:

        VIVO   (lo referencia algo)        3.319 B      6 %
        MUERTO (no lo referencia nadie)   51.064 B     94 %

Y de ese muerto, **44.768 B son cadenas C** y ~9.600 B son pool de flotantes y
relleno.

En `zFoundation`, el caso que el encargo llamaba limpio: de sus 2.328 B de
`.rodata`, **1.339 son muertos**; nosotros emitimos 1.008 con 951 vivos. Sus 188
funciones **existen todas** en nuestro objeto, **186 son byte a byte idénticas**,
y sus **150 parejas de reubicación a dato apuntan a la misma constante**: no hay
un solo byte vivo que discrepe. Lo que falta es literalmente basura del
compilador original.

## 2. Qué construcción interna un literal muerto — banco medido

`c29ro/t1.cpp` y `t4.cpp`, compilados con los `cflags` reales de `zFoundation`:

| forma | ¿interna la cadena? | ¿emite código? |
|---|---|---|
| `static inline const char *f() { return "X"; }` sin usar | **SÍ** | no |
| `inline const char *f()` sin usar | **SÍ** | no |
| miembro `static` inline de `struct` | **SÍ** | no |
| `const char *n = "X";` local sin usar | **SÍ** | no |
| `if (0) f("X");` dentro de función viva | **SÍ** | no |
| **`if (CONST_FALSA) f("X"); else f(NULL);`** | **SÍ** | **no** |
| `static const char *n = "X";` local | SÍ | +4 B en `.data` |
| `static const char *p = "X";` de fichero, sin usar | SÍ | +4 B en `.data` |
| **`COND_FALSA ? "X" : NULL`** | **NO** | — |
| `static const char a[] = "X";` sin usar | NO | — |
| argumento por defecto `f(const char* = "X")` | NO | — |
| plantilla sin instanciar | NO | — |
| `sizeof("X")` | NO | — |
| `static const char *f()` **no inline** sin llamantes | SÍ | **sí, la función** |
| **cualquier forma con una constante FLOTANTE muerta** | **NO** | — |

**La última fila es el techo del frente**: `c29ro/t2.cpp` prueba las seis formas
con `float`/`double` y la `.rodata` sale **vacía**. Una constante sólo entra en el
pool por `force_const_mem` durante el RTL de una función que se emite; si el
objetivo tiene un flotante muerto es porque su fuente tenía una expresión que se
optimizó **después** de internarla. Eso no se escribe: se reconstruye la fuente.

## 3. Lo que se ha ESCRITO — 11.264 B, y por qué era un fallo de cabecera

### 3.1 El diagnóstico

Comparando el multiconjunto de cadenas de cada `.o` nuestro contra el extraído
(`c29ro_cadenas.py`), y cada cadena contra el ELF original entero:

    cadenas que el OBJETIVO tiene y nosotros NO   2.278 en 37.696 B
    cadenas que NOSOTROS emitimos y que NO EXISTEN
      EN NINGUNA PARTE del ELF original              96 en  5.207 B

Las primeras, por número de unidades: `d:/mw/speed/indep/bware/inc/bware.hpp`
(24 ud), `%f,%f,%f` (24), `%f,%f,%f,%f` (24), `Attrib::Definition` (21),
`Attrib::TypeDesc` (21), `Attrib::Blob` (21), `Attrib::RefSpec` (20),
`Attrib::Gen::pursuitescalation` (19), `Attrib::Gen::aivehicle` (19)…

Las inventadas: **`Attrib::CollectionHashMap` (24 ud, 624 B — 0 apariciones en
el ELF original)**, `RefSpec` (24 ud, 192 B — 0 apariciones), `TODO` (21 ud).

Y el bloque de cabecera es **idéntico y en la MISMA posición en 31 de las 32**
unidades: `GAMECUBE | d:/mw/…/bware.hpp | bad_alloc | %f,%f,%f | %f,%f,%f,%f |
STL`, siempre al **principio** de la `.rodata`. Es el patrón `"%s  %s"` de la
r28 a escala.

### 3.2 La causa: `#ifdef MILESTONE_OPT` en `USE_ATTRIB_ALLOC`

`AttribSys.h` tenía la macro partida en dos:

```cpp
#ifdef MILESTONE_OPT
#define USE_ATTRIB_ALLOC(name)  ... Attrib::Alloc(bytes, name) ...
#else
#define USE_ATTRIB_ALLOC(name)  ... Attrib::Alloc(bytes, NULL) ...   // name SE PIERDE
#endif
```

`MILESTONE_OPT` **no está definido en `cflags_game` de GOWE69**, así que el
nombre no se nombraba nunca y la cadena no se emitía. Con el `#ifdef` había
además un `// TODO maybe the name is always passed here … Depends on whether
doing this leaves the string in .rodata`. **La respuesta es sí, y está medida.**

La cura es el idiom que el propio árbol ya usa (`Joylog.cpp:12`,
`int EnableJoylog = ENABLE_IN_MILESTONE;`): `ENABLE_IN_MILESTONE` es un **`bool`
constante** de `bWare.hpp`, no una condición de preprocesador. Una macro única
con `if (ENABLE_IN_MILESTONE) …; else …;` deja el literal internado y el `if`
muere sin emitir una instrucción.

**Y `-DMILESTONE_OPT` de verdad está VETADO, con cifra:** sobre `zAttribSys`
**rompe 30 funciones y arregla 0** (`c29ro_regres.py`), y `.text` pasa de 48.932
a 49.272 B. El objetivo pasa `NULL` en tiempo de ejecución y **aun así lleva la
cadena**: la bandera no es la respuesta, el `if` muerto sí.

### 3.3 Los cuatro cambios aplicados

| # | cambio | efecto |
|---|---|---|
| c1 | `AttribSys.h:1007` `Free(ptr, bytes, "RefSpec")` → `"Attrib::RefSpec"` | `RefSpec` pelado **no existe** en el ELF; `Attrib::RefSpec` sale 26 veces |
| c2 | `USE_ATTRIB_ALLOC`: una sola macro con `if (ENABLE_IN_MILESTONE)`; y comillas en los dos sitios que pasaban el tipo **sin comillas** (`AttribSys.h:123` y `AttribPrivate.h:265`, que con `-DMILESTONE_OPT` ni compilaban) | +40 B en `zAttribSys`, `.text` sha1 **idéntico** |
| c2b | 59 cabeceras de `Src/Generated/AttribSys/Classes/*.h`: `USE_ATTRIB_ALLOC("acceltrans")` → `("Attrib::Gen::acceltrans")` | **+13.080 B** en 24 unidades. Verificado una a una contra el ELF: las 59 cualificadas existen, y las 20 apariciones de `aivehicle\0` son **el sufijo** de `Attrib::Gen::aivehicle\0` — el corto no está nunca |
| c3 | `USE_ATTRIB_ALLOC("Attrib::Definition")` y `("Attrib::Blob")` en las dos clases que no lo tenían | +896 B |

**Resultado medido** (`build_direct.py` de las 32 antes y después, `seccdiff.py`):

| | antes | ahora |
|---|---|---|
| `.rodata` que **FALTA** | **32.472 B** | **21.208 B** |
| `.rodata` que sobra | 11.400 B | 14.552 B |
| neto | −21.072 | **−6.656** |
| cadenas que faltan | 37.696 B | **29.053 B** |

**Coste en código: CERO.** `.text` sha1 byte a byte idéntico en las 32 unidades
para c1+c2+c2b. En c3, `zPhysics` cambia de sha1 **sin cambiar de tamaño**, y
`c29ro_regres.py` sobre las 718 funciones del objetivo da **ROMPE 0, ARREGLA 0**
(692 casan antes y después). Y las cadenas nuevas nacen **muertas**, como en el
objetivo: 0 de las 28 nuevas de `zAI` y 0 de las 30 de `zFe2` tienen una
reubicación desde `.text`.

**Control del DOL: OK** (`9619ba57…`) antes y después, con el *rollback* del §6.

### 3.4 Lo que el cambio deja peor, y hay que decirlo

En siete unidades que ya emitían `.rodata` **de más** (`zEcstasy`, `zMisc`,
`zRender`, `zMiscSmall`, `zSim`, `zPhysics`, `zFeOverlay`) las cadenas nuevas
**engordan el exceso**: el bucket «emitimos de más pero SÍ existe en el original»
pasa de 10.707 a 15.725 B. No es dato inventado —las cadenas existen en el
ELF—, es que **nuestro unity build de esas unidades incluye cabeceras `Gen` que
el TU original no incluía**. Es un fallo de composición de SourceList que estaba
tapado y ahora se ve; queda como frente.

## 4. Ninguna SourceList promociona, y por qué — barrido de las 32

Enlace real del DOL completo, sustituyendo el objeto extraído por el nuestro,
una unidad por enlace:

| resultado | unidades |
|---|---|
| **ENLACE FALLA** (referencias sin dueño) | 15: zAI, zCamera, zEAXSound, zEAXSound2, zEagl4Anim, zFe, zFe2, zFeOverlay, **zFoundation**, zGameplay, zMisc, zPhysics, zSim, zSpeech, zTrack, zWorld |
| **DOL ROTO** | 17: zAnim, zAttribSys, zBWare, zDebug, **zDynamics**, zEcstasy, zFEng, zGameModes, zLua, zMain, zMiscSmall, zMission, zPhysicsBehaviors, zPlatform, zRender, zWorld2 |
| DOL OK | **ninguna** |

**El fallo de enlace se predice sin enlazar** con `c29ro_prestado.py`: son las
unidades que referencian símbolos del troceador (`lbl_`, `gap_`, `pad_`) **que
no definen** y que hoy les presta su propio objeto extraído. Hay **39** en seis
unidades: `zFoundation` 18, `zPhysics` 11, `zWorld` 5, `zGameplay` 3, `zCamera`
1, `zSim` 1. `zFoundation` falla exactamente por `lbl_803EB444`, que es lo que
el barrido dice.

`-strip-unused-data` **no salva** el exceso de datos: `zDynamics` tiene su
`.rodata` +296 B de `$LC` no referenciado y aun así da **DOL ROTO**. Lo que la
r28 midió para `.text` (los cuerpos weak duplicados se tiran) **no vale para
`.rodata`**.

### 4.1 `zDynamics`, la promoción «más barata», medida

No lo es. Su `.rodata` de hoy son **tres bloques `asm()` escritos a mano** en
`zDynamics.cpp` (436 B de `.rodata`, `algos.3467` de 36 B y un `gap` de 0x28 en
`.data`) **más** el pool `$LC` que el compilador emite igual: 840 contra 544.
Compilada **sin** los `asm` (a scratchpad, sin tocar el árbol):

    .text 28.976 = 28.976   ·   .bss 16 = 16   ·   .data 4 vs 44
    .rodata 368 vs 544  ->  faltan 104 B: 92 de cadenas muertas de cabecera,
                            12 de pool de flotantes muerto, y `algos.3467`

Los 92 B de cadenas son escribibles; **los 12 B de flotantes muertos no**, y
`algos.3467` es una tabla estática local que sí se puede escribir en C. O sea:
`zDynamics` **no se cierra sin ensamblador** con lo que sabemos hoy. Lo mismo
vale para `zFoundation`, que tiene ~470 B de pool de flotantes muerto.

## 5. Herramientas nuevas (scratchpad, prefijo `c29ro_`)

| script | qué hace |
|---|---|
| **`c29ro_faltan.py`** | **el simétrico de `extrasym.py`**: qué dato tiene el objetivo en el rango de una unidad que no emitimos, partido en **VIVO** (lo referencia algo) y **MUERTO**, y decodificado como cadena si lo es. `--sec`, `-v`, `--str` |
| **`c29ro_cadenas.py`** | diferencia de **cadenas** contra el extraído, y **contra el ELF original entero**: separa «falta» de «INVENTADA» (no existe en ninguna parte). `--orden <unidad>` alinea las dos listas para ver el desajuste de una cabecera |
| **`c29ro_prestado.py`** | las referencias a símbolos del troceador que no definimos: **predice el `ENLACE FALLA` de una promoción sin enlazar** |
| **`c29ro_pares.py`** | empareja reubicación a reubicación entre las funciones idénticas y compara **la constante apuntada**. Prueba que en `zFoundation` el dato **vivo** ya casa entero |
| **`c29ro_regres.py`** | cuántas funciones que casan deja de casar un cambio (el `c28lo_regres` de la r28, con la neutralización arreglada) |
| **`c29ro_link.py`** | `trypromo.py` con `--rollback` (devolver una unidad promocionada al objeto extraído) y `--obj U=fich.o`. **Sin `--rollback` esta ronda no se puede enlazar nada** (§6) |
| `c29ro_vivo.py` · `c29ro_clase.py` · `c29ro_cabecera.py` · `c29ro_snap.py` · `c29ro_gen.py` · `c29ro_strip.py` | vivo/muerto por unidad; cadena contra pool; el bloque de cabecera unidad a unidad; tamaño+sha1 por sección (el A/B de cabecera compartida); el renombrado verificado de las 59; quitar los `asm()` de un fichero |
| `c29ro/t1..t4.cpp` | el banco de formas del §2 |

**Trampa de la propia herramienta:** `neutral()` tiene que tapar el
desplazamiento de **toda** rama en los dos lados, no sólo donde hay reubicación:
el objeto extraído **no lleva reubicaciones para los saltos internos** y el
nuestro sí (`R_PPC_REL14`/`REL24` con símbolo vacío). Sin eso, `zFoundation`
daba 99 funciones «distintas» de 188 cuando difieren **2**.

## 6. Convivencia: el enlace del árbol está ROTO por otro agente

Con la lista de objetos tal cual, **el enlace de control falla**:

    postproc.c(1) : error: L0039: Reference to undefined symbol lbl_80411458

`lbl_80411458` y `lbl_80411460` caen en el rango de
`egami/rcmp/dev/source/decoder/cmn/maddeca.cpp` (0x80411358..0x804114A8), que
está **modificada y sin commitear** por otro agente y ya no los emite. Con
`c29ro_link.py --rollback egami/…/maddeca` el control da **DOL OK**.

**Esto envenena `trypromo.py`**: da `ENLACE FALLA` para *cualquier* unidad que se
pruebe, y el mensaje culpa a `postproc`. Todo el §4 está medido con el rollback.

Y al revés: el `.text` de `zPhysics` cambió (−8 B) entre dos de mis medidas por
el `#pragma interface` que otro agente añadió a
`Libs/allocator/1.5.0/Allocator/iallocator.h` a las 12:49, no por mi cambio.

## 7. Lo que NO he probado

1. **Escribir el bloque de cabecera** `GAMECUBE / bware.hpp / %f,%f,%f /
   %f,%f,%f,%f / STL` (92 B × 31 unidades ≈ 2.850 B). Sé **dónde** va (siempre el
   primero) y **con qué forma** (§2), pero no de qué construcción original sale;
   ponerlo a mano sería la misma deuda que el `"%s  %s"` de la r28 y no lo he
   hecho.
2. **`Attrib::CollectionHashMap`**: sé que **no existe** en el original (624 B de
   crédito falso en 24 unidades) pero **no sé qué nombre pasaba** el original en
   `TableAllocFunc`/`TableFreeFunc`. No lo he tocado.
3. **Las 96 cadenas inventadas restantes (5.037 B)**, entre ellas `TODO` (21 ud)
   y una docena de rutas `src/Speed/Indep/Src/...cpp` donde el original tiene
   rutas `d:/mw/...`.
4. **El frente de `.data`, que es el bueno que queda**: faltan **7.584 B** y de
   ellos **4.190 B están VIVOS** (dos tercios, al revés que en `.rodata`).
   Concentrados en `zSpeech` 1.216, `zFe` 1.196, `zFeOverlay` 680, `zFe2` 486.
   Son tablas y globales inicializadas: **eso sí se escribe en C**. No lo he
   tocado.
5. **Las siete unidades que ahora sobrepasan** (§3.4): no he mirado qué cabecera
   `Gen` incluye de más cada SourceList.
6. **Retirar los `asm()` de datos de `zDynamics` y `zOnline`.** Están medidos
   (§4.1) y son la única vía conocida hoy para una `.rodata` exacta, pero mi
   encargo prohíbe ensamblador y no me toca decidir si se quitan.
7. **Reconstruir el pool de flotantes muerto.** Es lo que separa a `zDynamics`
   (12 B) y a `zFoundation` (~470 B) de una `.rodata` exacta, y **no hay forma de
   fuente que lo interne**: haría falta la expresión original que lo creó antes
   de que la optimización la borrara.
8. **`configure.py`, `config/GOWE69/*` y `splits.txt`: no he tocado nada.** Y la
   única propuesta que salió por el camino, `-DMILESTONE_OPT` en `cflags_game`,
   **la retiro yo mismo**: rompe 30 funciones en `zAttribSys` y arregla 0.
