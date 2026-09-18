# Ronda 19 — grupo `main` (zMain, zGameplay, zPhysics, zAI, zPhysicsBehaviors, zFe2, zFeOverlay, zFe, zDynamics)

Base `base_r19_main.json`: **1.524.320/1.551.476 B, 98,2500%**.
Final `despues_r19_main.json`: **1.524.688/1.551.476 B, 98,2734%** (+368 B, +1 fn,
1 unidad cambia: zFe2 246.820 → 247.188).

Herramienta usada en todo el informe: **banco de un solo `.cpp`** con los cflags
exactos de la unidad y `objdiff-cli` contra el `.o` del troceador. Coste medido
por variante: `WideCharHistogram.cpp` 0,9 s · `SuspensionTraffic.cpp` 3 s ·
`AIPursuit.cpp` 13 s · `Smackable.cpp` 15 s · `CarCustomize.cpp` 12 s.
**Los cinco bancos reproducen el porcentaje de la unidad al quinto decimal.**

Dos avisos nuevos para quien reuse el banco:

- **`objdiff-cli diff` con un símbolo y sin `-o` abre la TUI y se queda colgado.**
  Hay que dar `-o <fichero>` (o `-o -`). Me costó una tanda de 120 s en vano.
- **zFeOverlay renombra `.text` → `.over`** (regla `prodg_rename`). Sin pasar
  `tools/rename_section.py -q <banco.o> .text=.over`, objdiff **no empareja los
  símbolos** y devuelve `match_percent: null` sin dar ningún error: parece que
  la función está perfecta cuando no se ha comparado nada.

---

## 1. `DoHumanSteering__15SuspensionRacer` (zPhysicsBehaviors) — CERRADA, y era un fallo de comportamiento

El fallo de auditoría que traía el encargo. **Resuelto: el índice del original es
`SteerInputRemapping - 1`, no `SteerInputRemapping`.**

La prueba, en tres piezas:

1. `symbols.txt` **no miente**: el DWARF trae
   `struct Table SteerInputRemapTables[4]; // size: 0x50, address: 0x80489F78`,
   así que `sizeof(Table) == 0x14` y el array empieza donde dice.
   La lectura «(b) symbols.txt sitúa mal el inicio» queda **descartada**.
2. Con `-fforce-addr` GCC **no funde** el desplazamiento en el símbolo: emite
   `lis/la` de la base y un `addi` aparte. Nuestra base era
   `SteerInputRemapTables` (+0x14 → `[1]`); la del objetivo es
   `SteerInputRemapTables-0x14`, que dtk nombra
   `PostCollisionSteerReductionData+0x18` porque **0x80489F64 cae dentro de ese
   símbolo** — es un nombre del desensamblador, no una referencia real. +0x14 → `[0]`.
3. `&a[i-1]` se pliega como `(a-20) + i*20`: la base sesgada es exactamente la
   firma de un `- 1` en el subíndice.

**c1** `SteerInputRemapTables[steer_remapping - 1]` → `lis 3,SteerInputRemapTables-20@ha`
/ `la` / `addi 3,3,20`, **125 instrucciones, byte a byte el objetivo**. Aplicado.

Corolario de coherencia que lo respalda: `SteerInputRemappingDrift = 4` **se sale
del array** con índices 0-based (4 elementos) y encaja con 1-based; y el comentario
del fuente («only the first steering remap table is actually used») pasa a ser
cierto: `STEER_REMAP_MEDIUM = 1` → tabla `JoystickInputToSteerRemap1`.
Antes usábamos `JoystickInputToSteerRemap2`: **el juego remapeaba el volante con
la curva equivocada.**

### Y `audit.py` tenía un punto ciego más

Con el arreglo aplicado, `audit.py` **seguía dando FALLA**:
`reubicacion a otro simbolo: PostCollisionSteerReductionData contra SteerInputRemapTables`.
Es un **falso positivo**: la variable `mismo_destino` (dirección final = símbolo +
addend) ya estaba calculada unas líneas antes, pero sólo se usaba para el fallo de
`tipo/addend`; la rama `nb != na` no la consultaba. Arreglado en `scripts/audit.py`
(guarda `if nb != na and mismo_destino: relocs += 1; continue`).

**Comprobado que no enmascara nada**: con una copia de `audit.py` sin el parche,
las nueve unidades dan `zFe2: 1` (el `__builtin_new`, real, de otro agente) y
`zPhysicsBehaviors: 1` (este falso positivo); con el parche, `zFe2: 1` y
`zPhysicsBehaviors: 0`. El `__builtin_new` no se toca porque los dos símbolos
resuelven a direcciones **distintas**.

