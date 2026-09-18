# r76 -- Despineo del frontend (zFe / zFe2 / zFeOverlay)

Fecha: 2026-09-16. Rama `agent/vedas-oraculo-r2`.

Directiva del usuario: **quitar los pines de registro y las barreras `asm`
"como sea"**. Sólo valen para el GCC de GameCube, rompen PS2/X360/PC y esconden
la forma real de las funciones. **Un porcentaje verdadero vale más que un 100 %
falso sostenido por un pin.**

Método de cada pieza:
1. `python scripts/dwarf1.py fn <nombre|0xADDR>` -- locales reales con registro,
   árbol de inlines y bloques léxicos. Si la variable del pin no está, es
   invención nuestra.
2. Quitar el andamio y escribir la forma que dice el DWARF.
3. Barrido de formas plausibles (`build_direct.py` + `fndiff.py`), y **quedarse
   con la de menos FILAS**, no con la de más fuzzy.
4. Ni pin, ni barrera, ni atributos de colocación, ni `#line`, ni función muerta.

Medición: `objdiff-cli diff -c function_reloc_diffs=none -c
ppc.calculatePoolRelocations=false` vía `scripts/fndiff.py`; "filas" = líneas de
diff (`>>>`). El conteo de unidad es `matched_code` (todo o nada por función).

---

## 0. Resultado global

| Unidad | configure.py | .text antes | .text después | Δ |
|---|---|---|---|---|
| zFe | NonMatching | 174.200 / 174.200 B = **100,0000 %** | 172.728 / 174.200 B = **99,1550 %** | −1.472 B |
| zFe2 | NonMatching | 250.732 / 250.732 B = **100,0000 %** | 250.020 / 250.732 B = **99,7160 %** | −712 B |
| zFeOverlay | **Matching** | -- | **NO TOCADA** (ver §7) | 0 |

**Pines y barreras en `src/Speed/Indep/Src/Frontend/` (contados con el patrón
corregido de la r67, el que sí ve la palabra clave corta `asm("")`):**

```
HEAD      : 12 pines + 10 barreras = 22   (en 7 ficheros)
ARBOL r76 :  1 pin   +  3 barreras =  4   (todas en CarCustomize.cpp)
```

Más los **dos `asm volatile("lwz %0, 0x2c(%1)")`** de `FEngInterfaceFEObjects.cpp`,
que ningún censo contaba porque el `volatile` rompe el patrón: **24 piezas → 4**.
Las cuatro que quedan son todas de `CarCustomize.cpp`, que es de `zFeOverlay`
(Matching) y está parada por la regla dura del encargo (§8).

**AVISO PARA EL COORDINADOR**: zFe y zFe2 estaban las dos al **100,0000 % de
`.text`** con los andamios puestos. Las dos siguen marcadas `NonMatching` en
`configure.py`, así que este cambio **no mueve el DOL** (el enlace usa los
`obj/*.o` del original). Pero si alguien pensaba promocionarlas, el precio del
despineo es exactamente **2.184 B de `matched_code` en 7 funciones**. Es la
decisión que el usuario ya tomó por escrito; se documenta para que nadie la
descubra por sorpresa.

Controles hechos:
- `zFe` y `zFe2` recompilados **dos veces** desde el árbol final: mismo sello
  sha1 las dos (`9fc15c43…` y `9b47d584…`). Sin sello no vale la medida
  (`build_direct` puede dejar un `.o` rancio diciendo "1 ok").
- Numeros "antes" medidos recompilando las seis fuentes de HEAD, no leídos de
  un informe viejo.
- Finales de línea preservados: cinco ficheros CRLF y `MemoryCardCallbacks.cpp`
  LF, como estaban. Ningún fichero con caracteres no ASCII.
- `MemoryCardCallbacks.cpp` tiene un `BNEW` (`new (__FILE__, __LINE__)`) en la
  línea 747: la nota nueva se dimensionó para dejar el fichero con **el mismo
  número de líneas** (858 antes y después, BNEW sigue en 52/77/747).

---

## 1. Tabla por función

