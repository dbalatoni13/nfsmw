# r30 — zWorld + zAI + zGameplay + zPhysicsBehaviors

Encargo: 11 near-miss, 16.164 B. **Cierres: 0. Aporte neto +0 B, +0 funciones.**
`git status src/` al cerrar: **ninguno de mis cinco ficheros aparece**
(`CarRender.cpp`, `CarLoader.cpp`, `AIPursuit.cpp`, `SAP.h`,
`SuspensionTraffic.cpp`). Todos los arneses restauran por md5 en el `finally`.

Lo que traigo, en orden de valor:

1. **Corrección al diagnóstico central de la r29.** Para `UpdateLoaded`,
   `-fno-gcse` da el **objeto byte a byte idéntico** (21 filas, 860 B): la r29
   atribuyó la diferencia a PRE con el volcado `-dG`, pero **PRE no decide nada
   ahí** — lo que insertó se deshace después. (§2.2)
2. **`Add__6RBGrid` / `UpdateAllAvoidables` (4.396 B): el diff está leído entero
   por primera vez.** No es «2 filas × 8 sitios»: son **2 diferencias distintas
   por sitio**, y la del `blt`→`b` es **`jump.c` invirtiendo el salto de cola del
   bucle contra un `do_cross_jump` que funde `cror;bso`**. Con el mecanismo
   nombrado, **23 banderas y 10 formas de bucle** lo dejan cerrado por
   construcción. (§3)
3. **`AssignClosestOffsets` (1.684 B): identificada la instrucción que falta y
   una forma de fuente que la produce.** `a1`/`a8`/`aH` (partir el
   `--x` del test) dan **1.684 B EXACTOS y matan el `mr.`** — pero desplazan el
   `subi` fuera de la cabecera del bucle. **16 formas medidas**; ninguna consigue
   las dos cosas. Es el hueco más estrecho que dejo. (§5)
4. **`SetMemoryPoolSize` (304 B): el desempate de `sched2` está al dígito**, con
   el volcado `-dR -fsched-verbose-5` que nombra las insns, el reloj y el término
   de `rank_for_schedule` que decide (**`depend_count`: 4 contra 3**). Y la
   cuarta dependencia del `stw` está identificada. **9 formas nuevas medidas, las
   tres mejores idénticas a la base.** (§4)
5. **`RenderFlaresOnCar` (2.908 B): la veda de la r29 (`r4`) iba en la dirección
   equivocada**, y la dirección buena se reduce a **UNA línea** (`f3` = sólo la
   2731): **2.908 B exactos**, y sigue sin aparecer el `lis` de la fila 123.
   **Noveno caso de «el tamaño exacto miente».** (§6)
6. **`UpdateLoaded`: el DWARF del original CASA ENTERO** (locales, registros,
   árbol de inlines, los dos bloques anónimos con `brake_spec`/`bt`). No queda
   fuente que corregir. (§2.1)

```
k1,k2,k4,k7,k8   formas for/while/continue del bucle de SAP.h
k9,kA,kB,kC,kD   while(true)+break, break en el test, node->mTail
f1..f7           unificacion de los simbolos del 0.0f/1.0f en RenderFlaresOnCar
m1,m2,m3,m5,m6,m7  orden de sentencias en SetMemoryPoolSize
m8,m9            sitio de NumSpongeAllocations = 0
a1,a3,a5,a7,a8,a9  formas del do-while de AssignClosestOffsets
aA,aE,aG,aH,aI,aJ,aK,aL,aM,aN  idem
23 banderas de diagnostico sobre tres simbolos
```

---

## 0. Verificación — reproduce EXACTO y cierra EXACTO

`build_direct.py zWorld zAI zGameplay zPhysicsBehaviors` → `5 ok, 0 fallidas`;
`triage.py --muro` da **las once líneas del encargo con su cifra**. Al cerrar,
tras un segundo `build_direct.py`, **las once líneas idénticas** (7 funciones /
11.556 B + muro de 4 / 5.212 B).

La columna de multiconjunto **volvió a bailar** entre las dos pasadas
(`blt-4, b+4` → `b+4, blt-4`; en `UpdateLoaded` `stmw+1, lwz-1, lmw+1, lis-1` →
`lis-1, lwz-1, lmw+1, stw-1`; en `AssignClosestOffsets` `cmpwi+1, mr.-1, mr+1` →
`mr.-1, mr+1, cmpwi+1`) con el objeto sin tocar. **Cuarta ronda consecutiva.**

### `audit.py` — FALLA: **VACÍA**, dos pasadas

| unidad | FALLA pasada 1 | FALLA pasada 2 |
|---|---|---|
| zWorld | 0 | 0 |
| zAI | 0 | 0 |
| zGameplay | 0 | 0 |
| zPhysicsBehaviors | 0 | 0 |

**Aviso de convivencia**: durante toda la ronda había otros agentes con
~95 ficheros modificados en `src/` (EAXSound, Frontend, Camera, Ecstasy,
Physics/Common…) **reconstruyendo zWorld y zPhysicsBehaviors en el `build/`
compartido** (`zWorld.o` cambió de fecha sin que yo lo tocara). Mis medidas de
zWorld y zPhysicsBehaviors van todas por `mn_repro` (objeto suelto al
scratchpad), así que **no las contamina**; las de zAI van por unidad entera
y **cada lote lleva su propio `base`** medido en la misma tanda.

