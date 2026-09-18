# r67 — lote `juego-fe-ecs`: 33 andamios de zFe, zFe2 y zEcstasy

**Encargo**: dejar fuente legítima retirando pines `register T x asm("rN")` y barreras
`__asm__("" ...)` con palancas de C normal (a: posición/ámbito, b: orden de declaración,
c: contenido muerto del DWARF, d: vista agregada, e: operandos `"=r"/"=f"` en asm legítimo).
Test único: digests por sección ALLOC del `.o` de la unidad, compilado a un `.o` **privado**
(`scratchpad/jfe67/o/`), nunca a `build/GOWE69`.

## 1. Cifras

| | |
|---|---|
| andamios del lote (encargo) | 33 (la lista por fichero suma 31; además hay 6 `asm(...)` fuera de la expresión del censo: 2 `"memory"` y 2 `asm volatile("lwz ...")` en FEngSetScaleX/Y, 2 `asm("" : "+r"(wasPaused))` en PushErrorPackage) |
| **retirados** | **0** |
| compilaciones de unidad medidas (variante + digest + diff contra el `.o` original) | 9, todas DISTINTAS y revertidas en el acto |
| volcados RTL de formas limpias (sueltas, `-dg -dl -dS -dR`) | 7 (M0, F0, F1, G2, K1, S1, L0), borrados al acabar |
| andamios con diagnóstico r67 escrito junto a ellos | 33 (22 bloques de comentario, 12 ficheros) |
| regresiones | 0 — las tres unidades reproducen su digest base exacto tras los comentarios |
| propuestas de `configure.py` / `config/GOWE69` / cabeceras | ninguna |

El control puede fallar y falla: las 9 variantes cambian `.text` (y `.rela.text`), y los
comentarios no cambian nada ALLOC.

## 2. Sellos (digests ALLOC, antes = después)

Compilación privada con `scratchpad/jfe67/cc.py` (base al empezar, «después» tras insertar
los comentarios). Sin `.sdata`/`.sdata2` en ninguna de las tres (`-G0`).

| `.o` | digest | `.text` | `.rela.text` | `.rodata` | `.rela.rodata` | `.data` | `.rela.data` | `.ctors` | `.rela.ctors` | `.bss` |
|---|---|---|---|---|---|---|---|---|---|---|
| zFe | `5a12f894633da873` → `5a12f894633da873` | 681582830fe9:185212 | c27d042ced53:167052 | e574107ba4da:20584 | 4a36de9408b3:9780 | 3e2c041a973c:1684 | 30283ac03e83:1332 | 9069ca78e745:4 | a9df38974b1d:12 | nobits:1620 |
| zFe2 | `d8f49a3064f13f92` → `d8f49a3064f13f92` | 5c65ba92ea4e:262704 | 3d4adaa498d6:207828 | 267b9e0edfca:26368 | 5357f56a8bfe:7476 | 1dfbef4916d8:5088 | 99c2ab34ae07:5652 | 9069ca78e745:4 | 70d824928865:12 | nobits:3504 |
| zEcstasy | `83b9a78ea8e2fd0d` → `83b9a78ea8e2fd0d` | 9a5c5191aac5:153320 | 7d58f8c53cd0:122088 | dabe8f8c5226:8424 | 70b381a3a237:72 | b49c6d74b962:4152 | 651d858c4fb6:396 | 9069ca78e745:4 | fbeb71aadc0c:12 | nobits:82136 |

## 3. Medidas (todas revertidas)

| id | función | forma sin andamio | digest unidad | filas |
|---|---|---|---|---|
| G1 | `SubTitler::GetElapsedTime` | `thetime_ms = diff*0.001f; lastTime = timenow; timeElapsed += thetime_ms; return timeElapsed;` con return por rama | zFe 668a7818 | 14 (−4 B) |
| G2 | idem | lo mismo con un solo `return timeElapsed;` | zFe 48bbc825 | **6** (−4 B) |
| P0 | `cFEng::PushErrorPackage` | `bWasPaused = true;` | zFe 8d1d10a0 | 14 (−8 B) |
| P1 | idem | P0 + `if (!(IsPaused() && !bWasPaused))` | zFe 8d1d10a0 | 14 |
| P2 | idem | P0 + `if (IsPaused() == false \|\| bWasPaused != false)` | zFe 8d1d10a0 | 14 |
| K0 | `FEKeyboard::ToggleCapsLock` | `mbShift = false;` delante de `mbCaps = ...` | zFe2 e51bdfdb | 13 |
| S1 | `eSolid::GetPostionMarker(ePositionMarker*)` | `next_marker` de función, asignada en el último bloque, sin pines | zEcstasy 82591aa3 | **9** (la r65 daba 15) |
| R0 | `eViewPlatInterface::Render(ePoly*,TextureInfo*,int)` | sin el `__asm__("")` | zEcstasy 5560b1e1 | 31 |
| L0 | `eLightMaterialPlatInterface::UpdatePlatInfo` (la excepción) | sin el pin `fr5` | zEcstasy b884f0f1 | 48 |

