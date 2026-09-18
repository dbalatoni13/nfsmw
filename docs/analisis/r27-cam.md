# Ronda 27 — zCamera

    ANTES   zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %
    DESPUES zCamera  113.080/125.008 B   90,4582 %   446 funciones al 100 %

**Bytes de `.text` ganados: 0.** El árbol queda **byte a byte como lo encontré**:
`git status src/Speed/Indep/Src/Camera/` y `src/Speed/Indep/Libs/` **vacíos**,
md5 de `UTLVector.h` = `d2d4c8e6…` y de `TrackCop.cpp` = `f7c2a733…`, los mismos
del arranque. `audit.py`: **446/446 ok, cero FALLA**, tres pasadas idénticas.
Todos los barridos se hicieron sobre **copias en el scratchpad** (una copia de
`TrackCop.cpp` y una de `UTLVector.h` tapada con un `-I` delante); el árbol
compartido no se tocó ni una vez.

Lo que traigo:

1. **`TrackCop::Update` (948 B): la veda nº 4 de la r24 se cae — su premisa era
   falsa — y en su sitio queda el mecanismo COMPLETO, con volcado y con dos
   controles.** El desempate de `sched2` no lo decide el orden de la FUENTE: lo
   decide el orden que dejó **`sched1`**, y el que lo rompe es **`regmove`**. (§1)
2. **El modelo de `rank_for_schedule` verificado al dígito, con sus CINCO
   niveles** —y `INSN_REG_WEIGHT`, que sólo actúa en `sched1`, no estaba escrito
   en ninguna parte del proyecto. Predice tres decisiones distintas del volcado
   y un control de fuente. (§1.2)
3. **`_Storage`: la pregunta que dejó la r26 CONTESTADA, y su hipótesis era
   equivocada.** Los +16 B de `d4` no son la llamada virtual `FreeVectorSpace`:
   son **la coexistencia de las dos mitades de `make_empty()`**. Cada mitad por
   separado es **byte-neutra** (1160 B, 16 diffs = base); juntas cuestan 16 B.
   Siete medidas nuevas. (§2)
4. **`TerrainVelocityNoise` (1.192 B): DWARF fresco = CERO diferencias**, y la
   causa de las 14 filas es concreta y nueva: **nosotros gastamos CINCO registros
   para cinco direcciones del pool y el objetivo gasta CUATRO** (reusa `r9`).
   Nuestro orden de `lis` es exactamente el de «distancia al uso»; el del
   objetivo es ése menos el reuso. (§3)
5. **Medida de veto para todo el proyecto**: `-fexpensive-optimizations` es
   **obligatoria** en zCamera. Sin ella la unidad cae de **83,3357 %** a
   **53,8081 %** (615 → 562 funciones al 100 %). (§1.4)

## 0. El encargo estaba VIGENTE

`build_direct.py zCamera` + `triage.py zCamera --muro` reproducen el brief clavado:

    3604 B  97.112%  __static_initialization_and_destruction_0  faltan 4, sobran 8, 2 SUST  addi-4
    1156 B  97.834%  __Q33UTL11Collectionst8_Storage2ZPQ28Camer faltan 1, sobran 2, 1 SUST  cmpwi-1, mr.+1, lwz-1
     168 B  96.667%  LoadCameraShakes__10ICEManagerP6bChunk     faltan 1  li+1
    MURO: 3868 ICEMover::Update / 1192 TerrainVelocityNoise / 992 TrackCar / 948 TrackCop

`measure.py zCamera`: **113080/125008 B, 90,4582 %, 446 al 100 %**, al empezar y
al terminar. `audit.py`: **446/446 ok, 0 FALLA**.

**Nota de lectura que ahorra media hora**: `triage.py` marca `TerrainVelocityNoise`
y `TrackCop` como «reorden local, dmax 2, **CERO reales**». Eso no es un muro
opaco: **`TrackCop` es UNA sola instrucción desplazada una ranura** (948 B por
un intercambio), y conviene sacar el `fndiff` antes de creerse la etiqueta.

## 1. `TrackCop::Update` (948 B): una instrucción, y ahora el mecanismo entero

