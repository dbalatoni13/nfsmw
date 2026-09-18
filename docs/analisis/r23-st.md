# Ronda 23 — `LibSN/steering` + `Play__9EAXCommon` / `EvalState` / `FnStatelessQ`

Encargo: la unidad con el porcentaje más bajo del árbol (53,42 %) y las tres
near-miss de `zEagl4Anim`/`zEAXSound` con el pase ya localizado.

## 0. Línea base VERIFICADA — coincide con el encargo instrucción a instrucción

`build_direct.py LibSN/steering zEagl4Anim zEAXSound` + `triage.py`:

    216 B   99.796%  VDevice_RecalcGammaTable   faltan 10, sobran 10, 0 SUST
    324 B   94.691%  SimThread_Init             faltan  8, sobran  8, 0 SUST
    276 B   88.623%  Effect_Init                faltan  6, sobran  6, 1 SUST
    116 B   93.103%  VDevice_GetFreeEffect      faltan  5, sobran  5, 0 SUST
    924 B   95.671%  SimThread_Step             faltan  2, sobran  1, 2 SUST  mr+2, li-1
    260 B   95.769%  CookValues                 faltan  1, sobran  1          extsb+1, mr-1
    196 B   93.265%  Effect_PerformEnvelope     faltan  1, sobran  0          mr+1
    MURO steering: 3 funciones, 1.768 B (Effect_Update 992, HandleTriggers 588,
                   VDevice_DownloadEffect 188)

    2352 B  91.337%  DynamicLoader::Initialize  faltan 14, sobran 5, 10 SUST
     492 B  96.829%  Play__9EAXCommon…          faltan  3, sobran 2, 1 SUST
     456 B  93.509%  EvalState…FnRawStateChan   faltan  2, sobran 2, 2 SUST  add-2, mr+2
    1480 B  99.459%  EvalSQT…FnStatelessQ       faltan  0, sobran 2          clrlwi-2

    measure.py  LibSN/steering  4680/8760 B  53,4247 %  26 funciones al 100 %

`audit.py` al empezar: **steering 26 ok / 0 FALLA, zEagl4Anim 313 ok / 0 FALLA,
zEAXSound 762 ok / 0 FALLA**. Ningún FALLA que confirmar con segunda pasada.
Repetido al cerrar: **los mismos 26 / 313 / 762, cero FALLA**.

## 1. LA TRAMPA DE LA RONDA: el `#pragma` de cierre mal puesto anula el barrido

**mwcc aplica los pragmas de optimización que ve al terminar de parsear el
cuerpo, no los que ve al empezarlo: dentro de una función manda el ÚLTIMO.**
Medido:

    f1  `#pragma optimization_level 0` delante de Effect_Update, SIN cierre
        -> Effect_Update 99,839 -> 84,226 %, 1.096 B  (y contamina el resto)
    f2  f1 + `#pragma optimization_level 4` delante de `return 1;`
        -> **objeto IDÉNTICO al base** (md5 igual, comprobado)
    f3  lo mismo con `scheduling off` / `scheduling 750`
        -> **objeto IDÉNTICO al base**

O sea: si el pragma de cierre cae **dentro** del cuerpo —y cae ahí en cuanto lo
anclas a la última sentencia en vez de a la llave— el barrido entero sale
«ninguno cambia nada» **y no es verdad: es que no se ha probado**. `optimization
_level` y `scheduling` son los que más lo sufren, porque son los que un pragma
posterior sobreescribe limpiamente; los `X off` / `X on` dan el efecto INVERTIDO
(la función se compila con `X off` sólo si el pragma de dentro es el `off`).

Esto invalidaba mi primer barrido de 60 variantes. **Rehecho con el cierre
detrás de la llave** (`c23st_prag.py`, que localiza la definición y su llave
final y mete el pragma fuera):

