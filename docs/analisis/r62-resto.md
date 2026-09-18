# r62 -- "resto": zAI, zLua, zSpeech, zFe, zFe2, zGameplay, zTrack, zPlatform

Las ocho de COLOCACION PURA. Ninguna promociona esta ronda. Lo que sale es:

* **-2.420 B de DOL en zTrack** (`dolwhere` 6.376 -> 3.956), en tres ediciones,
  **cero correcciones de `lcfix` pendientes** y **cero lineas de `config/`**.
* **Un mecanismo NUEVO de desajuste, medido, que ninguna herramienta del arbol
  veia**: un `gap_`/`pad_` escrito a mano con el TAMANO EQUIVOCADO desplaza
  TODA la seccion de las unidades que van detras, y ni `linkdelta` ni `seccdiff`
  lo detectan. En zTrack valia 2.260 B por si solo.
* **El mapa real de las ocho**, con dos cifras que no se habian tomado nunca:
  `dolwhere` y **simbolos globales desplazados en el ENLACE**.
* Cuatro negativos medidos (dos en zTrack, uno en zLua, uno de metodo).

---

## 1. El mapa: donde esta de verdad cada una

`dolwhere` = bytes del DOL que difieren al promocionar SOLO esa unidad.
`desplazados` = simbolos GLOBALES del ELF enlazado que cambian de direccion
(herramienta nueva, `scratchpad/resto62/symshift2.py`; enlaza la base una vez y
luego una vez por unidad).

| unidad | dolwhere | desplazados | lectura |
|---|---:|---:|---|
| **zTrack** | 6.376 -> **3.956** | 330 -> **6** | lo de esta ronda |
| **zLua** | 9.152 | **5** | **la mas cerca del proyecto entero** |
| zSpeech | 11.264 | 99 | vtables + cola de `finish_file` |
| zFe | 34.384 | 343 | |
| zGameplay | 35.438 | 181 | |
| zFe2 | 46.850 | 330 | |
| zPlatform | 95.754 | 178 | `.data`: `bin_globala_bun` (89.716 B) en el sitio equivocado |
| zAI | **244.740** | **2.642** | 168.173 B son `.data` permutada. Muy lejos |

Las ocho siguen en `linkdelta ... IGUAL` (verificado al empezar y al acabar).

**zLua es la joya del lote y estaba mal diagnosticada.** `symshift` dice que en
TODO el DOL le quedan **cinco** simbolos globales fuera de sitio: las dos
funciones que ya senalaba `textorder`, mas `luaO_nilobject` (-476) y
`luaP_opmodes` (-472). Sus 9.152 B se reparten **~8.000 B de ORDEN DE CADENAS
en `.rodata`** (68 de 446 fuera de sitio, `rodorden`) y ~930 B de `.text` que
son reubicaciones arrastradas por ese mismo pool. **A zLua no le falta
colocacion de codigo: le falta el pool.**

---

## 2. LO QUE VALE LA RONDA: el `gap_` con el tamano equivocado

### El hallazgo

`src/Speed/Indep/Src/World/EventManager.cpp:288` declaraba

    char gap_07_804A5DB8_bss[160];

y el objeto EXTRAIDO da ese mismo simbolo con **168 B** (`0xA8`). Consecuencia:
nuestra `.bss` enlazada de zTrack terminaba **8 B antes** que la del objetivo, y
**todo el `.bss` de las unidades que van detras salia a -8**. Primer simbolo
afectado: `hash.29913`, `0x804A5E60`, que es exactamente el final del rango de
zTrack en `splits.txt`.

Eso se pagaba en el DOL en forma de **945 bloques distintos de 1-2 B repartidos
por TODO el binario** (`Load__11CAnimPlayer`, `SNDebugBoot`, `ISIentry`,
`fn_803101DC`...): la mitad baja de cada `@l`/`@sda21` que apunta a un `.bss`
posterior. 2.260 B en total, y ni uno de ellos dentro de zTrack.

### Por que NINGUNA herramienta lo veia

