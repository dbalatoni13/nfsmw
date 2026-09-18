# El frente de `.data` no es un frente

Tras cerrar 11.632 B del déficit de `.rodata` en siete unidades, lo que quedó a
la vista fue el `.data`: `zAI −384`, `zEAXSound −384`, `zEAXSound2 −288`,
`zFe2 −160`, `zGameplay −128`, `zFe −32`. Parecía el siguiente frente. **No lo
es.**

## La medida

Comparando los símbolos de `.data` del objeto extraído contra el nuestro en las
siete unidades, y descontando dos cosas que no son trabajo:

| unidad | bruto | −contador de declaración | −nombres del troceador | **REAL** |
|---|---:|---:|---:|---:|
| zAI | 428 | 32 | 396 | **0** |
| zEAXSound | 426 | 8 | 418 | **0** |
| zEAXSound2 | 376 | 68 | 308 | **0** |
| zFe | 56 | 16 | 40 | **0** |
| zFe2 | 570 | 408 | 162 | **0** |
| zGameplay | 456 | 304 | 152 | **0** |
| zPhysicsBehaviors | 256 | 8 | 248 | **0** |
| **TOTAL** | **2.568** | **844** | **1.724** | **0** |

**Cero bytes de dato con nombre sin escribir.** Los 2.568 B se reparten en:

- **844 B de renombres**: `unlockType.38610` contra `unlockType.34747` en zFe2
  (252 B), `unlockType.38626` contra `.34763` (144 B)… Mismo símbolo, distinto
  contador de declaración. Ya está bien escrito.
- **1.724 B de nombres del troceador**: `pad_06_80415180_data`,
  `gap_06_80415230_data`, `lbl_80415438`. No son símbolos del programa: `dtk`
  sólo nombra lo referenciado y bautiza el resto.

## Y de esos 1.724 B, la mayoría son CEROS

| hueco | bytes | contenido |
|---|---:|---|
| `zFe gap_06_8041B5DC` | 4 | todo ceros |
| `zFe gap_06_8041B5FC` | 16 | todo ceros |
| `zFe gap_06_8041B63C` | 4 | todo ceros |
| `zAI gap_06_80415230` | 44 | todo ceros |
| `zAI pad_06_80415180` | 172 | con contenido |
| `zAI lbl_80415438` | 68 | con contenido |

Un bloque de ceros en `.data` no es dato que escribir: en C un global
inicializado a cero va a `.bss`, no a `.data`. **Es relleno de alineación**, y
sale solo cuando los objetos de alrededor tienen el tamaño y la alineación
buenos. No se escribe: se consigue.

## La trampa que casi me lleva por delante

El primer diagnóstico con `dolrod.py .data` sobre `zAI` daba esto:

```
FALTA obj 80415272  13  95d4803c95d4803c95d4803c...  ebrakegoals +0x2
SOBRA nue 80415056   1  92e4803c92e4803c92e4803c...  ebrakegoals +0x2
```

Parecía dato ausente en `ebrakegoals`, `coebrakegoals`, `ramgoals`,
`hrblockgoals`, `crossfollowgoals`… una docena de tablas. **No lo era.**

`ebrakegoals[1] = {"AIGoalPursuit"}` es una tabla de `const char *`, y los
valores son **punteros**: el objetivo apunta a `0x803C95D4` y nosotros a
`0x803C92E4`, 752 B antes. **Es la sombra del `.rodata` que todavía falta**, no
un problema de `.data`. Exactamente igual que los 226 B de `.text` de `zSim`,
que eran la sombra de sus 8 B de desplazamiento.

Y de paso, `dolrod` **imputa al símbolo equivocado**: dio «`FALTA` 212 B en
`_12AICopManager.mCopMinSpawnDist +0x0`» cuando ese símbolo está en
`0x80415264`, mide 4 B, y el hueco empieza en `0x80415190`. Nombra al siguiente
símbolo que encuentra. Ya estaba avisado en la memoria del proyecto para
`dolwhere`; vale igual para `dolrod`.

## Qué significa para el reparto

1. **No hay encargo de «escribir datos».** Cero bytes con nombre.
2. Lo que queda de `.data` es **relleno de alineación en su sitio**, y se
   consigue acertando el tamaño y la alineación de los objetos vecinos — o
   emitiéndolo anónimo con un `asm()`, que es como cerraron `libgcc2_4` y
   `pathnode` en la r51. Pero a diferencia del `.rodata`, **aquí la posición
   importa**: los huecos van intercalados entre símbolos vivos, así que el
   `asm()` tiene que ir entre las definiciones correctas, no al final.
3. **Antes de tocar `.data`, hay que cerrar el `.rodata` restante**, porque una
   parte del diff de `.data` es su sombra y desaparece sola.

Es el mismo resultado que `r50-jf-metrica-datos.md` sacó para la métrica global,
ahora confirmado sección por sección en las siete unidades donde se acaba de
trabajar.
