# r35 · grupo `att` — las cuatro SourceLists más cercanas

## Verificación del encargo (antes de tocar nada)

`promodist.py` reproduce las cuatro filas **exactas** del encargo, y `datacmp.py`
las confirma sección a sección:

| unidad | desbloquea | fns | `datacmp` de partida |
|---|---|---|---|
| zFEng | 71.460 B | 0 | `.rodata` 1.984/2.632 (36,8 %, 1ª dif +0x98) · `.data` 308/384 |
| zAttribSys | 48.776 B | 0 | `.rodata` 760/1.176 (13,3 %, 1ª dif +0x5C) · `.data` 48/96 |
| zAnim | 42.292 B | 0 | `.rodata` 4.976/5.688 · `.data` 920/1.048 · `.bss` +12 |
| zFoundation | 36.000 B | 0 | `.rodata` 1.184/2.328 · `.data` 48/248 |

(El encabezado de `promodist.py` ya dice 63 unidades / 25 con `.text` perfecto, no
las 69 / 31 del brief: otros agentes han promocionado por el camino.)

---

## 0. El hallazgo de la ronda: **el `.text` perfecto no es el blanco; el NOMBRE del dato muerto sí**

Las 25 unidades con `.text` perfecto no promocionan por dos cosas distintas, y
hasta ahora se estaban midiendo juntas:

1. **Dato que falta de verdad.**
2. **Daño de estripado**, que es nuevo y es enorme: `keep.lst` fuerza los símbolos
   **con el nombre que les da `dtk` en el objeto EXTRAÍDO** (`zAttribSys.o:$LC2151483872`).
   Nuestro objeto llama a esa misma cadena `$LC60`, la entrada de `keep.lst` no
   casa con nada, y **`-strip-unused-data` se lleva `size & ~7` de cada cadena
   muerta**. En el DOL enlazado se ve literalmente: `"Attrib::Attribute"` (18 B)
   queda en `"e\0"`, `"Attrib::Instance"` (17 B) en `"\0"`.

`keepchk.py` **no lo ve**: valida las entradas contra el objeto que está HOY en el
enlace, que para una unidad sin promocionar es el extraído.

### El mapa, medido unidad a unidad

Enlace completo × 3 por unidad (base · con nuestro `.o` · con nuestro `.o` y un
`keep.lst` que nombra NUESTROS símbolos). Herramienta:
`scripts/stripmap.py`. Delta en bytes del DOL:

| unidad | sustituyendo | + keep ampliado | **daño de estripado** | dato que falta |
|---|---|---|---|---|
| zEcstasy | −672 | **+5.664** | −6.336 | sobran 5.664 |
| zMisc | −2.208 | **+4.608** | −6.816 | sobran 4.608 |
| zPhysics | −1.568 | **+4.576** | −6.144 | sobran 4.576 |
| zFe | −3.200 | −352 | −2.848 | 352 |
| zEAXSound | −2.560 | −64 | −2.496 | 64 |
| zSpeech | −1.536 | +896 | −2.432 | sobran 896 |
| zMain | −3.008 | −800 | −2.208 | 800 |
| zRender | −768 | +992 | −1.760 | sobran 992 |
| zWorld | −4.832 | −3.072 | −1.760 | 3.072 |
| zFe2 | −4.992 | −3.296 | −1.696 | 3.296 |
| zEAXSound2 | −2.048 | −416 | −1.632 | 416 |
| zSim | −1.792 | −224 | −1.568 | 224 |
| zCamera | −2.080 | −544 | −1.536 | 544 |
| zPhysicsBehaviors | −1.088 | +320 | −1.408 | sobran 320 |
| zLua | −2.336 | −1.280 | −1.056 | 1.280 |
| zAnim | −1.728 | −800 | −928 | 800 |
| zWorld2 | −1.504 | −608 | −896 | 608 |
| zPlatform | −1.984 | −1.184 | −800 | 1.184 |
| zTrack | −2.528 | −1.728 | −800 | 1.728 |
| zMiscSmall | +128 | +480 | −352 | sobran 480 |
| zGameModes | +128 | +448 | −320 | sobran 448 |
| zEagl4Anim | −928 | −704 | −224 | 704 |
| zFoundation | −1.344 | −1.312 | −32 | 1.312 |
| **zFEng** | **−704** | **−704** | **0** | **704 — todo dato ausente** |
| **zAttribSys** | **0** | **0** | **0** | **cerrado, ver §2** |

