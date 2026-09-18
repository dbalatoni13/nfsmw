# r50 — sonido: `zEcstasy` + `zEagl4Anim` + `zSpeech` + `zEAXSound2`

Fecha: 2026-09-09. Gate: `zEAXSound`. Entrada: **9.824 B en 8 funciones**.

## Resultado

**Ninguna funcion cierra: cero bytes.** Lo que sale de la ronda:

1. **`GenerateRoadNoise` baja de 23 a 16 filas sin un solo `asm` ni pin**
   (97,1129 % -> **97,70968 %**, 1240/1240 B; `fncmp` 33 -> **20 insn**), y **se
   retiene**. Son DOS palancas independientes, las dos medidas, y la segunda es
   nueva: **el sitio de la declaracion de `slipBoost` vale 7 filas**, y no por el
   reparto sino porque decide en que bloque basico ve cse2 el `0,1f`.
2. **`EvalState` queda con el mecanismo EXACTO de sus 18 filas** —el volcado
   `-dL` lo nombra— y con la explicacion de la r49 **REFUTADA**:
   `-fmove-all-movables` y `-fno-move-all-movables` dan el objeto **identico**.
   No es `loop`; es `gcse`/PRE, y esta fuera del alcance de la fuente.
3. **Aviso de metodo que corrige lecturas de la r48 y la r49**: en `lmap.py`, la
   linea de una carga de POOL o de una carga de direccion **no es la de su
   sentencia**. Medido en nuestro propio compilado: el `lwz r11,0x28(r31)` que es
   el argumento del `bLength` lleva la linea de la sentencia **cinco lineas
   antes**. El mapa sirve para saber QUE sentencias hay y en que orden; no para
   atribuir una carga de constante.

| unidad | entrada | salida |
|---|---|---|
| `zEcstasy` | 4 fn / 5.180 B | 4 fn / 5.180 B — **byte a byte identicas** |
| `zEagl4Anim` | 2 fn / 2.808 B | 2 fn / 2.808 B — **byte a byte identicas** |
| `zSpeech` | 1 fn / 596 B | 1 fn / 596 B — **byte a byte identica** |
| `zEAXSound2` | 1 fn / 1.240 B (33 insn) | 1 fn / 1.240 B (**20 insn**) |
| `zEAXSound` | 0 de 773 | **0 de 773 — GATE cumplido** |

Dos ficheros tocados, los dos de mi territorio y los dos CRLF puro:

- `src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp` — el unico cambio de
  codigo;
- `src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp` — **solo comentarios** (el
  bloque de veda de `EvalState`, reescrito con el mecanismo y la refutacion).

`RawStateChan.h`, `RoadblockFlow.cpp`, `STICH_PlayBack.cpp`, `EcstasyEx.cpp`,
`eProject`/`eLightE`/`eMathE` y **`ENVIRO_AEMS.h` no se han tocado**.

---

## 1. `GenerateRoadNoise` (1.240 B) — 23 -> 16 filas, dos palancas, cero `asm`

Base de HEAD: `target=97.1129% size=1240/1240`, **23 filas** de `fndiff` (33 en
la cuenta de `fncmp`). Forma retenida:

```diff
     ftemp = bLength(this->m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
+    float slipBoost = 0.1f;                       // <- SUBE aqui
     if (ftemp > 0.15f) {
         ftemp = 0.15f;
     }

     fRightVol = fLeftVol + fLeftVol * ftemp;
-    float slipBoost = 0.1f;
-    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip);
     fRightVol = fRightVol + fRightVol * slipBoost;   // <- DELANTE del bLength
+    ftemp = bLength(this->m_pWheelCtl->m_bvTotalLeftWheelSlip);
     ftemp = ftemp * 0.01f;
```

`fndiff` 97,70968 %, 1240/1240 B, **16 filas**. Nada mas se mueve en las 930
funciones de `zEAXSound2`.

### 1.1 Palanca A — la acumulacion delante del `bLength`, y AHORA con la razon

