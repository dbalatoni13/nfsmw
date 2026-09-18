# Ronda 20 — `LibSN/steering` y `egami/.../madidct`

Encargo: 6.240 B en dos unidades. **La tabla del encargo estaba caducada.**

## 0. La línea base real, medida al empezar

El encargo daba `LibSN/steering` al **41,9 %** con 5.092 B pendientes. Ésa es la
cifra de la ronda 18. En medio, la rama `agent/mw-steering-r2` cerró
`Effect_UpdateEffect` (808 B, 69,08 -> 100 %) y subió `Effect_Init`
(29,23 -> 88,62 %) y `VDevice_GetFreeEffect` (77,07 -> 92,41 %). El fichero
`base_r20_big.json` **sí** trae el estado correcto y coincide exactamente con mi
medida inicial:

| unidad | exacto | total | % | pendiente |
|---|---:|---:|---:|---:|
| LibSN/steering | 4.476 B / 25 fn | 8.760 B / 36 fn | 51,0959 % | **4.284 B** |
| egami/.../madidct | 356 B / 1 fn | 1.504 B / 3 fn | 23,6702 % | 1.148 B |

Es decir **5.432 B**, no 6.240, y `steering` está al 51 %, no al 41,9 %.

Tres encargos del brief ya estaban hechos, y los he verificado en vez de
repetirlos:

* `VDevice_RecalcGammaTable` — el parche c18 está aplicado (99,796 %, 216/216 B).
* `Effect_UpdateEffect` — el `divw`/`divwu` está corregido y **cerró al 100 %**
  en la ronda anterior. No lo he tocado.
* `Effect_Init` — ya no está al 29 % sino al 88,62 %.

`audit.py` da **rc=0** en las dos unidades al empezar (dos pasadas) y
`frozen.py chk` dice «idéntico al congelado» en las dos.

## 1. Lo que cierra: `LGDownloadForceEffect`, 204 B — y el eje que lo abre

**El hallazgo de la ronda es que mwcc obedece los `#pragma` por función, y ese
eje no lo había tocado nadie.** Las rondas 18 y 19 barrieron *banderas de
unidad* (14 versiones x 7 niveles x 11 sub-banderas, mas `-inline` y `-opt`) y
concluyeron, con razón, que el óptimo global es el actual. Pero una bandera de
unidad tiene que ser buena para las 36 funciones a la vez; **un pragma regional
sólo tiene que serlo para una**.

`LGDownloadForceEffect` llevaba desde la ronda 18 en 99,510 % con una
transposición limpia `r3 <-> r7` en las dos expansiones de `LG_VALID`,
catalogada como muro del asignador. La causa era la CSE global: mwcc reconoce
que `dev != (LGDEVICE *)-1` se calcula dos veces (la del `if` de entrada y la de
`LG_SHUTDOWN`), la comparte, y al compartirla reparte los registros al revés que
el objetivo.

    p0  fuente actual                            ->  99,50980 %  204 B
    p1  #pragma scheduling off                   ->  95,05882 %
    p2  #pragma peephole off                     ->  93,33334 %  216 B
    p3  #pragma global_optimizer off             ->  96,47059 %
    p4  #pragma optimization_level 1             ->  83,03921 %  224 B
    p5  #pragma optimization_level 3             ->  99,50980 %  (sin cambio)
    p6  #pragma optimization_level 2             -> 100 %        204 B
    p7  #pragma opt_common_subs off              -> 100 %        204 B   <- queda

    (opt_lifetimes, opt_loop_invariants, opt_propagation, opt_strength_reduction,
     opt_dead_code, optimize_for_size, opt_dead_assignments, opt_unroll_loops,
     opt_vectorize_loops, load_store_elimination, no_register_coloring,
     opt_pointer_analysis, pool_data, use_lmw_stmw, fp_contract, once
     -> 99,50980 %, todas)