(`zAI`, `zFeOverlay` y `zGameplay` ni siquiera enlazan; `zBWare` falla con el
`keep` ampliado.)

**Lectura**: en 19 de 25 el estripado pesa más que el dato, y en tres
(`zEcstasy`, `zMisc`, `zPhysics`) es de 6 kB y esconde que a la unidad le SOBRAN
4-5 kB. La cuenta «rodata−N» del objeto **no predice** el delta del DOL: en
`zAttribSys` eran 464 B de objeto y 704 B de DOL.

---

## 1. Cómo se controla el pool de cadenas de cc1plus: una sola función

Antes de nada, el mecanismo que lo hace posible, y que no estaba escrito:

> **GCC 2.9 emite el literal de una función que NO emite.** Prueba mínima ya en el
> árbol: `bGetPlatformName()` (`bWare.hpp:166`) es `inline`, nadie la llama, no
> aparece en el `.text` de `zAttribSys` **ni hay una sola reubicación que la
> referencie**, y sin embargo `"GAMECUBE"` está en su `.rodata`.

De ahí sale la palanca. **Una función `inline` sin usar que nombre las cadenas en
el orden del objetivo reordena y RELLENA el pool entero, y no cuesta un byte de
código.** Es el truco de `_AttribAllocTagOrder` (r33) llevado al límite:

    inline const char *_zAttribSysPoolOrder(int i) {
        switch (i) {
        case 0: return "";
        case 1: return "Attrib::Attribute";
        ...
        case 40: return "AttribSys.gamecube";
        }
        return 0;
    }

En `zAttribSys`, colocada **delante de todos los `#include`**, esa única función
llevó la `.rodata` de **760 B / 13,3 % idéntica** a **1.192 B / 87,4 %**, y movió
la primera diferencia de `+0x5C` a `+0x352`. Un solo ensayo.

Aviso de escritura: el `case` del formato `"%s\%s\%s\%s\%d\%s\[]:"` necesita
`\\`; con una sola barra salió `%s%s%s%s%d%s[]:` y faltaban 8 B.

**Limitación medida**: vale para CADENAS, no para constantes numéricas. El pool de
flotantes se emite en RTL, función por función; una función no emitida no lo
emite. Un `0x4330000000000000` que falta es una **sentencia** que falta.

### Y el pool tiene DOS vueltas

`STL`, `bad_alloc` y `GAMECUBE` volvieron a internarse **después** del bloque, en
`+0x374`-`+0x394`: cc1plus **vacía la tabla de constantes** entre la pasada
principal y la diferida de `finish_file`, así que el mismo literal usado por una
plantilla instanciada tarde sale **otra vez**. Eso explica también que el objetivo
tenga `"STL"` dos veces (`+0x58` y `+0x23C`).

---

## 2. `zAttribSys`: dato CERRADO, `.rodata` y `.data` byte a byte

    .bss     40 / 40        IGUAL
    .data    96 / 96       100,0 % idéntica
    .rodata  1176 / 1176   100,0 % idéntica
    measure  48.776/48.776 B  100,0000 %  196 funciones al 100 %

Y lo que importa, con el `keep.lst` **de serie, sin tocar `config/`**:

    stripmap.py zAttribSys ->  test 0   +keep 0   estripado 0

o sea que el DOL sale con la **longitud exacta**. La forma final (e4) no es la de
la §1: la función de orden se retiró y **el pool va escrito a mano con los nombres
de `dtk`**, que es lo único que sobrevive al estripado.

**La receta, que es reutilizable tal cual:**

