# r51 · zSim: DOL IDÉNTICO

**Resultado: `trypromo Speed/Indep/SourceLists/zSim` → DOL OK.** El DOL enlazado
con nuestro `zSim.o` es **byte a byte igual** al original. `fncmp` sigue en
**0 de 402** funciones distintas: no se ha tocado una sola función.

Requiere **una línea de coordinación fuera de mi territorio**: `keep.lst` tiene
dos entradas `$LC` rancias. Ver §6.

    antes:   dolwhere zSim = 4.688 B   (2.172 bytes distintos exactos)
    después: 0 B, DOL IDENTICO  <- con las dos lineas de keep.lst corregidas (§6)
             74.701 B           <- con el keep.lst del arbol TAL CUAL, que quedo
                                   rancio por mi cambio y se arregla con lcfix

---

## 1. El diagnóstico: la permutación de vtables NO era el problema

`vtord.py` da 15 posiciones que no casan de 79, y **ninguna cuesta un byte**.
Son las **cinco vtables de más** que emitimos y el objetivo no
(`13IVehicleCache`, `6IModel`, `Q214EventSequencer8IContext`, `11IAttachable`,
`Q33UTL3COM8IUnknown`, 448 B). Están **muertas**: `-strip-unused-data` se las
lleva enteras y el enlace no las ve. Las 64 posiciones anteriores —todo el
bloque que sí enlaza— están en su sitio y en su orden.

Lo que rompía el DOL era que **el bloque entero de vtables estaba 8 B más
adelante que el del objetivo**. Los 226 B de `.text` (113 sitios, todos `d8`→`e0`)
eran la consecuencia: mitades bajas de `@l` apuntando 8 B más allá.

La herramienta que lo destapa no es `vtord` ni `permorden` (que da 402 de 402 en
su sitio): es **alinear la `.rodata` de zSim del ENLACE contra el DOL original y
sacar el script de edición**. En el estado inicial salían siete ediciones, y
**sumaban cero**: por eso el DOL sólo se diferenciaba en 2.172 B y no en 300 kB.

    =  584 palabras                             desfase  +0
    +  4 B  3c23d70a  /  - 4 B 3c23d70a         desfase  +0   <- 0.01f y 0.0f cambiadas
    -  8 B  "MNISComp"                          desfase  -8
    +  4 B  3f800000  (lbl_80404864 duplicado)  desfase  -4
    - 16 B  "MNotifyMovieFini"                  desfase -20
    + 24 B  "CopHandle"+"Param"+"NISActivity"   desfase  +4
    - 12 B  "NISActivity"                       desfase  -8
    + 16 B  ceros de relleno                    desfase  +8   <- AQUI se van las vtables
    -  8 B  3f800000 00000000                   desfase  +0   <- y aqui vuelve

El `+8` del relleno y el `-8` del final **se compensaban**, y por eso la unidad
parecía «casi bien» midiendo tamaños de sección. No lo estaba: entre esos dos
puntos hay 4.176 B de vtables desplazadas.

## 2. Qué construcción de fuente decide cada cosa

No es `cp/class.c`: para este frente la función que manda es
**`varasm.c:3797 output_constant_pool`**, y su ayudante
**`varasm.c:3891 mark_constant_pool`**.

* `output_constant_pool` recorre `first_pool → next`, o sea escribe las entradas
  del pool **en orden de CREACIÓN** (`force_const_mem`, `varasm.c:3585`), que es
  el orden de expansión a RTL, que es el orden de las SENTENCIAS. No el orden en
  que las instrucciones acaban usándolas.
* `mark_constant_pool` **descarta** las entradas que ningún insn superviviente
  referencia. Corolario importante: *no* se puede fabricar una entrada de pool
  con una sentencia muerta; la entrada tiene que estar referenciada por algún
  insn vivo, aunque sea otro y aunque esté más abajo.

De `cp/decl2.c` sólo hace falta un hecho, y es el que decide dónde se puede
escribir a mano: **`finish_file` (decl2.c:3643) corre DESPUÉS de todo el
fichero**, así que un `asm()` de ámbito de fichero se emite en la primera pasada
y **siempre aterriza delante de las vtables**. Para meter bytes en el pool que
sale *entre* las vtables de la primera vuelta y las de plantilla hay que usar un
`asm()` **dentro de una función que se emita en la segunda vuelta** (§3.6).

