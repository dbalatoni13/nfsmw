# Ronda 22 — `LibSN/steering`

Encargo: la unidad con el porcentaje mas bajo del arbol (53,4247 %), 4.080 B
pendientes. Metrowerks CodeWarrior GC/2.7, `-O4,s -proc gekko -inline off
-use_lmw_stmw on`.

## 0. Linea base VERIFICADA (coincide con el encargo, sin caducar)

`build_direct.py LibSN/steering` + `triage.py` al empezar:

    216 B   99.796%  VDevice_RecalcGammaTable   faltan 10, sobran 10, 0 SUST
    324 B   94.691%  SimThread_Init             faltan  8, sobran  8, 0 SUST
    276 B   88.623%  Effect_Init                faltan  6, sobran  6, 1 SUST
    116 B   88.966%  VDevice_GetFreeEffect      faltan  5, sobran  5, 2 SUST
    588 B   94.898%  HandleTriggers             faltan  0, sobran  4          mr-4
    924 B   95.671%  SimThread_Step             faltan  2, sobran  1, 2 SUST  mr+2, li-1
    260 B   95.769%  CookValues                 faltan  1, sobran  1          extsb+1, mr-1
    196 B   93.265%  Effect_PerformEnvelope     faltan  1, sobran  0          mr+1
    MURO: 2 funciones, 1180 B

    measure.py  ->  4680/8760 B  53,4247 %  26 funciones al 100 %

Coincide con la tabla del encargo instruccion a instruccion. **Un solo apunte:
la correccion `g1` de `VDevice_GetFreeEffect` de la ronda 20 SI estaba
aplicada** (el fuente ya tenia `chan` primero en el OR y la funcion medía
88,966 %, no 92,414 %); el encargo decia que no lo estaba.

`audit.py LibSN/steering`: **rc=0, 26 funciones ok, cero FALLA, en dos pasadas**.
`frozen.py chk`: «identico al congelado».

## 1. El mecanismo de la ronda: **la variable de ambito de funcion decide si mwcc fusiona la copia**

`HandleTriggers` tenia `sobran 4` — cuatro `mr` que el objetivo no emite, uno por
cada una de las cuatro expansiones de `AUTOCAL`. La copia es la inicializacion
`_mx = (mx)`: como la condicion externa (`raw->left > cal->max.left -
cal->dead.left * 2`) ya cargo `cal->max.left`, mwcc reutiliza esa carga por CSE y
`_mx = <temporal del CSE>` sale como `mr`. **El objetivo no tiene copia porque su
`_mx` ES el registro del CSE.**

### Caso minimo (40 lineas, 1 s por compilacion)

Reproduje el fallo exacto fuera del arbol: dos expansiones de `AUTOCAL` en ramas
mutuamente excluyentes cuya condicion externa lee el mismo campo `mx`. Base: 76
instrucciones, 6 `mr` (2 de ellos la copia `mr r0, r8`). Con **una sola**
expansion y cuerpo corto no hay copia; con el cuerpo completo si.

Barridos sobre el caso minimo, ninguno cierra:

* **orden de inicializacion** de `_v`/`_mn`/`_dz`/`_mx` (6 permutaciones),
* **orden de declaracion** de `_mn`/`_v`/`_mx` (las 6 permutaciones), `_r`/`_dz`
  primero,
* **tipos**: `_mx` u8, `_mn` u8, `_v` u8, `_dz` int,
* **local contra campo directo** para `_v`/`_mn`/`_mx`: las **8** combinaciones,
* **forma de los `if`**: `_mn = (mn) = _v`, `(mn) = _mn = _v`, orden
  `stb`/`mr` invertido, `else` explicito, ifs intercambiados,
* **forma de `_t`** y del `if (_r == 0)`,
* **37 pragmas** (`scheduling` x8, `opt_*` x18, `optimization_level` 0-4,
  `no_register_coloring`, `peephole`, `global_optimizer`, `pool_data`,
  `use_lmw_stmw`, `fp_contract`, `optimize_for_size`).