---

## 1. Herramientas dejadas (prefijo `c30wag_`)

- **`c30wag_rows.py`** — `c29ba_rows.py` con el JSON por PID renombrado. Úsalo:
  el `c26wag_rows.py` que llaman `c28wag_pr.py` y `c29wag_batch.py` escribe a un
  nombre fijo y **revienta con dos agentes a la vez**.
- **`c30wag_pr.py`** / **`c30wag_batch.py`** — copias de `c28wag_pr.py` y
  `c29wag_batch.py` cableadas a `c30wag_rows.py`.
- **`c30wag_flags.py`** — barrido de banderas por símbolo (18 por defecto,
  o la lista que le pases). ~12 s por bandera.
- **`c30wag_ubatch.py`** / **`c30wag_ur.py`** — **lo que faltaba**: parchean,
  construyen **la unidad entera** con `build_direct.py`, miden y restauran por
  md5, y **reconstruyen la base al terminar**. 35 s por variante. Son la única
  vía para los `.cpp` que **no compilan sueltos** (§5.1).
- `c30wag_vk.py`, `c30wag_vk2.py`, `c30wag_vf.py`, `c30wag_vm.py`,
  `c30wag_vm2.py`, `c30wag_va*.py` — los lotes de variantes de cada sección.

**Trampa nueva y cara**: `mn_repro.py` quiere la ruta **relativa a `src/`**
(`Speed/Indep/Src/...`), pero `c28wag_rtlmin.py` quiere la ruta **con `src/`**.
Pasarle `src/...` a `mn_repro` da `No such file or directory` y el barrido entero
sale **`COMPILA MAL` en las 18 banderas** sin decir por qué.

---

## 2. `UpdateLoaded` (856 B) — el DWARF casa entero y **PRE no es el mecanismo**

### 2.1 El hermano al 100 % ya explotado, y el DWARF del original

Leído `SuspensionTrailer::Tire::UpdateLoaded` (100 %) contra
`SuspensionTraffic::Tire::UpdateLoaded` (97,21 %): son funciones
**estructuralmente distintas** (el Trailer declara los cuatro `*_spec` al
principio y no envuelve los frenos en `if`), así que **no hay idioma que copiar**.

Lo que sí cierra el frente es el **volcado DWARF del original**
(`symbols/mw_dwarfdump.nothpp:1748848`, filtrado por `low_pc` — `dwbody.py` no
la encuentra porque hay **cinco** `Tire::UpdateLoaded` y coge la primera):

```
void Tire::UpdateLoaded(float lat_vel /* f30 */, float fwd_vel /* f31 */, float load /* f3 */, float dT /* f4 */) {
    float slip_speed;      // f11
    float catchupfriction;            <- SIN registro, igual que el nuestro
    float skid_speed;      // f1
    ... Max(a,b) ...
    /* anonymous block */ { const float brake_spec; float bt; ... ApplyTorque ... }
    /* anonymous block */ { const float ebrake_spec; float ebt; ... ApplyTorque ... }
    ... Abs, Atan2a, Abs, Sqrt, GRIP_SCALE x2, Abs, Sina, Abs, Min
}
```

**Nuestro volcado es idéntico línea a línea**, incluidos los dos bloques
anónimos, el orden `brake_spec` antes de `bt` y `catchupfriction` sin registro.
La única diferencia es que a nuestro `bt`/`ebt` el DWARF le da `// f13` y al del
original no, sin ninguna instrucción de por medio.

> **VEDA: en `SuspensionTraffic::Tire::UpdateLoaded` no queda fuente que
> corregir.** Locales, orden, bloques y árbol de inlines casan con el original.

### 2.2 La corrección: `-fno-gcse` da el objeto IDÉNTICO

La r29 concluyó, con el `-dG`, que PRE inserta **dos** `high($LC264)` (1.0f) al
final de bb15 y bb19 donde el objetivo hace una en un bloque dominador. Reproduje
el volcado (fresco, insns 848 y 851, reg 360) y **es cierto que PRE hace eso**.
Pero:

| bandera | % | tamaño | filas |
|---|---|---|---|
| — control | 97,21028 | 860 | 21 |
| **`-fno-gcse`** | **97,21028** | **860** | **21 (objeto idéntico)** |
| `-fno-cse-follow-jumps` | 97,21028 | 860 | 21 (idéntico) |
| `-fno-expensive-optimizations` | 97,21028 | 860 | 21 (idéntico) |
| `-fno-force-addr` / `-fno-rerun-loop-opt` / `-fno-move-all-movables` / `-fno-thread-jumps` / `-fno-strength-reduce` / `-fno-caller-saves` / `-fno-peephole` / `-fno-function-cse` / `-fno-defer-pop` | 97,21028 | 860 | 21 (idénticos) |
| `-fno-force-mem` | 96,977 | 860 | 26 |
| `-fno-schedule-insns2` | 94,710 | 860 | 47 |
| `-fno-rerun-cse-after-loop` | 84,673 | **848** | 79 |
| `-fno-schedule-insns` | 75,813 | 856 | 118 |
| `-fno-cse-skip-blocks` | 85,164 | 892 | 84 |

