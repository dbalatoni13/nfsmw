# r21 — zEAXSound

Agente de la ronda 21. Encargo: cinco near-miss, 3.732 B accionables.

## Resultado

| | antes | despues |
|---|---|---|
| `Speed/Indep/SourceLists/zEAXSound` | 141.072 B | **143.596 B** |
| funciones al 100 % (zEAXSound+zEAXSound2) | 1.680 | **1.683** |
| near-miss «falta o sobra codigo» | 5 fns / 3.732 B | **2 fns / 1.208 B** |
| muro | 9 fns / 6.288 B | 9 fns / 6.288 B (sin cambio) |

`measure.py --cmp c21eax_antes.json c21eax_final.json`:

    +2524 B, +3 funciones, 1 unidades cambian
         +2524 B    +3 fns  Speed/Indep/SourceLists/zEAXSound   141072 -> 143596

**zEAXSound2 no se mueve** (158.400 B casados antes y despues) y `zSpeech` sigue
`identico al congelado`, pese a que he tocado dos cabeceras compartidas (ver
abajo).

`audit.py Speed/Indep/SourceLists/zEAXSound`: **762 funciones, CERO FALLA**, en
las dos pasadas exigidas (y en la pasada previa al trabajo, tambien cero).
Congelado despues: `frozen.py cong` -> `5ba6c052db1392ce`.

## Verificacion del encargo

El brief coincidia con lo medido salvo un detalle de la firma de
`Play__9EAXCommon18eMenuSoundTriggers`: el brief decia `b+1, beq-1, mr-1, bne+1`
y `triage.py` me dio `bne+1, beq-1, li+1, b+1` (la lista de opcodes va truncada;
el multiconjunto real es `faltan: li, bne, b` / `sobran: beq, mr`). Los tamanos,
porcentajes y el resto de firmas eran exactos.

## Cerradas (3 funciones, 2.524 B)

### 1. `InitAsImpact__Q25Sound14CollisionEventRCQ36Attrib3Gen11audioimpact` — 908 B, 99,339 % -> 100 %

Firma: `faltan 0, sobran 1, mr-1`. El `mr r8,r6` de mas era una copia de la base
del array `levels` **hoisted al preencabezado del bucle** por `loop.c`: nuestra
rotacion manual del bucle (`if (levels[i]) do {...} while (levels[i]);`) escribe
`levels[i]` DOS veces en fuente, y eso crea un pseudo extra para la base dentro
del bucle. El original escribe el bucle **sin rotar** y con el limite en la
condicion.

    -   num_levels = 0;
    -   i = 0;
    -   if (levels[i] != 0) {
    -       do { i = i + 1; num_levels = num_levels + 1;
    -            if (i > 4) break; } while (levels[i] != 0);
    -   }
    +   num_levels = 0;
    +   for (i = 0; i < 5 && levels[i] != 0; i++) {
    +       num_levels++;
    +   }

Ensayos (`mn_forms.py`, stub de `SoundCollision.cpp`):

| | cifra |
|---|---|
| f0 base (do/while rotado a mano) | 99,33921 % 912 B |
| f1 `while` pelado | 96,11894 % 896 B |
| f2 `while` en una linea | 96,11894 % 896 B |
| f3 `for (i=0; levels[i]; )` con `if(i>4) break` | 96,11894 % 896 B |
| f4 `for(;;)` con dos `break` | 96,11894 % 896 B |
| f5 `while` con `++i` en el `if` | 96,11894 % 896 B |
| **f6 `while (levels[i] && i < 5)`** | 98,14097 % **908 B**, y *todos los registros ya identicos* |
| f7 variante de f1 | 96,11894 % 896 B |
| **g1 `while (i < 5 && levels[i] != 0)`** | **100 %** 908 B |
| **g2 `while (i <= 4 && ...)`** | **100 %** 908 B |
| **g3 `for (i=0; i<5 && levels[i]; i++)`** | **100 %** 908 B (elegida) |
| g4 `for` con el cuerpo en el incremento | 99,33921 % 912 B |
| **g5 g1 con `i = i + 1`** | **100 %** 908 B |
| g6 `i != 5 &&` | 98,98679 % 908 B |
| g7 `num_levels = i + 1` | 99,44934 % 908 B |

f6 fue la pista decisiva: con `levels[i] != 0 && i < 5` **el reparto de
registros ya casaba entero** (r10 base, r8 contador, r11 indice, r9 cargado) y
solo quedaba el orden de las dos salidas dentro del bucle; invertir los dos
terminos del `&&` lo cerro. Elijo `g3` porque encaja con el mapa de lineas del
original (`i = 0` + condicion en la linea 283, cuerpo e incremento en la 284).

