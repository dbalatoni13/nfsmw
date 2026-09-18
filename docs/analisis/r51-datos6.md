# r51 · `datos6` — las seis unidades con el código al 100 % y un dato de bloqueo

Encargo: las seis unidades de biblioteca que ya tenían el código al 100 % y no
enlazaban por un dato pequeño. **Cinco cierran** (`trypromo` dice DOL OK, cada
una por su cuenta y las cinco juntas); **`inittmr` no**, y su bloqueo está
medido y acotado a una sola cosa.

| unidad | ruta para `promote`/`trypromo` | `.text` | estado |
|---|---|---:|---|
| `libgcc2_4` | `libc/libgcc2_4` | 176 B | **DOL OK** |
| `pathnode` | `Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode` | 4.204 B | **DOL OK** |
| `sserver` | `Speed/Indep/Libs/snd/9/source/library/cmn/sserver` | 992 B | **DOL OK** |
| `ssysinit` | `Speed/Indep/Libs/snd/9/source/library/cmn/ssysinit` | 1.668 B | **DOL OK** |
| `interface` | `Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface` | 204 B | **DOL OK** |
| `inittmr` | `Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr` | 472 B | **NO**, sólo por `rodata-144` |

`fncmp` de las seis, antes y después: **0 funciones con el código distinto**. No
he empeorado ninguna.

---

## 0. Lo que resultó ser el diagnóstico de verdad

`promote.py` decía «secciones que emitimos de más: `.rodata`». **Es al revés en
las tres unidades de `.rodata`**: nosotros emitimos un pool de literales que el
objeto extraído **no tiene**, porque en el original ese pool quedó fuera del
rango de la unidad en `splits.txt` y el troceador lo metió en un hueco `auto_*`.
El objeto extraído lo referencia como símbolo externo (`lbl_8040FE28`,
`lbl_80413A48`, `lbl_80413EB8`…).

De ahí sale el patrón que resuelve dos de las tres:

> Al promocionar, nuestro objeto deja de referenciar el hueco. Nadie más lo
> referencia, `-strip-unused-data` se lleva el hueco **entero** —incluido lo que
> el troceador fundió dentro del mismo símbolo— y el `.rodata` enlazado sale
> corto. Si el hueco muere **completo** y nada emite `.rodata` entre su posición
> de enlace y la nuestra, basta con **emitir nosotros esos bytes**: caen en la
> misma dirección.

Los tres números que lo demuestran (`linkdelta.py`, o sea medido en el ENLACE,
no comparando tamaños de sección de objetos):

| unidad | hueco | vivo para nosotros | muerto que arrastra | delta antes |
|---|---|---:|---:|---:|
| `libgcc2_4` | `auto_05_8040FE28_rodata` (168 B) | 24 B | 144 B (cadena de ProDG) | `rodata-144` |
| `pathnode` | `auto_05_80413A48_rodata` (40 B) | 32 B | 8 B (`"%s  %s"`) | `rodata-8` |
| `inittmr` | `auto_05_80413E30_rodata` (**1.384 B**) | 24 B | 144 B | `rodata-144` |

La diferencia entre las dos primeras y la tercera es que en `inittmr` el hueco
**tiene dueños vivos delante y detrás**, así que los 168 B se estripan de EN
MEDIO y todo lo posterior sube. Eso es lo que la bloquea (§6).

---

## 1. `libgcc2_4` — 144 B de una cadena muerta de ProDG · **DOL OK**

**Qué la bloqueaba.** `__floatdisf` necesita tres constantes
(`0x4330000080000000`, `65536.0`, `0x4330000000000000`). El original las tiene en
`auto_05_8040FE28_rodata`, un hueco de 168 B cuyo tercer símbolo, `lbl_8040FE38`,
mide 0x98: sus primeros 8 B son la constante y los **144 restantes son una cadena
del runtime de ProDG** (`"\n*** Library error ***\nException handling requires
access to heap memory\nHook _register_malloc to your malloc rountine - see ProDG
manual.\n"`) que no referencia nadie en todo el DOL.

Al promocionar, el enlazador se lleva los 168 B y nosotros ponemos 24:
`rodata-144`, exactamente lo que medía `linkdelta`.

**Qué he cambiado.** `src/libc/libgcc2_4.c`: un `asm()` de fichero, detrás de la
función, que emite esos 144 B en `.rodata`. El pool `.LC0/.LC1/.LC2` sale antes
que la función en el `-S`, así que el bloque cae detrás y el `.rodata` de la
unidad queda en 168 B con el contenido exacto del original.