p6 y p7 dan **el mismo objeto**: el nivel 2 apaga la CSE global, así que es el
mismo mecanismo. Se queda p7 por ser el pragma estrecho. El precedente en este
mismo fichero es `Effect_Init`, que la ronda anterior envolvió en
`#pragma optimization_level 2`.

**Resultado: +204 B y +1 función.** `python scripts/audit.py LibSN/steering`
pasa (204 B, 14 ramas, 2 reubicaciones comprobadas contra el ELF original).

## 2. Correcciones estructurales que **no** cierran pero quitan diferencias reales

Las dejo en el árbol por el mismo criterio que la ronda 19 usó con sus tres
(seccion 2 de `r19-libs4.md`): cada una elimina diferencias de **opcode o de
registro** verificadas, no de porcentaje. Ninguna mueve a las vecinas: la unidad
sigue en 26 funciones al 100 % en todas las medidas.

### 2.1 `SimThread_Step` (924 B): 92,355 -> 95,671 %, y dos causas resueltas

Nadie la había abierto: no hay un solo ensayo suyo en `r18-libsn.md` ni en
`r19-libs4.md`. Tenía **dos diferencias de forma reales**, las dos cerradas.

**(a) El modo de direccionamiento del bucle de suma.** El objetivo emite
`addi`+`lwzx` sobre `st`, y nosotros `add`+`lwz` con desplazamiento:

    objetivo: slwi r4,r7,2 ; addi r7,r7,1 ; addi r3,r4,0x68 ; addi r0,r4,0xa8
              lwzx r3,r28,r3 ; lwzx r0,r28,r0
    nuestro:  slwi r0,r6,2 ; addi r6,r6,1 ; add r4,r28,r0
              lwz r3,0x68(r4) ; lwz r0,0xa8(r4)

La causa: con `st->dbuf[j]` y `st->tbuf[j]` el índice es **el mismo**, así que
`st + j*4` es una subexpresión común con dos usos y sale más barata. En el
objetivo no lo es. La forma que lo reproduce es el post-incremento en el segundo
acceso:

    p1  j++ como sentencia suelta                -> 92,355 %  916 B
    p2  timeSum += st->tbuf[j++];  wrap sobre j  -> 93,398 %  920 B  <- queda
    p3  posSum = timeSum = 0                     -> 92,377 %  916 B
    p4  temporales para los dos elementos        -> 92,333 %  916 B
    p5  timeSum = 0 antes de posSum = 0          -> 92,377 %  916 B
    p6  while en vez de for                      -> 92,355 %  916 B
    p7  ++j fuera del if                         -> 92,355 %  916 B

Con p2 las **nueve** instrucciones del cuerpo del bucle coinciden opcode a
opcode con el objetivo; sólo cambian los registros.

**(b) `st->pos - v` se calcula antes del `if`, no después.** El objetivo emite
`subf r4,r5,r4` en el bloque *anterior* al `bge`, o sea antes de la corrección
de `st->bufIndex`; nosotros dentro del bloque siguiente. Es posición de
sentencia:

    q1  v = st->pos - v; antes del st->bufIndex-- -> 95,346 %  920 B
    q2  q1 + posSum = timeSum = 0                 -> 95,368 %  920 B  <- queda
    q3  sólo los ceros encadenados                -> 93,398 %  920 B
    q4  q1 + los ceros antes de j                 -> 95,346 %  920 B
    q5  q1 con una local nueva en vez de reusar v -> 95,368 %  920 B

**(c) El orden de declaración decide el reparto de registros salvados.** El
objetivo pone `dt` en r31 y `polarSum`/`plainSum` en r30/r29; nosotros al revés.
Declarar `dt` antes de `polarSum` lo arregla:

    t1  dt declarada antes de polarSum            -> 95,649 %
    t2  dt la primera de todas                    -> 95,649 %
    t3  k antes de e                              -> 94,459 %
    t4  dt la última                              -> 95,368 %
    t5  i = 0 antes de v = 0 en el else           -> 95,390 %
    t6  polarSum/plainSum al final                -> 95,649 %
    u1  t1 + t5                                   -> 95,671 %  <- queda

