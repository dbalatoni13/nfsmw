# r67 — lote `juego-audio-resto`: 22 andamios, 5 retirados, 17 irreducibles con diagnóstico

**Encargo**: dejar fuente legítima en 16 ficheros de audio, plataforma y física
(zEAXSound, zEAXSound2, zPlatform, zPhysics; dos cabeceras compartidas).
Test único: digest de las secciones ALLOC por sección (`.text`, `.rodata`, `.data`,
`.bss`, `.sdata*`, `.ctors` y sus `.rela.*`), ignorando `.line`, `.debug*`, `.rela.debug*`,
`.comment` y `.stab*`. Idéntico → se queda. Distinto → se revierte.

## 1. Cifras

| | |
|---|---|
| andamios antes (censo sobre HEAD, sin comentarios) | **22** = 14 pines + 8 barreras `__asm__("")` |
| andamios después | **17** = 12 pines + 5 barreras |
| **retirados** | **5** (2 pines + 3 barreras), más 1 `asm("")` fuera del censo (EaxSoundTypes) |
| irreducibles con nota `r67` junto al andamio | **17** (todos los que quedan) |
| compilaciones de prueba medidas | 21, más 6 volcados RTL (`.lreg`/`.greg`) |
| regresiones | **0**: ALLOC idénticas en las 7 unidades afectadas |
| propuestas de `configure.py` / `config/GOWE69` / ficheros ajenos | ninguna |

El encabezado del encargo decía «18 andamios»; la lista por fichero del propio encargo
suma 22 y el censo lo confirma (tabla de §6).

## 2. Método (`scratchpad/jars67/`)

Con otros agentes compilando las mismas unidades, **no compilé nunca contra el árbol
real**. Monté un árbol privado (`tree/`) copiando `src/` e `include/` del working tree y lo
validé contra el índice con `git --work-tree=tree diff HEAD -- src include`: **0 líneas**.
Las cinco bases privadas reprodujeron las de la r66 (zEAXSound `9e92525a5436ae95`,
zEAXSound2 `904281697182401c`, zPlatform `7411942baadf95ea`).

Trampa medida al montarlo: `git archive HEAD` **no sirve**. Pierde las cabeceras generadas
e ignoradas (`Src/Generated/Hashes/*.h`: las cinco unidades no compilan) y reescribe los
finales de línea según `text=auto`: JoyE.cpp y ScratchPtr.h salen CRLF cuando en el árbol
son LF, y JoyE.cpp lleva `asm()` con cadenas multilínea.

Herramientas nuevas:

* `cc.py` — compila una unidad del árbol privado a `o/` y da el digest ALLOC.
* `prueba.py <spec>` — reemplazos exactos en el árbol privado, compila N unidades, compara
  con `base.txt`, diff de la función contra el `.o` original y revierte siempre.
* `rtl.py <unidad> --spec sp.py -dl -dg` — `.lreg`/`.greg` de una VARIANTE sin tocar
  ningún fichero: aplica el spec solo durante el preprocesado. `rtldump.py` escribe en
  `scratchpad/rtl`, compartido.
* `lm.py <símbolo...>` — mapa de líneas del original (`symbols/debug_lines.txt`) por
  símbolo, con offset relativo a la función. Una pasada de 38 MB para 13 funciones.
* `aplica.py`, `notas_run.py` — aplican lo aceptado al fichero real y al privado a la vez,
  exigiendo que sean iguales antes y después.

## 3. Retirados

| andamio | fichero | palanca | evidencia | medida |
|---|---|---|---|---|
| pin `mode` a r0 | SFXObj_Pathfinder.cpp | (e) | la expresión como operando `"r"` | IDÉNTICO con local sin pin y sin local |
| pin `rightPriority` a r0 (+ `asm("")`) | EaxSoundTypes.cpp | forma de fuente | DWARF: sin locales | IDÉNTICO |
| barrera de `_Alloc` | ScratchPtr.h | (a) ámbito | DWARF + mapa de líneas | IDÉNTICO en zMain, zPhysics, zPhysicsBehaviors, zSim |
| 2 barreras `+r`(m/m2) | EAXAemsManager.cpp | (a) + estructura | DWARF sin locales + mapa de líneas | IDÉNTICO |

