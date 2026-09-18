# r65 -- lote `fe`: retirada de andamios en el frontend

Unidades: **zFe**, **zFe2**, **zFeOverlay**. Las tres tienen `fncmp` 0, asi que el
criterio fue el mas duro posible: **digests sha1 POR SECCION de las secciones
ALLOC** (`.text`, `.rodata`, `.data`, `.bss`, `.sdata*`, `.ctors` y sus `.rela.*`),
ignorando `.line`, `.debug*`, `.comment` y `.stab*`. La herramienta esta en
`scratchpad/fe65/sello.py`.

Digests de referencia (compilacion limpia antes de tocar nada):

| unidad | ALLOC |
|---|---|
| zFe | `c6121afa1469c3c8` |
| zFe2 | `d6303dae7eb18785` |
| zFeOverlay | `d41283769e6e02c3` |

**Los tres estan IGUALES al terminar.** La revertida de
`FEngInterfaceFEObjects.cpp` reprodujo `c6121afa1469c3c8` exactamente, lo que
sirve tambien de control de determinismo del compilador.

## Cifras

| | antes | despues |
|---|---|---|
| censo oficial (barreras `__asm__("")` + pines `register asm`) | **17** | **13** |
| pines `register T x asm("rN")` | 11 | 7 |
| barreras `__asm__("" ...)` | 6 | 6 |
| barreras `asm("" ...)` (no contadas en el censo del encargo) | 3 | 2 |
| `asm volatile("lwz ...")` (no contado: no es plantilla vacia) | 2 | 2 |
| locales que el DWARF del original NO tiene | 8 | 7 |

Por fichero (censo oficial):

| fichero | antes | despues |
|---|---|---|
| `Frontend/FEngInterfaces/FEngInterfaceFEObjects.cpp` | 6 pines | 6 pines |
| `Frontend/MenuScreens/Safehouse/customize/CarCustomize.cpp` | 3 barreras + 1 pin | 3 barreras + 1 pin |
| `Frontend/Careers/UnlockSystem.cpp` | 4 pines | **0** |
| `Frontend/MemoryCard/MemoryCardCallbacks.cpp` | 3 barreras | 3 barreras |

## RETIRADA: `FEMarkerManager::ConvertBigBangMarkerAward` (zFe2)

`UnlockSystem.cpp`. Cinco construcciones fuera: **4 pines** (`r25`, `r26`, `r27`,
`r29`) y **una barrera de tres operandos**, mas la falsificacion que las sostenia.

Lo que habia era doble: dos copias pineadas de los parametros (`markerName`,
`partID`) y tres punteros `&unlockType[0].mCampo` recorridos con `[i * 3]`, como
si el array de estructuras fuesen tres arrays paralelos. El DWARF del original
(`0x8017C200 -> 0x8017C28C`) no tiene **ninguna** local aparte de `unlockType[21]`
y del `int i` del `for`, y sus miembros se llaman `MarkerName` / `PartName` /
`Marker`, no `mMarkerName` / `mPartName` / `mMarker`.

La forma que lo sustituye es la obvia --`unlockType[i].MarkerName`-- **con el `||`
partido en dos `if` con su propio `return`**, y eso ultimo es la llave:

```c
for (int i = 0; i <= 20; i++) {
    if (bStrICmp(marker_name, unlockType[i].MarkerName) == 0) {
        if (unlockType[i].PartName == 0) {
            return unlockType[i].Marker;
        }
        if (bStrICmp(partid, unlockType[i].PartName) == 0) {
            return unlockType[i].Marker;
        }
    }
}
```

Por que hace falta el doble `return`, con los numeros del compilador
(`rtldump -dl/-dg` sobre zFe2). Con el `||` en un solo `if`, GCC reduce las tres
direcciones de campo a `base`, `base+4` y `base+8`, y `allocno_compare`
(gcc/global.c) las ordena por `floor_log2(n_refs) * n_refs / live_length * 10000`:

| pseudo | refs | live | prioridad | reg nuestro | reg original |
|---|---|---|---|---|---|
| base (`.MarkerName`) | 5 | 44 | 2272 | r29 | r28 |
| `base+4` (`.PartName`) | 3 | 40 | 750 | r26 | r25 |
| `base+8` (`.Marker`) | 3 | 40 | 750 | r25 | **r29** |

`REG_ALLOC_ORDER` de `rs6000.h` da los preservados en orden `31, 30, 29, 28, 27,
26, 25`, asi que el primero en asignarse se lleva el mas alto. `base+8` empataba
en ultimo lugar y salia en r25: eso rotaba cinco registros (98,286 %, cero
diferencia de tamano pero once filas de reparto).

La clave es que **`REG_N_REFS` suma `loop_depth`, no 1**: una referencia dentro del
bucle vale 2. Con el doble `return`, `.Marker` se lee dos veces a profundidad 2 y
`base+8` pasa de 3 a 5 refs: `floor_log2(5) * 5 / 40 * 10000 = 2500`, que adelanta
al propio puntero base (2272). Con eso el reparto entero cae en su sitio.
Crossjumping une los dos `return`, asi que no cuesta ni un byte.

Medida:

| forma | ALLOC de zFe2 | fndiff |
|---|---|---|
| base (4 pines + barrera + `[i*3]`) | `d6303dae7eb18785` | 100 %, 140/140 B |
| limpia con el `||` | `7a762e368882d8ef` | 98,286 %, 140/140 B, 11 filas |
| **limpia con doble `return`** | **`d6303dae7eb18785`** | **100 %, 140/140 B** |

Las nueve secciones ALLOC coinciden una por una (`.text 5c65ba92ea4e18a3`,
`.rodata 267b9e0edfca7d2f`, `.data 1dfbef4916d854b2`, `.rela.text 3d4adaa498d67889`,
`.bss 1b761d50916e9875`, `.ctors 9069ca78e7450a28`, `.rela.data 99c2ab34ae07d95b`,
`.rela.rodata 5357f56a8bfe60b4`, `.rela.ctors 70d824928865579e`).

## MEDIA RETIRADA: `CustomizeMain::NotificationMessage` (zFeOverlay)

Fuera **una** de las cuatro locales que el original no tiene
(`CarCustomizeManager &mgr = *mgrp;`), usando `mgrp->` directo en los tres sitios.
`d41283769e6e02c3` antes y despues, identico seccion por seccion. Ademas los seis
`CustomizeIsInBackRoom()` de `CustomizeParts::Setup` pasaron al envoltorio miembro
`gCarCustomizeManager.IsInBackRoom()`, que es lo que el DWARF del original expande
(seis inlines); coste medido **cero** (`a6827e1cf5ce6ff6` con y sin, en el arbol
sin pin).

Las tres barreras se quedan, y la r65 deja medido **cuanto** falta:

* **`mgrp`**: sin ella GCC no forma el pseudo de `&gCarCustomizeManager`; lo
  rematerializa dos veces y arrastra ademas el par `lis/ori` del hash del `else`
  (96,6 %). Con `mgrp` usado tres veces --incluida `IsInBackRoom()`, cuyo `this`
  es muerto y no emite nada-- el pseudo ya sobrevive y queda **una sola
  permutacion r31<->r28** (99,876 %, `2e2e60f4e4cbd793`). Prioridades:
  `mgrp` 3 refs / 100 insns = **300**, el bool de `IsInBackRoom` 2 / 31 = **645**.
  La barrera sube `mgrp` a 4 refs y con `floor_log2(4) = 2` la prioridad pasa a
  **800 > 645**. Para retirarla hace falta una CUARTA referencia real a
  `gCarCustomizeManager` en el `case`, y en el `case` solo hay tres.
* **`fe` y `pkg`**: no es reparto, es **orden de emision de argumentos**
  (`INSN_LUID`, desempate de `rank_for_schedule`). Medido por separado, con
  `mgrp` puesto: sin ninguna `09c6801869bf1cbb` (2 filas), solo la de `pkg`
  `3c6787df4c7e26df` (**1 fila**: el `lis r4,0x6d5d` queda delante del
  `lwz r3, cFEng::mInstance`), `fe` como local sin barrera `09c68018` (cero
  efecto). El mismo texto en la rama de arriba SI casa: la diferencia esta en
  `expand_call`, no en el asignador.