Posición: `auto_05_8040FE28_rodata.o` está en el puesto 102 del enlace y
`libgcc2_4.o` en el 112, y ningún objeto intermedio emite `.rodata`; por eso
nuestros 168 B caen en 0x8040FE28.

**Prueba.**

    python scripts/linkdelta.py --all libgcc2_4   ->  .text +0   IGUAL
    python scripts/trypromo.py libc/libgcc2_4     ->  DOL OK

---

## 2. `pathnode` — el mismo patrón, 8 B · **DOL OK**

**Qué la bloqueaba.** Sus cuatro dobles del pool (`$LC0..$LC3`, 32 B) están en
`auto_05_80413A48_rodata`, un hueco de 40 B. Los 8 últimos —`"%s  %s"` y su
relleno— los fundió el troceador dentro de `lbl_80413A60`, que declara 0x10 B.
Promocionar mata los 40 y pone 32: `rodata-8`.

**Qué he cambiado.** `pathnode.cpp`: un `asm()` de fichero con esos 8 B.

**Prueba.**

    python scripts/linkdelta.py --all pathnode  ->  .text +0   IGUAL
    python scripts/trypromo.py Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode -> DOL OK

---

## 3. `sserver` — **la veda de la r46 está rota** · **DOL OK**

**Qué la bloqueaba.** Sólo el orden de dos símbolos de `.bss`: el objetivo pone
lista(+0), tick(+4), periodo(+8) y nosotros lista, periodo, tick. `dolwhere.py`
lo cuantifica: **20 B de DOL distintos**, todos el medio byte bajo (`0x94`
contra `0x98`) de las referencias `@ha/@l` desde `saems`, `sstautov`, `snddrv` y
`sndplatform`.

El fichero llevaba una **veda de la r46 con 18 formas medidas** que concluía «el
orden del `.bss` no se puede cambiar desde la fuente». **Es falsa**, y falsa por
la razón que dice la regla 10 del brief: se probaron N formas de la sentencia
equivocada.

**La causa real.** El volcado final de `.lcomm` no sale en el orden de las
DEFINICIONES: sale en el orden de la **cadena del espacio de nombres**, y esa la
fija la **primera declaración** de cada nombre. `sndcmn.h:362` ya declara
`gVariableTimerPeriod` mucho antes de que `sserver.c` abra su bloque
`namespace Snd`. Por eso las seis permutaciones de las definiciones —y un
`extern` puesto DESPUÉS del `#include`, que es lo que probó la r46— salían
idénticas.

**Qué he cambiado.** Nueve líneas al principio de `sserver.c`, **antes** del
`#include`:

    namespace Snd {
    extern int gVariableTimerTick;
    };

El `.lcomm` pasa de `lista, periodo, tick` a `lista, tick, periodo`. He
sustituido el comentario de la veda por la explicación correcta.

No toco `sndcmn.h`: declararlo allí también valdría (y probablemente es lo que
hacía el original), pero es cabecera compartida y con el `extern` local basta.

**Prueba.**

    python scripts/trypromo.py Speed/Indep/Libs/snd/9/source/library/cmn/sserver -> DOL OK

---

## 4. `ssysinit` — orden de `.bss` + 4 B que no pueden ir sueltos · **DOL OK**

**Qué la bloqueaba.** Tres cosas:

1. **Orden**: el objetivo pone `gSpeakerPositions`(+0, 20 B),
   `gTotalOutputChannels`(+20, 1 B), relleno, `gFoldDownTarget`(+24, 4 B).
   Nosotros sacábamos canales en +1 y altavoces en +4, por lo mismo que
   `sserver`: `sndcmn.h:367` declara `gTotalOutputChannels` antes que
   `gSpeakerPositions` (línea 368).
2. **Un `freekey` que no existe**: `SNDSYS_getopts` tenía un
   `static unsigned char freekey;` que no usa nadie y que, por ser estática de
   función, se emite ANTES de todo (en el `-S` sale en la línea 5838, contra las
   9300 de las de espacio de nombres) y se comía el `.bss+0`. En el original no
   puede estar ahí: no hay forma de que una estática de función salga detrás del
   volcado final. La he quitado; el código no cambia porque no se usaba.
3. **Los 4 B de cola** (0x804BED84..0x804BED88, `gap_07_804BED84_bss`). Nadie
   los referencia, pero **tienen que existir**: el `.bss` de `sdspmix` —el
   siguiente y ya promocionado— sale con alineación 4 (el extraído la tiene a
   8), así que con 28 B aquí arranca 4 B antes y se desplazan sus 5.636 B
   enteros (22 B de DOL distintos, medido con `dolwhere`).

