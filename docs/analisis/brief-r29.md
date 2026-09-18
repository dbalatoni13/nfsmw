# Ronda 29 — instrucciones comunes

Estado: **97,81 % matched**, 18.323 funciones, **`linked` 12,18 %** (443 de 599),
`main.dol: OK` (`9619ba57c9919f95f7f2ac951a2166a3517f91e3`).
Quedan **86.260 B en 37 unidades** de near-miss, y **32.472 B de `.rodata` que
FALTAN** en las SourceLists (§2).

## LEE PRIMERO `docs/HERRAMIENTAS.md` y el informe de tu predecesor

---

## 1. LA HERRAMIENTA NUEVA, y reabre vedas de TODO el árbol

**`__asm__("")` es una barrera TOTAL; un `asm` extendido NO VOLÁTIL no lo es.**
En `sched_analyze_2` de GCC 2.95 el camino de «clobber all» se toma con
`ASM_INPUT` (asm **sin** operandos) o con `MEM_VOLATILE_P`. Un asm **con
operandos y sin `volatile`** sólo crea las dependencias de **lo que nombra**:

```
__asm__("")                             barrera TOTAL
__asm__("" : "+r"(x) : : "memory")      ata SOLO a x y a la memoria
```

**Eso permite retrasar UNA instrucción sin tocar a sus vecinas.** Es lo que cerró
`Geometry::SphereVsBox` tras **siete rondas**: `__asm__("" : "+r"(a_lp) : :
"memory")` delante del incremento lo ata al último store y **no toca a los dos
`addi` que PRE inserta, que sólo leen r1**. Sale el orden del objetivo y **no
emite un byte**. zDynamics entera al 100 %.

**COROLARIO: todas las vedas del proyecto que dicen «probada la barrera en la
posición X» se refieren a la barrera TOTAL. El eje selectivo está SIN BARRER.**
Si tu encargo tiene una veda así, **vuelve a probarla**.

Y la otra mitad: a veces **no hace falta `asm` ninguno**. `EPlayRaceNIS` cerró
porque **un `const float` con inicializador constante se PROPAGA y nace en su
USO**, así que el orden de declaración no manda; **quitando el `const`**, cada
constante tiene su `set` y el orden se elige desde la fuente. zMain al 100 %.

## 2. EL FRENTE NUEVO: 32.472 B de `.rodata` que FALTAN

Lo que mueve el DOL **no es el `.text` de más** —eso no llega al DOL, está
medido— sino **`.rodata`/`.data`/`.sdata`**, y ahí las 32 SourceLists **van
CORTAS**: les faltan constantes que el original emite y nosotros no.

| unidad | `.rodata` | `.data` | `.text` (irrelevante) |
|---|---|---|---|
| `zFe2` | **−4.792** | −580 | +13.388 |
| `zWorld` | **−3.952** | +148 | +11.660 |
| `zFe` | **−2.568** | −1.548 | +14.572 |
| `zGameplay` | **−2.368** | −344 | +13.616 |
| `zFoundation` | −1.320 | −220 | **+348** |

`scripts/seccdiff.py` da la tabla; `scripts/extrasym.py`, los símbolos.
**`zFoundation` es el caso limpio**: su código casi casa y lo que le falta son
datos. Y **`zDynamics` está a 286 B de promocionar** (`$LC*` más dos renombres).

## 3. Cinco reglas nuevas, todas extrapolables

1. **El ORDEN de los operandos de un `|`** decide el árbol de `fold`, y el árbol
   decide qué registro acumula (24 permutaciones, 2 min). **Y la forma que suele
   faltar no es una permutación: es sacar `(C|r)` a un TEMPORAL que no sea la
   variable de destino.** *Si el objetivo acumula en un registro que no es el del
   destino, el original tenía un temporal, no un `|=`.*
2. **El ORDEN de declaración de los miembros DENTRO de la clase decide si GCC 2.9
   hace inline** (74,18 % contra 100 %). El volcado da el orden real.
3. **Un `lis sym@ha` izado sobre un `bl` = FALTA UNA CONVERSIÓN.**
   `expand_assignment` llama a la función antes de computar el lvalue **sólo si
   el árbol es un `CALL_EXPR` pelado**; cualquier conversión lo envuelve en
   `NOP_EXPR` y el `lis` sube. Cerró `DataLoadCB` cambiando **una palabra**:
   `extern unsigned int` → `int`. **Los tipos de los `extern` no son cosméticos.**