* `linkdelta zTrack` -> **IGUAL**. Las secciones del ELF miden lo mismo.
* `seccdiff zTrack` -> **`.bss` +0**. Los dos `.o` miden `0xB8BC`.
* `gapchk zTrack` -> **total 0**.
* `stripmap zTrack` -> `-160 B`, que apunta a `.rodata`, no a esto.

La razon de la invisibilidad esta medida: nuestro `.o` tiene 8 B **de mas** en
`.bss` (`value` + `_.tmp_0`, dos estaticos MUERTOS de `ScreenEffects.cpp`) que
`-strip-unused-data` se lleva, y 8 B **de menos** en el `gap_` final. Los dos
errores se cancelan **en el tamano del objeto** y no se cancelan **en el
enlace**. Es el hermano de `memory/nfsmw-text-cero-dos-errores`, pero en `.bss`.

### Como se caza (receta general, vale para las 21 unidades)

**Diferenciando las TABLAS DE SIMBOLOS de los dos ELF ENLAZADOS**, no los `.o`:

    python scratchpad/resto62/symshift2.py <unidad> [<unidad>...]

Enlaza la base una vez y luego sustituye cada unidad; imprime los simbolos
GLOBALES que cambian de direccion, agrupados por delta. **Hay que filtrar los
`$LC`**: son locales y su nombre colisiona entre unidades, asi que un diff por
nombre inventa desplazamientos de 200.000 B (paso en el primer intento).

Y el detector barato, sin enlazar, que apunta al sitio:

    python scratchpad/resto62/gapcmp.py <unidad>...

compara seccion a seccion los simbolos del `.o` extraido y del nuestro, y canta
los `gap_`/`pad_`/`lbl_` **de distinto tamano**. Ahi salio `168 vs 160` en un
segundo.

### El barrido sobre mis ocho: solo habia UNO

`gapcmp` sobre las ocho da un unico `gap_` con el tamano mal (el de zTrack) y
tres falsos positivos que conviene dejar escritos para que nadie los persiga:

* **`zAI pad_06_80415180_data` obj 172 / nue 164 -- NO ES UN FALLO.** Los 8 B
  que faltan son `AITrafficManager::mTrafficMinSpawn` y `mTrafficMaxSpawn`, que
  nosotros emitimos **con nombre** y `dtk` metio dentro del hueco anonimo.
  Nuestra `.data` en ese punto es CORRECTA: `RandomSortTCDir` cae en `0xAC` en
  los dos. Subir el `pad` a 172 seria un error.
* **`zTrack lbl_80408FB8` obj 540 / nue 444** -- si era real, ver la seccion 3.
* **`zFe2 lbl_803E4380` obj 92 / nue 956** -- bloque escrito a mano alargado a
  proposito por una ronda anterior. No tocar sin medir.

---

## 3. zTrack: las tres ediciones, con su cifra

Base de la ronda: `dolwhere zTrack` = **6.376 B**, 945 bloques.
`.o` sellado al final (TRES compilaciones identicas):
**`63d67e5002f69625ddc6fa15d9f4d00a139888d2`**.

| # | edicion | dolwhere | delta |
|---|---|---:|---:|
| 0 | base | 6.376 | |
| 1 | `gap_07_804A5DB8_bss` 160 -> **168** (`World/EventManager.cpp:288`) | **4.116** | **-2.260** |
| 2 | doble escape del `.asciz` de `PrecullerBooBooScript` (`zTrack.cpp:250`) | **4.057** | **-59** |
| 3 | `lbl_80408FB8` 0x1BC -> **0x21C** + retirar 6 duplicados del bloque de cola | **3.956** | **-101** |

### Edicion 2 -- la doble capa de escapes (trampa nueva, y general)