**Lo que NO funciona para el punto 3, medido**: una variable suelta al final del
espacio de nombres, con enlace interno (`static int`) **y** con enlace externo
(`int`). En los dos casos `-strip-unused-data` se la lleva y el DOL vuelve a
romper con el mismo sha1 (`bccb24d98624`). Con una lista `keep` temporal que
añade ese símbolo, **DOL OK** — o sea que el mecanismo es el correcto y lo único
que falla es que el símbolo esté suelto.

**Qué he cambiado.** Los 4 B van DENTRO de un símbolo vivo:

    struct FoldDownTargetCell { FoldDownTarget value; int tail; };
    FoldDownTargetCell gFoldDownTargetCell asm("_3Snd.gFoldDownTarget");

El nombre de ensamblador es el que declara `sndcmn.h`, así que `s3dlow`,
`satospkr`, `sdownmix`, `spat2hdr`, `sst` y `sst3dpos` —que sólo lo leen en el
desplazamiento 0— siguen resolviendo. Más el `extern float gSpeakerPositions[5];`
antes del `#include`, como en `sserver`.

**Prueba.**

    python scripts/trypromo.py Speed/Indep/Libs/snd/9/source/library/cmn/ssysinit -> DOL OK

**Alternativa más fiel, para cuando toque**: subir a 8 la alineación del `.bss`
de `sdspmix` y dejar esta unidad en 28 B. Es DOL-neutro por separado
(0x804BED68+32 ya está alineado a 8), pero toca una unidad promocionada que no
es de este encargo.

---

## 5. `interface` — los `#pragma` sacaban tres vtables de más · **DOL OK**

**Qué la bloqueaba.** Dos cosas, y **ninguna era los 10 símbolos de más** que
señalaba `promote.py`… aunque tampoco eran gratis, que es la trampa: en
`zAttribSys` y `zBWare` los símbolos sobrantes costaban cero **porque nadie los
referencia**, y aquí las vtables de `GcDevice`, `DeviceImp`, `GcPad` y
`GcInterface` —de otras unidades— **sí** apuntan a `GetData__Q29RealInput6Device`
y compañía, así que nuestras definiciones ganaban a las buenas y `dolwhere` daba
**15.206 B distintos**.

1. `interface.cpp` llevaba `#pragma implementation "input.h"` e `input.h` un
   `#pragma interface`. Con la pareja, GCC 2.95 emite en esta unidad **las cuatro
   vtables de la cabecera** (Interface, Event, Device, Effect: 256 B de `.rodata`
   contra los 80 del objetivo) y además los **siete cuerpos de las virtuales en
   clase de `Device`**, que empujan `_._Q29RealInput9Interface` de `.text+152` a
   `+208`. En el original las tres vtables sobrantes viven en el hueco
   `auto_05_80414CE0_rodata` y los métodos de `Device` en `device.cpp`.
   (Es exactamente lo de la memoria `nfsmw-pragma-interface-vtables`.)
2. `.sdata`: el objetivo son 8 B —`_9RealInput.gInterface`(+0, 4 B) y
   `gap_08_804FF6B4_sdata`(+4, 4 B)— y nosotros dábamos 4. `memvectors`, el
   siguiente en `.sdata` y ya promocionado, sale con alineación 4, así que
   arrancaba 4 B antes y arrastraba a `gc_interface`: `sdata-8` en `linkdelta`.

**Qué he cambiado.**

- Guarda en `input.h`:
  `#ifndef REALINPUT_INPUT_IMPLEMENTATION` / `#pragma interface "input.h"` /
  `#endif`, y en `interface.cpp` el `#pragma implementation` pasa a ser
  `#define REALINPUT_INPUT_IMPLEMENTATION`. Sin la pareja manda la regla normal
  del método clave: `Interface::AddRef` es la primera virtual no en clase y se
  define aquí, así que sale su vtable y **sólo** la suya. `.text` 204, `.rodata`
  80 y el destructor en +152: idénticos al objetivo.
- Los 4 B de `.sdata`, con la misma celda que en `ssysinit`:
  `struct InterfaceSlot { Interface *mpInterface; int mTail; };`
  `InterfaceSlot gInterfaceSlot asm("_9RealInput.gInterface") = {nullptr, 0};`

**Radio de la cabecera, medido** (regla 12: el control tiene que cambiar).
Reconstruidas las 14 unidades de `realcore/.../source/input` con y sin la guarda,
comparando el sha1 de **las secciones que llegan al enlace más la tabla de
símbolos**: cambia **una sola unidad, `interface`**, que es el control. Con el
sha1 del fichero entero cambiaba también `interfaceimp`, pero sólo por `.line` y
`.debug`: la guarda añade dos líneas a la cabecera y corre la numeración. El
enlazador no se lleva esas secciones.