**Lo que queda, y por qué es muro.** Tres filas, todas de la misma familia: el
objetivo materializa el 0 **una vez** y lo copia
(`li r8,0 ; mr r5,r8 ; mr r6,r8`) y nosotros emitimos tres `li`; y el objetivo
parte el rango de vida de `dt` (`subf r7,...` y luego `mr r31,r7`) donde
nosotros calculamos directo en el salvado. **Barrido con cifra y vedado**: siete
formas de inicializar los ceros, incluida `timeSum = i; posSum = i;` —copiar
explícitamente de una variable que vale cero— y mwcc **rematerializa igual** en
las siete:

    w1 i=0 primero              -> 95,649 %
    w2 i=0 antes de j           -> 95,649 %
    w3 i=0 suelto al final      -> 95,649 %
    w4 posSum = timeSum = i = 0 -> 95,649 %
    w5 i = posSum = timeSum = 0 -> 95,671 %
    w6 coma dentro del for      -> 95,649 %
    w7 timeSum = i; posSum = i  -> 95,649 %

Ninguna produce el `li`+`mr`.

### 2.2 `SimThread_Init` (324 B): 90,741 -> 94,691 %

También sin ensayos previos. 324/324 B exacto; sólo planificación y un par de
registros. **El bucle de `ia[]` usa en el original un índice propio**, no el `i`
compartido con los otros tres bucles:

    x1  st->histIndex = 0 tras el bucle de ia     -> 87,346 %  320 B
    x2  a[i] * 4096.0f                            -> 90,741 %  (idéntico)
    x3  hist antes que coeff en el 3er bucle      -> 85,914 %
    x4  while                                     -> 90,741 %  (idéntico)
    x5  do/while                                  -> 87,160 %
    x6  sin el histIndex = 0 intermedio           -> 92,222 %  316 B (pierde 2 insn: falso)
    y1  temporal float                            -> 90,741 %
    y2  escala 4096.0f en una local               -> 90,741 %
    y3  índice propio `n` para el bucle de ia     -> 94,691 %  324 B  <- queda
    y4  sin la línea en blanco                    -> 90,741 %
    z1  índice propio también en el 3er bucle     -> 94,753 %  DESCARTADA
    z3  índice propio en el bucle de LG_HISTORY   -> 91,790 %
    z4  los dos stores del 3er bucle al revés     -> 89,864 %
    aa1 do/while con n                            -> 85,185 %  328 B
    aa2 while con n                               -> 94,691 %  (idéntico a y3)
    aa3 literal double 4096.0                     -> 96,420 %  DESCARTADA
    aa4 división por 0.000244140625f              -> 93,951 %

Con y3 el precabezal del bucle coincide **instrucción a instrucción** hasta el
`addi r4,r3,ia@l`. Quedan **dos** cosas: el `lfs f1` va antes de
`li r3,0`/`mtctr` y en el objetivo después (desempate del planificador), y el 0
del último bucle cae en r6 en vez de r5.

**Dos ejemplos más de la trampa del porcentaje, y los dos del lado malo:**

* `z1` sube a 94,753 % **reordenando los dos stores** del cuerpo del bucle, que
  antes estaban en el orden del objetivo. Peor código, mejor número.
* `aa3` sube 1,7 pp hasta 96,420 % **cambiando `lfs`->`lfd` y `fmuls`->`fmul`**:
  dos errores de opcode contra un objetivo que es claramente de precisión
  simple. Es el peor caso visto del fenómeno.

### 2.3 `VDevice_GetFreeEffect` (116 B): el acumulador del OR