`fndiff` deja **exactamente dos filas**:

    152 addi r3, r1, 0x68     | addi r3, r1, 0x68
    153                       | stfs f30, 0x58(r1)     <- NUESTRO, una ranura antes
    154 mr r5, r3             | mr r5, r3
    155 stfs f30, 0x58(r1)    |                        <- OBJETIVO

    objetivo:  629 lwz r4  ·  627 addi r3  ·  646 mr r5,r3  ·  579 stfs 0x58 · …
    nuestro :  629 lwz r4  ·  627 addi r3  ·  579 stfs 0x58 ·  646 mr r5,r3 · …

`TrackCop.cpp` **compila suelto** con los cflags de zCamera en **4-5 s** y
reproduce nuestra forma clavada, así que todo lo de abajo es medido, no deducido.

### 1.1 La premisa de la r24 era falsa

La r24 dijo: «las dos instrucciones caen en el mismo ciclo y el desempate por
**LUID** favorece siempre a la primera `stfs` esté donde esté el bloque», y de
ahí sacó que mover el `bScale` no puede dar el orden del objetivo.

El desempate por LUID es correcto. **Lo que es falso es que ese LUID sea el de la
fuente.** `INSN_LUID` se reasigna al principio de CADA pase de planificación
recorriendo la cadena RTL; el LUID que ve `sched2` es **el orden que dejó
`sched1`**, no el de las sentencias. Volcado (`cc1plus … -dR -fsched-verbose-5`),
tabla de dependencias del bloque 15 **después de reload**, en orden de cadena:

    ;;      insn  code    bb   dep  prio  cost   units      dependientes
    ;;      629   512     0     2    18     2    lsu    : 795 648 623 616 609 585 582 644 579
    ;;      627    52     0     1    17     1    iu2    : 795 782 761 753 648 646
    ;;      579   523     0     3    16     2    lsu    : 795 782 761 648
    ;;      644    52     0     3    16     1    iu2    : 795 755 648
    ;;      646   512     0     3    16     1    iu2    : 795 753 757 648
    ;;      582   523     0     3    16     2    lsu    : 795 782 761 648
    ;;      585 … 609 … 616 … 623   (todas prio 16, 4 dependientes)

**Las SIETE candidatas tienen `prio` 16 y CUATRO dependientes.** No es que la
`579` tenga más prioridad, como suponía la r24: es un empate total, y el orden
de la cadena que entra a `sched2` es **579, 644, 646, 582, 585, 609, 616, 623**
— o sea la `579` ya viene delante de la `646` desde `sched1`.

### 1.2 El modelo de `rank_for_schedule`, con sus cinco niveles y verificado

La lista de listos se ordena de peor a mejor y `schedule_block` extrae de
`ready[n-1]`. Los desempates, en orden:

| nivel | criterio | ¿en `sched1`? | ¿en `sched2`? |
|---|---|---|---|
| 1 | `INSN_PRIORITY` mayor | sí | sí |
| 2 | **`INSN_REG_WEIGHT` MENOR** (regs que nacen − regs que mueren) | **sí** | no (`reload_completed`) |
| 3 | clase respecto de la última planificada (3 = independiente o coste 1; 2 = anti/salida; 1 = de datos): gana la clase **mayor** | sí | sí |
| 4 | **más dependientes** (`INSN_DEPEND`) | sí | sí |
| 5 | `INSN_LUID` menor | sí | sí |

Verificado con tres decisiones del volcado que **no** se explican sin él:

- `sched2` t=31: el grupo de `prio` 11 sale `753, 759, 757, 755`; por LUID sería
  `759, 757, 753, 755`. La `753` tiene **4** dependientes y las otras **3** →
  nivel 4. ✔
- `sched1` t=25: `644` gana a `646` aunque `646` tiene **4** dependientes y `644`
  **3**. `644` es `%4=r316+0x48` con `REG_DEAD r316` → peso **0**; `646` es
  `%5=%3` **sin** nota de muerte → peso **+1** → nivel 2. ✔
- `sched1` t=24: `642` (`prio` 17) por delante de `579` (16) → nivel 1. ✔

**El nivel 2 (`INSN_REG_WEIGHT`) no estaba documentado en el proyecto** y es el
que decide este caso.