Detalle:

* **Pathfinder `TestToLicensed`.** Pin retirado: `asm("cmpwi %0, 0" : : "r"(...EATraxMode) : "cr0")`
  sin local. GCC carga el valor en r0 él solo.
* **`ScheduledSpeechEvent::sort_nested_priority`.** La r66 midió la forma sin locales con
  `return rhs->priority < lhs->priority` (7 filas). Lo que faltaba era el SENTIDO:
  `return lhs->priority > rhs->priority` sale idéntico, con locales sin pin y sin ellas.
  Las locales en orden right/left dan 4 filas; en orden left/right con `<`, el mismo
  digest que sin locales.
* **`ScratchPtr<T>::_Alloc`** (el cuerpo «todo-o-nada» de la r61b). El DWARF dice
  `unsigned int i; // r10` y pone `spbuffer` en un bloque de rango +0x44..+0x50, que es
  **la rama del else**. El mapa de líneas da 143 a la rama del then y 147 a la del else:
  una escritura de `mPointer[i]` por rama, fundidas por cross-jump. Escrito así y sin
  barrera: idéntico en las cuatro unidades que incluyen la cabecera (búsqueda transitiva
  de `#include`). Quitar solo la barrera: `04578e77b8bb8a4f`, 9 filas. No se ha tocado
  `IMPLEMENT_SCRATCHPTR` (el `= {}` de los 916.004 B).
* **`EAXAemsManager::ResolveCurrentDataMemory`.** El DWARF del original **no tiene ninguna
  local** (ni `mgr`, ni `curLoad`, ni `MemLocation`, ni `m`/`m2`). El mapa de líneas pone
  `pmem = NULL; break;` dentro de cada `case` (1913-1914, 1918-1919, 1925), fundidos
  después en una sola cola. Escrita con `gAEMSMgr.m_pCurLoadSDLP->...` y esa estructura:
  idéntica. Con las locales y sin barreras: `f83034a3e7da14b9`, 6 filas (el `lis` del
  objeto sube delante del `lwz` del argumento).

## 4. Irreducibles (nota `r67` escrita junto a cada andamio)

| función | andamios | intentos r67 | diagnóstico |
|---|---|---|---|
| `GinsuSynthData::BindToData` | pin r0 | 2 | (e) `"=r"` sin pin: 4 filas, r0/r11. En el `.lreg`, la carga de 0x1c (3 insns, 2 refs) gana a `currentBlock` (5 insns, 2 refs). Forma del DWARF y del mapa de líneas (`mCurrentBlock = -1` en la 172, delante de minperiod): 12 filas y +8 B |
| `NFSMixMapState::CreateSubMixChannels` | 3 pines | 3 | sin numCh: 2 filas; offset sin pin y sin numCh: 6; sin zeroAdded: 2. `offset` fijado a r0 DURO le quita r0 a local-alloc; en el original es un pseudo |
| `SFXObj_Collision::InitSFX` | pin r10 | RTL | las tres constantes empatan en local-alloc (2 refs, misma vida) y 0xd queda r11 por número de qty; r10 exige un tercer valor vivo en r11 que el objetivo no muestra |
| `SFXCTL_AccelTrans::UpdateParams` | pin r11 | RTL | el mapa de líneas confirma el orden de la forma de la r66 (108/111/112/115). El valor nuevo (pseudo 96) nace cuando el 0 (r0) ya murió y su base muere en esa insn: local-alloc le da r0 |
| `startnextrequest` | pin r30 | 2 | estructura del mapa de líneas (`!curreq \|\| (state != PENDING && !next)`): 17 filas (antes 21). Orden de declaración del DWARF: el mismo digest. `.greg`: nop 1818 < req 2000 < CR 2307; necesita 6 refs o menos de 35 insns |
| `LGWheels::PlayFrontalCollisionForce` | pin r27 | RTL | `.greg`: &periodic 6 refs / 96 insns = 1250 frente a magnitude 2673. El objetivo la quiere entre 2673 y 3092 (9 refs) y solo tiene 6 usos. Firmas de Force.h y LGWheels.h = DWARF |
| `PhysicsObject::PhysicsObject(const char*...)` | pin r3 | 1 | `IBody(this)`: 5 filas. El otro ctor casa con `IBody(this)` y el DWARF da el mismo árbol de inlines en los dos: es contexto de sched |
| `ActualReadJoystickData` | pin r8 | 1 | (e) `int guard` sin pin: 8 filas. Los `data` caen en r8; guard (invisible) se queda antes r10/r11 |
| `cStichWrapper::Play` | barrera | 2 | el mapa de líneas da al `i++` línea propia (1143); escrito así: 4 filas (r30/r31, el empate de la r47). Sin `scale` (no está en el DWARF) empeora: `*0x7FFF` pasa a `slwi+subf` |
| `CARSFX_PreColWoosh::MsgBarrier` | pin + 3 barreras | 0 | el mapa de líneas confirma la r65 (65 stw, 67 stfs, 68 llamada con `li r4` delante); empate de sched en INSN_LUID |
| `CARSFX_RoadNoise::GenerateRoadNoise` | pin fr10 | 0 | el DWARF no tiene `slipBoost`: es el pseudo del literal 0.1f; r47-r61 saturado |
| `FX_ROADNOISE` (ENVIRO_AEMS.h) | barrera | 0 | el DWARF de `Play` trae los mismos bloques que nuestra fuente (attribID, refcnt, result y los once Set*); queda el empate 1111/1111 de la r47 |