### 2. `InitiateLoad__14EAXAemsManager` — 1.004 B, 98,665 % -> 100 %

Firma: `faltan 0, sobran 1, mr-1`. El `mr r3,r6` de mas estaba en el epilogo:
nuestra variable `result` vivia en **r6** y habia que copiarla a r3 al volver.
`r6` no sale de la nada: `result` se usa como **cuarto argumento de
`AddQueuedFile`** en tres sitios, y ese argumento va en r6, asi que
`local-alloc` le sugiere r6 a la variable entera. En el objetivo el valor de
retorno esta en r3 desde el principio (`li r3,-2`, `li r3,0`).

Dos cambios, medidos por separado:

| ensayo | cifra |
|---|---|
| base | 98,66534 % 1008 B |
| **m1** `result = -2; goto ReturnResult;` -> `return -2;` (los dos sitios) | 98,24303 % 1004 B — **revertido** |
| **m2** local nuevo `nbytes` para los tres tamanos que van a `AddQueuedFile` | **99,12351 %** 1004 B (el `mr r3,r6` desaparece) |
| **m3** `bMalloc(nSize, FileName.GetString(), 0, 0x1040)` en `TMP_ALLOC_MAIN` | **100 %** 1004 B |
| m4 `bMalloc(0x10000, "EAXAemsManager::m_pAsyncBuff", 0, 0)` | 100 % 1004 B (neutro, se queda por fidelidad DWARF) |

m3 sale de `dwbody.py`: el original expande **`bMalloc(int size, const char
*debug_text, int debug_line, int allocation_params)`** (la sobrecarga inline de
4 argumentos que ya existe en `bWare.hpp`) en dos sitios, y con ella un
`StringKey::GetString()` que a nosotros nos faltaba. La sobrecarga no emite ni
un byte —reenvia a `bMalloc(size, params)`— pero **cambia el pseudo del tamano**
y con el la asignacion de registros: r0 -> r11 y el planificador coloca el
`li r4,0x1040` donde toca.

**m1 documentado como veda util**: con `return <literal>` GCC **iza** el
`li r3,C` por encima del salto e invierte la condicion
(`cmpw r3,r0; li r3,-2; blt EPI`); con `result = C; goto L;` deja el bloque
suelto (`bge L; li r3,-2; b EPI`) y `jump.c` lo cruza con los demas. El objetivo
tiene la SEGUNDA forma en `InitiateLoad`, asi que los `goto ReturnResult` se
quedan.

### 3. `Play__11EAXFrontEnd18eMenuSoundTriggers` — 612 B, 97,320 % -> 100 %

Firma: `faltan 1, sobran 2, 1 SUST: mr.+1, cmpwi-1, mr-1`. Dos causas
independientes, las dos localizadas con `dwbody.py` + `lmap.py`:

**(a) locales que el original no tiene.** El DWARF del original solo lista
`int nvol; // r28` y `int testID; // r9`; nosotros teniamos ademas `adjusted` y
`pitch`. Y el `testID` del original **no es** `(int)etrigger`: es
`etrigger + 23` (el `addi r9,r30,0x17` de la linea 250 del original, usado solo
en las dos comparaciones). Al constructor se le pasa `etrigger` directo.
El mapa de lineas ademas dice que `nvol` se declara en su inicializacion
(linea 245) y que el retorno del camino nulo es `return 0;` literal (linea 244).

    int EAXFrontEnd::Play(eMenuSoundTriggers etrigger) {
        if (IsSoundEnabled == 0) return -1;
        if (this->m_pSFXOBJ_FEHUD == nullptr) return 0;
        int nvol = this->m_pSFXOBJ_FEHUD->GetDMixOutput(2, DMX_VOL);
        g_pEAXSound->SetCsisName(this->m_pSFXOBJ_FEHUD);
        int testID = static_cast<int>(etrigger) + 23;
        if ((unsigned)(etrigger - 80) < 16 && testID != 0x6E && testID != 0x6F) { ... }

**(b) `PlayFrontEndSample_RS::SetPitch` y `::SetAzimuth` en `FE_AEMS.h` estaban
escritas como asignacion pelada; el original las tiene con el mismo clamp de
copia que `SetVolume`.** Se ve en el mapa de lineas: las cuatro `Set*` del
original ocupan exactamente el mismo hueco (SetId 349-358, SetVolume 365-374,
SetPitch 381-390, SetAzimuth 397-406, 16 lineas de separacion) y en las cuatro
la primera instruccion esta 7 lineas por encima del `stw`. Con `pitch = 0x1000`
y `azimuth = 0` el clamp **se pliega a nada**, pero el pseudo que crea cambia el
reparto de registros del constructor entero.

| ensayo | cifra |
|---|---|
| base | 97,32026 % 616 B |
| n1 quitar `int volume` de `_RS::SetVolume` | 97,32026 % 616 B (neutro; **revertido**, ver abajo) |
| n2 quitar `adjusted` y `pitch` sin mas | 93,0 % 616 B |
| n3 `testID = etrigger + 23` (sin (a) completo) | 94,37908 % 616 B |
| **n4 (a) completo** | 95,09804 % **612 B**, `faltan 0, sobran 0` |
| **n5 = n4 + (b)** | **100 %** 612 B |

Ojo con n1: `dwbody` dice que el original NO tiene el local `volume` en
`_RS::SetVolume`, pero el **mapa de lineas si lo tiene** (`mr r10,r28` en la
linea 366, con el `if (x<0)` en la 367). El mapa de lineas gana: el local se
queda. Es un contraejemplo util de que la lista de locales del DWARF puede
omitir una copia totalmente coalescida.

## No cerradas (2 funciones, 1.208 B)

### `UpdateRPM__13SFXCTL_Enginef` — 716 B, 96,453 %

Firma: `faltan 0, sobran 1, 1 SUST: fmr-1`. **Diagnostico completo, sin palanca.**

Es una PERMUTA limpia de dos registros de coma flotante:

| | objetivo | nuestro |
|---|---|---|
| `Cur_RPM` | **f1** | f31 |
| `VisualRPM` | **f31** | f1 |
| `NormalRPM` | f13 | f0 |
| `PhysicsNewAudioRPM` | (sin registro) | f13 |

(confirmado por `dwbody.py`, que lista el registro de cada local.)

`Cur_RPM` se define con dos LLAMADAS (`GetShiftingRPM` y `smooth`), que devuelven
en f1. Si vive en f31 hay que copiar (`fmr f31,f1`) en las dos: dos `fmr` de mas.
El objetivo lo tiene en f1 y solo paga un `fmr f1,f31` antes de
`Record(VisualRPM)`. Total: nosotros 3 `fmr`, el objetivo 2.

La causa esta en `cse`: como `VisualRPM = Cur_RPM;` es una copia pelada, las dos
quedan en la misma clase de equivalencia y **`canon_reg` elige `Cur_RPM` como
representante**, asi que la resta del bloque de redlining
(`VisualRPM = VisualRPM - RedlineingVisualOffset`, que esta DESPUES de dos
llamadas a `smooth`) se escribe con `Cur_RPM`. Eso alarga la vida de `Cur_RPM`
por encima de esas llamadas y la obliga a un registro salvado (f31). En el
objetivo el representante es `VisualRPM`. En `cse.c`, `make_regs_eqv` solo pone
al reg nuevo de canonico si su ULTIMO uso cae mas alla de `cse_basic_block_end`,
o sea que depende de hasta donde llegue el camino de cse, no de la forma de la
sentencia — y eso explica que ninguna de las 21 formas probadas lo mueva.

Ensayos (todos `mn_forms.py` sobre el stub de `SFXCTL_Engine.cpp`, base
96,45252 % / 720 B / 16 diffs):

| grupo | formas | resultado |
|---|---|---|
| j1-j3 | `if/else if/else` (con llaves, sin llaves, anidado) en vez del ternario anidado de `Cur_RPM` — **es la forma del original**, lo dice el mapa de lineas (545/547/550/551/556) | **identicas al ternario**, 96,45252 % |
| k1-k4 | bloque de redlining: `else if`, `-=`, offset en local, `Target` fuera del `if` | k1-k3 identicas; k4 **94,21788 %** |
| r1-r5 | `NormalRPM` en tres sentencias, `m_RPM_LFO` en local, orden de sumandos, `SetEngRPM` con la expresion, `VisualRPM` despues | r1/r2/r4 identicas; r3 96,42458 %; r5 **92,53632 %** |
| s1-s5 | orden de declaracion de las cuatro locales (todas las permutaciones utiles) | **las cinco identicas** |

**Veda**: barridas la sentencia `Cur_RPM = <ternario/if-else>`, la sentencia
`VisualRPM = Cur_RPM;`, la sentencia `NormalRPM = VisualRPM + ... ;`, la
sentencia `VisualRPM = VisualRPM - this->RedlineingVisualOffset;` y el orden de
declaracion de las locales. Ninguna mueve el reparto f1/f31.

**Corregido de paso (0 B, se queda):** `SFXCTL::GetPhysRPM()` en `EAXCar.hpp`
leia `m_pEAXCar->PhysRPM` directamente; el original delega en
`EAXCar::GetPhysRPM()` (el DWARF del original expande el inline anidado en los
tres sitios de esta funcion). Medido: codigo identico al bit, 720 B, 96,45252 %.

### `Play__9EAXCommon18eMenuSoundTriggers` — 492 B, 96,829 %

Firma: `faltan 3, sobran 2, 1 SUST` (`faltan li, bne, b` / `sobran beq, mr`).
**Una sola decision de `jump.c`, 1 instruccion.**

Cabecera del objetivo:

    cmpwi IsSoundEnabled,0 ; beq  BLOQUE_M1
    lwz Debug ; cmpwi 0 ; li r3,-1 ; bne  EPILOGO      <-- el `li` IZADO
    lwz r3,0x24(r28) ; cmpwi r3,0 ; bne  L20
    BLOQUE_M1: li r3,-1 ; b EPILOGO
    L20: lwz r0,0xc(r3) ; cmpwi 0 ; bne CONT ; li r3,0 ; b EPILOGO
    CONT: li r4,1 ; li r5,0 ; bl GetDMixOutput

Nuestra version cruza los TRES `return -1` en un solo bloque, con lo que el
puntero no cae en r3 (cae en r9) y el `li r3,0` del cuarto guardia se puede izar
—justo al reves que el objetivo—, y hace falta `mr r3,r9` antes de la llamada.
Es decir: **el objetivo iza el `li` del guardia de `Debug_Common_FE_OFF` y
nosotros izamos el del guardia de `GetOutputPtr`**; el resto es consecuencia.

25 formas medidas, **todas exactamente 96,82927 % / 488 B / 9 diffs** salvo las
que empeoran:

| grupo | formas | resultado |
|---|---|---|
| c1-c8 | local `SFX_Base *p`, `\|\|` de los dos primeros guardias, `nvol = -1` como valor de retorno, `NULL` explicito, declaracion arriba | identicas |
| c5 | `Debug` antes que `IsSoundEnabled` | **96,74797 %** (peor) |
| d1-d10 | `goto` a salida unica, `if` anidado con `return -1` al final, llaves en los cuatro guardias, `else if` encadenado, `\|\|` de los tres, `do {...} while(0)` con `break` | d3/d5/d6/d7/d8 identicas; d1 95,28455 %; d2 95,68293 %; d9 96,78049 %; d10 **91,90243 %** |
| e1-e3 | `GetOutputPtr(0)` en vez de `GetOutputBlockPtr()` (**es lo que usa el original**, lo dice `dwbody`) con tres cuerpos distintos del accesor | identicas al bit |
| p1-p3 | `PlayCommonSample::SetPitch/SetAzimuth` en forma de copia (por si era como en `_RS`) | identicas |
| q1-q3 | `nvol` declarada en su inicializacion, guardias con llaves, `!= 0` explicito | identicas |
| t1-t5 | `Debug` con `else if` sobre el puntero, `Debug \|\| !ptr`, primer guardia con variable, `goto` propio del guardia de Debug | identicas |

**Veda**: barridas las cuatro sentencias de guardia
(`if (!IsSoundEnabled)`, `if (Debug_Common_FE_OFF)`, `if (!m_pSFXOBJ_FEHUD)`,
`if (!...GetOutputBlockPtr())`), su agrupacion en `||`/`else if`, la forma del
valor de retorno (literal / variable / `goto` a salida unica) y el accesor del
bloque de salida. La decision no es de fuente: es a que bloque le aplica
`jump.c` el izado del `li` antes del cruce de saltos.

**Hallazgo de fidelidad, medido pero NO aplicado**: el original llama a
`SndBase::GetOutputPtr(int index)` (existe en el DWARF de `SndBase`, junto con
`IsMixInConnected`, `IsMixOutConnected`, `SetOutputValue`, `GetGroupID`,
`GetInstanceID`, `GetSFX_ID`, `GetUniqueID`, `SetObjectID`, `GetObjectIndex`,
`GetStaticTypeInfo` — ninguno lo tenemos). Anadirlo a `SndBase.hpp` y llamar
`GetOutputPtr(0)` da **codigo identico al bit** (e1-e3), asi que lo he revertido
para no tocar una cabecera compartida a cambio de nada. Queda apuntado por si a
alguien le hace falta el DWARF exacto. `EAXFrontEnd::Play(void*)` y
`EAXCommon::Play(void*)` (ambas ya al 100 %) tambien lo usan en el original.

## Ficheros tocados

| fichero | por que |
|---|---|
| `EAXAemsManager.cpp` | `InitiateLoad` (ver aviso abajo) |
| `SoundCollision.cpp` | `InitAsImpact` |
| `EAXFrontEnd.cpp` | `EAXFrontEnd::Play(eMenuSoundTriggers)` |
| `SND_GEN/FE_AEMS.h` | **cabecera compartida** — clamp en `PlayFrontEndSample_RS::SetPitch/SetAzimuth` |
| `EAXCar.hpp` | **cabecera compartida** — `SFXCTL::GetPhysRPM()` delega en `EAXCar::GetPhysRPM()` (0 B, fidelidad DWARF) |

Las dos cabeceras compartidas: A/B por objetos hecho. `FE_AEMS.h` solo entra en
`EAXFrontEnd.cpp` (zEAXSound) y `SND_GEN/FE_AEMS.cpp` (zEAXSound2);
`EAXCar.hpp` solo en `Src/EAXSound/**`. Tras el cambio, `measure.py --cmp` da
**zEAXSound2 sin mover un byte** y `frozen.py chk zSpeech` dice `identico al
congelado`. `SndBase.hpp` la toque para un ensayo y la **devolvi byte a byte**.

## Que NO he probado

- **`UpdateRPM`**: no he mirado `lreg.py` (tabla de allocnos del compilador) ni
  `cc1plus -dG` para ver la decision literal de `cse`/`local-alloc` sobre f1/f31;
  es lo primero que haria el siguiente. Tampoco he probado un
  `register float x asm("f1")` (ultimo recurso, y aqui haria falta un pin de
  ENTRADA, no vacio). Tampoco he tocado `SetEngRPM`/`SetEngTorque` ni el bloque
  `PhysicsNewAudioRPM` (que en el original no tiene registro y en el nuestro si).
- **`Play__9EAXCommon`**: no he probado banderas por funcion (`#pragma`); la
  unidad es ProDG/GCC 2.9, no mwcc, asi que el eje de los `#pragma` de la ronda
  20 no aplica. No he probado `permuter.py`. No he mirado si el orden de las
  funciones dentro de `EAXFrontEnd.cpp` cambia el cruce de saltos.
- **El muro** (9 funciones, 6.288 B): no lo he tocado; el encargo eran las cinco
  de `FALTA O SOBRA`.
- No he mirado el resto de la clase `SndBase` que le falta al arbol
  (`IsMixInConnected`, `SetOutputValue`, `GetGroupID`, …): puede haber
  funciones de otras unidades que dependan de esos inlines.
- No he comparado `.sdata`/`.sdata2` byte a byte contra el ELF (`audit.py` no
  las mira enteras).

## Aviso: cambios previos sin commitear

Al empezar, `EAXAemsManager.cpp` y `sfxctl/SFXCTL_MasterVol.cpp` ya venian
modificados de una sesion anterior (merge parcial de `zEAXSound2-r7`). **No son
mios.** Lo que habia:

- `EAXAemsManager.cpp` — en `InitiateLoad`, cuatro sitios donde se habia
  eliminado el local `stBankSlot *pBankSlot` y se leia
  `this->m_pCurLoadSDLP->mBankSlot` en linea.
- `SFXCTL_MasterVol.cpp` — en `UpdateParams`, un temporal
  `__typeof__(RandarRange) __ct8 = ...` alrededor de `SetDMIX_Input(10, ...)`.

**`SFXCTL_MasterVol.cpp` no lo he tocado.** De `EAXAemsManager.cpp` lo mio es
exactamente esto (diff contra el estado que me encontre):

1. local nuevo `int nbytes;` y sus tres usos como cuarto argumento de
   `AddQueuedFile` (antes `result`),
2. `bMalloc(nSize, FileName.GetString(), 0, 0x1040)` en `TMP_ALLOC_MAIN`,
3. `bMalloc(0x10000, "EAXAemsManager::m_pAsyncBuff", 0, 0)` en el camino
   `TMP_ALLOC_MAIN` del buffer asincrono.

Nada mas. Los `goto ReturnResult` / `pBankSlot` estan como me los encontre.