116/116 B. El objetivo empieza la cadena de OR por `(createcount & 0xFF) << 8`
(`clrlslwi`) e inserta `chan << 16` con `rlwimi`; nosotros al revés. **Se
arregla poniendo el término de `chan` primero en la fuente**, porque mwcc toma
como acumulador el *segundo* operando:

    g0  cc | chan | idx  (fuente anterior)        ->  92,414 %  7 filas distintas
    g1  chan | cc | idx                           ->  88,966 %  4 filas   <- queda
    g2  cc | (chan | idx)                         ->  87,069 %
    g3  idx | cc | chan                           ->  87,586 %
    g4  idx | chan | cc                           ->  92,069 %
    g5  tres sentencias con |=                    ->  83,448 %
    g6  chan primero, con |=                      ->  84,655 %
    g7  paréntesis explícitos por la izquierda    ->  92,414 %  (= g0)
    g8  sin la máscara de idx                     ->  87,069 %  112 B

**g1 deja CERO diferencias de registro** (g0 tiene tres) y hace coincidir
`clrlslwi r0,r0,24,8` y `rlwimi r0,r3,16,0,15`. Lo único que queda es una
rotación de tres instrucciones alrededor de la carga volátil de `createcount`:
el objetivo mete `lwz r3,0xf0(r3)` **antes** de esa carga y nosotros después.

Barrido y vedado sobre esa rotación —ninguna forma la mueve, las nueve marcadas
dan 88,966 %—:

    h1  createcount sin volatile                  ->  82,586 %  (la iza demasiado)
    h2  sólo quitar volatile                      ->  87,586 %
    h3  los dos statics sin volatile              ->  79,310 %  112 B
    h4  createcount const volatile                ->  80,000 %  104 B
    h5  createcount volatile u8                   ->  84,138 %
    k1  vd->dev en una local antes                ->  88,966 %
    k2  vd->dev->chan en una local antes          ->  88,966 %
    k3  k1 con el orden de OR anterior            ->  92,414 %
    k4  k2 con el orden de OR anterior            ->  92,414 %
    k5  (createcount << 8) & 0xFF00               ->  88,966 %
    k6  g1 sin la máscara de idx                  ->  83,448 %  112 B
    k7  chan | idx | cc                           ->  87,069 %
    n1  jumbleeffectid &= 0x3F                    ->  88,966 %
    n2  (u32)createcount                          ->  88,966 %
    n3  (u32)vd->dev->chan                        ->  88,966 %
    n4  (i + j) & 0x3F en vez de idx              ->  83,448 %  112 B
    n5  los dos statics declarados al revés       ->  88,966 %

Nota: `createcount` **tiene que seguir siendo `volatile`**. Sin él (h1/h2/h3)
mwcc iza su carga por encima de los dos stores de `jumbleeffectid`, que es
exactamente lo que la ronda anterior arregló poniéndolo.

## 3. `VDevice_RecalcGammaTable`: el muro, ahora **demostrado**, no supuesto

Las rondas 18 y 19 gastaron **veinte** ensayos aquí y concluyeron «cinco formas
distintas dan el mismo reparto de ranuras». Eso no prueba que no haya forma.
**Esto sí lo prueba**, y cuesta una compilación:

Compilé la función **con una sola rama** (quitando el `if (i < 0)`, que es
código muerto: `i` va de 0 a 128), de modo que sólo quedan **dos** conversiones,
una int->double y una double->int. Leído del objeto resultante:

    stw r30, 0x8(r1) ; stw r0, 0xc(r1) ; lfd f0, 0x8(r1)     <- int->double  en 0x8
    stfd f0, 0x10(r1) ; lwz r0, 0x14(r1)                     <- double->int  en 0x10

**Dos conversiones, dos ranuras.** El asignador de temporales de mwcc con estas
banderas **nunca reutiliza** una ranura de conversión, ni siquiera cuando sólo
hay dos y no se solapan en absoluto. El objetivo mete las **tres** (dos
int->double mutuamente exclusivas y un `fctiwz`) en `0x8(r1)`, y por eso su
marco es 0x20 frente a nuestro 0x30. Ninguna forma de fuente puede cambiar eso,
porque la decisión no depende de la fuente. Es el único fallo que le queda a la
función: las 54 instrucciones y todos los registros coinciden.

