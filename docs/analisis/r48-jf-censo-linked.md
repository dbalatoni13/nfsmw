# El censo del frente de `linked`: 1,64 MB de código perfecto sin enlazar

Hecho mientras los siete agentes de la r48 trabajan sobre el código pendiente.
Todo es lectura de los dos ELF: no toca el árbol.

## El tamaño real del frente

`matched` va al **98,98 %** y `linked` al **19,55 %**. La distancia no es una
curiosidad de la métrica: es dónde está el trabajo que queda.

| | |
|---|---:|
| unidades del reporte | 619 |
| sin enlazar | **45** |
| de ésas, **con el código YA al 100 %** | **28** |
| código perfecto que no promociona | **1.643.216 B** |
| código todavía pendiente (los 7 agentes) | 40.340 B |

**El código perfecto sin enlazar es cuarenta veces el pendiente.** Y once
SourceLists son el 98 % de él:

| B | unidad | | B | unidad |
|---:|---|---|---:|---|
| 272.796 | `zAI` | | 146.124 | `zPhysics` |
| 250.732 | `zFe2` | | 141.472 | `zGameplay` |
| 174.200 | `zFe` | | 96.400 | `zSim` |
| 159.776 | `zMain` | | 94.264 | `zLua` |
| 151.092 | `zEAXSound` | | 78.008 | `zMisc` |
| | | | 42.292 | `zAnim` |

## El censo de símbolos de más, por primera vez completo

`extrasym.py` sobre las 32 SourceLists, separando las dos familias:

| familia | símbolos | B |
|---|---:|---:|
| **A** — existe en el ELF original, en el rango de OTRA unidad | 1.040 | 124.276 |
| **B** — NO existe en ninguna parte del original | 2.786 | **163.096** |

## Y aquí cae una veda del proyecto

`ventana-pendiente.md` §2 dice que nueve SourceLists casan al 100 % y no
promocionan **porque exportan funciones que no referencia nadie**, y concluye:
«o esas funciones no se emiten desde la fuente, o esas nueve unidades no
promocionan».

**Los símbolos de más NO bloquean la promoción por sí solos.** Cruzando el censo
con el estado de enlace, **nueve SourceLists enlazan hoy teniéndolos**:

| unidad | símbolos de más | B | de los cuales familia B |
|---|---:|---:|---:|
| `zAttribSys` | 23 | 3.132 | **3.132 — el 100 %** |
| `zBWare` | 126 | 2.808 | **2.808 — el 100 %** |
| `zRender` | 22 | 2.556 | 992 |
| `zMiscSmall` | 21 | 2.524 | 960 |
| `zFEng` | 142 | 2.296 | 2.240 |
| `zDebug` | 7 | 736 | 736 |
| `zDynamics` | 9 | 292 | 292 |
| `zFoundation` | 11 | 184 | 36 |
| `zMission` | 8 | 40 | 40 |

`zAttribSys` y `zBWare` son el contraejemplo limpio: **todos sus símbolos de más
son de la familia B** —código que el original no tiene en ninguna parte— y las
dos enlazan con el DOL byte a byte. El enlazador se los lleva.

**Y no es un umbral de tamaño**: `zGameModes` tiene **496 B** de símbolos de más
y NO enlaza; `zAttribSys` tiene **3.132 B** y sí. O sea que lo que decide no es
cuántos bytes sobran, sino si algo los **referencia** —un símbolo que nadie usa
se estripa; uno que entra en una vtable, en una tabla de `.ctors` o en una
reubicación de otra unidad, no—.

## Qué cambia esto

El frente estaba planteado como una decisión de fidelidad —«o dejamos de emitir
esas funciones desde la fuente, o esas unidades no promocionan»— y **no lo es**.
Es un problema de referencias, y por tanto tiene arreglo unidad por unidad sin
tocar la fuente de las funciones sobrantes.

La pregunta correcta para cada unidad no es *cuántos símbolos de más tiene* sino
**cuáles de ellos están referenciados y desde dónde**. Eso lo dan las
reubicaciones de los otros 618 objetos, y es una medida, no una conjetura.

## Y el diagnóstico de datos que había era una lectura de la métrica

