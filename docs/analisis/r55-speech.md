# r55 — zSpeech: `SpeechManager.hpp` cerrada, y por qué zSpeech NO está a una función

Agente `speech`. Propiedad: `SpeechManager.hpp` (cabecera), `zSpeech` (unidad).
Una sola línea tocada en el árbol.

---

## 0. Titular

**El culpable no era la herencia de `UTL::Std::list` ni el destructor: era el
constructor `SPCHEventList() {}` escrito EN CLASE.** Quitarlo cierra el
sobre-definido en las siete unidades, y está medido en el enlace:

| | `clear__…_List_base<SPCHType_1_EventID>` al promocionar zSpeech |
|---|---|
| antes | **−160.352 B** — se lo lleva otra unidad |
| después (árbol de ahora, vecinas ya reconstruidas) | **+284 B** — se queda en zSpeech |

Los **152.496 B** del censo quedan cerrados, y con ellos el caso latente que en
el mundo todo-promocionado se lo habría llevado **zAI, el objeto 0**, de donde ya
no se recupera por orden de enlace.

**El árbol enlaza el DOL correcto ahora mismo** (`DOL OK`, 0 bytes distintos)
con el arreglo puesto y las seis vecinas ya reconstruidas por sus dueños.

Y el segundo titular es negativo: **zSpeech NO está «a una función de
promocionar»**. Aun con `Setup` al 100 % le quedan `.rodata +8` y el orden de
emisión interno. `trypromo` sigue dando **DOL ROTO**.

---

## 1. La cabecera: qué se probó y qué falló

`src/Speed/Indep/Src/EAXSound/Stream/SpeechManager.hpp:142-145`

```cpp
 struct SPCHEventList : public UTL::Std::list<SPCHType_1_EventID, _type_list>, public AudioMemBase {
   public:
-    SPCHEventList() {}
 };
```

### La ablación

TU-sonda que **sólo** hace `#include` de la cabecera, con los cflags exactos de
zSpeech. Control: una sonda con un error de sintaxis metido en la cabecera
sombreada falla el compilado, así que la sombra estaba realmente en uso.

| forma | ¿emite el COMDAT de 120 B? |
|---|---|
| como está en `HEAD` | **sí** |
| `virtual ~SPCHEventList();` declarado (cuerpo al `.cpp`) | sí |
| sin `public AudioMemBase` | sí |
| heredando `_STL::list<…>` directamente | sí |
| `GetLastEventID()` sacado de la clase | sí |
| truncando la cabecera en la línea 100 | no |
| **sin `SPCHEventList() {}`** | **no** |

El diff completo de símbolos exportados de la sonda entre `HEAD` y el arreglo es
**una sola línea**: desaparece `clear__…SPCHType_1_EventID…` y no cambia nada más.

El mecanismo encaja con `instantiate_decl` (`cp/pt.c:9439-9461`): el constructor
por defecto **inline** de una clase cuya base tiene destructor no trivial es una
función, y su cuerpo se genera en el punto del parseo → `nested = in_function_p()`
→ se instancia `~_List_base` → `clear` **en toda TU que incluya la cabecera**.
Con el constructor implícito, el cuerpo sólo se sintetiza donde se construye el
objeto, que es `SpeechManager.cpp` (`SPCHEventList Manager::mEvtHistory;`), y ése
está en zSpeech y en ningún otro sitio (`grep` de `SPCHEventList` en todo `src/`:
la cabecera, y tres líneas de `SpeechManager.cpp`).

### El efecto, medido

Barrido de los **547** objetos nuestros del árbol buscando el símbolo:

* antes: lo definían **7** (`zAI, zEAXSound, zEAXSound2, zGameplay, zLua, zSim,
  zSpeech`), todos `WEAK`; el original lo define **uno** (`obj/zSpeech.o`, GLOBAL).
* ahora, con las seis vecinas ya recompiladas por sus dueños: lo define **1**,
  `zSpeech`. Igual que el original.

Y dentro del objeto de zSpeech el símbolo **cae en la ranura del objetivo**:

```
OBJETIVO  …DispIntroRace__10MiscSpeech │ clear │ reserve__…SPCHSampleRequest │ __less__… │ __adjust_heap__… │ make_heap__ │ pop_heap__
ANTES     …find__…List_iterator        │ clear │ clear__…IAttachable        │ find__…IVehicleCache │ RaiseToPower__ │ …
DESPUES   …reserve__…SpeechSampleVec   │ clear │ reserve__…SPCHSampleRequest │ __less__… │ __adjust_heap__… │ make_heap__ │ pop_heap__
```

La cadena de sucesores pasa a ser **idéntica** a la del objetivo. Antes estaba en
el racimo temprano de la cabecera.

### Coste en el DOL de hoy: ninguno

A/B limpio, mismo árbol, mismo minuto, sólo cambia la cabecera, objeto
sustituido **con el nombre de fichero correcto** (ver §3):

| | secciones al promocionar zSpeech | bytes de DOL distintos |
|---|---|---:|
| con `SPCHEventList() {}` | `.text +0`, `.rodata +8`, `.data +0` | 215.038 |
| sin él | `.text +0`, `.rodata +8`, `.data +0` | 215.151 |

