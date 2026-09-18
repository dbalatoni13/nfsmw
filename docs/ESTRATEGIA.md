# Estrategia de cierre

**NO TE CREAS LAS CIFRAS DE LAS SECCIONES 1 Y 2: están congeladas en la r51.**
Las vivas las dan dos herramientas, y las dos se niegan a hablar si están
rancias:

    python scripts/estado.py     # el frente de hoy
    python scripts/serie.py      # el ritmo real, y en ENCARGOS, no en rondas

Estado al cerrar la r54 (DOL `9619ba57…` verificado):

| | r51 (lo que dicen §1-§2) | hoy |
|---|---:|---:|
| `linked` | 506 / 616 | **518 / 616**, techo 544 |
| entradas de función abiertas | 32 | **29** (28 reales + relleno del DOL) |
| unidades que faltan | 39 | **27** |
| bloque A (código al 100 %) | 13 uds, 1.748.504 B | **13 uds, 1.652.576 B** |

Las secciones **3 y 4 sí están al día**: §3 es por qué se van las rondas y §4 la
apuesta actual. Empieza por ellas.

---

## 1. Lo que falta, exactamente

### Código: 32 funciones, 36.832 B

Eso es **todo**. No hay más código pendiente en el proyecto: 3.909.216 de
3.946.048 B, el **99,067 %**.

Y hay un dato que cambia el reparto: **las 32 están en unidades que no enlazan.**
Ninguna es trabajo que sólo mueva el porcentaje; todas están en el camino
crítico de `linked`.

| unidad | fn | bytes | |
|---|---:|---:|---|
| zCamera | 5 | 10.812 | incluye el `__static_initialization_and_destruction_0` de 3.604 B |
| zEcstasy | 4 | 5.180 | |
| zWorld | 4 | 4.772 | |
| zWorld2 | 2 | 3.796 | las dos por encima del 99,6 % |
| zEagl4Anim | 2 | 2.808 | |
| steering | 6 | 2.568 | la unidad con más funciones abiertas |
| zPlatform | 1 | 1.588 | |
| zEAXSound2 | 1 | 1.240 | |
| madidct | 2 | 1.148 | **46 % y 50 %**: los dos peores del proyecto |
| zPhysicsBehaviors, criticalpath, zTrack, zSpeech | 4 | 2.900 | |
| `pad_00_8000348C_init` | 1 | 20 | relleno de un comodín, no es una función |

Once están por encima del 99 %: son near-miss de una a tres instrucciones.

### Datos: **cero bytes de trabajo real**

La cifra de `report.json` —`Data 34,54 %`, «faltan 841.700 B»— **no mide datos**.
Está demostrado en `docs/analisis/r50-jf-metrica-datos.md` y ahora lo dice
`estado.py` al arrancar. Partida por estado de enlace:

| | unidades | datos | % | |
|---|---:|---:|---:|---|
| enlazadas | 247 | 426.497 / 435.625 | 97,9 % | los 9.128 B que «faltan» son **nomenclatura**: una unidad enlazada produce el DOL correcto por definición |
| sin enlazar | 102 | 88.276 / 850.108 | 10,4 % | **sin sentido**: los nombres no emparejan (contador de declaración, `$LC` contra `lbl_`, relleno `gap_`/`pad_`) |

Nueve unidades sin enlazar dan **exactamente 4 B casados** sobre totales de 25 a
141 kB. Eso no es una medida, es objdiff emparejando un símbolo por casualidad.

**El 34,54 % es un proxy de cuántas unidades enlazan.** Sube cuando una
promociona, no cuando se escribe un dato. No debe usarse para repartir trabajo, y
ningún encargo debe justificarse con él.

### Enlace: 506, y el techo no es 616

**71 de las 110 que faltan son comodines `auto_*`** — rangos que el troceador no
atribuye a nadie. No son fuente y no pueden enlazar nunca.

    techo real = 616 − 71 comodines − prodg_fixes = 544

**Quedan 39 unidades**: 23 SourceLists y 16 de biblioteca. (Los ocho de la
ventana —rcmpbase, rcmp_mad_codec, rcmp_vp6_codec, avplayer, bigyuvswizzler,
bigswizzler, sdspmix, filesys— y `OSUtf.c` ya están dentro de las 506.)

---

## 2. Las 39, partidas por lo que de verdad las bloquea

Esto es lo que `estado.py --uds` imprime, y es el reparto que hay que respetar
porque **las tres familias no se atacan igual**.

### A · 13 SourceLists con el código al 100 % — 1.748.504 B