| función | mejor de 29 pragmas | base |
|---|---|---|
| `Effect_Update` | 99,83871 (base) | 99,83871 |
| `Effect_Init` | 88,62319 (base) | 88,62319 |
| `Effect_PerformEnvelope` | 93,26530 (base) | 93,26530 |
| `SimThread_Init` | 94,69136 (base) | 94,69136 |
| `SimThread_Step` | 95,67100 (base) | 95,67100 |
| `CookValues` | 95,76923 (base) | 95,76923 |
| `VDevice_GetFreeEffect` | 93,10345 (base) | 93,10345 |
| `VDevice_DownloadEffect` | 99,36170 (base) | 99,36170 |
| `VDevice_RecalcGammaTable` | 99,79630 (base) | 99,79630 |
| `HandleTriggers` | 98,29932 (base) | 98,29932 |

**Ningún pragma abre ninguna función.** La conclusión de la r20 se mantiene,
pero **ahora está verificada sin la trampa**: los 29 pragmas × 10 funciones dan
valores que varían de 41 % a 99 % (el barrido sí muerde), y ninguno sube.

Corrección de paso a la r20: **`scheduling 7400` NO siempre es el objeto
actual**. En `Effect_Update` sí (idéntico), en `Effect_Init` da 85,797 % contra
88,623 % del base. `scheduling 750` sí coincide en las que probé.

## 2. `Effect_Update` (992 B, 99,839 %) — cero ensayos en tres rondas, ahora 58

Los seis diffs son el bloque `EF_RAMP`:

    objetivo: lha r7,0xc  lwz r6,0x4  subf. r3,r7,r0  srawi r0,r6,1
              divw r0,r3,r6   add r3,r7,r0
    nuestro:  lha r6,0xc  lwz r7,0x4  subf. r3,r6,r0  srawi r0,r7,1
              divw r3,r3,r7   add r3,r3,r6

O sea **tres** errores, no uno: (a) `start`/`duration` cambiados de registro,
(b) el `divw` escribe en el registro de `n` en vez de en `r0`, y (c) la suma
final es `cociente + start` y el objetivo hace `start + cociente`.

### La forma que arregla (b) y (c): `b5`

```c
        {
            int d = e->e.duration;
            int n = e->e.u.ramp.end - e->e.u.ramp.start;
            int m = e->elapsed;
            int h = d / 2;
            if (n < 0) h = -h;
            n *= m;
            n += h;
            v = n / d;
        }
        Effect_PolarToRect(e->e.u.ramp.start + v, e->e.u.ramp.direction, &e->force);
```

Dos cambios sobre la fuente actual: **`d` se lee antes que `n`**, y **el cociente
va a `v` (variable distinta de `n`) y la suma con `start` se escribe dentro de la
llamada**. Con eso:

    base:  99,83871 %  6 filas mal = swap start/dur + destino del divw + orden del add
    b5:    99,81855 %  6 filas mal = SÓLO una transposición r3<->r7 de `n` y `start`

`duration` cae en r6 (como el objetivo), el `divw` escribe en r0 (como el
objetivo) y el `add` sale `start` primero (como el objetivo). **Lo único que
queda es un 2-ciclo r3↔r7.**

**NO la he aplicado**: el porcentaje baja 0,02 pp, el número de filas malas es el
mismo (6) y la distancia al cierre —una permutación de dos registros— no cambia.
Queda escrita aquí para adoptarla con una sola edición si la siguiente ronda
decide que el criterio de «opcodes y filas» pesa más que el número.

### Barridos (58 formas de fuente, ninguna cierra)

| | |
|---|---|
| **meseta 99,81855** (= b5, misma transposición) | `b5 c4 c5 c8 c10 c12 c14 d4 d6 d7 d8 d9 d11 h7 h8 h9` — 16 formas |
| **idénticas al base** | `a7` (`n += st` con `st` local), `b1` (`d` primero, resto igual), `b6` (declaración estilo MULDIV), `b12` (`m` antes de `n`), `b13` (`h` antes de `n`), `h2` (`s` tras `d` + `s+v` en la llamada) |
| **peores** | `a1/a2/a6/b8/b9/b11` 99,718 · `a3` 99,476 · `a4/b3/b4/b7/c3/c13/h3/h6` 99,758 · `a5/c9` 99,617 · `a8/d5` 99,778 · `a9` 97,056 · `b2` 99,698 · `b10` 99,536 · `b14` 99,738 · `c1` 99,577 · `c2/c7` 99,677 · `c6` 98,871 (988 B) · `c11/d10` 98,488 · `d3` 99,798 · `d12` 99,718 · `d13` 99,758 · `d14` 99,677 · `h1/h4` 99,718 · `h5` 99,790 |
| no compilan | `d1`, `d2` (la local `s` fuera de ámbito en la llamada) |