### Estado de auditoría de las nueve unidades

    zMain 0 · zGameplay 0 · zPhysics 0 · zAI 0 · zPhysicsBehaviors 0
    zFe2 1 (ShowHelpBlurb: __builtin_new contra __builtin_vec_new — de otro agente)
    zFeOverlay 0 · zFe 0 · zDynamics 0

Los **6 fallos de zMain y el de zFeOverlay** que traía el encargo **ya no
aparecen** (segunda pasada, con el árbol reconstruido).

---

## 2. `PackString__17WideCharHistogram` (zFe2) — CERRADA, +368 B

Acotación previa correcta: sobraba `extsb` y faltaba el par `add`+`stb`
(el objetivo materializa la dirección; nosotros usábamos `stbx`).

**Ensayos sobre la expresión sola (c1-c9): todos BAJAN**, y explican por qué la
asociación parecía acoplada:

| | forma | tam | pct |
|---|---|---|---|
| base | `(char)h - (char)e * 0x80 - 0x80` | 368 | 96,467 |
| c1 | sin el cast de `entry` | 364 | 92,967 |
| c2 | sin ningún cast | 364 | 92,967 |
| c3 | cast al resultado | 364 | 92,967 |
| c4 | `h - (e*0x80 + 0x80)` | 364 | 93,250 |
| c8 | `h - 0x80 - e*0x80` | 364 | 95,489 |
| **c9** | `h - (e + 1) * 0x80` | 364 | **95,815** |

**c9 es el que enseña la lección**: pone `entry` en r11 y el temporal en r0, y
con eso **las seis filas del bucle interno (r9↔r11) casan**. O sea: el reparto
r9/r11 lo decide **dónde muere `entry`**, no la asociación.

Lo que faltaba era el `add`+`stb`. **Ensayos de sentencia (v1-v12, w1-w6):**

- v1, v2, v4-v9, v11, v12 (separar el `++`, sacar un local, `<<7`, `*(p+i)`,
  reordenar los sumandos): **todos 364 B / 92,97%**.
- v10 (`string[pos]`/`string[pos+1]` y `pos += 2`): 364 B / 93,73%, 27 filas.
- **v3, w1, w2, w3, w4 → 100%, 368 B, 0 filas de diff.** Los cuatro son la misma
  cosa: **el segundo byte se escribe por puntero**.

Aplicado (v3):

    string[string_pos++] = static_cast<char>(index128);
    char *dest = &string[string_pos++];
    *dest = histogram_index - entry * 0x80 - 0x80;

w5 (`- (e+1)*0x80` con el puntero) baja a 94,73 y w6 (puntero **con** los casts)
a 95,87 con 372 B: **hacen falta las dos cosas a la vez**, quitar los casts y
sacar la dirección a un puntero. Ésa era la «asociación acoplada al `add`».

---

## 3. `AssignClosestOffsets__9AIPursuit` (zAI, 1.684 B, 99,667%) — TECHO, 13 formas más

Diagnóstico confirmado y **cerrado a UNA fila**: el objetivo hace
`mr r9,r29` + `cmpwi r29,0`; nosotros `mr. r9,r28`. Las otras 6 filas
(260, 263, 271, 295, 314, 352) son el intercambio r28↔r29 que cae de ahí.

Dos datos nuevos del ensamblador del objetivo, que descartan la explicación fácil:

- La etiqueta de la cola, `.L_80031D70`, tiene **cinco predecesores** (la caída
  más los cuatro `continue`), y **el `mr` y el `cmpwi` están los dos DESPUÉS de
  ella**: no hay frontera de bloque entre ambos. No es que combine no pueda
  fundirlos por estar en bloques distintos.
- La estructura del bucle es idéntica en los dos lados: `subi rX, r9, 1` en la
  cabeza (`.L_80031B7C`) y la copia de arrastre en la cola. Lo único distinto es
  **qué pseudo nombra la comparación**: el objetivo el ORIGEN de la copia, nosotros
  el DESTINO — y con destino sí hay `LOG_LINK`, así que combine sustituye y funde.

**Formas barridas esta ronda (ninguna mejora; base 1.680 B / 99,66746%):**

| | forma | tam | pct |
|---|---|---|---|
| d1 | dos variables + coma en el `while` | 1704 | 98,563 |
| d2 | `--n` al principio del cuerpo | 1700 | 97,553 |
| d3 | `while (--n != 0)` | 1680 | 99,656 |
| d4 | `while (n-- > 1)` | 1688 | 99,059 |
| d5 | `while ((n = n - 1) > 0)` | 1680 | **99,667 (igual)** |
| d6 | `while (0 < --n)` | 1680 | **99,667 (igual)** |
| d7 | dos variables, copia dentro del cuerpo | 1704 | 98,112 |
| e1 | `n = numCols` en vez de `.size()` | 1656 | 97,150 |
| e2 | declarar `n` antes de `INDEX_ASSIGNED` | 1680 | **99,667 (igual)** |
| e3 | `register int n` | 1680 | **99,667 (igual)** |
| e4 | `--n;` al final del cuerpo | 1684 | 99,268 |
| e5 | `unsigned int n` | 1680 | 99,656 |
| e6 | `while (--n + 1 > 1)` | 1688 | 99,665 |

