# r76b — zFeOverlay: los cuatro andamios de CarCustomize.cpp

Fichero único del lote:
`src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.cpp`

**Los cuatro andamios están RETIRADOS.** `censoasm.py --clase PIN` y `--clase BARRERA`
ya no devuelven ninguna línea para este fichero. No se ha tocado ningún otro fichero
(`git diff --stat` = 1 fichero, 63 inserciones / 119 borrados).

**zFeOverlay YA NO REPRODUCE: hay que degradarla a NonMatching.** Medido con
`fncmp.py Speed/Indep/SourceLists/zFeOverlay`:

| | funciones con el código distinto | bytes |
|---|---|---|
| antes | **0 de 467** | 0 |
| después | **2 de 467** | 2 516 |

Las dos son las del lote. Ninguna otra función de la unidad se ha movido: no hay
desplazamiento de `$LC` ni efectos colaterales.

---

## Tabla

| función | andamio | oráculo | % antes | % después | nuestro/objetivo | formas descartadas | veredicto del cruce `.line` |
|---|---|---|---|---|---|---|---|
| `CustomizeParts::Setup`<br>`Setup__14CustomizeParts` | `register unsigned int vinyl_group_number asm("r21")` (:2780) | la local **SÍ existe y SÍ vive en r21**: el pin no inventaba nada | **100,000 %** | **99,836 %** | 1708 / 1708 B | fundir las dos `SetInitialOption` → 96,059 % y 1660 B · declarar las cuatro locales tras `part_list` (orden del DWARF) → `.o` IDÉNTICO · [r65/r67] mover `installed_index = 0` → peor, 17 filas · `part_list` arriba → 96,37 % | **INTACTO.** Mapa de líneas byte a byte idéntico con y sin pin (236 offsets, 48 discordantes, 264 notas del original contra 308 nuestras, las mismas en las dos versiones). Es la misma sentencia; el pin sólo elegía registro. |
| `CustomizeMain::NotificationMessage`<br>`NotificationMessage__13CustomizeMainUlP8FEObjectUlUl` | tres barreras: `mgrp` (:2066), `fe` (:2094), `pkg` (:2096) — y con ellas la local `mgrp` | el bloque anónimo `0x803B9358-0x803B9428` **no lista NINGUNA local** | **100,000 %** | **96,609 %** | 808 / 808 B | conservar `mgrp` sin barreras → 97,896 % · `if (!IsCareerMode())` con el `else` delante → 88,564 % · `if (IsInBackRoom()) { SwitchRooms(); return; }` → 93,589 % (94,876 % con `mgrp`) · sacar `RelinquishControl` del bloque del `const` → refutada: el rango del bloque del original llega hasta el `b` final | **El 100 % era FALSO en mapa de líneas.** Ver abajo. |

Sellos: base `2c7f412eccf3`, final `edf494f236b1`.

---

## El 100 % de NotificationMessage no reproducía el mapa de líneas

El cruce de `.line` contra `symbols/debug_lines.txt`, en la función entera
(`0x803B925C`, 808 B):

| forma | % | notas `.line` (original: **72**) |
|---|---|---|
| 3 barreras + `mgrp` | 100,000 % | **78** (seis de más) |
| sin barreras, con `mgrp` | 97,896 % | 75 |
| sin barreras ni `mgrp` (**entregada**) | 96,609 % | **74** |

Acotado al `case 0x911ab364` (`[+0xfc, +0x1cc)`, original: **31** notas):
37 → 34 → **33**.

Las seis notas de más de la forma al 100 % están exactamente donde estaban los
andamios: dos en `+0x104` (la declaración de `mgrp` y su `__asm__`), una en
`+0x1a4`, y las de `+0x1ac` y `+0x1b0` (`fe`, `pkg` y sus dos `__asm__`).
Retirar los andamios **quita tres notas espurias del mapa de líneas**: el fuente
se acerca al original aunque el código se aleje.

### Por qué `mgrp` también sale

