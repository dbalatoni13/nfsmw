# Brief de la ronda 47

Sustituye a `brief-r46.md`. Leelo entero antes de tocar nada.

## Estado

`matched` **98,93 %** (3.903.740 / 3.946.048 B, 18.389 / 18.432 funciones).
`linked` **19,55 %** (492 / 619 unidades).
**El SDK entero esta al 100 %**: 98 de 98 unidades, 834 de 834 funciones.
Quedan **42.308 B en 42 funciones de 19 unidades**.
DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## Reglas duras

1. **Nunca lances un `ninja` completo ni `configure.py`.** Usa
   `python scripts/build_direct.py <unidad>`. Hay otros agentes en el arbol.
2. **No hagas commits ni `git add`.** Deja los ficheros modificados.
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Si hace
   falta un cambio ahi, PROPONLO con la medida que lo respalda.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques --salvo que tu territorio sea el unico consumidor,
   y entonces el gate es la SourceList entera.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Ancla cada parche con **dos o tres lineas de contexto**.
7. Muchos ficheros son **CRLF entero** (y alguno mixto). Un parche con saltos
   de linea Unix casa 0 de N sin avisar.
8. **Si retiras un ensayo, comprueba que la fuente vuelve de verdad.** En las
   iteraciones del usuario un cierre ya integrado se perdio al restaurar una
   copia, el `Matching` de `configure.py` se quedo, y el DOL salio con
   **513.077 bytes distintos**. Deja siempre `fncmp` de tus unidades al final.
9. **Cero bytes con `asm` puesto es deuda**: si una funcion no llega al 100 %,
   revierte y deja la veda escrita, salvo que el avance sea estructural (el
   tamano pasa a exacto, desaparece el marco o el derrame) y lo digas claro.

## Herramientas

**Tria por INSTRUCCIONES distintas, no por porcentaje.**

    python scripts/fncmp.py <unidad>              # que funciones NO son identicas
    python scripts/fncmp.py <unidad> <Nombre>     # detalle instruccion a instruccion
    python scripts/fndiff.py <unidad> <simbolo>   # confirmalo ANTES de empezar
    python scripts/triaje.py <unidad>             # PERMUTADOR / falta local / estructura
    python scripts/lmap.py <unidad> <simbolo>     # la linea de fuente de cada instruccion
    python scripts/build_direct.py <unidad>

`matched_code` es **todo o nada**: una funcion al 99,99 % aporta CERO bytes. Y
el fuzzy engana: puede SUBIR bajando las filas exactas. **Cuenta filas.**

**AVISO**: `fncmp.py` y `triaje.py` **no ven `zFeOverlay` ni `zOnline`** --su
codigo va en seccion `.over`, no `.text`--. `fncmp` aborta y `triaje` contesta
«0 funciones». Usa `fndiff` ahi.

## El instrumento nuevo: los volcados RTL

**El fuente de GCC 2.95.3 esta en el arbol**: `orig/prodg/NGC_GNU_SRC/NGC/gcc/`
(`cse.c`, `sched.c`, `local-alloc.c`, `global.c`, `jump.c`, `combine.c`...). No
inferences el comportamiento del compilador: leelo.

Y se pueden sacar sus volcados. `ngccc.exe` se come `-da`/`-dg` sin hacer nada,
pero invocando **`CPP.exe` y luego `build/compilers/ProDG/3.9.3/cc1plus.exe`**
a mano, con los cflags reales de la unidad mas `-dg -dl` (o `-da`, `-dj`,
`-dL`), salen. Una SourceList entera tarda 22 s; **un `.cpp` suelto que
reproduzca la funcion byte a byte tarda 0,4 s** y es lo que hay que buscar.

- **`.greg`**: el orden de asignacion **ya ordenado por `allocno_compare`**, la
  matriz de conflictos y el `reg_renumber` final. Un near-miss de reparto deja
  de ser adivinanza: se ve que allocno gana y por que.
- **`.combine`**: por que `combine` no pliega algo. Asi cerro `EvalSQT`.
- **`-dj`/`-dL`**: contar rtx por bloque, para el presupuesto de `cse`.

## Las palancas

Todas son de GCC. **En `LibSN/steering` no funciona ninguna** (es mwcc); ahi lo
unico que ha funcionado es **leer a traves de un puntero `volatile`** para fijar
el punto de carga sin anadir instrucciones.