Las once SourceLists grandes salen en `report.json` con decenas de miles de
bytes de datos y **exactamente 4 B casando** —`zAI` 31.316 → 4, `zFe2` 34.136 →
4, `zMain` 71.864 → 4—. Eso hizo pensar que el bloqueo eran los datos.

Comparando las secciones de los dos objetos, no lo es:

| unidad | `.text` | `.rodata` | `.data` | `.bss` |
|---|---:|---:|---:|---:|
| `zGameplay` | **+12.144** | −2.072 | −152 | +24 |
| `zAI` | **+5.660** | −768 | −388 | +64 |
| `zLua` | **+17.436** | −1.032 | −324 | +96 |

Los datos difieren en cientos de bytes, no en decenas de miles: el `4` del
reporte es un artefacto de emparejamiento de objdiff, no una medida. Lo que
sobra de verdad es `.text`, y lo que **falta** es `.rodata` —el objetivo tiene
más, no menos—, que es un frente distinto y sin diagnosticar.

## Medida 1 — cuáles de los símbolos de más están REFERENCIADOS

Índice de UNDEF de los 619 objetos del enlace (6.079 símbolos referenciados
distintos), cruzado con los símbolos de más de cada unidad sin enlazar.
Arnés: `scripts/refs.py`.

**De 2.010 símbolos de más, sólo 413 los referencia otro objeto (46.096 B).**
Los otros 1.597 no los pide nadie y el enlazador se los lleva.

| unidad | sobran | referenciados | B |
|---|---:|---:|---:|
| `zMain` | 165 | 71 | 7.560 |
| `zSim` | 158 | 60 | 6.392 |
| `zPhysics` | 200 | 56 | 5.060 |
| `zGameplay` | 196 | 48 | 5.160 |
| `zLua` | 159 | 41 | 4.432 |
| `zFe2` | 301 | 38 | 4.132 |
| `zEAXSound` | 224 | 25 | 4.016 |
| `zAnim` | 188 | 23 | 3.076 |
| `zFe` | 185 | 22 | 3.216 |
| `zMisc` | 91 | 20 | 2.844 |
| `interface` | 7 | **7** | 56 |
| **`zAI`** | 114 | **2** | **152** |

Y **dieciséis de las veintiocho tienen CERO** símbolos de más referenciados:
`pathbank`, `filesys`, `inittmr`, `sserver`, `ssysinit`, `sdspmix`,
`zGameModes`, `avplayer`, los tres `rcmp*`, los dos swizzlers, los dos
`rem_pio2` y `libgcc2_4`. En ésas el frente de los símbolos **no es el bloqueo**,
y hay que mirar las secciones.

**`zAI` es el hallazgo**: 272.796 B de código perfecto retenidos, y sólo **dos**
símbolos de más referenciados, de **152 B** entre los dos —
`RaiseToPower__H1i10_i_i`, que pide `zGameplay`, y una instanciación de
`find__` que pide `zSpeech`—. Los referencian porque en el original viven en
otra unidad; promocionar `zAI` los duplicaría, y ahí manda la regla de la r46
(`ngcld` descarta duplicados: GLOBAL gana a WEAK, y entre dos WEAK gana la del
objeto anterior en el orden de enlace).

## Medida 2 — el «déficit de datos» no existe: son 4 bytes

Clasificando todo lo que el objetivo tiene en `.rodata`/`.data`/`.sdata` y
nosotros no, **emparejando por NOMBRE BASE** (arnés `scratchpad/r48_jf/`):

| familia | B |
|---|---:|
| **literales** — el mismo byte con `$LC` en un lado y `lbl_` en el otro | **78.364** |
| **`gap_*` / `pad_*`** — etiquetas que el troceador pone al relleno | 5.217 |
| **mismo dato con otro contador de sufijo** — ya escrito | **1.764** |
| **dato REAL que de verdad falta** | **4** |

### Por qué hay que emparejar por nombre base

Un estático de función sale como `nombre.NNNNN`, donde `NNNNN` es el contador de
declaraciones de la unidad de traducción, y **ese contador no coincide entre los
dos objetos** porque nuestra SourceList tiene otro número de declaraciones
previas. Emparejando por nombre completo, el MISMO dato se cuenta como «falta» y
«sobra» a la vez.