**Veda**: barridas 58 formas del bloque `EF_RAMP` —orden de lectura de los cuatro
valores (las 6 permutaciones útiles), `MULDIV` contra el bucle a mano, el
cociente en `n` / en `v` / en `t` / en `level` / en una quinta local, la suma
final en la llamada / en sentencia / dentro de la asignación, `start` cacheada en
local y en variable de función, `h` en `round`, `end` cacheada, el `if` con
llaves, sin la local `m`— más 54 pragmas (25 bien cerrados sobre la forma vieja +
29 sobre la nueva). **La transposición r3↔r7 no se mueve.** Sin probar:
restricciones de registro y el permutador.

## 3. `Effect_PerformEnvelope` (196/192 B, 93,265 %) — 19 ensayos nuevos, muro

Al objetivo le sobra `mr r5, r9` = `out = mag`. Los dos son simultáneamente vivos
en el objetivo (`mag` se usa en las filas 12, 34 y 41, después de la copia de la
fila 7); mwcc **propaga la copia siempre** y fusiona los dos en un registro.

    k1  neg antes de la copia                      IDÉNTICO
    k2  `mag = …; out = mag;` (copia explícita)    IDÉNTICO
    k3  k2 con neg en medio                        IDÉNTICO
    k4  `out` declarada la primera                 IDÉNTICO
    k5  `mag` como u32                             IDÉNTICO
    k6  `out` como u32                             IDÉNTICO
    k7  `mag = out = …`                            IDÉNTICO
    k8  `out = …; mag = out;`                      IDÉNTICO
    k9  `neg = level < 0`                          IDÉNTICO
    k10 abs con `if` en vez de ternario            74,898  196 B  PEOR
    k11 orden de declaración sustain/v primero     IDÉNTICO
    k12 `out` la segunda declarada                 IDÉNTICO
    l1  `out` asignada en TODAS las ramas (dos `else` explícitos, sin copia inicial)
                                                   81,735  204 B  PEOR
    l2  sólo el `else` interior explícito          IDÉNTICO
    l3  primer `if` invertido                      45,714  PEOR
    l4  cola con dos `return`                      88,061  PEOR
    l5  cola con ternario                          88,061  PEOR
    i1  `#pragma opt_common_subs off`              91,837  **196 B**
    i2  `#pragma global_optimizer off`             91,633  **196 B**

**`i1` es el caso 12 de «el tamaño exacto miente»**: clava los 196 B del objetivo
y la instrucción de más **no es el `mr r5,r9`**, es una recarga `lbz r3,0x8(r4)`
de `attackLevel`. Verificado leyendo el diff, fila a fila.

**Veda (28 formas ya, contando la de la r20 y las 10 de la r22)**: la copia
`out = mag` está **agotada por fuente en mwcc 2.7** — orden, tipos, cadena de
asignación en los dos sentidos, copia explícita en cualquiera de las dos
direcciones, `else` explícitos, y los 29 pragmas bien cerrados. Lo único no
probado: restricción de registro sobre `out`/`mag`.

## 4. `SimThread_Init` (324 B, 94,691 %) — 12 ensayos nuevos

Los ocho `faltan`/`sobran` son **dos** cosas y ninguna es código que falte:

* **(a) filas 51-55**: el objetivo emite `li r3,0 ; mtctr r0 ; lfs f1,@sda21` y
  nosotros `lfs f1 ; li r3,0 ; mtctr r0` — desempate del planificador en el
  precabezal del bucle de `ia[]`.