`zAI, zFe2, zFe, zMain, zLua, zMisc, zAnim, zGameModes, zSim, zEcstasy…`

**1,75 MB de código perfecto que no promociona.** Es el 44 % del código del
juego. Aquí **no hay nada que hacer de codegen**: mandar un agente a mejorar
funciones de zAI es tirar la ronda, porque zAI ya está al 100 %.

Las bloquean dos cosas, las dos medidas:

1. **El orden de emisión.** `permorden.py` da la permutación exacta y ordena la
   cola: `zSim 0 · zMain 32 · zLua 68 · zFe2 144 · zAI 182 · zGameplay 270`
   funciones desplazadas. zSim llegó a **cero** y sigue rompiendo el DOL, así que
   el orden es necesario pero no suficiente.
2. **El déficit de `.rodata`**, que son literales muertos (`r50-rodata.md`).

### B · 12 unidades de biblioteca que se cierran con casi nada

**Este es el frente barato, y no lo ha tocado nadie.** Un cuarto de lo que queda.

**Seis sin fuente** — están en `splits.txt` pero no hay un `.c` que compilar, y
lo que emitían es minúsculo:

| unidad | qué emite el original |
|---|---|
| `sformat`, `soutputmap`, `mpeghufftables` | **nada en absoluto**: 0 B de código y 0 de datos |
| `mpegl3base` | 8 B de `.rodata` |
| `smixvec` | 28 B de `.bss` |
| `author` | 64 B de `.data` |

**Seis con el código al 100 % y un bloqueo de datos de ≤32 B** (medido con
`promote.py`):

| unidad | qué le pasa |
|---|---|
| `libgcc2_4` | emitimos 24 B de `.rodata` de más |
| `pathnode` | emitimos 32 B de `.rodata` de más |
| `sserver` | dos símbolos de `.bss` en orden cambiado |
| `ssysinit` | `.bss` 28 contra 32 B, y dos símbolos descolocados |
| `inittmr` | `.rodata` 24 B, `.sbss` 4 B, `.sdata` 4 contra 8, sobra `TIMERhz` |
| `interface` | el mayor: `.text` 260 contra 204 y 10 símbolos de más |

### C · 3 unidades de biblioteca con funciones abiertas

`steering` (6 fn), `madidct` (2 fn al 46 % y 50 %), `criticalpath` (1 fn).

**CORREGIDO en la r51** (`r51-steer.md`): no es «codegen normal» en las dos que
se midieron.

- **`steering`: REFUTADO en la r55. Su deuda de datos esta PAGADA.** Decia aqui
  que no promocionaria aunque se cerraran sus seis funciones, por 40 B de `.bss`,
  40 de `.sdata2`, `g_bInitialized` fuera de rango y cuatro reubicaciones con
  sufijo de direccion. Los tres rangos entraron en la r54 con DOL OK, y hoy
  `promote.py` deja solo esto:

      - .text mide 8752 B y el extraido 8760 B
      - 7 simbolo(s) en OTRO SITIO de su seccion   <- consecuencia de los 8 B
      - 4 reubicacion(es) a simbolo con sufijo de direccion

  Y **el tercer sintoma es ruido**: comprobado simbolo a simbolo, los DOS objetos
  referencian `g_iGammaLookup_804B9E78`, `g_iQuarterSineTable_804B9EFC` y
  `g_iRampUpTable_804B9F7C` como `UNDEF` con el mismo nombre. `promote.py` marca
  ahi un PATRON sospechoso, no una diferencia.

  O sea que `steering` es **codegen puro**: 2.568 B en seis funciones que
  desbloquean 8.760 B, y cerrarlas ahora si la promociona.
- **`madidct` esta agotada desde la fuente**: el DWARF da sus 9 locales con su
  registro y nuestra fuente ya es la del original. Los pines sobre los registros
  que dicta el propio DWARF salen todos peores, y los cinco ProDG del arbol dan
  un `.text` byte a byte identico. Solo le faltan sus dos funciones para
  enlazar, pero no hay palanca de fuente conocida.

Y para las tres vale el catalogo nuevo: `steering` va con **Metrowerks**, no con
GCC, y `PLAYBOOK.md` entero esta escrito para GCC. El catalogo de MWCC esta en
`r51-steer.md` §1.

### D · 1 imposible

`prodg_fixes`: sus tres rangos llevan `skip`. Nunca enlazara. **El techo baja a
544.**

---

## 3. Por qué se van las rondas — medido, no opinado

### 3.1 La mitad de `scripts/` no son herramientas