Los 30 pragmas de la sección 7 tampoco lo mueven (99,796 % en todos los que no
rompen). **Frente cerrado**: si alguien quiere estos 216 B, el camino es el
compilador o las banderas de unidad, no la fuente.

## 4. `CookValues` (260 B): la rematerialización del `extsb`

`extsb-1 mr+1`. El objetivo **vuelve a extender el byte crudo** (`extsb r0,r5`)
para `_mx = _v`, y nosotros copiamos el valor ya extendido (`mr r0,r6`). Es
asimétrico: para `_mn = _v` el objetivo **sí** copia (`mr r4,r6`), igual que
nosotros. Esa asimetría es la prueba de que no es un cast de la fuente.

La ronda 19 no gastó ensayos aquí «por ser la misma familia que
`HandleTriggers`». He gastado cuatro, midiendo también `HandlePedals` y
`HandleTriggers` porque comparten `AUTOCAL`:

    c_mx_relee_vv  `_mx = (vv)`  -> CookValues 94,231 %  HandlePedals 99,062 % REGRESA
    c_mx_cast_T    `_mx = (T)_v` -> CookValues 95,769 %  HandlePedals 100 %  objeto idéntico
    c_mx_relee_mx  `_mx = (mx)`  -> CookValues 94,231 %  HandlePedals 99,062 % REGRESA
    c_mn_relee_vv  `_mn = (vv)`  -> CookValues 93,385 %  HandlePedals 99,062 % REGRESA

**Veda**: barridas las tres formas de rematerialización expresables en la
fuente (releer el campo, castear a `T`, releer el destino). El cast a `T`
**compila idéntico** —mwcc lo pliega— y las otras dos tiran `HandlePedals` del
100 %.

## 5. `VDevice_DownloadEffect` (188 B): permutación de locales, agotada

Transposición limpia r30<->r31 (`ret` y `idx`). Probadas **las seis**
permutaciones de la declaración, más dos de inicialización:

    ret,e,idx (actual) 99,362 %   ret,idx,e 99,362 %   idx,ret,e 99,362 %
    e,ret,idx 98,511 %            e,idx,ret 98,511 %   idx,e,ret 98,511 %
    ret asignada tras *pid  95,213 %      e = &vd->pool[idx]  98,936 %

Y los 30 pragmas de la sección 7: ninguno pasa de 99,362 %. Confirma la ronda 18.

## 6. `madidct`: diagnóstico más fino, y sigue siendo muro

La ronda 19 dejó dicho que el DWARF trae las dos funciones enteras y coincide
variable a variable, así que **no falta código**. Confirmado. Lo que añado:

**`IdctColumn` (632/632 B).** La diferencia son dos recargas nuestras. En la
cadena del `if ((src[1] | ... | src[7]) == 0)` el objetivo **derrama `src[4]` a
`0x8(r1)` y a la vez lo conserva vivo en r5** para el `or` inmediato; nosotros
derramamos, reutilizamos el registro para `src[5]` y **recargamos**
(`lwz r11, 0x8(r1)`). Es la pasada de *reload* de GCC 2.95.

He barrido lo único que la ronda 19 no tocó, la propia cadena de OR:

    m1  src[4] al final de la cadena              ->  48,563 %  628 B
    m2  src[4] al principio                       ->  47,905 %
    m3  la cadena al revés (7...1)                ->  50,361 %
    m4  con paréntesis por pares                  ->  47,171 %  636 B
    m5  `if (!(...))` en vez de `== 0`            ->  50,449 %  (idéntico)

Nota lateral verificada: el bloque de salida rápida **recarga `src[0]` después de
cada store** (ocho `lwz` + ocho `stw`) en los dos lados, porque `dest` y `src`
pueden solaparse. Eso ya casa y no hay nada que hacer ahí.

**`IdctRow` (516 B objetivo / 496 B nuestro).** El objetivo tiene **cinco
instrucciones más**: derrama `src[6]` a **CTR** (`mtctr` + dos `mfctr`) *además*
del bit de acarreo a LR, y hace dos `mr` que nosotros no necesitamos. Es decir,
**su asignador va más apretado que el nuestro**: no es código que nos falte, es
presión de registros que no reproducimos.