En un `asm()` de C hay **DOS** pasadas de escape. La cadena de C llevaba una
barra invertida doble por cada separador de ruta, o sea que al ENSAMBLADOR le
llegaba UNA sola barra: `.asciz "..\..\World\Global\Scripts\..."`. Y NgcAs se
come `\.`, `\W`, `\G`, `\S` y `\P`. Emitiamos
`"....WorldGlobalScriptsPrecullerBooBooScript.hoo"`: **5 B mas corta** que el
objetivo y con el contenido mal. En la fuente de C hacen falta **CUATRO** barras
por separador.

`rodorden` no lo canta (la cadena existe, con otro contenido); se ve listando
las cadenas del `.o` una a una.

**Vale la pena barrer esto en todo el arbol**: cualquier `.asciz` dentro de un
`asm()` con una ruta de Windows tiene el mismo fallo.

### Edicion 3 -- el bloque de cabecera media 540 B, no 444

`lbl_80408FB8` en el `.o` EXTRAIDO mide **`0x21C` = 540 B**, y `0x80408FB8 +
0x21C = 0x804091D4`, que es exactamente donde empieza el primer literal
REFERENCIADO del objetivo. Los 96 B que faltaban son el final de
`Pkt_Body_Send` (el bloque lo cortaba por la mitad), `World_OneShotEffect`,
`Pkt_Effect_Send`, `WorldEffectConn`, `Pkt_Effect_Open` y `Pkt_Effect_Service`
-- **los mismos seis que emitiamos duplicados** en el bloque `.asciz` del final
del TU. Ahora `obj[0:29] == nue[0:29]` en `rodorden` (en secuencia 86 -> 93) y
la primera diferencia de `.rodata` se corre de `0x80409174` a `0x804091D4`.

**El detalle que hay que entender antes de repetirlo en otra unidad**: al quitar
los 13 duplicados de golpe, `.rodata` se quedaba **-128 en el ENLACE** aunque el
`.o` midiera EXACTAMENTE `0xDA0` como el objetivo. Es que esos duplicados no
eran contenido, eran **relleno**: el estripado se lleva 128 B de `$LC` muertos
que el objetivo NO tiene (`deadlink zTrack` no propone ni una entrada nueva, o
sea que salvarlos seria contenido equivocado), y los duplicados los compensaban.
Por eso se han dejado **siete** de ellos (138 B) con un comentario que lo dice.

> **Regla, y es nueva:** en una unidad con relleno escrito a mano, **acertar el
> tamano del `.o` no es acertar el tamano del ENLACE**. Hay que mirar las dos.
> Retirar el relleno solo se puede hacer a la vez que se escriben los 128 B de
> contenido real que faltan.

---

## 4. Los negativos, con su cifra

### N1 (zTrack) -- `BWARE_PREFIX_GAMECUBE`: positivo de 4 B, y no compensa

La veda de la r60 decia "MEDIDO Y NEGATIVO". **Era un artefacto de herramienta**,
y se ha remedido con **copia privada de `keep.lst` ya pasada por `lcfix`**:

    #define BWARE_PREFIX_GAMECUBE (_bwarePrefix)   ->  4.116 -> 4.112 B

O sea **positivo, pero valen CUATRO BYTES**, y cuesta **39 correcciones de
`lcfix`** sobre `keep.lst`. **No se ha aplicado**: se recoge el dia que se haga
el reorden entero del pool de la unidad. Anotado en `zTrack.cpp` junto a la
veda vieja.

### N2 (zTrack) -- `ATTRIB_TAGS_HAND_POOL` NO vale aqui, y por que

La receta que en zFe, zAI, zRender y zAnim es positiva **destroza zTrack**:
`AttribSys.h` usa **UNA sola guarda** para dos cosas distintas (`:43` el
`ATTRIB_TAG()` de las ocho etiquetas y `:88` el de `USE_ATTRIB_ALLOC`), asi que
ademas de matar la segunda copia de `Attrib::Attribute..Blob` mata los
`Attrib::Gen::<clase>` -- y el objetivo **SI tiene** `ecar`, `camerainfo`,
`pvehicle` y `engine` (`obj[66..69]` de `rodorden zTrack`). Medido: 167 -> 144
cadenas, **15 parejas `@lc` de `keep.lst` caducadas** y **`.rodata` -320 ->
secciones descuadradas**.