Comprobado al byte en `zMisc`: el objetivo tiene `hexChars.33689` (17 B,
`"0123456789abcdef"`), `total_captured.32815` (4 B) y
`StaticEasterEggsTable.34218` (120 B); nosotros tenemos `hexChars.27570`,
`total_captured.26949` y `StaticEasterEggsTable.27745`, **con el mismo tamaño**.
Y la fuente ya los tiene escritos, en `MD5.cpp:148` y `Joylog.cpp:374`.

Es la misma familia que el `symbol_mapping` de `zGameplay`, que ya caducó una
vez por esto.

### Los 4 bytes

`counter` en `zEAXSound`: el objetivo lo tiene en `.data` y nosotros en `.bss`.
La causa es la regla documentada —en GCC 2.9 **el inicializador decide la
sección aunque el valor sea cero**— y el arreglo es una línea en
`src/Speed/Indep/Src/EAXSound/SoundCollision.cpp:291`:

```c
static int counter = 0;    /* hoy: static int counter; */
```

**Pendiente para la ventana**, no para ahora: `zEAXSound` lo está recompilando
el agente de `snd` para verificar que sigue a cero funciones, y escribir su `.o`
a la vez sería una carrera.

### Qué cambia esto

**El frente de datos está cerrado.** No hay tablas estáticas que escribir: el
99,99 % del déficit aparente es nomenclatura —literales, relleno del troceador y
contadores de sufijo—, y lo que falta de verdad son cuatro bytes con causa
conocida y arreglo de una línea.

Con la Medida 1, eso deja el frente de `linked` reducido a **una sola pregunta**
por unidad: qué símbolos de más están referenciados y desde dónde. En dieciséis
de las veintiocho, ninguno.

## Medida 3 — `promote.py` por unidad: el bloqueo real, con nombre

Tres muestras, una de cada tipo:

**`zGameModes`** (124 B de código, cero símbolos referenciados). Su bloqueo son
**secciones que el objetivo NO TIENE**: emitimos `.data` de 64 B y `.rodata` de
248 B donde el extraído no lleva ninguna de las dos. Los 22 símbolos de más no
los referencia nadie y se estriparían; lo que no se estripa es una sección
entera de más. Es la unidad más barata del censo para atacar.

**`zMisc`** (78.008 B). Secciones descompensadas en las cuatro (`.text` +6.888,
`.rodata` +3.392, `.bss` +288, `.data` +28), 98 símbolos de más y una
reubicación a `lbl_8041F8C3`, que ya está documentado como que cae **dentro de
otra unidad**.

**`zAI`** (272.796 B). Aquí sale el bloqueo con nombre y apellidos:

```
no definimos 1 simbolo(s) que el extraido exporta:
    value.10688_80456560  <- LibSN/crt0
```

**`crt0` referencia un estático de `zAI` por el nombre que le puso el
troceador.** El símbolo es `value.10688` en `.bss:0x80456560` —los primeros 4 B
de la `.bss` de la imagen— y el troceador le añadió el sufijo de dirección
porque el nombre es local y ambiguo. Nosotros emitimos el mismo estático como
`value.15790`: otra vez el contador de declaraciones.

Es exactamente la familia del racimo de `spchpick`, y tiene receta conocida —el
alias `__asm__("value.10688_80456560")` sobre la declaración—, sólo que aquí hay
que **definirlo**, no referenciarlo.

`zAI` tiene además `.bss` +64 B con 1.114 símbolos desplazados, y eso sí es un
bloqueo real y del tipo que ya cerré en la r45: **el orden dentro de `.bss` no
se manda desde la fuente** (18 formas medidas). Habrá que ver si aquí el
desplazamiento viene de un símbolo de más y no del orden.

## El mapa, después de las tres medidas

| clase | unidades | qué hay que hacer |
|---|---:|---|
| secciones de más que el objetivo no tiene | `zGameModes`, `zMisc`, y las de `.rodata` del pool | quitar la sección, no el símbolo |
| símbolo con el nombre del troceador | `zAI` (`value.10688_80456560`) | alias `__asm__` en la definición |
| `.bss` descompensada | `zAI`, `zMisc` | ver r45: el orden no tiene mando |
| ninguno de los anteriores | 16 de 28 | pasar `promote.py` y clasificar |

## Lo que queda por medir