1. **Todo el bloque de cadenas muertas, a mano**, en el `asm()` que ya abría el
   fichero, con los nombres exactos del objeto extraído
   (`scripts/genrodata.py <unidad> <lo> <hi>` los genera solo, y comprueba
   que el rango no tiene huecos). Son las 41 cadenas de `+0x5C` a `+0x380`; 38 de
   ellas están muertas **también en el objetivo** (contado por reubicaciones: el
   objetivo sólo referencia `""`, `"STL"` y `"%s_%08x"`).
2. **Que cc1plus no interne NINGUNA de esas cadenas en esa TU**, o emite su copia
   detrás y desplaza las vtables. Interruptor `ZATTRIBSYS_HAND_POOL`, definido
   **sólo** en `SourceLists/zAttribSys.cpp`, y `#define ATTRIB_TAG(s) NULL` en
   `AttribSys.h`. **No cuesta una instrucción**: el nombre se descarta dentro de
   `AttribAlloc`, y por eso el `.text` sigue al 100 % con 196 funciones.
3. **Las tres cadenas VIVAS se redirigen a la copia escrita a mano**, con el
   renombrado `asm()` de GCC — así el código sigue apuntando a la dirección del
   objetivo y no nace un literal nuevo:

       extern const char _zAttribSysSTL[]  asm("$LC2151484348");   // +0x23C
       extern const char _zAttribSysEmpty[] asm("$LC2151483868");  // +0x5C
       extern const char _zAttribSysGUKfmt[] asm("$LC2151484444"); // +0x29C

   Más `"GAMECUBE"` y `"bad_alloc"`, que en esa TU son muertas pero **son valores
   de retorno**: apuntan al prefijo de bWare, que ya estaba escrito a mano.
4. **Los 48 B de `.data`** son dos huecos muertos que `keep.lst` ya nombra
   (`gap_06_804158E4_data`, 4 B, y `gap_06_8041590C_data`, 44 B). El primero cae
   **entre dos `#include`** de la SourceList (detrás de `gByValueBytes` de
   `AttribCollection.cpp` y delante de `gDatabaseSelfDestruct` de
   `AttribDatabase.cpp`): un `asm()` de ámbito de fichero **entre los dos
   `#include`** lo pone en su sitio. El segundo va al final del fichero. Los dos
   cierran con `.previous`.

### Lo único que le queda: **el ORDEN del `.text`**

    DESCOLOCADAS: 43 de 197     SALTOS DE DELTA: 15

`textorder.py` no se había mirado en esta unidad: el informe de la r34 comprobó
que las 196 funciones comunes **miden** lo mismo, que no es lo mismo que estar en
su sitio. Enlazando con el `keep` ampliado, el DOL sale de la **longitud exacta** y
**todas** las diferencias que quedan (46 grupos, 9.808 B) son mitades `@l` de
`.text`: es el desorden y nada más. Los 15 saltos (∼8 causas) están en el volcado
de arriba; los tres racimos gordos son
`__lower_bound__…Definition…` (+1.472), `Transfer__Q26Attrib7HashMap` (+6.860) y
`_._Q26Attrib9TypeTable` (−204/+208).

Un salto **ya cayó de paso**: quitar el internado de `"Attrib::Attribute"` movió
el constructor/destructor de `Attribute` a su sitio (47 → 43 descolocadas,
18 → 15 saltos).

---

## 3. `zFEng` — el mejor candidato que queda, y por qué no cerró hoy

Es la única con **daño de estripado 0**: sus 704 B son dato ausente puro. Y
`textorder.py` dice **ORDEN PERFECTO, 0 de 344**. O sea que **escribir esos 704 B
la promociona**.

El desglose (`scripts/datadiff.py zFEng`) da 750 B que faltan y 102 que
sobran. Lo que falta son tres cosas:

- **Cadenas muertas** — el frente que la §1 resuelve: `"Scroll Left/Right/Up/Down"`,
  `"[HEADER]"`, `"Languages"`, `"[LABEL]"`, `"[PLATFORM]"`, `"[COMMENT]"`,
  `"[FLAGS]"`, `"[MAX_SIZE]"`, `"Major version: 3"`, `"Minor version: 3"`,
  `"Build number: 22"`, `"FEngine::Render()"`, `"Setup"`, `"Object traversal"`,
  `"SortObjects()"`, `"RenderObjectList()"`, `"FEngMalloc"`,
  `"FEngMalloc[somefile]"`, `"d:/mw/speed/indep/src/feng/FEPackage.cpp"`,
  `"Init"`, `"Hide"`, `"cmod"`, `"cset"`, `"cseta"`, `"cadd"`, `"wa"`, `"sr"`,
  `"sg"`, `"sb"`, `"r"`, `"g"`, `"b"`, `"a"`, `"w"`.
- **Constantes de pool** (`4330000000000000` ×3, varios `3f800000`,
  `3727c5ac`, `358637bd`…) — **esto NO lo da la función de orden**: son
  sentencias que faltan.
- **Dos arrays mal colocados**: `FETrackOffsets` (44 B) y `FEKeyTypeSize` (28 B)
  están al FINAL de nuestra `.rodata` (`+0x778`, `+0x7A4`) y el objetivo los tiene
  DENTRO del pool (`+0x428` y `+0x138`).

Y le sobran los mismos 28 B de siempre: `"bad_alloc"` y `"GAMECUBE"` reinternados,
que se quitan con el patrón de la §2.3.

**`zFoundation`** también tiene **ORDEN PERFECTO (0 de 189)** pero su hueco es de
1.312 B y está mucho más cargado de constantes numéricas (bloques enteros de
`ASSERTION FAILURE` y de flotantes), además de las 8 etiquetas `Attrib::*` que a
esa unidad le faltan.

**`zAnim`** tiene 27 de 316 descolocadas y 928 B de daño de estripado: es la más
lejana de las cuatro.

---

## 4. Qué queda tocado en el árbol (sin commitear)

Nada de esto rompe el enlace y **el radio de explosión es cero, medido**: el
`.ii` preprocesado de `zDynamics` (promocionada) antes y después de TODOS mis
cambios difiere en **un espacio** (`"STL" )` en vez de `"STL")`, expansión de
macro), y en el `.ii` de `zDynamics` y `zFe2` no aparece **ni una vez**
`_zAttribSys`, `ATTRIB_TAG` ni `ZATTRIBSYS_HAND_POOL`.

    src/Speed/Indep/SourceLists/zAttribSys.cpp        pool a mano + 2 huecos de .data + interruptor
    src/Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h            ATTRIB_TAG, 7 sitios
    src/Speed/Indep/Tools/AttribSys/Runtime/Common/AttribClass.cpp        3 sitios
    src/Speed/Indep/Tools/AttribSys/Runtime/Common/AttribCollection.cpp   3 sitios
    src/Speed/Indep/Tools/AttribSys/Runtime/Common/AttribDatabase.cpp     3 sitios (las 3 LoadData NO: son VIVAS)
    src/Speed/Indep/Tools/AttribSys/Runtime/Common/AttribLoadAndGo.cpp    5 sitios
    src/Speed/Indep/Tools/AttribSys/Runtime/Common/AttribInstance.cpp     "%s_%08x"
    src/Speed/Indep/Src/Misc/attribuserinclude.h      StringKey: la cadena vacia
    src/Speed/Indep/bWare/Inc/bWare.hpp               "GAMECUBE"   (CRLF: parcheado en binario)
    src/Speed/GameCube/bWare/GameCube/SN/include/new  "bad_alloc"
    src/Speed/Indep/Libs/Support/stlgc/stl/_alloc.h   _STLP_MEM_TAG

