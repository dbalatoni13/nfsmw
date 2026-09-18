# r53 — `speech`: zSpeech

**`trypromo` NO dice DOL OK.** Y no está a «8 bytes y una función»: eso es lo que
mide `linkdelta`, que compara **TAMAÑOS de sección**. Midiendo el ENLACE símbolo
a símbolo, zSpeech emitía **83 de sus 703 funciones en otro orden** —130.078 B
del `.text` de la unidad distintos en el DOL— y además tiene el `.rodata`
permutado (cadenas y vtables) y el `.data` permutado.

Lo que traigo:

- **El orden de emisión de `.text`: de 83 desplazadas a 27** (82 → 26 midiendo el
  objeto), **sin mover un byte de código**: `fncmp` sigue en 1 de 703.
- **`.rodata` de −8 a +8 B**, o sea **16 B ganados** — pero exige una línea de
  `keep.lst` que **NO he tocado** (§4). **El paquete es ATÓMICO**: sin esa línea
  la unidad queda en **−16**.
- **`Setup`: la veda de la r52 queda REFUTADA como estaba escrita**, con el
  control que faltaba: la MISMA función tiene otra llamada virtual en contexto
  idéntico donde el objetivo elige el orden CONTRARIO, y en ella el `lwz` ya
  tiene `depend_count` 3 y pierde igual (§5).

| medida | partida (HEAD) | ahora |
|---|---|---|
| `fncmp` código distinto | 1 de 703 (596 B) | **1 de 703 (596 B)** |
| orden de `.text`, objeto | 82 desplazadas | **26** |
| orden de `.text`, ENLACE | 83 desplazadas | **27** |
| `linkdelta` | `.text +0  rodata−8` | `.text +0  rodata+8` (con la línea de `keep.lst`) |
| `vtord` | 31 de 36 fuera de sitio | 31 de 36 (sin tocar) |

---

## 1. El diagnóstico que faltaba: `linkdelta` no ve el ORDEN

`linkdelta` da `.text +0` porque compara el **tamaño** de la sección del ELF
enlazado. Enfrentando las **tablas de símbolos** de los dos enlaces (base contra
base-con-nuestro-zSpeech), con las mismas 703 funciones y **el mismo tamaño cada
una**, las direcciones se mueven miles de bytes:

```
8028A8BC delta +5308  Destroy__Q26Speech7Manager            (436 -> 436 B)
8028BDA4 delta +14336 AttachSFXOBJ__Q26Speech7Manager...    (56 -> 56 B)
8028E1C4 delta -14416 ClearPlayback__Q26Speech7Manager      (208 -> 208 B)
802A8F50 delta +16424 ShuffleActors__7SoundAI               (2012 -> 2012 B)
```

`dolwhere zSpeech` en la partida: **333.417 B distintos en todo el DOL**, de los
que **130.078 B caen dentro de la ventana `.text` de zSpeech** (0x8028A17C..0x802B5C44).
El resto es el arrastre de la `.rodata`: un `-8` en la sección desplaza en −8 la
mitad baja de **2.653 símbolos** de todo el juego (el delta más repetido de la
comparación de tablas de símbolos).

**Regla para el resto del frente**: `.text +0` en `linkdelta` **no** quiere decir
que el `.text` esté bien. La herramienta que lo dice es `permorden`, y su cifra
para zSpeech (78 desplazadas) llevaba puesta desde antes y nadie la había leído
junto al brief.

## 2. Qué he movido, y por qué se mueve

El orden que emite GCC 2.9 es **orden de DEFINICIÓN** para las funciones
normales, y **`finish_file`** (bloque diferido, al final de la TU) para inline,
plantillas y vtables —ver `nfsmw-orden-de-emision` y `nfsmw-inline-es-posicion`—.
O sea que casi todo esto se arregla **permutando bloques de definición en el
`.cpp`**, sin tocar una línea de código.

