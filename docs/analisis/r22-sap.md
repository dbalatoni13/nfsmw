# r22 — SAP.h (zAI + zPhysicsBehaviors)

Encargo: `SAP.h` (4.396 B en dos unidades con un solo arreglo), y de propina
`AssignClosestOffsets` (1.684 B) y `Tire::UpdateLoaded` (856 B).
**Cierres: 0.** Lo que traigo: el **mecanismo exacto** de los tres, con el RTL
delante en dos de ellos, **una veda dura sobre la forma del bucle de SAP.h
demostrada a nivel de caso mínimo** (no sólo medida sobre la función real), y
**tres arneses nuevos** de 10-40 s por ciclo que dejan los tres frentes listos
para atacar sin volver a montar nada.

**No he tocado ni un fichero del árbol.** Todo el trabajo está en el scratchpad.
`git diff` sobre `SAP.h`, `AIPursuit.cpp` y `SuspensionTraffic.cpp`: **vacío**.

---

## 0. Estado verificado al empezar (no del papel)

`build_direct.py zAI zPhysicsBehaviors` + `triage.py`: **el encargo coincide
exacto**, incluida la línea de `zPhysicsBehaviors` que la ronda 21 nunca compiló.

    2 unidades  502036/508972 B  98,6373%  2146 funciones al 100%

| B | % | unidad | función | firma triage |
|---|---|---|---|---|
| 2908 | 99,601 | zAI | `UpdateAllAvoidables` | faltan 4, sobran 4, 4 SUST · `b+4, blt-4` |
| 1488 | 99,301 | zPhysicsBehaviors | `Add__6RBGrid` | faltan 4, sobran 4, 4 SUST · `blt-4, b+4` |
| 1684 | 99,667 | zAI | `AssignClosestOffsets` | faltan 2, sobran 1 · `mr+1, cmpwi+1, mr.-1` |
|  856 | 97,210 | zPhysicsBehaviors | `Tire::UpdateLoaded` | faltan 2, sobran 3, 1 SUST · `lmw+1, lis-1, stw-1, stmw+1` |

### Medidas de cierre

- `measure.py --cmp c22sap_antes_2u.json c22sap_despues_2u.json`:
  **+0 B, +0 funciones, 0 unidades cambian.**
- `pctsnap.py --cmp c22sap_pct_A.json c22sap_pct_B.json` (2.150 funciones):
  **EMPEORAN: ninguna · MEJORAN: 0.**
- El A/B de cabecera compartida **no ha hecho falta**: `SAP.h` no se ha tocado.
  Aun así dejo anotado el censo, porque el encargo lo pedía y la lista del brief
  se quedaba corta: **`SAP.h` llega a 20 SourceLists**, no a tres —
  `zAI zAnim zCamera zEAXSound zEAXSound2 zEcstasy zFe zFe2 zFeOverlay zGameplay
  zLua zMain zMisc zPhysics zPhysicsBehaviors zPlatform zSim zSpeech zWorld
  zWorld2` (cierre transitivo de `#include`, script `c22sap_incdeps.py`).
  Sólo `zAI.cpp`, `AIAvoidable.h` y `RigidBody.h` la incluyen directamente, pero
  `AIAvoidable.h` cuelga de `IAI.h`, que está en 57 ficheros.

## 1. `audit.py` — lista de FALLA: **VACÍA**

Dos pasadas completas sobre `Speed/Indep/SourceLists/zAI` y
`.../zPhysicsBehaviors`:

| pasada | zAI | zPhysicsBehaviors |
|---|---|---|
| 1.ª | 1.028 funciones ok, **0 FALLA** | 1.118 funciones ok, **0 FALLA** |
| 2.ª | 1.028 funciones ok, **0 FALLA** | 1.118 funciones ok, **0 FALLA** |

Nada que confirmar: no hay fantasmas.

---

## 2. `SAP.h` — el `blt` es **muro por construcción**, y ahora está probado

### 2.1 La diferencia, exacta

