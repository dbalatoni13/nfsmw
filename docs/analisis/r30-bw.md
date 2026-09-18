# Ronda 30 — zBWare (`_bOutput`) + zEcstasy (cinco near-miss)

## 0. Verificación del encargo

`build_direct.py zBWare zEcstasy` + `triage.py --muro` reproducen **las seis
cifras del encargo al dígito**:

    5180 B  99.98301%  _bOutput                          (zBWare)
    2072 B  93.305016% epCalculateLocalDirectionalPOS16  (zEcstasy)
    2044 B  98.65949%  UpdatePlatInfo                    (zEcstasy)
     796 B  98.99497%  GenerateHorizonFogDisplayList     (zEcstasy)
     696 B  98.074715% Render__13EmitterSystem           (zEcstasy)
     268 B  85.82089%  eProject                          (zEcstasy)

`measure.py` al abrir y al cerrar: **170.600/181.656 B, 93,9138 %, 772 funciones
al 100 %**. `--cmp` base/final: **+0 B, +0 funciones, 0 unidades cambian**.
`audit.py`, **dos pasadas** en cada unidad y al abrir y al cerrar: **zBWare
238 ok / 0 FALLA · zEcstasy 534 ok / 0 FALLA**.

**Ninguna función cerrada. Cero regresiones. El árbol queda sin tocar** (`git
status` no lista ninguno de mis cinco ficheros como modificado salvo los
comentarios de esta ronda).

> **Aviso**: `frozen.py chk` dice **HA CAMBIADO** en zBWare y zEcstasy. **No es
> mío**: los `.json` de `docs/congelado` son del 5-sep 20:56/21:33, `measure`
> da +0 B contra mi propia base de esta sesión, `audit` da los mismos 238/534 y
> las seis cifras de `triage` son las del encargo. Es el ruido de convivencia
> que documenta HERRAMIENTAS §0.3 (otros agentes tocando cabeceras y el pool de
> `$LC`). **No he re-congelado.**

---

## 1. LO QUE TRAIGO: la palanca de `n_refs` funciona, y ahora está calibrada

El encargo señalaba como único eje sin probar el del brief §1 — *«un `asm` no
volátil cuenta como REFERENCIA sin emitir un byte»*. **Funciona, y se puede
predecir al dígito**, pero hace falta la corrección que sigue, que es lo que
hizo fallar los primeros siete ensayos:

### 1.1. La corrección: el asm sólo sirve si AÑADE UNA INSN

`__asm__("" : "+f"(x))` sobre un **temporal de un solo uso** (`diff_g`,
`halfVP3` recién definida, `multiple`…) **NO añade una insn**: GCC lo absorbe en
la copia que ya había. Medido en `UpdatePlatInfo` con el volcado delante:

    base  416 insns en el .lreg      con __asm__("" : "+f"(diff_g))  416 insns
    y la tabla de allocnos SALE IDENTICA (live_length de los 28 pseudos, igual)

Eso explica los «IDÉNTICO» que la r29 anotó como veda del eje `asm` en
`GenerateHorizonFog` (`"+r"(multiple)`) y en `CookieTrailCurvature`: **el asm no
estaba ahí**. La veda no era del eje, era de la forma.

**Sobre una variable de vida larga sí nace la insn**, y entonces:

- **`live_length` de TODOS los pseudos vivos en ese punto sube exactamente 1**
  (verificado pseudo a pseudo en 28 allocnos, cinco variantes: g1…g5),
- **`n_refs` del operando sube 1**,
- **el objeto no crece: 2.044 B contra 2.044 B en las diez variantes.**

### 1.2. Vale para `global_alloc` Y para `local_alloc`

| pase | fórmula | dónde lo he medido |
|---|---|---|
| `global_alloc` (`allocno_compare`) | `floor_log2(n_refs)*n_refs/live_length*10000*size` | `UpdatePlatInfo`: 75 → **66** diffs |
| `local_alloc` (`QTY_CMP_PRI`) | `(n_calls_crossed+1)*n_refs/length*10000` | `eProject`: 25 → **15** diffs |

