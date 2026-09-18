# r54 · `loaded` — `SuspensionTraffic::Tire::UpdateLoaded` (856 B)

**Resultado: CERO bytes, y el mecanismo entero, nombrado pase a pase y con las dos
desigualdades numéricas que lo deciden.** `zPhysicsBehaviors` sigue con **1 de 1120**
funciones con el código distinto (`fncmp` antes y después: idéntico).

No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`. No hay `asm`
puesto en ningún sitio. El único cambio en el árbol es **un comentario** en
`src/Speed/Indep/Src/Physics/Behaviors/SuspensionTraffic.cpp` que corregía un diagnóstico
equivocado de la r36d; verificado que el `.o` sale **byte a byte idéntico**
(`sha1 1802e4d0…`, 578.588 B antes y después).

---

## 0. El encargo venía con una premisa muerta

El brief pedía montar «la anticipabilidad de PRE, descrita en la r50 y nunca montada».
**Esa premisa ya estaba refutada en la r30** (`-fno-gcse` da el objeto IDÉNTICO) y ahora
sé *por qué* el `-fno-gcse` no mueve nada, que es distinto de que PRE no actúe:

> **PRE SÍ actúa** —hace las cinco sustituciones, lo dice su propio registro— y **cse2 las
> deshace SIEMPRE**, por una desigualdad de coste que no depende de la fuente. Apagar PRE y
> dejar que cse2 lo deshaga dan, byte a byte, el mismo objeto.

Montar el volcado de anticipabilidad habría sido media ronda tirada. Lo dejo dicho para que
no se vuelva a asignar.

---

## 1. Las 21 filas son UNA causa, y la aritmética cierra

| | objetivo | nuestro |
|---|---:|---:|
| tamaño | 856 B (214 insns) | 860 B (215 insns) |
| `lis` de `1.0f` (`lbl_803FB6B8` = `$LC917`) | **3** | **4** |

```
objetivo   fila  86  lis r30, lbl_803FB6B8@ha      <- IZADA, delante de bl VU0_Atan2
           fila 101  lfs f0, lbl_803FB6B8@l(r30)   <- uso A
           fila 120  lfs f0, lbl_803FB6B8@l(r30)   <- uso B  (tras bl VU0_sqrt)
           fila 148  lis r8,  ...@ha  / 152 lfs @l(r8)    <- uso C, rematerializado
           fila 199  lis r9,  ...@ha  / 201 lfs @l(r9)    <- uso D, rematerializado

nuestro    fila 100  lis r9 / 101 lfs @l(r9)       <- uso A
           fila 119  lis r9 / 120 lfs @l(r9)       <- uso B
           fila 148, 199                            <- C y D, iguales al objetivo