**111 de los 217 ficheros son sondas de un solo uso** que un agente escribió para
una ronda y nadie borró. `agent_phb` sola son 40 ficheros. Un agente que busca
herramienta tiene que cribar eso.

### 3.2 El índice documentaba 40 de 110

Entre las 70 que faltaban estaban justo las que deciden el frente: `linkdelta`,
`permorden`, `promote`, `triaje`, `fncmp`, `refs`, `dolwhere`.

**Coste medido, no hipotético:** la r48 entera se construyó comparando OBJETOS
—y de ahí salió un predicado, una lista de culpables y un titular, todos falsos—
mientras `linkdelta.py` llevaba **dos días en el árbol** con un docstring que
avisaba de ese error exacto.

*Arreglado hoy:* `scripts/indice.py` lo genera de los docstrings y `--check` dice
si está rancio.

### 3.3 El manual pesa 442 kB

`PLAYBOOK.md`: 6.239 líneas. Nada de ese tamaño se lee entero, así que el
conocimiento no llega a quien trabaja. 248 informes de ronda más, 5,5 MB de
`docs/`.

### 3.4 Las medidas mienten más de lo que parece

Sólo en esta sesión, en herramientas que yo mismo había promocionado:

- `linkdelta.py` **no resolvía ni una unidad de biblioteca** (construía la ruta
  con el basename) y colapsaba 6 basenames repetidos en una sola fila.
- `permorden.py` daba **17 funciones «que no emitimos»** en zFe2 que están todas
  definidas: exigía `st_size`, y un alias de `asm()` no lleva `.size`.
- `promote.py` responde «sin objeto construido» si le das un basename en vez de
  una ruta — que es lo que hice yo hace una hora.

La memoria `nfsmw-validar-metrica` va por el caso número doce.

### 3.5 Los andamios caducan con el árbol

`CullParts` cerró hoy quitando un `asm("" : : : "r11")` que **tres rondas
distintas** (r36d, r48, r49) habían medido como imprescindible. Con el árbol
corregido era al revés: 4 → 0 filas. Ningún `asm` de andamio vale para siempre.

### 3.6 «La ronda» no es una unidad de trabajo, y llevábamos pronosticando en ella

La r42 fueron **2** informes de agente; la r52 fueron **10**. Y la r46 llevaba
dentro **seis iteraciones**: entre el commit que añade su `cierre` y el que añade
`brief-r47.md` hay cinco commits de trabajo real, uno titulado «El SDK entero al
100 %: 98 de 98 unidades». Su propio documento de cierre está escrito a mitad de
ronda y sus cifras no son las del cierre.

Así que el «80-160 **rondas**» de la r53 medía con una regla que cambia de largo
entre 2 y 10. **La unidad honesta es el encargo de agente**, y con ella el frente
se acota de verdad: quedan 29 entradas de función y 26 unidades, que a los ritmos
de la serie son **78 y 61 encargos** — o 174 y **ninguno** al ritmo de las dos
últimas rondas, porque el bloque A lleva dos rondas dando cero y **un numerador a
cero no tiene ritmo que extrapolar**.

`scripts/serie.py` reconstruye la serie de los propios documentos y **se niega a
pronosticar si su última fila no reproduce `report.json`**. Detalle y las dos
formas en que el propio reconstructor se equivocó primero:
`docs/analisis/r54-jf-serie.md`.

---

## 4. El plan

### 4.1 La apuesta grande: los dos mecanismos de la r54

**Cerrada la anterior.** La apuesta de la r50/r51 era el prefijo estándar de TU
—~600 B de `.rodata` por unidad que `cc1plus` no emite porque en nuestro árbol
están muertas—. Se aplicó en la r52 y cerró **11.632 B de déficit en siete
unidades**; el resto del vocabulario resultó no ser una palanca sino veintiuna,
porque de las 43 cadenas comunes hay **catorce órdenes distintos**. El frente de
`.rodata` bajó de −24.416 a −6.080 B y dejó de ser lo que bloquea.

**La apuesta ahora son los dos mecanismos que nombró la r54**, y los dos tienen
línea de GCC y palanca probada:

**1. La posición la decide el punto de USO dentro de una función.**
`instantiate_decl` (`cp/pt.c:9439-9461`) instancia una plantilla *inline* en el
punto del parseo si —y sólo si— su primer uso está dentro de una función
(`nested = in_function_p()`, `pt.c:9308`); si no, se difiere al final de la TU.
Y `finish_file` (`cp/decl2.c:3643`) drena **colas distintas**: las plantillas por
`pending_templates`, las vtables por `walk_globals`. Por eso tocar el `#include`
mueve unas y no las otras.