**Es la primera vez que se mueve `local_alloc` desde el fuente en este
proyecto.** El resto del catálogo (barrera total, barrera selectiva, `"m"`) sólo
tocaba `sched` y `cse`.

---

## 2. `UpdatePlatInfo` (2.044 B) — la tabla de allocnos ya es EXACTA

### 2.1. El modelo, cerrado al dígito

`alloc.py` sobre el `.greg` (banco de 3 s: `eLightE.cpp` compila suelto y
reproduce el objeto de la unidad **exacto**, 98,65949 % / 75 diffs / 437 de 512
filas):

    pseudo  refs  live  pri   nuestro     lo que quiere el objetivo
    189     5     307   325   r62 (f30)   f30
    93      7     439   318   r61 (f29)   f29
    96      7     440   318   r60 (f28)   f28
    195     5     315   317   r59 (f27)   **f26**   envmap_min_b
    90      7     442   316   r58 (f26)   **f27**   diffuse_min_r
    200     5     318   314   r57 (f25)   f25
    206     5     326   306   r39 (f7)    f8

La ventana que la r28 calculó es correcta, pero **la r29 cerró la ruta ancha por
la razón equivocada**: no hay que alargar el rango de `envmap_min_b` ni acortar
el de `diffuse_min_r` por separado. Basta **+1 uniforme a todos**:

    +1  ->  90: 443 -> 316   195: 316 -> 316   EMPATE, gana 90 (allocno menor)

y con eso 93 (440→318) y 96 (441→317) conservan su orden. **Es el único punto
del espacio que da el reparto del objetivo sin tocar el orden de declaración.**

### 2.2. Pero +1 rompe `specular_min_r/g/b`, y +3 lo arregla

`142/145/148` (`specular_min_r/g/b`, 3 refs, live 441/440/439) empatan los tres
en `pri = 68` y se ordenan por número de allocno. Con **+1** pasan a 442/441/440
→ **67, 68, 68** y el orden se rompe (r52/r51/r50 van a 145, 148, 142). Con
**+2** también. Con **+3** (444/443/442) vuelven a empatar los tres en 67.

    g1 (+1)  98.68884 %  75 diffs   142/145/148 -> r52 r51 r50  MAL
    g2 (+2)  98.68884 %  75 diffs   MAL
    g3 (+3)  98.77691 %  **66 diffs**  TODA la tabla FLOAT igual a la base
    g4 (+4)  98.77691 %  66 diffs
    g5 (+5)  98.77691 %  66 diffs

**Con g3 la asignación de `global_alloc` es la del objetivo en los 26 pseudos
FLOAT**: `99→f6, 114→f5, 106→f4, 110→f3, 115→f2, 165→f1, 192→f31, 203→f8,
189→f30, 93→f29, 96→f28, **90→f27**, **195→f26**, 200→f25, 206→f7, 117→f24,
111→f23, 107→f22, 103→f21, 142→f20, 145→f19, 148→f18, 118→f17, 153→f16,
157→f15, 161→f14`.

### 2.3. Lo que queda son 66 diffs de `local_alloc`, no de `global_alloc`

El reparto de bandas volátiles está **corrido un registro**:

    nosotros reparte  f6 f5 f4 f3 f2 f1 | f31 f8 f30 f29 f28 f27 f26 f25 f7 f24 ...
    el objetivo       f7 f6 f4 f3 f5 f2 | f1  ...            f8  ...

O sea: **en el objetivo f7 está libre cuando se asigna el primer allocno FP
(`diffuse_min_a`, pri 2000) y en el nuestro no**; a nosotros nos lo ocupa una
cantidad de `local_alloc` que entra en conflicto con él. **Eso ya no lo mueve el
número de insns** (g3, g4 y g5 dan los mismos 66) y es el frente que queda.

### 2.4. Ensayos numerados (banco de 3 s / 8 s con volcado; base 98,65949 % · 75)

