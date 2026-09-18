# `zSim`: el bloqueo son las VTABLES, no el pool de literales

Hecho en coordinación mientras corrían los tres últimos agentes de la r49.
`zSim` no es territorio de ninguno.

## Por qué `zSim`

Es la unidad más cercana del frente que quedó identificado en la r49: 96.400 B
de código al 100 % (`fncmp` **0 de 402**), y la **única** de las 23 cuyas
secciones enlazadas salen **idénticas a la base** en `linkdelta.py`. Todo lo
demás cuadra y aun así `trypromo` da `DOL ROTO (a3bb5b30f68a)`.

## La hipótesis heredada, refutada

El informe `r49-linked.md` situó la causa en el pool de literales: `"FEngHUD"`
se emite **dos veces** —el `$LC481` de GCC más el `asm` a mano de
`LocalPlayer.cpp:118`, dentro de `#ifdef ZSIM_HAND_POOL`—, y la deriva
acumulada de esa permutación sería el `+8` que mueve 72 vtables.

**La duplicación existe pero no es la causa.** Medido:

| | copias de `"FEngHUD\0"` |
|---|---:|
| nuestro `zSim.o` | **2** |
| `zSim.o` extraído | 1 |
| ELF original | 1 |
| nuestro `main.elf` | 1 |

O sea que **el enlazador ya deja una sola copia en los dos lados**. Y
desactivando el blob a mano, el DOL sigue roto: cambia de `a3bb5b30f68a` a
`891512551302`, con el código todavía en 0 de 402 y `lcfix --check` limpio.

## Lo que sí es

`dolwhere.py` da los **5.925 B** que difieren, y están concentrados en
**entradas de vtable**:

```
80405CDE  42 B  _vt.…GarbageNode2ZQ23Sim8Activityi40_9Collector5_Nodei40 +0xE
       obj a044000000008031ab80000000008031ab80000000008028
       nue 9ae8000000008028a014000000008028a01c000000008028
80405D1E  42 B  _vt.…FixedVector3Z…GarbageNode2Z…Activityi40_9Collector5_Nodei40i16 +0xE
       obj 9f900000000080289ddc0000000080289de4000000008028
       nue 9dc0000000008028a014000000008028a01c000000008028
```

Son punteros a función cuya **mitad baja** difiere: las funciones a las que
apuntan están en **direcciones distintas**.

**Y no es una deriva acumulada, es una permutación.** Los deltas tienen signo y
magnitud distintos entre sí —`9f90` contra `9dc0` es −0x1D0; `9e34` contra
`a070` es +0x23C—. Una deriva de `+8` daría el mismo desplazamiento a todas.

## El diagnóstico

`zSim` emite **los mismos cuerpos de plantilla que el original, en otro orden**.
El `.text` mide lo mismo una vez enlazado (`linkdelta` = `+0`) y el código de
cada función es idéntico; lo que cambia es **dónde acaba cada instanciación**, y
las vtables lo delatan porque son la única estructura que guarda esas
direcciones por escrito.

Es la familia de [[nfsmw-inline-es-posicion]]: cómo coloca GCC 2.9 una unidad de
traducción en `finish_file`. El frente no es de código ni de datos, es **de
orden de emisión de plantillas**.

## La permutación exacta: son TRES destructores

`scripts/permorden.py` compara el orden de los símbolos de `.text` de los dos
objetos, quedándose con los 402 comunes —los 158 que sólo emitimos nosotros los
estripa el enlazador y no cuentan—.

| | |
|---|---:|
| funciones comunes | 402 |
| **ya en el orden correcto** | **399** |
| desplazadas | **3** |

Y la cola, lado a lado, lo enseña entero:

| # | objetivo | nuestro |
|---:|---|---|
| 377 | `AllocVectorSpace FV<Model>` | **`dtor FV<Activity>`** |
| 378 | `FreeVectorSpace FV<Model>` | **`dtor FV<Entity>`** |
| 379 | `GetGrowSize FV<…>` | **`dtor FV<Model>`** |
| 380-391 | los accesores, en el mismo orden | los mismos, desplazados +3 |
| 392 | `dtor V<Activity>` | `GetGrowSize V<…>` |
| 393 | **`dtor FV<Activity>`** | `GetGrowSize V<…>` |
| 394 | `dtor V<Entity>` | `GetGrowSize V<…>` |
| 395 | **`dtor FV<Entity>`** | `dtor V<Activity>` |
| 396 | `dtor V<Model>` | `dtor V<Entity>` |
| 397 | **`dtor FV<Model>`** | `dtor V<Model>` |
| 398-401 | idénticos | idénticos |

**El objetivo emite cada destructor de `FixedVector` INMEDIATAMENTE DESPUÉS del
de su `Vector`** —`V<Act>`, `FV<Act>`, `V<Ent>`, `FV<Ent>`, `V<Mod>`, `FV<Mod>`—
y nosotros sacamos los tres `FV` **veinte posiciones antes**, delante de todos
los accesores, dejando los tres `V` sueltos al final.

Todo lo demás —los doce accesores de en medio— está en el mismo orden relativo;
sólo aparece corrido `+3` porque esos tres se han ido de ahí. **Una sola causa,
tres funciones.**

## Y sirve para ordenar el frente entero

La misma medida sobre las demás unidades da el eje de triaje que faltaba:

| unidad | comunes | desplazadas | ciclo mayor |
|---|---:|---:|---:|
| **`zSim`** | 402 | **3** | 13 |
| `zMain` | 1.380 | 32 | 93 |
| `zLua` | 537 | 68 | 50 |
| `zFe2` | 1.290 | 144 | 856 |
| `zAI` | 1.030 | 182 | 326 |
| `zGameplay` | 767 | 270 | 652 |

`zSim` no es sólo «la más cercana»: está a **tres funciones** de tener el orden
del original, y la siguiente está a treinta y dos. Y `zFe2` emite 1.290 de las
1.307 del objetivo: **hay 17 que no emitimos**, que es un problema distinto.

## Lo que hay que medir en la r50

1. **Por qué adelantamos los tres `~FixedVector`.** El destructor de
   `FixedVector` llama al de `Vector`, así que el emparejamiento del objetivo
   —`V` y justo detrás `FV`— sugiere que allí el `FV` se emite *arrastrado* por
   el `V`. En el nuestro salen antes que los accesores, o sea que algo los
   instancia antes. El fuente de `finish_file` está en
   `orig/prodg/NGC_GNU_SRC/NGC/gcc/cp/`: es lectura, no conjetura.
2. Si el arreglo de `zSim` generaliza, `zMain` (32 desplazadas de 1.380) es el
   siguiente.
3. Las 17 funciones que a `zFe2` le faltan.