Formas medidas sólo con volcado RTL (sin compilar la unidad porque el `.greg` ya da el
reparto): F0/F1 (`FEngSetScaleX` limpia y con `data` declarada tras `scale`), M0
(`FoundEntry` limpia), K1 (`ToggleCapsLock` limpia).

## 4. Diagnóstico por andamio

Todos llevan en la fuente, junto al andamio, un bloque `r67 (juego-fe-ecs)` con estos datos.

* **FEngSetScaleX/Y (6 pines)** — la palanca (a) no llega. `.greg` de F0: object 82 = 6 refs /
  50 insns, data 85 = 3 refs / 36; F1 da exactamente los mismos números (data 86). El
  `live_length` lo cuenta flow antes de combine desde el set hasta el `stfs Size`, y lo fija el
  árbol de compares del switch, no el ámbito. data necesitaría ≤ 12 insns (u object ≥ 145).
  Ninguna preferencia de registro duro que usar (sólo object←r3, podada por la llamada).
* **MemcardCallbacks::FoundEntry (3 barreras)** — orden de emisión, medido: el `high` del
  literal nace en `precompute_register_parameters` (calls.c:652) porque `-fforce-addr` fuerza
  la dirección (expr.c:8110) antes de que `load_register_parameters` cree el `lwz` de mName.
  sched1 deja `lwz` delante (como el objetivo); sched2 lo deshace por LUID (407 < 412). El
  objetivo además pone el `addi` detrás de los dos `mr`, que apunta a una dirección
  materializada después de los argumentos enteros. Sin forma C encontrada.
* **SubTitler::GetElapsedTime (pin fr1 + barrera)** — G2 es coherente con el DWARF
  (`thetime_ms` sin registro = intermedio fundido; timenow r30) y queda a 6 filas: la suma se
  ata a f1 en `combine_regs` porque f1 muere en el mismo `fmadds`. El objetivo necesita f1
  nacido antes de que muera la suma: el retorno cargado antes del store, que en C no se escribe.
* **cFEng::PushErrorPackage (2 pines + 2 barreras)** — P0 = P1 = P2 (mismo objeto): sin el pin,
  CSE (`-fcse-follow-jumps -fcse-skip-blocks`) reutiliza el r10 que ya vale 1 y borra el
  `li r0,1`; el uso extra alarga la vida del pseudo y cruza r9/r11 arriba. La forma de la
  condición no mueve nada. Pista abierta: el DWARF cierra el primer bloque anónimo justo en el
  `li r0,1` (0x801339B4) y el del else llega al final: las dos ramas no son simétricas.
* **FEKeyboard::ToggleCapsLock (pin r10 + barrera)** — el cero es un pseudo local y local_alloc
  le da r0 porque **r0 es el primer GPR de REG_ALLOC_ORDER**. El r10 del objetivo exige r0
  excluido (clase BASE_REGS o vida en más de un bloque) y el DWARF no tiene locales ni inlines.
* **ArrayScroller::ScrollVer (pin r11)** — sin intento nuevo. Dato nuevo: en el objetivo r11
  lo comparten la copia de new_index y un pseudo de `width` que es copia de su carga en r0 (en
  las dos ramas). El origen a buscar es ese pseudo de width, no la copia.
* **EmitterSystem::Render (3 pines + 2 barreras)** — r65 revalidó los cinco. (b): el DWARF del
  bloque interior tiene el mismo orden que la fuente. (c): sin locales a rango cero que falten.
  (e): no aplica, aquí los asm son los propios andamios. `world_size` es f6 en el DWARF.