La r49 la justificaba diciendo que la segunda acumulacion «esta dentro del bloque
basico del `bLength`, luego la sentencia tiene que estar delante». Eso es la
consecuencia, no la causa. La causa esta un paso antes y es la **primera**
acumulacion:

```
objetivo:  102  fmadds f29,f31,f11,f31   (fRightVol = fLeftVol + fLeftVol*ftemp)
           113  fmadds f29,f29,f10,f29   (la segunda acumulacion)
HEAD:      114  fmadds f29,f31,f11,f31
           134  fmadds f29,f29,f7,f29    <- en el bloque de UNION, tras el sqrt
```

Con la acumulacion detras del `bLength`, su `fmadds` nace en el bloque de union y
**el `fmadds` anterior se queda sin dependiente dentro del bloque**: prioridad 1,
y se hunde de la ranura 102 a la 114. Con la acumulacion delante, los dos estan
en el mismo bloque, el primero recupera su dependiente y sube a la 102 —la ranura
del objetivo—. Es el mismo mecanismo que la r48 documento en `ProcessUpdate`
(«a la insn rezagada le falta un dependiente»), aqui producido por la POSICION de
la sentencia en vez de por el registro.

### 1.2 Palanca B (NUEVA) — donde se declara el `0,1f` decide 7 filas, y es cse2

`slipBoost` tiene que quedar, **en tiempo de cse2**, en un bloque ANTERIOR al
rombo del `sqrt` del segundo `bLength`. Si se declara DESPUES de ese `bLength`,
cse2 funde su `0,1f` con el `* 0.1f` de quince lineas mas abajo y **desaparecen
los 8 B** del segundo par `lis`/`lfs`:

| forma | B | filas |
|---|---:|---:|
| decl **antes** del segundo `bLength` | **1.240** | 16-23 |
| decl **despues** (lo que dice el orden de lineas del objetivo) | **1.232** | 29 |

Y no lo arregla ninguna bandera —el barrido completo sobre esa forma—:

```
(base)                          1232 B  29 filas
-fno-gcse                       1232 B  45
-fno-cse-follow-jumps           1232 B  29
-fno-cse-skip-blocks            1256 B  80
-fno-rerun-cse-after-loop       1176 B  168
-fno-expensive-optimizations    1232 B  63
```

O sea que **quien funde es cse2**, y el unico control que tenemos sobre el es el
bloque en el que cae la carga. Eso convierte «donde se declara la local» en una
palanca de fuente de pleno derecho, y una que no estaba en el catalogo.

### 1.3 El barrido completo de la posicion (20 medidas, todas 1240/1240 B)

`float slipBoost;` con las demas locales y `slipBoost = 0.1f;` detras de la linea
que se indica (numeracion del fichero de HEAD):

| `slipBoost = 0.1f;` tras | acumulacion ANTES | acumulacion DESPUES |
|---|---:|---:|
| L380 `speed = …GetVelocityMagnitudeMPH();` | **16** (98,145 %) | 19 |
| L381 `fLeftVol = …GetValue(speed)…` | 17 | 20 |
| L382 (blanco) | 17 | 20 |
| **L383 `ftemp = bLength(Right) * 0.01f;`** | **16** (97,710 %) | 17 |
| L384 `if (ftemp > 0.15f) {` | **16** | 18 |
| L385 `ftemp = 0.15f;` | **16** | 18 |
| L386 `}` | 55 | 23 |
| L387 (blanco) | 55 | 23 |
| L388 `fRightVol = fLeftVol + fLeftVol * ftemp;` | 55 | 23 |
| L389 | 55 | 23 |
| decl con inicializador arriba (V49 de la r49) | 18 | 21 |

El escalon esta exactamente en el `}` del primer clamp: en cuanto la asignacion
cae en el bloque de union, `slipBoost` se lleva **f11** —el registro que el
objetivo usa para la longitud del `bLength`— y la permuta f10/f11 recorre la
funcion entera (55 filas).

Elegida L383 (y no L380, que puntua 98,145 %) porque **L380 iza el `0,1f` a f30,
un preservado**, y L383 lo deja en un volatil (f8), que es la forma del objetivo
(f10). Las dos dan 16 filas.

