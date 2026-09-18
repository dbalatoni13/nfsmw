# Brief de la ronda 46

Sustituye a `brief-r36f.md`. Leelo entero antes de tocar nada: incorpora lo que
midieron las nueve rondas siguientes, y saltartelo cuesta medio dia.

## Estado

`matched` **98,65881 %** (3.893.124 / 3.946.048 B, 18.373 / 18.432 funciones).
`linked` **18,91 %** (480 / 617 unidades, 746.188 B).
Quedan **52.924 B en 59 funciones de 27 unidades**.
DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## Reglas duras

1. **Nunca lances un `ninja` completo.** Usa `python scripts/build_direct.py <unidad>`.
2. **No hagas commits.** Deja los ficheros modificados; la coordinacion comprueba.
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Si hace falta
   un cambio ahi, PROPONLO en el informe con la medida que lo respalda.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Ancla cada parche con **dos o tres lineas de contexto**: un `replace(old,new,1)`
   con una linea sola cae en otra funcion.
7. Muchos ficheros son **CRLF entero** (y alguno mixto). Un parche con saltos de
   linea Unix casa 0 de N sin avisar.
8. **Un comentario que copia una anotacion del DWARF lleva un fin de comentario
   dentro**: eso cierra el bloque antes de tiempo. Quitalos.
9. **Cero bytes con `asm` puesto es deuda**: si una funcion no llega al 100 %,
   revierte y deja la veda escrita, salvo que el avance sea estructural (el
   tamano pasa a exacto, desaparece el marco o el derrame) y lo digas claro.

## Herramientas

**Tria por INSTRUCCIONES distintas, no por porcentaje.**

    python scripts/fncmp.py <unidad>          # que funciones NO son identicas
    python scripts/fncmp.py <unidad> Nombre   # el detalle instruccion a instruccion
    python scripts/triaje.py <unidad>         # PERMUTADOR / falta una local / estructura
    python scripts/tamfn.py src/.../x.cpp     # compila y mide, 1 segundo
    python scripts/fndiff.py <unidad> <simbolo>   # confirmalo ANTES de empezar

`matched_code` es **todo o nada**: una funcion al 99,99 % aporta CERO bytes.

## Las palancas

Todas son de GCC. **En unidades de mwcc no funciona ninguna** (`LibSN/steering`,
`LibSN/vm`, `LibSN/vmbase`, `DebuggerDriver`, `Packages/*`, `OdemuExi2`); ahi
solo queda reordenar la fuente, y lo unico que ha funcionado en nueve rondas es
**leer a traves de un puntero `volatile`** para fijar el punto de carga sin
anadir instrucciones (dos cierres en `steering`, r40/r41).

### 1. Barrera selectiva

`asm("" : "+r"(x))`, `"+f"`, `"+m"`. Extendida y NO volatil: ata solo lo que
nombra. **Ocho usos medidos:**

- **adelanto del planificador**: el valor acaba en un preservado donde el
  objetivo lo tiene en un volatil. Firma de `regmap`: `original f2 -> f30`.
- **pliegue de CSE**: impide plegar una constante o replegar un puntero.
- **hundimiento entre bloques** y **coalescing**.
- **orden de preparacion de argumentos de una llamada**: **un `asm` por argumento
  DESCOLOCADO**, no por argumento. Y **sin una local propia el resultado es 12
  veces peor**.
- **`"+m"` fija el `stw` de una local de PILA.** Si la variable se pasa por
  referencia vive en la pila y un `"+r"` **se ignora en silencio**.
- **la constante de un store a un temporal con barrera** rompe el desempate de
  `rank_for_schedule` entre la preparacion de argumentos y una sentencia ajena.
- **la local de desplazamiento explicito** para el orden de operandos de
  `lwzx`/`stwx`. **Decide la local, no el cast.**
- **NUEVO r37 --- la cadena de tres.** En `AddToQueue` ninguna de las tres
  dependencias aislada movia nada y las tres juntas cerraron la funcion. Cuando
  el objetivo emite una secuencia concreta (id, luego Head, luego desplazar,
  luego enmascarar), escribe **una dependencia por eslabon** y midelas solo
  juntas. No es lo mismo que repetir la barrera sobre la misma variable.

**El eje «a quien»**: `asm("" : "+r"(x) : "r"(y))`. El operando de **ENTRADA**
sube `n_refs` del pseudo y voltea `allocno_compare`. Sube a saltos de
`floor_log2`. Tiene que ser entrada.