**Apagar `gcse` entero no mueve un byte.** Las dos inserciones de PRE (reg 360,
dos `set` en bloques distintos) se **deshacen** después —los cinco usos que PRE
sustituyó vuelven a materializar su `high` local— y el resultado es el mismo que
sin PRE. Lo que fija la forma final es el par `cse2` + `local_alloc`
(`-fno-rerun-cse-after-loop` es la única bandera que cambia el **tamaño**).

> **VEDA (corrige la r29): en `UpdateLoaded`, `gcse`/PRE NO decide el número de
> `lis`.** Cualquier palanca dirigida a las inserciones de PRE está tirando el
> presupuesto: con `-fno-gcse` el objeto es el mismo.

### 2.3 Lo que queda, y es el mismo techo que §6

El objetivo tiene **UN pseudo `high(1.0f)` de vida larga en r30**
(callee-saved, vivo a través de `bl VU0_Atan2` y `bl VU0_sqrt`, filas 86→120) y
nosotros **dos `lis` locales** (filas 100 y 119). El marco (0x30 vs 0x28) y
`stmw r30` vs `stw r31` son **consecuencia**, no diferencias aparte.

Es exactamente la misma forma que `RenderFlaresOnCar` (§6): *al objetivo le vive
un pseudo más*. Ahí sí es `gcse` (§6.1); aquí **no**.

---

## 3. El `blt`→`b` (4.396 B) — el diff leído entero, y el pase nombrado

### 3.1 No son «2 filas del mismo tipo»: son DOS diferencias por sitio

`UpdateAllAvoidables` (2.908 B, 4 sitios) y `Add__6RBGrid` (1.488 B, 4 sitios),
las 8 apariciones iguales:

```
      fila  OBJETIVO              NUESTRO
>>>    92   lwz r9, 0x4(r9)       lwz r9, 0x4(r6)      <- base del load
>>>    98   b  .Lcond             blt .Lbody           <- salto de cola
```

### 3.2 El bucle del objetivo, entero (lmap de `UpdateAllAvoidables`)

```
8000EC54  mr r9, r0        SAP.h:89 SAP.h:91   head = this->mRoot
8000EC5C  lfs f0, 0x8(r9)  SAP.h:94            head->mPosition          (preencabezado)
8000EC60  li r6, 0x0       SAP.h:90            node = nullptr
8000EC6C  fcmpu cr0, f0, f13                   la condicion, 1a iteracion
.L_8000EC78:                                   <- .Lcond
8000EC78  cror un, eq, gt                      !(head->mPos < this->mPos)
8000EC7C  bso .L_8000ECA0                      -> salida
8000EC80  mr r6, r9        SAP.h:95            node = head
8000EC84  lwz r9, 0x4(r9)  SAP.h:97 SAP.h:99   head = head->mTail   (DOS notas)
8000EC88  cmpwi r9, 0x0
8000EC8C  beq .L_8000ECA0                      head != nullptr
8000EC90  lfs f13, 0x8(r9) SAP.h:94            la condicion, siguiente iteracion
8000EC94  lfs f0, 0x8(r3)
8000EC98  fcmpu cr0, f13, f0
8000EC9C  b .L_8000EC78    SAP.h:95            <- salto de cola INCONDICIONAL
```

**El objetivo tiene el mismo número de instrucciones que nosotros.** El
`cror;bso` está **compartido** entre el preencabezado y el fondo del bucle, y el
fondo llega con un `b`. Nosotros tenemos el `cror;bso` sólo para el
preencabezado y un `blt` en el fondo.

### 3.3 El mecanismo, nombrado

`expand_end_loop` mueve al fondo del bucle **la última** prueba de salida y la
invierte (por eso el test de `head != nullptr` sigue saltando a la salida —
`cmpwi;beq` — y sólo el de la posición se invierte). Sobre PowerPC, `>=` en
`CCFPmode` usa el bit 3 y sale como `cror un,eq,gt` + `bso` (dos insns); su
inversión, `<`, es `blt` (una).

- **Nosotros**: `jump.c` aplica «salto condicional por encima de un salto
  incondicional» → `bge .Lexit; b .Lbody; .Lexit:` se convierte en
  `blt .Lbody`. Neto −1 insn.
- **El objetivo**: esas dos insns se funden con las del preencabezado
  (`do_cross_jump`, cola idéntica de **una** insn de salto: el `fcmpu` no casa
  porque tiene los operandos en registros cruzados, f0/f13 contra f13/f0) y
  queda `b .Lcond`. Neto −1 insn **también**.

Las dos transformaciones ahorran lo mismo y **son excluyentes**; en `jump.c` la
inversión corre en el bucle principal y el cross-jump al final, así que la
inversión gana siempre. **Para llegar a la forma del objetivo hay que impedir la
inversión, y no hay bandera ni forma de fuente que lo haga.**

