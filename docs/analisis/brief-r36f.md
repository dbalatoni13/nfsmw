# Brief de la ronda 36f

Lee esto entero antes de tocar nada. Es el estado del proyecto mas lo que
midieron las cinco rondas anteriores; saltartelo cuesta medio dia.

## Estado

`matched` 98,49 % (3.886.632 / 3.946.048 B, 18.360 / 18.432 funciones).
Quedan **58.020 B en 29 unidades**. DOL verificado en
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## Reglas duras

1. **Nunca lances un `ninja` completo.** Usa `python scripts/build_direct.py <unidad>`.
2. **No hagas commits.** Deja los ficheros modificados; yo comprometo.
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Si hace falta
   un cambio ahi, PROPONLO en el informe con la medida que lo respalda.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Ancla cada parche con **dos o tres lineas de contexto**: un `replace(old,new,1)`
   con una linea sola cae en otra funcion.
7. Muchos ficheros son **CRLF entero** (y alguno mixto). Un parche con `\n` casa
   0 de N sin avisar.
8. **Un comentario que copia una anotacion del DWARF lleva `/* */` dentro**: eso
   cierra el bloque antes de tiempo y la unidad deja de compilar. Quitalos.

## Herramientas y como elegir

**Por INSTRUCCIONES distintas, no por porcentaje.**

    python scripts/fncmp.py <unidad>          # que funciones NO son identicas
    python scripts/fncmp.py <unidad> Nombre   # el detalle instruccion a instruccion
    python scripts/tamfn.py src/.../x.cpp [-flags]    # compila y mide, 1 segundo

`fncmp.py` esta **validado al byte contra el informe oficial en 29 de las 31
unidades con codigo pendiente**. Aun asi: **confirma con `fndiff <unidad>
<simbolo>` ANTES de empezar con una funcion**, no solo al terminar. En la ronda
pasada cuatro encargos llevaban de cabecera una funcion que ya estaba al 100 %.

`matched_code` es **todo o nada**: una funcion al 99,99 % aporta CERO bytes.

## Las palancas

Todas son de GCC. **En unidades de mwcc no funciona ninguna** (`LibSN/*`,
`Packages/*`, `OdemuExi2`, `steering`); ahi solo queda reordenar la fuente.

### 1. Barrera selectiva --- `asm("" : "+r"(x))` / `"+f"` / `"+m"`

Extendida y NO volatil: ata solo lo que nombra. **Siete usos medidos:**

- **adelanto del planificador**: el valor acaba en un preservado donde el
  objetivo lo tiene en un volatil. Firma de `regmap`: `original f2 -> f30`.
- **pliegue de CSE**: impide plegar una constante o replegar un puntero al
  simbolo. Cerro `DoFSReadHeader` y `__cvt_fp2unsigned`.
- **hundimiento entre bloques** y **coalescing**.
- **orden de preparacion de argumentos de una llamada**: el `lis @ha` de un
  literal una o dos ranuras fuera de sitio respecto al `lwz`/`mr` de otro
  argumento de la MISMA llamada. **Un `asm` por argumento DESCOLOCADO**, no por
  argumento (ponerlas en todos empeora). Vale tambien cuando el argumento es una
  **direccion de pila**. Y **sin una local propia el resultado es 12 veces
  peor**: `bVector3 *dst = &x; asm("" : "+r"(dst));` da 2 filas y el mismo asm
  sobre la expresion da 25.
- **`"+m"` fija el `stw` de una local de PILA** (r36e). Si la variable se pasa
  por referencia vive en la pila y un `"+r"` **se ignora en silencio**. En
  `PATHI_loadbankdata` doce filas cayeron a cero; `"m"` de solo lectura y el
  clobber `::: "memory"` dejan cuatro.
- **la constante de un store a un temporal con barrera** rompe el desempate de
  `rank_for_schedule` entre la preparacion de argumentos de una llamada y una
  sentencia ajena: `int done = 1; asm("" : "+r"(done)); flag = done;` cerro
  `TIMER_init`. Las dos formas que dictaba el catalogo son PEORES ahi.
- **la local de desplazamiento explicito** para el orden de operandos de
  `lwzx`/`stwx`: `int off = n * sizeof(T);` y luego `(T*)((char*)p + off)`.
  **Decide la local, no el cast** --el mismo texto en linea deja la fila--,
  porque `combine` intercambia los operandos de un PLUS conmutativo cuando el
  primero es 'o' y el segundo no, y un ASHIFT no lo es.

**El eje «a quien»**: `asm("" : "+r"(x) : "r"(y))`. El operando de **ENTRADA**
sube `n_refs` del pseudo y voltea `allocno_compare`; es el unico mando conocido
sobre una permutacion entre pseudos del compilador. Sube a saltos de
`floor_log2`. Tiene que ser entrada.