## 3. Lo que he movido — ocho cambios, cuatro ficheros

Todos en territorio zSim. Ninguno toca una función.

### 3.1 `NISActivity.cpp` · prefijo «MNISComp» (+8 B)

`$LC` de `"MNISComplete"` mide 13 B, está muerta, y `-strip-unused-data` se lleva
`size & ~7` = 8 B **desde la cabeza**: sólo sobrevivía `"lete\0"`. Mismo
mecanismo (y misma receta) que el `"MNotifyS"` de la r36d en
`GameplayActivity.cpp`: ocho bytes crudos **delante del `#include`**, porque
cc1plus emite el literal al parsear la cabecera.

### 3.2 `NISActivity.cpp` · prefijo «MNotifyMovieFini» (+16 B)

Idéntico: el `$LC` mide 21 B, `21 & ~7` = 16 estripados, sobrevivía `"shed\0"`.

### 3.3 `zSim.cpp` · `#define SOUNDAI_H_FWD_MUNSPAWNCOP` (−12 B)

`SoundAI.h` entra por `NISActivity.cpp:126` y sólo usa `MUnspawnCop` **por
referencia**. Arrastrarla entera hacía que `ZMAIN_MESSAGES_LUA_INLINE` emitiera
los literales de su `BuildMessageTable`: `"CopHandle"` (4 B tras el estripado) y
`"Param"` (8 B). El objeto original **no los tiene** —buscados en su `.rodata`,
`"CopHandle"`, `"Param\0"` y `"MUnspawnCop"` no aparecen—. La guarda ya existía
y sólo la usaba `zMain.cpp`; el comentario de `SoundAI.h:12` ya decía que el
objetivo parsea ese mensaje el último, en `EventDefs.cpp`.

### 3.4 `NISActivity.cpp` · «IntroNisBL09» delante de «NISActivity» (0 B)

El objetivo interna `"IntroNisBL09"` justo detrás de `"Attrib::Gen::speechtune"`
y **delante** de `"NISActivity"` (que sale del `return "NISActivity"` de
`GetName`). Nosotros lo internábamos en la definición de
`NisNamesToDisablePreculler`, al final del fichero, y las dos cadenas salían
cambiadas de sitio: 26 B de `.rodata` y el puntero de la tabla apuntando 4 B más
allá (ese era el único byte de `.data` del diff).

Adelantarlo sin mover la tabla de `.data`: una `static inline` muerta que
devuelve la cadena, escrita en el punto del orden de parseo. cc1plus emite el
literal **al parsearlo** aunque la inline no llegue a emitirse, y la definición
de más abajo reutiliza el mismo `$LC` por ser el literal idéntico.

### 3.5 `zSim.cpp` · fuera el relleno de 12 B del final (−16 B)

Es **la palanca grande**: 4.176 B de vtables y los 226 B de `.text`.

Entre el último literal y la primera vtable el objetivo tiene **8 B a cero**.
Nosotros teníamos 24: el `$LC` de cierre del pool de `GameplayActivity`, el
relleno del `.balign 8`, las tres palabras que escribía la r36c-e5 y el relleno
de alineación de la vtable. El bloque de la r36c/r36d **sobraba entero**, no en
16 B como decía su comentario.

### 3.6 `LocalPlayer.hpp` · 8 B en el SEGUNDO pool (+8 B)

El objetivo tiene ocho palabras en el pool que sale entre las vtables de la
primera vuelta y las de plantilla —`1.0, 0, 0, 1.0, 0, 1.0, 0, 0`— y nosotros
seis. Rastreando las reubicaciones de los dos objetos:

| palabras | objetivo | nuestro |
|---|---|---|
| pool de `__static_initialization_and_destruction_0` | `1.0` | `1.0` |
| en medio | `0, 0, 1.0` (sin ninguna reubicación que las nombre) | `0` (de `Default__Q37Physics4Info11Performance`) |
| pool de `ChargeGameBreaker__11LocalPlayerf` | `0, 1.0` | `0, 1.0` |
| pool de `GetTimeStart__16CAnimMomentScene` | `0` | `0` |

