# r67b — lote `audio-plat`: 20 barreras cortas, 5 retiradas, 15 irreducibles con diagnóstico

**Encargo**: las barreras con la palabra clave corta `asm("" ...)` de seis ficheros de zEAXSound y
zPlatform (las dos unidades pendientes). Test único: digest de las secciones ALLOC por sección
(`.text`, `.rodata`, `.data`, `.bss`, `.sdata*`, `.ctors` y sus `.rela.*`), ignorando `.line`,
`.debug*`, `.rela.debug*`, `.comment` y `.stab*`, más `.symtab`/`.strtab`. Idéntico → se queda.
Distinto → no entra.

## 1. Cifras

| | |
|---|---|
| barreras cortas antes (censo sin comentarios, HEAD) | **20** |
| barreras cortas después | **15** |
| **retiradas** | **5** (4 en SFXCTL_3DObjPos.cpp, 1 en EAXAemsManager.cpp) |
| irreducibles con nota `r67b` junto al andamio | **15** (todas las que quedan) |
| pines | 2 → 2 (no se ha convertido ninguna barrera en pin ni en otra forma de asm) |
| asm con instrucciones | 7 → 7 (5 del clon bSqrtNoCJ, 2 `asm volatile` de JoyE) |
| compilaciones medidas | 18 de prueba + 4 de sello (prístino y final) en zEAXSound/zPlatform, 26 de la propuesta, 4 volcados RTL |
| regresiones | **0**: ALLOC, `.symtab` y `.strtab` idénticos en zEAXSound y zPlatform |
| propuestas en ficheros ajenos | 1, medida en las 14 unidades que incluyen la cabecera (§5) |

Censo (`scratchpad/audioplat67b/censo2.py`, quita los comentarios antes de contar):

| fichero | barreras | pines | asm-instr |
|---|---|---|---|
| SFXCTL_3DObjPos.cpp | 6 → **2** | 0 → 0 | 5 → 5 |
| EAXAemsManager.cpp | 5 → **4** | 0 → 0 | 0 → 0 |
| SFXCTL_AccelTrans.cpp | 1 → 1 | 1 → 1 | 0 → 0 |
| SFXCTL_NISReving.cpp | 1 → 1 | 0 → 0 | 0 → 0 |
| JoyE.cpp | 6 → 6 | 1 → 1 | 2 → 2 |
| TextureInfoPlat.cpp | 1 → 1 | 0 → 0 | 0 → 0 |

## 2. Método (`scratchpad/audioplat67b/`)

Como en la r67, **nunca compilé contra el árbol real**. Seis árboles privados (`tree`, `tree2`…`tree6`)
copiados de `src/` + `include/` y validados contra el índice: `git --work-tree=tree diff HEAD` da
**0 líneas**. Las bases privadas reprodujeron las de la r67: zEAXSound `9e92525a5436ae95`, zPlatform
`7411942baadf95ea`. Cada prueba va en su propio árbol, así que se miden en paralelo sin pisarse.

* `pr.py <spec> --tree N`: reemplazos por líneas en el árbol N, compila, compara con la base, lista las
  secciones distintas y las filas de objdiff de la función, y revierte siempre.
* `repl.py`: los reemplazos van **por líneas** y conservan el final de cada una. SFXCTL_3DObjPos.cpp
  mezcla LF y CRLF (las 8 líneas de las barreras de los cases eran LF dentro de un fichero CRLF): el
  reemplazo por bytes de `prueba.py`/`rtl.py` de la r67 no casa ahí.
* `final.py tree|real`: aplica lo aceptado y las notas al árbol privado limpio; `real` exige que cada
  fichero real siga siendo el prístino, escribe y comprueba que queda igual byte a byte al privado.
* `rtl.py --tree N --spec`: `.lreg`/`.greg` de una variante; `scripts/alloc.py` para la tabla de allocnos.
* `lm.py`: mapa de líneas del original por símbolo.

Ninguno de mis seis ficheros usa `BNEW`/`__LINE__`/`__FILE__`: las líneas de nota no mueven código.

## 3. Retiradas