## 5. Reglas medidas que no estaban escritas

1. **Operandos `"r"` de GPR (la regla que la r67 pedía medir).** REG_ALLOC_ORDER de
   rs6000 empieza los GPR por **r0, r9, r11, r10, r8…r3, r31…r13, r12**.
   * Un operando de **entrada** `"r"` cuya carga no es base de memoria y no compite en su
     bloque **cae en r0 sin pin** (Pathfinder: idéntico).
   * Una **salida** `"=r"` no: compite en local-alloc por prioridad
     (`floor_log2(refs)·refs/vida`) y la pierde frente a un qty más corto
     (ginsudata: 4 filas).
   * **Ninguna letra de restricción de rs6000 nombra r0 solo**: `b` es BASE_REGS, que lo
     excluye.
2. **Si el DWARF del original no tiene locales, primero la forma sin locales, y después
   el mapa de líneas.** Las dos barreras de AemsManager y el pin de EaxSoundTypes cayeron
   así. Las locales «nuestras» portadoras de barrera no eran el pin (la palanca 3 agotada),
   sino la estructura: `pmem = NULL; break;` por `case`, y el sentido `>`.
3. **Dos entradas del mapa de líneas en la misma dirección = sentencia sin código
   propio.** Con los rangos de bloque del DWARF localiza en qué rama se declaró una local
   optimizada (ScratchPtr: `spbuffer` en el else).
4. **El sentido de una comparación decide el reparto aunque genere el mismo `subfc`.**
   `a < b` contra `b > a` (EaxSoundTypes).
5. **En `startnextrequest` el orden de declaración no llega al objeto** (el mismo digest
   con el del DWARF), confirmando la r66. La estructura del `if`, en cambio, sí mueve
   filas (21 → 17).

## 6. Sellos: digest ALLOC por `.o` tocado

Compilación privada, árbol = HEAD + exactamente los bytes finales de mis 16 ficheros
(retiradas y notas `r67`). Los 16 ficheros reales son byte a byte iguales a los privados
(`cmp`).

| unidad | ficheros del lote | ALLOC antes | ALLOC después |
|---|---|---|---|
| zEAXSound | EAXAemsManager, EaxSoundTypes, stream, SFXCTL_AccelTrans, STICH_PlayBack | `9e92525a5436ae95` | `9e92525a5436ae95` |
| zEAXSound2 | PreColWoosh, NFSMixMapState, Roadnoise, Collision, Pathfinder, ginsudata, ENVIRO_AEMS.h | `904281697182401c` | `904281697182401c` |
| zPlatform | JoyE, LGWheels | `7411942baadf95ea` | `7411942baadf95ea` |
| zPhysics | PhysicsObject, ScratchPtr.h | `ff1fb3c41d3c588b` | `ff1fb3c41d3c588b` |
| zPhysicsBehaviors | ScratchPtr.h | `cb81a4cb41f386f6` | `cb81a4cb41f386f6` |
| zMain | ScratchPtr.h | `cc5a538171d2ac53` | `cc5a538171d2ac53` |
| zSim | ScratchPtr.h | `6913d4cebd1c34a7` | `6913d4cebd1c34a7` |