### 1.4 Lo que queda: UNA cosa, y esta acotada

Las 16 filas son la carga del `0,1f` **un bloque demasiado pronto**:

```
objetivo:  99 lis 0.5@ha | 100 lis 0.1@ha | 101 lwz pWheelCtl | 102 fmadds#1
          103 lfs 0.5    | 104 lfs 0.1    | 105 lfs vec.y     | …  113 fmadds#2
nuestro:   91 lis 0.1@ha | 94 lfs 0.1 -> f8   (bloque del PRIMER bLength)
           99 lwz pWheelCtl | 100 lis c1@ha | 101 lfs c1 | 102 fmadds#1 | …
```

Mismas 16 instrucciones en el bloque salvo ese par, que el objetivo tiene dentro
y nosotros dos bloques antes; de ahi salen 11 filas de desplazamiento y 3 de
nombre de registro (f8 nuestro contra f10 del objetivo, y la constante de
`bMath` al reves). **Meterlo en ese bloque cuesta f11** (55 filas) porque el
allocno del `0,1f` se adelanta al de la longitud. Pinchandolo:

| forma | filas |
|---|---:|
| asignacion en el bloque de union + `register float slipBoost asm("fr10")` | 20 |
| lo mismo con `asm("fr8")` | 21 |
| la retenida, sin `asm` | **16** |

O sea que **el pin no compra nada aqui**: 20 > 16, y ademas seria deuda.

### 1.5 La veda de la r47 CADUCA, pero `slipBoost` sigue haciendo falta

Medido sobre la forma retenida:

```
sin slipBoost, literal 0.1f en la acumulacion ..... 1.240 B   55 filas
const float slipBoost = 0.1f ...................... 1.240 B   54 filas
float slipBoost = 0.1f (retenida) ................. 1.240 B   16 filas
```

**Ya no mide 1.232 B**: con este orden el literal suelto vuelve a dar el tamano
exacto, tal como predijo la r49. Pero pasa de 16 a 55 filas por la misma permuta
f10/f11. **La variable sigue siendo imprescindible, ahora por otra razon**, y
queda escrito en el fuente para que nadie la retire creyendo la veda vieja.

### 1.6 Otras medidas negativas (todas 1240/1240 B)

`ftemp = bLength(Left) * 0.01f` fundido en una sentencia con la acumulacion
delante: 16 (igual). `float slipBoost;` como PRIMERA local: 16 (igual).
Declaracion con inicializador en el sitio de la asignacion en vez de
decl-arriba + asignacion: 16 (igual, y es la forma retenida por ser mas limpia).

---

## 2. `EvalState` (456 B, 18 filas) — el mecanismo, con el volcado que lo nombra

**La r49 se equivoco de pasada.** Su veda decia que el `mr r4,r11` del objetivo
es «una COPIA del invariante que `loop` (con `-fmove-all-movables`) subio al
preencabezado». Refutado por medida:

```
(base)                     456/456 B  18 filas
-fmove-all-movables        456/456 B  18 filas   <- IDENTICO
-fno-move-all-movables     456/456 B  18 filas   <- IDENTICO
```

### 2.1 Lo que dice el `.loop` (`cc1plus -dL`)

Bucle de ida (insns 125..273):

```
Insn 166/168/169  regno 143/145/146  -> movidas a 730/731/732   (rama IMPAR: nf, 2nf, 2nf+10)
Insn 178/180/181  regno 150/152/153  -> "done matches 166/168/169"   (rama PAR: LAS MISMAS)
Insn 182          regno 154          -> movida a 733    (reg154 = reg85 + reg146)
```

y dentro del bucle quedan exactamente dos insns que asignan `keyData`:

```
insn 170 (rama impar): (set (reg 147) (plus (reg 85) (reg 146)))   <- la que RECALCULA
insn 183 (rama par):   (set (reg 147) (plus (reg 154) (const_int 2)))
```

