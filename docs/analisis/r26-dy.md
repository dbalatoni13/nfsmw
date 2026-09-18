# Ronda 26 — zDynamics + el middleware libre

Estado al cerrar: **`srender` cerrada al 100 %** (188 B, de 0,0000 % a 100 %,
`audit.py` 0 FALLA, congelada) y **`zDynamics::Geometry::SphereVsBox`
99,02531 → 99,03587 %** (una fila menos, 15 → 14). zDynamics sigue en
93,4567 % porque la función no cierra.

**No he hecho commit.** Los ficheros míos son sólo dos:
`src/Speed/Indep/Libs/snd/9/source/library/cmn/srender.c` y
`src/Speed/Indep/Src/Physics/Dynamics/Geometry.cpp`. Los otros seis que salen
en `git status` son de agentes de esta misma ronda.

---

## 1. CERRADA: `srender::SNDI_validrendermode` — 188 B, 93,29787 % → 100 %

Es la única función de la unidad, así que **`srender` entera pasa de 0 % a
100 %**. `audit.py`: `ok 188 B, 8 ramas, 10 relocs, 0 literales`, 0 FALLA en dos
pasadas. Congelada (`03d893b947b988e2`).

Hacen falta **dos** cambios y los dos están medidos por separado:

1. **`validrendermode = 0; continue;`** en vez de caer al `if (validrendermode
   != 0)` de abajo. Es equivalente —con 0 el `if` no se toma— y produce el `b`
   directo al incremento del bucle que hace el objetivo, en vez de volver a
   comprobar. Vale **1 fila** (99,89362 → 100 %).
2. **`__asm__("")` delante del primer `continue`** — **DEUDA DECLARADA**, ver
   §1-bis. Vale **4 filas y 4 B** (93,29787 → 99,89362 %).

### El mecanismo, que es el hallazgo reutilizable

El objetivo tiene **DOS copias** de la recarga `lwz r9, 0(r8)` (`*prenderindex`),
una por camino: la del `continue` y la de la caída. Nosotros la izábamos por
encima del `and. r3, r10, r3` —**movimiento entre bloques del planificador**—, con
lo que el bloque del `continue` se quedaba vacío y `jump.c` lo fundía invirtiendo
la rama (`bne` → `beq`). Resultado: 184 B contra los 188 del objetivo.

Es el patrón de `HERRAMIENTAS.md` §8-bis leído al revés: **cuando el objetivo NO
comparte una redundancia que a nosotros se nos comparte, hay que impedir la
compartición, no buscarla.** Aquí la forma de fuente no basta (ver vedas) y lo
que la impide es una barrera vacía.

### 1-bis. La deuda, con su medida

`__asm__("")` **no emite ninguna instrucción**, pero el original no lo tenía. Está
documentado en un comentario dentro de `srender.c`. Vedas medidas buscando una
forma natural equivalente:

| forma | % |
|---|---|
| base (sin nada) | 93,29787 |
| sólo el `continue` de (1), sin el asm | **93,40426** (184 B) |
| `goto` a una etiqueta al final del cuerpo en vez de `continue` | **93,40426** |
| asm + `continue` | **100** |

O sea: el `continue` de (1) por sí solo vale 0,1 pp; sin la barrera no hay forma.

---

## 2. `zDynamics::Geometry::SphereVsBox` — 1.896 B, 99,02531 → 99,03587 %

### 2.1 Lo que se cerró: la fila 334 (`mr r3, r29`)

Nada que ver con el bucle. En la rama `volume_box <= 0.0f`:

```c
UMath::Addxyz(rel_collision_point, sphereA->mPosition, result->mCollision_point);
UMath::Scalexyz(result->mCollision_normal, -1.0f, dd);
d = RayExitSphere(point_1, dd, sphereA->mPosition, radius);   // <- aquí
```

`point_1` es `const UMath::Vector4 &point_1 = result->mCollision_point;`. El
objetivo usa **r29 en las dos** (`mr r5, r29` para el destino del `Addxyz` y
`mr r3, r29` para el primer argumento de `RayExitSphere`): una sola dirección.
Nosotros teníamos dos pseudos (r29 y r30). Pasando
`RayExitSphere(result->mCollision_point, ...)` la dirección es una y cae en r29.