```

Todo lo demás —`stmw r30` contra `stw r31`, el marco `0x30` contra `0x28`, `lmw` contra
`lwz`, y los doce `ARG_MISMATCH` de desplazamiento— es **consecuencia** de que al objetivo
le vive un GPR preservado más. **Un solo `high` sirviendo a los usos A y B: eso son los 4 B.**
Ni «que no se hunda el pseudo» basta: si el pseudo de PRE sobrevive pero sólo sirve a A,
seguimos en 4 `lis` y 860 B (bajan las filas a ~13, no el tamaño).

---

## 2. La cadena que SÍ produce `lis rPRESERVADO, <pool>@ha`

No es una teoría: está **medida en tres funciones que casan al 100 %** y leída de los
registros que el propio compilador escribe (`-dG`).

### El control positivo: `AddRoadNoise__13CarRenderConn…` (zWorld, casa)

Su asm tiene exactamente la forma del objetivo —un `lis r25, lbl_8040AF10@ha` sin uso
cercano, dos `lfs …@l(r25)` en bloques distintos y tras llamadas, y **otras cuatro**
apariciones del mismo literal rematerializadas—. El log de gcse de *nuestra* compilación:

```
PRE: redundant insn 227 (expression 5) in bb 10, reaching reg is 253
PRE: redundant insn 373 (expression 5) in bb 17, reaching reg is 253
PRE: redundant insn 410 (expression 5) in bb 21, reaching reg is 253
PRE: redundant insn 556 (expression 5) in bb 28, reaching reg is 253
COPY-PROP: Replacing reg 151 in insn 329 with reg 253      <- ESTO es lo que decide
COPY-PROP: Replacing reg 197 in insn 512 with reg 253
```

### Los cuatro pasos

1. **cse1** le da al pseudo del `high` de un sitio un **SEGUNDO uso `lo_sum` en OTRO bloque
   básico**. En `AddRoadNoise`: `reg 151` se pone en la insn 227 y se usa en la 230 (su
   bloque) **y en la 329** (un bloque posterior).
2. **gcse-PRE** iza `(high (*$LC))` a un bloque dominador (`reaching_reg`) y convierte la
   insn del `high` en una **copia** `(set (reg 151) (reg 253))`.
3. El **`one_cprop_pass` siguiente** mete `reg 253` **en el uso lejano** (insn 329). Ése uso
   ya no es una copia con nota, es un `(mem (lo_sum (reg 253) sym))`: **cse2 no lo puede
   deshacer**.
4. `REG_N_REFS(253) = 3` ⇒ `update_equiv_regs` **no lo hunde** ⇒ el `lis` se queda donde lo
   puso PRE, cruzando las llamadas ⇒ `global_alloc` le da un **preservado**.

El paso 4 lo cierra `cse2` regalando el *primer* uso: su `canon_reg` sustituye el pseudo
local por el de PRE porque `make_regs_eqv` (cse.c:1005) elige como canónico **al que tiene
el último uso más tarde** — y el de PRE tiene cinco usos por detrás. **Eso ya nos pasa a
nosotros** (insn 468 usa `reg 362` en el volcado `.cse2`). Lo que nos falta es sólo el
paso 3.

Otros dos controles con la misma forma y al 100 %: `UpdateForces__19SteeringWheelDevice…`
(zMain: `lis r24` izado + un `lis r9` rematerializado con su uso local en el mismo bloque de
entrada, y tres usos lejanos en r24) y `GetDifferentialAngularVelocity__C12EngineSpline`
(zPhysicsBehaviors, reproducida al 100 % en una mini-TU de 2,3 s: ahí el reparto lo hace
cse1 solo, sin PRE).

---

## 3. Las dos desigualdades que nos bloquean

### 3.1 cse2 rehace SIEMPRE la copia de PRE: `0 < 1`

En `cse_insn` (cse.c ~7100), «find the cheapest valid equivalent»:

* `src` = el pseudo de PRE ⇒ `COST` = **1** (cse.c:519: un pseudo que no es `REG_USERVAR_P`
  y hard vale 1).
* `src_eqv` = el `(high (symbol_ref))` de la nota `REG_EQUAL` que PRE deja intacta ⇒
  `notreg_cost` → `rtx_cost` → **rs6000.h, `CONST_COSTS`, `case HIGH: return 0`**.

`src_eqv_cost (0) < src_cost (1)` ⇒ el `high` gana **en todos los bloques, para toda
fuente**. Nuestras cinco copias vuelven a ser cinco `elf_high`. Verificado: `.gcse` trae
`(set (reg 238) (reg 362))`, `.cse2` trae `(set (reg 238) (high …))`.

> **VEDA NUEVA, y vale para todo el proyecto:** *ningún* near-miss del tipo «al objetivo le
> vive un `lis` de pool más» se ataca por cse2. La copia de PRE está condenada por una tabla
> de costes de la máquina. Lo único que se puede pelear es el paso 3 (COPY-PROP).

### 3.2 COPY-PROP es SÓLO global: el `high` y su `lfs` están en el mismo bloque

`gcse.c`, `cprop()`: recorre bloque a bloque con `reset_opr_set_tables()` al entrar y
`mark_oprs_set(insn)` al salir de cada insn; `cprop_insn` descarta el reemplazo si
`! oprs_not_set_p (reg, insn)` —«*If the register has already been set in this block,
there's nothing we can do*»— y `find_avail_set` sólo mira `cprop_avin[BLOCK_NUM (insn)]`,
lo disponible **a la entrada** del bloque. El propio comentario de `pre_gcse` lo reconoce:
*«one could add a pass here to propagate the new register through the block»* — 2.95 no lo
tiene.

En `UpdateLoaded` los cinco sitios del `1.0f` tienen su `elf_high` **en el mismo bloque
básico que su `lfs`**:

```
PRE: redundant insn 528 (expression 40) in bb 23, reaching reg is 362   <- uso B, lfs en 531, bb 23
PRE: redundant insn 694 ... bb 26      PRE: redundant insn 751 ... bb 31
PRE: redundant insn 860 ... bb 36      PRE: redundant insn 920 ... bb 39
(ni una sola linea COPY-PROP con reg 362)
```

### 3.3 Y cse1 no puede darle el segundo uso: la etiqueta de unión

Para que el `high` del uso B se ponga en un bloque anterior, cse1 tendría que compartirlo
con una aparición previa. En el volcado `.cse` de la base, **los seis `high` del `1.0f`
tienen UN solo uso cada uno** (regs 218@465, 238@528, 291@694, 306@751, 338@860, 351@920):
cse1 no comparte nada en esta función.

La razón es exacta: `cse_end_of_basic_block` (cse.c:8508) corta el bloque extendido en
**la primera `CODE_LABEL`**, y sólo puede seguir un salto condicional cuyo
`LABEL_NUSES (JUMP_LABEL (p)) == 1`. En `UpdateLoaded` sólo hay **dos etiquetas con
`NUSES == 2`**, y una de ellas —la 480— es justo la unión del `if/else` de `mSlip`, **entre
el uso A y el uso B**:

```
(code_label 480 …  [num uses: 2])     <- los dos `bso` del `&&`
(code_label 609 …  [num uses: 2])
   (todas las demas: [num uses: 1])