Es la evidencia más limpia de la ronda, y tiene precisión de una instrucción.
En `+0xfc` (`lis r9, gCarCustomizeManager@ha`) el original pone dos notas: la del
`case` y **la de la línea del `if`**; y **no tiene ninguna nota en `+0x104`**,
que es el `mr r3, r31`. O sea: en el original, el `lis`/`addi`/`mr` pertenecen
enteros a la sentencia `if (gCarCustomizeManager.IsCareerMode())`. Una
declaración propia de `mgrp` parte eso en dos y obliga a la nota de `+0x104`,
que es justo lo que nos sobraba. El DWARF ya decía que el bloque no tiene
locales; el mapa de líneas lo confirma instrucción a instrucción.

**PROPUESTA para el jefe (reversible en una línea):** volver a poner
`CarCustomizeManager *mgrp = &gCarCustomizeManager;` y usar `mgrp->` en las tres
llamadas recupera **+1,287 pp** (96,609 → 97,896 %) sin ningún `asm` y sin nada
que rompa PS2 ni Xbox 360. Se ha entregado **sin** la local porque el DWARF y el
`.line` coinciden en que el original no la tiene, y el protocolo del lote dice
que la forma real es la que no lleva la variable inventada. Si se prefiere el
margen a la fidelidad, el cambio es de cuatro líneas.

### Lo que se pierde, que son dos averías independientes

1. **Reparto (`mgrp`).** Sin la local, GCC no forma el pseudo de
   `&gCarCustomizeManager` y lo rematerializa dos veces. Prioridades del
   asignador (`rtldump .greg`, medidas en r65/r67):
   `mgrp` 3 refs / 100 insns → 300, contra el bool de `IsInBackRoom`
   2 refs / 31 insns → **645**. Gana el bool y se lleva r31. Haría falta una
   **cuarta referencia real** a `gCarCustomizeManager` en el `case`, y en el
   `case` sólo hay tres (`IsCareerMode`, `IsInBackRoom` —cuyo `this` es muerto—
   y `RelinquishControl`).
2. **Orden de emisión (`fe`/`pkg`).** En el `else`, el par `lis r4 / ori r4` del
   hash se adelanta al `lwz` de `cFEng::mInstance`. No es reparto: **el mismo
   texto en la rama de arriba sí casa**, y lo que cambia entre las dos ramas es
   el tamaño del bloque básico. Es `expand_call`.

---

## Hallazgo colateral (fuera de lote, para quien coja `CustomizeParts::Setup`)

En el cruce de `Setup` hay cinco offsets donde **al original le sobran notas
respecto a nosotros**, que es la señal contraria a la habitual y significa que
al original le caben más sentencias ahí:

```
+34c   orig[2] CarCus:3941 CarCus:3944          nuestro[1]
+4e8   orig[3] CarCus:3983 CarCus:3984 CarCus:411   nuestro[1]
+500   orig[3] CarCus:3985 Custom:118 CarCus:3987   nuestro[1]
+520   orig[2] Custom:118 CarCus:3990           nuestro[1]
+58c   orig[2] CarCus:4003 CarCus:4004          nuestro[1]
```

El `CarCus:411` de `+0x4e8` es una **inline del propio CarCustomize.cpp**
expandida dentro del bucle que nosotros no expandimos ahí. `Setup` casa al
99,836 % y esto no cambia un byte hoy, pero es una pista concreta de una
inline que falta, y está a un `grep` de la línea 411 del fichero original.

## Métodos

- Oráculo: `python scripts/orac.py fn CustomizeParts::Setup` y
  `fn CustomizeMain::NotificationMessage`.
- Test de barrera muerta sobre las tres líneas: **las tres VIVAS**
  (`2c7f412eccf3` → `cb967b1bf7a8` / `698dc28c1a94` / `802b089a9e19`).
  Ninguna salía gratis.
- Medida: `build_direct.py zFeOverlay` + `fndiff.py`, con sello sha1 del `.o` en
  cada paso.
- Cruce `.line`: lector propio sobre la sección `.line` de nuestro `.o` contra
  `symbols/debug_lines.txt`, acotable por rango.