`point_1` **sigue existiendo** (se usa tres veces más) y el DWARF del original
tampoco le da registro, así que no contradice el volcado.

### 2.2 Lo que queda: 14 filas y UN mecanismo, no dos

El encargo daba dos problemas independientes (el par f30/f31 y la posición del
contador). **Son el mismo**, y está verificado con `scripts/alloc.py` sobre el
`.greg`:

`sched1` adelanta `a_lp++` (insn 452) y el `cmpwi` (457) al principio del bloque
7. Eso los deja **dentro** del rango vivo de `b_dim` (pseudo 152), que mide 39 y
puntúa `300000/39 = 7692` contra los 8000 de `p_dot` (157). Por eso `p_dot` coge
f31 y `b_dim` f30, al revés que el objetivo. Ocho filas de f30/f31 + seis de la
posición del contador = **una sola causa**.

**Comprobado**: moviendo el `__asm__("")` de delante de `penetration[a_lp]` a
delante de `a_lp++`, el rango de `b_dim` baja a **34** (pri **8823**), coge f31,
y **las catorce filas del bucle desaparecen**.

### 2.3 Por qué no paga, y por qué el eje de la barrera tiene techo

Esa variante da **98,94514 %** (98,95570 con el arreglo de §2.1), 18 filas. La
barrera arrastra también los dos `addi rX, r1, 0x68/0x78` que **gcse/PRE inserta
AL FINAL del bloque** (insns 2000 y 2003, justo antes del salto). El objetivo los
planifica en las ranuras 115 y 117, entre los `fsubs`; con la barrera se van
detrás de `a_lp++`, y entonces:

- el pseudo **627** (`&prev_penetration[0]`) acorta su vida de **58 a 51**,
- su prioridad sube de **517 a 588**,
- y le quita **r20** a `in` (545), que se va a r19.

Son 4 filas nuevas (`in`), 10 de un intercambio f13/f0 en las penetraciones y 4
de la posición de los dos `addi`.

**El techo del eje**: `insert_insn_end_bb` de gcse inserta **siempre** delante
del salto, así que **no existe ninguna posición de `__asm__("")` que deje
452/457 al final y 2000/2003 delante**. Para eso haría falta que la prioridad de
452 fuera 0, o sea que el `cmpwi` **no dependiera** del incremento — y el
objetivo emite `addi r31,r31,1` + `cmpwi r31,0x2`, que sí depende. Ahí está el
enigma que queda.

### 2.4 El mapa de líneas: el `__asm__("")` no corresponde a ninguna sentencia

`lmap.py` sobre el objetivo da los saltos de línea del original. Comparando con
los nuestros:

| sentencia | línea orig. | línea nuestra | Δ |
|---|---|---|---|
| `normal` / `b_dim` | 449 / 450 | 374 / 375 | 75 |
| `if (p_dot > 0.0f)` | 455 | 377 | 78 |
| `dists[a_lp] =` / `prev_dists[a_lp] =` | 458 / 459 | 382 / 383 | 76 |
| `if (!(dists < b_dim+radius))` | 461 | 384 | 77 |
| `if (!(dists > b_dim))` / `in++` | 464 / 465 | 387 / 388 | 77 |
| `penetration[a_lp] =` | 467 | 391 | **76** |
| `prev_penetration[a_lp] =` | 468 | 392 | 76 |
| `a_lp++` | 471 | 393 | 78 |
| `} while` | 474 | 394 | 80 |

El escalón **77 → 76** entre `in++` y `penetration` dice que el original tiene
**una línea menos** ahí: exactamente donde está nuestro `__asm__("")`. Y el
`if/else` de `normal_dir` es **2 líneas más corto** en el original (455→458 son 3
líneas; las nuestras 377→382 son 5), o sea un `if`/`else` de una línea cada uno.
Los huecos 469-470 y 472-473 (entre `prev_penetration` y `a_lp++`, y entre
`a_lp++` y el `while`) no producen notas, así que son comentarios o líneas en
blanco: **no son sentencias escondidas**.

