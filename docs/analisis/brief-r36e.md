# CORRECCION IMPORTANTE (leela antes que nada)

**La cola que te di en el encargo estaba INFLADA.** `fncmp.py` tenia tres
falsos positivos en las SourceLists y ya estan arreglados; varias funciones
que aparecian como «1 insn» o «AUSENTE» **estan al 100 %**. Confirmadas una
a una con `fndiff`, son fantasmas y no hay nada que hacer en ellas:

| unidad | funcion que NO hay que tocar |
|---|---|
| zEcstasy | `eMotionBlurEffect` (2.188 B), `eDisplayFrame` (3.300 B) |
| zAI | `ComputePotentials` (3.256 B), `GetPotentialSpeed` (1.960 B), `Update__12AIActionRacef` (3.924 B) |
| zEAXSound | `UpdateMixerOutputs` (1.212 B), las dos `CreateState` (120 B) |
| zPlatform | `DVDErrorTask` (1.904 B) |
| zWorld2 | `FindNodes` (3.456 B) |
| zGameplay | `Compare__...GatherVisibleIcons` (16 B), no falta |

**Vuelve a pasar `python scripts/fncmp.py <unidad>` con la version de ahora**
y trabaja sobre esa lista. La criba coincide al byte con el informe oficial
en 29 de las 31 unidades con codigo pendiente.

Y la regla que sale de esto, que ya estaba en el brief y ahora sabes por que:
**confirma con `fndiff <unidad> <simbolo>` ANTES de empezar con una funcion**,
no solo al terminar. Cuesta un minuto y te ahorra una tarde.

---
# Brief de la ronda 36e

Lee esto entero antes de tocar nada. Son las reglas del proyecto mas lo que
midieron las cuatro rondas anteriores; saltartelo cuesta medio dia.

## Estado

