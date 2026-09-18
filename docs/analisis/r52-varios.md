# r52 — `varios`: zSpeech, zEagl4Anim, zPlatform, zTrack

**Cerrado: nada.** Ninguna de las cuatro unidades queda con el código al 100 %.
Todo revertido; las cinco funciones vuelven al byte y al porcentaje de partida.

Lo que sí traigo son **tres mecanismos con nombre y cifra** que convierten vedas
vagas en vedas con condición de salida —y **dos salidas que las rondas
anteriores dejaron abiertas y que resultan FALSAS**:

- `zSpeech/Setup` — la diferencia es el **desempate por `INSN_LUID` de
  `rank_for_schedule` en `sched2`**, y el orden que ve `sched2` lo fija `sched1`
  con prioridad 8 contra 6. Volcado RTL adjunto.
- `zEagl4Anim/Initialize` — **la salida que propuso la r47 (hacer
  `node_is_bounded` el nodo {2}) NO reproduce el objetivo**: da 15-16
  instrucciones de árbol y el objetivo tiene 17. La forma real del objetivo
  exige un nodo **RANGO** que empieza en 2, y todas sus cotas altas alcanzables
  chocan con `case SHT_STRTAB`.
- `zTrack/GetLoadingPriority` — **cinco órdenes de declaración del bloque muerto
  medidos: cero bytes de efecto.** Los 16 B no son una reordenación. Y el
  oráculo del marco está calibrado con dos controles.

Base de la ronda (`fndiff`, filas contadas, no fuzzy):

| unidad | función | B obj / nuestro | % | filas |
|---|---|---|---|---|
| zSpeech | `Setup__Q26Speech13RoadblockFlow` | 596 / 596 | 98,255035 | **3** |
| zEagl4Anim | `Initialize__Q25EAGL413DynamicLoader...` | 2.352 / 2.356 | 98,87585 | 28 |
| zEagl4Anim | `EvalState__Q29EAGL4Anim14FnRawStateChan...` | 456 / 456 | 98,070175 | 18 |
| zPlatform | `ActualReadJoystickData__Fv` | 1.588 / 1.580 | 99,3199 | 16 |
| zTrack | `GetLoadingPriority__13TrackStreamer...` | 708 / 708 | 94,72317 | 75 |

---

## 1. zSpeech — `RoadblockFlow::Setup`: es el nivel 5 de `rank_for_schedule`

La veda de la r48 decía «solo el orden del thunk de `PursuitApproaching`, sin
palanca nueva». Ahora está el mecanismo entero, medido.

### La diferencia

```
objetivo: lwz r9,0(r31); lwz r0,0x2e4(r9); lha r3,0x2e0(r9); mtlr r0; add r3,r31,r3; blrl
nuestro:  lwz r9,0(r31); lha r3,0x2e0(r9); lwz r0,0x2e4(r9); add r3,r31,r3; mtlr r0; blrl
```

### Lo que NO es (medido, con control)

Barrido de **15 banderas** sobre un TU mínimo que reproduce la función entera
byte a byte (`#include SpeechManager.hpp` + `RoadblockFlow.cpp`, 149 insns
idénticas a la SourceList): `-fno-schedule-insns`, `-fno-schedule-insns2`,
`-fno-force-mem`, `-fno-force-addr`, `-fno-gcse`, `-fno-expensive-optimizations`,
`-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`, `-fno-rerun-cse-after-loop`,
`-fno-move-all-movables`, `-fno-rerun-loop-opt`, `-fomit-frame-pointer`,
`-fno-defer-pop`, `-fno-peephole`. **Ninguna mueve esa ventana.**

*Control que TENÍA que cambiar*: `-fno-schedule-insns2` y `-fno-schedule-insns`
sí cambian OTRAS partes de la misma función (filas 12, 20-21, 23, 25 y la
numeración de etiquetas), o sea que las banderas se aplicaron de verdad.

Y seis formas de fuente/`asm`, todas neutras:

| forma | insns | ventana |
|---|---|---|
| base | 149 | `lha` primero |
| `__asm__("")` delante de la llamada | 149 | = base, binario idéntico |
| `__asm__("")` detrás | 149 | = base |
| `__asm__("" : : : "r0")` delante | 149 | = base |
| `__asm__("" : : : "r3")` delante | 149 | = base |
| `__asm__("" : : : "r9")` delante | 149 | cambia el resto, no la ventana |

### Lo que SÍ es: el volcado con `-fsched-verbose-5`

`cc1plus -dS -dR -fsched-verbose-5`, bloque 23 de `Setup`:

```
sched1                                   sched2
 insn  dep prio                           insn  dep prio
  469   1   10  : 484 480 476              469   1   10  : 484 480 476
  476   2    8  : 484 482                  476   3    8  : 484 482
  480   2    6  : 484                      480   2    8  : 484 573
  482   3    6  : 484                      482   3    6  : 484
                                           573   2    6  : 484
  484   8    2  : 486                      484   9    2  : 486
```

- En **`sched1`** no hay empate: `prio(476)=8 > prio(480)=6`, porque la cadena
  del delta tiene un salto más (`lha -> add -> call`) que la del puntero de
  función (`lwz -> call`). El planificador emite 476 en el ciclo 6 y 480 en el 7.
  **Ese orden es el que hereda `sched2` como `INSN_LUID`.**
- En **`sched2`** el `mtlr` (insn 573, que mete `reload`) alarga la cadena del
  puntero y **las dos prioridades empatan a 8**. La lista de listos es
  `480 476` y `rank_for_schedule` (gcc/haifa-sched.c:4158) las recorre:
  1. prioridad — empate 8/8;
  2. `INSN_REG_WEIGHT` — **no se evalúa**, va bajo `!reload_completed`;
  3. interbloque — mismo `bb`;
  4. clase respecto de la última planificada (469): las dos son dependencia de
     datos con coste 2 → clase 1 las dos;
  5. `depend_count`: 476 → {484, 482} = 2; 480 → {484, 573} = 2 — empate;
  6. **`INSN_LUID`: gana 476.**

### La condición de salida, exacta

Para reproducir el objetivo hace falta **`depend_count(480) ≥ 3`** o
**`LUID(480) < LUID(476)`**. Y las dos están cerradas desde la fuente:

- Un tercer dependiente de 480 tendría que leer o escribir `r0` entre el `lwz` y
  el `blrl`. Cualquier `asm` que se pueda escribir en C cae **antes** de la
  expansión de la llamada o **detrás** del `call`, y el `call` marca
  `reg_pending_sets` de todos los `call_used_regs` (`sched_analyze`), así que un
  clobber posterior depende del `call`, no del `lwz`. Medido: los cuatro
  clobbers de arriba dan el binario base.
- Bajar el `LUID` exige que `sched1` emita 480 primero, y eso exige
  `prio(480) ≥ 8` en `sched1`, o sea una insn más entre el `lwz` y el `call`
  **antes** de `reload`. La ABI de vtable sin thunks no la produce.

**Veda reafirmada con mecanismo.** No volver a barrer formas de la sentencia:
las tres llamadas virtuales *que sí casan* en la misma función tienen la misma
estructura y el mismo desempate, y casan porque en ellas el objetivo también
eligió el `lha`.

### Los andamios de `Setup`, RE-MEDIDOS: siguen pagando

Los tres `__asm__("# a"/"# b"/"# c")` de `RoadblockFlow.cpp` **no han caducado**.
Quitándolos los tres, la función pasa de **149 a 142 instrucciones (596 → 568 B)**:
bloquean el `cross-jumping` de siete instrucciones de cola que el original
tiene duplicadas. No tocar.

---

## 2. zEagl4Anim — `DynamicLoader::Initialize`: la salida de la r47 es falsa

El árbol del `switch` sigue igual (28 filas, +4 B, una sola instrucción de más:
un `b`). La r47 dejó escrito que el `emit_jump` que nos falta «solo sale si el
nodo {2} es `node_is_bounded`». **Leyendo `stmt.c:5875 emit_case_nodes` y
contando instrucciones, esa salida no reproduce el objetivo.**

El objetivo emite, en la hoja del `2`:

```
cmplwi r0,1 ; ble FIN      <- LT node->low, con low = 2 (GCC canonicaliza <2 a <=1)
(cae al cuerpo de SHT_SYMTAB)
```