### 1.3 Quién rompe el orden: `regmove`, y está en el volcado

Cadena de pases: `combine` → **`regmove`** → `sched1` → alloc → reload → `sched2`.

    .combine :  (insn 642  (set (reg:SI 3 r3) (reg:SI 315)))          ; sin REG_DEAD
                (insn 646  (set (reg:SI 5 r5) (reg:SI 315)))          ; REG_DEAD (reg 315)

    .regmove :  (insn 642  (set (reg:SI 3 r3) (reg:SI 315)))          ; REG_DEAD (reg 315)  <- se la queda
                (insn 646  (set (reg:SI 5 r5) (reg:SI 3 r3)))         ; sin nota            <- reescrita

Es `optimize_reg_copy_2` (`regmove.c`, gobernada por `flag_expensive_optimizations`,
no por `-fregmove`: con `-fno-regmove` **no cambia nada**, medido): ante
`(set <duro> <pseudo>)` en la que el pseudo **no muere**, sustituye el pseudo por
el registro duro en los usos posteriores del bloque. Consecuencias, las dos
fatales:

1. **`646` pasa a depender de `642`** (antes dependía de `627`), así que en
   `sched1` no está lista hasta el ciclo 25 en vez del 24;
2. **`646` pierde su `REG_DEAD`**, así que su `INSN_REG_WEIGHT` sube de 0 a +1 y
   cae por debajo de las seis `stfs` en el nivel 2.

Sin la reescritura, en `sched1` t=24 la lista de listos sería
`{579,582,585,609,616,623,642,646}` con `prio` 16 todas, pesos 0 salvo `642`
(+1), clase 3 todas y **`646` con 4 dependientes contra 2 de `579`** → **`646`
primero**. Y entonces `sched2`, con `LUID(646) < LUID(579)`, empata en los
niveles 1-4 y desempata por LUID a favor de `646`: **el orden del objetivo, fila
por fila, incluido `582` antes que `644` en el ciclo siguiente.**

**Control que lo prueba** (`t0n`, 4 s): con `-fno-expensive-optimizations` la
región sale **exactamente** la del objetivo:

    lwz 4,536(31) · addi 3,1,104 · mr 5,3 · stfs 30,88(1) · stfs 30,92(1) ·
    addi 4,4,72 · stfs 30,96(1) · stfs 30,104(1) · stfs 30,108(1) · stfs 30,112(1)

**Segundo control, del modelo LUID** (`u5`): mover el `bScale` DETRÁS del zurcido
de `look_offset` hace que la primera tienda emitida pase a ser `stfs 30,104(1)`
(`look_offset.x`), que es justo lo que predice el nivel 5. El modelo acierta las
dos veces.

### 1.4 Por qué la bandera no es la salida: medida sobre la unidad entera

    zCamera con  -fexpensive-optimizations   120941/145125 B   83,3357 %   615 fn al 100 %
    zCamera sin  -fexpensive-optimizations    78089/145125 B   53,8081 %   562 fn al 100 %

**−42.852 B y −53 funciones.** La bandera es correcta; el original se compiló con
ella y `optimize_reg_copy_2` también le corrió. (No he tocado `configure.py`: es
una medida, no una propuesta.)

### 1.5 Diez formas de fuente barridas, todas sobre la copia del scratchpad

Puntuadas contra la secuencia exacta del objetivo (4-5 s cada una). El requisito
es que la insn `%5 = <pseudo>` conserve su pseudo y su `REG_DEAD`.

    t0   base                                                       stfs antes de mr   (= nosotros)
    t0n  base + -fno-expensive-optimizations (control)              **OBJETIVO**
    t1   `bVector3 *plo=&look_offset;` y zurcido/llamada por plo    stfs antes de mr
    t2   tercer argumento con `(const bVector3 *)`                  stfs antes de mr
    t3   `const bMatrix4 *orient = …GetGeometryOrientation();`      stfs antes de mr
    t4   tercer argumento `&(const bVector3 &)look_offset`          stfs antes de mr
    t5   `bVector3 look_offset(0.0f,0.0f,0.0f);`                    stfs antes de mr
    u1   `bVector3 *d=&…; const bVector3 *s=&…;` y llamada con d,s  stfs antes de mr
    u2   `bFill(&look_offset,0,0,0)` en vez de tres asignaciones    stfs antes de mr
    u3   `bVector3 &lo = look_offset;` y `eMulVector(&lo,…,&lo)`    stfs antes de mr
    u4   tercer argumento `&look_offset + 0`                        stfs antes de mr
    u5   `bScale` detrás del zurcido (control del modelo)           cambia la PRIMERA tienda a 0x68

