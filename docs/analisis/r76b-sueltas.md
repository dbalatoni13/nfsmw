# r76b — las cuatro barreras sueltas

Lote: una barrera por unidad, las cuatro unidades `Matching`, las cuatro barreras
**vivas** (comprobado en la tanda anterior: sellando el `.o` antes y después, las
cuatro lo cambian). El objetivo no era medir si estaban muertas, sino encontrar la
forma de C que las sustituye.

Resultado: **una cerrada al 100 % sin andamio** (y con el mapa de líneas cuadrando
al completo), **tres sin cerrar**. Los cuatro andamios salen igualmente.

## Tabla

| función | unidad | andamio retirado | qué dijo el oráculo | % antes | % después | nuestro/objetivo | formas probadas y descartadas | cruce `.line` |
|---|---|---|---|---|---|---|---|---|
| `FastMem::SplitOrExpand` | zFoundation | `asm volatile ("" : : : "r3");` | `this`=r31, `bytes`=r4 (no se usa), **una** local: `void *block; // r10` — se llama `block`, no `newBlock`; sin inlines | 100,0 | **100,0** | 132/132 | — (cerrada a la primera forma que salió del mapa de líneas) | **15 de 15 notas, delta constante 188.** Exacto |
| `bList::Sort` | zBWare | `asm volatile("" : "+r"(did_swap));` | `this`=r29, `check_flip`=r27, `node`=r30, `next_node`=r31, `did_swap`=r28. **No existe la local `cmp`** que teníamos | 100,0 | 94,535 | 172/172 | 11 variantes (abajo) | Estructura del bucle correcta tras reordenar; sólo desvía la pareja `+18`/`+1c` |
| `Dynamics::Collision::Geometry::SphereVsBox` | zDynamics | `__asm__("" : "+r"(a_lp) : : "memory");` | 16 locales, **todas presentes**; `a_lp`=r31, `in`=r20; bloque del cuerpo `0x80089E94–0x80089F3C` sólo con `normal`, `b_dim`, `p_dot` | 100,0 | 98,489 | 1896/1896 | `for (a_lp = 0; a_lp <= 2; a_lp++)` → **97,721 %, peor** (65 diferencias en vez de 22); `} while (++a_lp <= 2);` → 98,489 %, idéntico | El original pone `a_lp++` en la línea 476, seis por detrás de la última del cuerpo (471/474): suelto al final, como el nuestro |
| `CWorldAnimEntity::Init` | zAnim | `asm("");` | 20 locales; `play_flags`=r28, `skel`=r27, `info`/`anim_part` sin registro. **Tres** bloques anónimos con `int res` (los tres `CreateFnAnimFromNamehash`) y **ninguno** para `anim_part->Init(skel)` | 100,0 | 97,982 | 912/912 | `play_flags` asignado antes de `skel` → **97,408 %, peor**; quitar el bloque `{ int res = … }` → 97,982 %, igual pero **más fiel** (se queda) | El original no tiene nota de cpp entre `SetLocalDelayTime` (232) y `GetAnimPart`/`"ROOT"` (242): **nueve líneas sin código** |

## Veredicto por unidad

| unidad | funciones | ¿reproduce su objeto? |
|---|---|---|
| **zFoundation** | 188 de 188 al 100 % | **SÍ. Sigue `Matching`, no hay que tocar nada.** |
| **zDynamics** | 40 de 41 (falta `SphereVsBox`, 1896 B) | **NO. Hay que degradarla a `NonMatching`.** |
| **zAnim** | 314 de 315 (falta `Init`, 912 B) | **NO. Hay que degradarla a `NonMatching`.** |
| **zBWare** | 238 de 239 (falta `Sort`, 172 B) | **NO. Hay que degradarla a `NonMatching`.** |

Coste medido de retirar los cuatro andamios: **2.980 B de `.text`** que dejan de casar
y **tres unidades de `Matching` a `NonMatching`**. Los tres tamaños son íntegros
(`matched_code` es todo-o-nada), no proporcionales al porcentaje.