Eso es la rama **«nodo RANGO sin hijos, `!low_bound && high_bound`»**
(`stmt.c`, bloque `else { /* Node is a range */ } ... node->right == 0 &&
node->left == 0`), que emite el test de cota baja y después
`emit_jump (node->code_label)` —que `jump.c` borra por caer al siguiente—.

Si en vez de eso el nodo {2} fuese `node_is_bounded`, `emit_case_nodes` emitiría
**solo** `emit_jump` **sin ningún test**. Cuenta de instrucciones del árbol:

| variante | insns del árbol |
|---|---|
| **objetivo** | **17** |
| nuestra base | 18 |
| nodo {2} `node_is_bounded` (la salida de la r47) | 15, o 16 si el `b` no cae al siguiente |

O sea que bounded-{2} **se pasa de largo**: quita dos instrucciones donde el
objetivo quita una.

**Lo que el objetivo exige de verdad**: que el nodo que cubre `SHT_SYMTAB` sea un
**RANGO** `[2,H]` con `H > 2` (si `low == high`, `emit_case_nodes` entra por
`tree_int_cst_equal` y emite `do_jump_if_equal`, que es nuestro `cmpwi 2;beq`),
y con `node_has_high_bound` satisfecho por un antecesor cuyo `low == H+1`. Los
antecesores disponibles en el subárbol BAJO son `{3}` (→ `H = 2`, contradice el
rango) y la raíz `{8}` (→ `H = 7`, se solapa con `case SHT_STRTAB`). Y
`node->low == TYPE_MIN` (0) daría un nodo acotado, sin test.

**Veda ampliada**: el árbol de `Initialize` no sale ni de editar la lista de
`case` (r36e/r47, 622 variantes) **ni de acotar el nodo {2}**. La única forma
alcanzable exigiría que `case SHT_SYMTAB` fuese un rango que se come el
`case SHT_STRTAB`, y el objetivo emite `cmpwi 3; beq STRTAB`.

No he vuelto a barrer formas del `switch`: no las hay nuevas que la r47 no
midiese.

---

## 3. zEagl4Anim — `FnRawStateChan::EvalState`: la base común colapsa la rama

18 filas, 456/456 B. Las dos filas que importan siguen siendo
`mr r4,r11` / `mr r4,r9` del objetivo contra nuestro `add r4,r30,r11`.

El diagnóstico de la r50 es que hace falta que **las dos ramas de `GetKeyData`
compartan el pseudo de `2*mNumFields+10`** para que PRE lo ice. Cuatro formas
nuevas de `GetKeyData` (RawStateChan.h, cabecera de mi territorio: sólo la usan
`RawStateChan.cpp` y `MemoryPoolManager.cpp`, ambos de zEagl4Anim):

| forma | filas | tamaño | % |
|---|---|---|---|
| base (dos ramas) | **18** | **456** | 98,07018 |
| base común + `keyData += 2` en el `else` | 76 | 412 | 54,96491 |
| base común + `(ushort*)keyData + 1` | 77 | 404 | 62,91228 |
| `unsigned short *d` común, `+2` en char* | 69 | 412 | 60,23684 |
| base común + `(mNumFields & 1) ? 0 : 2` | 76 | 412 | 54,96491 |

Las cuatro **funden la rama entera** (456 → 404/412 B): en cuanto las dos ramas
difieren sólo en una constante, GCC las convierte en un `select` y desaparece el
`if` que el objetivo sí tiene. La cabecera queda restaurada byte a byte.

**Veda**: compartir el pseudo desde la fuente y conservar la bifurcación son
incompatibles con este `GetKeyData`. Si alguien vuelve, la palanca tiene que
actuar sobre PRE/`gcse` (bloque común dentro del bucle), no sobre la forma del
accesor.

---

## 4. zPlatform — `ActualReadJoystickData`: el clobber entero, calibrado

16 filas en tres grupos (r18/r19 ×6, r10/r11↔r8 ×6, y el par que falta ×2, que
son los −8 B). Nueve variantes nuevas, todas negativas:

| forma | filas | tamaño | % |
|---|---|---|---|
| **base** | **16** | **1.580** | **99,3199** |
| `data = (short)data; store; v = data & 0x8000;` | 20 | 1.576 | 98,55164 |
| `__asm__("" : : : "r8")` dentro del bloque de stickX | 21 | 1.580 | 99,25693 |
| `v = v & 0x8000;` + `asm("" : "+r"(v))` tras el store | 20 | 1.576 | 98,55164 |
| `register int data asm("r11")` | 26 | 1.576 | 98,90428 |
| `__asm__("" : : : "r19")` antes del bloque de calibración | 167 | 1.572 | 97,17632 |
| `__asm__("" : : : "r18")` en el mismo sitio | 167 | 1.572 | 97,17632 |
| `__asm__("" : : : "r19")` dentro del bloque | 167 | 1.572 | 97,17632 |
| `__asm__("")` en el mismo sitio | 21 | 1.580 | 99,25693 |

### Calibración de la palanca nueva de la r51 (`__asm__("" : : : "rN")`)

Como r18 y r19 daban la MISMA cifra, exigí un control que tuviera que
distinguirlas:

| clobber | filas | tamaño | % |
|---|---|---|---|
| `"r14"` | 169 | 1.572 | 97,15113 |
| `"r3"` | 21 | 1.580 | 99,25693 (= `asm` vacío) |
| `"r14","r15","r16"` | 114 | 1.580 | 95,58942 |

**El nombre del registro SÍ importa** (r14 ≠ r18/r19 ≠ tres a la vez), y un
clobber de un registro *call-clobbered ya muerto* (`r3`) equivale al `asm`
vacío. Que r18 y r19 coincidan es coherente con el diagnóstico de la r47: los
dos allocnos empatados simplemente se corren a la pareja libre siguiente, den
igual cuál de los dos bloquees. **La palanca está viva y es dirigida; en esta
función es negativa por 150 filas.**

---

## 5. zTrack — `GetLoadingPriority`: el oráculo del marco, y lo que NO son los 16 B

75 filas; 37 son el desplazamiento mecánico de +0x10 del marco
(`stwu r1,-0x120` del objetivo contra nuestro `-0x110`).

### El oráculo, calibrado con dos controles

El bloque `if (RemoteCaffeinating && TrackStreamerRemoteCaffeinating)` **emite
cero instrucciones** (rango DWARF `0x802BC340 -> 0x802BC340`) pero reserva sus
locales. Cualquier cambio dentro sólo puede mover el marco, así que el número de
filas es una medida limpia:

| control | filas | tamaño |
|---|---|---|
| base | 75 | 708 |
| `char layer_name[48]` (16 B más) | **32** | 708 |
| `FloatVector` extra + una llamada fabricada | **32** | 708 |

Confirmado: **16 B más en el bloque valen 43 filas de golpe**, y el resto (32)
ya es reparto de coma flotante (f30↔f31).

### Lo que he descartado con medida: NO es el orden de declaración

Cinco reordenaciones legítimas del bloque, **las cinco a cero bytes de efecto**
(75 filas / 708 B / 94,72317 % exactos, idénticas a la base):

1. `FloatVector face[4]` exterior declarado **detrás del bucle** (la hipótesis
   explícita de la r49);
2. `FloatVector face[4]` interior declarado **antes** de `point1`/`point2`;
3. `face[4]` exterior subido **al principio del bloque**, junto a `layer_name`;
4. `FloatVector pos` subido junto a `layer_name`;
5. (1)+(2) a la vez.

**Veda nueva**: los 16 B **no** son una reordenación de declaraciones dentro del
bloque. Quien vuelva no tiene que probar órdenes; tiene que encontrar la
sentencia que crea un **temporal sin nombre de 16 B** (un `FloatVector` redondea
0xC a 0x10 en `assign_stack_temp`).

### Y una inconsistencia del volcado que conviene apuntar

El DWARF del original (`symbols/mw_dwarfdump.nothpp:2177094`) dice, textualmente:

```
char layer_name[32];          // r1+0x8
struct FloatVector pos;       // r1+0x28
struct FloatVector face[4];   // r1+0x38      <- exterior
    ...
    struct FloatVector face[4];   // r1+0x48  <- interior
```