### 3.4 Las 23 banderas (dos símbolos) — ninguna la reproduce

`Add__6RBGrid`, control **99,30108 %, 1488 B, 8 filas**:

| bandera | % | tamaño | filas |
|---|---|---|---|
| `-fno-expensive-optimizations`, `-fno-force-mem`, `-fno-rerun-loop-opt`, `-fno-move-all-movables`, **`-fno-thread-jumps`**, `-fno-strength-reduce`, `-fno-caller-saves`, `-fno-peephole`, `-fno-function-cse`, `-fno-defer-pop`, `-fno-delayed-branch`, `-O2`, `-fno-inline-functions` | 99,301 | 1488 | **8 (idénticas)** |
| `-fno-schedule-insns2` | 96,505 | 1488 | 20 |
| `-fno-force-addr` | 91,887 | 1500 | 137 |
| `-fno-cse-skip-blocks` | 85,863 | 1560 | 137 |
| `-fno-rerun-cse-after-loop` | 85,581 | 1512 | 242 |
| `-fno-gcse` | 82,952 | 1468 | 220 |
| `-fno-cse-follow-jumps` | 81,352 | 1584 | 255 |
| `-fno-schedule-insns` | 89,793 | 1488 | 193 |
| **`-fno-fast-math`** | 98,172 | 1472 | **16** |

**`-fno-fast-math` es el diagnóstico interesante y sale al revés de lo esperado**:
convierte el `cror un,eq,gt; bso` del **preencabezado** en un `bge` suelto
(−16 B) y **deja el `blt` del fondo intacto**. O sea que la forma `cror;bso` del
objetivo **exige** `-ffast-math` (que es lo que tenemos) y el problema no es de
flags de la unidad.

### 3.5 Diez formas del bucle de `SAP.h` (`Node::Node`, líneas 26-29)

Control **99,30108 %, 1488 B, 8 filas**:

| # | sentencia barrida | % | tamaño | filas |
|---|---|---|---|---|
| k1 | `for (; cond; head = head->mTail) { node = head; }` | 99,301 | 1488 | **8 (idéntico)** |
| k2 | igual con `Node *head` declarada en el `for` | 99,301 | 1488 | **8 (idéntico)** |
| k4 | `while` con `continue;` al final del cuerpo | 99,301 | 1488 | **8 (idéntico)** |
| k7 | `for (...) node = head;` sin llaves | 99,301 | 1488 | **8 (idéntico)** |
| k8 | `for (; cond;)` con el cuerpo original | 99,301 | 1488 | **8 (idéntico)** |
| kC | `while (head != nullptr) { if (pos >= pos) break; node=head; head=head->mTail; }` | 99,301 | 1488 | **8 (idéntico)** |
| kD | `head = node->mTail;` en vez de `head->mTail` | 99,301 | 1488 | **8 (idéntico)** |
| k9 | `while (true)` + los dos `break` explícitos | 88,272 | **1440** | 131 |
| kA | `for (;;)` + `if (!(a < b)) break;` + `if (head == nullptr) break;` | 88,272 | 1440 | 131 |
| kB | `while (pos < pos)` + `if (head == nullptr) break;` | 88,272 | 1440 | 131 |

> **VEDA DE ÁRBOL: la forma sintáctica del bucle (`for` contra `while`, con o sin
> llaves, con `continue`, con la declaración en el `for`, con el test de posición
> escrito como `if(...) break` dentro de un `while (head != nullptr)`) NO es una
> palanca.** Siete formas distintas dan **objeto byte a byte idéntico**: GCC 2.95
> normaliza todas a la misma RTL. Con las `s1..s5` de la r28 y las `s6..sA` de la
> r29 son **15 formas medidas** en tres rondas.

> **Y `kD` cierra por su cuenta la lectura de la fila 92**: escribir
> `head = node->mTail` (leer del registro de `node`, que es lo que hacemos en el
> objeto) da el **objeto idéntico**. La elección de registro base del `lwz`
> **no viene de la fuente**; es consecuencia del salto de cola.

### 3.6 El mapa de líneas: la lectura de la r29 estaba equivocada

La r29 dijo «entre `node = head` (95) y `head = head->mTail` (99) el original
tiene TRES líneas que nosotros no tenemos» y «hay una nota, la 97, en la SALIDA
del bucle». **El `lmap` dice otra cosa**: las notas 97 y 99 están **las dos sobre
el mismo `lwz`** (8000EC84), no repartidas, y la salida `.L_8000ECA0` no lleva la
97. Dos notas en la misma dirección es la firma de **insns borradas por
cross-jump** (justo lo del §3.3), no de sentencias que falten.

> **Retiro la pista de la r29 §4.1**: no hay evidencia de un `break` condicional
> en el cuerpo. Las líneas 96-98 del original no emiten nada, y las notas 97/99
> juntas sobre el `lwz` son el rastro del cross-jump.

---

## 4. `SetMemoryPoolSize` (304 B) — el desempate de `sched2`, al dígito

### 4.1 El diff son DOS filas y es sólo el sitio de un `stw`