### 1. Barrera selectiva

`asm("" : "+r"(x))`, `"+f"`, `"+m"`. Extendida y NO volatil: ata solo lo que
nombra. Usos medidos: adelanto del planificador (preservado contra volatil),
pliegue de CSE, hundimiento entre bloques, coalescing, orden de preparacion de
argumentos (**un `asm` por argumento DESCOLOCADO**, y **sin una local propia el
resultado es 12 veces peor**), `"+m"` para fijar el `stw` de una local de PILA
(un `"+r"` sobre una variable que vive en la pila **se ignora en silencio**), y
la constante de un store a un temporal.

**La cadena de N.** En `AddToQueue` ninguna de las tres dependencias aislada
movia nada y las tres juntas cerraron la funcion; en `SetupNextLoad` fueron
cuatro dependencias reales de `this` en dos cadenas. Cuando el objetivo emite
una secuencia concreta, escribe **una dependencia por eslabon** y midelas solo
juntas.

**El eje «a quien»**: `asm("" : "+r"(x) : "r"(y))`. El operando de **ENTRADA**
sube `n_refs` del pseudo y voltea `allocno_compare`. Tiene que ser entrada.

### 2. La cantidad fantasma

    register int guard asm("r19");
    asm("" : "=r"(guard));
    ...
    asm("" : "+r"(algo_vivo) : "r"(guard));

El `asm` de cierre **tiene que tocar un valor vivo**; si no, DCE se lleva la
pareja y el objeto sale IDENTICO. **EL REGISTRO IMPORTA**: r19 dio 4 filas
donde r30 dio 53. Veda: en `madidct` la serie r14..r31 no movio nada.

### 3. Pin de registro

`register float x asm("fr7")`. Lo unico que llega a `local_alloc`. Se ignora si
se toma la direccion. **Si el pin empeora, el reparto es un SINTOMA.** Pinchar
UNO arrastra a los demas. Y pinchar el contador de un bucle hace que GCC lo
**coalesce** con su sucesor y elimine el `mr` que el objetivo si tiene.

### 4. Barrera de ranura

Un `asm` de cero bytes que ocupa una ranura. Decide **de que lado del store**.

### 5. `asm()` de AMBITO DE FICHERO, y el `asm` CON CUERPO

Para un alias, un talon o una llamada de cola sin pasar la unidad a `.s`.

**NUEVO r46 --- el `asm` que emite la instruccion que falta.** En `EvalSQT` el
`.combine` enseno que la instruccion del objetivo YA estaba y lo unico que
sobraba era un `zero_extend` que `combine` no podia tirar (el pseudo tenia una
def por copia que envenena `reg_nonzero_bits`). En C no habia salida. La
palanca es un `asm` cuyo operando de **ENTRADA es el campo con su tipo
estrecho** --HImode, asi que GCC pasa el registro crudo sin extender-- y que
emite la instruccion que el objetivo ya tiene. **No es deuda si el tamano pasa
a exacto y el diff a cero.** Firma: `triaje` da `sobra>0` con
`regs=falta=otro=0` y diferencia de tamano multiplo de 4.

### 6. Las guardas de cabecera que nadie enciende

Busca `#ifdef` en las cabeceras de tu territorio que no aparezcan en ningun
`#define` ni en los cflags. **Escribir la llamada a mano NO equivale**: lo que
decide es que la funcion se expanda inline.

### 7. El compilador equivocado, y el objeto que son varias TU

`__va_arg` paso ocho rondas al 58 % con diez formas de fuente. No era la
fuente: era **Metrowerks** en un objeto de GCC. Y `OdemuExi2` resulto ser
**TRES unidades de traduccion** en un objeto, cada una con su version de MWCC:
partido, las tres cierran enteras. `LibSN/vm` eran tres tambien.

**Como se detecta**: barre las versiones de compilador del arbol sobre la
unidad y mira si los fallos se parten en conjuntos **disjuntos y separados por
direccion**. Si es asi, la frontera es la costura entre TU.

**Y como NO se valida**: comprobar el reparto de estaticos **compilando** las
mitades no basta --cada objeto sale bien y solo el ENLACE destapa que un
`static` de una mitad lo referencia la otra--.

### 8. El mapa de lineas manda sobre `-ffast-math`

