# r67b — lote ecs-cam-cabecera: 9 barreras `asm("")` cortas -> 8

**Una retirada (ICEManager), ocho irreducibles con su diagnóstico escrito junto al andamio
(marca r67b), y una palanca nueva medida en el fuente de GCC: las notas de bucle son
barrera de haifa-sched.** Sin commit. Sin tocar configure.py, config/ ni keep.lst.

| fichero | unidad | cortas HEAD | cortas ahora | `__asm__("")` | pines |
|---|---|---:|---:|---:|---:|
| `Src/Ecstasy/EmitterSystem.cpp` | zEcstasy | 2 | 2 | 2 = 2 | 3 = 3 |
| `Src/Camera/ICE/ICEManager.cpp` | zCamera | 2 | **1** | 0 | 0 |
| `Src/Camera/Movers/TrackCop.cpp` | zCamera | 2 | 2 | 0 | 0 |
| `Src/Animation/AnimEntity_WorldEntity.cpp` | zAnim | 1 | 1 | 0 | 0 |
| `Libs/Support/Utility/UTLVector.h` | 24 unidades | 2 | 2 | 0 | 0 |
| **total** | | **9** | **8** | 2 | 3 |

Censo con `(?<![\w])(?:__asm__|asm)\s*\(\s*""` sobre líneas que no son comentario, HEAD
(`git show`) contra copia de trabajo. No se ha convertido ninguna barrera en otra forma de asm.

## 1. El test

`scratchpad/ecscam67b/cc.py` compila a un directorio propio (no toca `build/`, no compite con
otros agentes) y sella sha1 por sección de todas las ALLOC, sus `.rela.*` (salvo las de
depuración), `.symtab` y `.strtab`. Base sellada ANTES de tocar nada, en las 24 unidades que
incluyen `UTLVector.h`. Cierre: las 24 recompiladas con TODOS los cambios del lote.

**Resultado final: 24 de 24 IDÉNTICAS** (todas las secciones comparadas, 9-11 por unidad).
Cada variante se midió contra la base y se revirtió en el acto si cambiaba; cada nota se
selló por separado en su unidad antes del cierre global.

| unidad | .text (tamaño:sha1) | .symtab | antes = después |
|---|---|---|---|
| zAI | 278280:6210151c7c189a32 | 53856:7523fa98f9c0e951 | sí |
| zAnim | 49852:76c06bfc14ad0102 | 18080:27e2e9922f592a6b | sí |
| zCamera | 136020:c9e4271e29424a7c | 30768:a927bf91f1fd2f73 | sí |
| zEAXSound | 172340:da8aa2352b993a10 | 42544:448856c06524b879 | sí |
| zEAXSound2 | 182356:46501ac84a13d750 | 45056:98b6638a679eb9c7 | sí |
| zEcstasy | 153320:9a5c5191aac553d5 | 45184:c111c0590ac5e2d3 | sí |
| zFe | 185212:681582830fe9dd78 | 45360:127600b94f21cd83 | sí |
| zFe2 | 262704:5c65ba92ea4e18a3 | 62128:364a6d363ef42b30 | sí |
| zFeOverlay | (.over) | 25968:925d9851d1059c54 | sí |
| zFoundation | 36184:d9db593e2f103afd | 5936:6691affc55e4b77a | sí |
| zGameplay | 153376:f83a7bb7e86a5264 | 29424:4ae9c03d42e6885e | sí |
| zLua | 111748:4bedf53a2cb37fe6 | 27872:acd27937faf6c9f0 | sí |
| zMain | 175204:911d8ef5f6fadfa2 | 53136:2a3af88ac5112089 | sí |
| zMisc | 84720:d13c241a7fe30644 | 31792:c7410cc7b1183a44 | sí |
| zMiscSmall | 3292:a9b9ead2fdc38752 | 3152:9e84886954775601 | sí |
| zPhysics | 158256:081fed3288811a02 | 33968:cd2a533da8f17309 | sí |
| zPhysicsBehaviors | 249668:1b5a8640d8d99874 | 53952:0ebdf81b6ea7edc7 | sí |
| zPlatform | 35044:6c91d0f291f47b72 | 12320:13128237999413e5 | sí |
| zRender | 2864:737680a74a311281 | 3440:7db65281013fcbab | sí |
| zSim | 109324:43193f3b66724c0a | 24400:52e7c6dc851f5a7b | sí |
| zSpeech | 189636:80ac56ddf62d5e7a | 36752:4fd72247b1bdd3a0 | sí |
| zTrack | 67280:53c6c4a7645e3389 | 14592:26f062124c8694d6 | sí |
| zWorld | 171088:661048ab2440c16c | 46336:3b07e9fcb6ed28eb | sí |
| zWorld2 | 148268:f3ba20f9dd3c2d20 | 21760:ce3a55d8b2a4b1e8 | sí |