```
      fila  OBJETIVO                       NUESTRO
       59   lis r6, "Cars"@ha              lis r6, "Cars"@ha
>>>    60   (nada)                         stw r0, CarLoaderMemoryPoolNumber@l(r29)
       61   addi r6, r6, "Cars"@l          addi r6, r6, "Cars"@l
>>>    62   stw r0, CarLoaderMemoryPoolNumber@l(r29)   (nada)
       63   bl bInitMemoryPool             bl bInitMemoryPool
```

### 4.2 El volcado que lo cierra (`-dR -fsched-verbose-5`, bloque 8)

```
;;      insn  code    bb   dep  prio  cost  units        dependientes
;;       247   506     0     0    12     1  iu2      : 256
;;       256   507     0     5    11     1  iu2      : 282 270 258
;;       232   512     0     5    11     2  lsu      : 288 282 270 258
;;       250   512     0     2    11     1  iu2      : 288 282 270 266 258

;;   14   252 %4=[%31+0x60]        247 %6=high(`*$LC277')
;;   Ready list (t = 15):    256  232  250
;;       --> scheduling insn <<<250>>>     (mr r3,r0, luego borrada por no-op)
;;       --> scheduling insn <<<232>>>     (el stw)   <- se acaba el issue rate
;;   Ready list (t = 16):    256
;;       --> scheduling insn <<<256>>>     (addi r6,r6,low)
```

Las tres candidatas del ciclo 15 tienen **la misma prioridad (11)**, así que
manda `depend_count` (más dependientes primero): **250 → 5, 232 → 4, 256 → 3**.
Con `issue_rate = 2` entran 250 y 232, y el `addi` se queda para el 16. Ése es
**todo** el diff.

**Los cuatro dependientes del `stw` (insn 232)** son los tres `bl` (dependencia
de memoria: `bInitMemoryPool`, `bSetMemoryPoolDebugFill`,
`bSetMemoryPoolTopDirection`) **más la anti-dependencia WAR sobre r0** del
`li r0,0` final (`this->NumSpongeAllocations = 0;`). Los tres del `addi` (256)
son los tres `bl` (r6 es call-clobbered). **Le sobra exactamente una: la WAR.**

Y **la WAR también existe en el objetivo**: sus filas 70-71 son `li r0, 0x0` /
`stw r0, 0x68(r31)`, idénticas a las nuestras, y su fila 56 es el mismo
`mr r0, r3`. O sea que **el `depend_count` es el mismo a los dos lados** y el
desempate real tiene que estar en el término anterior de `rank_for_schedule`
(la clase respecto a `last_scheduled_insn`, que en el ciclo 14 es el propio
`lis` del que depende el `addi`) o en el LUID.

### 4.3 Nueve formas nuevas, ninguna mejora

Control **97,36842 %, 304 B, 2 filas**:

| # | sentencia barrida | % | tamaño | filas |
|---|---|---|---|---|
| m1 | local `int pool` + `CarLoaderMemoryPoolNumber = pool` | 97,368 | 304 | **2 (idéntico)** |
| m2 | `bInitMemoryPool(CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum(), …)` | 97,368 | 304 | **2 (idéntico)** |
| m7 | `int pool` + `bInitMemoryPool(CarLoaderMemoryPoolNumber = pool, …)` | 97,368 | 304 | **2 (idéntico)** |
| m3 | asignación del número de pool **antes** de `MemoryPoolSize = size` | 93,145 | 300 | 7 |
| m5 | `const char *pool_name = "Cars";` justo antes de la asignación | 93,158 | **308** | 7 |
| m6 | `const char *pool_name = "Cars";` al principio del bloque | 92,368 | **308** | 8 |
| m8 | `this->NumSpongeAllocations = 0;` **antes** de `bInitMemoryPool` | 90,579 | 300 | 9 |
| m9 | `this->NumSpongeAllocations = 0;` entre `bInitMemoryPool` y el `DebugFill` | 94,579 | 304 | 6 |

> **VEDA: sacar el literal `"Cars"` a una local para adelantar su `lis/addi`
> (m5, m6) EMPEORA en +4 B.** Era la palanca «obvia» contra el LUID y cuesta una
> instrucción de más.
> **VEDA: mover `this->NumSpongeAllocations = 0;` (m8, m9) para romper la WAR
> sobre r0 no la rompe** — el `li r0,0` sigue naciendo en r0.
> **VEDA: meter la asignación del número de pool dentro de la lista de argumentos
> (m2, m7) da objeto IDÉNTICO** — `expand_call` precalcula el argumento con
> efectos laterales y el orden de LUID no cambia.

**No he probado** la palanca del brief §1 (asm no volátil como referencia sin
emitir byte). Con el volcado ya en la mano, lo que haría el siguiente es
comprobar si un `__asm__("" : "=r"(x) : "0"(x))` sobre el valor almacenado añade
un **dependiente** al `addi` (no al `stw`), que es lo único que cambia el signo
del `depend_count`.

---

## 5. `AssignClosestOffsets` (1.684 B) — a UNA instrucción, y una forma que la crea

### 5.1 El `.cpp` NO compila suelto

`AIPursuit.cpp` necesita declaraciones que le llegan de los `.cpp` anteriores del
SourceList (`VehicleClass`, `INIS`, `IRBVehicle`…): `mn_repro` sobre él da
`parse error` en la línea 638 y **el arnés entero sale `COMPILA MAL`**. Por eso
`c30wag_ubatch.py`: construye la unidad entera (23 s) en vez del `.cpp`.

### 5.2 El diff: sobra un `mr.` y falta un `cmpwi`

```
      fila  OBJETIVO                       NUESTRO