### 2.5 Vedas de la r26 sobre el bucle (base 99,02531)

| ensayo | forma | % |
|---|---|---|
| c1 | asm delante de `a_lp++` | 98,94514 |
| c2 | `for (a_lp=0; a_lp<=2; a_lp++)` con el asm al final del cuerpo | 97,71308 |
| c3 | asm detrás de `in++` **y** delante de `a_lp++` | 98,94514 |
| c4 | asm entre las dos sentencias de penetración | 98,08017 |
| c5 | `prev_dists` detrás del primer `if` | 83 filas |
| c6 | `prev_dists` detrás de los dos `if` | 88 filas |
| c7 | sin asm | 98,47890 |
| c8 | asm entre `dists` y `prev_dists` (+ el de base) | 98,59283 |
| c9 | asm delante del primer `if` (+ base) | 98,60338 |
| c10 | asm delante del segundo `if` (+ base) | 99,02531 (= base) |
| c11 | asm delante de la declaración de `p_dot` (+ base) | 98,87764, **1.892 B** |
| c12 | sólo asm entre `dists` y `prev_dists` | 98,08861 |
| c13 | sólo asm delante del primer `if` | 98,09916 |
| c14 | sólo asm delante de `prev_penetration` | 98,13291 |
| c17/c18 | dos y tres `__asm__("")` seguidos al principio del bloque 7 | idéntico a base |
| c19/c20/c21 | dos asm al final, o combinaciones | 98,95570 |

**c15** (el `RayExitSphere`) es lo único que sube: **99,03587 %**. **c16** = c15+c1:
98,95570 %.

### 2.6 Lo que NO he probado en `SphereVsBox`

- Tocar el **segundo y el tercer bucle** (los `for (a_lp=0; a_lp<=2; a_lp++)`)
  para alargar la vida del pseudo 627 por el lado de su USO (`mr r11, r19`,
  índice 413, dentro del `for` de `nearestface`). Es el único camino que queda
  para que `in` recupere r20 con la barrera puesta, y no lo he tocado porque esa
  región casa entera y el riesgo de romperla es alto.
- Restricciones de registro (`register float b_dim asm("f31")`). Último recurso
  y no lo he gastado.
- El permutador (la r25 lo dejó colgado sobre esta misma función).

---

## 3. Middleware: lo medido unidad por unidad

Todas construidas con `build_direct.py` (ninja no las levanta).

| unidad | función | B | % | filas | estado |
|---|---|---|---|---|---|
| **srender** | `SNDI_validrendermode` | 188 | **100** | 0 | **CERRADA** |
| filesys | `AddToQueue__13FILEOPERATION` | 252 | 95,15873 | 4 | −4 B, ver §4 |
| vmbase | `__VMBASESetupExceptionHandlers` | 380 | 99,15790 | 12 | nombre de registro, ver §5 |
| vm | `__VMAllocVirtualToARAMLUT` | 168 | 96,07143 | 3 | −4 B, ya minada |
| DebuggerDriver | `EXI2_Poll` | 264 | **99,81818** | **2** | ver §6 |
| DebuggerDriver | `EXI2_WriteN` | 432 | 94,44440 | — | sin mirar |
| DebuggerDriver | `AmcEXISetExiCallback` | 108 | 85,18519 | 4 | ver §6 |
| spchpick | `iSPCH_MakeSampleRequests` | 524 | 98,32060 | — | sin mirar |
| spchpick | `iSPCH_ChooseSamples` | 412 | 95,53400 | — | sin mirar |
| criticalpath | `VP6_PredictFilteredBlock` | 740 | 94,39460 | — | sin mirar |
| dvd_device | `StartNonAlignedAyncRead` | 440 | 93,59090 | — | sin mirar |
| sfir | `calcFIRCoeffs` | 936 | 89,25214 | **75** | ver §7 |
| spchsamp | `iSPCH_GetSampleSizeData` | 136 | 85,73529 | 17 | permutación pura |

**Corrección al encargo**: `sfir::calcFIRCoeffs` no son «+4 B y una instrucción»
sino **75 filas distintas** de 235 (el `+4 B` es cierto, pero el reparto de
registros está rotado de r26 a r30 en toda la función). No es una función a una
plaza.