Las diez producen **las mismas 237 instrucciones**. CSE funde siempre los dos
`&look_offset` en **un solo pseudo**, y mientras sea uno solo `optimize_reg_copy_2`
dispara. Para que no dispare harían falta **dos pseudos distintos** que reload
asignara **al mismo `r3`** sin emitir un `addi` de más: incompatible por
construcción (dos definiciones vivas a la vez no pueden compartir registro, y si
no lo comparten sale `addi r5,r1,0x68` en vez de `mr r5,r3`).

### 1.6 `dwbody` fresco: no hay fuente que corregir

`regmap.py zCamera "TrackCopCameraMover::Update" --ours` + `dwbody.py`: **mismo
conjunto de locales, mismo árbol de bloques y mismo reparto (15/15)**. Las únicas
diferencias del cuerpo son las dos que ya anotó la r22 y **las dos tienen rango
cero**: dos `bTan(unsigned short)` delante de `operator/=` y tres accesores de más
(`GetGeometryPosition`, `GetVelocity`, `GetGeometryPosition`) entre `bDistBetween`
y `SetTargetDistance`. `TrackCar::Update` tiene **exactamente las mismas dos**.

## 2. `_Storage` (1.156 B): la pregunta de la r26, contestada — y su causa era otra

El arnés tapa `UTLVector.h` con una copia del scratchpad (`-I` **delante** de
`-I src`) y compila zCamera entera en **15 s**. Reproduce la base al dígito, así
que las cifras son comparables con las de la r26.

**Trampa que costó una compilación: `UTLVector.h` es CRLF.** Un parche anclado
con `\n` literal falla en silencio (`AssertionError`); hay que anclar con
`\r?\n` línea a línea, como dice `HERRAMIENTAS` §5.

    base                                                    97,83391 %  1160 B  16 diffs
    d4    outer if + Contains + make_empty() + reserve      96,41177 %  1176 B  22 diffs   (= r26, clavado)
    d9    SOLO el `if (minSize > capacity()) { reserve; }`  97,83391 %  1160 B  16 diffs   = BASE
    d7    d4 con la rama de `make_empty` VACIA              97,83391 %  1160 B  16 diffs   = BASE
    d12   d4 con SOLO la primera mitad de make_empty
          (`int num=size(); for(...) pop_back();`)          97,83391 %  1160 B  16 diffs   = BASE
    d13   d4 con SOLO la segunda mitad de make_empty
          (`if (mBegin) { FreeVectorSpace…; mBegin=0; … }`) 97,83391 %  1160 B  16 diffs   = BASE
    d17   d4 con las DOS mitades escritas a mano            96,41177 %  1176 B  22 diffs   = d4
    d11   d4 con `make_empty()` -> `mSize = 0;`             97,28027 %  1164 B  18 diffs
    d14   d4 con la primera mitad como `while (size()>0)`   88,92387 %  1260 B  54 diffs

**Lo que dice la tabla, y contradice la hipótesis de la r26:**

- **el `if (minSize > capacity())` exterior es GRATIS** (d9 = base). No cuesta un
  byte tener la comparación duplicada;
- **`Contains(srcBeg)` es GRATIS** (d7 = base). Con `mBegin` conocido 0 se pliega
  entero, como decía la r26;
- **`make_empty()` no es cara por su llamada virtual**: cada una de sus dos
  mitades, **por separado**, es byte-neutra (d12 y d13 = base) — y eso incluye la
  mitad que contiene el `FreeVectorSpace` virtual (d13). La sospecha de la r26
  («la llamada virtual `FreeVectorSpace` invalida la tabla de memoria de CSE»)
  **queda refutada por medida**;