Y en el mismo barrido:

    #define HAND_POOL_TAG(s) ((const char *)0)
    #define DEAD_STR_DONE(s) ((const char *)0)      ->  4.112 -> 54.288 B

**PEOR EN 50.176 B.** En zTrack `EventSequencerSystems`, `EAGL4::SymbolEntry` y
`done` **no son parasitos**: su cadaver (`size & ~7`) es lo que cuadra el pool
que va detras. La receta de zFe/zAI **no se extrapola**
(`memory/nfsmw-extrapolar-frentes`).

De paso, una linea de `keep.lst` que sobra pase lo que pase: **`@lc zTrack
"Attrib::Gen::gameplay"` (`keep.lst:1489-1490`, `$LC237`) salva una cadena que
el objetivo NO tiene en su rango** -- `rodorden` no la ve en ninguno de los 138
puestos del objetivo.

### N3 (zLua) -- adelantar la peticion de instanciacion arrastra el arbol entero

`textorder zLua` deja SOLO DOS funciones descolocadas de 538: el objetivo emite
`__Q33UTL3Stdt3map3ZUiZ18LuaAttribAccessorsZ9_type_map` (116 B) **delante** de
`Thunk__10VoidBinderP9lua_State` (48 B) y nosotros al reves, porque
`LuaBindery.cpp` se parsea antes que `LuaAttributes.cpp` y la cola de
`finish_file` sale en el orden del PRIMER PUNTO DE USO. Sembrando la peticion
antes del `#include`:

    #include "Speed/Indep/Src/Lua/LuaAttributes.h"
    static inline void _seed() { UTL::Std::map<unsigned int, LuaAttribAccessors, _type_map> _m; }

**coloca las dos en el orden del objetivo** -- pero arrastra el `_Rb_tree`
entero: **DESCOLOCADAS 2 -> 86 de 538**, con dos saltos nuevos
(`_M_erase`/`_M_insert` a -10.324 B y `BindVoidFunction` a +104). Para recoger
esto hay que pedir **solo el constructor**, no el arbol. Anotado en `zLua.cpp`
delante del `#include` de `LuaBindery.cpp`.

### N4 (metodo) -- la metrica del encargo se puede automatizar, y hay que hacerlo

Medir una edicion que renumera `$LC` sin tocar el `keep.lst` del arbol se puede
hacer entero, y sin carreras con los demas agentes:

    cp config/GOWE69/keep.lst scratchpad/<tuyo>/keep_priv.lst
    KEEPLST=... python scratchpad/<tuyo>/keepsync.py <unidad>     # quita @lc caducados + lcfix
    KEEPLST=... python scratchpad/<tuyo>/dolwhere_priv.py <unidad>

`lcfix_priv.py` y `dolwhere_priv.py` son las del arbol con `KEEP`/`LDFLAGS`
leidos de `$KEEPLST`. **Control hecho**: con la copia privada intacta dan la
MISMA cifra que las originales (4.116 B las dos). Sin ese control la medida no
vale nada.

---

## 5. Lo que NO propongo, y por que

* **Ni una linea de `splits.txt`, `symbols.txt`, `keep.lst` o `configure.py`.**
  El paquete P3 de `r61c-aranges.md` toca `.data`/`.bss` de siete de mis ocho;
  ninguna de esas rayas arregla nada de lo medido aqui y todas exigen
  re-extraer. El `gap_` de zTrack se arregla en la FUENTE y sale gratis.
* **`zTrack.o:gap_07_804A59DA_bss`** (`keep.lst:586`) nombra un hueco de 2 B que
  el objeto extraido tiene y el nuestro **no emite**. Hoy es inofensivo --la
  `.bss` cae en su sitio-- y **anadirlo romperia el arreglo de la seccion 2**.
  Dejarlo como esta.

---

## 6. El siguiente paso, por rentabilidad