Controles que fallan (el test puede fallar): quitar el `asm("")` de AnimEntity cambia `.text`
y `.rela.text` de zAnim; I2, T2 y U1 (abajo) cambian el tamaño de `.text`.

## 2. La palanca nueva: las notas de bucle son barrera de planificación

`haifa-sched.c`, `sched_analyze_insn` (líneas 3776-3826 del árbol): si un insn lleva notas
`NOTE_INSN_LOOP_BEG/END` (o EH, o SETJMP), se marca `schedule_barrier_found` y se le añaden
dependencias contra **todos** los usos y definiciones anteriores, igual que a un `ASM_INPUT`.
Un `do { } while (0);` vacío emite cero bytes y deja esas notas. Es la barrera completa de
sched2 escrita en C.

La diferencia con `asm("")` decide dónde vale: la nota **no es un insn propio**. Se cuelga del
siguiente insn real. Si ese insn tarda varios ciclos (una carga), todo lo que viene detrás sale
un ciclo más tarde. Y además el bucle abre etiquetas: `-fcse-follow-jumps` las atraviesa (no
esconde un valor a cse), pero las constantes de coma flotante sí se rematerializan, y dentro de
bucles anidados loop.c cambia el reparto de toda la función.

Complemento de lectura (`schedule_insns`, 8600-8680): tras reload el planificador trabaja con
`find_single_block_region`, o sea **las unidades son los bloques básicos de flow**.

## 3. Por fichero

### 3.1 `ICEManager::LoadCameraShakes` — 1 retirada, 1 irreducible

- **Tercera pieza de la r36f, `asm("" : "+r"(guard))` tras `int i = 0;` -> `do { } while (0);`:
  zCamera IDÉNTICO.** Esa barrera solo retrasaba el `cmpw` para que `li r27,0` saliera antes. Una
  barrera completa en ese punto fuerza exactamente ese orden.
- **Primera pieza, `asm("" : "+r"(guard) : "r"(num_tracks))`: IRREDUCIBLE.** Con el bucle en su
  sitio (I2): 164 B y `cmpw r28,r27`. cse sigue el salto del bucle y funde los dos ceros. La
  salida `"+r"` es lo único que le esconde a cse que `guard` vale 0. Las formas de
  `warned_overflow` (DWARF: r0) están medidas en r18/r19/r21/r36e.
- regmap sigue diciendo que `i` vive en un bloque más profundo en el original y que `guard` es
  solo nuestra. Es la misma estructura que ya se sabía, no una forma nueva.

### 3.2 `CWorldAnimEntity::Init` — irreducible

La barrera (`ASM_INPUT`) sostiene el **horario**, no el reparto (regmap IDÉNTICO, 20 locales).
Sin ella, `lis r3,"ROOT"` sube dos ranuras y r9/r11 se cruzan en los dos `Set*DelayTime`.

| forma (sin la barrera) | resultado |
|---|---|
| declaraciones en el orden del DWARF (play_flags, info, anim_part, skel, Trans...) | = sin barrera |
| sin `{ int res = anim_part->Init(skel); }` (el DWARF original no tiene ese bloque) | = sin barrera |
| `do { } while (0);` en su sitio (C2) | **2 filas** |
| `while (0) { }` (F1) | = C2 |
| C2 + orden del DWARF / C2 + sin el bloque `res` | = C2 |
| C2 + `skel` antes que `anim_part` | cambia el marco (-8 B) |

El volcado `.sched2` de C2 (`-fsched-verbose=5`, bloque 6) lo explica: la nota queda colgada
del `lwz r30,0x14(r31)`, que es una carga de 2 ciclos. El `lis` sale en el ciclo 18, y en el 19
`addi r3,@l` (470) y `addi r30,r30,0x20` (456) empatan. El LUID desempata al revés que en el
original. Con `asm("")` la barrera es un insn propio y el empate no se produce. No existe un insn
de cero bytes en C que pueda llevar la nota.

Dato para quien siga: el DWARF original pone el inline `GetAnimPart` en 0x80049338, detrás de
los `Set*DelayTime`. El nuestro lo pone al principio de la unidad. Encaja con una barrera justo
delante en el original. Otras diferencias de estructura del DWARF que no tocan esta barrera:
el original no tiene el bloque `res` de `Init(skel)` y sí uno `{ int res; GetAllocated;
GetAnimPart }` tras los tres `CreateFnAnimFromNamehash`.