| fichero | qué | desplazadas que cierra |
|---|---|---|
| `EAXSound/Stream/SpeechManager.cpp` | reordenados los 42 cuerpos de `Speech::Manager` al orden del objetivo | 78 → 61 |
| `Speech/SoundAI.cpp` | reordenados los 51 cuerpos de `SoundAI` | |
| `Speech/EAXCop.cpp` | `SetRank` delante de `SwapVoices`; `CallforEV`+`InitialCallForBackup` delante de `GetBackupTypeFromDispatch` | |
| `Speech/SpeechCache.cpp` | `~Cache` detrás de `GetEventPool` | |
| `Speech/EAXCharacter.cpp` | `~EAXCharacter` detrás de `Reset` | |
| `Speech/MiscSpeech.cpp` | `SMSCellCall` al principio | 61 → 26 |

Y dos que no son reordenación sino **colocación**:

- **`GameSpeech.hpp`**: `GetCSIptr`/`GetChannel`/`GetEventDat` estaban definidas
  fuera de clase en `SpeechManager.cpp` y salían en medio del `.text`; el
  objetivo las tiene en el **bloque diferido**, pegadas a `IsDataLoaded`, que sí
  es en clase. Pasadas a en-clase, igual que las de `SED_NISSFX` en
  `NISSFXModule.hpp` —que ya estaban bien y sirven de control—.
- **`ClassKey__Q36Attrib3Gen10speechtune`**: `zSpeech.cpp` definía
  `ATTRIB_NO_INLINE_CLASSKEY` y `Observer.cpp` suministraba el cuerpo fuera de
  línea; eso la sacaba **a mitad de la unidad**. El objetivo la tiene en el
  bloque diferido. Quitando la macro y el cuerpo, la `inline` de
  `speechtune.h:409` cae en el caso `DECL_COMDAT` de `nfsmw-decl-comdat-inline`
  —se usa en `speechtune.h:96`, dentro de un constructor en clase, **antes** de
  que se vea su cuerpo, así que no se puede inlinear y sale en `finish_file`— y
  el símbolo aparece en el sitio bueno. **Medido: sigue existiendo** (`fncmp`
  cuenta las mismas 703).

Ninguno de los nueve cambios toca el cuerpo de ninguna función: `fncmp` da 1 de
703 antes y después, y `Setup` mantiene 596 B / 98,255035 % exactos.

## 3. Lo que QUEDA del orden: las 26 son todas del bloque diferido