**LAS FAMILIAS SE COMBINAN**, pero no siempre: en `MsgBarrier` hubo que mezclar
barrera de argumento y pin, y en otras dos unidades combinarlas empeoro. Es una
tecnica, no una ley.

**Donde NO vale**: empates del asignador y CSE local (`cse1`).

### 2. La cantidad fantasma

    register int guard asm("r19");
    asm("" : "=r"(guard));
    ...
    asm("" : "+r"(algo_vivo) : "r"(guard));

El `asm` de cierre **tiene que tocar un valor vivo**; si no, DCE se lleva la
pareja y el objeto sale IDENTICO --un ensayo que da el numero exacto de la base
es sospechoso de esto, no de «no funciona»--. Solo paga si al objetivo le sobra
una cantidad. **EL REGISTRO IMPORTA**: r19 dio 4 filas donde r30 dio 53.

**Veda r44/r45**: en `madidct` la serie completa r14..r31 no movio los 496 B de
`IdctRow`. No repitas cantidades sin inicializar ahi.

### 3. Pin de registro

`register float x asm("fr7")`. La unica que llega a `local_alloc`. Se ignora si
se toma la direccion. **Si el pin empeora, el reparto es un SINTOMA**: arregla
antes la primera diferencia que no sea de registro. **Cardinalidad**: ante una
rotacion de N registros, pinchar UNO arrastra a los demas. Y midelo sobre la
BASE LIMPIA.

**Aviso r45**: pinchar el contador de un bucle hace que GCC lo **coalesce** con
su sucesor, emita `addi rN,rN,1` y elimine el `mr` que el objetivo si tiene. Si
el objetivo separa dos pseudos, nombra el **valor sucesor** y separalos con una
barrera; pero eso no arrastra la rotacion global.

### 4. Barrera de ranura

Un `asm` de cero bytes que ocupa una ranura de emision. Lo que decide no es el
clobber sino **de que lado del store** se pone.

### 5. `asm()` de AMBITO DE FICHERO

Para lo que en C no sale --una llamada de cola, un alias, un talon-- **sin pasar
la unidad a `.s`**. En un `.s`, escribe `bl simbolo+0x24` y no una etiqueta
local: con la etiqueta el ensamblador reubica contra la SECCION y objdiff lo
cuenta como distinto aunque el byte sea el mismo.

### 6. Las guardas de cabecera que nadie enciende

`gc_interface` cerro ENTERA porque su cabecera traia una guarda que convierte
una llamada en inline y nadie la habia definido. **Escribir la llamada a mano NO
equivale**: lo que decide es que la funcion se expanda inline.

### 7. NUEVO --- el compilador equivocado

`__va_arg` paso ocho rondas atascada al 58 % con diez formas de fuente medidas.
No era la fuente: la funcion es de **Metrowerks** y el troceador la habia metido
en un objeto de GCC. **Si una unidad se resiste con un porcentaje bajo y su
codigo no se parece al idioma del compilador que le asigna `build.ninja`,
comparala contra el otro compilador antes de seguir escribiendo variantes.**

### 8. NUEVO --- el mapa de lineas manda sobre `-ffast-math`

`GetLoadingPriority` emitia los productos flotantes reasociados porque estaban
en una sola sentencia. El ELF atribuye cada multiplicacion a una linea distinta;
escritas como sentencias separadas, GCC ya no las reagrupa. **Antes de pelear
con el orden de un calculo flotante, mira cuantas lineas le da el mapa.**

## Avisos que costaron una ronda

- **`regmap` da falsos positivos**: empareja POR POSICION cuando los nombres
  difieren. Confirma con `symbols/mw_dwarfdump.nothpp`.
- **«Permutador agotado» NO es techo**: su catalogo es de formas de FUENTE y no
  contiene ninguna variante con `asm`.
- **El fuzzy sube con menos instrucciones exactas.** En r45 una variante subio de
  98,67 a 99,35 % **bajando** las filas exactas de 230 a 225. Cuenta filas.
- **Un encadenado de `asm` de cero bytes NO propaga `INSN_PRIORITY`.**
- `local_alloc` corre **despues de `sched1`**.
- El SHA del `.o` entero se mueve aunque el `.text` no cambie. Compara los
  BYTES DE LA FUNCION.
- **Nunca QUITES un literal** en una unidad con entradas `$LC` en `keep.lst`.
  Rellenalo a multiplo de 8. Pasa `lcfix.py --check`.
- **El puntero a miembro no es una llamada virtual barata**: en `zSpeech` activo
  la expansion ABI generica y anadio 80 B.

## El reparto

