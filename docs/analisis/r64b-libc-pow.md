# r64b - libc-pow: retirar andamios de sf_log10.c, ef_pow.c y e_pow.c

Lote: `src/libc/sf_log10.c`, `src/libc/ef_pow.c`, `src/libc/e_pow.c`.
Las tres unidades estan promocionadas y en el enlace (`linked True` en
`build.ninja`, objeto en `build/GOWE69/src/libc/`).

## 0. El encargo decia 53 andamios. Son 23

El encargo conto todas las apariciones del token `__asm__`. Separadas por tipo:

| fichero | barreras `__asm__("")` | pines `register T x asm("rN")` | nombres de simbolo `__asm__("nombre")` |
|---|---|---|---|
| `sf_log10.c` | 12 | 10 | 7 (`SDA_*_X`) |
| `ef_pow.c`   | 0  | 1  | 15 |
| `e_pow.c`    | 0  | 0  | 15 |
| **total**    | **12** | **11** | **37** |

Los 37 restantes no son andamios de codigo: son declaraciones de nombre de
ensamblador (`extern float pow_fabsf(float) __asm__("fabsf")`,
`static double mone__store __asm__("mone")`, `SDA_FLOAT_X(...)`). Colocan datos
y renombran simbolos; no tapan ningun hueco de generacion de codigo.

**Andamios de codigo reales en el lote: 23.** Y estan repartidos de forma muy
desigual: **22 de los 23 estan en `sf_log10.c`**, un fichero de 98 lineas.
`e_pow.c` -2.000 B de `.text`, el mas grande de los tres- no tiene ninguno.

## 1. El censo de `regmap`: 0 de 3 clasificables

    python scripts/regmap.py libc "log10f"
    aviso: "log10f" no esta en la unidad libc del original; esta en: zFoundation

`symbols/mw_dwarfdump.nothpp` **no contiene ninguna de las tres funciones**. Las
unicas apariciones de `log10f` y `powf` en el volcado son dos `static union
ExprValType` de zFoundation, sin relacion. El volcado es de codigo compilado por
Metrowerks; la libm del original venia de una biblioteca sin esa informacion de
depuracion.

**Reparto de veredictos: ESTRUCTURA 0 / PERMUTACION 0 / REPARTO 0 / IDENTICO 0 /
SIN DATOS 3.**

Aviso para el resto de la tanda: `regmap` no dice "no se" cuando no encuentra la
funcion, dice que esta **en otra unidad**. Un agente que se fie de esa linea
persigue una funcion equivocada.

En su lugar, aqui el oraculo es mejor que el DWARF: el fuente original de fdlibm
es publico, y el `.o` actual ya reproduce el objetivo.

## 2. La correccion del criterio: el sha1 del `.o` NO PUEDE quedar igual

Los objetos se compilan con `-gdwarf+`. La seccion `.line` guarda la posicion de
**cada sentencia**. Un `__asm__("")` es una sentencia. Por tanto:

> **Quitar una barrera cambia siempre el sha1 del `.o`, aunque el codigo
> generado sea identico byte a byte.**

Control medido: reescribir el comentario de cabecera de `sf_log10.c` sin cambiar
el numero de lineas da un `.o` **identico bit a bit** (`f927c1c7...`). El
compilador es determinista y ni el comentario ni la ruta se filtran. Pero
retirar la barrera de la linea 52 -codigo generado identico- deja `.line` en 548
en vez de 568.

El criterio utilizable es: **todas las secciones que no son de depuracion,
identicas byte a byte**. Es mas fuerte que el sha1 para lo que importa (el
enlazador tira `.debug`/`.line`; al DOL no llega nada de eso) y es el unico que
puede pasar. Herramientas usadas, en `scratchpad/libcpow64b/` (borradas al
terminar): `secdiff.py` (compara seccion a seccion) y `nodebug.py` (sha1 de todo
lo que no es depuracion).

**Sospecho que esto explica los negativos de rondas anteriores.** Si `eProject` o
`UpdatePlatInfo` se midieron por sha1 del `.o`, toda retirada limpia salio
"empeora".

## 3. `ef_pow.c`: el pin retirado, y por que estaba ahi

Unico andamio del fichero:

```c
{
    register int sign __asm__("r9") = (unsigned int)hx >> 31;
    xnegative = sign;
    sign -= 1;
    if ((sign | yisint) == 0)
        return (x - x) / (x - x);
}
```

`xnegative` esta declarado en la lista de locales y **fdlibm no lo tiene**.
fdlibm escribe la condicion en linea, y **nuestro propio `e_pow.c` -la version
double, que no lleva un solo andamio- ya la escribe asi**:

```c
if ((((((unsigned int)hx) >> 31) - 1) | yisint) == 0)
    return (x - x) / (x - x);
```

Copiada esa forma a `ef_pow.c` y borrados el pin, el bloque y la local:

- `.o` antes: `ee06a87c4e6c4f461260799016cff95b3f14c30d`
- `.o` despues: `ec46ca8971481fc01b765106684323f6c7c4214c`
- secciones fuera de depuracion que difieren: **0**
- `.text`: 1780 B -> 1780 B, sha1 no-depuracion `04f16764...` en los dos casos
- lo unico que cambia: `.debug` 5292->5196, `.line` 2308->2268, `.rela.debug`
  3516->3456 (la local que ya no existe)

Y la prueba de que el diagnostico es el correcto, no una coincidencia: quitar
**solo** el pin, dejando `xnegative`, da `.text` de **1776 B** (4 B de menos).
El pin era imprescindible *para esa forma*. Arreglada la forma, sobra.

Esto es exactamente el caso `ESTRUCTURA` de `regmap` -sobra una local- pero
detectado sin `regmap`, comparando con fdlibm y con la unidad hermana.

## 4. `sf_log10.c`: el andamio real es un cflag que falta

### 4.1 Barrido de caducidad

Retirado cada uno de los 22 por separado, y despues en avance voraz hasta punto
fijo. **2 de 22 estaban caducados**:

- linea 41 `register float wordFloat __asm__("fr0") = x;` (el pin; la barrera de
  la 43 se queda y basta)
- linea 52 `__asm__("" : "+r"(normalLimit) : "r"(hx), "r"(k));`

Los otros 20 son load-bearing hoy. Resultado aplicado:

- `.o` antes: `f927c1c7db103082e99c34b7ad8c29e89ed10f5e`
- `.o` despues: `5e84923c5f4db26b2e2301915421d7dbcf789c24`
- secciones fuera de depuracion que difieren: **0** (`.text` 268 B, sha1
  no-depuracion `daa0789d...` en los dos casos)
- solo cambian `.line` 568->548 y `.debug_srcinfo`

### 4.2 La medida que contesta la pregunta del encargo

Escrito el cuerpo **fdlibm limpio** -macros `GET_FLOAT_WORD`/`SET_FLOAT_WORD`
reales, locales `float y,z; int i,k,hx;`, `if (hx < 0x00800000)`,
`y = (float)(k+i)`- **sin una sola barrera y sin un solo pin**, y compilado con
`-msafe-sda` anadido:

```
--- BASE (22 andamios)                 +++ fdlibm limpio + -msafe-sda
 00000098  lis r10, 0x4330              00000098  lis r10, 0x4330
 0000009C  add r9, r9, r0               0000009C  add r9, r9, r0
-000000A0  lfd f0, lbl_804FF168@sda21(r0)
+000000A0  lfd f0, .sdata@sda21(r0)
 000000A4  xoris r9, r9, 0x8000         000000A4  xoris r9, r9, 0x8000
```

**Esa es la unica diferencia en las 67 instrucciones.** Mismo tamano (268 B),
mismo orden, **mismos registros** (r8/r9/r10/f0/f13/f31), mismos desplazamientos
de pila (xword en sp+8, el doble en sp+0x10). Y no es la fuente lo que cambia:
es de que simbolo sale el sesgo `0x4330000080000000` de la conversion
int->double, del pool propio del compilador o del `extern` a la etiqueta partida.

Sin `-msafe-sda`, el mismo cuerpo limpio mete el sesgo en `.rodata` y cuesta un
`lis @ha` de mas (272 B) y **desordena todo el reparto** (r7/r11 en vez de
r8/r9). Los 20 andamios que quedan reconstruyen a mano lo que el flag da gratis.

Con lo cual el comentario que llevaba el fichero -"compiler/ABI flag experiments
did not move it to .sdata"- **queda refutado y corregido en el propio fichero**.
`-msafe-sda` lo mueve. Es el flag que `e_pow.c` y `ef_pow.c` ya llevan
(`configure.py:2117` y `:2120`) y `sf_log10.c` no (`configure.py:2211`).

### 4.3 La receta completa, que NO he ejecutado (config prohibido)

Las siete constantes viven en `0x804FF158`..`0x804FF180`, y en `splits.txt`
**`libc/sf_log10.c` no tiene rango `.sdata` ninguno**: cae en el hueco entre
`libc/sf_log.c` (acaba en `0x804FF128`) y `libc/ef_pow.c` (empieza en
`0x804FF180`). Hoy lo sirve `auto_08_804FEDD0_sdata.s` con nombres `lbl_`.

El contenido y el orden son exactamente el pool que emitiria el compilador para
esta funcion, por orden de primer uso y con el relleno de alineacion a 8:

| dir | 0x158 | 0x15C | 0x160 | (pad) | 0x168 | 0x170 | 0x174 | 0x178 |
|---|---|---|---|---|---|---|---|---|
| | `-two25/zero` plegado | `zero` | `two25` | 4 B | sesgo int->double (8 B) | `log10_2lo` | `ivln10` | `log10_2hi` |