### Lo que SI mueve la fusion: la forma de `_r` — y por que no vale

    _r = (_mx - _mn) - _dz * 2;     -> 76 insn, COPIA   (base)
    _r = _mx - _dz * 2 - _mn;       -> 74 insn, limpio
    _r = (_mx - _dz * 2) - _mn;     -> 74 insn, limpio
    _r = _mx - (_mn + _dz * 2);     -> 74 insn, limpio
    _r = _mx - (_dz * 2 + _mn);     -> 74 insn, limpio
    _r = _mx - _mn; _r -= _dz * 2;  -> 76 insn, COPIA
    _r = _d; _d = _mx - _mn; ...    -> 76 insn, COPIA
    _r = (_mx - _mn) - (_dz << 1);  -> 76 insn, COPIA
    _r = (_mx - _mn) - _dz - _dz;   -> 76 insn, COPIA
    _r = -(_dz*2) + (_mx - _mn);    -> 76 insn, COPIA

**Regla medida: si el fuente escribe `_mx - _mn` como subexpresion, mwcc NO
fusiona la copia; si `_mx` se combina antes con `_dz`, si.** Pero el objetivo
emite `subf r0, r7, r10` = `_mx - _mn` **y ademas** fusiona, asi que la
reasociacion no es la respuesta: en el arbol real da `HandleTriggers` 97,177 %
con el tamaño exacto pero tira `HandlePedals` del 100 % al 98,36 % y `CookValues`
a 91,92 % (ensayos c6/c7, revertidos).

### La respuesta: `mx` es una variable de la FUNCION, no del macro

Con el maximo cacheado en una local de `HandleTriggers` — usada en la condicion
externa **y** como `_mx` dentro del macro, actualizandose a la vez que el campo —
el caso minimo da **74 instrucciones y la secuencia del objetivo entera, opcode a
opcode y papel a papel**:

    lbz  _mn        cmpw _v,_mn    bge    stb _v,(mn)   mr _mn,_v
    cmpw _v,_mx     ble            stb _v,(mx)          mr _mx,_v
    clrlslwi _dz2   subf t,_mn,_mx subf. _r,_dz2,t      bne
    li 1            add _mn+_dz    subf _v-...          mulli  divw.

Y en el arbol real, con `AUTOCALX` (macro gemelo que recibe la variable, sin
`_mx` propio) mas `mxl`/`mxr`:

    c9   mxl/mxr de funcion            94,898 -> 97,619 %   604 -> 588 B (exacto)
    d6   c9 + vl/vr (el valor crudo)   97,619 -> 98,299 %   <- QUEDA

**`triage.py` pasa de `faltan 0, sobran 4  mr-4` a `faltan 0, sobran 0`: las 147
instrucciones y los 12 saltos coinciden fila a fila y solo queda una permutacion
de registros.** `HandlePedals` sigue al 100 % (usa el `AUTOCAL` de siempre) y
`CookValues` no se mueve.

Lo que queda es un ciclo de tres: objetivo `_mn`=r7 `_dz`=r8 `_v`=r9 `mx`=r10;
nuestro `_dz`=r7 `mx`=r8 `_v`=r9 `_mn`=r10 (identico en las dos mitades).

## 2. `VDevice_GetFreeEffect` (116 B): 88,966 -> 93,103 %, y desaparecen los dos SUST

El unico fallo era una rotacion de cuatro accesos a memoria alrededor de la carga
volatil de `createcount`:

    objetivo: lwz vd->dev ; lwz createcount ; lwz dev->chan ; clrlslwi ; rlwimi
    nuestro:  lwz createcount ; lwz vd->dev ; clrlslwi ; lwz dev->chan ; rlwimi

Sacar `vd->dev` a una local **antes de los dos stores de `jumbleeffectid`** hace
que la carga se emita al principio del bloque y que el resto —`lwz createcount`,
`lwz dev->chan`, `clrlslwi`, `rlwimi`— quede **fila a fila con el objetivo**:

    n1  dev = vd->dev  tras los dos stores        ->  88,966 %  (mwcc la refunde)
    n2  dev = vd->dev  antes del 1er store        ->  93,103 %  <- QUEDA
    n3  dev = vd->dev  al principio de la funcion ->  86,897 %
    n4  dev = vd->dev  primera del if             ->  93,103 %  (= n2)
    n5  chan = vd->dev->chan antes del id         ->  88,966 %
    n6  dev antes + chan despues                  ->  93,103 %  (= n2)
    n8  n2 + orden de OR cc-primero               ->  90,000 %
    n9  dev fuera del if, dentro del bucle        ->  88,966 %
    n10 dev entre los dos stores                  ->  93,103 %  (= n2)