>>>   260   lis r28, lbl_803CA86C@ha       lis r29, $LC1506@ha     (r28<->r29, consecuencia)
>>>   263   subi r29, r9, 0x1              subi r28, r9, 0x1       <- cabecera del cuerpo
      ...
>>>   388   mr r9, r29                     mr. r9, r28
>>>   389   cmpwi r29, 0x0                 (FALTA)
      390   bgt -> fila 263                bgt -> fila 263
```

`do { … } while (--copsToAssignOffsets > 0);` (línea 1018). El DWARF del original
confirma la local y su registro: **`int copsToAssignOffsets; // r9`**, y **no hay
ninguna local extra**. La resta está izada a la **cabecera del cuerpo** (fila 263,
destino del salto de cola) en los dos lados; lo único que cambia es que a
nosotros `combine` funde el `mr` con el `compare` en `mr.` (patrón
`(set cc (compare op1 0)) (set op0 op1)` de rs6000.md) y al objetivo no. Le
sobra el `compare` sobre el **temporal** (r29) en vez de sobre el **destino**.

### 5.3 Dieciséis formas medidas — y una que da el tamaño exacto

Control **99,66746 %, 1680 B, 8 filas** (el objetivo son 1684 B):

| # | sentencia barrida | % | tamaño | filas |
|---|---|---|---|---|
| **a1** | `copsToAssignOffsets = copsToAssignOffsets - 1;` + `while (… > 0)` | 99,268 | **1684** | **11** |
| **a8** | `--copsToAssignOffsets;` + `while (… > 0)` | 99,268 | **1684** | **11** |
| **aH** | `copsToAssignOffsets -= 1;` + `while (… > 0)` | 99,268 | **1684** | **11** |
| a9 | `while (--copsToAssignOffsets >= 1)` | 99,667 | 1680 | 8 (idéntico) |
| aI | `while ((copsToAssignOffsets = copsToAssignOffsets - 1) > 0)` | 99,667 | 1680 | 8 (idéntico) |
| aJ | `while ((copsToAssignOffsets -= 1) > 0)` | 99,667 | 1680 | 8 (idéntico) |
| aK | `while (0 < (copsToAssignOffsets = copsToAssignOffsets - 1))` | 99,667 | 1680 | 8 (idéntico) |
| aL | `while ((copsToAssignOffsets = … - 1) >= 1)` | 99,667 | 1680 | 8 (idéntico) |
| aN | `while ((--copsToAssignOffsets, copsToAssignOffsets) > 0)` | 99,667 | 1680 | 8 (idéntico) |
| a3 | `while (copsToAssignOffsets-- > 1)` | 99,059 | 1688 | 11 |
| aE | `while (copsToAssignOffsets-- > 0)` | 99,062 | 1688 | 11 |
| a5 | `for (int c = size(); c > 0; --c)` | 97,411 | 1684 | 28 |
| aM | local `remaining` + `while (remaining > 0)` | 97,784 | 1692 | 21 |
| aA | `--copsToAssignOffsets;` al **principio** del cuerpo | 97,553 | 1700 | 21 |
| aG | idem con `= … - 1` | 97,553 | 1700 | 21 |
| a7 | `int remaining` declarada **dentro** del `do` | — | — | NO COMPILA (fuera de ámbito en el `while`) |

**a1/a8/aH dan el tamaño del objetivo CLAVADO (1.684 B) y matan el `mr.`**: las
filas 388-390 pasan a casar. Lo que rompen es **dónde vive el `subi`**:

```
a1:   >>> 262  srawi r9, r9, 2        | srawi r29, r9, 2
      >>> 263  subi r29, r9, 0x1      | li r11, 0x0            <- el subi ya no esta aqui
      >>> 349  (nada)                 | subi r5, r29, 0x1      <- esta 86 filas mas abajo
      >>> 389  mr r9, r29             | mr r29, r5
```

Con `--x > 0` la resta es la actualización del BIV y GCC la iza a la cabecera del
cuerpo; en cuanto se parte en dos sentencias deja de serlo y se queda donde está.
**Ninguna de las 16 formas consigue las dos cosas a la vez.**

> **VEDA: `--x > 0`, `>= 1`, `(x = x-1) > 0`, `(x -= 1) > 0`, `0 < (x = x-1)` y
> `(--x, x) > 0` dan todas el MISMO objeto.** GCC 2.95 normaliza la expresión de
> decremento-y-test; no hay forma de fuente que compare el temporal en vez de la
> variable dentro de la condición.
> **VEDA: decrementar al principio del cuerpo (aA, aG) cuesta +20 B.**
> **VEDA: una local intermedia (aM) cuesta +12 B**, y además el DWARF del
> original dice que esa local no existe.