## IRREDUCIBLE: `CustomizeParts::Setup` (zFeOverlay) -- pin `r21`

`regmap` con el pin da **IDENTICO** (mismas locales, mismo arbol, mismo reparto).
Sin el pin sigue dando 1708/1708 B, mismo conjunto de locales y mismo arbol: lo
unico que cambia es que `vinyl_group_number` e `installed_index` se **intercambian
r21<->r22** (14 filas, 99,836 %, `a6827e1cf5ce6ff6`).

El pin vale **OCHO PUNTOS sobre 10.000**:

| pseudo | refs | live | prioridad |
|---|---|---|---|
| `vinyl_group_number` | 12 | 1388 | **259** |
| `installed_index` | 4 | 318 | **251** |

Los 12 refs son nueve escrituras (el `= 0` de la declaracion y los ocho `case`),
la lectura del `GetCarPartList` y la del bucle, que pesa 2 por estar a
`loop_depth` 2. Gana `vinyl` por 259 a 251, se asigna primero y se lleva r22.
Basta con quitarle UNA ref a `vinyl` (11 refs -> 237) o darle UNA a
`installed_index` (5 refs -> 314), pero las 12 y las 4 son exactamente las que el
original tiene: los nueve `li r21, N` estan en su `.text` uno por uno.

Probado y negativo:

* mover `installed_index = 0;` detras de los otros tres asignados:
  `f1ccb8e39bc4d751`, 17 filas (peor, y el r21/r22 sigue igual).
* declarar `part_list` arriba, entre `part_found` e `installed_index`, que es el
  orden que trae el DWARF del original: `47ea24ad76ad50aa`, 96,37 % -- el
  constructor de `bTList` se sube al prologo. **El orden de locales del volcado NO
  es el orden textual cuando la local tiene constructor**; eso corrige una lectura
  facil de `regmap`.

## IRREDUCIBLE: `FEngSetScaleX` / `FEngSetScaleY` (zFe) -- 6 pines + 2 barreras

La forma **sin andamio existe y es estructuralmente EXACTA**: `regmap` pasa de
"ESTRUCTURA, 2 locales solo nuestras y 2 desajustes del arbol de bloques" a
**PERMUTACION PURA**. 256 B contra 256 B, las 64 instrucciones en el mismo orden,
`data`/`scale`/`size` en sus registros. Solo r30 y r31 estan cambiados (8 filas,
99,296875 %).

No sale, y esta cuantificado:

| pseudo | refs | live | prioridad |
|---|---|---|---|
| `object` (parametro) | 6 | 50 | **2400** |
| `data` | 3 | 36 | **833** |

Con `REG_ALLOC_ORDER` = `31, 30, 29...`, el primero en asignarse se lleva r31, y
`object` gana por 3 a 1. Para invertirlo `data` necesitaria **5 refs**
(`2*5/36 = 2777`) o `object` un `live_length > 144` (tiene 50). Ni una ni otra se
escriben sin emitir instrucciones: `data` se toca tres veces (el set, el `lfs` de
`size` y el `stfs` de `Size.x`) y **no hay bucle**, asi que no hay peso de
`loop_depth` que explotar --que es justo lo que si hubo en `UnlockSystem`.

Probado y negativo (base zFe `c6121afa1469c3c8`):

* limpia sin nada: `cd9036eb81a5aaee`, 8 filas r30/r31.
* `register FEObjData *data asm("r31")`: `b52ea564ba0fd3d4`, **PEOR y ademas
  MISCOMPILA** -- GCC mete `object` tambien en r31 (`mr. r31,r3` y luego
  `lwz r31,0x2c(r31)`). Un pin sobre una local cuyo rango choca con el parametro
  no es seguro; conviene saberlo.