* **(b) filas 67-74**: transposición limpia **r5↔r6** en el último bucle (el cero
  de `st->hist[i]` contra el temporal de dirección `st + i`).

    v1  índice propio `k` para el último bucle   94,753  <- la trampa de siempre
    v2  `n` declarada antes que `i`              IDÉNTICO
    v3  `n` también en el último bucle           IDÉNTICO
    v4  local `z = 0` para `st->hist[i]`         IDÉNTICO
    v5  último bucle como `while`                IDÉNTICO
    v6  `float f = a[n]` intermedio              IDÉNTICO
    v7  bucle de ia como `while`                 IDÉNTICO
    v8  `a[n] * 4096.0f`                         IDÉNTICO
    v9  bucle de ia descendente                  85,432  PEOR
    v10 los dos stores del último bucle al revés 89,864  PEOR
    v11 temporal `t = ia[i]`                     IDÉNTICO
    v12 `impulse[0]` asignado en vez de inicializado  92,148  PEOR

`v1` es **el mismo 94,753 % de `z1` (r20) y `m3` (r22)**: sube 0,06 pp permutando
el mismo problema. Tercera vez que se mide; no volver a gastarlo.

**Veda**: sumadas a las 20 de la r20 y las 5 de la r22 van **37 formas**. El
frente son un desempate del planificador y un 2-ciclo de registros.

## 5. `Play__9EAXCommon18eMenuSoundTriggers` (492 B, 96,829 %) — el hallazgo de la ronda

### 5.1 La hermana que casa al 100 % tiene EXACTAMENTE la forma del objetivo

`Play__9EAXCommonPv` (432 B, **100 %**, mismo fichero, mismos campos) produce:

     8 cmpwi r0,0            ; IsSoundEnabled
     9 li r3,-0x1            ; IZADO
    10 beq  EPI
    11 lwz r3, 0x24(r28)     ; el puntero DIRECTAMENTE en r3
    12 cmpwi r3,0
    13 beq  <bloque -1>
    14 lwz r0, 0xc(r3)       ; GetOutputBlockPtr
    15 cmpwi r0,0
    16 bne  <cuerpo>
    17 li r3,0x0             ; el bloque del guardia EN SU SITIO
    18 b    EPI

que es fila a fila lo que le falta a `Play(eMenuSoundTriggers)`: el `li` izado, el
puntero en r3 sin `mr`, y el bloque `li r3,0 ; b EPI` en su sitio. Su fuente es
la **anidada**: `if (m_pSFXOBJ_FEHUD) { if (!…GetOutputBlockPtr()) return 0; … }
return -1;`.

### 5.2 El mecanismo, medido sobre la hermana

Cambiando **sólo** su `return -1;` final por `return 0;`:

    r1_ret0   100,000 % / 432 B  ->  98,046 % / 424 B, 4 diffs
              desaparecen EXACTAMENTE `li r3,0` y `b EPI` del cuarto guardia
              y el `bne` se convierte en `beq` al bloque final

**Regla nueva, y vale para todo el árbol de ProDG**: si un guardia hace
`return K` y **la última sentencia de la función devuelve el mismo `K`**, GCC
funde los dos bloques y el bloque del guardia desaparece; el `bcond` se
reapunta al retorno final. Es lo que nos pasa a nosotros… salvo que **el
objetivo devuelve 0 en los dos sitios y NO los funde**, y una sonda lo confirma:

    s1_retm1  `return -1;` al final de nuestra Play  ->  96,821 % / **488 B**
              el bloque del cuarto guardia SIGUE sin salir

O sea: en `Play(eMenuSoundTriggers)` la desaparición del bloque **no depende del
valor del retorno final**. El `.cse` (volcado con la receta de r22-ae §2) lo
enseña al insn: `jump_insn 76` pasa de `(ne CC103) → label 84` en `.jump` a
`(eq CC103) → label_ref 451` en `.cse`, con una **insn nueva, la 456**, metida
entre la comparación (75) y el salto (76). En la hermana, `jump_insn 53` llega a
`.cse` intacto (`(ne CC96) → label 62`) y su bloque `insn 58 / jump 60` sobrevive.