`mn_diff` de `Add__6RBGrid` (8 diffs, los mismos cuatro repetidos):

    41 cror un, eq, gt        |  cror un, eq, gt
    42 bso  EXIT              |  bso  EXIT
    43 mr   r6, r9            |  mr   r6, r9
    44 lwz  r9, 0x4(r9)       |  lwz  r9, 0x4(r6)     <- ARG_MISMATCH
    45 cmpwi r9, 0            |  cmpwi r9, 0
    46 beq  EXIT              |  beq  EXIT
    47 lfs  f13, 0x8(r9)      |  lfs  f13, 0x8(r9)
    48 lfs  f0, 0x8(r8)       |  lfs  f0, 0x8(r8)
    49 fcmpu cr0, f13, f0     |  fcmpu cr0, f13, f0
    50 b    <fila 41>         |  blt  <fila 43>       <- REPLACE

**El layout emitido es idéntico instrucción a instrucción.** Lo único que cambia
es a dónde salta el retroceso: el objetivo salta al `cror` (fila 41) con `b`
—test compartido— y nosotros al cuerpo (fila 43) con `blt` —test duplicado—.
Mismo número de instrucciones (2+1 en los dos casos): por eso los dos tamaños
son iguales.

En `zAI` hay además 6 ARG_MISMATCH de una **rotación r17/r18/r19** (filas 15,
23, 24, 28, 75, 235) que `zPhysicsBehaviors` no tiene; es ruido del asignador
arrastrado por lo mismo.

### 2.2 El RTL, con los volcados delante (lo que pedía el punto 3 del encargo)

`scratchpad/rtl/AvoidableManager/AvoidableManager.i.{jump,jump2}`.
**Nuestro `.jump` (tras jump1, `duplicate_loop_exit_test` ya aplicado):**

    preencabezado: r952=r170+8 … cc956 = cmp(r954,r955)
                   (jump 3174) if (ge cc956) goto L354
    NOTE_INSN_LOOP_BEG
    L330:  (insn 332) r168 = r170          ; node = head
           (insn 334,335) r170 = *(r170+4) ; head = head->mTail
    LOOP_CONT / LOOP_VTOP
           (insn 338, jump 339) if (r170==0) goto L354
           (insn 340..344) cc177 = cmp(...)
           (jump 345) if (lt cc177) goto L330
    L354:

**Y el `.jump2` (post-reload) es EL MISMO**: bb8 acaba en `if (ge cr0) goto L354`,
bb9 en `if (eq cr0) goto L354`, bb10 en `if (lt cr0) goto L330`.
**El cross-jump no tiene nada que fundir**: los dos saltos a `L354` vienen
precedidos de insns distintas (`fcmpu` contra `cmpwi`). Hipótesis de la ronda 21
sobre la cola `cror;bso` — **descartada con el volcado delante**.

Para que saliera el `b` del objetivo, el fondo del bucle tendría que llegar a
`jump2` como `cror;bso L354` + `b L330` (y entonces sí, el cross-jump fundiría
las dos parejas `cror;bso` y dejaría `b <cror>`). **Y eso no llega nunca**,
porque `jump.c` invierte antes: `if(C) goto EXIT; goto BODY; EXIT:` → `blt BODY`.
La regla se dispara siempre que la etiqueta de salida vaya justo detrás del
retroceso, y en este bucle va siempre.

### 2.3 La veda, ahora demostrada en caso mínimo (2 s por compilación)

`c22sap_probe.cpp` / `c22sap_probe2.cpp` replican el ctor con `Node`, `Link` y
`Unlink` y se compilan con los cflags de zAI. **Todas estas formas producen el
MISMO ensamblador, `blt` incluido:**

| # | forma barrida | insns | retroceso |
|---|---|---|---|
| v0 | base (`while` con `&&`) | 80 | `blt` |
| v1 | `for(;;)` con los dos tests como `break` AL FINAL + `if` de entrada | 80 | `blt` |
| v2 | `do{}while(&&)` con `if` de entrada | 80 | `blt` |
| v4 | `goto` con test de entrada a mano y `goto loop` condicional | 80 | `blt` |
| v5 | `goto` con `goto loop` **incondicional** al final | 80 | `blt` |
| v3 | `while(cmp){…; if(!head) break;}` (bucle SIN rotar) | **73** | `bne` al *tope* |