* **eLightMaterialPlatInterface::UpdatePlatInfo (pin fr5 + 4 barreras)** — LA EXCEPCIÓN. Sin el
  pin: 48 filas, ciclo de cuatro (diffuse_min_a f7→f6, diffuse_max_scale f6→f5, diffuse_rng_a
  f5→f8, specular_min_scale f8→f7). El volcado lo explica: `envmap_min_scale` (pseudo 187) y
  `specular_min_scale` (140) son **qty LOCALES del bloque 0** y los reparte local_alloc antes que
  a los globales (99, 101, 115 en los puestos 88, 71 y 100 de 131). En el nuestro le queda f8
  libre; en el objetivo los qty locales solapados ocupaban f13..f6 y cae en f5. El pin fija ese
  qty local, y global_alloc sigue dando f5 a diffuse_rng_a (vidas disjuntas). (b) ya está en el
  orden del DWARF y (a) no aplica (todo de ámbito de función). **Pin devuelto a su sitio.**
* **eSolid::GetPostionMarker (2 pines)** — S1 baja de 15 a 9 filas y deja SÓLO el cruce
  r4↔r11. `.greg`: la tabla (84, prioridad 6666) va antes que prev_marker (83, 5555) y en la
  pasada 0 de `find_reg` excluye r4 porque lo prefiere 83, que conflicta con ella → r11; luego
  83 coge su r4. La tabla tendría que preferir r4, y r4 sólo existe como parámetro.
* **eProject (barrera)** — (b) el DWARF está en el orden de la fuente; familia D ya midió 14→23.
* **eViewPlatInterface::Render(ePoly…) (barrera)** — R0: 31 filas = un solo cruce r28↔r30 de
  los dos pseudos que guardan el CR de `use_previous_data == 0x70/0x68` (temporales del
  switch reutilizados por CSE). No son locales: ni (a) ni (b) llegan.
* **GenerateHorizonFogDisplayList (barrera `half`)** — sin intento; el DWARF tiene
  `multiple` en r0 y no `half`; la insn de cero bytes del original sigue sin encontrarse.

## 5. Reglas medidas que no estaban escritas

1. **La pasada 0 de `find_reg` excluye los registros que prefiere un vecino en conflicto**
   (`regs_someone_prefers`): un pseudo de mayor prioridad NO se queda con el registro de
   entrada de un parámetro con el que conflicta, aunque vaya antes. Es el mecanismo del cruce
   parámetro↔carga de `GetPostionMarker`; para invertirlo hace falta una preferencia, no
   prioridad.
2. **Un qty LOCAL de bloque lo reparte local_alloc antes que a cualquier global**, y un pin
   sobre él no bloquea ese registro para un global de vida disjunta. En `UpdatePlatInfo` el
   problema no es el orden de global_alloc sino qué qty locales se solapan con
   `envmap_min_scale` en el bloque 0.
3. **r0 es el primer GPR de REG_ALLOC_ORDER**: toda constante local que no sea base cae en r0.
   Un `li rN` (N≠0) del objetivo para una constante local es señal de clase BASE_REGS o de
   pseudo global.
4. **`live_length` no se mueve con el ámbito de un pseudo inicializado**: F0 y F1 dan los
   mismos números; la palanca (a) sólo actúa sobre variables vivas desde la entrada por
   caminos sin inicializar.
5. **Operandos `"=r"` para GPR**: no se ha usado en este lote (no hay asm legítimo en él), así
   que la regla de GPR sigue sin medir.
6. **Trampa de herramientas**: `FEngInterfaceFEObjects.cpp` tiene finales de línea MEZCLADOS
   (736 CRLF y 73 LF). Un script que normalice `\r\n` y reescriba uniforme cambia el fichero
   entero; aquí sólo se escribió de vuelta el original byte a byte y los comentarios se
   insertaron con el final de línea de su línea ancla.

## 6. Herramientas (`scratchpad/jfe67/`)

* `cc.py`, `tool.py`, `d.py`, `sym.py` — los de `resto66` con la salida en mi directorio
  (`d.py` con JSON por PID, para diffs en paralelo).
* `t.py <spec>` — el spec define `edit(txt)`; aplica, compila la unidad a scratch, compara con
  `base_<unidad>.txt`, lista secciones distintas y filas contra el `.o` original, y revierte
  salvo IDÉNTICO. Las 9 medidas pasaron por aquí (`sp/*.py`, `sp/*.out`).
* `rtldump.py` — copia de `scripts/rtldump.py` con `TMP` propio (el original escribe en
  `scratchpad/rtl`, compartido); con `--file` y `--extra "-I <dir del fichero>"` compila una
  variante suelta en segundos.
* `sec.py`, `linemap.py` — sección de una función en un volcado, y línea de fuente → pseudo.
* `notas.py` — inserta los bloques r67 junto a cada andamio conservando el final de línea.