`scan_loop` (`loop.c:905`) exige `set_in_loop[regno] == 1` o
`consec_sets_invariant_p`. `keyData` (reg 147) tiene **DOS** asignaciones en el
bucle, una por rama, y no son consecutivas: **la insn 170 no es movable**. La 182
si lo es porque `reg154` —el temporal que la rama PAR necesita antes del `+2`—
tiene una sola. Y `move_movables` **nunca deja una copia dentro del bucle**: el
unico camino que emite un `gen_move_insn` es `m->partial && m->match`
(`loop.c:1874`) y lo emite `before loop_start`.

### 2.2 Conclusion: el `mr` del objetivo lo pone gcse/PRE, y no es alcanzable

Para que el objetivo tenga `mr r4,r11` en la rama impar, `reg85 + (2nf+10)` tiene
que haberse izado al **bloque comun de dentro del bucle** antes de `loop` —eso es
PRE—, y para eso las dos ramas tienen que **compartir el pseudo de `2nf+10`**.
Nosotros tenemos reg146 y reg153 distintos hasta que `combine_movables` los casa,
que ya es demasiado tarde para gcse. Y cse2 tampoco puede: la etiqueta de
cabecera del bucle tiene dos predecesores y corta el bloque extendido.

Para compartir el pseudo hay que escribir una base comun en el accesor, y eso
cambia los TRES sitios donde `GetKeyData` no esta en un bucle:

| forma de `GetKeyData` | insn | filas |
|---|---:|---:|
| base (if/else con dos expresiones independientes) | 114 | 18 |
| temporal `unsigned short *d` solo en la rama impar | 114 | **18 (identico)** |
| temporal `unsigned char *d` solo en la impar | 114 | 18 |
| temporal en las DOS ramas | 114 | 18 |
| `+ 0` en la rama impar | 114 | 18 |
| `d` **comun** antes del `if` | **103** | 105 |
| ternario con `d` comun | **103** | 96 |
| `&mDecodeData[mNumFields + 1]` | **111** | 101 |

El objetivo tiene 114 instrucciones. **Las formas que comparten el pseudo pierden
11**: el front-end pliega los temporales que no cambian la estructura, y las que
si la cambian dejan de casar los tres sitios sin bucle. Frente cerrado.

### 2.3 Barrido de banderas (todas 456/456 B, 114 insn)

**IDENTICAS a la base (18 filas)**: `-fmove-all-movables`,
`-fno-move-all-movables`, `-fgcse`, `-frerun-loop-opt`, `-fno-cse-skip-blocks`,
`-fno-thread-jumps`, `-fno-strength-reduce`, `-fno-force-addr`, `-fno-regmove`,
`-freduce-all-givs`, `-fno-caller-saves`, `-fno-peephole`, `-fno-function-cse`,
`-fno-defer-pop`, `-fno-strict-aliasing`, `-fno-branch-count-reg`.
**Peores**: `-fno-force-mem` 19; `-funroll-loops` 59 (115 insn);
`-fno-schedule-insns2` 65; `-fno-gcse` 81; `-fno-expensive-optimizations` 90;
`-fno-schedule-insns` 90; `-fno-rerun-cse-after-loop` 97;
`-fno-cse-follow-jumps` 99; `-fno-inline` 113; `-fno-omit-frame-pointer` 113;
`-fvolatile` 127.

### 2.4 Y el mapa de lineas del original no deja hueco para nada

Sentencia a sentencia, el original contra el nuestro: 107 cabecera, 108 `c`,
126 el `if`, 127 `i = mKeyIdx`, 128 el `while`, 130 `k = GetKeyData(i)`,
133 el `if` interior, 136/138/139 `Decode`/`mKeyIdx`/`return`, 141 `i++`,
144/145/146 la salida del `while`, 149/150 el `else` y el `for`,
152 `k = GetKeyData(i)`, 155 el `if`, 158 `mKeyIdx = i`, 163 el `}` del `for`,
167/168 la salida, 171 `return`. **Una a una las nuestras**, con mas lineas en
blanco en el original y **ninguna sentencia extra**. Tambien queda descartado que
el `for` inverso tenga el decremento en una sentencia aparte: GCC 2.95 atribuye
el `subic.`+`bge` del fondo del bucle **al `}` de cierre**, no a la cabecera del
`for` (comprobado en nuestro propio compilado: cae en nuestra 184, el `}`).