| # | forma | % | diffs | tabla |
|---|---|---|---|---|
| a1 | `__asm__("" : "+f"(diff_g))` antes de `diffuse_min_r=` (bloque 0x2388DD82) | 98,25245 | 83 | **sin cambio** |
| a2 | idem en el bloque 0x33310077 | 98,53229 | 79 | sin cambio |
| a3 | idem con `diff_b` | 98,258316 | 82 | sin cambio |
| a4 | en los dos bloques | 98,125244 | 87 | sin cambio |
| a5 | dos asm en el bloque A | 97,96086 | 85 | sin cambio |
| a6/a7 | `diff_r` / `diffuse_max_g` | 98,31 | 80 | sin cambio |
| b1/b2/b3 | orden de `diffuse_min_r/g/b = diffuse_max_*` (grb, gbr, brg) | 98,65949 | **75 idéntico** | sin cambio |
| b7 | idem en el bloque B | 98,65949 | 75 idéntico | sin cambio |
| b4/b5/b8 | `diffuse_rng_r` (o `_g`) delante de las tres `diffuse_min_*` | 98,13-98,18 | 94-99 | **2.040 B**: pierde una insn |
| b6/b9 | las tres `rng` delante de las tres `min` | 97,99-98,00 | 80-82 | **2.032 B** |
| c1 | `diffuse_min_a` detrás de `envmap_min_b` | no compila | | |
| c2/c8 | `specular_magic` (± `specular_power`) detrás de `envmap_min_b` | 97,86105 | 97 | sin cambio |
| c3/c9 | `envmap_magic` / `envmap_max_scale` movidas | 98,65949 | 75 idéntico | sin cambio |
| **c4** | **orden `envmap_min` r,b,g** | 98,38552 | **67** | **90→f27, 195→f26** ✔ pero mete estructura |
| c5 | orden `envmap_min` b,r,g | 98,30528 | 73 | 90→f27 ✔ |
| c6/c7 | la conversión `envmap_min_ib` movida entre las `envmap_max_i*` | 97,80-97,87 | 155/160 | **2.052 B** |
| d1/d2/d4/d5 | `__asm__("" : "+f"(diffuse_min_a / diffuse_rng_a))` detrás de `envmap_min_b` o de `envmap_max_b` | 95,06-95,49 | 102-110 | 90→f27 ✔ |
| d3/d6/d7 | idem con `specular_power` / `"+r"(plat_info)` / `"+f"(envmap_min_b)` | 94,67-94,95 | 109-172 | 90→f27 ✔ |
| d8 | `"+f"(specular_min_scale)` | 98,65949 | 75 idéntico | **absorbido** |
| **e2** | **`__asm__("" : "+f"(diffuse_min_a))` al principio de `case 0x33A26CB6`** | **98,68884** | 75 | 90→f27 ✔ (rompe 142/145/148) |
| e1/e4/e8 | idem con `"+r"(plat_info)` en tres sitios | 94,68 | 117-119 | 90→f27 ✔ |
| e3/e7 | `"+r"(material_data)` / `"+r"(light_material)` | 98,65949 | 75 idéntico | **absorbido** |
| f1…f7 | `diffuse_min_a` / `diffuse_max_scale` / `specular_min_scale` / `specular_power` / `specular_magic` **declaradas por encima de `diffuse_min_r`** (7 formas) | 98,65949 | **75 idéntico en las siete** | **`live(90)` sigue en 442** |
| **g3** | **tres `__asm__("" : "+f"(diffuse_min_a))` al principio de `case 0x33A26CB6`** | **98,77691** | **66** | **TABLA EXACTA** |

**Veda con su sentencia — el orden de declaración del bloque de cabecera no
mueve un `live_length`.** Barridas las **siete** formas f1-f7 (subir
`diffuse_min_a`, `diffuse_max_scale`, `specular_min_scale`, `specular_power`,
`specular_power`+`specular_magic`, dos de ellas a la vez, y un control):
**las siete dan objeto y tabla de allocnos idénticos**. Razón: `REG_LIVE_LENGTH`
se recalcula (`recompute_reg_usage`) **DESPUÉS de `sched1`**, y `sched1`
reordena a su gusto todas esas cargas independientes dentro del bloque de
entrada. **Mover sentencias independientes de un mismo bloque básico no cambia
ningún `live_length`: sólo lo cambia añadir o quitar insns.** Es regla general y
vale para cualquier empate de `allocno_compare` del árbol.