`GetLoadingPriority` emitia los productos flotantes reasociados por estar en una
sola sentencia. El ELF atribuye cada multiplicacion a una linea distinta.
**Antes de pelear con el orden de un calculo flotante, mira cuantas lineas le
da el mapa** (`lmap.py`).

### 9. NUEVO r46 --- el area X del marco

Los 8 B de marco de `pathtrack` que dos rondas dieron por inalcanzables no eran
una ranura de derrame: son el **area X del marco SVR4 de rs6000**, el temporal
de conversion entero<->flotante, que GCC 2.9 reserva al **expandir** la
conversion y **no libera** aunque el optimizador se lleve despues todo su
codigo. Firma: mismas instrucciones, todas identicas, marco del objetivo 8 B
mayor, y lineas del original sin una sola instruccion.

### 10. NUEVO r46 --- los dos grupos de peso de `sched1`

`rank_for_schedule` desempata por **`INSN_REG_WEIGHT`**, un nivel que solo
existe en `sched1`. En una tirada de escrituras de miembros que leen el mismo
registro constante, el store que **mata** ese registro pesa -1 y los demas 0:
salen **primero todos los de peso -1 en orden de fuente, y despues todos los de
peso 0 en orden de fuente**. Y el `REG_DEAD` esta en el ultimo uso **en la
fuente**, asi que mover una sentencia decide quien pesa -1. Cerro `zFe2` sin un
solo `asm`, solo moviendo cuatro sentencias.

### 11. NUEVO r46 --- el parametro que no sale en el diff

En `dvd_device`, r36e midio siete barreras sobre el parametro que el diff
senalaba y se quedo a 4 B. La palanca era sacar **el otro** parametro de su
registro cacheando su cast en una local: entonces GCC coloco el primero donde
tocaba por su cuenta. **Si el diff senala un valor, prueba tambien a mover a su
vecino.**

### 12. NUEVO r47 --- la forma del bucle decide el idioma de la rama

El hallazgo mas rentable de la temporada. En `SAP.h`, con
`while (head && head->mPos < mPos)` GCC emite `blt`; escrito como

```cpp
while (head != nullptr) {
    if (head->mPosition < this->mPosition) { node = head; } else { break; }
    head = head->mTail;
}
```

sale el idioma `fcmpu + cror + bso` que el original tiene bajo `-ffast-math`.
**Cerro 4.396 B en dos funciones a la primera compilacion**, y tumbo una veda
que la ronda anterior habia dado por cerrada con analisis de `jump.c`.

Leccion: **una comparacion flotante dentro de un `&&` no genera el mismo codigo
que la misma comparacion en un `if` propio.** Si al objetivo le sobra un `cror`
o un `bso`, mira la forma del bucle antes que el planificador.

## Avisos que costaron una ronda

- **La reciproca de «falta una local» es FALSA.** Que el DWARF del original no
  liste una local NO significa que sobre: tres contraejemplos medidos en una
  ronda (`slipBoost`, `scale`, y las tres de `DefragmentPool`, que cuestan 47
  filas si se retiran). Es una hipotesis que hay que medir quitandola.
- **`regmap` da falsos positivos**: empareja POR POSICION cuando los nombres
  difieren. Confirma con `symbols/mw_dwarfdump.nothpp`.
- **«Permutador agotado» NO es techo**: su catalogo es de formas de FUENTE.
- **Las vedas caducan.** Tres han caido esta temporada, y las tres por haber
  probado N formas de **la sentencia equivocada**. Si una veda cita un
  porcentaje, **remidelo sobre la base actual antes de creertela**: la de
  `EvalState` daba 97,76 % y sobre la base de hoy da 95,26 %.
- **Nunca QUITES un literal** en una unidad con entradas `$LC` en `keep.lst`.
  Rellenalo a multiplo de 8. Pasa `lcfix.py --check`.
- El SHA del `.o` entero se mueve aunque el `.text` no cambie. Compara los
  BYTES DE LA FUNCION.

## El reparto