`promote.py` sobre las 25 unidades restantes. Es vergonzosamente paralelo
—cada unidad es independiente— y hoy va en serie con un parser de ELF en Python
puro que relee el mismo objeto de 270 kB varias veces por unidad. Paralelizarlo
y cachear las tablas de símbolos daría 6-8x, y es lo que hace falta para
terminar el mapa.

## Medida 4 — el criterio de promoción, como predicado computable

La r48 dejó dos medidas que parecían contradecirse. El agente de `linked` midió
que en `zFeOverlay` el 90 % de los símbolos de más son **huérfanos dentro de
nuestro propio objeto** y que **no hay ni una sección `.gnu.linkonce`**, y
concluyó que el enlazador no puede sacar una función de un `.text` monolítico.

**Eso es falso, y se ve en una tabla**: las nueve SourceLists que enlazan hoy
tienen el `.text` MAYOR que su objeto extraído —`zAttribSys` +3.132 B, `zBWare`
+2.808— y ninguna usa `.gnu.linkonce`. O sea que `ngcld` **sí** estripa
funciones sueltas.

Lo que reconcilia las dos medidas es que **sobrevive al estripado lo que está
referenciado**, de dentro o de fuera. Y con la regla de duplicados de la r46
—`ngcld` descarta el duplicado y se lleva sus bytes— sale el predicado:

> una unidad promociona si y sólo si **todos** sus símbolos de más son
> huérfanos por los dos lados **y** ningún otro objeto del enlace los define.

**Contrastado contra la realidad: 20 aciertos y 1 fallo** sobre las unidades con
el código al 100 % (`scripts/promopred.py`). El único fallo es `zBWare`, que
enlaza con 8 vivos de 236 B y queda como pregunta abierta.

### Y el número de culpables se desploma

Aplicando el predicado, los cientos de símbolos de más se quedan en **uno o dos
por unidad**:

| unidad | sobran | de verdad vivos | B |
|---|---:|---:|---:|
| `zSim` | 158 | **1** | 104 |
| `zGameModes` | 18 | **1** | 80 |
| `zGameplay` | 196 | **2** | 120 |
| `zAnim` | 188 | **2** | 156 |
| `zAI` | 114 | **2** | 208 |
| `zMain` | 165 | **2** | 284 |
| `zFe`, `zMisc` | 185, 91 | 3 | 228, 320 |
| `zFe2` | 301 | 9 | 272 |
| `zEAXSound` | 224 | 13 | 1.300 |
| `zPhysics` | 200 | 21 | 3.432 |
| `zLua` | 159 | 23 | 2.728 |

### UN símbolo bloquea VEINTE unidades

```
20 unidades   104 B   _M_erase__Q24_STLt8_Rb_tree5ZPQ26Hermes13_h_HHANDLER__...
```

Una instanciación de `_Rb_tree::_M_erase` de la STL, de 104 B, que emitimos en
veinte SourceLists y que el original no tiene en ninguna. **Es el objetivo
individual más valioso del frente de `linked` en todo el proyecto.**

El resto de la lista son colas de una unidad cada uno: los `DumpConstants` /
`DumpHeader` / `DumpFunction` de `zLua` —el volcador de bytecode, que el
original no enlazó—, y una veintena de destructores
`_._Q33UTL11Collectionst8_Storage2Z...`, que son la misma familia de plantilla
con distinto tipo.

### La palanca, medida por el agente de `linked`

`-fno-implicit-templates` sobre `zFeOverlay`: `.text` 148.408 → **142.528**, con
**cero funciones distintas y cero símbolos perdidos**. Añadiendo
`-fno-implement-inlines`, 141.648 contra los 141.224 del objetivo. Es **por
unidad**, y el criterio de seguridad es que no falte ningún símbolo.

Encaja con el predicado: la bandera deja de emitir precisamente las
instanciaciones implícitas, que es lo que son el `_M_erase` y los veinte
destructores de `_Storage`.

## El mapa completo de `promote.py` (28 unidades, en paralelo)

`scripts/promomap.py`, 4 procesos, 32 min con la máquina saturada por los siete
agentes. Detalle en `scratchpad/promomap.json`.