Suman **13** a las 9 que ya estaban barridas. **No he probado**: tocar el bucle
`j` interior (12 formas ya barridas antes), ni banderas (25 ya barridas), ni
restricciones de registro.

---

## 4. `__9Smackable` ctor (zPhysics, 3.120 B, 99,262%) — TECHO, pero con el orden del fuente DEMOSTRADO

Las tres cosas que faltan (`faltan 3, sobra 1`) cuelgan de **una**: CSE reutiliza
el 0 de `UCrc32 smack_class;` para el último argumento (`collision_mask = 0`) de
`RBComplexParams`, mientras el objetivo emite **dos ceros independientes**
(`li r0,0` para `smack_class`, muerto al instante, y `li r11,0` fresco para la
máscara).

**Lo que sí queda demostrado esta ronda** (y cierra una hipótesis abierta):

- **El orden del fuente del original es `active` primero y `smack_class` después**,
  como lo tenemos. Se lee en el asm del objetivo: el `li r0,0` + `stw r0,0x58(r1)`
  están **en el bloque de unión del `||`** (detrás de `li r29,1`), no antes del
  test. Mover la declaración cambia el sitio de la tienda, no sólo los registros.
- El mapa de líneas confirma la identificación: fila 565 → `UCrc.h` (el
  `UCrc32() : mCRC(0) {}`), filas 616/627 → `UCrc.h` dentro de `RBComplexParams`.
- **`if (simple_physics)` a secas da el TAMAÑO EXACTO (3.120 B) y es peor**:
  98,015%, 65 filas. El motivo, medido: no es que se alargue r17, es que
  **`simple_physics` se DERRAMA a pila** (`stw r9,0xd4(r1)` + recarga
  `lwz r9,0xd4(r1)` antes del `cmpwi`) en vez de vivir en r17, y el marco pasa de
  0x150 a 0x158. Otro caso de «el tamaño exacto miente».

**Ensayos (base 3.112 B / 99,2615% / 26 filas):**

| | forma | tam | pct | filas |
|---|---|---|---|---|
| s1 | `if (simple_physics)` | 3120 | 98,015 | 65 |
| s2 | `if (simple_physics != false)` | 3120 | 98,015 | 65 |
| s3 | `if (!simple_physics) {complejo} else {simple}` | 3120 | 90,646 | 166 |
| s4 | `if (simple_physics == false) {…}` | 3120 | 90,646 | 166 |
| s5 | `if (…)` + `0u` en la máscara | 3120 | 98,015 | 65 |
| s6 | `== true` + `UCrc32::kNull.GetValue()` | 3120 | 99,012 | 27 |
| s7 | `UCrc32 smack_class = UCrc32();` | 3112 | 99,262 | 26 |
| s8 | `} else if (!simple_physics) {` | 3120 | 95,809 | 98 |
| **u1** | **`smack_class` ANTES de `active`** | **3116** | 99,153 | **16** |
| u2 | `smack_class` después del if/else | 3128 | 97,492 | 139 |
| u3 | `if (…)` + local `unsigned int mask = 0` | 3120 | 98,015 | 65 |
| u4 | `== true` + `mask` local dentro del `else` | 3112 | 99,262 | 26 |
| u5 | `const bool use_simple = simple_physics;` | 3120 | 98,015 | 65 |
| u6 | segundo `if (simple_physics)` al final | 3136 | 97,477 | 73 |
| u8 | u1 + `if (simple_physics)` | 3120 | 97,490 | 71 |
| u13 | u1 + `!= false` | 3120 | 97,490 | 71 |

**u1 es el hallazgo**: mover `UCrc32 smack_class;` delante de `active`
**rompe la cadena de CSE** y produce a la vez el `li r0,0` (registro correcto,
temporal muerto) y el **`li r11,0` fresco** de la máscara — las dos taras que
traía el encargo. Baja a 16 filas. **No se aplica** porque introduce una tercera
tara: la tienda de `smack_class` se adelanta siete instrucciones, y **acabo de
demostrar que el orden del original es el contrario**. Dejarlo commiteado sería
grabar en el fuente un orden que sé falso, y no aporta bytes (`matched_code` es
todo-o-nada).

**Lo que queda** es exactamente lo que decía el encargo: la rama *skip-blocks* de
`cse_end_of_basic_block` cruza el `beq cr2` hacia `.L_80219E94` (la etiqueta del
`else`, un solo predecesor visible). **No he probado**: dar a esa etiqueta un
segundo uso en la RTL con una forma de fuente natural — no se me ocurrió ninguna
que no cambiara la semántica. Se deja `simple_physics == true`.