| # | clave | territorio | B | fn |
|---|---|---|---:|---:|
| 1 | `cam` | `zCamera` | 10.812 | 5 |
| 2 | `world` | `zWorld` + `zWorld2` + `zTrack` | 10.112 | 8 |
| 3 | `ecs` | `zEcstasy` + `zEagl4Anim` | 8.708 | 7 |
| 4 | `plat` | `steering` + `zPlatform` + `zPhysicsBehaviors` | 5.012 | 6 |
| 5 | `snd` | `zEAXSound` + `zEAXSound2` + `zSpeech` + `zFeOverlay` | 4.040 | 8 |
| 6 | `libs` | `madidct`, `sfir`, `criticalpath`, `pathnode`, `spchsamp` | 3.604 | 7 |

### Notas por territorio

**1. `zCamera`.** El `__static_init` (3.604 B) **no** es codigo de juego, y su
censo de constructores ya salio LIMPIO en r46 --las 34 llamadas identicas y en
el mismo orden, y el multiconjunto de 192 referencias a globales tambien--. El
diagnostico vivo es el **presupuesto de `cse`**: hay que quitar 107 rtx y el
limite es 826. La veta sin explorar: 134 cargas de literal usan una forma cara
de 3 rtx y 67 una barata de 2 (`memory_address()` -> `force_reg`); nadie
consiguio aislar que la dispara. `Update__8ICEMoverf` tiene **prohibicion
explicita de inventar locales**.

**2. `world`.** `HolePunchAvoidables` esta a **9 filas** (99,92 %) tras la
correccion del orden de argumentos de `bCross`: 4 de empate `cr2`/`cr3` --las 6
permutaciones de declaracion ya barridas-- y 5 de los temporales del pin
`fr10`. Es la mas cerca del arbol. Y **queda pendiente auditar el orden de
argumentos de los 35 `bCross(`** leyendo la forma `fmuls`/`fmsubs`: uno estaba
invertido y nadie lo habia visto.

`GetLoadingPriority`: las dos bases ya medidas; se queda la de HEAD. Los 16 B
de marco estan **dentro del bloque muerto de `RemoteCaffeinating`**, no en una
local viva.

**3. `ecs`.** `Initialize` (2.352 B): los 4 B estan en la **forma del arbol del
`switch`** --el objetivo usa la forma de RANGO (`cmplwi 1; ble` + caida) donde
nosotros usamos la de valor unico--; 8 variantes y 31 combinaciones ya medidas
sin exito. `epCalculateLocalDirectionalPOS16`: los 8 B son una **ranura
huerfana que nadie referencia**, un `assign_stack_temp` de mas, no reparto.

**4. `plat`.** `steering` es mwcc: solo el puntero `volatile`, y el eje de
cflags y versiones ya esta barrido (19 versiones, 232 compilaciones de
pragmas). `ActualReadJoystickData`: los cinco `mr` muertos **desaparecen con
`-fno-schedule-insns`**, o sea que son copias del reparto y el frente vivo es
la presion. `UpdateLoaded`: el `@ha` del literal 1.0f vivo en r30.

**5. `snd`.** `SetupNextLoad` YA CERRO --no la toques--. Queda
`cStichWrapper::Play` (384 B), que es lo unico que separa a `zEAXSound` de
promocionar entera. `Play__16CARSFX_RoadNoise` esta a 7 filas y su cierre exige
invertir la prioridad de dos allocnos que nacen en el constructor inline de
`SND_GEN/ENVIRO_AEMS.h`: **saca el `.greg` antes de probar nada**. `zFeOverlay`
solo se mide con `fndiff`.

**6. `libs`.** `sfir`: 4 B de mas y cero de menos, pero **no es un `if`
defensivo** --son 11 INSERT contra 10 DELETE, un bloque puente que PRE crea en
la arista `default` del switch--. `madidct`: la receta `"+c"(src6)` + `"+l"(t9)`
ya da **516/516 B y 129 instrucciones contra 129**, la primera vez con el
tamano exacto; se retiro por exigir una local que el DWARF no nombra, y esta
escrita en la fuente. Es el lead mas concreto del territorio.

## Que entregar

1. Informe en `docs/analisis/r47-<tu-clave>.md`: lo cerrado (con `fndiff` al
   100 % citado), lo medido y negativo **con la cifra**, y las propuestas fuera
   de territorio.
2. Verificacion obligatoria: `fndiff` = 100.0 en lo que cierres; `fncmp`
   antes/despues sobre TODAS tus unidades sin que **ninguna** empeore;
   `lcfix.py --check` limpio; `git status` de tus ficheros revisado.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**.