4. **Un accesor inline EN LA CONDICIÓN bloquea la rotación de `expand_end_loop`**
   (86,28 %); en el cuerpo no hace nada (objeto idéntico).
5. **El orden de las sentencias de un bloque no lo conserva el planificador.**
   Permutar 4 asignaciones independientes son 24 variantes en 2 min: **1,88 pp y
   18 filas** entre el mejor y el peor, y **el orden «natural» era el PEOR**.

## 4. La regla de estructura: SIETE contraejemplos, y cómo leerla

**Mientras quede UNA diferencia de estructura, quitar las otras EMPEORA.** El
único caso a favor quitó **las tres cosas a la vez**. Y **las `// Labels` de
`dwbody` prueban que el DWARF no las NOMBRA, no que no existan** (quitar tres
`goto` por esa lectura costó −1,24 pp; el objetivo **sí** tenía el bloque fuera
de línea). **Las vedas CADUCAN al mejorar el fuente debajo.**

## 5. Herramientas: lo que falla y cómo

- **`lreg.py` está roto por tres sitios. Usa `scripts/alloc.py`** — pero **no
  puede decir nada de un pseudo que no llega a `global_alloc`**: comprueba la
  línea `;; N regs to allocate:` del `.greg` antes de fiarte.
- **`-fsched-verbose-5`, CON GUION.** Con `-dS` el de `sched1` va al `.sched` y el
  de `sched2` a stderr; con `-dR`, el de sched2 va **dentro del `.sched2`**.
- **El volcado DWARF tiene 59 funciones SIN CUERPO** (`ERROR: Failed to process
  tag … GlobalSubroutine`): **«no aparece» ≠ «el original no la tenía»**.
- **La caché de `dwbody.py` se queda rancia tras CADA `build_direct.py`** y sigue
  imprimiendo. Mira la fecha de la cabecera.
- **`triage.py`**: sin `--muro` sólo da el recuento; sin argumentos sólo barre las
  33 SourceLists; **trunca los nombres a 42 caracteres**; y **su columna de
  multiconjunto no es estable entre pasadas**.
- **`pines.py` no ve las barreras con clobber**; **`audit.py` compara VALORES** de
  literal, así que no ve un duplicado.
- **Comparar cuerpos de función en crudo MIENTE**: los campos de `bl` y de
  reubicación **los parchea el enlazador**, hay que taparlos antes.
- **Finales de línea MEZCLADOS** dentro del mismo fichero (`SFXCTL_Engine.cpp`,
  `SubTitle.cpp`, `Geometry.cpp`) y **`UTLVector.h` es CRLF**: ancla línea a línea
  con `splitlines(True)`.

## 6. Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida, y base y medida SEGUIDAS.**
2. **Un barrido que toque una cabecera compartida NO se lanza en segundo plano**,
   y el A/B va sobre los **`.ii` preprocesados**.
3. **Si tu palanca es un constructo que el original no tenía, DILO.**
4. **Antes de dar una unidad por terminada, compara el TAMAÑO DE SUS SECCIONES**
   (`scripts/seccdiff.py`), no sólo su porcentaje.

## Método

- **Verifica tu encargo primero.** Si no reproduce, dilo antes de tocar nada.
- **Ensayos numerados** con su cifra. Si no cierra, **revierte** y anota la veda
  **diciendo qué sentencia barriste**.
- `audit.py` una pasada al empezar; **confirma los FALLA con una segunda**;
  congela al cerrar. Detalle en `docs/analisis/r29-<grupo>.md`. **No commits.**

## Prohibido

- **Escribir ensamblador.** Van diez falsificaciones retiradas. Pero antes de
  llamar falsificación a un `asm`, **cuenta las líneas de DWARF del original en
  su rango**: con `asm` en el original salen 2-4 en cientos de bytes.
- **`configure.py`, `config/GOWE69/*` y `splits.txt`** salvo que tu encargo lo
  diga, y ahí sólo **proponer con verificación**.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).

## Convivencia

Scratchpad con tu prefijo `c29<grupo>_`. **Vigila el disco** (14 GB libres).