**Y sólo se iza UN guardia por función**: el objetivo iza el del segundo guardia
(`li r3,-1` + `bne EPI`) y conserva el bloque del cuarto; nosotros izamos el del
cuarto y cruzamos los tres bloques de `-1` en uno. Con el cuerpo metido dentro
de `if (GetOutputBlockPtr())` (`p5`) **el izado se muda solo al segundo guardia y
las filas 0-21 casan exactamente**, lo que confirma que es un recurso único.

### 5.3 Diagnóstico de banderas: no es `skip-blocks` ni `gcse`

    base                            96,82927  488 B   9 diffs
    -fno-cse-skip-blocks            94,06504  500 B  18 diffs (no arregla la cabecera
                                    y rompe la CSE de g_SliderValue)
    -fno-cse-follow-jumps           96,74797  488 B  11 diffs
    -fno-gcse                       96,82927  488 B   9 diffs (IDÉNTICO)
    -fno-expensive-optimizations    96,74797  488 B  11 diffs

**El izado no lo apaga ninguna de las cuatro.** Es cse «a secas».

### 5.4 Los 18 ensayos de fuente

    p1  guardias 3+4 anidados como la hermana, `return -1` al final   93,488  488 B  15 diffs
    p2  p1 + cuerpo dentro del if de GetOutputBlockPtr                93,285  484 B  12 diffs
    p3  p1 con `else { return -1; }` y `return 0` al final            91,049  492 B  18 diffs
    p4  `== nullptr` con llaves en el cuarto guardia                  IDÉNTICO
    p5  cuerpo dentro de `if (GetOutputBlockPtr())`  (= bb1 de la r22) 96,618  484 B  **6 diffs**
    s1  `return -1;` final (SONDA, cambia la semántica)               96,821  488 B  10 diffs
    s2  `return 1;` final  (SONDA)                                    96,821  488 B  10 diffs
    s3  cuarto guardia con llaves                                     IDÉNTICO
    s4  `void *pblk = …GetOutputBlockPtr(); if (!pblk)`               IDÉNTICO
    s5  `Debug_Common_FE_OFF != 0`                                    IDÉNTICO
    s6  `IsSoundEnabled == 0`                                         IDÉNTICO
    u1  `int nvol = 0;` y el cuarto guardia `return nvol;`            IDÉNTICO
    u2  cadena `else if` en los cuatro guardias                       IDÉNTICO
    u3  llaves en los cuatro guardias                                 IDÉNTICO
    u4  cuarto guardia como `if (ptr) {} else { return 0; }`          IDÉNTICO
    u5  puntero cacheado en local para los guardias 3 y 4             IDÉNTICO
    u6  guardias 1 y 2 unidos con `||`                                IDÉNTICO

**`u6` es un dato aparte que conviene recordar**: `if (!A) return -1; if (B)
return -1;` y `if (!A || B) return -1;` dan el **objeto idéntico** con estos
cflags. Ese eje no distingue nada.

Con `p5` sólo quedan **6 diffs** y son todos el mismo hueco: el objetivo tiene
`bne cuerpo ; li r3,0 ; b EPI` y nosotros `beq <final>`; más el `stw r0,0x1c(r28)`
desplazado dos filas por el arrastre. **Revertido** (baja 0,21 pp).

**Lo que queda por hacer, con el hilo más corto que nunca**: hace falta la fuente
que (a) deje el cuarto guardia en forma de retorno temprano —para que su bloque
se emita ahí— y (b) haga que cse gaste su único izado en el **segundo** guardia.
Los 25 barridos de la r21, los 13 de la r22 y los 18 de aquí demuestran que **no
está en la forma de los cuatro guardias**; y `p5` demuestra que el izado se muda
solo si el cuarto guardia deja de ser candidato. Falta la forma que lo
descandidatee **sin** borrar su bloque.

## 6. `EvalState__…FnRawStateChan` (456 B, 93,509 %) — la idea que la r22 dejó pendiente, probada