| función | barreras | forma que las retira | medida |
|---|---|---|---|
| `EAXAemsManager::RegisterSlots` | `"+r"(SPUMainAllocsEnd)` | DWARF (sin esa local) + orden del mapa de líneas | IDÉNTICO |
| `SFXCTL_3DObjPos::GenerateSinglePlayerMix` | 2 × `"+f"(fDistToObj)` | ninguna: sobraban | IDÉNTICO |
| `bSqrtNoCJ` (clon local) | 2 barreras vacías | ninguna: sobraban | IDÉNTICO |

* **RegisterSlots.** El DWARF del original solo tiene `NewSlot`, `n` y `DebugString`. El mapa de
  líneas da el orden 2125 `Index`, 2126 `Type`, 2127 `MAINmemLocation`, 2128 `MAINmemSize`, 2129
  `pLastAlloc`, 2131 `m_SPUMainAllocsEnd -= SizePerSlotSPU`, 2133 `BANKmemLocation`, 2134 `BANKMemSize`,
  2135 `pAssetParams`. Escrita así, sin la local y sin la barrera: idéntica. `pNewSlot` **sí** hace falta
  (sin ella y con `NewSlot.Clear()`: 6 filas): hace de `this` del constructor inline de `stBankSlot`, que
  en el original llama a `Clear()` (el mapa pone SndDataParams.hpp 273-274 en el `addi r3,r1,8` y en el
  `bl Clear`). Ver la propuesta del §5.
* **GenerateSinglePlayerMix.** Las dos `"+f"(fDistToObj)` (op2) y las dos barreras vacías del clon
  (op3) dan el objeto idéntico **por separado y juntas** (op23). Venían del commit 4e8d4944 junto a
  los clobbers de r9: el clobber era lo único que sostenía el reparto.

## 4. Irreducibles (nota `r67b` junto a cada andamio)

| función | andamios | intentos r67b | diagnóstico |
|---|---|---|---|
| `EAXAemsManager::SetupNextLoad` | 4 barreras `"+r"(x) : "r"(this)` | 2 + RTL | ver abajo |
| `GenerateSinglePlayerMix` | 2 clobbers `: : : "r9"` | 1 + RTL | sin ellos, 4 filas: `lis r9` en vez de `lis r11` |
| `SFXCTL_Physics::UpdateNIS` | 1 barrera `"+r"(current_state) : : "r9"` | 3 + RTL | tres formas, un solo objeto |
| `SFXCTL_AccelTrans::UpdateParams` | 1 barrera (+ el pin de la r66) | 1 | la barrera es la que da efecto al pin |
| `TextureInfoPlatInterface::LockPalette` | 1 barrera `"+r"(Pal32)` | 3 | tres formas, las mismas 3 filas |
| `ActualReadJoystickData` | 5 de relleno + la `"=r"(guard)` | 0 nuevos | escalera r61 y palanca (e) r67 ya medidas |

**SetupNextLoad.** El DWARF del original no trae esta función ("Failed to process tag E5B398"). El mapa
de líneas da la forma sin locales: 912 puntero a NULL, 913 if, 915 `++`, 916 puntero, 919
`DeleteRefToAsset`, 922 `RemoveBankListing`, 925 recursión. Escrita así (la de antes de e66b11e2 con la
cabecera actual): `a03b55692ad0849f`, 17 filas, `this` en r27. Quitando solo las barreras y dejando las
locales sale **el mismo digest**. En el `.greg`, `this` (pseudo 82) tiene 12 refs en 241 insns →
prioridad 1493, por debajo de las dos givs de `deleteCount` del inline `DeleteRefToAsset` de
EAXAemsManager.h (6 refs en 58 insns → 2068), que se quedan r29 y r28. Con 16 refs, `floor_log2` sube
de 3 a 4 y la prioridad a 2612, la de la base: son exactamente las cuatro entradas `"r"(this)`. Desde el
`.cpp` no hay cuatro referencias reales más; si hay palanca, está en la vida o en las refs de esas
givs, o sea en la cabecera (no es mía).