**+113 B sobre 215.000**: ruido dentro de la permutación interna de zSpeech, con
las secciones idénticas. A cambio se cierra un desplazamiento de 152.496 B que en
cuanto zAI promocione ya no se puede deshacer.

---

## 2. El acoplamiento con `grid` en el pool de zSim — AVISO, no propuesta

**En el árbol de ahora no hace falta nada**: zSim reconstruido da exactamente el
`.o` de `grid` (sha `177ec1a88618`, comprobado byte a byte) y el enlace da
**DOL OK**.

Pero en una instantánea anterior del árbol (≈00:15) medí lo siguiente, y conviene
tenerlo escrito porque puede reaparecer:

* zSim compilado con la cabecera **original** → `DOL OK`.
* zSim compilado con la cabecera **arreglada** → `DOL ROTO`, **66 B**, en dos
  rangos de `.rodata`: `0x8040490C` (48 B) y `0x80404B48` (24 B).
* Causa: la numeración `$LC` de zSim se corre **+1** a partir de `$LC554`, y
  `keep.lst` fija **seis** literales de zSim **por nombre**:

  | keep.lst | contenido (lo dice el propio `# @lc`) | pasa a ser |
  |---|---|---|
  | `zSim.o:$LC554` | `NISName` | `$LC555` |
  | `zSim.o:$LC555` | `MPerpBusted` | `$LC556` |
  | `zSim.o:$LC556` | `Perpetrator` | `$LC557` |
  | `zSim.o:$LC557` | `ISimable` | `$LC558` |
  | `zSim.o:$LC630` | `SimTime` | `$LC631` |
  | `zSim.o:$LC631` | `TimeStep` | `$LC632` |

* **Probado**: con una copia de `keep.lst` con esas seis renumeraciones y ese
  zSim → **DOL OK, 0 bytes distintos**. Y era **atómico**: sólo `keep.lst`, sin
  reconstruir zSim, rompía el DOL en 567 B.
* En el árbol de ahora la edición de `grid` compensa el corrimiento y la
  numeración vuelve a la original. **No propongo tocar `keep.lst`.**

**Qué mirar si el DOL se rompe tras esta ronda**: si los bytes distintos caen en
`0x8040490C` y `0x80404B48`, es esto, y el arreglo son esas seis líneas
(`lcfix.py` las resuelve por contenido). Y quien genere los `$LC` de `keep.lst`
para zAI, zEAXSound, zEAXSound2, zGameplay o zLua al promocionarlas **tiene que
hacerlo con esta cabecera puesta**, porque también les mueve el pool.

---

## 3. TRAMPA NUEVA: `keep.lst` empareja por NOMBRE DE FICHERO del objeto

Me costó dos conclusiones falsas y merece entrar en el catálogo.

`keep.lst` tiene 1.255 entradas de la forma `<objeto>.o:<simbolo>`. El enlazador
las empareja por el **nombre base** del objeto. Si sustituyes un objeto en el
enlace con otra ruta y **otro nombre**, pierdes en silencio todos sus `-keep`:

| objeto | sha del `.o` | `.rodata` enlazada | bytes de DOL distintos |
|---|---|---:|---:|
| `build/GOWE69/src/…/zSpeech.o` | `d0b1ff71aaa9` | **+8** | 215.151 |
| `…/scratchpad/real_fix_zSpeech.o` | `d0b1ff71aaa9` | **−1248** | 276.006 |

**Los mismos bytes exactos**, dos resultados de enlace distintos. Cualquier
herramienta que sustituya objetos —`trypromo`, `dolwhere`, `linkdelta` y las mías—
tiene que conservar el nombre base. Las tres del árbol lo hacen (sustituyen
`obj/X.o` por `src/X.o`); una sonda casera, no.

---

## 4. zSpeech: dónde está de verdad

`fncmp` **antes y después** de tocar nada, idéntico:

    1 de 703 funciones con el CODIGO distinto -- Setup__Q26Speech13RoadblockFlow (596 B, 4 insn)
    122 mas solo con nombres de simbolo distintos (83340 B)

Ni una regresión. Y el resto:

    reorden.py   .text CONTENIDO = 4 palabras (las de Setup) ; .rodata CONTENIDO 0
    linkdelta    .text +0   .rodata +8
    dolwhere     secciones no coinciden (.rodata +8 -> +32 con el relleno de 32 B del DOL)
    trypromo     DOL ROTO (fac385ac7273)   215.151 B de DOL distintos

**Lo que bloquea la promoción son tres cosas, y `Setup` es la más pequeña:**

1. **`.rodata +8`.** El objeto lleva `+368 B` de pool (`lcpool`), de los que el
   enlazador tira casi todo y quedan **8**. Los símbolos `.rodata` **nombrados**
   suman exactamente lo mismo que el objetivo (138.041 B en los dos), así que los
   8 B están en el pool `$LC`/relleno. No lo he localizado: el emparejamiento por
   contenido no sirve porque el objetivo agrupa varias cadenas bajo un solo
   `lbl_` y nosotros las partimos en un `$LC` cada una.