**Deuda declarada**: g3 y e2 usan `asm`. **El DWARF los contradice**: el volcado
de `UpdatePlatInfo` no tiene ninguna nota de `asm` y declara los locales en el
mismo orden que nosotros. **Revertido; queda sólo como diagnóstico.**

---

## 3. `eProject` (268 B) — de 25 diffs a **15**, y con `local_alloc` movido

El comentario del fuente (r24) decía exactamente lo que faltaba: *«halfVP2 tiene
que asignarse ANTES que clipX (objetivo f0/f13, nosotros f13/f0): prioridad
3/len(halfVP2) > 2/len(clipX); hoy 5000 vs 6666»*. **Es el mismo empate, en
`local_alloc`, y la palanca de `n_refs` lo rompe.**

Datos del `.lreg` (`eMathE.cpp` compila suelto y reproduce el objeto exacto):

    Register 137  3 refs / 6 insns   -> halfVP2   pri 5000
    Register 107  2 refs / 3 insns   -> clipX     pri 6666
    Register 143  3 refs / 5 insns   -> halfVP3   pri 6000
    Register 136  4 refs / 29 insns  -> oneOverW

Con **una** `__asm__("" : "+f"(halfVP2))` metida entre la definición de
`halfVP3` y el `*sx`: `halfVP2` pasa a 4 refs / 7 insns = **5714** y `clipX` a
2 refs / 4 insns = **5000** → **halfVP2 gana**.

| # | forma | % | diffs |
|---|---|---|---|
| base | | 85,82089 | 25 |
| p1/p5/p7 | `halfVP3` antes de `halfVP2` · `oneOverW` detrás de los half · `0.5f * vp[2]` | 85,82089 | **25 idéntico** |
| p2/p3 | `halfVP3` declarada detrás del `*sx` | 81,208954 | 27 |
| p4 | `clipX` declarada la última | 85,07462 | 32 |
| p6/p9 | `oneOverW * halfVP2 * clipX` · `halfVP2 * (clipX * oneOverW)` | 86,69-86,99 | 33/36 |
| p8 | `clipX * halfVP2 * oneOverW` | 85,970146 | 24 |
| **q1** | **`__asm__("" : "+f"(halfVP2))` detrás de `halfVP3`** | **93,67164** | **16** |
| q5 | idem delante del `*sx` (mismo sitio efectivo) | 93,67164 | 16 |
| q2/q8 | dos / tres asm sobre `halfVP2` | 84,36-87,70 | 27/36 |
| q3/q4 | asm sobre `halfVP3` / sobre `clipX` | 85,82089 | 25 idéntico (**absorbido**) |
| q6/q7 | asm sobre los dos half a la vez / sobre `oneOverW` | 85,06-85,15 | 32/33 |
| r1/r2/r4/r7/s6/s8 | q1 + asm sobre `halfVP3` en cuatro sitios y los dos órdenes | 93,67164 | 16 (**sin efecto**) |
| r3/r6 | q1 + dos / tres asm sobre `halfVP3` | 90,78 / 83,88 | 17 / 32 |
| s1/s2 | `halfVP3` detrás del `*sx` sobre q1 | 84,64179 | 22 |
| s3 | q1 + `float negClipY = -clipY;` | 93,67164 | 16 |
| **s4 / t3** | **q1 + `*sy = vp[1] + halfVP3 + -clipY * halfVP3 * oneOverW;`** | **93,82089** | **15** |
| s5 | `halfVP3 * oneOverW * -clipY` | 88,01492 | 31 |
| t1/t2/t8 | q1 + `clipY`/`clipZ` reordenadas (3 formas) | 92,76-93,67 | 16-25 |
| t4 | q1 + s4 + `clipZ, clipY, clipX` | 89,35821 | 24 |
| t5/t7 | q1 + asm sobre `clipY` | 86,93-87,07 | 24/25 (**272 B**) |
| t6 | q1 + s4 + dos asm sobre `halfVP3` | 81,13433 | 35 |