Con `FloatVector` de 0xC (x,y,z sin relleno, confirmado), `face[4]` mide 0x30 y
**0x38 + 0x30 = 0x68 > 0x48**: los dos arrays se solapan. Con asignación
secuencial eso es imposible, así que **una de las dos entradas del volcado no
puede leerse al pie de la letra** —y las dos lecturas que han circulado (r46:
«el exterior en 0x38 en los dos»; r48/r49: «en 0x38 hay un temporal de 0x10 y el
exterior está en 0x78») son incompatibles entre sí. La lista de *inlines* del
bloque sí es idéntica a la nuestra (`espCreateObjectAsync`,
`espSetAttributeString`, `GetNumPoints`, `espCreateUserMesh`, `espSetUserMeshFace`
×2, `GetPoint` ×2), o sea que **los 16 B no salen de una llamada más**.

Sigue en pie la prohibición de rellenar con una local inventada.

---

## Vedas: estado

**Nuevas**

- `zSpeech/Setup`: desempate por `INSN_LUID` en `sched2` con
  `depend_count` 2/2 y prioridad 8/8; el orden lo fija `sched1` con 8 contra 6.
  Cerrada salvo que aparezca una insn entre el `lwz` del puntero y el `call`
  **antes de `reload`**.
- `zEagl4Anim/EvalState`: cualquier forma de `GetKeyData` con base común funde
  la bifurcación (456 → 404/412 B). No insistir por el accesor.
- `zTrack/GetLoadingPriority`: los 16 B no son orden de declaración (5 formas).

**Refutada (salida falsa, no veda caducada)**

- `zEagl4Anim/Initialize`: «basta con que el nodo {2} sea `node_is_bounded`»
  (r47). Da 15-16 instrucciones de árbol contra las 17 del objetivo.

**Re-medidas y CONFIRMADAS (no han caducado)**

- Los tres `__asm__("# a"/"# b"/"# c")` de `RoadblockFlow::Setup`: sin ellos la
  función pierde 7 instrucciones (596 → 568 B). Siguen pagando.

**Calibrada**

- `__asm__("" : : : "rN")` (palanca r51): el nombre del registro **sí** importa
  (r14 ≠ r18 ≠ tres a la vez), y clobrar un `call_used` ya muerto equivale al
  `asm` vacío. En `ActualReadJoystickData` es negativa: 167-169 filas.

---

## Verificación

- `python scripts/build_direct.py zSpeech zEagl4Anim zPlatform zTrack` → **4 ok,
  0 fallidas**. Nunca se lanzó `ninja` ni `configure.py`.
- `fndiff` final de las cinco funciones, idéntico a la partida:
  `Setup` 596/596 98,255035 %; `Initialize` 2.352/2.356 98,87585 %;
  `EvalState` 456/456 98,070175 %; `ActualReadJoystickData` 1.588/1.580
  99,3199 %; `GetLoadingPriority` 708/708 94,72317 %. **Ninguna empeora.**
- `fncmp` de las cuatro unidades, después de revertirlo todo: zSpeech **1 de
  703** con el código distinto (596 B), zEagl4Anim **2 de 318** (2.808 B),
  zPlatform **1 de 136** (1.588 B), zTrack **1 de 259** (708 B). Exactamente el
  censo de partida: ninguna función vecina se movió.
- `git status` en mi territorio (`RoadblockFlow.cpp`, `eagl4supportdlopen.cpp`,
  `RawStateChan.h/.cpp`, `JoyE.cpp`, `TrackStreamer.cpp`): **limpio**. Cero
  commits, cero `git add`. No se tocó `configure.py`, `config/GOWE69/*`,
  `splits.txt` ni `keep.lst`.
- Todos los barridos restauran el fichero en un `finally` y recompilan la unidad
  al terminar; el arbol quedó compilable en todo momento.
- Volcados RTL (`.rtl` de 60 MB, `.greg`, `.sched`, `.sched2`, `.s`) **borrados**;
  sólo se borraron ficheros propios, por nombre exacto (el scratchpad es
  compartido y tiene material de otros agentes).