`matched` 98,43 % (3.884.092 / 3.946.048 B, 18.352 / 18.432 funciones).
DOL verificado en `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

## Reglas duras

1. **Nunca lances un `ninja` completo.** Usa `python scripts/build_direct.py <unidad>`.
2. **No hagas commits.** Deja los ficheros modificados y punto; yo comprometo.
3. **No toques `configure.py`, `config/GOWE69/*` ni `splits.txt`.** Si hace falta
   un cambio ahi, PROPONLO en el informe con la medida que lo respalda.
4. **Quedate en tu territorio.** Si la palanca esta en una cabecera compartida,
   proponla; no la apliques.
5. **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
   `bMath.hpp` y `UVectorMath.hpp`.
6. Ancla cada parche con **dos o tres lineas de contexto**: un `replace(old,new,1)`
   con una linea sola cae en otra funcion. Le paso a la r36d, cinco ensayos
   perdidos.
7. Muchos ficheros son **CRLF entero**. Un parche con `\n` casa 0 de N sin avisar.

## Como elegir en que trabajar

**Por INSTRUCCIONES distintas, no por porcentaje.** El porcentaje manda al sitio
equivocado; una funcion de 1.700 B a 5 instrucciones vale mas que una de 300 B
al 60 %.

    python scripts/fncmp.py <unidad>        # que funciones NO son identicas
    python scripts/fncmp.py <unidad> Nombre # el detalle instruccion a instruccion
    python scripts/tamfn.py src/.../x.cpp [-flags]   # compila y mide, 1 segundo

`fncmp.py` es nuevo (r36e) y **el censo por tamano enganaba de largo**: en
`FSasync` daba 1 funcion pendiente y son 2; en `filesys` 1 y son 3; en
`steering` 2 y son 11. Validado contra el informe oficial en 17 de 18 unidades.

Confirma siempre con `fndiff <unidad> <simbolo>` antes de dar algo por cerrado:
`matched_code` es **todo o nada**, una funcion al 99,99 % aporta CERO bytes.

## Las palancas, con lo que se sabe de cada una

Todas son de GCC. **En unidades de mwcc no funciona ninguna** (`LibSN/*`,
`Packages/*`, `OdemuExi2`); ahi solo queda reordenar la fuente.

### 1. Barrera selectiva --- `asm("" : "+r"(x))` / `"+f"` / `"+m"`

Extendida y NO volatil: ata solo lo que nombra. **Cinco usos medidos:**

- **adelanto del planificador**: el valor acaba en un preservado donde el
  objetivo lo tiene en un volatil. La firma la da `regmap`: `original f2 ->
  nuestro f30`. Va ANTES de la sentencia, atando su entrada.
- **pliegue de CSE**: impide que GCC pliegue una constante o repliegue un
  puntero al simbolo. Cerro `DoFSReadHeader` (el `16 << 8` sin plegar) y
  `__cvt_fp2unsigned` (puntero local + barrera).
- **hundimiento entre bloques** y **coalescing**.
- **orden de preparacion de argumentos de una llamada** (r36d-fe): la firma es
  el `lis @ha` de un literal una o dos ranuras fuera de sitio respecto al
  `lwz`/`mr` de otro argumento de la MISMA llamada. **Un `asm` POR argumento**:
  los tres juntos dan 98,4 % y tres separados dan 100 %. La fuente no lo mueve
  (cinco formas dan binario identico): lo decide `expand_call`.

**El eje «a quien» (r36d-ai)**: `asm("" : "+r"(x) : "r"(y))`. El operando de
**ENTRADA** sube `n_refs` del pseudo y voltea `allocno_compare`. Es el primer
mando conocido sobre una permutacion entre pseudos del compilador --los que
`regmap` no ve--. Tiene que ser entrada; con `"+r"` en el otro sale un `mr` de
mas. El aumento va a saltos de `floor_log2`: de 3 a 4 referencias mueve el
numerador de 3 a 8.

**Donde NO vale**: empates del asignador y CSE local (`cse1`). Medido en
`sfir::calcFIRCoeffs` con siete formas.

### 2. La cantidad fantasma (r36d-wld)

Cuando al objetivo le SOBRA una cantidad viva respecto a nosotros:

```c
register int guard asm("r28");
asm("" : "=r"(guard));
...
asm("" : "+f"(endOffset) : "r"(guard));   // consume ADEMAS un valor vivo
```

Cero bytes y **no es barrera de planificacion**. Dos reglas:

- el `asm` de cierre **tiene que tocar un valor vivo**; si no, DCE se lleva la
  pareja entera y el objeto sale IDENTICO. Un ensayo que da el mismo numero
  exacto que la base es sospechoso de esto, no de «no funciona».
- solo paga si al objetivo le sobra una cantidad: cuentalo con
  `grep -oE '\br(2[4-9]|3[01])\b'` sobre los dos `.s`.

### 3. Pin de registro --- `register float x asm("fr7")`

La unica que llega a `local_alloc`. Se ignora en silencio si se toma la
direccion. **Si el pin empeora, el reparto es un SINTOMA**: arregla la primera
diferencia que no sea de registro. Y cuidado: pinchar un `float` del que se toma
un cast a `sn_ps` produce codigo MEJOR que el original (falso positivo).

### 4. Barrera de ranura

Un `asm` de cero bytes que ocupa una ranura de emision. Gana el desempate de
`rank_for_schedule` por numero de dependientes. Lo que decide no es el clobber
sino **de que lado del store** se pone.

### 5. `asm()` de AMBITO DE FICHERO

Si el objetivo tiene algo que en C no sale --una llamada de cola, un alias, un
talon de una instruccion-- **no hace falta pasar la unidad a `.s`**:

```c
__asm__("\t.align 2\n\t.globl __shr2u\n\t.type __shr2u,@function\n"
        "__shr2u:\n\tb __lshrdi3\n\t.size __shr2u,.-__shr2u\n");
```

Saca el destino de las **reubicaciones del objeto extraido**, no del nombre que
parezca logico, y copia el idioma del propio compilador (`ngccc -S`). Rompio una
veda de siete funciones en `ppc2D2`.

## Avisos de herramienta que costaron una ronda

- **`regmap` da falsos positivos**: empareja POR POSICION cuando los nombres
  difieren. Confirma con el volcado DWARF antes de perseguir una permutacion.
- El `%` de `triaje.py` sale de un `report.json` que `build_direct.py` deja
  rancio; solo `difs` es fresco.
- `local_alloc` corre **despues de `sched1`**, asi que sus rangos no son los del
  `.s` final: razonar sobre el ensamblador induce a error.
- **Nunca QUITES un literal** en una unidad con entradas `$LC` en `keep.lst`:
  renumera todo lo posterior y las entradas pasan a salvar la cadena de al lado.
  Rellenalo a multiplo de 8 en vez de quitarlo. Pasa `python scripts/lcfix.py
  --check` al terminar.
- `-strip-unused-data` quita `size & ~7`, no `size`.

## Que entregar

1. Un informe en `docs/analisis/r36e-<tu-clave>.md` con: lo cerrado (con
   `fndiff` al 100 % citado), lo medido y negativo **con la cifra**, y las
   propuestas que caen fuera de tu territorio.
2. Verificacion obligatoria antes de dar nada por bueno:
   - `python scripts/fndiff.py <unidad> <simbolo>` = 100.0 en lo que cierres;
   - `pctsnap --cmp` (o `fncmp` antes/despues) sobre TODAS las unidades de tu
     territorio: **ninguna funcion puede empeorar**;
   - `python scripts/lcfix.py --check` limpio.
3. Si algo empeora aunque sea una funcion, **revierte y anotalo**. Media funcion
   no es neutra: baja vecinas y unidades sin agente.