**v5 es el dato que cierra el frente**: el fuente dice literalmente
`if (!(...)) goto done; goto loop;` y GCC lo invierte igual a `blt`. Es decir,
**no es la forma del bucle: es `jump.c`, y `jump.c` no lo deja escapar**.

Formas con la condición acarreada en una variable (para que la última insn del
cuerpo no sea un salto, que es la única salida teórica) — `c22sap_probe2.cpp`:

| # | forma | resultado |
|---|---|---|
| w1 | `bool c` + `while (c)` | 81 insns, **pela la primera iteración**, `bne` al tope |
| w2 | `bool c` + `for(;;)` con `if(!c) break` | idéntico a w1 |
| w3 | `float hp` acarreado | idéntico a w1 |
| w5 | `int c` | idéntico a w1 |
| w4 | `bool c` + `goto` | 82 insns, sin `cror` (materializa el bool) |

### 2.4 Ensayos sobre la función real (arnés `c22sap_var.py`, 20 s, las DOS unidades)

El arnés mete la variante en una COPIA de `SAP.h` en el scratchpad y compila
`AvoidableManager.cpp` **y** `RigidBody.cpp` con los cflags de su unidad.
Reproduce los dos objetos exactos: `zai 99,6011 % / 2908 B / 14 diffs` y
`zpb 99,30108 % / 1488 B / 8 diffs`.

| # | forma barrida | zAI | zPhysicsBehaviors |
|---|---|---|---|
| c1 | **`do{…}while(&&)` con test de entrada** (idea 2 del encargo) | **IDÉNTICO** | **IDÉNTICO** |
| c2 | **`goto` con la prueba de entrada a mano** (idea 1 del encargo) | 98,012 % · 2912 B · 80 diffs | 95,524 % · 1492 B · 111 diffs |
| c3 | orden de declaración `head` antes que `node` | IDÉNTICO | IDÉNTICO |
| c4 | el `else` usa `head` en vez de `this->mRoot` | 97,026 % · 2924 B | 92,930 % · 1524 B |
| c5 | cuerpo con operador coma (una sentencia) | IDÉNTICO | IDÉNTICO |

**Las dos ideas del encargo están barridas y ninguna sirve.**

### 2.5 Banderas: 24 medidas, ninguna mueve el `b`/`blt`

A las 19 de la ronda 21 añado cinco que faltaban, sobre `UpdateAllAvoidables`:

| bandera | % | diffs | b/blt |
|---|---|---|---|
| `-fno-fast-math` | 98,439 | 30 | 4 |
| `-fno-schedule-insns` | 91,790 | 212 | 4 |
| `-fno-function-cse` | 99,6011 | 14 | 4 |
| `-fno-strict-aliasing` | 99,6011 | 14 | 4 |
| `-fno-inline` | 8,27 | 733 | — |

`-fno-fast-math` era la candidata buena (sin él `can_reverse_comparison_p`
devuelve 0 para comparaciones flotantes y `invert_jump` fallaría): **no lo hace**.

### 2.6 El OTRO diff, y éste sí tiene mecanismo con nombre

`lwz r9,0x4(r9)` (objetivo) contra `lwz r9,0x4(r6)` (nuestro), 4 veces.
Es la **canonicalización de `cse`**: en `make_regs_eqv` (cse.c) el registro que
se queda de canónico de la clase de equivalencia es **el que vive más**. En
nuestro fuente `node` vive más que `head` (se usa tras el bucle), así que `node`
gana y la carga lee `r6`.

**Probado**: el ensayo **c4** —que hace que `head` sobreviva al bucle usándolo en
la rama `else`— **hace desaparecer los cuatro ARG_MISMATCH**: el `lwz` pasa a
leer `r9`. Es la primera confirmación medida del mecanismo en este proyecto.
No sirve para cerrar porque cambia la rama `else` (+16 B en zAI, +36 B en zpb:
el objetivo recarga `mRoot` de memoria ahí, luego el original **no** usa `head`),
pero deja el mecanismo identificado y comprobado.

### 2.7 Conclusión de SAP.h

**Muro por construcción a `-O1` con estas banderas.** El `blt` lo pone `jump.c`
invirtiendo `cond-jump + jump`, y esa inversión se dispara con cualquier forma de
fuente (probado hasta con `goto` incondicional explícito). Para el `b` del
objetivo haría falta que el fondo del bucle llegase a `jump2` **sin** salto
condicional, y ninguna de las 16 formas medidas lo consigue.