- **el coste es la COEXISTENCIA de las dos mitades** (d17 = d4 exacto): con el
  bucle contado **y** el `if (mBegin)` detrás, el `if (num > capacity())` de
  dentro de `reserve` deja de plegarse y salen las 4 instrucciones de más;
- y el efecto no es un umbral de «cualquier escritura»: `mSize = 0;` a secas
  (d11) cuesta **+4 B**, que es **su propia instrucción** y nada más — CSE sigue
  plegando el `reserve` interior.

O sea: lo que rompe el plegado del `reserve` interior es **el número de bloques
que CSE tiene que atravesar** (bucle + rama + rama), no una invalidación de la
tabla de memoria. `d14` (el bucle como `while (size()>0)`, que no se pliega a
cero) lo confirma desde el otro lado: 1260 B.

**No he aplicado nada.** Aunque se encontrara la forma de `make_empty()` que
vuelve a plegar, `d4` seguiría dando **1160 B / 16 diffs = la base**: el racimo
(a) es muro con demostración (r26 §2) y `matched_code` es todo-o-nada, así que
**la ganancia sería cero bytes**. Y `UTLVector.h` la ven más de veinte unidades.

## 3. `TerrainVelocityNoise` (1.192 B): DWARF a cero, y la causa es el reparto

`dwbody.py` con volcado fresco: **ORIG 194 líneas, NUESTRO 195, cero diferencias
de estructura** (la única línea es un salto de línea final). **No queda fuente
que corregir en esta función.**

Las 14 filas del `fndiff` se explican con una sola frase: los dos lados emiten
los **mismos cinco `lis`** para las **mismas cinco direcciones del pool**, pero

    objetivo   lis r9 <-1D88   lis r8 <-1D8C   lis r10 <-1D94   lis r11 <-1D90   lis r9 <-1D70
    nuestro    lis r10<-1D88   lis r8 <-1D70   lis r7 <-1D8C    lis r11<-1D94    lis r9 <-1D90

    usos:      1D88 en 126 · 1D70 en 130 · 1D8C en 132 · 1D94 en 133 · 1D90 en 136

**Nuestro orden es exactamente el de «distancia al uso»** (126, 130, 132, 133,
136) — o sea el que da la prioridad del planificador con cinco `lis` vivos a la
vez. **El objetivo emite el de 1D70 el ÚLTIMO, pegado a su uso, porque reutiliza
`r9`**: el mismo registro que llevaba 1D88 y que muere en la fila 126. Con `r9`
reusado hay una **antidependencia** que impide subir ese `lis` por encima de la
126.

O sea: **el objetivo gasta CUATRO registros donde nosotros gastamos CINCO.** No
es colocación de `gcse`/PRE (que era lo que apuntaba la r26 y lo que mandaba
mirar con `cc1plus -dG`): es **presión de registros en `global_alloc`**, y la
colocación de los `lis` es su consecuencia. La permutación **f0 ↔ f12** de las
filas 116-128 es del mismo tipo (reparto), en el banco de coma flotante.

**No he gastado ensayos aquí**: con el DWARF a cero y el diagnóstico apuntando a
`global_alloc`, el siguiente paso es `alloc.py` sobre el `.greg`, no un barrido
de formas de fuente. Lo dejo servido con la medida hecha.

## 4. Lo que NO he tocado

- **static-init (3.604 B)**: cero ensayos. Las vedas de la r22/r25/r26 siguen en
  pie (dirección descendente descartada, +26..31 insns RTL muertas hacia arriba,
  árbol de inlines 303/303 con volcado fresco).
- **`LoadCameraShakes` (168 B)**: cero ensayos. La r26 la cerró con DWARF
  (`warned_overflow` en `r0`, ninguna sentencia detrás del `if (group)`).
- **`ICEMover::Update` (3.868 B)**: sólo he leído el diff. Sus 15 filas son
  **reparto de enteros** —el objetivo usa `r7,r8,r9,r10,r11` (cinco) donde
  nosotros usamos `r8,r9,r10,r11` (cuatro), y en la fila 596 el objetivo compara
  **el valor cargado** (`cmpwi r8`) mientras nosotros comparamos **la copia**
  (`cmpwi r11`)—. Es el mismo frente que `TerrainVelocityNoise` pero al revés
  (allí gastamos uno de más, aquí uno de menos). **No he sacado su `.greg`.**