2. **El orden de emisión interno.** `shift.py` sobre el enlace promocionado
   enseña decenas de símbolos propios de zSpeech desplazados entre ±100 y ±3.300 B
   (`_vt.Q26Speech5Cache +3.328`, `_vt.Q26Speech7copList −3.128`,
   `reserve__…SpeechSampleVec −5.356`…). Es el frente de `permorden`, no de código.
3. **`Setup`**, 596 B que no cuentan.

Con `Setup` cerrado, `trypromo` seguiría dando ROTO. La frase del encargo
—«a UNA función de promocionar»— **es falsa**, y conviene corregirla antes de que
alguien repita la ronda contando con ella.

---

## 5. `Setup__Q26Speech13RoadblockFlow`: diagnóstico y siete formas medidas

La única diferencia son **cuatro instrucciones**, en la cuarta llamada virtual
(`primary->PursuitApproaching()`):

```
OBJETIVO   lwz r9,0(r31) │ lwz r0,0x2e4(r9) │ lha r3,0x2e0(r9) │ mtlr r0 │ add r3,r31,r3 │ blrl
NUESTRO    lwz r9,0(r31) │ lha r3,0x2e0(r9) │ lwz r0,0x2e4(r9) │ add r3,r31,r3 │ mtlr r0 │ blrl
```

Mismas instrucciones, permutadas por parejas. **Las otras CUATRO llamadas
virtuales de la misma función casan**, todas con el orden `lha` primero
(`CallForSubRB` 0x2f0, `IsHeli` 0x348, `NegRBReply` 0x2c8, `RBApproach` 0x2d0).
Sólo en ésta el objetivo adelanta la cadena de LR.

Formas probadas, **todas dan byte a byte el mismo bloque**:

1. `__asm__("# d");` **después** de la llamada
2. `__asm__("# d");` **antes**
3. `EAXDispatch *dispatch = ai->GetDispatch();` sacado a local
4. `EAXCop *approacher = primary;` (alias por puntero)
5. `EAXCop &approacher = *primary;` (alias por referencia)
6. `__asm__("" : : : "r0");` antes de la llamada
7. `__asm__("" : "+r"(primary));` antes de la llamada
8. `__asm__("" : : : "lr");` antes de la llamada

Ninguna mueve una sola instrucción. El bloque básico es idéntico al del objetivo
en sus cuatro instrucciones previas, así que la diferencia no viene de la forma
de la sentencia: viene del orden que `sched1` le deja a `sched2`
(ver `nfsmw-rank-for-schedule`). El siguiente paso serio es el volcado RTL
(`cc1plus -dR -fsched-verbose-5`) sobre esta función, no más formas de fuente.

### Y de paso: los tres andamios de `RoadblockFlow.cpp` NO son deuda

Re-medido esta ronda, como manda «los andamios caducan». Quitando los tres
`__asm__("# a"/"# b"/"# c")`:

    Setup: 596 B -> 568 B      (7 instrucciones menos)

El enlazador de bloques funde las tres colas de `blrl` (cross-jumping) y la
función deja de tener el tamaño del objetivo. **Son imprescindibles**; los repuse.

---

## 6. Sorpresas

1. **No era la herencia.** El informe del censo apuntaba a
   `public UTL::Std::list<…>` y al destructor implícito; medido, quitar la
   herencia o declarar el destructor **no cambia nada**. Es el constructor inline.
   La misma sospecha está escrita para `WGridNode.h:28` y para
   `WGridManagedDynamicElem.h`: conviene que `grid` compruebe si allí también es
   el constructor y no la herencia.
2. **`keep.lst` empareja por nombre de fichero** (§3). Dos veredictos falsos.
3. **Mi arreglo y el de `grid` están acoplados** en la numeración `$LC` de zSim
   (§2): cada uno por separado la corrió +1 en la instantánea de las 00:15; los
   dos juntos la dejan igual. Un `keep.lst` que fija literales por nombre hace que
   dos arreglos independientes dejen de serlo.
4. **El `.o` de partida de zSpeech estaba rancio.** El `linkdelta` que medí al
   empezar (`rodata +8`) no lo reproducía una compilación fresca con la cabecera
   original. Medir contra el `.o` que hay en `build/` sin recompilar puede estar
   midiendo el árbol de hace dos horas.

---

## 7. Lo que queda, con cifra

| qué | cuánto | de quién |
|---|---:|---|
| `.rodata +8` de zSpeech | 8 B, y son los que rompen el DOL por el relleno a 32 | mío, sin localizar |
| orden de emisión interno de zSpeech | decenas de símbolos, ±100..3.300 B | mío, frente `permorden` |
| `Setup`: 4 instrucciones de planificación | 596 B que no cuentan | mío, techo hasta el volcado RTL |
| que `grid` compruebe si en sus dos cabeceras el culpable es el constructor | 1.723.208 B | `grid` |

Herramientas de un solo uso: todas en mi scratchpad, ninguna en `scripts/`.
Nada de `config/`, `configure.py`, `splits.txt` ni `keep.lst` tocado.