`triage.py`: de `faltan 5, sobran 5, de ellas 2 SUST` a `faltan 5, sobran 5, **0
SUST**`. Ya no hay diferencia de opcode: **la unica fila que sobra es la posicion
del `lwz r3,0xf0(r3)`**, que mwcc sube a la cabeza del bloque basico y el objetivo
deja detras de los dos stores volatiles. No hay forma de fuente que lo deje en
medio: puesto antes se sube, puesto despues se refunde en la expresion.

## 3. `Effect_Init` (276 B, 88,623 %): MURO DEMOSTRADO, no supuesto

Diagnostico del encargo confirmado: el objetivo iza `lis 0x4330` (la constante de
conversion int->double) al preencabezado del bucle y la guarda en un **sexto**
registro salvado (`stmw r26`), nosotros la rematerializamos dentro del bucle con
cinco (`stmw r27`). Mismo numero de instrucciones (72/72); es donde vive el `lis`.

Reproduje `Effect_Init` en un caso minimo que da **exactamente** nuestro objeto
(`stmw r27, 0x1c(r1)`, `lis` dentro) y barri:

* **28 pragmas manteniendo `optimization_level 2`** — ninguno iza la constante;
* **9 formas del bucle** (`for`, `while`, indexado, puntero `*p++`, `float fi`
  intermedio, `(double)i`, `(u32)i`, stores intercambiados, `phase` antes) —
  ninguna la iza;
* **dos conversiones int->float en el mismo bucle** (diagnostico) — **tampoco**
  la iza: no es cuestion de cuantos usos tiene.

Y la razon esta medida:

    nivel 2:  69 insn   lis DENTRO   stmw r27   0 registros FP salvados
    nivel 3:  86 insn   lis DENTRO   stmw r26   4 registros FP salvados
    nivel 4:  86 insn   lis DENTRO   stmw r26   4 registros FP salvados
    nivel 4 + peephole off / scheduling off:  IZADO, pero 86-92 insn y 4 FP

El nivel 2 **nunca** hace movimiento de invariantes de bucle; el nivel 3/4 lo
hace pero ademas iza las **cuatro constantes flotantes** a f28-f31 y las salva
(el objetivo las recarga cada vuelta: `lfd f2,@961`, `lfs f3,@1129`, `lfs
f0,@956`, `lfs f0,@1130`). Las dos conductas que necesita el objetivo —bucle de
puesta a cero de nivel 2 y izado de la constante entera— **son mutuamente
excluyentes en mwcc 2.7 con estas banderas**. Igual que
`VDevice_RecalcGammaTable`, es un muro del compilador, no de la fuente.

De paso: quitar el `#pragma optimization_level 2` deja `Effect_Init` en **41,96 %
y 344 B** (i1), y `#pragma push`/`#pragma pop` da **el mismo objeto** que el par
`optimization_level 2` / `optimization_level 4` actual (i4), asi que el pragma de
cierre no contamina al resto de la unidad. Duda cerrada.

## 4. Lo demas, con cifra

### `Effect_PerformEnvelope` (196 B, 93,265 %) — sigue abierta, 10 ensayos nuevos

Al objetivo le sobra un `mr r5, r9`: mantiene `out` y `mag` en registros distintos
(`out` en r5, el registro del parametro `level`) y nosotros los fusionamos.

    h1  mag = ...; out = ...  (dos veces la misma expresion)  ->  85,918 %  204 B
    h2  out = ...; mag = ...                                  ->  85,918 %  204 B
    h3  mag = level; if (mag<0) mag = -mag; out = mag;         ->  76,939 %
    h4  if/else en dos ramas para mag, luego out = mag         ->  93,265 %  (identico)
    h5  neg antes del out = mag = ...                          ->  93,265 %
    h6  mag = ...; neg = ...; out = mag;                       ->  93,265 %
    h7  out = ...; neg = ...; mag = out;                       ->  93,265 %
    h8  out = mag + 0                                          ->  93,265 %
    h9  reutilizar el parametro `level` como `out`             ->  93,265 %
    h10 h9 con level = mag antes de neg                        ->  90,306 %