**Otro agente está en la misma unidad, y en la mitad que falta**: durante la ronda
aparecieron en el árbol `AttribSupport.cpp` y `Attribute.cpp` modificados por
alguien más, moviendo `~Attribute` de una a otra «entre el constructor de tres
argumentos y `operator=`» — es exactamente el frente del ORDEN del `.text` de la
§2. **No son míos y no los he tocado.** Las dos mitades encajan: con su orden y
mi dato, `zAttribSys` promociona. Verificado que su cambio no rompe el mío:
`measure` sigue dando 48.776/48.776 B al 100 % y `stripmap` sigue dando 0/0.

`config/`, `configure.py` y `splits.txt` **no se han tocado**, y no hace falta
tocarlos: la forma final de `zAttribSys` casa con el `keep.lst` de serie.

**Trampa a la que caí y que costó una hora**: `-keep` con un `keep.lst` ampliado
es un buen DIAGNÓSTICO pero no es la solución — la solución es que el objeto traiga
los nombres de `dtk`, y eso sólo lo da el dato escrito a mano.

---

## 5. Vedas y avisos

- **VEDA: la función `inline` de orden no sirve para constantes numéricas.**
  Barrida la sentencia «basta con nombrar la constante en una función sin usar»:
  el pool de flotantes se emite en RTL y una función no emitida no lo emite.
- **VEDA: ampliar `keep.lst` con nuestros nombres NO cierra una promoción por sí
  solo.** Probado en las 25: sólo tapa el estripado; el dato que falta sigue
  faltando, y el orden del `.text` sigue mandando.
- **`keepchk.py` da falsos negativos en unidades sin promocionar**: valida contra
  el objeto extraído. Para una unidad que vas a promocionar, hay que comprobar los
  nombres contra NUESTRO `.o`.
- **`textorder.py` antes de gastar un minuto en datos.** `promodist.py` con
  `fns=0` **no** quiere decir que la unidad esté a punto: `zAttribSys` tenía 47
  funciones descolocadas y nadie lo había mirado.
- **La base se rompió tres veces durante la ronda** (`LibSN/metrotrk` con
  `lbl_804AC6E0`/`lbl_804B4DE4`/`lbl_804AC6C0` indefinidos, y una ventana con
  `build\GOWE69\src\allocator.o` en la lista de enlace antes de existir). Congelar
  la lista de objetos **no** vale: se queda rancia en minutos y el enlace falla por
  otra cosa. Lo que vale es **releer `build.ninja` y comprobar que la base da
  `9619ba57` en la misma ejecución**.
- **La base PARPADEA mientras otro agente construye.** A las 01:49 el enlace daba
  `0a7cb069` y `build/GOWE69/main.dol` en disco era ese mismo hash; diez minutos
  después volvía a dar `9619ba57` sin que nadie tocara nada. Sustituir el objeto
  extraído de las cuatro SourceLists promocionadas y de `pro2D4` da `9619ba57` en
  los cinco casos, o sea que **ningún cambio mío toca una unidad promocionada**.
  Antes de acusar a tu cambio: relanza la base.
- **Escribir el `.rsp` del enlazador en modo texto en Windows lo rompe en
  silencio**: los CRLF dan un DOL del tamaño correcto y hash distinto, y parece un
  fallo tuyo. `open(rsp, 'w', newline='\n')`.

## 6. Herramientas nuevas — las tres útiles están en `scripts/`

| | |
|---|---|
| **`scripts/stripmap.py [unidades]`** | la tabla de la §0: **daño de estripado contra dato ausente**, por unidad. Tres enlaces por unidad; `+keep == 0` quiere decir que el dato ya está entero y sólo fallan los nombres |
| **`scripts/datadiff.py <u> [secc]`** | diff BYTE a BYTE **con resincronización**: `datacmp.py` dice cuánto falta, esto dice **qué** falta y en qué desplazamiento de cada lado. Es lo que convierte un `rodata−648` en una lista de cadenas concretas |
| **`scripts/genrodata.py <u> lo hi`** | genera el `asm()` de un rango de `.rodata` del objeto extraído **con los nombres de `dtk`**. Aborta si el rango tiene un hueco sin símbolo (límites mal elegidos) |

Se han borrado los temporales del scratchpad (`c35att_*`, `pp_*.ii`, `bak/`).