**Prueba.**

    python scripts/linkdelta.py --all interface   ->  .text +0   IGUAL
    python scripts/trypromo.py Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface -> DOL OK

---

## 6. `inittmr` — el negativo, acotado a una sola cosa

Entró con cuatro síntomas y **sale con uno**. Lo que he arreglado y he dejado
puesto (el código sigue al 100 %, `0 de 4 funciones distintas`):

| síntoma de `promote.py` | estado |
|---|---|
| `.sbss(4B)` de más + «exportamos `TIMERhz`» | **arreglado**: quien lo define es `systemvars.cpp` (`.sdata` 0x804FF644); aquí sobra la definición. Ahora es `extern`. |
| `.sdata` 4 B contra 8, `bIsTimerInited` en +0 contra +1 | **arreglado** (abajo) |
| `.rodata(24B)` | **NO**: es el bloqueo |

`linkdelta.py` pasa de `rodata-144 sbss+8` a **`rodata-144`** y nada más.

### 6.1 El `.sdata`, y una técnica que sirve para más sitios

El objetivo son 8 B con la bandera en el **+1**: 1 B delante
(`pad_08_804FF628_sdata`) y 6 detrás (`gap_08_804FF62A_sdata`), ninguno
referenciado. Con la bandera sola en el +0, las cinco referencias `@sda21` de la
unidad apuntan un byte antes.

Lo probado y **descartado**: meter la bandera como campo de una `struct` viva de
8 B. Funciona para el tamaño pero **cuesta código**: GCC no pliega el sumando en
la instrucción y emite `li r9, sdataCell+0x1@sda21` + `lbz r0, 0x0(r9)` en vez de
`lbz r0, bIsTimerInited@sda21` — +4 B en `TIMER_init` y +4 en `TIMER_restore`.

Lo que sí funciona: **bytes sin símbolo**, con `asm()` de fichero alrededor de la
variable, aprovechando que una estática de fichero **con inicializador** se emite
en el punto de su declaración:

    asm(".section \".sdata\"\n .byte 0\n");
    static unsigned char bIsTimerInited = 0;
    asm(".section \".sdata\"\n .byte 0,0,0,0,0,0\n");

**Bytes que no llevan símbolo el enlazador no los puede estripar**, y la bandera
conserva su propio símbolo y su `@sda21`. `.sdata` 8 B, bandera en +1, código
intacto. *(Esta es la palanca que le faltaba a `ssysinit` en `.bss`; allí no
sirve porque el `.bss` sale diferido y el `asm()` no controla su posición.)*

### 6.2 Por qué el `.rodata` no se puede cerrar como los otros dos

Las tres constantes (`0x4330000000000000`, `0x4330000080000000`, `1.0f`) están en
`auto_05_80413E30_rodata`, un hueco de **1.384 B** cuyo resto —la vtable de
`GcHdFileDeviceDriver`, `lbl_80413F60` (`"D:/env/…/shpcreate.cpp"`, que usa
`creates.cpp`), `gTexelTypeToBpp`, `lbl_80414110`— **lo referencian otras
unidades y sobrevive**. Nuestros 168 B se estripan de EN MEDIO (offset 0x88 del
hueco) y todo lo posterior sube 168 B.

Lo medido, forma por forma:

| ensayo | resultado |
|---|---|
| emitir los 144 B muertos con `asm()`, como en `libgcc2_4` | `.rodata` total correcto pero **descolocado**: `dolwhere` da **1.252 B** distintos. Nuestro bloque va al puesto 491 del enlace y el hueco está en el 489, con dueños vivos delante y detrás |
| `keep.lst` temporal con `lbl_80413EB8`+`EC0`+`EC8`, código original | **DOL ROTO**: el hueco queda entero pero nuestro pool de 24 B sigue sobrando. *(Control: la misma lista con `libgcc2_4` da DOL OK, o sea que la lista no rompe nada por sí sola.)* |
| referenciar los tres símbolos desde C (`extern const double lbl_… asm("lbl_…")` + las dos conversiones entero→double escritas a mano) | **el `.rodata` desaparece y las reubicaciones son las buenas**, pero `TIMER_init` sale de 260 B contra 268: marco 0x38 contra 0x30 y dos `mr` de menos |
| lo mismo + `fctiwz` por `asm` para compartir la ranura de pila | marco **0x30 correcto**, sigue en 260 B: el planificador adelanta `bl ttInit__Fv` y obliga a usar dos preservados más |