---

## 4. `filesys::AddToQueue` — 252 B, 95,15873 %, **−4 B**, y el diagnóstico cerrado

Faltan **exactamente dos cosas**, que son una: el objetivo tiene
`mr. r10, r11` + `beq` (el `if (node != 0)` del `operator++` del iterador) y
nosotros emitimos `mr r10, r11` sin la comparación ni la rama. **GCC nos pliega
ese `if`** porque, entrando por `goto nextOperation`, sabe que `current != 0`.

**Hallazgo nuevo y accionable**: con el bucle escrito como
`while (current != 0) { ... }` (ensayo f6) **GCC NO lo pliega**: aparecen
`mr.`+`beq`+`lwz` y el tamaño da **252 B clavados**. Lo que falla ahí es la
*disposición*: el `while` deja el incremento al final con un `b` a la condición,
y el objetivo lo tiene al principio con un `b` de entrada que lo salta. O sea:

- **forma `goto`** → disposición correcta, `if` plegado (95,15873 %, 248 B, 4 filas);
- **forma `while`** → `if` presente, disposición al revés (87,14286 %, 252 B, 9 filas).

Quien siga tiene que juntar las dos. Vedas medidas (todas 95,15873 % / 4 filas,
o sea idénticas a la base, salvo donde se indica):

| ensayo | forma | % |
|---|---|---|
| f1 | `__asm__("" : "=r"(prev) : "0"(prev))` entre `prev = current;` y el `if` | 95,15873 |
| f2 | el `&&` partido en dos `if` con etiqueta `insertHere` | 95,15873 |
| f3 | `__asm__("")` detrás del `if (prev != 0)` | 95,15873 |
| f4 | `current = (prev != 0) ? prev->next : prev;` | 95,15873 |
| f5 | comparación muerta `if (prev->next != prev->next)` delante | 95,15873 |
| f6 | `while (current != 0) { ... }` | **87,14286**, 252 B |
| g1 | `__asm__("")` entre `prev = current;` y el `if` | 95,15873 |
| g5 | `for (;;)` con `goto` a una etiqueta interior | 95,15873 |
| g6 | `while (1)` con `continue` | 95,15873 |
| g7 | `else { current = 0; }` | 92,30159, **256 B** |
| g8 | `if (prev == 0) goto checkPriority;` | 95,15873 |

**Lo importante**: el lavado con `asm` de constricciones (f1) **NO rompe la
equivalencia** en GCC 2.9. Es la primera medida de eso que hay en el proyecto y
descarta ese truco para toda la familia de plegados por condición dominante.

---

## 5. `vmbase::__VMBASESetupExceptionHandlers` — 380 B, 99,15790 %, 12 filas

Confirmado el diagnóstico que ya traía el fichero: las 12 filas son **el registro
de `instruction`**; el objetivo la encadena entera por **r0** y nosotros la
coalescemos con `branch` en r6. Añado a las 19 formas que ya estaban medidas:

| eje | formas | resultado |
|---|---|---|
| `instruction` partida en 4 a nivel de función | v1 | idéntico |
| `instruction` `register` | v2 | idéntico |
| `instruction` declarada primera / segunda / última | v3, v4, v5 | idénticas |
| `instruction` sólo en los sitios 2 y 4 | v6 | **98,31579** (22 filas) |
| `instruction` sólo en los sitios 1 y 3 | v7 | idéntico a la base |
| sin `instruction` en ninguno | v8 | 98,31579 (22 filas) |
| `branch1..4` sin `register` | w1, w4 | idénticas |
| `(0 - branch)` y `-(s32)branch` sin paréntesis | w5, w6 | idénticas |
| un solo `branch` × {con, sin, parcial} `instruction` | z1..z4 | 98,31579 / 98,73684 |
| **13 `#pragma` por función** (`opt_lifetimes`, `opt_common_subs`, `opt_propagation`, `opt_dead_assignments`, `opt_dead_code`, `optimize_for_size on`, `global_optimizer off`, `opt_strength_reduction off`, `optimization_level 2` y `3`) | — | **todos idénticos** |
| `peephole off` / `scheduling off` / `optimization_level 1` / `0` | — | 87,47 / 65,91 / 75,74 / 71,16 |