**Veda**: barridas las diez formas de separar `out` de `mag`, incluida la mas
prometedora (h9, que el objetivo sugiere porque `out` vive en r5 = el registro de
`level`): mwcc fusiona la copia en todas.

### `CookValues` (260 B, 95,769 %) — la veda de la r20 confirmada por otra via

Probé la idea de la seccion 1 aqui: cachear `cal->max.steering` en una variable de
funcion y usar `AUTOCALX`.

    p1  s8 mxs   ->  87,692 %  264 B
    p2  s32 mxs  ->  95,769 %  (objeto identico)
    p3  u8 mxs   ->  87,692 %  264 B

No reproduce el `extsb r0,r5` del objetivo. **Veda ampliada**: a las tres formas
de rematerializacion de la r20 se suman las tres tipificaciones de la variable
cacheada.

### `SimThread_Init` (324 B, 94,691 %) — dos fallos, cinco ensayos, ninguno cierra

Quedan (a) `li r3,0`/`mtctr` antes o despues del `lfs f1` del bucle de `ia[]` y
(b) un intercambio r5/r6 entre el cero de `st->hist[i]` y el temporal de
direccion del ultimo bucle.

    m1  variable `z = 0` para el cero de hist        ->  94,691 %  (identico)
    m2  indices intercambiados (i para ia, n para el ultimo) ->  90,802 %
    m3  tercer indice propio para el ultimo bucle    ->  94,753 %  DESCARTADO
    m4  escala 4096.0f en una local declarada        ->  94,691 %  (identico)
    m5  a[n] * 4096.0f (operandos al reves)          ->  94,691 %  (identico)

**m3 es la trampa del porcentaje otra vez, pero mas fina que la z1 de la r20**: no
reordena los stores (esa era la razon de descartar z1), arregla `add r6,r31,r3` y
`stw r0,0x10(r6)`… y rompe `addi r4,ia@l` y `lwzx`. Cuatro filas mal antes y
cuatro despues: **+0,06 pp por permutar el mismo problema**. Descartado.

### `SimThread_Step` (924 B, 95,671 %) — el segundo racimo, cuatro ensayos

Dos racimos: (a) el vedado de la r20 (`li r8,0 ; mr r5,r8 ; mr r6,r8` contra tres
`li`), y (b) uno que la r20 no menciona: **el objetivo tiene `out` en r26
(salvado) toda la cola y nosotros en r5 (volatil) mas un `mr r27,r5` para
cruzarlo por la llamada**. `out` cae en r5 porque es el **tercer argumento** de
`SIControlSteering` — el mecanismo 5 del brief de la r21.

    q1  v = out; SIControlSteering(..., v)      ->  95,671 %  (identico)
    q2  v = out justo tras st->lastOutput = out ->  95,671 %  (identico)
    q3  pasar st->lastOutput en vez de out      ->  95,186 %  924 B  <- TAMAÑO EXACTO Y PEOR
    q4  toda la cola sobre `v` en vez de `out`  ->  95,671 %  (identico)

q3 es el **undecimo** caso medido de «el tamaño exacto miente»: clava los 924 B y
baja 0,49 pp. Sacar el valor de la ranura de argumento no lo libera: mwcc vuelve
a fusionar.

## 5. Resultado medido

`build_direct.py` antes de **cada** medida (`ninja` no construye LibSN).

    measure.py --cmp c22st_base.json c22st_desp.json
        +0 B, +0 funciones, 0 unidades cambian
    LibSN/steering   4680/8760 B  53,4247 %  26 funciones al 100 %   (sin cambio)

Ninguna funcion cierra, asi que **no hay bytes nuevos**: `matched_code` es
todo-o-nada. Lo que hay son dos correcciones estructurales reales, que dejo en el
arbol por el criterio del brief (ninguna unidad baja, ninguna vecina se mueve):