**GenerateSinglePlayerMix, r9.** Sin los dos clobbers: `25efe5aec5419b6d`, 4 filas. En el `.lreg`, la
pseudo de `high($LC983)` (el 100.0f que carga la unión del Newton-Raphson antes de saltar a la cola
común de `SetDMIX_Input(0)`) es local de bloque, con 2 usos en 6 insns, y sin el clobber tiene r9
libre. En TPMIX_AVE_CAM_CAR, con el bSqrt de bMath.hpp, sale r11 sin ayuda. El clon no sobra: con el
`bDistBetween` de bMath.hpp en los cuatro cases, que es lo que lista el DWARF, salen 98 filas (74,9 %,
`a5f5e264d54f7354`).

**UpdateNIS.** El DWARF no tiene `current_state` y sí la etiqueta `Continuing` en +0x270 (el switch).
(1) Sin local ni barrera, con `eCurNisRevingState = NIS_OFF` en el else (el `stw r11` del objetivo es el
1 que cse conoce por el `bne`); (2) la misma con la etiqueta `Continuing` y la rama del patrón saltando
a ella; (3) la local sin la barrera. Las tres dan **`c6b9d1381a490dd0`**, 3 filas (el estado se carga
en r9 y no en r11). En el `.lreg`, la pseudo del estado (3 refs, 8 insns, global) solo choca con r0 (la
comparación de `PattternPlay`); r9 está libre en toda su vida y `REG_ALLOC_ORDER` (r0, r9, r11…) se lo
da. El clobber de r9 es el conflicto que el objetivo tenía y que su código no muestra.

**AccelTrans.** Sin la barrera y con el pin intacto sale `0e166060ad90523d`: **el mismo objeto que sin
pin ni barrera** (r66). El pin a r11 solo actúa porque la barrera le da la referencia en ese punto; pin
y barrera son un único andamio, irreducible por el diagnóstico RTL de la r67.

**LockPalette.** El objetivo copia el resultado del `new` a r6 *antes* del `cmpwi r3` y recorre la
paleta con r6; sin la barrera sale `mr. r3,r3` y el bucle usa r3. (1) Pal32 dentro del if, donde lo
pone el DWARF (su bloque empieza en +0x34; la línea 229 no tiene código): `e435743e0874a98c`. (2) El
`new` como el inline `operator new[](size, file, line)` de bware.hpp que lista el DWARF, con Pal32
dentro o fuera del if: `0fe55fb92fd03144`, las mismas 3 filas, y además mete el `__FILE__` en `.rodata`
(el zPlatform original no contiene esa cadena). (3) Guarda envolvente, que es lo que da el mapa (no hay
return en la 225, solo el 243 del final): `e435743e0874a98c`. Las tres con las mismas 3 filas.

**JoyE.** Sin intento nuevo porque todo lo que se podía probar está medido: la escalera de la r61 (G4
6 filas, +1 11, +2 14, +3 8, +5 cero; detrás del `extsh` DCE se las lleva) y la palanca (e) de la r67
(8 filas). Los seis andamios sostienen dos `asm volatile` con instrucciones que reproducen la línea 307
del original, y el relleno solo mueve `live_length`.

## 5. Propuesta (cabecera ajena, medida)

`src/Speed/Indep/Src/EAXSound/UG/SndDataParams.hpp`, línea 144:

```
-    stBankSlot() {} // Decl: 287
+    stBankSlot() { Clear(); } // Decl: 287
```

y, **solo con eso aplicado**, en `EAXAemsManager.cpp` (`RegisterSlots`):

```
     stBankSlot NewSlot;
-    stBankSlot *pNewSlot = &NewSlot;
-    pNewSlot->Clear();
```

Medido en árbol privado: zEAXSound **IDÉNTICO** (`9e92525a5436ae95`) con los dos cambios. Las otras 13
SourceLists que incluyen la cabecera de forma transitiva, solo con la cabecera cambiada, también
**IDÉNTICAS**: zAI `f86c2656c7e38244`, zEAXSound2 `904281697182401c`, zEcstasy `83b9a78ea8e2fd0d`, zFe
`5a12f894633da873`, zFe2 `d8f49a3064f13f92`, zFeOverlay `9e4d806d60a97306`, zGameplay
`8608d060553a271a`, zLua `693ade7a3e0240f0`, zMain `cc5a538171d2ac53`, zMisc `0dc0e9a8f689e27d`,
zPlatform `7411942baadf95ea`, zSim `6913d4cebd1c34a7`, zSpeech `39d1580dcbc5228a`. Quita una local que
el DWARF no tiene (no es un andamio asm). Quien la aplique tiene que retocar la nota r67b de
`RegisterSlots`, que explica `pNewSlot`.