```

**Y el objetivo tiene esos dos mismos `bso`** (filas 95 y 104, ambos a `0x19a50`): su
`LABEL_NUSES` también era 2. Con la estructura de bloques que el propio binario original
exhibe, cse1 tampoco podía compartir A y B allí.

### 3.4 Y cuando sí se puede compartir, se comparte el VALOR y desaparece la dirección

Es la trampa que mata las formas «obvias». Si metes un uso de `1.0f` antes, en el mismo
bloque extendido, cse1 comparte **el `lfs` entero**, no el `@ha`: el sitio B se queda sin
`lo_sum` y no hay nada que propagar. Medido tres veces (§4, controles `t1`, `t2`, `t3`).

En `AddRoadNoise` no pasa porque **los dos `mem` son distintos**:

```
insn 230  (set (reg/v:SF 149) (mem/u:SF (lo_sum (reg 151) …)))   <- /u  (RTX_UNCHANGING)
insn 329  (set (reg:SF 183)   (mem:SF   (lo_sum (reg 151) …)))   <- SIN /u, e insn/i
```

El segundo viene de un cuerpo **inlineado** (`insn/i`) y ha perdido el `RTX_UNCHANGING_P`,
así que hashea distinto: cse1 comparte la **dirección** (por `canon_reg`) y **no** el valor.
Los dos usos de `UpdateLoaded` son los dos `mem/u` y los dos de código propio.

---

## 4. Lo medido (24 formas en esta ronda, ninguna mueve `lis30`)

Arnés: mini-TU (prefijo de `zPhysicsBehaviors.cpp` + `SuspensionTraffic.cpp`), **2,9 s por
prueba** contra 22 s de la SourceList; reproduce la función al dígito
(97,21028 %, 860 B, 21 filas, 16 `lis`).

| forma | % | B | filas | `lis30` |
|---|---:|---:|---:|:--:|
| base | 97,21028 | 860 | 21 | no |
| `1.0f < Abs(fwd_vel)` primero en el `&&` | 92,911 | 860 | 38 | no |
| `skid_speed > 1.0f` primero en el `&&` | 96,182 | 860 | 26 | no |
| `1.0f < skid_speed` (canonicalizado) | **97,21028** | 860 | 21 | no *(idéntica)* |
| `mSlip = 0.0f` antes, sin `else` | 94,220 | 848 | 52 | no |
| ifs anidados | 94,220 | 864 | 52 | no |
| `skid_speed` calculado antes del `if` de `mSlip` | 79,173 | 868 | 75 | no |
| `Abs(skid_speed) > 1.0f` | 93,916 | 888 | 29 | no |
| `const float one = 1.0f` en los 2 sitios | **97,21028** | 860 | 21 | no *(idéntica)* |
| `const float one = 1.0f` en los **5** sitios | **97,21028** | 860 | 21 | no *(idéntica)* |
| `if (0.5f) { if (1.0f) …else… } else …` | **97,21028** | 860 | 21 | no *(idéntica)* |
| `skid_speed > 1.0f` en el `if` de `mLateralForce` | **97,21028** | 860 | 21 | no *(idéntica)* |
| `Abs(fwd_vel) > 1.0f` | **97,21028** | 860 | 21 | no *(idéntica)* |
| `\|\|` invertido (`>=` … `else` con `slip_speed`) | 94,757 | **856** | 30 | no |
| `!(…) \|\| !(…)` | 94,757 | **856** | 30 | no |
| ternario dentro del `if` externo | 93,864 | 864 | 54 | no |
| `const bool fast = 1.0f < Abs(fwd_vel)` reusado | 84,360 | 844 | 63 | no |
| `float skid_speed = …` (declaración en el uso) | **97,21028** | 860 | 21 | no *(idéntica)* |
| `float slip_speed = …` (declaración en el uso) | **97,21028** | 860 | 21 | no *(idéntica)* |
| `const float abs_fwd = Abs(fwd_vel)` reusado | 93,850 | 840 | 46 | no |

**Controles (tenían que cambiar, y cambiaron — en la dirección que predice §3.4):**

| control | qué mete | resultado |
|---|---|---|
| `c2` | `if (skid_speed > 1.0f) mSlipping = true;` antes del `if` de 0.5f | 876 B; el `high` sigue en bb22-23 con su `lfs`, **cero COPY-PROP** |
| `t1` | `mLongitudeForce = 1.0f;` recto, en el bloque del test de 0.5f | 864 B; el `high` **se mueve a bb22**… y el `lfs` con él (valor compartido): cero COPY-PROP |
| `t2` | lo mismo **antes** de `bl VU0_sqrt` | 864 B; `.cse` confirma que el sitio B **pierde su `high`**: el valor se comparte a través de la llamada |
| `t3` | `UMath::Min(skid_speed, 1.0f)` (inline con `fsel`) en bb22 | 880 B; igual: valor compartido, cero COPY-PROP |

Los controles `t1`/`t2`/`t3` son la prueba de §3.4: **dentro de un bloque extendido de cse1
se comparte el valor, no la dirección**; fuera de él no se comparte nada. No hay hueco
intermedio en esta función.

---

## 5. Vedas: cuál cae, cuál se refina, cuál nace

* **Se refina (r36c/r36d, comentario de la fuente):** *«la divergencia está dentro de cse2 …
  frente abierto para quien pueda tocar cse2 desde la fuente»*. **cse2 no es el frente**: su
  comportamiento está fijado por `COST(HIGH)=0 < COST(pseudo)=1`. El frente, si lo hay, es
  cse1 (§3.3) y COPY-PROP (§3.2). Comentario corregido en la fuente (objeto idéntico).
* **Se confirma (r30):** *«en `UpdateLoaded` no queda fuente que corregir»* — locales,
  bloques anónimos y árbol de inlines casan con el DWARF del original. Las 24 formas de esta
  ronda no la rompen: **las siete que dan el objeto IDÉNTICO** (incluida `const float one`
  en los cinco sitios) dicen que el frontend canonicaliza todo lo que se puede escribir ahí.
* **Nace (dura, con la desigualdad):** *cse2 rehace siempre la copia de PRE de un
  `high(symbol_ref)`; `0 < 1`, en toda función y para toda fuente.*
* **Nace (palanca positiva, reutilizable):** *para que un literal de pool viva en un GPR
  preservado hacen falta dos usos `lo_sum` en **bloques básicos distintos** pero en el
  **mismo bloque extendido de cse1**, y que el valor NO se comparta (en la práctica: uno de
  los dos `mem` viene de un cuerpo inlineado y ha perdido `/u`).*

---

## 6. Para las otras asignaciones de esta ronda (esto sí es accionable)

Censo sobre el original (`build/GOWE69/asm/Speed/Indep/SourceLists/*.s`): **199** casos de
`lis rPRESERVADO, lbl_*@ha` con ≥2 usos; **41** con una etiqueta de unión entre los usos;
sólo **4 de esos 41 son funciones sin bucle**. Cruzando con las 28 funciones abiertas:

| función | agente | literal | reg | usos | uniones entre los usos |
|---|---|---|---|---:|---:|
| `UpdateLoaded` | `loaded` | `lbl_803FB6B8` | r30 | 2 | **1** |
| `RenderFlaresOnCar` | `flares` | `lbl_8040AD04` | r16 | 3 | 3 |
| `UpdateWheelYRenderOffset` | `flares` | `lbl_8040AA84` | r19 | 2 | 4 |
| `ActualReadJoystickData` | — (zPlatform) | `lbl_80403108` | r14 | 2 | **0** |

* **`flares`**: la r29 dijo que `UpdateLoaded` y `RenderFlaresOnCar` eran «la MISMA familia».
  **Lo son, y ahora está el nombre de los cuatro pasos.** Que mire si sus usos comparten
  bloque con su `high` (`-dG` y buscar `COPY-PROP … with reg N`): es un grep, no una ronda.
* **`ActualReadJoystickData` (1.588 B, 99,32 %, zPlatform, sin agente esta ronda)** es el
  único de los cuatro con **CERO uniones entre los dos usos**: es el que tiene el paso 1
  al alcance de cse1. Si el frente del `lis` preservado se va a atacar en algún sitio, es ése.

---

## 7. Herramientas (nada nuevo en `scripts/`)

Todo con lo que ya había: `scripts/fncmp.py`, `scripts/fndiff.py`, `scripts/rtldump.py`
(`-ds -dG -dt -dl` = `.cse`, `.gcse`, `.cse2`, `.lreg`) y `scripts/build_direct.py`. El
arnés de mini-TU y los extractores de volcado se quedan en el scratchpad de la sesión; el
patrón está copiado de `scripts/agent_phb_harness.py`, que ya existía.

**Aviso de método que me costó dos horas**: `ngccc` produce `lis 30,$LC…`, **sin la `r`**.
Un `grep 'lis r30'` sobre nuestro asm devuelve cero y hace creer que el compilador no emite
nunca un `lis` a preservado. Sí lo emite.