**Lo que queda (15 diffs) es el MISMO empate un piso más abajo**: `halfVP3`
(3/5 = 6000) contra el temporal de `clipY`; el objetivo pone `halfVP3` en f13 y
`clipY` en f12, nosotros al revés. **Y ahí el asm no entra**: las seis formas
r1/r2/r4/r7/s6/s8 dan objeto **idéntico** — el asm sobre `halfVP3` se absorbe
porque su definición es la sentencia inmediatamente anterior.

**Deuda declarada**: q1 es un `asm`; el DWARF de `eProject` no lo tiene.
**Revertido.**

---

## 4. `GenerateHorizonFogDisplayList` (796 B) — MURO, y ahora con los números

La r29 dejó demostrado que el fuente no lo toca (16 formas). Esta ronda añade
**la prueba de por qué**, con el volcado de `sched2` (`-dR -fsched-verbose-5`)
delante. Bloque básico 2, **después de reload**:

    insn  code  dep  prio  cost  unidad
    212   211    0    14    1    iu2     srwi r0,r31,31
    703   333    0    11    1    iu2     *floatsidf2_loadaddr  (CERO bytes)
    214    52    1    13    1    iu2     add  r0,r31,r0
    699   523    0     2    1    fpu     fmr  f30,f22
    216   230    1    12    1    iu2     srawi r0,r0,1     <- i/2
    231   182    0     2    1    [iu iu2] andi. r11,r31,1  <- i&1
    702   134    1    11    1    iu2     xoris
    ...
    232   763   13     1    1    bpu     el salto

    ;;	Ready list (t =  3):    231  216
    ;;		--> scheduling insn <<<216>>> on unit iu2
    ;;		--> scheduling insn <<<231>>> on unit [iu iu2]

**Los dos se emiten en el MISMO ciclo (t=3) y el orden lo decide
`rank_for_schedule`, cuyo primer criterio es `INSN_PRIORITY`: 12 contra 2.** No
es un empate, y por eso el LUID (que es lo único que mueve el fuente) **no
llega a mirarse**. `prio(216)=12` viene de la cadena
`216→702→704→705→706→707→226→227` (la conversión `int→float` de `i/2`);
`prio(231)=2` porque el `andi.` sólo alimenta al `beq`.

Para invertirlo habría que **ocupar `iu2` en t=3 con una tercera insn de
prioridad > 12**, y toda insn con dependientes en esa cadena emite bytes.
**`_bOutput` y `GenerateHorizonFog` son ahora los dos muros con demostración
numérica del árbol.**

---

## 5. `epCalculateLocalDirectionalPOS16` (2.072 B) — la ranura, medida offset a offset

Confirmado el hallazgo de la r28 y **acotado**: las referencias a `(r1)` de los
dos lados coinciden **una a una de 0x8 a 0x7c** (misma cuenta en los 18 offsets)
y a partir de ahí el patrón es idéntico pero **corrido 8 B**:

    objetivo   0x8..0x7c locales | 0x80-0x87 LIBRE | 0x88+0x8c conversion (3+2 refs)
               | 0x90 libre | 0x94 (2) | 0x98 stmw r14 | marco 0x170
    nuestro    0x8..0x7c locales | 0x80-0x8f LIBRE | 0x90+0x94 conversion (3+2 refs)
               | 0x98 libre | 0x9c (2) | 0xa0 stmw r14 | marco 0x178

O sea: **la ranura que sobra son 8 B contiguos jamás referenciados entre el
último local (0x80) y el par de la conversión**, y **no está arriba del par**
(ahí los dos lados son iguales). Es una ranura que `alter_reg` reserva para un
pseudo derramado cuyas referencias acaban desapareciendo, no una `sn_ps` del
fuente — lo que confirma por otra vía el negativo de la r29 (los cuatro ensayos
sobre los `(sn_ps)` de `hack_scale`/`arn_HackAlpha`).