Faltaban `0, 1.0` entre el pool de `Default` y el de `ChargeGameBreaker`. Un
`asm()` de fichero no sirve (§2). La solución es un `asm()` **dentro de
`InGameBreaker()`**, que es la última función que se emite antes de
`ChargeGameBreaker`: emite cero bytes de `.text`, y sus 8 B de `.rodata` caen
exactamente en el hueco. Sin `.size`, así que sobreviven al estripado y no hacen
falta en `keep.lst`. Va bajo `ZSIM_HAND_POOL`, o sea inerte para zMain, que es
el otro TU que ve esta cabecera.

### 3.7 `QuickGame.cpp` · `lbl_80404864` pasa a ser un ALIAS (−4 B)

**El compromiso de la r36c, resuelto sin pagarlo.** Había una *definición* a mano
`extern const float lbl_80404864 = 1.0f;` que duplicaba el 1.0f: cc1plus ya emite
uno (`$LC526`) en 0x80404864, justo donde el objetivo lo tiene, y la copia caía
4 B más adelante y desplazaba el bloque de cadenas que sigue. Las dos salidas que
la r36c midió costaban una función cada una:

* dejar la definición → +4 B de `.rodata` mal colocada, 320 B desplazados;
* usar `= 1.0f` → `OnManageTime` 564 → **560 B**: con una constante de pool GCSE
  saca el `lis` del `@ha` fuera del `if` (`lis r29, $LC526@ha`) y comparte el
  registro entre las dos recargas de más abajo, donde el objetivo **recarga**.
  Medido otra vez hoy, con el pin `asm("fr13")` y la barrera `asm("" : "+f")` ya
  puestas: **sigue costando la función**. El andamio no había caducado.

La tercera salida es no definirlo, sino **declararlo alias** del literal que el
compilador ya puso ahí:

```c
asm(".globl lbl_80404864\n"
    ".set lbl_80404864, $LC526\n");
```

Cero bytes, y el código sale idéntico al objetivo.

### 3.8 `QuickGame.cpp` · la cantidad fantasma que ordena el pool (0 B)

El pool de `OnManageTime` del objetivo va `{1.0, 0.0, 0.01, 0.25, 2.0, 0.5}` y el
nuestro salía `{1.0, 0.01, 0.0, ...}`: por §2, la entrada de `0.0f` nace en el
`delta_speed = 0.0f`, que va **detrás** del `UMath::Max(target_speed, 0.01f)`.

Mover esa sentencia arregla el pool y **cambia 18 instrucciones** (medido: el
`stw` de `mInGameBreaker` y la carga de `0.0f` se quedan en el bloque anterior al
`bso`, y el asignador pasa de `f13` a `f12`). Las tres permutaciones de las tres
sentencias del bloque están medidas y ninguna casa. Declarar
`float delta_speed = 0.0f;` arriba es peor todavía: 137 instrucciones.

Lo que sí vale es **crear la entrada sin emitir código**:

```c
{
    float phantom;
    asm("" : "=f"(phantom) : "f"(1.0f), "f"(0.0f));
}
```

Un `asm` **no volátil** con la salida muerta: `force_const_mem` ya ha corrido
—las dos entradas quedan creadas, y en ese orden— y `flow` borra el insn entero,
cero instrucciones. Las entradas sobreviven a `mark_constant_pool` porque las
referencian el `return 1.0f` y el `delta_speed = 0.0f` de más abajo. Es la
palanca de `nfsmw-cantidad-fantasma`, usada aquí para **ordenar el pool** en vez
de para mover un reparto de registros.

*El `1.0f` va primero a propósito.* Con sólo `"f"(0.0f)` el 0.0f se adelanta al
1.0f, `$LC526` pasa a ser el 0.0f y el alias de §3.7 apunta a la constante
equivocada: el `.text` sale idéntico y el DOL rompe en 8 B. Es el caso de
`nfsmw-el-cero-que-miente` al revés.

## 4. Las medidas