**LAS FAMILIAS SE COMBINAN.** En `TrackCopCameraMover::Update` una barrera de
ranura y una de argumento dejan cada una **exactamente dos** filas y solo
cambian CUALES; juntas, cero. Si una familia se estanca en N filas, prueba OTRA
ENCIMA antes de darla por agotada.

**Donde NO vale**: empates del asignador y CSE local (`cse1`).

### 2. La cantidad fantasma

Cuando al objetivo le SOBRA una cantidad viva respecto a nosotros:

```c
register int guard asm("r19");
asm("" : "=r"(guard));
...
asm("" : "+r"(algo_vivo) : "r"(guard));
```

Cero bytes y no es barrera de planificacion. Tres reglas:

- el `asm` de cierre **tiene que tocar un valor vivo**; si no, DCE se lleva la
  pareja y el objeto sale IDENTICO. Un ensayo que da el mismo numero exacto que
  la base es sospechoso de esto, no de «no funciona».
- solo paga si al objetivo le sobra una cantidad (`grep -oE '\br(2[4-9]|3[01])\b'`
  sobre los dos `.s`), y **si los dos usais el banco entero no hay hueco**.
- **EL REGISTRO IMPORTA** (r36e): en `EmitterSystem::Render`, r19 da 4 filas,
  r18 6, r20 7, r17 8, r16 10, r15 12, r14 14 y r30 53. Al reves que el pin.

### 3. Pin de registro --- `register float x asm("fr7")`

La unica que llega a `local_alloc`. Se ignora si se toma la direccion. **Si el
pin empeora, el reparto es un SINTOMA**: arregla antes la primera diferencia que
no sea de registro. **Cardinalidad**: ante una rotacion de N registros, pinchar
UNO --el que senala el DWARF-- arrastra a los demas; dos pines dan 24 filas
donde uno da 2. Y midelo sobre la BASE LIMPIA: la r36c/r36d midieron pines
encima de una reasociacion, donde salen peor, y de ahi salio una veda falsa.

### 4. Barrera de ranura

Un `asm` de cero bytes que ocupa una ranura de emision. Lo que decide no es el
clobber sino **de que lado del store** se pone.

### 5. `asm()` de AMBITO DE FICHERO

Para lo que en C no sale --una llamada de cola, un alias, un talon-- **sin pasar
la unidad a `.s`**. Saca el destino de las **reubicaciones del objeto
extraido**, y copia el idioma del propio compilador (`ngccc -S`). En un `.s`,
escribe `bl simbolo+0x24` y no `bl .L_etiqueta`: con la etiqueta local el
ensamblador reubica contra la SECCION y objdiff lo cuenta como distinto aunque
el byte sea el mismo (valio dos funciones en `metrotrk`).

### 6. Las guardas de cabecera que nadie enciende

`gc_interface` cerro ENTERA porque su cabecera ya traia
`REALMC_GC_MESSAGE_INIT_INLINE` --que convierte una llamada en inline-- y nadie
lo habia definido. Busca `#ifdef` en las cabeceras de tu territorio que no
aparezcan en ningun `#define` ni en los cflags. **Escribir la llamada a mano NO
equivale**: lo que decide es que la funcion se expanda inline.

## Avisos que costaron una ronda

- **`regmap` da falsos positivos**: empareja POR POSICION cuando los nombres
  difieren. Confirma con el volcado DWARF (`symbols/mw_dwarfdump.nothpp`), que
  da las locales del original CON SU REGISTRO.
- **«Permutador agotado» NO es techo**: su catalogo es de formas de FUENTE y no
  contiene ninguna variante con `asm`. 1.450 variantes ciegas dieron cero
  mejoras en una funcion que luego cayo al 100 % con dos barreras.
- **Un encadenado de `asm` de cero bytes NO propaga `INSN_PRIORITY`** (1, 2, 4 y
  8 eslabones dan el objeto identico).
- `local_alloc` corre **despues de `sched1`**: sus rangos no son los del `.s`.
- El SHA del `.o` entero se mueve aunque el `.text` no cambie, porque `-gdwarf+`
  renumera lineas. Compara los BYTES DE LA FUNCION.
- **Nunca QUITES un literal** en una unidad con entradas `$LC` en `keep.lst`:
  renumera todo lo posterior. Rellenalo a multiplo de 8. Pasa `lcfix.py --check`.

## Que entregar

1. Informe en `docs/analisis/r36f-<tu-clave>.md`: lo cerrado (con `fndiff` al
   100 % citado), lo medido y negativo **con la cifra**, y las propuestas fuera
   de territorio.
2. Verificacion obligatoria: `fndiff` = 100.0 en lo que cierres; `fncmp`
   antes/despues sobre TODAS tus unidades sin que **ninguna** empeore;
   `lcfix.py --check` limpio.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**.
4. **Cero bytes con `asm` puesto es deuda**: si una funcion no llega al 100 %,
   revierte y deja la veda escrita, salvo que el avance sea estructural (el
   tamano pasa a exacto, desaparece el marco o el derrame) y lo digas claro.