La palanca son dos líneas y cuesta `.text +0` porque el enlazador estripa la
función muerta. En `zMain`: `dolwhere` 17.269 → **7.961 B**, con `fncmp` 0/1.380
y `linkdelta` IGUAL. **Es un andamio** y hay que marcarlo como tal.

**2. El cambio de dueño.** Un símbolo que *nuestro* objeto define y el original
de esa unidad no, y que otro objeto también define, se lo lleva el enlazador.
Invisible para `linkdelta`, `permorden` y `fncmp`; se mide en **dos restas**, ya
en `movidos.py`. Un método de plantilla de **120 B** envenena `zMisc`,
`zPhysics` y `zGameplay` a la vez.

Y con ellos, la medida que faltaba: **`reorden.py`** contesta *«¿es orden o es
contenido?»* recolocando nuestros bytes en la secuencia del objetivo. En `zMain`
da 0 de 39.617 palabras; en `zFe2` dice que **no**, y ése es el control que la
hace útil.

**El límite conocido, y lo firma el propio agente que lo midió**: `zMain` **no
es representativa** —es la más limpia de las trece por un factor de 20 a 76—,
así que propagar allí valía 159.776 B y no 1,65 MB. Por eso la r55 **empieza
midiendo las doce con las medidas nuevas** en vez de repartir por la tabla
antigua.

### 4.2 El reparto de la r55

Lo decide el triaje, no la intuición: doce agentes, uno por unidad del bloque A,
más el censo global de cambio de dueño hecho por el predicado correcto —de
**definición**, no de salto de dirección, que es lo que se le escapó a la r54 y
por lo que `zLua` salía como «dueño 0» cuando se lleva 17.676 B de `.text`—.

Objetivos individuales ya nombrados y con cifra, a la espera del triaje:

- **`clear__..._List_base<WGridManagedDynamicElem,…>`** — 120 B, tres unidades.
- **`ActualReadJoystickData`** (zPlatform, 1.588 B) — el único de 199 casos con
  **cero uniones** entre sus dos usos, o sea el único donde la cadena de `cse1`
  queda al alcance.
- **`CarCustomize.hpp:686`** — catorce globales **definidas** en una cabecera en
  vez de declaradas; `uiQRCarSelect.hpp` las propaga a `zAnim` y `zFe2`.

### 4.3 Reglas nuevas, para no repetir lo de §3

1. **Un encargo empieza con `python scripts/estado.py`.** No se rehace el censo.
2. **Ninguna sonda de un solo uso en `scripts/`.** Van al scratchpad y se borran.
   Lo que merezca quedarse, se queda **con docstring** y sale en el índice solo.
3. **Antes de repartir una medida nueva, contrastarla contra tres diagnósticos ya
   conocidos.** Es la regla de `nfsmw-validar-metrica` y lleva doce casos.
4. **Cuando un cambio de árbol mejore una función, re-medir todos sus `asm` de
   andamio.** Lo de §3.5 no fue mala suerte.
5. **`linked` se mide en el ENLACE** (`linkdelta.py`), nunca comparando tamaños
   de sección de los objetos.

### 4.4 Deuda que pagar cuando haya hueco

- Sacar las 111 sondas de `scripts/` a `docs/congelado/` o borrarlas.
- Partir `PLAYBOOK.md` por frentes, para poder darle a cada agente su trozo.
- Docstring a las 9 herramientas que no lo tienen (las lista `HERRAMIENTAS.md`).
- `promote.py`: aceptar el basename, o decir «no encuentro esa unidad» en vez de
  «sin objeto construido».

---

## 5. Cuánto queda, honestamente

| | r51 | tras la r54 | objetivo |
|---|---:|---:|---:|
| código | 99,067 % | **99,1463 %** | 100 % = **28 funciones reales** |
| enlace | 506 | **518** | **544** (no 616) |
| unidades que faltan | 39 | **27** | — |

Lo cifra `serie.py`, y en la unidad honesta: **78 encargos de agente para las
funciones y 61 para las unidades** al ritmo de toda la serie; 174 y **ninguno**
al ritmo de las dos últimas rondas, porque el bloque A lleva dos rondas dando
cero y un numerador a cero no tiene ritmo que extrapolar.

**Y la premisa del bloque B está mal**, medido en la r55: de las seis unidades
«a UNA función», ninguna de las tres medibles lo está. `zPhysicsBehaviors` tiene
**cero** contenido que escribir y su única función quedó vetada
estructuralmente; `zTrack` y `zPlatform` arrastran además `rodata` corto. Ver
`docs/analisis/r55-jf-bloqueB.md`.