No he tocado `configure.py`: la degradación de las tres queda como **PROPUESTA** para
la ventana de decisión.

## El hallazgo: `FastMem::SplitOrExpand`, y por qué es reutilizable

Sin la barrera faltaba **una sola instrucción**, un `mr r3, r31` delante del `bl CoreAlloc`
(132 contra 128 B). `this` ya está en r3 al entrar y nunca se toca, así que CSE ve que
el movimiento es redundante y lo borra; el clobber de `"r3"` lo único que hacía era
mentirle a CSE. Para que el original lo tenga, entre el prólogo y la llamada tiene que
haber algo que corte el bloque básico extendido de CSE, y como no sobra ninguna
instrucción, sólo puede ser una **`CODE_LABEL`**.

El mapa de líneas dijo cuál:

```
+00 → 263   la firma
+14 → 265   LAS DOS condiciones en la MISMA línea  ->  un `||`
+2c → 268   lwz r5,0x100(r31) ; mr r3,r31 ; bl CoreAlloc
+38 → 269   mr. r10,r3 ; beq
...
+6c → 270   li r3,0     <- el `return false` está en la 270, NO al final de la función
```

Un `if (A || B) { return false; }` emite el bloque `then` **entre** la condición y el
resto, y la etiqueta de caída queda justo delante de la llamada. Después el
reordenamiento de bloques se lleva el `li r3,0` al final, pero el `mr r3,r31` ya está
en el RTL y nadie lo vuelve a borrar. La forma:

```c
bool FastMem::SplitOrExpand(std::size_t bytes) {
    void *block;
    if (mExpansionSize == 0 || mLocks != 0) {
        return false;
    }
    block = CoreAlloc(mExpansionSize, mName);
    if (block == 0) {
        return false;
    }
    ((void **)block)[0] = mBlock;
    ((unsigned int *)block)[1] = mExpansionSize;
    mBlock = block;
    mBytes = mExpansionSize;
    mUsed = 16;          // el original lo pone AQUÍ, no en medio
    return true;
}
```

100,0 %, y las quince notas de línea con **delta constante 188**: `block` en su propia
línea (264 en el original), las dos condiciones juntas, el `return false` temprano y
`mUsed = 16` la última asignación. Las tres cosas las dictó el mapa de líneas, ninguna
se adivinó.

**Regla que sale de aquí, y que no teníamos:** *un `mr rN, rM` de más en el original
delante de una llamada, sin ninguna otra instrucción de diferencia, no es reparto: es
una `CODE_LABEL` que a nosotros nos falta. La fuente que la fabrica es un `||` o un
`return` temprano, y el mapa de líneas dice cuál porque el número de línea del
`return` delata dónde estaba el bloque.* Es barata de comprobar y aplica a cualquier
near-miss de ±4 B con un `mr` a un registro de argumento.

## `bList::Sort` — por qué no sale, con la aritmética

Sin la barrera el tamaño no cambia (172/172) y las diferencias son nueve:
`check_flip` y `did_swap` se intercambian r27 y r28 (ocho instrucciones), y el
`li did_swap,0` y el `lwz node,0(this)` salen en orden inverso.

No es adivinanza: el volcado `.lreg` de cc1plus (`-dl`, sobre el `.ii` preprocesado)
da los números que usa `allocno_compare` de `global.c`, cuya prioridad es
`floor_log2(n_refs) * n_refs / live_length`:

| pseudo | local | n_refs | live_length | prioridad | registro |
|---|---|---|---|---|---|
| 87 | `next_node` | 25 | 25 | 40000 | r31 |
| 84 | `node` | 16 | 27 | 23703 | r30 |
| 82 | `this` | 7 | 32 | 4375 | r29 |
| 83 | `check_flip` | 4 | 32 | **2500** | r28 ← el original le da r27 |
| 89 | `did_swap` | 6 | 54 | **2222** | r27 ← el original le da r28 |