Los `add-2, mr+2` son las filas 40 y 80: el objetivo hace `mr r4, base` y nosotros
`add r4, r30, r9` recalculando, **aunque la base ya está viva** (`add r8,r30,r10`
tres filas antes). Además, primera fila del diff: nosotros cargamos `mKeyIdx`
(`lwz r10,0x10(r29)`) en la fila 7 y el objetivo en la 19.

La r22 dejó apuntado como no probado: «nombrar la base en la fuente de
`EvalState` (`unsigned char *base = c->GetKeyData(0);` + `k = base + i*GetKeySize()`)».
**Probado, y es de las malas:**

    e1  base nombrada, usada en los DOS bucles     68,895  424 B  59 diffs
    e2  base nombrada sólo en el `while`           75,386  456 B  52 diffs
    e3  base nombrada sólo en el `for` descendente 71,825  452 B  63 diffs
    e4  `keySize` izado a local + GetKeyData(0)    89,465  448 B  46 diffs
    e5  `while` escrito como `for`                 IDÉNTICO
    e6  base nombrada DENTRO de cada bucle         IDÉNTICO

**Veda ampliada (29 formas: 12 de la r21, 11 de la r22 y 6 aquí)**: nombrar la
base en `EvalState` es tan catastrófico como factorizarla en `GetKeyData`
(−24,6 pp). La forma del original es la que hay; lo que falta es que
`local-alloc` copie en vez de recalcular en dos de los cinco sitios inline.

## 7. `EvalSQT__…FnStatelessQ` — no tocada

La r22 la cerró por fuente con el volcado RTL: 36 formas barridas y demostrado
que el RTL de entrada a `combine` es **idéntico byte a byte** al de la hermana
que sí casa. No he gastado un ensayo: sin poder instrumentar `try_combine` no
hay nada nuevo que aportar y el encargo lo dice.

## 8. Resultado medido

`build_direct.py` antes de **cada** medida (`ninja` no construye LibSN).

    LibSN/steering  4680/8760 B  53,4247 %  26 funciones al 100 %   (SIN CAMBIO)
    zEAXSound + zEagl4Anim + zEAXSound2  409.516/434.364 B  94,2795 %  (SIN CAMBIO)

**Cero bytes, cero funciones cerradas, y NINGÚN fichero del árbol tocado.**
`git status` lo confirma: ni `src/LibSN/steering.c`, ni `EAXFrontEnd.cpp`, ni
`RawStateChan.cpp`, ni ninguna cabecera aparecen modificados. Todas las variantes
se compilaron sobre **copias** en el scratchpad (arneses nuevos, §10).

`audit.py` al cerrar: **26 / 313 / 762 ok, cero FALLA** en las tres unidades.
`frozen.py chk`: `LibSN/steering` **idéntico**, `zEagl4Anim` **idéntico**.

### Aviso: `zEAXSound` no coincide con su congelado, y NO es mío

`frozen.py chk Speed/Indep/SourceLists/zEAXSound` → `*** HA CAMBIADO ***`. El
congelado es de **hoy 17:01**, anterior a mi sesión, y mi fichero no se ha tocado
en ningún momento: `audit.py` da los mismos 762 ok al empezar y al cerrar, y
`triage.py` da las mismas cuatro near-miss. Las funciones que se mueven en esa
unidad son de `EAXAemsManager.cpp` (`SetupNextLoad` 98,671, `DataLoadCB` 94,488,
`ResolveCurrentDataMemory` 90,238, `QueueFileLoad` 99,864), que estaba
**modificado sin commitear al empezar mi sesión y ahora está limpio**: el
congelado se tomó con ese fichero en su estado modificado. No he re-congelado
para no pisar la línea base de quien trabaja ahí.

## 9. Qué NO he probado

* **Restricciones de registro** (`register … asm("rN")`) en ninguna función. Los
  tres candidatos naturales son los 2-ciclos limpios: `Effect_Update` (r3↔r7 con
  la forma `b5`), `SimThread_Init` (r5↔r6) y `Effect_PerformEnvelope` (`out` en
  r5). El encargo las deja para el último recurso y el eje de fuente no está
  agotado en `Play`.