**El bloqueo real, en una frase**: para que `inittmr` enlace, su objeto no puede
emitir pool propio, y para no emitirlo hay que escribir a mano dos conversiones
entero→double que el generador de código hace solo — y hoy eso cuesta 8 B de
`.text`.

**Dónde seguir, por orden de coste:**

1. **Codegen** (2 instrucciones y el reparto de registros): es la vía limpia y
   deja la unidad cerrada sin tocar nada de `config/`. Está a dos pasos: el marco
   ya sale a 0x30 con el `asm` del `fctiwz`; falta que el planificador no adelante
   `bl ttInit__Fv`. Frente para un agente de codegen, no de datos.
2. **`splits.txt` + orden de enlace**: darle a `inittmr.cpp` el rango
   `.rodata 0x80413EB8..0x80413ED0` parte el hueco en dos `auto_*` y **exige**
   que `inittmr.o` quede entre los dos en la lista de objetos. Hoy los `auto_*`
   van por dirección y el nuevo caería en el puesto 490, delante de `inittmr.o`
   (491): haría falta tocar el orden en `configure.py`, no sólo `splits.txt`.
   Coincide con lo que dice la memoria `nfsmw-rango-no-basta`.

---

## 7. Lo que hay que aplicar

Cinco promociones. **Ninguna es atómica con otra**: cada una da DOL OK por su
cuenta y las cinco juntas también (comprobado en las dos formas). Se pueden
meter sueltas o de golpe.

En `configure.py`, pasar a `Matching`:

    libc/libgcc2_4.c
    Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp
    Speed/Indep/Libs/snd/9/source/library/cmn/sserver.c
    Speed/Indep/Libs/snd/9/source/library/cmn/ssysinit.c
    Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface.cpp

`linked` 506 → **511** sobre un techo de 544.

**No hace falta nada de `config/`**: ni `keep.lst`, ni `splits.txt`, ni orden de
enlace. Los ficheros tocados son sólo fuente:

    src/libc/libgcc2_4.c
    src/Speed/Indep/Libs/path/5.01.04/source/cmn/pathnode.cpp
    src/Speed/Indep/Libs/snd/9/source/library/cmn/sserver.c
    src/Speed/Indep/Libs/snd/9/source/library/cmn/ssysinit.c
    src/Speed/Indep/Libs/realcore/6.24.00/source/input/cmn/interface.cpp
    src/Speed/Indep/Libs/realcore/6.24.00/include/common/realcore/input.h   (guarda, radio medido = 0)
    src/Speed/Indep/Libs/realcore/6.24.00/source/system/gc/inittmr.cpp      (mejora parcial, NO promociona)

`inittmr.cpp` queda mejor que como estaba (código al 100 %, `.sbss` y `.sdata`
correctos, un solo delta) pero **no se promociona**.

---

## 8. Para la memoria del proyecto

1. **El pool que vive en un hueco.** Cuando `promote.py` diga «`.rodata` de más»
   en una unidad al 100 %, mira si el objeto EXTRAÍDO referencia `lbl_*`: casi
   seguro es su propio pool, que `splits.txt` no le dio. Si el hueco muere entero
   y nada emite `.rodata` entre su puesto de enlace y el tuyo, **emítelo tú con
   un `asm()` de fichero**, con la cola muerta incluida. Si el hueco tiene dueños
   vivos, esa vía está cerrada.
2. **El orden del `.bss`/`.lcomm` SÍ tiene mando** (veda de la r46 rota): el
   volcado final va en el orden de la cadena del espacio de nombres, y la fija la
   **primera declaración**, que casi siempre está en una cabecera. La palanca es
   un `extern` **antes** del `#include`. Un `extern` después no hace nada — que
   es lo que se probó y por lo que salió la veda.
3. **Los bytes de relleno van sin símbolo.** Un símbolo de relleno sin
   referencias se lo lleva `-strip-unused-data` (probado con enlace interno y
   externo). Sirven dos formas: bytes anónimos por `asm()` cuando la sección es
   `.sdata`/`.rodata` y la posición se controla, o meterlos DENTRO de un símbolo
   vivo con `asm("nombre_del_objetivo")` cuando no.
4. **«Los símbolos de más cuestan cero» tiene letra pequeña.** Sólo si nadie los
   referencia. En `interface` diez símbolos de más costaban **15.206 B de DOL**
   porque las vtables de otras unidades apuntaban a ellos. `refs.py` antes de dar
   por gratis un símbolo sobrante.