| Unidad | Función | Tamaño obj | Antes | Después | Filas | Bytes nuestros | Andamio retirado |
|---|---|---|---|---|---|---|---|
| zFe | `FEngSetScaleX` | 256 | 100 % | **99,296875 %** | 8 | 256 | pin r30 `object_reg` + pin r31 `data` con `lwz` a mano + pin fr13 `initialScale` + barrera `"+f"` |
| zFe | `FEngSetScaleY` | 256 | 100 % | **99,296875 %** | 8 | 256 | las mismas cuatro piezas |
| zFe | `cFEng::PushErrorPackage` | 332 | 100 % | **96,86747 %** | 14 | 324 | 2 pines r0 `wasPaused` |
| zFe | `SubTitler::GetElapsedTime` | 116 | 100 % | **95,51724 %** | 5 | 112 | pin fr1 `thetime_ms` + barrera `"+f"` |
| zFe | `MemcardCallbacks::FoundEntry` | 512 | 100 % | **97,96875 %** | 3 | 512 | 3 barreras `"+r"` (`nm`, `iGuessSize`, `fDefault`) + 2 locales inventadas |
| zFe2 | `FEKeyboard::ToggleCapsLock` | 92 | 100 % | **91,08696 %** | 3 | 92 | pin r10 `off` + barrera `"+r"` |
| zFe2 | `ArrayScroller::ScrollVer` | 620 | 100 % | **96,29032 %** | 54 | 616 | pin r11 `original_index` |

**Diez de las veinte piezas retiradas nombraban variables que el DWARF del
original NO tiene**: `object_reg` (×2 funciones), `initialScale` (×2),
`wasPaused` (×2 ramas), `off`, `original_index`, `scr`, `nm`. No eran "la forma
que falta": eran nuestras.

Las otras diez sí nombraban variables reales (`data`, `thetime_ms`,
`iGuessSize`, `fDefault`) y lo único que hacían era **cambiarles el registro**;
y los dos `asm volatile("lwz %0, 0x2c(%1)")` suplantaban un inline
(`GetObjData`) que el propio DWARF lista en su árbol.

---

## 2. `FEngInterfaceFEObjects.cpp` -- `FEngSetScaleX` / `FEngSetScaleY`

El fichero con más andamio del árbol: **8 piezas**, cuatro por función.

DWARF (`dwarf1.py fn FEngSetScaleX`, 0x80131A70, 256 B):

```
param object r30 | param x f31 | local data r31 | local scale f13
local size f30   | local SizeEpsilon (optimizado fuera)
INLINE GetObjData en 0x80131A94 (0 B)
bloque {0x80131AE4} con pTex (optimizado fuera)
```

El `INLINE GetObjData` del propio DWARF es lo que el `asm volatile("lwz %0,
0x2c(%1)")` estaba suplantando. Y `scale` **ya vive en f13 sin pin**: el `stfs
f13` sale igual. `size` cae sola en f30.

Lo único que falta son **8 filas ARG_MISMATCH y son una permutación pura
r30/r31**: el objetivo pone `object` en r30 y `data` en r31, nosotros al revés.
Está calculado con los números del propio compilador (`global.c:allocno_compare`
sobre volcados `-dg/-dl`):

```
object = 6 refs / 50 insns -> floor_log2(6)*6/50*10000 = 2400
data   = 3 refs / 36 insns -> floor_log2(3)*3/36*10000 =  833
```

y `REG_ALLOC_ORDER` de `rs6000.h` da los preservados en el orden 31, 30, 29…:
el de más prioridad se lleva r31 y `object` gana por 3 a 1. Para invertirlo
`data` necesita 5 refs (2\*5/36 = 2777) o `live_length <= 12`, u `object`
`live_length > 144` (tiene 50). Ninguna de las tres se escribe en C sin emitir
instrucciones que el objetivo no tiene.

**Seis formas medidas, todas sin andamio y TODAS el mismo objeto**
(99,296875 %, 256/256 B, las mismas 8 filas):

| | forma | resultado |
|---|---|---|
| V0 | la del DWARF (aplicada) | 99,296875 %, 8 filas |
| V1 | el store repetido en `FE_String` y en `FE_Group` | igual -- jump2 hace cross-jumping y funde los dos `stfs`: las refs de `data` **no suben** |
| V2 | `object->Flags = object->Flags \| FF_DirtyCode;` | igual |
| V3 | `size` declarada antes que `scale` | igual |
| V4 | `FEObjData &data = *object->GetObjData();` | igual |
| V5 | `data` declarada antes del null-check | igual |