| funcion | antes | ahora | triage antes | triage ahora |
|---|---|---|---|---|
| `HandleTriggers` 588 B | 94,898 % / 604 B | **98,299 % / 588 B** | faltan 0, sobran 4, `mr-4` | **faltan 0, sobran 0** |
| `VDevice_GetFreeEffect` 116 B | 88,966 % | **93,103 %** | faltan 5, sobran 5, **2 SUST** | faltan 5, sobran 5, **0 SUST** |

Estado por funcion al terminar:

    992 B  99,839%  Effect_Update             muro (r6/r7, vedado r18)
    924 B  95,671%  SimThread_Step            veda nueva (seccion 4), racimo (b) identificado
    588 B  98,299%  HandleTriggers            94,898 -> 98,299  (+3,4), estructura exacta
    324 B  94,691%  SimThread_Init            5 ensayos, sin cierre
    276 B  88,623%  Effect_Init               MURO DEMOSTRADO (seccion 3)
    260 B  95,769%  CookValues                veda ampliada
    216 B  99,796%  VDevice_RecalcGammaTable  MURO DEMOSTRADO (r20)
    196 B  93,265%  Effect_PerformEnvelope    veda nueva, 10 ensayos
    188 B  99,362%  VDevice_DownloadEffect    agotado (r20)
    116 B  93,103%  VDevice_GetFreeEffect     88,966 -> 93,103  (+4,1), cero SUST

`audit.py LibSN/steering`: **rc=0, 26 funciones ok, cero FALLA, dos pasadas.**
Congelada con `frozen.py cong` (`e1f882f46b7b0919`); `chk` responde «identico al
congelado».

Ficheros tocados: **`src/LibSN/steering.c` y nada mas** (mas
`docs/congelado/LibSN__steering.json`, que lo escribe `frozen.py`). Sin commit.

## 6. Lo que NO he probado

* **`Effect_Update` (992 B, 99,839 %)**: ni un ensayo. Vedada en la r18, y con
  `faltan 0, sobran 0` es asignacion pura.
* **`VDevice_RecalcGammaTable` y `VDevice_DownloadEffect`**: no he gastado ni un
  ensayo, por orden del encargo. Los di por cerrados.
* **La permutacion de registros que le queda a `HandleTriggers`**: he probado
  orden de declaracion (4 formas), orden de asignacion, cachear tambien `dz` (lo
  empeora: vuelve la copia), variables compartidas entre las dos mitades (3
  formas), `_mn` de ambito de funcion, y **22 pragmas** sobre la version nueva.
  Todo se queda en 98,299 %. **No he probado restricciones de registro**
  (`register ... asm("rN")`): el ciclo r7/r8/r10 es un candidato natural, pero el
  brief las deja para el ultimo recurso y no he agotado el eje de fuente.
* **El permutador**: no lo he lanzado sobre ninguna funcion de la unidad.
* **`ppc2D2` y `madidct`**: no son de este encargo, no los he tocado ni medido.
* **`configure.py`, `config/GOWE69/*`, `splits.txt`, banderas de unidad,
  cabeceras compartidas**: intactos.
* **No he ejecutado nada ni verificado el DOL**: la unidad no esta enlazada
  (`linked False` en `build.ninja`), asi que el DOL no puede cambiar.

## 7. Aviso de entorno

A mitad de ronda el disco se lleno (**42 MB libres**) y un `cp` de 38 kB fallo con
`No space left on device` — la trampa que documenta HERRAMIENTAS §5. El
scratchpad compartido
(`...\3eb1ea2d-b037-4ced-b620-8e690908107f\scratchpad`) tiene **16,6 GB** en
volcados JSON de objdiff de rondas anteriores (`zai.json`, `d_zAI.json`,
`jf_rel_zFe2.json`… de 30-36 MB cada uno). Borre solo mis propios `.o`/`.s`
temporales y recupere 84 MB. **No he tocado los ficheros de otros agentes**, pero
conviene limpiarlos: con el disco asi, cualquier medida de cualquier agente puede
salir en falso sin dar error.