* `register` a secas sobre `data`: `cd9036eb81a5aaee` (cero efecto).
* `__asm__("" : : : "r31")` entre el null-check y la carga: `0ba5f1f5d1019807`, no
  mueve el reparto.

## IRREDUCIBLE: `MemcardCallbacks::FoundEntry` (zFe) -- 3 barreras

`regmap` dice **ESTRUCTURA**: el original no tiene ni `scr` ni `nm`. Escrito como
el original --`GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault);`-- el
`.o` sale de 512 B, `iGuessSize` en r30 y `fDefault` en r28 **iguales** al
original, y quedan **tres filas** de 128: el par `lis r5,$LC@ha / addi r5,r5,$LC@l`
del literal `""` nos sale delante del `lwz r4,0(r31)` de `mName`, y en el original
detras.

Es **orden de emision**, no reparto. Prioridades de `sched2` empatadas
(`lwz -> call` = 2, `lis -> addi -> call` = 2) y el desempate es `INSN_LUID`: el
original evalua el argumento 1 antes que la direccion del argumento 2 y nosotros
al reves. Ni `n_refs` ni `live_length` tocan eso.

Medido (base zFe `c6121afa1469c3c8`):

* forma limpia, sin locales ni barreras: `908789e798f0679f`, 512/512 B, 3 filas.
  **Es la version con menos deuda de fuente de todas**, y queda anotada por si
  alguien ataca `expand_call`.
* solo `nm` + su barrera (sin `scr`): `a158c3cfd7b8d80d`, **peor** -- 516 B, un
  preservado mas (r27) y marco de 0x20.

## Lo que deja esta ronda para las demas

1. **`REG_N_REFS` suma `loop_depth`**, asi que una referencia dentro de un bucle
   vale 2 y una fuera 1. Es la palanca que cerro `ConvertBigBangMarkerAward`, y
   convierte "esto es una permutacion, pon un pin" en una cuenta con solucion:
   `rtldump -dl` da `n_refs` y `live_length`, y `floor_log2(n)*n/L*10000` dice
   exactamente cuantas referencias faltan.
2. **Un `return` duplicado no cuesta bytes** (crossjumping lo une) y sube en 2 el
   `n_refs` de todo lo que lea dentro del bucle. Es la version barata de "usar la
   variable una vez mas".
3. **`REG_ALLOC_ORDER` de rs6000 es `31, 30, 29, ...`**: el primero en asignarse
   se lleva el registro preservado MAS ALTO. Sirve para leer un diff de
   permutacion al reves y deducir el orden de asignacion del original.
4. **Un pin `register ... asm("rN")` puede miscompilar** si el rango de la local
   choca con un parametro que acabe en el mismo registro (medido en
   `FEngSetScaleX`). No es solo "peor porcentaje".
5. **El orden de locales del volcado DWARF no es el orden textual cuando la local
   tiene constructor** (`part_list` en `CustomizeParts::Setup`). Mover la
   declaracion "para que case con regmap" sube el constructor al prologo.
6. Dos de los cinco irreducibles del lote (`FoundEntry`, las barreras `fe`/`pkg`
   de `NotificationMessage`) **no son de reparto sino de orden de emision de
   argumentos en `expand_call`**. Es una familia distinta, con tres casos ya
   medidos, y ninguna de las palancas del PLAYBOOK la toca.

## Veredicto

**PARCIAL, y con el techo del lote medido.** 17 andamios del censo -> **13**
(-23,5 %), los 4 pines y la barrera de `UnlockSystem` retirados con las nueve
secciones ALLOC identicas, mas una local falsa menos en `NotificationMessage` y
seis llamadas puestas en su envoltorio miembro. Los 13 que quedan estan los cinco
casos **diagnosticados con numeros** y anotados junto al andamio, donde `previo.py`
los encuentra: tres son permutaciones a las que les faltan entre 8 y 1567 puntos de
prioridad que no se pueden fabricar sin emitir codigo, y dos no son de reparto en
absoluto. Ni un solo digest movido.