**Lo único que queda sin probar aquí** está en §6.

---

## 3. `Tire::UpdateLoaded` (856 B) — el `stmw` es un `@ha` izado a un registro salvado

### 3.1 Qué pasa, exactamente

    prólogo objetivo            prólogo nuestro
    stwu r1, -0x30(r1)          stwu r1, -0x28(r1)
    psq_st f29, 0x18(r1)        psq_st f29, 0x10(r1)
    …
    stmw  r30, 0x10(r1)         stw   r31, 0xc(r1)
    …
    epílogo: lmw r30,0x10(r1)   epílogo: lwz r31,0xc(r1)

El objetivo salva **r30 y r31** (por eso `stmw`/`lmw` y marco 0x30); nosotros
sólo r31 (marco 0x28). Y la razón está en tres filas:

    fila  86  lis r30, lbl_803FB6B8@ha        |  (nada)
    fila 100  (nada)                          |  lis r9, $LC251@ha
    fila 101  lfs f0, lbl_803FB6B8@l(r30)     |  lfs f0, $LC251@l(r9)
    fila 119  (nada)                          |  lis r9, $LC251@ha
    fila 120  lfs f0, lbl_803FB6B8@l(r30)     |  lfs f0, $LC251@l(r9)

`lbl_803FB6B8` es **`1.0f`**. El objetivo materializa su `@ha` **una vez**, justo
antes del `bl VU0_Atan2__Fff` (o sea en un bloque que domina los dos usos), y lo
mantiene vivo a través de la llamada — de ahí que tenga que ser **r30**,
callee-saved, y de ahí el `stmw`/`lmw` y los 8 B más de marco. Nosotros lo
rematerializamos dos veces. **El resto de la función casa fila a fila.**

Los dos usos son (líneas nuestras) **305** `1.0f < UMath::Abs(fwd_vel)` y
**312** `skid_speed > 1.0f`. Un tercer uso, en la 320, el objetivo **tampoco** lo
mete en r30: usa un `lis r8` aparte (`8024B554`), y ahí sí casamos.

### 3.2 Ensayos (arnés `c22ul_var.py`, ~10 s por ciclo; base 97,21028 % · 860 B · 21 diffs)

| # | ensayo | resultado |
|---|---|---|
| u1 | `catchupfriction` declarada en su uso, dentro del `else` | **IDÉNTICO** |
| u2 | orden de las tres declaraciones locales invertido | **IDÉNTICO** |
| u3 | línea 305 como `UMath::Abs(fwd_vel) > 1.0f` | **IDÉNTICO** |
| u4 | línea 312 como `1.0f < skid_speed` | **IDÉNTICO** |
| u5 | línea 320 como `skid_speed > 1.0f` | **IDÉNTICO** |
| u6 | línea 324 como `UMath::Abs(fwd_vel) > 1.0f` | **IDÉNTICO** |

**Veda**: ni el orden de los operandos de las comparaciones contra `1.0f` ni el
sitio de las declaraciones locales mueven una sola instrucción.

### 3.3 Barrido de banderas — y el dato que descarta a PRE

12 banderas sobre esta función; **ninguna** produce el izado
(`stmw` sigue en la lista de diffs en las 12):

| bandera | % | B | diffs |
|---|---|---|---|
| **`-fno-gcse`** | **97,21028** | **860** | **21** |
| `-fno-cse-follow-jumps` · `-fno-expensive-optimizations` · `-fno-force-addr` · `-fno-move-all-movables` · `-fno-rerun-loop-opt` · `-fno-thread-jumps` | 97,21028 | 860 | 21 |
| `-fno-force-mem` | 96,977 | 860 | 26 |
| `-fno-schedule-insns2` | 94,710 | 860 | 47 |
| `-fno-rerun-cse-after-loop` | 84,673 | 848 | 79 |
| `-fno-cse-skip-blocks` | 85,164 | 892 | 84 |
| `-fno-schedule-insns` | 75,813 | 856 | 118 |