1. **Barrer `gapcmp`/`symshift2` sobre las 13 unidades que no son mias.** Ha
   valido 2.260 B en la primera unidad a la que se ha aplicado, cuesta un
   segundo (`gapcmp`) y dos enlaces (`symshift2`), y **es invisible para todas
   las herramientas que se estaban usando**. Ninguna de las nueve unidades "sin
   codigo pendiente" ha pasado nunca por esta prueba.
2. **Barrer los `.asciz` de rutas de Windows dentro de `asm()`** en todo el
   arbol (seccion 3, edicion 2). Una barra de menos acorta la cadena y corre el
   bloque entero.
3. **zLua, y solo el pool.** Es la unidad mas cerca del proyecto: cinco simbolos
   globales fuera de sitio en todo el DOL. Sus 8.000 B son orden de cadenas, y
   el camino es el de zFe (ordenar los `#include` de `Generated/AttribSys` y los
   `asm()` de `.rodata` de fichero, que salen DONDE ESTAN ESCRITOS).
4. **zTrack, los 3.956 B que quedan**: 2.894 B de `.rodata` a partir de
   `0x804091D4`, 34 B de `VisibleGroupInfoTable` (punteros a esas cadenas: se
   arreglan solos) y ~1.030 B de `.text`. El primer literal del objetivo ahi es
   una CONSTANTE DE COMA FLOTANTE (`0x43300000 0x80000000`), no una cadena: o
   sea que `GAMECUBE` + las ocho `Attrib::*` + `EventSequencerSystems` + `done`
   + `GManager` + `EAGL4::SymbolEntry` (~330 B) **no deberian estar ahi**, y
   retirarlas exige a la vez escribir los 128 B de contenido que faltan
   (seccion 3).

---

## 7. Herramientas nuevas (en `scratchpad/resto62/`, sin promocionar a `scripts/`)

| fichero | que hace |
|---|---|
| `symshift2.py` | enlaza la base UNA vez y luego cada unidad; los simbolos GLOBALES que cambian de direccion, por delta. **La unica que ve el fallo de la seccion 2.** |
| `gapcmp.py` | `gap_`/`pad_`/`lbl_` de distinto tamano entre el `.o` extraido y el nuestro, seccion a seccion. Instantaneo. |
| `seclay.py` | las dos tablas de simbolos de una seccion en paralelo, con marca en cada fila que no cuadra. |
| `rodlist.py` | las cadenas de `.rodata` del objetivo y las nuestras, numeradas, para leer los indices que da `rodorden`. |
| `keepsync.py` | quita de una COPIA PRIVADA de `keep.lst` las parejas `@lc` caducadas y llama a `lcfix`. |
| `lcfix_priv.py` / `dolwhere_priv.py` | las del arbol con `KEEPLST` por variable de entorno. |

---

## 8. Estado del arbol al cerrar

Ficheros tocados (los tres, UTF-8 puro, cero bytes > 127):

* `src/Speed/Indep/Src/World/EventManager.cpp` -- `gap_07_804A5DB8_bss` 160 -> 168.
  **Solo lo incluye `zTrack.cpp`** (comprobado), asi que no hunde ninguna unidad
  sin agente.
* `src/Speed/Indep/SourceLists/zTrack.cpp` -- ediciones 2 y 3, mas los negativos
  N1 y N2 escritos junto a la veda vieja para que `previo.py` los encuentre.
* `src/Speed/Indep/SourceLists/zLua.cpp` -- **solo comentario** (negativo N3).

Sellos (`build_direct.py` x3, identicos las tres veces):

* `zTrack.o` = `63d67e5002f69625ddc6fa15d9f4d00a139888d2`
* `zLua.o`   = `0bf04a34791ca896c148f6c4596bf60ed7bd1927`

`python scripts/lcfix.py zTrack --check` -> **todas al dia, rc=0**.
**Correcciones de `lcfix` pendientes que dejo: CERO. Ninguna venenosa.**
`config/` sin tocar. Sin commit.