**Ahí es donde yo seguiría**: con `a1` aplicado el problema se reduce a
**recolocar un `subi` y una asignación de registro**, sobre un objeto del tamaño
exacto. Es el hueco más estrecho de todo el encargo.

---

## 6. `RenderFlaresOnCar` (2.908 B) — la r29 unificó en la dirección equivocada

### 6.1 El diff, y quién decide

18 filas, 2912 contra 2908 B. El objetivo tiene
`lis r16, lbl_8040AD04@ha` en la **fila 123** (línea 4061 del original, el bloque
de `CarTypeInfoArray`), en r16 callee-saved, y desde ahí sirve las cargas de las
filas 460, 600 y 601. Nosotros materializamos **dos**: `lis r9` en la 452 y
`lis r30` en la 461. Todo lo demás (`li r14/r16/r17/r30`, las dos filas de
`stw r9,0x1c(r1)` y `cmpwi cr3`) es el fichero de registros corrido una posición.

Aquí, al contrario que en §2.2, **`gcse` sí manda**: `-fno-gcse` da **224 filas,
93,242 %, 2940 B**. Ninguna de las 11 banderas probadas mejora
(`-fno-rerun-loop-opt` y `-fno-caller-saves` idénticas; `-fno-cse-follow-jumps`
29 filas; el resto entre 120 y 362 filas).

### 6.2 El 0.0f partido en dos símbolos: la dirección buena es UNA línea

Nuestro fuente declara `extern "C" const float lbl_8040AD04` (= 0,0f, verificado
leyendo `main.elf`: `0x8040AD04 = 00000000`, y `lbl_8040ACF0 = 3f800000 = 1,0f`)
y lo usa en **cinco** sitios del cuerpo (líneas 2731, 2800, 2803, 2810, 2812),
mientras los ~19 restantes son literales `0.0f` que van al pool `$LC630`.

La r29 probó **unificar hacia `lbl_8040AD04`** (r2/r4/r5/r6, 96,5-97,6 %). **Es
la dirección equivocada**: el objetivo tiene un pool de GCC, no un `extern`.
Unificando hacia el literal:

| # | sentencia barrida | % | tamaño | filas |
|---|---|---|---|---|
| — | control | 98,61761 | 2912 | 18 |
| f1 | los **cinco** `lbl_8040AD04` del cuerpo → `0.0f` | 98,054 | **2908** | 62 |
| **f3** | **sólo la línea 2731** (`float intensity = lbl_8040AD04;`) | **98,054** | **2908** | **62** |
| f6 | f1 + `sizescale = 1.0f` | 98,054 | 2908 | 62 |
| f2 | sólo las dos de `eRenderLightFlare` | 97,497 | 2928 | 28 |
| f4 | 2731 + 2800 + 2803 | 96,945 | 2936 | 72 |
| f5 | 2800 + 2803 | 96,878 | 2936 | 72 |
| f7 | sólo `lbl_8040ACF0` → `1.0f` | **98,618** | **2912** | **18 (idéntico)** |

**f1 = f3 = f6 al byte**: de las cinco, la única que mueve algo es la **2731**.
Y el resultado es **2.908 B exactos** — pero el `lis` de la fila 123 **sigue sin
aparecer**; lo que hace f3 es quitar el `lis r30` de la 461 y meter uno nuevo
(`lis r9, $LC555@ha`) en la 450. **Noveno caso medido de «el tamaño exacto
miente».**

> **VEDA: unificar el símbolo del 0,0f (en cualquiera de las dos direcciones) no
> crea el pseudo de vida larga.** Y **f7 prueba que el `extern` de `lbl_8040ACF0`
> es indistinguible del literal `1.0f`**: objeto idéntico. Esa declaración no
> aporta nada.

---

## 7. `CullParts` (836 B) — qué es exactamente el `mr` que falta

Cinco filas, `faltan 1: mr+1`. Leído con el fuente delante:

```
      95   lwz r0, 0x8(r30)     | lwz r11, 0x8(r30)
      96   li r9, 0x0             li r9, 0x0            <- `int debug_print = 0;` (linea 634 del original)
      97   cmpwi r0, 0x0        | cmpwi r11, 0x0
      99   lwz r0, 0xc(r30)       lwz r0, 0xc(r30)      <- plane_info->NumPlanes
     101   cmpw r9, r0          | cmpw r11, r0          <- la guarda `n < NumPlanes`
     102   mr r9, r0            | (FALTA)               <- debug_print = NumPlanes, IZADO
     103   bge …                  bge …
     144   cmpwi r0, 0x1        | cmpwi r11, 0x1
```

El `mr r9, r0` que falta es **`debug_print = plane_info->NumPlanes;` (línea 332)
izada al preencabezado por ser invariante del bucle**, y el objetivo la coloca
**delante del `bge` de la guarda**, o sea que la ejecuta incluso cuando el bucle
no corre. Nosotros no la izamos porque reutilizamos como `n` el registro r11
—que en esa rama ya se sabe cero por el `cmpwi r11,0; bne` de la fila 97— en vez
del cero de `debug_print` (r9), y entonces r9 ya vale lo que hace falta.