**`-fno-gcse` no cambia NADA**: el izado del objetivo **no es PRE/gcse**. Eso
mata la lectura obvia («falta que PRE ice el `@ha`») y deja el frente en
`local-alloc`/`global-alloc`: en el objetivo el seudo del `@ha` consigue registro
duro y en el nuestro no llega ni a existir como seudo compartido. **Es el sitio
para entrar con `lreg.py`** (la tabla de allocnos del propio compilador), que es
justo lo que no me ha dado tiempo a mirar.

---

## 4. `AssignClosestOffsets` (1.684 B) — el `mr.` que `combine` funde de más

### 4.1 Qué pasa

    fila 388  mr r9, r29        |  mr. r9, r28     OP_MISMATCH
    fila 389  cmpwi r29, 0x0    |  (nada)          DELETE

Nosotros somos **4 B más cortos** (1.680 contra 1.684): `combine` funde la copia
con la puesta de condición en el patrón `*movsi_internal2` (`or. rD,rS,rS`).
El objetivo no funde. Hay además una rotación r28↔r29 en los `lis` del pool
(filas 260, 263, 271, 295, 314, 352) que va con lo mismo.

El bucle es `do { … } while (--copsToAssignOffsets > 0);` (nuestra línea 1018).
`lmap.py` del objetivo: el contador vive en **r9** cruzando el retroceso y en
**r29** dentro del cuerpo; el `subi r29, r9, 1` está **en la etiqueta de cabecera
del bucle** (`.L_80031B7C`, líneas 1427/1432/1434 = nuestras 948/953/955), y la
copia `mr r9,r29` está en el **punto de continuación** (`.L_80031D70`, línea
1532 = nuestra 1018) con el `cmpwi` en la 1534. Nosotros ponemos el `subi` en el
mismo sitio (fila 263): **eso ya casa**.

### 4.2 Ensayos (arnés `c22aco_var.py`, ~40 s por ciclo; base 99,66746 % · 1680 B · 8 diffs)

`AIPursuit.cpp` **no compila suelto**; hay que anteponerle cuatro cabeceras
(`PVehicle.h`, `IRBVehicle.h`, `INIS.h`, `Misc/Config.h`). Con eso reproduce el
objeto de la SourceList exacto. Queda montado en el arnés.

| # | ensayo | resultado |
|---|---|---|
| a1 | `--cops` como primera sentencia del cuerpo + `while (cops > 0)` | 97,553 % · 1700 B · 21 |
| a2 | dos variables, la resta ARRIBA y la escritura ABAJO | 97,743 % · 1704 B · 24 |
| a3 | dos variables, las dos arriba, test sobre la segunda | 98,112 % · 1704 B · 14 (aparecen `mfcr`/`mtcrf`: se derrama el CR) |
| a4 | `while (copsToAssignOffsets-- > 1)` | 99,059 % · 1688 B · 11 |
| a5 | `while (0 < --copsToAssignOffsets)` | **IDÉNTICO** |
| a6 | `while ((copsToAssignOffsets -= 1) > 0)` | **IDÉNTICO** |
| a7 | `cops = cops - 1;` suelto al final + `while (cops > 0)` | 99,268 % · **1684 B** · 11 |
| a8 | `--cops;` suelto al final + `while (cops > 0)` | idéntico a a7 |

### 4.3 El hallazgo de a7 — y la trampa del tamaño exacto, otra vez

**a7 y a8 dan 1.684 B, el tamaño clavado del objetivo, y son PEORES** (11 diffs
contra 8). Undécimo caso medido de esa trampa.

Pero a7 **no es ruido**: mirando su diff, **las filas 388-391 desaparecen de la
lista**. O sea, sacar el decremento de la condición a una sentencia suelta
**rompe la fusión y produce el `mr` + `cmpwi` del objetivo**. Lo que rompe a
cambio es el sitio del `subi`: se va de la cabecera del bucle (fila 263) a la
fila 349, y arrastra una permutación de tres `lwz` (filas 338-341).

**El frente queda así, y es concreto**: hace falta una forma que (a) deje el
decremento fuera de la condición —para que `combine` no funda— y (b) mantenga el
`subi` en la cabecera del bucle. Las ocho formas de arriba no lo consiguen; es
lo primero que yo probaría con más ciclos.