Histórico con el andamio puesto (r65/r67, **no repetir**): `register FEObjData
*data asm("r31")` **miscompila** (mete `object` también en r31); `register` a
secas y `__asm__("" : : : "r31")` no mueven el reparto.

---

## 3. `FEngInterface.cpp` -- `cFEng::PushErrorPackage`

Dos pines idénticos, uno por rama del `if (FEDatabase == nullptr)`. Retirados
los dos; la forma es `bWasPaused = true;`.

- **100 % → 96,86747 %**, 324 B contra 332 B del objetivo (8 B menos), 14 filas.
- Mecanismo (r67, reconfirmado): sin el pin, CSE (la unidad lleva
  `-fcse-follow-jumps` y `-fcse-skip-blocks`) ve que r10 ya vale 1 desde
  `SetErrorScreen`/`ToggleErrorScreenMode`, sustituye la constante por ese
  registro y **borra el `li r0,1` que el objetivo sí tiene**; ese uso de más
  alarga la vida del pseudo y de rebote cruza r9/r11 en las cargas de
  `mPauseRequest` y `mFEng`.
- Formas medidas y negativas (las tres dan EXACTAMENTE el mismo objeto): P0
  `bWasPaused = true;`, P1 `if (!(FEManager::IsPaused() && !bWasPaused))`,
  P2 `if (FEManager::IsPaused() == false || bWasPaused != false)`.
- Pista sin cerrar: el DWARF cierra el primer bloque anónimo en 0x801339B4,
  justo en el `li r0,1`, y el del `else` llega al final de la función.

---

## 4. `SubTitle.cpp` -- `SubTitler::GetElapsedTime`

DWARF (0x80136C24, 116 B): `this` r31, `timenow` r30 (unsigned int) y
`thetime_ms` float **sin ubicación** (optimizada fuera), y **cero bloques
léxicos**. O sea que el temporal `t` que había dentro del `if` **tampoco
existe**: abría un bloque léxico que el original no tiene.

Aplicada la forma del DWARF (dos locales de ámbito de función, sin `t`):
**100 % → 95,51724 %**, 112 B contra 116 B, **5 filas**.

Lo que falta: el objetivo guarda el valor calculado y el devuelto en **dos
pseudos** (`fmadds f0` + `fmr f1,f0` + `stfs f0`) y nosotros los fundimos
(`fmadds f1` + `stfs f1`). El `fmr` de más arrastra las dos filas de f12/f13.
Es *coalescing*, no reparto.

| | forma | resultado |
|---|---|---|
| S5 | la del DWARF, sin `t` (aplicada) | **95,517 %, 112 B, 5 filas** |
| S0 | S5 con el temporal `t` dentro del `if` | 95,517 %, 112 B, 5 filas -- **el temporal no basta**: GCC coalesce igual sin la barrera |
| S2 | `thetime_ms = diff; timeElapsed += thetime_ms * 0.001f;` | 94,827 %, 120 B, 8 filas |
| S3 | S2 con `timeElapsed = timeElapsed + …` | 94,827 %, 120 B, 8 filas |
| S6 | `timeElapsed += bGetTickerDifference(…) * 0.001f;` | 94,827 %, 120 B, 8 filas |
| S7 | S2 con las ramas invertidas | 67,586 %, 112 B, 14 filas (invierte el layout) |

Nótese que S5/S0 tienen **menos fuzzy que la r36c** pero **menos filas** que
S2/S3/S6, que son las que la ronda anterior había marcado como "la mejor sin
andamio" mirando sólo el porcentaje.

---

## 5. `FEpkg_MU_Keyboard.cpp` -- `FEKeyboard::ToggleCapsLock`

**Corrección a la nota que había en el fuente**: decía que la función *"NO está
en el volcado DWARF original"*. Sí está: `dwarf1.py fn 0x8015300C` (92 B) y
lista **únicamente `this` en r3** -- cero locales, cero inlines, cero bloques.
El `bool off` era invención nuestra.

**Este es el caso donde el fuzzy y las filas se contradicen**, y manda el conteo
de filas:

| | forma | fuzzy | bytes | filas |
|---|---|---|---|---|
| **T1** | la del DWARF, `mbShift = false;` en su sitio (aplicada) | 91,087 % | 92 | **3** |
| T15 | T1 con return temprano | 91,087 % | 92 | 3 (objeto idéntico) |
| T2 | con un `bool off = false;` inventado delante | **98,261 %** | 92 | **8** |
| T12 | `mbShift = false;` detrás del `if` de MODE_FILENAME | 71,174 % | 100 | 16 |
| T14 | `mbCaps = !mbCaps;` en vez de `mbCaps != 1` | 76,609 % | 80 | 12 |

T2 sube 7 puntos de fuzzy y **empeora**: acierta el orden de emisión y a cambio
manda `{load, bool, mode}` a r0/r11/r9 y descoloca tres registros más. Se
descarta por filas y porque su local no existe en el DWARF.

Lo que falta en T1 (3 filas, **mismo número de instrucciones**): el cero va a
r10 en el objetivo y a r0 en el nuestro, y de rebote el `stw 0x3c` se emite una
ranura antes. r0 es el primer GPR de `REG_ALLOC_ORDER` (0, 9, 11, 10…) y el
pseudo del cero sólo lo evita si nace antes del `cmpwi r0,1` de `mbCaps`; toda
forma C con una única def constante la propaga `cse` al uso y la def renace en
el *join* con r0 libre.

---

## 6. `feArrayScrollerMenu.cpp` -- `ArrayScroller::ScrollVer` (la más cara)

DWARF (0x80174474, 620 B): `this` r29, `dir` r30, `new_datum` r26, `new_index`
r31, `new_start` r27, y sólo `i` (r30) y `start` (r5) dentro de sus bloques.
`original_index` **no existe**, y `width`/`height` **no son locales**: son
miembros (offsets 0x1C y 0x20).

**100 % → 96,29032 %**, 616 B contra 620 B, **54 filas**. Es la pieza más cara
del lote.

Pero **la diferencia real es UNA instrucción**; las otras 53 filas son la
cascada de reparto que arrastra. El objetivo hace `mr r11,r31` justo antes del
`cmpwi r30,-1` (copia de `new_index` viva sólo en la rama `eSD_NEXT`), y sin
ella el asignador recoloca medio marco: `this` r29→r31, `dir` r30→r28,
`new_index` r31→r29 y el `addi …,0xc` r31→r30.

| | forma | resultado |
|---|---|---|
| A0 | `new_index = new_index + width;` (aplicada) | 96,290 %, 616 B, 54 filas |
| A1 | `new_index += width;` | idéntica |
| A3 | `new_index = new_index + GetWidth();` | idéntica |
| A2 | `new_index = GetCurrentDatumNum() - 1 + width;` | 87,839 %, 624 B, 79 filas -- `GetCurrentDatumNum` es `TraversebList`, una llamada real: no se comparte |

**Dónde mirar si alguien vuelve** (leído en el objetivo, r67): r11 no es sólo
esa copia. Tras el `cmpwi r30,-1` las dos ramas hacen `lwz r0,0x1c(r29)` +
`mr r11,r0` y usan r11 como *width* en todo el bloque -- dos pseudos de vidas
disjuntas comparten r11. Esa carga con copia **ya la emitimos igual**. El
objetivo es el origen del pseudo de WIDTH, no la copia de `new_index`.

---

## 7. `MemoryCardCallbacks.cpp` -- `MemcardCallbacks::FoundEntry`

Tres barreras `"+r"` y **dos locales que existían sólo para tener algo que
atar** (`scr`, `nm`). El DWARF (0x80139CFC, 512 B) da la razón a la forma
limpia: `this` r29, `info` r31, y de locales sólo `g_GC_Disk_GameName`, `iSize`
(optimizada fuera), `iGuessSize` **r30** y `fDefault` **r28** -- los registros
del original, **sin barrera ninguna**.

Aplicado `GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault);`:
**100 % → 97,96875 %**, 512/512 B, **3 filas**.