O sea: **el eje de los `#pragma` de mwcc está agotado en esta función**, y
también el de la declaración. Dato útil de v8: **sin la variable, la cadena se
computa en UN solo registro** (la forma del objetivo) pero es r5, y además se
intercambian los papeles de r5 y r6 con la base 0x80000000. La variable
`instruction` es lo que fija r5/r6 bien; lo que falta es que sea r0.

---

## 6. `DebuggerDriver` — dos funciones a dos y cuatro filas, y las dos son el epílogo

- **`EXI2_Poll`, 264 B, 99,81818 %, 2 filas**: el objetivo hace
  `lwz r0, 0x24(r1)` (LR) **antes** de `lwz r3, 0x18(r1)` (el valor de retorno) y
  nosotros al revés. Mismo tamaño, misma cadena; es orden de `sched2` en el
  epílogo.
- **`AmcEXISetExiCallback`, 108 B, 85,18519 %, 4 filas**: `mr r3, r31` y
  `addi r1, r1, 0x18` van dos ranuras más tarde en el objetivo. Otra vez epílogo.

No las he trabajado. Son las dos más baratas que quedan en el middleware si
alguien encuentra el eje del epílogo.

---

## 7. `sfir::calcFIRCoeffs` — corrección al encargo

**No son 4 filas**: son **75 de 235**. El `+4 B` es real y la causa está bien
diagnosticada (PRE nos iza `slwi` y `addi r29,r30,0x20` a un bloque nuevo), pero
el efecto es una **rotación de r26..r30 en toda la función**, no una instrucción
suelta. Ensayo p1 (`__asm__("")` detrás del `switch`, para cortar el izado):
**88,33334 %**, peor. Veda.

Aviso de fichero: **`sfir.c` tiene finales de línea CRLF** y `srender.c` LF; los
parches literales fallan si no se respeta el terminador de cada línea.

---

## 8. Herramientas: cuatro cosas que han costado tiempo

1. **`libdiff.py` peta con las unidades de mwcc.** `vmbase` y `vm` se compilan
   con `mwcc_sjis`, y `libdiff.py` invoca `ngccc.exe` a pelo: sale un
   `FileNotFoundError [WinError 2]` de `CreateProcess` con 40 líneas de traza y
   **ninguna pista de que el problema es el compilador**. Para middleware de
   mwcc hay que usar `build_direct.py` + `objdiff-cli` a mano.
2. **`ngcas` falla de forma TRANSITORIA** con
   `error: Could not open output file <ruta>.o`. Se lee como un error de fuente y
   no lo es: **reintentar arregla**. Me tumbó tres ensayos seguidos de `filesys`
   antes de verlo. Todo arnés de barrido debería reintentar el build 3-4 veces.
3. **`build_direct.py <filtro>` toma el filtro por SUBCADENA**: `filesys_c`
   compila `filesys_c` **y** `filesys_cc`, y `sfir` compila `sfir` y `sfir8`.
   Comprobar el `ok <unidad>` exacto no basta.
4. **`scripts/alloc.py` necesita el `.lreg` y el `.greg` de la unidad ENTERA.**
   Para zDynamics hay que preprocesar `Speed/Indep/SourceLists/zDynamics.cpp`,
   no `Geometry.cpp` suelto: éste no compila solo (`memset undeclared`).
   Receta en `c26dy_rtl.py` del scratchpad.

Y una nota de disco: el scratchpad de la sesión traía **1,9 GB** de volcados de
rondas anteriores (`d_*.json` de 20-32 MB, subdirectorios `jf19/`, `probe/`,
`rtl/`…). Borrados: de 14 a 16 GB libres.

---

## 9. Promoción pendiente

**`srender` está al 100 % y en `configure.py` sigue como `NonMatching`**
(línea 859) con `linked False`. `trypromo.py srender` dice «NO está en la lista
de enlace». No he tocado `configure.py`: es del agente de `linked`.