---

## 3. Lo que NO he tocado, y por que

- **`UpdatePlatInfo`** (2.044 B): el encargo lo prohibe y la r49 lo demostro sin
  premio (`matched_code` es todo-o-nada y sus 6-7 filas de literales son
  irreducibles). Cero tiempo.
- **`Setup__RoadblockFlow`** (596 B, 4 filas), **`GenerateHorizonFog`** (796 B, 2
  filas), **`eProject`** (268 B, 13 filas), **`Initialize`** (2.352 B): cerradas
  con prueba en la r48/r49. No las he reabierto.
- **`epCalculate`** (2.072 B, 164 filas): la unica hipotesis viva sigue siendo
  `secondary_memlocs_elim[]` de `reload` y no cabe en una ronda compartida.

---

## 4. Propuestas y avisos fuera de mi territorio

1. **AVISO DE HERRAMIENTA, y corrige conclusiones ya escritas: en `lmap.py`, la
   linea de una carga de POOL o de una carga de DIRECCION no es la de su
   sentencia.** Medido en nuestro propio compilado de `GenerateRoadNoise`: el
   `lwz r11,0x28(r31)` que es el argumento del `bLength` de la linea 393 lleva
   la linea **388**, cinco sentencias antes; y las constantes del `sqrt` de
   `bMath.hpp` unas veces llevan la linea de la cabecera y otras la del
   llamante. La razon es que `restore_line_notes` de haifa devuelve a cada insn
   la nota que tenia **cuando se creo**, y una carga de constante que `cse`,
   `gcse` o `loop` movio se creo en otra region. **Fiable**: que sentencias
   existen, en que orden y cuantas hay. **No fiable**: atribuir un `lis`/`lfs`/
   `lwz` a una sentencia concreta. Esto invalida el paso «562 es el `bLength`»
   que yo mismo intente y explica por que el orden de sentencias que sugiere el
   mapa (declaracion DETRAS del `bLength`) da 1.232 B en vez de 1.240.
2. **Palanca nueva para el catalogo: «donde se declara la local decide el
   bloque, y el bloque decide si cse2 funde el literal».** Firma: el objetivo
   CARGA dos veces la misma entrada del pool y nosotros una sola, con 8 B de
   menos y sin ninguna otra diferencia. Arreglo de cero `asm`: subir la
   declaracion por encima del rombo (`if`/`sqrt`/clamp) que separa las dos
   cargas. Se comprueba en una compilacion. En `GenerateRoadNoise` vale los 8 B
   **y** 7 filas.
3. **Regla nueva sobre `loop.c`, barrible en todo el arbol**: si el objetivo
   tiene un `mr rA,rB` DENTRO de un bucle donde nosotros recalculamos la
   expresion, **no lo puso `loop`** —`move_movables` solo emite copias *antes*
   del bucle, y solo por el camino `m->partial && m->match`—. Lo puso gcse/PRE,
   y eso exige que las dos ramas compartan el pseudo del operando. Se
   diagnostica en un `-dL`: si en la lista de movables aparece
   `done matches N` y la insn del `plus` final NO aparece, es que su destino
   tiene dos asignaciones en el bucle. Evita barrer formas de fuente durante una
   ronda entera, como paso en `EvalState` (r48, r49 y r50).
4. **`rtldump.py` sigue sin `--extra`.** Lo pidio la r48, lo repitio la r49 y en
   la r50 he tenido que escribir por tercera vez un arnes propio para pasar
   `-dL`/`-dl` a `cc1plus`. Es una linea.
5. **`lmap.py` deberia tener un modo `--ours`.** El `.s` que emite el compilador
   trae las notas en `.section .line` (`.4byte <linea> # <fichero>`), asi que
   sacar NUESTRO mapa de lineas son 20 lineas de parseo; con el, la comparacion
   nota a nota contra el objetivo es inmediata y es lo que ha resuelto las dos
   funciones de esta ronda. Mi version esta en `scratchpad/r50_snd/rnmap.py`.