* **`permuter.py`**: no lo he lanzado sobre ninguna función.
* **`DynamicLoader::Initialize`** (2.352 B, 91,337 %): vedada por el encargo, ni
  un ensayo.
* **`FnStatelessQ`**: ni un ensayo (§7).
* **`Effect_Init`, `VDevice_RecalcGammaTable`, `VDevice_DownloadEffect`,
  `CookValues`, `SimThread_Step`, `HandleTriggers`, `VDevice_GetFreeEffect`**:
  sólo el eje de pragmas rehecho (§1). Ni una forma de fuente nueva; sus vedas de
  la r20/r22 siguen en pie y no las he vuelto a barrer.
* **Banderas de unidad**: sólo cuatro como DIAGNÓSTICO sobre `zEAXSound`
  (§5.3), nunca aplicadas. `configure.py`, `config/GOWE69/*` y `splits.txt`
  intactos.
* **Cabeceras compartidas**: no he tocado ninguna. En `EvalState` los ensayos
  e1-e6 son todos del `.cpp`; **`RawStateChan.h` no se ha modificado** (habría
  hecho falta el A/B con `MemoryPoolManager.cpp`, que es la otra unidad que la
  incluye).
* **`.sdata`/`.sdata2` byte a byte contra el ELF**: no comparadas.
* **El DOL**: `LibSN/steering` no está enlazada (`linked False`), así que no
  puede cambiar; y no he cambiado nada en las otras dos. No he ejecutado la
  construcción completa.
* **El volcado RTL de `Effect_Update`/`SimThread_Init`**: no existe para mwcc
  (no hay `-da`), así que ahí sólo hay caso mínimo y pragmas.

## 10. Herramientas dejadas en el scratchpad (prefijo `c23st_`)

* **`c23st_st.py [fuente.c] [SÍMBOLO] [--only]`** — compila `src/LibSN/steering.c`
  (o una copia) con los cflags exactos de mwcc y lista el % por función, o el
  diff lado a lado de un símbolo. **1,0 s por iteración.**
* **`c23st_sw2.py <spec.py> [nombres…]`** — barrido multi-parche sobre steering.
  El spec es `VARIANTS = {nombre: [(bloque_viejo, bloque_nuevo), …]}`; comprueba
  que cada bloque aparezca **una sola vez**, casa con `\r?\n` por línea y
  **nunca toca el árbol** (compila una copia). ~0,5 s por variante.
* **`c23st_prag.py [funciones…]`** — barrido de 29 pragmas por función con el
  **cierre fuera del cuerpo** (§1). Detecta solo el `optimization_level` vigente
  antes de cada función para restaurarlo bien.
* **`c23st_eax.py <spec.py>`** — el equivalente para ProDG: copia el `.cpp` al
  scratchpad, lo parchea y compila un stub que lo incluye **por ruta absoluta**,
  así que **no toca el árbol** (a diferencia de `c22ae_sw.py`). `EXTRA=…` añade
  banderas de diagnóstico, `DUMP=<nombre> ONLY=1` vuelca el diff. 15 s en
  zEAXSound, 1 s en zEagl4Anim (ahí hace falta `EXTRA="-I
  src/Speed/Indep/Src/EAGL4Anim"` porque `RawStateChan.cpp` incluye su cabecera
  por ruta relativa).
* `c23st_spec_[a-e,g,h,i,k,l,v].py` — los ensayos de steering.
* `c23st_spec_[p,r,s,u,play0].py` — los de `Play`; `c23st_spec_ev.py` los de
  `EvalState`.
* `c23st_fe.ii`, `c23st_fe.ii.jump`, `c23st_fe.ii.cse` y los extractos
  `c23st_jump_ms.txt` / `c23st_cse_ms.txt` / `c23st_jump_pv.txt` /
  `c23st_cse_pv.txt` — el volcado RTL de `EAXFrontEnd.cpp` con las dos `Play`
  enfrentadas. **Los 15 volcados grandes (58 MB el `.rtl`) los he borrado.**
