# Ronda 25 — zEcstasy: la premisa del encargo era vieja, y el error estaba en una cadena de asignaciones

Estado medido (`build_direct.py zEcstasy` delante de **cada** medida, y el A/B
tomado seguido revirtiendo **solo mi fichero**):

    sin      140.008 / 145.884 B   95,9721 %   534 funciones al 100 %
    con      140.008 / 145.884 B   95,9721 %   534 funciones al 100 %
    measure.py --cmp c25ecs_sin.json c25ecs_con.json -> +0 B, +0 funciones,
                                                        0 unidades cambian
    pct por funcion sobre las 2.014 del informe: CAMBIA UNA
      UpdatePlatInfo  98,27985 %  ->  **98,65949 %**   (2044/2044 B)

    audit.py  534 ok, CERO FALLA  (dos pasadas, la segunda tras el cambio)
    frozen.py chk: HA CAMBIADO (por mi cambio). NO he vuelto a congelar: no hay
    cierre, y el brief dice congelar solo al cerrar.

**Cero bytes.** Lo que traigo: (1) **un error de fuente real en `UpdatePlatInfo`,
localizado con el asm del objetivo y corregido** (+0,38 pp, el primer avance de
esa funcion desde la r22 que no sea quitar invenciones nuestras), (2) **la deuda
de `eProject` RESUELTA con medida** —el reorden de la r24 es correcto y lo
demuestra el DWARF—, (3) **`GenerateHorizonFog` cerrada por construccion con las
prioridades que imprime el propio compilador**, y (4) una trampa de porcentaje
detectada y **rechazada a proposito**.

## 0. AVISO: la premisa de mi encargo estaba caducada

El encargo decia: *«el commit `ef92ca4e` del permutador PERMUTO LA FUENTE de
`UpdatePlatInfo`; restaura el orden real y todas las `lfs`/`fmuls` casan fila a
fila»*.

**Eso ya estaba hecho, y lo hizo la r21.** `git log` de `eLightE.cpp` da
`ef92ca4e` -> `cf343e9e` -> `70d2f869`; la §2.1 de `r21-ecs.md` es exactamente
ese diagnostico (`c1` + `c2`, «c2 SE QUEDA»), y `c2` es lo que bajo la funcion de
99,235 % a 98,583 % **a proposito** para dejarla fiel. Verificado hoy sobre el
arbol, no heredado:

    dwbody.py  mismas locales, mismo orden de declaracion, mismo arbol de bloques
    regmap.py  43 iguales, 11 con registro distinto, 0 de ambito equivocado,
               0 solo del original, 0 solo nuestras
    rows       73 ARG_MISMATCH (nombres de registro) + 1 instruccion movida
               sobre 512 filas.  CERO INSERT/DELETE estructurales.

O sea: las `lfs`/`fmuls` **ya casaban fila a fila** al empezar mi ronda. Quien
escriba el brief de la r26 que no lo vuelva a repartir.

La regla general que si vale y he aplicado: **antes de diagnosticar, mira si el
permutador paso por el fichero.** Lo hice en los cinco ficheros; solo `eLightE.cpp`
tiene un commit del permutador y ya estaba revertido.

## 1. `UpdatePlatInfo` (2.044 B) — e1: la cadena de asignaciones iba al reves

### 1.1 La palanca: **el registro que recibe el `lfs` es el MAS INTERNO de la cadena**

En `a = b = c = V;` GCC 2.95 (C++) evalua `c = V` primero y **el valor de una
asignacion es un lvalue**, asi que los demas leen `c`. En el asm eso se ve sin
ambiguedad: **la carga del literal cae en el registro de `c`**, y salen tantos
`fmr` como variables restantes, **en el orden de la cadena de dentro a fuera**.

Objetivo, caso `0xA6348EE3` (linea 194 del original):

    8010A040  lfs f31, arnEnvMax@l(r9)     <- f31 = envmap_max_g   (el MAS INTERNO)
    8010A048  fmr f25, f31                 <- f25 = envmap_max_r   (el siguiente)
    8010A04C  fmr f8,  f31                 <- f8  = envmap_max_b   (el ultimo)

    => la fuente original es  envmap_max_b = envmap_max_r = envmap_max_g = arnEnvMax;

Nosotros teniamos `envmap_max_r = envmap_max_g = envmap_max_b = arnEnvMax;`, y el
asm lo delataba: `lfs f7` (nuestro `envmap_max_b`), luego `fmr f25(max_g), f7` y
`fmr f26(max_r), f7`. **La carga caia en la variable equivocada.**