| | `dolwhere` | bytes exactos | `.text` | `.rodata` | `.data` | `fncmp` |
|---|---:|---:|---:|---:|---:|---:|
| antes (r0) | 4.688 | 2.172 | 226 | 1.945 | 1 | 0/402 |
| §3.1+§3.2 (prefijos) | — | — | — | — | — | 0/402 |
| +§3.5 (fuera el relleno) | — | — | — | — | — | 0/402 |
| +§3.7 (alias) | — | **2.021** | 225 | 1.795 | 1 | 0/402 |
| +§3.3 (MUnspawnCop) | — | 44.908 | 3.707 | 40.850 | 351 | 0/402 |
| +§3.6 (pool 2) | — | **42** | 7 | 34 | 1 | 0/402 |
| +§3.4 (IntroNisBL09) | — | **13** | 5 | 8 | 0 | 0/402 |
| +§3.8 (fantasma) | **0** | **0** | 0 | 0 | 0 | **0/402** |

Los 44.908 B de la fila de en medio no son un retroceso de verdad: al quitar los
12 B de `"CopHandle"`/`"Param"` la `.rodata` de zSim se quedó 8 B **corta**, y
esos 8 B desplazan toda la `.rodata` de las unidades que van detrás. Es la misma
compensación de §1 vista desde el otro lado, y se cerró con §3.6.

`dolwhere` cuenta rangos, no bytes: da 4.688 donde los bytes distintos son 2.172.
Las dos cifras están arriba para que se puedan comparar con informes viejos.

Reparto del «antes» por tramos de la `.rodata` de zSim:

    prefijo             80403F48-80404900       8 B distintos de  2.488
    cola de literales   80404900-80404B70     526 B distintos de    624
    VTABLES             80404B70-80405BC0   1.411 B distintos de  4.176
    cola                80405BC0-80405E10       0 B distintos de    592

## 5. Verificación

    python scripts/fncmp.py Speed/Indep/SourceLists/zSim
      -> 0 de 402 funciones con el CODIGO distinto

    python scripts/trypromo.py --ldflags "-strip-unused-data -keep <keep corregido>" \
           Speed/Indep/SourceLists/zSim
      -> DOL OK

Comprobado **sin overlays**: durante parte de la sesión `EventSequencer.h` estuvo
roto por otro agente y usé una copia de HEAD por `-I`; la medida final es con el
árbol tal cual, ya arreglado.

Inercia para otras unidades, comprobada:

* `ZSIM_HAND_POOL` sólo lo define `zSim.cpp`, así que el `asm` de
  `LocalPlayer.hpp` (que zMain también ve, por `EPursuitBreaker.cpp`) no existe
  fuera de zSim.
* `SOUNDAI_H_FWD_MUNSPAWNCOP` ya lo definía `zMain.cpp`; ningún otro TU cambia.
* `NISActivity.cpp` y `QuickGame.cpp` sólo los compila `zSim.cpp`.

## 6. Lo que hace falta de fuera (coordinación)

**1 · `keep.lst`, dos líneas.** Quitar `"CopHandle"`/`"Param"` cambia la
numeración de los `$LC` de zSim, y las dos entradas que salvan `"SimTime"` y
`"TimeStep"` quedan rancias. Lo arregla la herramienta del proyecto:

    python scripts/lcfix.py zSim
      CORRIGE  linea 2336: zSim.o:$LC633 -> zSim.o:$LC630
      CORRIGE  linea 2338: zSim.o:$LC634 -> zSim.o:$LC631

Sin eso, esas dos cadenas se estripan, la `.rodata` de zSim sale 16 B corta y el
DOL rompe (`linkdelta` lo enseña como `rodata-16`, `dolwhere` como 74.701 B). No
lo he aplicado yo porque `config/GOWE69/*` es coordinación.

**2 · La promoción en `configure.py`.**

**3 · Deuda que dejo apuntada:** el alias de §3.7 lleva `$LC526` escrito a mano,
o sea la misma trampa que `keep.lst` —el número se desplaza si se añade o quita
un literal ANTES en el orden de parseo (OBB, Simulation, Util, SimSurface,
LocalPlayer, Sim*, la primera mitad de QuickGame)—. Y a diferencia de `keep.lst`,
**nadie lo comprueba**: `lcfix.py` sólo mira `keep.lst`. Hoy hay exactamente una
referencia `$LC` en fuente en todo el árbol (`QuickGame.cpp`), así que extender
`lcfix.py` para cubrirla es barato y evita un DOL roto en silencio. El comentario
del sitio ya avisa.