ENVIRO_AEMS.h solo la incluye zEAXSound2 y no usa `__LINE__`/`__FILE__`.

El control puede fallar y falla: 15 de las 21 compilaciones cambiaron el digest. Las 6
idénticas son pf1, pf2, types3, types4, sa2 y aems2; en sa1 cambió zPhysicsBehaviors y
zPhysics no.

Censo por fichero (pines / barreras `__asm__("")`, HEAD → ahora):

| fichero | HEAD | ahora |
|---|---|---|
| CARSFX_PreColWoosh.cpp | 1 / 3 | 1 / 3 |
| NFSMixMapState.cpp | 3 / 0 | 3 / 0 |
| CARSFX_Roadnoise.cpp | 1 / 0 | 1 / 0 |
| SFXObj_Collision.cpp | 1 / 0 | 1 / 0 |
| SFXObj_Pathfinder.cpp | 1 / 0 | **0 / 0** |
| ginsudata.cpp | 1 / 0 | 1 / 0 |
| ENVIRO_AEMS.h | 0 / 1 | 0 / 1 |
| EAXAemsManager.cpp | 0 / 2 | **0 / 0** |
| EaxSoundTypes.cpp | 1 / 0 | **0 / 0** |
| stream.cpp | 1 / 0 | 1 / 0 |
| SFXCTL_AccelTrans.cpp | 1 / 0 | 1 / 0 |
| STICH_PlayBack.cpp | 0 / 1 | 0 / 1 |
| JoyE.cpp | 1 / 0 | 1 / 0 |
| LGWheels.cpp | 1 / 0 | 1 / 0 |
| PhysicsObject.cpp | 1 / 0 | 1 / 0 |
| ScratchPtr.h | 0 / 1 | **0 / 0** |

Fuera del censo quedan 12 `asm("")` en estos ficheros (antes 13): 6 de relleno en JoyE,
5 en EAXAemsManager (`SetupNextLoad` y `RegisterSlots`) y 1 en SFXCTL_AccelTrans.
`RegisterSlots` es candidato: su DWARF no tiene ni `pNewSlot` ni `SPUMainAllocsEnd`.

## 7. Para el cierre

* **No he relinkado.** Con ALLOC idénticas no hay bytes de DOL que mover, pero el DOL
  hay que verificarlo desde el edge de `main.elf`.
* **Los `.o` de `build/GOWE69` de estas 7 unidades no los he regenerado**: compilé en
  privado. `build_direct.py` no mira cabeceras y ScratchPtr.h cambió, así que hay que
  reconstruir **zMain, zPhysics, zPhysicsBehaviors y zSim**, además de zEAXSound,
  zEAXSound2 y zPlatform, antes de `censorancios.py`. Deberían salir `ok_debug`: difieren
  en `.line`/`.debug`.
* **`previo.py r67` encuentra las 17 notas.** Asigna la de MsgBarrier a
  `MsgBarrierHit` (su detector de función va una línea tarde); el texto está en el sitio
  correcto.

## 8. Veredicto

**PARCIAL con cifra: 5 de 22 retirados (22,7 %), 17 irreducibles con diagnóstico r67
junto al andamio, 0 regresiones.**

Lo que sacó los cinco no fue ninguna palanca de registro, sino la **forma que dice el
original**: el DWARF (qué locales existen y en qué bloque) y el mapa de líneas (qué
sentencia y en qué orden). Donde ya coincidían con nuestra fuente (AccelTrans, Collision,
LGWheels, stream, Roadnoise, ENVIRO), lo que queda es un empate del asignador o de sched
cuyos números están escritos junto al andamio.