La linea gemela de `min` (linea 201 del original) **ya era correcta**:
`lfs f1` = `envmap_min_g`, `fmr f30(min_r)`, `fmr f26(min_b)`, que es
`envmap_min_b = envmap_min_r = envmap_min_g = 0.4f;` = lo que teniamos.

### 1.2 Medida (e1)

    e1  envmap_max_b = envmap_max_r = envmap_max_g = arnEnvMax;   (eLightE.cpp:197)

    UpdatePlatInfo  98,27985 % -> **98,65949 %**   2044/2044 B
    regmap          envmap_max_r pasa a CASAR (f25 en los dos lados);
                    envmap_min_b f8 -> f27 (objetivo f26: un slot de distancia)
    rows            el bloque del case queda **estructuralmente identico** al
                    objetivo: lfs + fmr + fmr con los mismos destinos relativos
    A/B por funcion sobre las 2.014 del informe: **CAMBIA UNA**, esta.
    measure --cmp:  +0 B, 0 unidades cambian.  audit.py: 534 ok, cero FALLA.

**Se queda.** No es un cambio de porcentaje: es la forma de fuente que produce el
asm del objetivo, y ademas sube el porcentaje.

### 1.3 Por que NO cierra, con los numeros de `lreg.py` (rehecho hoy)

`lreg.py` de hoy (con `-G0` y con los pseudos de FPR, las dos correcciones de
esta sesion) sobre el objeto **recien compilado**, allocnos globales:

    pseudo  var                refs  live_len  pri   reg
    192     envmap_min_g       7     314       445   f31    (objetivo f1)
    203     envmap_max_g       7     325       430   f8     (objetivo f31)
    189     envmap_min_r       5     307       325   f30    (= objetivo)
    195     envmap_min_b       5     315       317   f27    (objetivo f26)
    200     envmap_max_r       5     318       314   f25    (= objetivo)
    206     envmap_max_b       5     326       306   f7     (objetivo f8)

`pri = floor_log2(n_refs) * n_refs / live_length * 10000` reproduce las seis
cifras al digito. La cadena de e1 vale **+2 refs** al mas interno (max_g paso de
5 a 7 refs, max_b de 7 a 5): es la palanca de `n_refs` que la r22 buscaba, y
**se mueve desde la fuente sin cambiar una instruccion**.

Lo que queda es **una cascada de un solo slot**, y esta acotada:

    objetivo   nuestro   variable
    f7         f6        diffuse_min_a
    f6         f5        diffuse_max_scale
    f5         f2        diffuse_rng_a
    f2         f1        envmap_power
    f1         f31       envmap_min_g
    f5         f8        envmap_min_scale     <-- LA RAIZ
    f8         f7        envmap_max_scale / envmap_max_b

`REG_ALLOC_ORDER` de FPR es `f0, f13, f12, f11, f10, f9, f8, f7, f6, f5, f4, f3,
f2, f1, f31…f14`. **La raiz es `envmap_min_scale`: se lleva f8 (7.º) y en el
objetivo se lleva f5 (10.º, compartido con `diffuse_rng_a`).** Todo lo demas baja
un escalon detras de eso. En el objetivo f8 lo ocupan `envmap_max_scale` y
`envmap_max_b`; en el nuestro lo ocupan `envmap_min_scale` y `envmap_max_g`.

Para cerrar hace falta que `envmap_min_scale` **no** encuentre f8 libre, o que
`envmap_max_g` suba a 8 refs (`floor_log2` salta de 2 a 3: `3*8/325 = 738`, por
encima de los 445 de `envmap_min_g`). **No he encontrado forma de fuente para
ninguna de las dos** y no he inventado codigo.

### 1.4 El mapa de lineas de `UpdatePlatInfo` va a la deriva (dato para el que siga)

El desfase `objetivo - nuestro` **no es constante**, asi que `lmap` no se puede
usar a ojo en esta funcion. Anclajes medidos hoy:

    funcion (stwu)        132 / 134   -2
    specular_magic        150 / 152   -2
    envmap_magic          161 / 163   -2
    switch                175 / 178   -3
    case 0xA6348EE3       193 / 196   -3
    case 0x471A1DCA break 221 / 223   -2
    case 0x8812634B pow=8 244 / 248   -4
    case 0x2388DD82 pow=6 296 / 301   -5
    diffuse_max_scale=0.3 300 / 307   -7
    diff_r del 0x33310077 338 / 338    0
    diffuse_min_r del mismo 344 / 343 +1
    envmap_min_ir         355 / 354   +1