El orden de asignación reproduce exactamente el reparto observado. Para invertir la
pareja hace falta `pri(did_swap) > 2500`: bastaría con **7 referencias** en vez de 6
(2592), o con un `live_length` de 47 en vez de 54. Y `-fschedule-insns` está activo,
así que sched1 corre **antes** del reparto: el adelanto del `lwz` y el cruce de
registros son el mismo problema, no dos.

Probadas y descartadas (todas dan exactamente el mismo asm):

1. quitar la local `cmp` (se queda quitada: **el DWARF no la tiene, era invención nuestra**);
2. `did_swap` declarado antes de `node`/`next_node`;
3. `did_swap` declarado después;
4. `did_swap++` al final del bloque del `if` (**se queda**: es lo que dice el `.line`);
5. condición del `if` invertida, con el swap en el `else`;
6. `if/else` en vez de `continue`, con `next_node = node->GetNext()` común al final;
7. lo mismo con las ramas al revés;
8. `for (; cond; )` en vez de `while`;
9. `for (;;)` con `break`, `next_node` calculado sólo dentro del bucle;
10. `do { … } while (1)` con `break`;
11. `while (1)` con los dos `break` separados;
12. `if (did_swap == 0) { return; }` en vez del `if` que envuelve al `MergeSort`;
13. `if (did_swap)` a secas.

Un pin de diagnóstico (`register int did_swap asm("r28")`, retirado inmediatamente)
arregla el reparto pero **no** el orden: la primera diferencia que no es de registro
sigue ahí, así que el pin no era el camino.

El cruce `.line` sí aportó: el original pone `Remove()` en la 114 y `did_swap++` en la
120, es decir el incremento va **detrás** del swap. Reordenado; ahora `+4c` lleva las
dos notas de `Remove` y `+50` la del incremento, igual que el original. El código
quedó más fiel aunque el porcentaje no se mueva.

## `Geometry::SphereVsBox` — dónde está

22 diferencias, 1896/1896 B. `addi a_lp,a_lp,1` y su `cmpwi` se adelantan **once
ranuras**, por delante del `stfsx` de `penetration[]`, y eso arrastra el reparto
(`f30`↔`f31`, `r19`↔`r20`). Es sched2 puro: el reparto de FPR es consecuencia, no
causa. Las dos formas del bucle que quedaban por probar están medidas arriba; el `for`
es claramente peor, así que el `do/while` con el `a_lp++` suelto al final es la forma
buena — confirmado además por el mapa de líneas del original.

## `CWorldAnimEntity::Init` — dónde está

16 diferencias, 912/912 B. El `lis/addi` de la dirección de `"ROOT"` sube dos ranuras y
sched lo usa de relleno entre los dos `Set*DelayTime`; el original lo tiene detrás, en
`+0x18c`. De ahí sale el cruce de `r9`/`r11`.

Dato nuevo del mapa de líneas que no teníamos: entre `SetLocalDelayTime` (cpp 232) y
`GetAnimPart` + `"ROOT"` (cpp 242) el original **no emite ni una nota**: nueve líneas
seguidas sin código. Sea lo que sea que haya ahí, no son instrucciones (los tamaños
coinciden), pero marca que en el original esas dos sentencias están separadas del
bloque anterior por bastante más que las dos líneas que tenemos.

Cambio aplicado por fidelidad, no por porcentaje: `{ int res = anim_part->Init(skel); }`
pasa a `anim_part->Init(skel);`. El DWARF-1 tiene un bloque anónimo con `int res` para
cada uno de los tres `CreateFnAnimFromNamehash` y **ninguno** aquí, así que el `int res`
y sus llaves eran nuestros.

## Comprobaciones

- `build_direct.py` de las cuatro unidades: `1 ok, 0 fallidas` en las cuatro.
- `censoasm.py --clase BARRERA`: cero barreras y cero pines en los cuatro ficheros del
  lote. Lo que queda en `AnimEntity_WorldEntity.cpp` son dos `asm()` de **datos**
  (huecos anónimos de `.rodata`), fuera de este lote.
- `unitscore.py` de las cuatro: cifras de la tabla de veredictos.
- Ningún fichero tocado fuera de los cuatro del lote; ninguna cabecera compartida.