## 6. Sellos

Compilación privada; árbol = HEAD + los bytes finales de mis 6 ficheros. Los 6 ficheros reales son
byte a byte iguales a los privados (`cmp`).

| unidad | ficheros del lote | ALLOC antes | ALLOC después | `.symtab` antes = después | `.strtab` antes = después |
|---|---|---|---|---|---|
| zEAXSound | EAXAemsManager, 3DObjPos, AccelTrans, NISReving | `9e92525a5436ae95` | `9e92525a5436ae95` | `448856c06524b879` (42.544 B) | `29acff0a481caa1f` (72.196 B) |
| zPlatform | JoyE, TextureInfoPlat | `7411942baadf95ea` | `7411942baadf95ea` | `13128237999413e5` (12.320 B) | `405c9af7673ad688` (19.517 B) |

El control puede fallar y falla: de las 18 compilaciones de prueba, 13 cambiaron el digest (at1, lp1-4,
nis1-3, snl1-2, op1, op4, rs2). Las 5 idénticas: rs1, rs3, op2, op3 y op23.

Finales de línea: TextureInfoPlat, JoyE y EAXAemsManager siguen 100 % LF; AccelTrans y NISReving,
100 % CRLF; SFXCTL_3DObjPos conserva su mezcla (287/8 → 295/6: se van dos líneas LF de barrera y las
líneas de nota nuevas van en CRLF, el final dominante). Las líneas `-` de `git diff` son solo las
barreras retiradas, las sentencias reordenadas de `RegisterSlots` y el comentario reescrito del clon:
ningún andamio perdido por una nota.

## 7. Reglas medidas

1. **Si quitar la barrera sola da el mismo objeto que quitar pin y barrera, el pin es inerte sin ella**
   (AccelTrans): son un único andamio y no se puede retirar uno de los dos.
2. **Las barreras de una misma función caducan por separado.** Las cuatro de 3DObjPos que sobraban
   convivían con dos clobbers que sí sostienen: mide cada pareja por separado y después juntas.
3. **Una barrera `"+r"(x) : : "rN"` puede ser solo su clobber**: en UpdateNIS la local con y sin su
   entrada `"+r"` da el mismo objeto; lo único que actúa es el clobber. No se puede partir sin convertirla
   en otra forma de asm (regla 9), así que se diagnostica entera.
4. **"Cuatro referencias de más" es un escalón de `floor_log2`**: 12 → 16 refs duplica el factor. Si el
   hueco de prioridad solo se cierra cruzando ese escalón y el `.cpp` no tiene cuatro usos reales más,
   no hay forma de C en ese fichero (SetupNextLoad).
5. **Un puntero local que el DWARF no lista puede ser el `this` de un constructor inline** que en el
   original hacía trabajo (RegisterSlots: `pNewSlot` ↔ `stBankSlot() { Clear(); }`). La corrección
   está en la cabecera, no en quitar la local.
6. **Herramienta**: el reemplazo por bytes con un único final de línea no casa en ficheros mixtos;
   `repl.py` compara sin CR y conserva el final de cada línea existente.

## 8. Para el cierre

* No he relinkado ni tocado `build/`: con ALLOC idénticas no hay bytes de DOL que mover. Los `.o` de
  `build/GOWE69` de zEAXSound y zPlatform deberían salir `ok_debug` (difieren en `.line`/`.debug`).
* Sin propuestas de `configure.py`, `config/GOWE69` ni `keep.lst`.
* `previo.py r67b` encuentra las notas. Algunas las asigna a la función anterior (su detector de
  función va una línea tarde, igual que en la r67); el texto está en su sitio.

## 9. Veredicto

**PARCIAL con cifra: 5 de 20 retiradas (25 %), 15 irreducibles con diagnóstico r67b junto al andamio,
0 regresiones y 1 propuesta medida en 14 unidades.** Las retiradas salieron de la forma del mapa de
líneas (RegisterSlots) y de medir cada pareja por separado (3DObjPos). En los irreducibles, el volcado
RTL da el número exacto que el andamio aporta y que ninguna forma de C del fichero reproduce.