**No he hecho ensayos de fuente aquí**: sin identificar el pseudo, cualquier
barrido es a ciegas y las 106 diffs restantes (`dcrg` f3→f5, `dcba` f2→f4,
`scba` f4→f13) siguen colgando del pseudo 797.

**Lo que sí dejo montado**: `EcstasyEx.cpp` **ya compila suelto** (ver §7) y
reproduce las dos funciones **exactas** (93,305016 % / 155 diffs y 98,99497 % /
2 diffs). Era lo que faltaba para poder trabajarlas.

---

## 6. `Render__13EmitterSystem` (696 B) — el diagnóstico de la r29 hay que corregirlo OTRA VEZ

La r29 corrigió a la r28 («no falta un valor, sobra un hueco») y dijo que en
`Render` el hueco de r19 **empuja el valor 18 a r0, donde `caller-save` mete el
`stw`/`lwz`**. Con el objeto delante, **no es `caller-save`: es un DERRAME de
`reload`**, y el `stw`/`lwz` son consecutivos:

    objetivo   addi r0, r23, 0x37c   stw r0, 0xc8(r1)   lwz r0, 0xc8(r1)   cmpw r25, r0
    nuestro    addi r14, r23, 0x37c  --                 --                 cmpw r25, r14

Censo de r14-r31 contado sobre **todo** el listado de las dos partes:

    objetivo  r14 r15 r16 r17 r18 __ r20 r21 ... r31   17 registros + 1 DERRAMADO
    nuestro   r14 r15 r16 r17 r18 r19 r20 ... r31      18 registros, ninguno derramado

**El objetivo nunca escribe r19** (cero apariciones), o sea el hueco es real, y
el pseudo del centinela (`&elist`, `r23+0x37c`) **vive en memoria**, en
`0xc8(r1)`: ésos son los 8 B de más del marco (0x128 contra nuestro 0x120) y las
dos instrucciones que `triage` lee como «faltan 2». El resto de la función es
nuestro reparto **corrido uno** (su r14 = nuestro r15, sus r15/r16/r17 = nuestros
r16/r17/r18, su r18 = nuestro r19).

O sea: **el objetivo tiene código PEOR que el nuestro**, y para reproducirlo hace
falta que `reload` se quede sin registro de derrame y coja r19. **No hay
sentencia que añadir** (eso de la r29 sigue en pie) y tampoco «falta un valor de
larga vida»: **hay que hacer que un pseudo NO quepa**. Ensayos de fuente: **cero**
— he gastado el presupuesto en montar el banco (que no existía) y en cerrar el
diagnóstico.

---

## 7. Herramienta nueva: los tres `.cpp` de zEcstasy ya compilan sueltos

Era el bloqueo que el encargo señalaba («`EmitterSystem.cpp` no compila suelto y
nadie montó el prefacio»). Los tres prefacios, verificados **contra el `.o` de la
unidad** (HERRAMIENTAS §2):

| fichero | prefacio (`PRE=`, separador `|`) | reproduce |
|---|---|---|
| `eLightE.cpp` | ninguno | 98,65949 % / 75 diffs **exacto** |
| `eMathE.cpp` | ninguno | 85,82089 % / 25 diffs **exacto** |
| `EcstasyEx.cpp` | `#include "dolphin.h"` · `#include ".../EcstasyEx.hpp"` · `#include ".../GameCube/Src/Ecstasy/Ecstasy.hpp"` · `extern cSphereMap SphereMap;` · `extern cSpecularMap SpecularMap;` · las **ocho** tablas `testl1/l2/l3, teste1/e2/e3, testc0/c1` copiadas de `EcstasyE.cpp:3365-3372` | 93,305016 % y 98,99497 % **exactos** |
| `EmitterSystem.cpp` | `#include ".../Misc/Profiler.hpp"` · `#include ".../Interfaces/SimEntities/IPlayer.h"` | 98,074715 %, 696/688 B **exacto** |

Arneses en el scratchpad, prefijo `c30bw_`: `c30bw_h.py` (una función, 3 s),
`c30bw_v.py` (variantes con restauración garantizada), `c30bw_va.py` (variantes
**más** la tabla de allocnos del `.greg`, 8 s), `c30bw_rtl.py` (volcados RTL de
un `.cpp` suelto: `SRC=`, `PRE=`, `TAG=`, `DUMPS=`).