Receta, simetrica con `ef_pow.c`:

1. `configure.py:2211` -> `Object(Matching, "libc/sf_log10.c", extra_cflags=["-msafe-sda"])`
2. `splits.txt` -> dar a `libc/sf_log10.c` `.sdata start:0x804FF158 end:0x804FF180`
3. cuerpo fdlibm limpio con `static const float two25, ivln10, log10_2hi, log10_2lo, zero;`
4. borrar los 20 andamios restantes y las 7 declaraciones `SDA_*_X`

Aviso: el paso 2 es justo el peligro de
`docs/analisis/nfsmw-rango-no-basta.md`. Ademas el hueco de `.sdata`
`0x804FF128`..`0x804FF158` (doce floats huerfanos, `lbl_804FF12C`..`lbl_804FF154`)
sigue sin dueno y esta *antes* del rango que se reclama; hay que resolverlo en la
misma pasada o el dato se desplaza.

## 5. `e_pow.c`: nada que retirar, dos negativos utiles

Cero barreras y cero pines. Es la unidad mas limpia de las tres y la mas grande.
Aprovechando la parada, medi si sus dos trucos de datos han caducado:

| prueba | resultado | `.text` |
|---|---|---|
| `huge` vuelve a `const` (el comentario dice que `gcse` iza su carga) | DISTINTO `43fa5fec...` | 2000 -> **1996** |
| las 13 constantes tardias como `static const double` de fdlibm arriba, sin el truco `__store` | DISTINTO `9e2b185b...` | 2000 -> 2000 |

Las dos siguen siendo imprescindibles, y el comentario de `huge` sigue siendo
exacto: como `const` se pierden 4 B, la pareja `beq`/`b` desaparece.

## 6. Negativos de `sf_log10.c`

Todas dan `.text` de 268 B -el tamano correcto- pero contenido distinto. Sha1 de
las secciones sin depuracion; la base es `daa0789d...`:

| forma probada | sha1 | nota |
|---|---|---|
| `if (hx < 0x00800000)` de fdlibm en vez de `normalLimit` pinado | `a0d57d3f` | GCC iza `lis/ori` a r11 y lo pone *antes* del `stfs/lwz` |
| fdlibm puro, `y = (float)(k+i)` | `1ec218dd` | 272 B: `lis .rodata@ha` + `lfd @l` en vez de un `lfd @sda21` |
| fdlibm + sesgo a mano (lsw, luego msw) | `951cfe8d` | reparto entero desplazado r8->r11, r9->r10 |
| fdlibm + sesgo a mano (msw, luego lsw) | `7c06f6b3` | ademas `fsub f31,f0,f31` invertido |
| fdlibm + `INSERT_WORDS` | `e48cdb4b` | 272 B |
| fdlibm + `k += i` antes de convertir | `951cfe8d` | identico al anterior; GCC reordena |
| fdlibm + sesgo a mano + pines en `i`/`k`/`hx` | `ef72c8a1` | **260 B**, 8 de menos |
| lo anterior + pin de `highWord` en r10 | `f05dc3c1` | **264 B**, 4 de menos |

Las dos ultimas confirman lo de
`docs/analisis/nfsmw-si-el-pin-empeora.md`: puestos sobre la forma equivocada,
los pines **quitan** instrucciones que el objetivo tiene.

## 7. Veredicto

**El frente FUNCIONA, pero no como retirada de andamios uno a uno.**

Cifras del lote: **3 andamios retirados de 23 (13 %)**, en 2 de las 3 unidades, y
las tres unidades con todas las secciones no-depuracion identicas byte a byte.

Pero la cifra que importa es otra. En `ef_pow.c` la pregunta del encargo
-"existe una forma de fuente que lo haga innecesario?"- tuvo respuesta **si**, y
la respuesta estaba escrita en la unidad hermana. En `sf_log10.c` la respuesta
es **si para los 22**, y el precio es un cflag y un rango de `splits.txt`: eso
es deuda de *configuracion*, no de fuente, y **ningun barrido de andamios la
habria encontrado**. Lo que la encontro fue comparar con fdlibm y con la unidad
hermana que si lleva el flag.

Tres cosas que se llevan a otros lotes:

1. **Medir por secciones, no por sha1.** Con `-gdwarf+` el sha1 no puede quedar
   igual al retirar una sentencia. Es probable que haya negativos falsos en las
   rondas anteriores por esto.
2. **La unidad hermana es un oraculo.** `e_pow.c`/`ef_pow.c` difieren solo en
   precision: una tenia 0 andamios y la otra 1, y la limpia traia la forma buena.
3. **Un fichero con 22 andamios y sus vecinos con 0 no es un problema de fuente.**
   Es un flag, un rango o un `#pragma` que falta. Vale la pena mirar los cflags
   de la unidad *antes* de tocar el cuerpo.