**El permutador ciego, lanzado por primera vez sobre `IdctColumn`** (300
variantes de un solo cambio, 6 rondas, recocido 0,004, 0,25 s por variante):
sube su puntuación de 0,2595 a 0,3813 y el multiconjunto de opcodes de 0,677 a
0,788, con 160 instrucciones frente a 158. **No cierra**, y las rondas 5 y 6
sólo encuentran pasos laterales. Séptima ronda del permutador sin cerrar nada.

`madidct` se compila con **ProDG 3.9.3 (GCC 2.95)**, no con mwcc, así que el eje
de pragmas de la sección 7 **no existe** aquí. Los únicos ejes que quedan son
las restricciones de registro y las banderas de unidad.

## 7. El eje nuevo, completo: `#pragma` por función (sólo mwcc)

Verificado que mwcc los obedece y que un pragma mal escrito **rompe la
compilación** (`-maxerrors 1`), así que la lista de abajo es de pragmas
realmente aceptados, no de nombres inventados.

Comprobado de paso que `scheduling 750` y `scheduling 7400` dan **exactamente el
objeto actual**, lo que confirma que `-proc gekko` usa el planificador correcto
y cierra esa duda: 601, 603, 604, 8240, 505 y `off` empeoran **todas** las
funciones.

Barrido sobre **las diez** funciones pendientes, 30 pragmas cada una:

    scheduling off / 601 / 603 / 604 / 750 / 7400 / 8240 / 505
    opt_common_subs, opt_lifetimes, opt_loop_invariants, opt_propagation,
    opt_strength_reduction, opt_dead_code, opt_dead_assignments,
    opt_unroll_loops, opt_vectorize_loops, opt_pointer_analysis,
    optimize_for_size, peephole, global_optimizer, load_store_elimination,
    no_register_coloring, pool_data, use_lmw_stmw, fp_contract, once,
    optimization_level 1 / 2 / 3

**Un solo acierto**: `opt_common_subs off` (y su equivalente
`optimization_level 2`) en `LGDownloadForceEffect`. Todo lo demás deja la
función igual o peor. Barridas también **las 66 parejas** de los 12 pragmas más
activos sobre las diez funciones: **ningún acierto nuevo**.

Aviso para quien siga: el pragma hay que **cerrarlo** después de la función
(`#pragma opt_common_subs on`). Si se deja abierto contamina el resto de la
unidad; en mi primer barrido, un `#pragma scheduling off` sin cerrar tiró la
unidad de 25 funciones a 14 sin que la función bajo prueba cambiara.

## 8. Lo que NO he probado

* **No he tocado `ppc2D2`**, que entra en la línea base y no es mío. Ni
  `configure.py`, ni `config/GOWE69/*`, ni banderas de unidad, ni ninguna
  cabecera compartida. Los dos ficheros modificados son
  `src/LibSN/steering.c` y nada más (`madidct.cpp` queda **idéntico a HEAD**).
* **Restricciones de registro** (`register ... asm("rN")`): ninguna. Los cuatro
  candidatos naturales —`Effect_Update`, `HandleTriggers`, `IdctRow`,
  `IdctColumn`— son de asignación pura y el brief las permite sólo tras agotar
  la búsqueda de fuente. Documentada queda; no las he aplicado.
* **`Effect_Update` (992 B) y `HandleTriggers` (588 B)**: no he gastado ensayos
  de fuente. Las rondas 18 y 19 les dedicaron 15 y 11 respectivamente y las dos
  quedaron vedadas con cifra; me he limitado a pasarles el eje de pragmas
  (nada) y a comprobar que mis cambios no las mueven.
* **`Effect_Init` (276 B)**: sólo el eje de pragmas. Su problema conocido —el
  objetivo iza `lis 0x4330` a un sexto registro salvado (r26-r31) y nosotros lo
  rematerializamos dentro del bucle con cinco (r27-r31)— sigue abierto y no le
  he buscado forma de fuente.