O sea: **al original le sobran lineas en blanco respecto a nosotros dentro del
switch** (y a nosotros nos sobran 3 antes de el). Por la veda de la r22 («las
NOTE de linea NO cuentan en `live_length`») esto vale **cero bytes**, pero
cuadrar los doce anclajes convertiria `lmap` en una herramienta usable aqui. **No
lo he hecho**: son huecos, no codigo (el multiconjunto de instrucciones ya casa).

## 2. `eProject` (268 B) — la deuda de la r24: **RESUELTA, y el reorden se queda**

El encargo pedia cerrarlo con medida y no con opinion. La medida es esta,
`dwbody.py … both` sobre el objeto de hoy:

    ORIGINAL                          NUESTRO (r24, el actual)
    struct Vec local;  // r1+0x8      struct Vec local;  // r1+0x8
    struct Vec eye;    // r1+0x18     struct Vec eye;    // r1+0x18
    float oneOverW;                   float oneOverW;
    float clipX;       // f13         float clipX;       // f0
    float clipY;                      float clipY;
    float clipZ;       // f10         float clipZ;       // f10
    float halfVP2;     // f0          float halfVP2;     // f13
    float halfVP3;     // f13         float halfVP3;     // f12
    inline eRecip { recip//f9; val }  inline eRecip { recip//f9; val }

**El arbol es IDENTICO**: mismas locales, mismo orden de declaracion, mismo
ambito, misma expansion inline, y `clipY` sin registro **en los dos lados**.
`regmap.py`: **14 iguales, 3 con registro distinto, 0 de ambito equivocado, 0
solo del original, 0 solo nuestras.**

**Veredicto: el reorden de la r24 es correcto y se queda.** Los 3,03 pp que
cuesta no son una regresion: son el precio de que objdiff puntue el NOMBRE del
registro, y `matched_code` es todo-o-nada (85,82 y 88,85 valen los mismos **cero**
bytes). Volver al 88,85 % significaria enviar un orden de declaracion que
**contradice a la vez el DWARF y el mapa de lineas**, que es exactamente el
pecado de `ef92ca4e`. El comentario `// NON_MATCHING:` se queda como esta.

### 2.1 Y una trampa de porcentaje que he encontrado y RECHAZADO

Con el caso minimo (§5) barri 16 formas de las tres sentencias finales. Dos
suben:

    x1  *sx = vp[0] + halfVP2 + clipX * halfVP2 * oneOverW;   (operandos girados)
    y1  *sy = vp[1] + halfVP3 + -clipY * halfVP3 * oneOverW;
        mini: 24 filas distintas -> **22**

**No lo aplico, y esto es lo importante:** el objetivo emite
`fmuls f0, f0, f13` con `f0 = halfVP2` y `f13 = clipX`, o sea **`halfVP2 * clipX`**;
nosotros tenemos los dos registros al reves, asi que girar la fuente hace que el
TEXTO de la instruccion coincida **con los operandos semanticamente cambiados**.
Es porcentaje comprado con una permutacion de fuente: `ef92ca4e` otra vez. Queda
anotado como **veda: no girar los productos de `*sx`/`*sy` para ganar filas.**

### 2.2 Lo que falta en `eProject`, sin cambios respecto a la r24

    local      objetivo  nuestro
    halfVP2    f0        f13
    clipX      f13       f0
    halfVP3    f13       f12

Es `local_alloc` sobre FPR volatiles. Condicion (r24 §3.3, reproducida):
`halfVP2` con `live_length` 4 o `clipX` con 5. **Barridas y REVERTIDAS 16 formas**
(4 de `*sx` x 3 de `*sy` x 2 de `*sz`, mas 11 sueltas): ninguna mueve un solo
registro; la mejor baja de 24 a 22 filas y es la trampa de la §2.1.

**VEDA (medida en el caso minimo, 16 formas):** asociatividad y orden de
operandos de `*sx`/`*sy`/`*sz`, `0.5f * vp[n]`, `halfVP2 * (1 + …)`,
`vp[1] + halfVP3 - clipY * …`, declaraciones partidas de `halfVP2`/`halfVP3`, y
`clipX` con los sumandos girados. Ninguna toca el reparto.

## 3. `GenerateHorizonFogDisplayList` (796 B, 2 filas) — CERRADA POR CONSTRUCCION, con las cifras del compilador

Las dos filas son **una transposicion**:

    objetivo  … fmr f30,f22 · andi. r11,r31,0x1 · srawi r0,r0,1 · xoris …
    nuestro   … fmr f30,f22 · srawi r0,r0,1 · andi. r11,r31,0x1 · xoris …

La r21 y la r24 dijeron «es el planificador» sin numeros. Aqui estan, y salen del
**propio `cc1plus`**: `-fsched-verbose-5` (con guion, no `=`; `-fsched-verbose=N`
**no existe** en este build y da «Invalid option») junto a `-dR` imprime la tabla
de dependencias, las listas de listos por ciclo y la visualizacion por unidad
funcional. En el bloque de cabecera del bucle interno:

    insn  code  bb  dep  prio  cost  units      dependientes
    127   211    0   0    14    1    iu2     : 147 129      <- srwi
    129    52    0   1    13    1    iu2     : 147 131      <- add
    131   230    0   1    12    1    iu2     : 147 452      <- srawi
    449   523    0   0     2    1    fpu     : 147          <- fmr f30,f22
    146   182    0   0     2    1    [iu iu2]: 147          <- andi.
    147   763    0  13     1    1    bpu     :              <- beq

    Ready list (t = 3):  146  131
      --> scheduling insn <<<131>>> on unit iu2      (prioridad 12)
      --> scheduling insn <<<146>>> on unit [iu iu2] (prioridad 2)

Los dos se emiten en el **mismo ciclo 3**; lo que decide el orden en el fichero es
`rank_for_schedule`, y ahi **manda la prioridad**. `INSN_PRIORITY(andi.) = 2`
porque su **unico** dependiente en el bloque es el `beq`; `INSN_PRIORITY(srawi) = 12`
porque encabeza la cadena `xoris -> stw -> lfd -> fsub -> frsp -> fmuls f28`.

**Conclusion, y es un veto, no un «no lo encontre»:** mientras la conversion
`int->float` de `grid_pointX` viva dentro de ese bloque, ninguna forma de fuente
puede subir el `andi.` por encima de 12, porque su cadena hacia delante **no
existe**. Barridas 18 formas en el caso minimo; las **dos unicas** que ponen el
`andi.` delante (`v2`, `w1`, `w2`) son las que evaluan la ternaria de
`grid_pointY` **antes** de `grid_pointX`, y esas **parten el bloque** (la
conversion cae detras del `beq`) — que es justo la veda medida de la r18-plat
(`c16`, 91,482 %) y de la r24 (`g2`, 800 B).

**VEDAS NUEVAS (caso minimo, orden emitido comprobado en el `.s`):** `int odd =
i & 1;` izado (confirma la r21 h1), `if/else` en vez de ternaria (confirma la r24
g3), `i % 2`, `!(i & 1)` con brazos girados, `int multiple` declarado dentro del
bucle, `float fm = (float)multiple;` explicito, `(i/2)` repetido sin variable,
`+= j*spacing` fundido en la ternaria, el bloque `tex_*` antes del `grid_*`, y
`grid_pointX` guardado en un temporal. **Las diez son IDENTICAS al estado actual.**

## 4. `epCalculate` (2.072 B) y `Render` (696 B) — solo composicion

No he hecho ensayos en ninguna de las dos. Lo que si aporto es el reparto exacto
de sus diferencias, que ninguna ronda habia contado:

    epCalculate  531 filas: 124 ARG_MISMATCH + 13 INSERT + 13 DELETE + 5 REPLACE
    Render       174 filas:  32 ARG_MISMATCH +  2 DELETE (el stw+lwz que faltan)
    eProject      71 filas:  17 ARG_MISMATCH +  4 INSERT +  4 DELETE
                            (2 de los 25 «diffs» son el formato del destino del
                             `bne`/`b`: `0x16438` contra `eProject+0x64`, la MISMA
                             direccion. Cuentan como diff y no lo son.)

En `epCalculate` los 13+13+5 son **movimientos**, no instrucciones de mas ni de
menos (multiconjunto identico, como decia `triage`): son las `psq_l`/`ps_madd`
del bucle interno cambiadas de hueco. Sigue en pie el aviso de la r24: aunque se
cierren los 8 B de marco quedan ~106 filas de permutacion de `f2..f13`.

## 5. Herramientas

- **`-fsched-verbose-5` + `-dS`/`-dR` llamando a `cc1plus` a mano.** Es la receta
  del brief, y en este build **la sintaxis es con guion**: `-fsched-verbose=3`
  sale con «Invalid option» y se lee como «no hay verbose». Da la tabla
  `insn/code/bb/dep/prio/cost/units`, la lista de listos por ciclo y quien se
  programa en cada unidad. Cerro el diagnostico de `GenerateHorizonFog` en 20
  minutos despues de tres rondas de conjeturas.
- **Dos casos minimos nuevos**, los dos a **~1 s por compilacion** y los dos
  reproducen el `.s` de la unidad **instruccion a instruccion**:
  `scratchpad/mini/` (bucle de `GenerateHorizonFog`, con `sweep.py` que dice si
  el `andi.` sale delante) y `scratchpad/mini2/` (`eProject` entero, con
  `score.py` que **puntua contra el asm del objetivo** normalizando registros y
  literales: baseline 24 filas). Construir zEcstasy entero tarda **16 s**, asi que
  el caso minimo gana por 16x y ademas deja hacer volcados RTL sin tocar el
  `build/` compartido.
- **`c25ecs_rows.py`** (scratchpad): filas objetivo|nuestro alineadas con su
  `diff_kind`. El JSON de `objdiff-cli` anida las instrucciones en
  `instruction.formatted`, no en `mnemonic`/`arguments`: un parser hecho de
  memoria saca `?` en todas las filas **sin dar error**.
- **`lreg.py` reproduce el reparto de zEcstasy al digito** con las dos
  correcciones de esta sesion. Aviso de lectura: su columna `reg` son **numeros
  de registro duro**, o sea `r32..r63` = `f0..f31` (`r44` = f12, `r63` = f31), y
  los pseudos de FPR salen con `pref ?`.
- **`regmap.py`/`dwbody.py` quieren el nombre con clase** y hay que borrar
  `build/regmap/our_zEcstasy_*` antes de cada pasada (sigue vigente de la r24).
- **Disco**: empece con 14 GB y termino con 13 GB. Los 555 MB de
  `build/GOWE69/lreg` son cache compartida de varios agentes: no la he borrado.

## 6. Que NO he probado

- **`epCalculate` (2.072 B): cero ensayos.** Ni la ranura fantasma de 8 B ni las
  tres locales `dcrg`/`dcba`/`scba`. Con `-fsched-verbose-5` ya disponible, sus
  13+13+5 movimientos se pueden atribuir a ciclos concretos como he hecho en
  `GenerateHorizonFog`; **eso es lo que yo haria primero en la r26.**
- **`Render` (696 B): cero ensayos.** Sigue el diagnostico de la r24 (al objetivo
  le sobra una decision de derrame con un registro libre).
- **`UpdatePlatInfo`, la raiz de la §1.3**: no he encontrado forma de fuente que
  quite f8 a `envmap_min_scale` ni que le de una 8.ª referencia a
  `envmap_max_g`. Tampoco he probado los tramos `d = -5..-4, +2..+5, +8..+12` de
  la ventana de la r22, que siguen sin tocar desde entonces.
- **Las lineas en blanco del §1.4**: no las he cuadrado. Doce anclajes dados.
- **El permutador**, ni guiado ni ciego, en ninguna funcion. Sigue sin usarse en
  esta unidad desde la r21.
- **`frozen.py cong`**: no he vuelto a congelar (no hay cierre).
- **Extrapolar la §1.1 al resto del arbol**: contado, no supuesto. En todo
  `src/Speed/*/Src/Ecstasy/` hay **cuatro** cadenas de asignacion mas
  (`EcstasyE.cpp:274` y `:1954`, `EmitterSystem.h:28` y `:100`) y **las cuatro
  estan en codigo que ya casa al 100 %**. Es un caso, no un frente. Fuera de
  Ecstasy no lo he contado.

## 7. Lo que queda en el arbol

**Un solo fichero y una sola linea**:
`src/Speed/GameCube/Src/Ecstasy/eLightE.cpp:197`

    -        envmap_max_r = envmap_max_g = envmap_max_b = arnEnvMax;
    +        envmap_max_b = envmap_max_r = envmap_max_g = arnEnvMax;

`measure --cmp`: +0 B, 0 unidades cambian. `pct` por funcion: cambia **una**.
`audit.py`: 534 ok, cero FALLA en dos pasadas.