- **`TrackCar::Update` (992 B)**: sólo el diff y el `dwbody`. Sus 15 filas son
  permutación de registros de coma flotante más orden de tres `stfs`
  (`0x98/0x90/0x94`). Mismo frente de reparto.
- **La vtable `_vt.Q33Sim9Collision9IListener`**: sigue siendo el único símbolo
  ausente (`symtabdiff`). Es `.rodata`; no lo he intentado.
- **Las dos correcciones de fidelidad con rango cero de `TrackCop`/`TrackCar`**
  (dos `bTan(fov)` y tres accesores): confirmadas otra vez con volcado fresco,
  **no escritas** — habría que inventar el código muerto que las produjo, y el
  brief prohíbe inventar.
- **El ensayo `c1` de la r21** (`RevToAng`/`FloatToAng`): **no aplicado**. El
  brief lo condicionaba a cerrar algo que ganara bytes; no he ganado ninguno.
- **`permuter.py`**: sigue sin usarse en zCamera.
- **El `.cse` literal de `d4`**: no lo he sacado. Lo he sustituido por siete
  medidas de aislamiento (§2), que contestan la pregunta con menos disco y de
  paso refutan la hipótesis que iba a comprobar.

## 5. Ensayos numerados, con su cifra

    -- TrackCop (region entre `bl bCross` y `bl eMulVector`, copia del scratchpad)
    t0, t1..t5, u1..u5   diez formas de fuente        LAS DIEZ: `stfs 30,88(1)` antes de `mr 5,3`
    t0n  control -fno-expensive-optimizations         **la secuencia del OBJETIVO**
    u5   control del modelo LUID (bScale detras)      la primera tienda pasa a 0x68  (predicho)

    -- unidad entera (arnes propio, .o al scratchpad)
    base  con -fexpensive-optimizations               120941/145125 B  83,3357 %  615 fn
    noexp sin -fexpensive-optimizations                78089/145125 B  53,8081 %  562 fn   NO APLICABLE

    -- _Storage (UTLVector.h tapada con -I, zCamera entera, 15 s)
    base                                              97,83391 %  1160 B  16 diffs
    d4    reproduccion de la r26                      96,41177 %  1176 B  22 diffs
    d7    d4 con la rama de make_empty vacia          97,83391 %  1160 B  16 diffs
    d9    solo el if exterior + reserve               97,83391 %  1160 B  16 diffs
    d11   make_empty -> `mSize = 0;`                  97,28027 %  1164 B  18 diffs
    d12   solo la 1a mitad de make_empty              97,83391 %  1160 B  16 diffs
    d13   solo la 2a mitad de make_empty              97,83391 %  1160 B  16 diffs
    d14   1a mitad como `while (size()>0)`            88,92387 %  1260 B  54 diffs
    d17   las dos mitades a mano (= d4)               96,41177 %  1176 B  22 diffs

**Nada aplicado. Nada revertido a medias. Cero cambios en el árbol.**

## 6. Vedas nuevas (con la sentencia barrida)

1. **`TrackCop`: la veda nº 4 de la r24 queda SUSTITUIDA.** No es cierto que «el
   desempate por LUID favorezca siempre a la primera `stfs` esté donde esté el
   bloque»: el LUID que ve `sched2` es el orden que dejó `sched1`. Lo que cierra
   la función es **`optimize_reg_copy_2` de `regmove`**, que convierte
   `(set (reg 5) (reg 315))` en `(set (reg 5) (reg 3))` y le quita el `REG_DEAD`;
   con eso `646` pierde un ciclo en `sched1` y sube su `INSN_REG_WEIGHT` de 0 a
   +1. Barridas **diez** formas de la sentencia `eMulVector(&look_offset, …,
   &look_offset)` y de la inicialización de `look_offset`: las diez idénticas.
   La única salida sería tener **dos pseudos** para `&look_offset`, y CSE los
   funde siempre en uno.
2. **`-fexpensive-optimizations` no se puede quitar de zCamera**: −42.852 B y
   −53 funciones al 100 % (83,3357 → 53,8081 %). Medido sobre la unidad entera.