---

## 8. `_bOutput` (5.180 B) — no lo he trabajado

El encargo lo prohíbe salvo con ángulo nuevo y **no lo tengo**: el techo de la
r28 (`LUID(PRE) < LUID(desiredPrecision)` en `sched1`, con `INSN_REG_WEIGHT` y
número de dependientes empatados) **es del mismo tipo que el de
`GenerateHorizonFog`** y mi palanca sólo mueve `live_length`/`n_refs`, que ahí no
intervienen. **Cero ensayos, cero cambios; sigue en 99,98301 % / 2 filas.**
zBWare queda como estaba.

---

## 9. Vedas nuevas, con la sentencia

| función | sentencia barrida | resultado |
|---|---|---|
| `UpdatePlatInfo` | **subir una declaración por encima de `diffuse_min_r`** — 7 formas (f1-f7) | **las siete IDÉNTICAS**: `sched1` reordena el bloque, el fuente no mueve `live_length` |
| `UpdatePlatInfo` | orden de `diffuse_min_r/g/b = diffuse_max_*` en los dos bloques del switch — 4 formas | las cuatro IDÉNTICAS |
| `UpdatePlatInfo` | `diffuse_rng_*` delante de `diffuse_min_*` — 5 formas | **pierden 1-3 instrucciones** (2.040 y 2.032 B) |
| `UpdatePlatInfo` | `asm` sobre temporales de un solo uso (`diff_g/b/r`, `diffuse_max_g`, `specular_min_scale`, `material_data`, `light_material`) — 9 formas | **absorbido**: ni una insn más |
| `eProject` | `asm` sobre `halfVP3` encima de q1 — 6 sitios/formas | **objeto idéntico** (absorbido) |
| `eProject` | orden de `clipX/clipY/clipZ` — 5 formas | 16-32 diffs, ninguna baja de 15 |
| `GenerateHorizonFog` | — | **no hay empate que romper**: prio 12 contra 2 en el mismo ciclo |

Y **una veda de la r29 que caduca**: «el eje `asm` no toca
`GenerateHorizonFog`/`CookieTrail`» era cierto por la forma usada
(`"+r"(<temporal de un uso>)`), no por el eje.

---

## 10. Lo que NO he probado

- **`epCalculate`**: ningún ensayo de fuente. No he identificado el pseudo dueño
  de la ranura de 8 B (haría falta cazar la llamada a `alter_reg`), ni he tocado
  las 106 diffs de `dcrg`/`dcba`/`scba`. **El veto del encargo sigue en pie: los
  13+13+5 movimientos no son eje de planificación.**
- **`Render`**: ningún ensayo de fuente. No he corrido `alloc.py` sobre su
  `.greg` (el banco ya está montado para hacerlo).
- **`UpdatePlatInfo`**: no he atacado el corrimiento de la banda volátil
  (`local_alloc` con f7 ocupado), que es lo único que queda tras g3. No he
  probado pines de registro (último recurso, y `pines.py` dice que los 193 del
  árbol están en funciones ya al 100 %).
- **El permutador** en ninguna.
- **`_bOutput`**: nada, por encargo.
- **Combinar g3 con c4** (asm + reorden de `envmap_min`): no lo he medido.

## 11. Convivencia

Scratchpad con prefijo `c30bw_`. **Disco: 15 GB libres al empezar, 13 GB al
acabar**; el scratchpad está en 3,5 GB y **1,8 GB son de otros agentes**
(`c30lk_try`, `c30lk_o`, `c29lk_try`, `c30da_srcbak`) — **no he borrado nada que
no sea mío**. No he tocado `configure.py`, `config/GOWE69/*` ni `splits.txt`,
**no he escrito ensamblador de instrucciones**, no he tocado los
`#if defined(__ANDROID__)` y **no he hecho commit**. Todas las `asm` de este
informe son cadenas vacías que emiten cero bytes y **ninguna queda en el árbol**.