Es decir: **una coalescencia de copia que a nosotros nos sale y al objetivo no**,
sobre un valor cuya igualdad a cero sólo se conoce por dominancia. Confirmo lo de
la r29 y del encargo: **la barrera `asm("" : : : "r11")` del fuente hace falta**
y no la he tocado. Sin ensayos nuevos: las tres direcciones obvias ya están
vedadas en el comentario del fuente (inicializar `debug_print` a `NumPlanes`:
824 B; asignarlo también antes del bucle: 828 B; moverlo al principio del cuerpo:
836 B con 59 diffs).

---

## 8. Vedas nuevas, con la sentencia

- **k1/k2/k4/k7/k8/kC/kD** — siete formas sintácticas del bucle de `SAP.h`
  (`for` con y sin declaración, `continue`, sin llaves, `if(...)break` dentro de
  `while (head != nullptr)`, `node->mTail`): **objeto byte a byte IDÉNTICO**.
- **k9/kA/kB** — `while(true)`/`for(;;)` con los dos `break` explícitos:
  **88,272 %, 1440 B**, 131 filas.
- **`-fno-fast-math`** sobre `Add__6RBGrid`: convierte el `cror;bso` del
  preencabezado en `bge` (98,172 %, 1472 B). **El `cror;bso` del objetivo exige
  `-ffast-math`**: no es un problema de cflags de la unidad.
- **f1/f3/f6** — unificar el 0,0f de `RenderFlaresOnCar` hacia el literal:
  **98,054 %, 2908 B exactos**, 62 filas. **f3 sola (línea 2731) hace todo el
  efecto.**
- **f7** — `float sizescale = lbl_8040ACF0;` → `1.0f`: **objeto IDÉNTICO**.
- **m1/m2/m7** — local o asignación embebida en la llamada a `bInitMemoryPool`:
  **objeto IDÉNTICO**.
- **m5/m6** — sacar `"Cars"` a un `const char *`: **+4 B** (308).
- **m8/m9** — mover `this->NumSpongeAllocations = 0;`: 90,58 % / 94,58 %.
- **a1/a8/aH** — partir `--copsToAssignOffsets` del test: **1.684 B EXACTOS**,
  quitan el `mr.`, desplazan el `subi` 86 filas. **No es veda cerrada: es el
  hueco más estrecho que dejo.**
- **a9/aI/aJ/aK/aL/aN** — cualquier reescritura de `--x > 0` como expresión de
  asignación, `>= 1`, orden invertido o coma: **objeto IDÉNTICO**.
- **aA/aG** — decremento al principio del cuerpo del `do-while`: **1.700 B**.
- **aM** — local `remaining` intermedia: **1.692 B**; y el DWARF dice que esa
  local no existe.
- **`-fno-gcse` en `UpdateLoaded`: objeto IDÉNTICO.** Corrige el diagnóstico de
  la r29: PRE no decide el número de `lis` en esa función.
- **El DWARF de `SuspensionTraffic::Tire::UpdateLoaded` casa entero** (locales,
  registros, dos bloques anónimos, árbol de inlines). No queda fuente que tocar.

## 9. Lo que NO he probado

- **`SetMemoryPoolSize`**: la palanca del brief §1 (**asm no volátil como
  referencia sin emitir byte**). Con el volcado de `sched2` ya hecho, la pregunta
  exacta es si se le puede añadir **un dependiente al `addi` (insn 256)**, no
  quitárselo al `stw`. Tampoco he probado `permuter.py`.
- **`AssignClosestOffsets`**: con `a1` aplicado (1.684 B exactos), **no he
  barrido nada más**. Falta atacar el sitio del `subi` (formas que lo vuelvan a
  hacer actualización de BIV manteniendo el test sobre el temporal) y las tres
  `continue` del cuerpo, que son las que dan al bloque del test varios
  predecesores.
- **`RenderFlaresOnCar`**: con `f3` aplicado no he vuelto a barrer. Tampoco he
  combinado `f3` con las `e*` de la r26 ni con la `f1` de la r27. Y **no he
  mirado por qué el objetivo iza en la fila 123** (bloque de `GetCarTypeInfo`)
  con el `-dG` fresco: la única lectura coherente que le veo es
  `one_code_hoisting_pass` (mismo mensaje `PRE/HOIST`), que nadie ha mirado.
- **`UpdateLoaded`**: no he atacado `cse2`/`local_alloc`, que es donde el §2.2
  deja el mecanismo. Ni `alloc.py` sobre el `.greg`.
- **El `blt`→`b` de `SAP.h`**: no he encontrado forma de impedir la inversión de
  `jump.c`, y no he probado **nada que meta una etiqueta o una insn activa entre
  el salto condicional del fondo y el `b` de vuelta**, que es la única condición
  de `jump_optimize` («salto condicional por encima de un salto incondicional»)
  que queda por romper.
- **`CullParts`**: cero ensayos nuevos (las tres direcciones ya vedadas en el
  fuente).
- **`GTriggerRCUi`, `GenerateIndex`, `DefragmentPool`, `UpdateWheelYRenderOffset`**:
  cero ensayos (negativos firmes del encargo).