### 3.3 `EmitterSystem::Render` — las dos cortas, irreducibles

El DWARF original pone los dos `operator*=` en 0x80112A9C, a mitad del bloque de `world_size`.
El nuestro los pone al principio (+0x14c). Es la misma firma que en AnimEntity: una barrera
justo antes de `xbasis *= world_size`. Pero la de C no la sustituye:

| variante | filas / tamaño |
|---|---|
| E1: las dos -> `do { } while (0);` | 37 filas. `particle` r30->r31 desde +0xf4: el bucle anidado cambia loop.c |
| E2: E1 + sin pines fr6/r24 ni fantasma r19 | 64 filas, 688 B (se va el marco del derrame) |
| E4: solo la de `"+m"(xbasis)` -> bucle | 7 filas: la nota cae en el `lfs f9` y el `ori` sale antes |

Los pines y el fantasma (`__asm__`) no se han tocado. r65/r67 ya los revalidaron.

### 3.4 `TrackCopCameraMover::Update` — las dos, irreducibles

- `"+m"(hcomp)` -> `do { } while (0);` (T2): 952/948 B. El bucle corta el bloque de cse y el
  `0.0f` de `vert_comp` se vuelve a materializar (`fmr f0,f30`).
- `"+r"(dst)`: no admite una barrera completa. El objetivo emite los tres `stfs f30` de
  `look_offset` **después** del `addi r4,r4,0x48` de `eMulVector`, y una barrera entre el relleno
  y la llamada prohíbe ese orden. `dst` sigue sin estar en el DWARF.
- Las notas de bloque del DWARF cuadran con las nuestras (de `operator/=` a `SetTargetDistance`,
  todo en +0x214): aquí no falta una barrera en el original, es un empate de sched2.
- Aviso: `FEManager *fe` (primera local del DWARF) **ya existe** en la fuente, en la guarda de
  pausa. Mi intento de añadirla no compiló y no cuenta como medida.

### 3.5 `UTLVector.h`, `Vector::assign` — las dos, irreducibles

Son palancas de **reparto**: `n_refs` y el pseudo que parte la salida `"+r"` (r52-cam §1.2), no
de horario. U1 (las dos -> `do { } while (0);`) da zCamera .text 136020 -> 136028 y zEAXSound
172340 -> 172368. El bucle no suma referencias y corta el bloque de cse. Revertido en el acto.
Posiciones, clobbers y reordenaciones ya barridas en r52-cam §1.3. Solo zEAXSound emite `assign`
como símbolo; zCamera lo lleva inline en `_Storage<CameraAI::Director*,2>`. La nota r67b junto a
las barreras pasó el test en las 24.

## 4. Reglas nuevas

1. **Un bucle vacío es la barrera de sched2 escrita en C** (`NOTE_INSN_LOOP_BEG/END` ->
   `schedule_barrier_found`). Sustituye a una barrera **cuyo único efecto es el orden** y cuyo
   siguiente insn real no es una carga larga (ICEManager). No sustituye a una barrera que esconde
   un valor a cse (cse sigue el salto), ni a una palanca de reparto, ni dentro de bucles
   anidados (loop.c reparte distinto).
2. **La nota de bucle no es un insn**: se cuelga del siguiente insn real y hereda su coste en
   ciclos. Si ese insn es una carga, lo que va detrás sale un ciclo tarde y aparecen empates que
   decide el LUID.
3. **Firma de barrera que falta en el original**: un inline del DWARF original con `Range` a mitad
   de una unidad de planificación, cuando el nuestro lo tiene al principio (`rm_other_notes` junta
   las notas de bloque). Se vio en AnimEntity y EmitterSystem. En TrackCop no aparece, y allí la
   barrera es un empate puro.

## 5. Propuestas

Ninguna: todo cabía en los ficheros del lote.

## 6. Finales de línea

`sub.py` conserva el final de cada línea. ICEManager, TrackCop y UTLVector.h siguen en CRLF.
EmitterSystem sigue en LF puro, como estaba. AnimEntity ya mezclaba: tenía 6 líneas LF alrededor
del `asm("")`, y las 13 de la nota r67b heredaron el LF de esa línea.

Herramientas del lote (scratchpad/ecscam67b): `cc.py` (compilar a directorio propio + sellos),
`fd.py` (diff contra un .o propio), `lines.py` (mapa de líneas DWARF1 de nuestro .o), `sub.py`,
`censo.py`. Volcados RTL y objetos de variante borrados.