* **`Effect_PerformEnvelope` (196 B)**: un solo ensayo. El objetivo no fusiona
  `out` y `mag` (emite `mr r5,r9`) y nosotros sí; partir `out = mag = ...` en
  dos sentencias **compila idéntico** (93,265 %, 192 B), que era la hipótesis
  obvia. No he buscado más.
* **El permutador sobre `IdctRow`**, sobre las funciones de `steering`, y con
  `--guided`: sólo lancé el ciego sobre `IdctColumn`.
* **La cadena completa**: ninguna de las dos unidades está enlazada, así que el
  DOL no puede cambiar; **no he ejecutado la construcción completa ni he
  verificado el DOL**.
* **No he ejecutado nada**: todo son bytes e instrucciones.

## 9. Resultado medido

`build_direct.py` antes de **cada** medida (`ninja` no construye ninguna de las
dos unidades). `base_r20_big.json` -> `despues_r20_big.json`, con `ppc2D2`
incluido en las dos por estar en la línea base:

    3 unidades  6296/12712 B  49,5280%  37 funciones al 100%

    measure.py --cmp base_r20_big.json despues_r20_big.json
        +204 B, +1 funciones, 1 unidades cambian
        +204 B  +1 fns  LibSN/steering   4476 -> 4680

    LibSN/steering   4.476 B / 25 fn  ->  4.680 B / 26 fn   51,0959% -> 53,4247%
    egami/.../madidct  356 B /  1 fn  ->    356 B /  1 fn   sin cambio
    main/ppc2D2      1.260 B / 10 fn  ->  1.260 B / 10 fn   sin cambio (no es mío)

`ppc2D2` y `madidct` **no se han tocado**; `madidct.cpp` es idéntico a HEAD.

`python scripts/audit.py` da **rc=0 y cero FALLA en las dos unidades, en dos
pasadas**. Las dos quedan congeladas con `frozen.py cong` y `chk` responde
«idéntico al congelado»:

    LibSN/steering                              74daa0c732497cde
    egami/rcmp/dev/source/decoder/cmn/madidct   ba38170839a98a47

### Estado por función al terminar

    992 B  99,839%  Effect_Update              muro (r6/r7, vedado r18)
    924 B  95,671%  SimThread_Step             92,355 -> 95,671   (+3,3)
    588 B  94,898%  HandleTriggers             muro (4 mr, vedado r19)
    324 B  94,691%  SimThread_Init             90,741 -> 94,691   (+4,0)
    276 B  88,623%  Effect_Init                sin ensayos de fuente
    260 B  95,769%  CookValues                 veda nueva (seccion 4)
    216 B  99,796%  VDevice_RecalcGammaTable   muro DEMOSTRADO (seccion 3)
    196 B  93,265%  Effect_PerformEnvelope     un ensayo
    188 B  99,362%  VDevice_DownloadEffect     seis permutaciones, agotado
    116 B  88,966%  VDevice_GetFreeEffect      92,414 -> 88,966 %, y MEJOR
                                               (cero diferencias de registro)

Y lo que dice `triage.py`, que es la medida honesta de las tres correcciones
que no cierran:

    antes:  SimThread_Step        faltan 6, sobran 4, 5 SUST   mr+2 lwz-2 lwzx+2 addi+2
    ahora:  SimThread_Step        faltan 2, sobran 1, 2 SUST   mr+2 li-1
            (la familia entera del direccionamiento ha desaparecido)

    antes:  SimThread_Init        faltan 8, sobran 8, 2 SUST
    ahora:  SimThread_Init        faltan 8, sobran 8, 0 SUST

    antes:  VDevice_GetFreeEffect faltan 6, sobran 6, 3 SUST   slwi-1 clrlslwi+1
    ahora:  VDevice_GetFreeEffect faltan 5, sobran 5, 2 SUST   (sin delta de opcode)

    MURO:   3 funciones / 1.384 B   ->   2 funciones / 1.180 B