| familia de bloqueo | uds |
|---|---:|
| sección descuadrada | 22 |
| símbolo desplazado | 20 |
| exportamos de más | 16 |
| nos falta un símbolo | 4 — `zAI`, `pathbank`, `ef_rem_pio2`, `kf_rem_pio2` |
| reubicación con sufijo de dirección | 2 — `zMisc`, `avplayer` |
| sección de más | 8 |

Una unidad sale en varias familias: son todos sus motivos, no uno.

### La síntesis: tres de las familias son el MISMO problema

«Sección descuadrada» y «símbolo desplazado» **no son causas, son síntomas**.
Nuestro `.text` mide más porque emitimos símbolos de más; al medir más, todos
los símbolos posteriores caen en otro offset. Las tres familias grandes —22, 20
y 16 unidades— son la misma cosa vista en tres sitios.

Y por la Medida 4 sabemos que dentro de «exportamos de más» sólo cuentan **uno o
dos símbolos por unidad**, y que **uno solo bloquea veinte**.

Quedan como frentes propios, y son pequeños:

- **«nos falta un símbolo»** (4 unidades): `zAI` con
  `value.10688_80456560` que pide `crt0`, y la familia `rem_pio2` más
  `pathbank`. Es la receta del alias `__asm__`.
- **«reubicación con sufijo de dirección»** (2): `zMisc` con `lbl_8041F8C3`, que
  ya está documentado como que cae dentro de otra unidad.
- **«sección de más»** (8 unidades, 9.340 B): las pequeñas del pool, más
  `zGameModes`, que emite `.data` de 64 B y `.rodata` de 248 donde el extraído
  no lleva ninguna.

## El orden de ataque que sale de todo esto

1. **`-fno-implicit-templates` por unidad.** Ataca la causa de las tres familias
   grandes a la vez. Medido en `zFeOverlay`: −5.880 B, cero funciones distintas,
   cero símbolos perdidos. Criterio de seguridad: que no falte ninguno.
2. **El `_M_erase` de `_Rb_tree`**, si la bandera no lo cubriera en alguna: un
   símbolo, veinte unidades.
3. Las cuatro de «nos falta un símbolo», con el alias `__asm__`.
4. `zGameModes`, que es la más barata: 124 B de código y dos secciones de más.

## Medida 5 (r49) — `.bss`, la última sección sin medir: no falta NADA

El censo midió código, `.rodata` y `.data`. Faltaba `.bss`, que no viaja en el
DOL pero **decide las direcciones que resuelven las reubicaciones** —y por eso
rompió `sserver` y `ssysinit` en la r45—. Los deltas asustaban: `zFeOverlay`
4.072 B **corta**, `zMain` +552, `zMisc` +288, `zEAXSound` +288.

Clasificado por familia y emparejando **por nombre base** —quitando tanto el
contador de declaraciones (`.NNNNN`) como el sufijo de dirección que el
troceador añade a los locales homónimos (`_80456560`)—:

| familia | B |
|---|---:|
| mismo símbolo con otro contador de sufijo | 5.000 |
| relleno `gap_*` / `pad_*` del troceador | 4.631 |
| temporales `_.tmp_N.M` del compilador | 116 |
| **dato REAL que falta** | **0** |
| dato REAL que sobra | 701 |

**No falta ni un byte de `.bss` en ninguna de las 28 unidades.** Y los 4.072 B
que parecían faltarle a `zFeOverlay` son **4.104 B de etiquetas `gap_`**: no es
dato, es el relleno que el troceador nombra.

### Los 701 B que sobran

256 son **una sola tabla**: `g_StreamTable`, un
`static STREAMHEADERtag *g_StreamTable[64]` de `realstream/src/stream.cpp:140`,
en `zEAXSound`. Y **no existe en ninguna parte del ELF original** —aunque
nuestro código, que casa al 100 %, la usa en la línea 175—. O es un dato huérfano
que el troceador no atribuyó, o el original la tenía en otra forma. Es la única
pista de datos concreta que queda, y cae en la unidad más valiosa del censo
(151.092 B).

El resto son ~110 estáticos de 4 B llamados `value` y `upper`: nombres genéricos
cuyo emparejamiento por base falla porque el número de apariciones difiere entre
los dos lados. Son la misma familia del contador, no dato nuevo.

### El frente de datos, cerrado del todo

Sumando las tres secciones:

| | falta de verdad |
|---|---:|
| `.rodata` + `.data` + `.sdata` | 4 B (ya arreglados: un `= 0`) |
| `.bss` + `.sbss` | **0 B** |

**El «déficit de datos» del frente de `linked` era enteramente nomenclatura.**
Lo único que queda del lado de los datos son los 256 B de `g_StreamTable`, que
sobran, no faltan.

---

# CORRECCION (r49): el censo media los OBJETOS, y el frente esta en el ENLACE

Todo lo de arriba compara las secciones de **los dos objetos**. Eso engana, y la
herramienta que lo dice **ya estaba en el arbol antes de esta sesion**:
`scripts/linkdelta.py`, cuyo docstring abre con

> «`promodist.py` y `datacmp.py` comparan los tamanos de seccion de los OBJETOS,
> y eso enganya: el enlazador lleva `-strip-unused-data` y **se lleva todo lo que
> no se referencia, codigo incluido**.»

No la busque. Lo que sigue lo midio el agente de `linked` de la r49, y hay que
leerlo **en lugar de** las conclusiones de arriba, no adem&aacute;s.

## Lo que cae

**1. El frente de los simbolos de mas NO EXISTE.** `linkdelta.py` da el `.text`
a **delta CERO en 20 de 23 unidades**: el enlazador se lleva todo lo que no se
referencia. Los 163.096 B de la «familia B» no bloquean nada.

**2. `_M_erase` no bloquea veinte unidades: no bloquea ninguna.** No esta en el
ELF original **ni en el nuestro**. Comprobado.

**3. `-fno-implicit-templates` no es la palanca.** Barrida sobre las 13
SourceLists con el codigo al 100 %: el criterio de seguridad falla en **12 de
13**, y no sobre el papel --el enlace falla de verdad con simbolos indefinidos--.
Y en la unica segura, `zFeOverlay`, **no compra nada**: los dos objetos, con y
sin la bandera, dan **el mismo DOL**. 5.880 B de diferencia en el objeto, cero
en el enlace.

**4. `promopred.py` mide algo que no es el bloqueo.** `zBWare` no era un fallo
del predicado sino del modelo: sus 8 simbolos «vivos» **no estan en el ELF
enlazado**. La prueba de «referenciado desde dentro» no es transitiva desde
raices vivas, asi que una vtable muerta que apunta a un destructor muerto marca
vivos a los dos.

## Lo que queda en pie

Las medidas de **datos** siguen valiendo, porque son sobre simbolos con nombre y
no sobre tamanos de seccion: el «deficit» era nomenclatura --contadores de
sufijo, literales renombrados y relleno del troceador-- y lo que faltaba de
verdad eran 4 B en `.data` y **0 B en `.bss`**.

## El frente de verdad: `.rodata` VIVA en el enlace

`linkdelta.py`, unidad por unidad:

| unidad | `.text` | resto |
|---|---:|---|
| `zSim` | +0 | **IGUAL** |
| `zGameModes` | +0 | `rodata+104` |
| `zEcstasy` | +0 | `rodata-288 data+64 bss+32` |
| `zSpeech` | +0 | `rodata-624` |
| `zPhysics` | +0 | `rodata-784 bss-32` |
| ... | | |
| `zFe2` | +0 | `rodata-4216 data-160 over-16` |
| `zFeOverlay` | +0 | `bss-4096 sbss+4 over-1136` |

**Veintidos unidades tienen un deficit de `.rodata` viva de 288 a 4.216 B**, y no
es el relleno `gap_`/`pad_` --medido aparte: 0 a 908 B contra esos deficits--.
Es el unico frente que queda.

**`zSim` es la unidad mas cerca del arbol**: la unica con las secciones
enlazadas IDENTICAS a la base. Su bloqueo es de ORDEN, no de codigo --el pool de
literales permuta desde `0x80404868`, y su deriva acumulada es el `+8` que mueve
72 vtables--. Origen localizado: `"FEngHUD"` se emite **dos veces**, el `$LC481`
de GCC mas el `asm` a mano de `LocalPlayer.cpp:118`.

## La leccion

Antes de construir un modelo, **buscar si la medida ya existe**. `linkdelta.py`
llevaba dos dias en `scripts/` contestando la pregunta que yo reconstrui mal
desde cero, y su primer parrafo advierte del error exacto que cometi.