Ya no queda ni una desplazada en el cuerpo de la unidad. Las 26 son el orden
**dentro de `finish_file`** más una instanciación de plantilla
(`reserve__...SpeechSampleVec`, obj#449 contra nue#416).

`finish_file` (`cp/decl2.c:3643`) recorre `saved_inlines` **en orden de
PARSEO del cuerpo** y llama a `wrapup_global_declarations` **en pasadas** (el
`do { … } while (reconsider)`): en cada vuelta sólo saca las que ya tienen
`TREE_SYMBOL_REFERENCED`, y las virtuales se marcan cuando sale **su vtable**.
Así que el orden final es *(pasada, orden de parseo)*, no sólo orden de parseo.

Orden del objetivo, para quien siga (índices del objeto extraído):

```
604-608  _._SpeechSampleVec  _._VoiceUsage  _._SchedSpchEvents  _._copList  _._observations
609-612  Speech::Module::{GetNumBanks,GetFilename,QueStream,IsStreamQueued}
613-616  Speech::GameSpeech::{GetCSIptr,GetChannel,GetEventDat,IsDataLoaded}
617-619  Speech::SpeechFlow::{GetState,Reset,IsBusy}
620      Attrib::Gen::speechtune::ClassKey
621-640  EAXCharacter (20 accesores)
641-658  EAXCop (18 accesores)
659      EAXAirSupport::IsHeli
660-663  Speech::SED_NISSFX::{GetCSIptr,GetChannel,GetEventDat,IsDataLoaded}
664-666  _._VecHashMap64  _._SpchSampleMap  RebuildTable__VecHashMap
667-679  Hermes::Handler::MemberHandler::Call (13)
680-683  _._SpeechHashIDMap  _._EventHistory  _._SPCHEventList  _._SampleReqList
684+     FixedVector/Vector (idéntico al nuestro)
```

El nuestro: EAXCharacter primero, luego Module, el trío de `VecHashMap`,
GameSpeech, SED_NISSFX, ClassKey, SpeechFlow, EAXCop, EAXAirSupport, y los cinco
`_._` de contenedores partidos en dos sitios.

**La causa medida del primero**: `SpeechManager.hpp` incluye `EAXCharacter.h`
(línea 14) **antes** de `SpeechModule.hpp` (línea 17), y `SpeechManager.hpp` es
el primer `#include` de la unidad. El objetivo tiene `Module` antes que
`EAXCharacter`. **No lo he tocado**: mover ese `#include` cambia el orden de
parseo de toda la unidad y hay que medirlo con tiempo, no al final de una ronda.

## 4. Los bytes de `.rodata`: la línea de `keep.lst` (PAQUETE ATÓMICO)

`keep.lst` salva cadenas muertas por su nombre `$LC<n>`, y **n se desplaza en
cuanto se mueve un literal**. Mi reordenación mueve uno. `lcfix --check` lo caza:

```
CORRIGE  linea 2780: zSpeech.o:$LC915 -> zSpeech.o:$LC930
```

es la entrada bajo `# @lc zSpeech "SoundAI CarCustomization"` (línea 2892-2893 de
`config/GOWE69/keep.lst`). Las tres variantes, medidas con el mismo objeto:

| `keep.lst` | `.rodata` del ELF enlazado |
|---|---|
| como está (`$LC915`, ya rancia) | **−16** |
| esa entrada BORRADA | **−16** (o sea: `$LC915` hoy no salva nada) |
| corregida a `$LC930` | **+8** |

Partida (HEAD, con `$LC915` todavía correcta): **−8**. O sea que **la
reordenación gana 16 B de `.rodata`**, y hace falta la línea para cobrarlos.

    # arréglalo con:  python scripts/lcfix.py zSpeech
    # o a mano, en config/GOWE69/keep.lst:
    # @lc zSpeech "SoundAI CarCustomization"
    zSpeech.o:$LC930

**No he tocado `keep.lst`.** Si esto se promociona sin la línea, la unidad pierde
24 B de `.rodata` (la cadena, estripada como `size & ~7`).

**Aviso**: con la línea corregida la cadena queda **DOS veces** —el `$LC930` que
emite cc1plus y la copia a mano del bloque `asm` del final de `zSpeech.cpp`, que
puso la r52—. Por eso el resultado es **+8** (superávit) y no −16: son 24 B de
más que tapan un déficit real de ~16 B en otro sitio. Quitando la copia del
bloque `asm` se vuelve a −16. Lo dejo como está y lo señalo: es dato de la r52,
no mío, y en valor absoluto +8 está más cerca que −16.

## 5. `Setup`: el control que refuta la condición de salida de la r52

La r52 dejó escrito que la única ventana mala de `Setup` se explica por el
desempate de `INSN_LUID` en `sched2`, y que la salida sería
**`depend_count(lwz) ≥ 3`** o **invertir el LUID**.

**Falta el control, y lo hay dentro de la propia función.** `Setup` tiene DOS
llamadas virtuales en contexto idéntico —llamada normal, y a continuación
llamada virtual sobre el mismo puntero en `r31`—:

```
sitio A (fila 42):  ai->GetDispatch()->RBUpdate(primary, 0);  if (!primary->IsHeli())
sitio B (fila 129): ai->GetDispatch()->RBUpdate(primary, 1);  primary->PursuitApproaching();
```

**El objetivo las emite AL REVÉS una de otra**, y nosotros las emitimos igual:

```
sitio A   objetivo = nuestro:  lwz r9,0(r31); lha r3,0x348(r9); lwz r0,0x34c(r9); add; mtlr; blrl   <- CASA
sitio B   objetivo:            lwz r9,0(r31); lwz r0,0x2e4(r9); lha r3,0x2e0(r9); mtlr; add;  blrl
          nuestro:             lwz r9,0(r31); lha r3,0x2e0(r9); lwz r0,0x2e4(r9); add;  mtlr; blrl  <- las 4 insns
```

### La traza, los dos sitios lado a lado

`schedtrace --file` sobre una TU reducida (`SpeechManager.hpp` + `RoadblockFlow.cpp`),
`-dS -dR -fsched-verbose-5`. Columnas `insn / dep / prio` y lista de dependientes:

```
                 sched1                          sched2
sitio A   168  1 11 : 187 183 179 175     168  1 11 : 187 183 179 175
 (IsHeli) 175  2  9 : 187 183 181         175  3  9 : 187 183 181      <- lha
          179  2  7 : 187 183             179  2  9 : 187 183 561      <- lwz
          181  3  7 : 187 183             181  3  7 : 187 183
                                          561  2  7 : 187 183
          183  7  3 : 187 186             183  8  3 : 187 186

sitio B   469  1 10 : 484 480 476         469  1 10 : 484 480 476
 (Pursuit 476  2  8 : 484 482             476  3  8 : 484 482          <- lha
  Approa- 480  2  6 : 484                 480  2  8 : 484 573          <- lwz
  ching)  482  3  6 : 484                 482  3  6 : 484
                                          573  2  6 : 484
          484  8  2 : 486                 484  9  2 : 486
```

Y las listas de listos, idénticas en los dos sitios y en las dos pasadas:

```
sitio A  sched1/sched2:  Ready list (t=6): 179 175  -> scheduling insn <<<175>>>  (el lha)
sitio B  sched1/sched2:  Ready list (t=6): 480 476  -> scheduling insn <<<476>>>  (el lha)
```

### Qué refuta

1. **`depend_count(lwz) ≥ 3` NO es la salida.** En el sitio A el `lwz` (179)
   **ya tiene 3 dependientes** en `sched2` —`{187, 183, 561}`— y pierde igual,
   porque el `lha` también tiene 3. Lo que haría falta es
   **`depend_count(lwz) > depend_count(lha)`**, no `≥ 3`.
   El tercer dependiente del sitio A es la **`jump_insn` 187**, que
   `sched_analyze` cuelga con `REG_DEP_ANTI` de **todas** las insns del bloque:
   no discrimina, y por eso empata. En el sitio B el bloque acaba en la llamada y
   no hay `jump_insn` dentro de la región, así que los dos se quedan en 2.
2. **`INSN_REG_WEIGHT` (la palanca nueva del brief) NO puede actuar aquí.**
   `gcc/haifa-sched.c:4175`:

   ```c
   /* prefer an insn with smaller contribution to registers-pressure */
   if (!reload_completed &&
       (weight_val = INSN_REG_WEIGHT (tmp) - INSN_REG_WEIGHT (tmp2)))
     return (weight_val);
   ```

   En `sched2` `reload_completed` vale 1: **el escalón no se evalúa**. Y en
   `sched1`, donde sí se evaluaría, **no se llega**: las prioridades difieren
   (9/7 en A, 8/6 en B) y el nivel 1 devuelve antes. La r52 ya lo decía y es
   correcto; el brief de la r53 lo daba por palanca viva y no lo es en esta
   función.
3. **Dos formas de `asm` que la r52 no midió, medidas: neutras.**

   | forma | tamaño | % |
   |---|---|---|
   | base | 596 | 98,255035 |
   | `__asm__("" : : : "r0")` **detrás** de la llamada virtual | 596 | 98,255035 |
   | `__asm__("" : : : "r0")` **entre** `RBUpdate` y la virtual | 596 | 98,255035 |

   Coherente con el mecanismo: la llamada marca `reg_pending_sets` de todos los
   `call_used_regs`, así que un clobber posterior cuelga de la **llamada**, no
   del `lwz`; y uno anterior cae delante de toda la expansión de la virtual.

### La veda, reescrita con su desigualdad

> El sitio B de `Setup` sólo cambia de orden si en `sched1`
> **`INSN_PRIORITY(lwz) ≥ INSN_PRIORITY(lha) = 8`**, y hoy vale 6 porque la
> cadena del delta lleva una insn más (`lha → add → call`, coste 2+4) y la del
> puntero no (`lwz → call`, coste 4). Hacen falta **dos unidades de coste más**
> entre el `lwz` del puntero de función y la llamada, **antes de `reload`** —el
> `mtlr` lo mete `reload` y por eso en `sched2` ya empatan a 8—.
> La vía de `depend_count` está cerrada por el desempate del sitio A, y la de
> `INSN_REG_WEIGHT` por `!reload_completed`.

Y una consecuencia nueva: **los dos sitios son la misma construcción y el
objetivo los resuelve distinto**, así que la diferencia **no** está en la
expansión de la llamada virtual ni en `rank_for_schedule`: está en algo del
fuente original del sitio B que todavía no tenemos. Quien vuelva debe buscar la
sentencia, no la bandera. Pista medida: en la RTL, el `add` del sitio B lleva
`REG_DEAD (reg/v:SI 31 r31)` y el del sitio A no —`primary` muere ahí—; es la
única asimetría que he encontrado entre los dos sitios.

## 6. Lo que queda de zSpeech, con número

| frente | medida | herramienta |
|---|---|---|
| `Setup` | 4 insns, 596 B | `fndiff` |
| orden de `.text` (bloque diferido) | 26 de 703 | `permorden` |
| orden de vtables | **31 de 36** fuera de sitio, y **2 vtables de más** (`Q33Sim9Collision9IListener`, `12AudioMemBase`) que el objetivo no emite | `vtord` |
| orden de cadenas de `.rodata` (ENLACE) | 284 del objetivo, **239 en secuencia** | probe propia |
| `.rodata` | **+8 B** con la línea de `keep.lst` | `linkdelta` |

Las vtables de zSpeech viven **dentro** de su ventana de `.rodata`
(0x80407A7C..0x80408FB8) y sus posiciones bailan entre −3.128 y +3.328 B: **el
déficit de `.rodata` de esta unidad ya no es contenido, es orden**, y el orden de
las vtables es la mitad de él. `prefijotu` dice 0 cadenas ausentes.

Las dos vtables de más son el caso de `nfsmw-la-virtual-que-era-pura` al revés:
emitimos la vtable primaria de una base (`Sim::Collision::IListener`,
`AudioMemBase`) que en el objetivo sólo aparece como sub-vtable de su derivada.

## 7. Verificación

- `python scripts/build_direct.py zSpeech` → **1 ok, 0 fallidas**. **Nunca** se
  lanzó `ninja` ni `configure.py`.
- `python scripts/fncmp.py Speed/Indep/SourceLists/zSpeech` → **1 de 703** con el
  código distinto (`Setup`, 596 B), antes y después. Ninguna vecina se movió.
- `python scripts/fndiff.py zSpeech Setup__Q26Speech13RoadblockFlow` →
  **596/596 B, 98,255035 %**, idéntico a la partida.
- Base contra final medidos con el MISMO procedimiento (restaurando los nueve
  ficheros a `HEAD`, compilando y midiendo): 82 → 26 desplazadas en el objeto,
  83 → 27 en el enlace, `rodata −8 → +8`.
- `trypromo` con `keep.lst` corregida: **DOL ROTO (0c9c50abccb1)**. Con la de
  disco: **DOL ROTO (d0120b91186b)**.
- No he tocado `configure.py`, `config/GOWE69/*`, `splits.txt` ni `keep.lst`.
  Cero commits, cero `git add`.
- Ficheros tocados (9, todos de zSpeech): `SourceLists/zSpeech.cpp`,
  `EAXSound/Stream/{SpeechManager.cpp,GameSpeech.hpp}`,
  `Speech/{SoundAI.cpp,EAXCop.cpp,SpeechCache.cpp,EAXCharacter.cpp,MiscSpeech.cpp,Observer.cpp}`.
  `GameSpeech.hpp` sólo la incluyen `SpeechManager.cpp` y `PursuitFlow.cpp`, las
  dos de zSpeech.
- Volcados borrados por nombre exacto: `scratchpad/r53sp_mini.cpp` y los seis
  ficheros de `scratchpad/schedtrace/`. Sin comodines.
- **Cero `asm` nuevos en el árbol**: los tres `__asm__("# a"/"# b"/"# c")` de
  `RoadblockFlow.cpp` siguen donde la r52 los dejó y los dos ensayos de §5 se
  revirtieron.