| # | territorio | B | funciones |
|---|---|---:|---:|
| 1 | `zCamera` | 10.812 | 5 |
| 2 | `zWorld` + `zWorld2` + `zTrack` | 10.112 | 8 |
| 3 | `zEcstasy` + `zEagl4Anim` | 10.188 | 8 |
| 4 | `zAI` + `zPhysicsBehaviors` + `zPlatform` + `zFe2` + `zFeOverlay` | 7.664 | 6 |
| 5 | `zEAXSound` + `zEAXSound2` + `zSpeech` | 4.948 | 8 |
| 6 | bibliotecas GCC: `sfir`, `madidct`, `criticalpath`, `pathnode`, `pathtrack`, `spchsamp`, `dvd_device` | 4.468 | 9 |
| 7 | mwcc: `steering`, `DebuggerDriver`, `vm`, `vmbase`, `FSasync` | 4.712 | 14 |
| 8 | `linked`: el racimo `gc_interface` + `asd2` | 6.240 de linked | --- |

### Notas por territorio

**1. `zCamera`.** Su mayor funcion pendiente no es codigo de juego: es el
`__static_initialization_and_destruction_0` de la SourceList, 3.604 B al
97,11 %. Eso no se arregla con barreras --lo decide **que estaticos con
constructor tiene la unidad y en que orden**--, asi que es trabajo de censo:
compara la lista de inicializaciones del objetivo con la nuestra antes de mirar
una sola instruccion. `Update__8ICEMoverf` (3.868 B al 99,91 %) tiene
prohibicion explicita de inventar locales, precedente r36e.

**2. `zWorld`/`zWorld2`.** `HolePunchAvoidables` (2.980 B) ya gano 2,7 puntos con
la barrera selectiva de tipo «preservado contra volatil»; es la unidad donde esa
familia esta mejor caracterizada.

`GetLoadingPriority` de `zTrack` (708 B) **NO esta cerrada, y ojo con la base**:
r37 dejo la forma secuenciada del calculo flotante --que es la que reproduce el
mapa de lineas y evita la reagrupacion de `-ffast-math`-- pero eso **bajo** el
porcentaje de 97,18 a 94,72 con delta 0 B. Mide las dos bases antes de decidir
desde cual trabajas. Lo que manda no es el porcentaje: la primera diferencia es
la **instruccion cero**, `stwu r1,-0x120` contra nuestro `-0x110`. Al objetivo
le sobran 16 B de marco, o sea que le falta una local --familia
«la local que roba el registro»--, y hasta que aparezca el reparto de f29..f31
es un sintoma. Precedente vigente: `9de121ee` retiro un `pos2` inventado por no
tener respaldo en el DWARF, asi que la local hay que **encontrarla en el
volcado**, no inventarla.

El hallazgo del mapa de lineas (palanca 8) sigue siendo bueno como diagnostico:
mira si `RenderFlaresOnCar` (2.908 B) tiene el mismo sintoma de reagrupacion.

**5. Sonido.** `SetupNextLoad` tiene tres rondas de pines medidos y agotados
(r36c, r36f, r45): **no repitas pines del contador, del indice ni del sucesor**;
la hipotesis viva es cambiar el CFG de la segunda pasada. `RoadblockFlow::Setup`
de `zSpeech` esta a dos desempates de planificacion dentro de un thunk.

**6. Bibliotecas.** `sfir` esta a **4 B de mas y cero de menos**: es la firma
exacta de «sobra un `if` defensivo que el original no tiene», no de reparto.
`madidct` tiene vedas grandes ya escritas en la propia fuente: **leelas antes**;
lo unico no probado es reproducir simultaneamente CTR y LR vivos.

**7. mwcc.** Ninguna palanca de GCC existe aqui. Lo unico que ha funcionado es
el puntero `volatile`, y `steering` tiene ocho funciones abiertas: es el
territorio con mas funciones por byte, y el mas barato de medir.

**8. El racimo.** Ver `r45-cierre.md`. El diagnostico esta cerrado: falta un
`Realmc::ROOT_DIRECTORY_NAME` con constructor en la unidad, que es lo que decide
el nombre de `_GLOBAL_.I.`. Vale 6.240 B de `linked` de una vez.

## Que entregar

1. Informe en `docs/analisis/r46-<tu-clave>.md`: lo cerrado (con `fndiff` al
   100 % citado), lo medido y negativo **con la cifra**, y las propuestas fuera
   de territorio.
2. Verificacion obligatoria: `fndiff` = 100.0 en lo que cierres; `fncmp`
   antes/despues sobre TODAS tus unidades sin que **ninguna** empeore;
   `lcfix.py --check` limpio.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**.