3. **`-fno-regmove` NO desactiva `optimize_reg_copy_2`**: la gobierna
   `flag_expensive_optimizations`. Medido: con `-fno-regmove` el `.s` de
   `TrackCop` no cambia ni una fila.
4. **`_Storage`: el `if (minSize > capacity())` exterior y `Contains()` son
   GRATIS** (d9 y d7 = base al dígito). El coste de `d4` es **entero** de
   `make_empty()`, y **no** de su llamada virtual: cada una de sus dos mitades es
   byte-neutra por separado (d12, d13) y sólo juntas cuestan 16 B (d17 = d4).
5. **`TerrainVelocityNoise` no es colocación de `lis` por `gcse`/PRE**, que es lo
   que dejó apuntado la r26: nuestro orden de `lis` es exactamente el de
   distancia al uso, y la diferencia es que el objetivo **reutiliza `r9`** y usa
   cuatro registros donde nosotros usamos cinco. Es `global_alloc`.
6. **`dwbody` de `TerrainVelocityNoise` con volcado fresco: CERO diferencias.**
   No queda fuente que corregir en esa función.

## 7. Herramientas (scratchpad, prefijo `c27cam_`)

    c27cam_h.py      compila UN .cpp suelto con los cflags EXACTOS de zCamera
                     (4-5 s). Con `--rtl` preprocesa a un .ii propio y llama a
                     cc1plus A MANO: ahi si salen los volcados por pase (`ngccc`
                     los borra). Acepta `-da`, `-dS`, `-dR`, `-fsched-verbose-5`.
                     OJO: cc1plus no traga `-I`/`-D`/`-x c++`; hay que filtrar
                     la opcion Y SU ARGUMENTO (van separados en los cflags).
    c27cam_sw.py     barrido de variantes sobre una COPIA de TrackCop.cpp;
                     puntua la region bCross->eMulVector contra el objetivo y
                     avisa si cambia el resto de la funcion. El arbol no se toca.
    c27cam_unit.py   compila zCamera ENTERA a un .o del scratchpad quitando o
                     anadiendo banderas, y la mide con objdiff. 15 s.
    c27cam_stor.py   igual pero tapando `UTLVector.h` con una copia parcheada
                     (`-I` DELANTE de `-I src`); imprime el % y los diffs de
                     `_Storage`. 15 s. El arbol no se toca.
    c27cam_d4.py … c27cam_d17.py   los parches de la §2 (CRLF-safe).
    c27cam_upd.sched / .sched2     los volcados del planificador ya recortados
                                   a `TrackCopCameraMover::Update`.
    c27cam_dwcop.txt, c27cam_dw_*.txt   los dwbody leidos.

**Receta de volcado RTL que faltaba en `HERRAMIENTAS.md`**: `sched_verbose >= 5`
imprime, delante de cada bloque, la tabla `insn code bb dep prio cost blockage
units` **con la prioridad y la lista de dependientes de cada insn**, en **orden
de cadena** (que es el orden de LUID). Es lo que permite resolver un empate al
dígito en vez de suponerlo. Y `-fsched-verbose-5` **con guion**.

Disco: arranqué con 16 GB libres y termino con 14 GB (los otros agentes gastaron
la mayor parte). **Mis ficheros ocupan 480 kB**: he borrado los `.json` de
objdiff (17-22 MB cada uno), los `.o`, los `.ii`, los `.s` y los volcados RTL.

## 8. Verificación final

    build_direct.py zCamera                       ok
    triage.py zCamera --muro                      identico al brief (3 near-miss, 4 muros)
    measure.py zCamera                            113080/125008 B  90,4582 %  446 al 100 %
    audit.py Speed/Indep/SourceLists/zCamera      446/446 ok, CERO FALLA  (tres pasadas)
    md5 UTLVector.h                               d2d4c8e66789362ea032f7af80a6c70a  (= arranque)
    md5 TrackCop.cpp                              f7c2a733a30af8b7e83c25b92813a648  (= arranque)
    git status src/Speed/Indep/Src/Camera/        vacio
    git status src/Speed/Indep/Libs/              vacio

`frozen.py` no lo he tocado: no he cambiado nada.