Lo que falta son 2 instrucciones de 128: el par `lis r5,$LC@ha` / `addi
r5,r5,$LC@l` del literal `""` sale delante del `lwz r4,0(r31)` de `mName`, y el
objetivo emite `lwz → lis → mr r6 → mr r7 → addi`. Es **orden de emisión**, no
reparto, y el diagnóstico está cerrado: prioridades de sched2 empatadas y
desempate por `INSN_LUID`; el `high` nace en `precompute_register_parameters`
(`calls.c:652`) porque con `-fforce-addr` la dirección del literal se fuerza a
registro (`expr.c:8110`) **antes** de que `load_register_parameters` cree el
`lwz`. Un literal siempre pasa por *precompute*, haga lo que haga la fuente.
Quien vuelva tiene que atacar `expand_call`.

---

## 8. PARADA: `CarCustomize.cpp` (zFeOverlay es **Matching**)

`configure.py` marca `Speed/Indep/SourceLists/zFeOverlay.cpp` como **Matching**
(con `section_rename=".text=.over"` y `-fno-implicit-templates`). Cualquier
cambio que no mantenga el 100 % **movería el DOL**, así que —por la regla dura
del encargo— **no se ha aplicado nada** y el fichero queda intacto. Queda para
que lo decida el coordinador:

| Función | Andamio | Qué dice el DWARF |
|---|---|---|
| `CustomizeMain::NotificationMessage` (808 B) | 3 barreras `"+r"` sobre `mgrp`, `fe`, `pkg` (`:2066,2094,2096`) | pendiente de mirar |
| la del `switch (Category)` (`:2780`) | pin r21 `vinyl_group_number` | **la variable SÍ existe** en el original; el pin sólo le cambia el registro (r22 → r21) |

La nota r67 del propio fichero trae el margen calculado: sin el pin,
`vinyl_group_number` (12 refs / 1388 insns → 259) se lleva r22 e
`installed_index` (4 refs / 318 insns → 251) se queda r21. Para invertirlo harían
falta 13 instrucciones menos en el bucle o 44 más delante de
`installed_index = 0`; no hay forma legítima que las mueva sin mover un byte.
**Traducción: si se despina, esta unidad deja de casar y hay que degradarla a
`NonMatching`.** Es una decisión de coordinación, no de agente.

---

## 9. Control de portabilidad (`tools/scratch/x360_sonda/sonda.py`)

| Fichero | Antes | Después |
|---|---|---|
| `FEngInterfaces/FEngInterface.cpp` | GCCASM | **OK** |
| `SubTitle.cpp` | GCCASM | **OK** |
| `MenuScreens/InGame/FEpkg_MU_Keyboard.cpp` | GCCASM | **OK** |
| `MenuScreens/Common/feArrayScrollerMenu.cpp` | GCCASM | **OK** |
| `FEngInterfaces/FEngInterfaceFEObjects.cpp` | GCCASM | **OTRO** -- ya no falla por `asm`; el primer error es `C3861 'FEngGetCenterX': identifier not found` en `:726`, un problema de orden de declaración del árbol, preexistente |
| `MemoryCard/MemoryCardCallbacks.cpp` | GCCASM | **GCCASM**, pero **ya no por este fichero**: el error viene de `MemoryCard/MemoryCardHelper.hpp:17` (`__asm__` en una cabecera compartida, fuera del alcance del encargo) |
| `.../customize/CarCustomize.cpp` | PLATAFORMA | PLATAFORMA (intacto) |

**Cuatro ficheros pasan a OK y ninguno empeora.** Los dos que no llegan a OK no
lo hacen por andamio propio.

---

## 10. Para el cierre de ventana

- **No he commiteado, ni corrido `ninja`, ni tocado `configure.py`, `config/`,
  `keep.lst`, `build.ninja` ni ninguna cabecera compartida.**
- No he añadido ni quitado ningún literal de cadena ni ninguna constante nueva,
  así que **no debería haber corrimiento de `$LC`**; aun así, `lcfix.py --check`
  es del jefe y toca correrlo igual, leyendo las líneas `FALLO` una a una.
- Si el coordinador decide **promocionar zFe o zFe2**, este cambio se lo impide:
  estaban al 100,0000 % y ahora están al 99,155 % y 99,716 %. La alternativa es
  revertir las siete funciones, que es exactamente lo que la directiva dice que
  no se haga.
- Los andamios retirados y sus medidas están **junto a cada función en el
  fuente**, no sólo aquí: `previo.py <función>` los encuentra.