---

## 5. `UpdateLoaded__Q217SuspensionTraffic4Tire` (zPhysicsBehaviors, 856 B, 97,210%) — TECHO, 8 formas más

Mecanismo confirmado (el `@ha` de larga vida): el objetivo iza
`lis r30, 1.0f@ha` **antes de `bl VU0_Atan2`**, lo guarda en r30 (salvado, de ahí
`stmw r30`/`lmw r30` y el marco 0x30 contra 0x28) y lo sirve a **dos** usos —
`1.0f < |fwd_vel|` y `skid_speed > 1.0f`. Nosotros emitimos un `lis` por uso.
Neto: nos SOBRAN 4 B (860 contra 856).

**Ocho formas más, ninguna mueve nada** (base 860 B / 97,21028%):

| | forma | tam | pct |
|---|---|---|---|
| t1 | `float absfwd = Abs(fwd_vel)` reutilizado | 840 | 93,850 |
| t2 | `Abs(fwd_vel) > 1.0f` | 860 | **97,210 (byte a byte igual)** |
| t3 | `1.0f < skid_speed` | 860 | **igual** |
| t4 | t2+t3 | 860 | **igual** |
| t5 | `mSlipAngle` después de `slip_speed` | 868 | 88,710 |
| t6 | `const float ONE = 1.0f` | 860 | **igual** |
| t7 | `float ONE = 1.0f` (sin const) | 860 | 95,794 |
| t8 | `mEBrake > 0.0f && Abs(fwd) > 1.0f` | 860 | **igual** |

Dato útil para el catálogo: **el sentido de la comparación y sacar la constante a
un `const float` con nombre son byte a byte neutros** (t2, t3, t4, t6, t8). Sólo
mueven el código quitar el `const` (t7), hoistar `Abs` (t1) o reordenar
sentencias (t5). Con las 18 formas y 17 banderas anteriores, van **26 formas**.
**No he probado** la dirección contraria (`Place`, donde izamos nosotros y el
objetivo no) — no está en mis unidades.

---

## 6. Lo que queda en las nueve unidades (`triage.py`)

    2908 B  99,601%  zAI                UpdateAllAvoidables       4 SUST   VEDA
    1488 B  99,301%  zPhysicsBehaviors  Add__6RBGrid              4 SUST   VEDA
    3120 B  99,262%  zPhysics           Smackable ctor            §4
    1684 B  99,667%  zAI                AssignClosestOffsets      §3
     856 B  97,210%  zPhysicsBehaviors  Tire::UpdateLoaded        §5
     808 B  98,886%  zFeOverlay         NotificationMessage       §7
     440 B  94,500%  zFeOverlay         SetScreenNames            §7
     116 B  95,517%  zFe                SubTitler::GetElapsedTime §7
    --> 8 funciones, 11.420 B   ·   MURO: 12 funciones, 15.352 B

## 7. Las tres pequeñas, medidas y no cerradas

- **`NotificationMessage__13CustomizeMain`** (808 B): **una sola tara**, el
  intercambio r28↔r31 entre `CarCustomizeManager &mgr` y el resultado de
  `CustomizeIsInBackRoom()`; el `li r6,0` que se adelanta una ranura cae de ahí.
  n2 (`bool inBackRoom` local) sale **byte a byte igual**; n1 y n3 (usar
  `gCarCustomizeManager` directo, o puntero) **no compilan** porque `mgr` se usa
  después. Es reparto de registros puro.
- **`SetScreenNames__13CustomizeMain`** (440 B): el objetivo materializa **todas**
  las direcciones de cadena antes de cualquier `stw`; nosotros intercalamos el
  primer `stw`. **Hay un `__asm__("")` commiteado en `CarCustomize.cpp:2012`**
  dentro de esta función. Medido: **con él 94,500% / 32 filas, sin él 91,727% /
  23 filas** — o sea, **no cierra la función** (el caso que la ronda 18 mandó
  retirar). No lo he tocado porque cambia el porcentaje en los dos sentidos y no
  vale bytes; **queda señalado para decisión.**
- **`SubTitler::GetElapsedTime`** (116 B): al objetivo le sobra un `fmr f1,f0`
  (calcula en f0 y copia al registro de retorno; nosotros calculamos en f1).
  g1 (`timeElapsed = …; return timeElapsed;`), g2 (temporal de bloque) y
  g3 (`+=`) dan **las tres 112 B / 95,345%**, peor que la base. GCC funde la copia
  siempre. **No he probado** restricciones de registro.

## 8. Congelado

    frozen.py cong Speed/Indep/SourceLists/zFe2
    frozen.py cong Speed/Indep/SourceLists/zPhysicsBehaviors