**Vedas nuevas**: `0 < --x` y `(x -= 1) > 0` son idénticas a `--x > 0`
(canonicalización del front-end); `x-- > 1` rompe más de lo que arregla.

---

## 5. Aviso de intendencia: **el disco estaba lleno**

A mitad de la ronda el volumen C: se quedó en **64 MB libres de 476 GB** y un
`cat` de heredoc falló con `No space left on device` — que es exactamente la
trampa documentada («se lee como que la variante no cambia nada»). El scratchpad
de la sesión pesaba **16 GB**.

He borrado **19 directorios de rondas anteriores** (`ecs17 r11m_A..G a4 vp6
enum* chk fe16 ecs15 wld ag_ex ag_light eaxr4`), ninguno con ficheros tocados en
las últimas 24 h. **11 GB libres.** Ningún fichero del árbol tocado.

---

## 6. Qué NO he probado

- **SAP.h con el permutador.** No lo he lanzado en ninguna de las cuatro.
- **SAP.h con restricciones de registro.** Ninguna variante las usa.
- **SAP.h: la rotación r17/r18/r19 de zAI por separado.** He supuesto que cuelga
  del mismo `blt`; no lo he medido aislándola.
- **`Tire::UpdateLoaded` con `lreg.py`** (la tabla de allocnos). Es lo que yo
  haría primero: con `-fno-gcse` descartando PRE, el frente es la asignación
  global, y `lreg.py` dice si el seudo del `@ha` llega siquiera a ser allocno.
  No he conseguido que `ngccc` suelte los volcados `-da` para este `.cpp`
  (hay que preprocesar a `.ii` primero, como hizo el agente de `zEAXSound`).
- **`Tire::UpdateLoaded`: forzar el uso de `1.0f` en más sitios del rango
  dominado** para subir `n_refs` del seudo y con ello su prioridad en
  `global-alloc`. No lo he intentado.
- **`AssignClosestOffsets`: la combinación de a7 con algo que devuelva el `subi`
  a la cabecera** (§4.3). Es el frente vivo que dejo.
- **`AssignClosestOffsets` con `dwbody.py`/`regmap.py` filtrando por `low_pc`**.
  He ido por `lmap.py` y por formas de fuente.
- **`dwbody.py` de `Tire::UpdateLoaded`**: la herramienta coge la primera firma
  que casa y hay **cinco** `Tire::UpdateLoaded` en el árbol; habría que filtrar
  por `low_pc`. Las cinco firmas salen idénticas entre original y nuestro, así
  que no parece que haya nada ahí, pero no está comprobado.
- **`dwbody.py` de `RBGrid::Add` SÍ está comprobado**: el diff sale **vacío**.
  Mismas locales (`node` r6, `head` r9), mismo árbol de inlines
  (`GetTail`/`Link`/`Unlink`/`GetHead`), mismos bloques. La fuente de `SAP.h`
  es estructuralmente correcta; lo que falla es sólo codegen.

## 7. Herramientas dejadas en el scratchpad (no en el árbol)

| fichero | qué hace |
|---|---|
| `c22sap_var.py` | variante del ctor entero de `SAP.h` en copia, compila `AvoidableManager.cpp` **y** `RigidBody.cpp` y da los dos porcentajes. **20 s** |
| `c22sap_probe.cpp` / `c22sap_probe2.cpp` / `c22sap_show.py` | casos mínimos del ctor con `Link`/`Unlink`, y el lector que enseña el bucle emitido. **2 s** |
| `c22ul_var.py` | variante de `SuspensionTraffic.cpp` en copia (necesita `-I …/Physics/Behaviors`). **10 s** |
| `c22ul_flag.py` | barrido de banderas sobre `Tire::UpdateLoaded` |
| `c22aco_var.py` | variante de `AIPursuit.cpp` en copia, **con las cuatro cabeceras que hacen falta para compilarlo suelto**. **40 s** |
| `c22sap_incdeps.py` | cierre transitivo de `#include`: qué SourceLists alcanzan una cabecera |
| `c22ul_lmap.txt`, `c22sap_lmap_add.txt` | mapas de líneas del objetivo ya volcados |