6. **Disco: C: en 5,2 GB libres** con seis agentes. Mi scratchpad queda en
   **140 kB** (he borrado 61 MB de volcados y objetos al terminar).

---

## 5. Verificacion

```
python scripts/build_direct.py Speed/Indep/SourceLists/zEcstasy \
    Speed/Indep/SourceLists/zEagl4Anim Speed/Indep/SourceLists/zSpeech \
    Speed/Indep/SourceLists/zEAXSound2 Speed/Indep/SourceLists/zEAXSound
  -> 5 ok, 0 fallidas

diff fncmp_before.txt fncmp_after.txt   (las CINCO unidades)
  49c49
  <     1240  GenerateRoadNoise__16CARSFX_RoadNoise   33 insn, 72 reub
  >     1240  GenerateRoadNoise__16CARSFX_RoadNoise   20 insn, 73 reub
  (la UNICA linea que cambia en las cinco unidades)

fncmp zEcstasy    4 de 539  -- 5.180 B   (igual)
fncmp zEagl4Anim  2 de 318  -- 2.808 B   (igual)
fncmp zSpeech     1 de 703  --   596 B   (igual)
fncmp zEAXSound2  1 de 930  -- 1.240 B   (igual; 33 -> 20 insn)
fncmp zEAXSound   0 de 773  --     0 B   <- GATE cumplido

fndiff GenerateRoadNoise__16CARSFX_RoadNoise  97,70968 %  1240/1240  (entrada 97,1129 %)
fndiff EvalState__…FnRawStateChan             98,070175 % 456/456    (sin cambio)
fndiff Setup__Q26Speech13RoadblockFlow        98,255035 % 596/596    (sin cambio)

audit  zEcstasy · zEagl4Anim · zSpeech · zEAXSound2 (929 ok / 0 FALLA) · zEAXSound
       -> exit 0 en las cinco
python scripts/lcfix.py --check  -> todas las entradas @lc estan al dia
git diff --check                 -> limpio
find src -name "__r50*" -o -name "__r49*"  -> nada
```

SHA-256 (16 primeros) al terminar:

```
CARSFX_Roadnoise.cpp   0727e10d4181a794  (modificado, CRLF puro, 501 lineas)
RawStateChan.cpp       6dc401d0c03cbcd4  (solo comentarios, CRLF puro, 274 lineas)
RawStateChan.h         f18155348cecf4e0  = base, sin tocar
```

Ninguna funcion empeora en ninguna de las cinco unidades. Sin commits, sin
`ninja`, sin `configure.py`, sin `config/GOWE69/*`, sin `splits.txt`, sin `asm`
nuevo y sin tocar `ENVIRO_AEMS.h`.

## 6. Bancos reproducibles (`scratchpad/r50_snd/`, 140 kB)

| fichero | que |
|---|---|
| `rsc.py` | TU reducida de `RawStateChan.cpp` + comparacion canonica contra el ORIGINAL (5 s por variante), heredada de la r49 |
| `ev3.py`, `ev5.py` | los dos barridos de banderas de `EvalState` (36 medidas) |
| `ev4.py` | las 8 formas de `GetKeyData` en la cabecera |
| `dump.py` | volcados `-dL`/`-dt`/`-dG` de la TU reducida |
| `mini.py`, `rn.py` | compilacion de `CARSFX_Roadnoise.cpp` SUELTO con los cflags de `zEAXSound2` (11 s) y comparacion con `objdiff-cli` |
| `rn1.py` … `rn11.py`, `rn_final.py`, `rn_veda.py` | las ~70 variantes medidas de `GenerateRoadNoise` |
| `rnmap.py`, `mkv.py`, `mkv2.py` | **NUESTRO** mapa de lineas (parsea `.section .line` del `.s`), que es lo que ha resuelto la ronda |
| `rndump.py`, `rnlreg.py` | volcados RTL de una variante de `CARSFX_Roadnoise.cpp` |
| `lmap_rn.txt` | el mapa de lineas del ORIGINAL de `GenerateRoadNoise` |
| `logs/fncmp_before.txt`, `logs/fncmp_after.txt` | la verificacion de las cinco unidades |
