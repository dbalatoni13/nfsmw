# r56 — `zEAXSound` + `zEAXSound2`: −20.793 B de DOL, `zEAXSound2` a `IGUAL`, y `linkdelta` mide otra cosa

Agente `eax`. Unidades: `Speed/Indep/SourceLists/zEAXSound` y `…/zEAXSound2`.
Ficheros tocados: **los dos `.cpp` de SourceList y nada más**. Cero cabeceras compartidas,
cero `configure.py`, cero `splits.txt`, **cero `keep.lst`** (§3: no hace falta).
`fncmp` limpio antes y después en las dos.

---

## 0. La medida, y su control

`linkdelta` y `trypromo` siguen siendo el veredicto, pero **no sirven para diagnosticar**
(§2). La sonda de la ronda (borrada al terminar) enlaza **BASE** y **PROMO** en la misma
corrida y da:

* el **tamaño de la ventana** de cada sección de cada unidad, medido como
  `dirección(primer símbolo de la unidad siguiente) − dirección(primer símbolo de la mía)`,
  **restando la misma resta hecha sobre el enlace BASE**;
* los bytes distintos del DOL contra el original, **atribuidos a ventana** (`.text`,
  `.rodata` y `.data` de cada unidad) o a «fuera»;
* la contabilidad de cadenas de la ventana de `.rodata` enlazada.

El control `BASE == DOL original` va impreso en cada corrida y salió `True` en todas las
cifras de abajo.

**El control no es opcional y casi me cuesta la ronda**: sin él medí `.rodata −688`, luego
`−568`, luego `−688` otra vez sobre fuentes que no habían cambiado. La causa es que somos
siete en el mismo árbol y `objetos_del_enlace()` mete los `src/*.o` de las unidades ya
promocionadas: cualquier reconstrucción ajena aguas arriba desplaza la dirección absoluta.

---

## 1. Antes y después

| | zEAXSound | zEAXSound2 |
|---|---|---|
| `fncmp` antes | 0 de 773 | 1 de 930 (`GenerateRoadNoise`, 1.240 B) |
| `fncmp` después | **0 de 773** | **1 de 930** (la misma) |
| `linkdelta` antes | `.text +0  rodata−120 data−416 bss+32` | `.text +0  rodata−48 data−288 bss−32` |
| `linkdelta` después | `.text +0  **bss+32**` | `.text +0  **IGUAL**` |
| ventana `.rodata` antes → después | **−688 → 0** | **−496 → 0** |
| ventana `.data` antes → después | **−408 → 0** | **−236 → 0** |
| ventana `.bss` antes → después | +16 → **+16** | −64 → **0** |
| `sha1` del `.o` antes | `a370fca1bdc3d63bdb5b0b07fbdcdf67851b899e` | `f9720dfa5d4baafd9f8d17c1a36c774229bfdbd5` |
| `sha1` del `.o` después (3 compilaciones iguales) | `694f17bbc8563ca9ffc123c11fa3ae2cbf316501` | `d66c11df22e16bce9f46409a7058f58536245bc7` |
| `trypromo` | `DOL ROTO (0d4693969d3b)` | `DOL ROTO (5d2a0cf071c9)` |

**DOL, bytes distintos con las dos unidades sustituidas: 83.255 → 62.462 (−20.793 B).**

Y lo estructural: **ninguna sección del DOL mide ya distinto del original**. Al empezar,
`.rodata` (−160) y `.data` (−704) eran incomparables y arrastraban **13.000 símbolos**
(escalones `−168` ×8.805 y `≈−872` ×4.400). Ese arrastre es lo que se ha ido:

| bytes distintos | antes | después |
|---|---:|---:|
| **fuera de mis ventanas** (daño colateral en el resto del juego) | **60.887** | **14.444** |
| dentro de `zEAXSound` `.text` / `.rodata` / `.data` | 14.212 | 13.702 / 8.753 / 1.237 |
| dentro de `zEAXSound2` `.text` / `.rodata` / `.data` | 8.156 | 7.798 / 10.826 / 5.702 |

Los 60.887 B de «fuera» eran `lis/addi` de TODO el juego apuntando a `.rodata`/`.data` que
se habían movido. Ya no.

---

## 2. El hallazgo: **`prefijotu` mide el OBJETO y el que manda es el ENLACE**

`python scripts/prefijotu.py zEAXSound` dice **«nos faltan 0 cadenas»**. Y a la ventana
**ENLAZADA** le faltaban **33 cadenas, 704 B**.

Las dos cosas son ciertas:

* cc1plus **sí** emite las 33 (`Attrib::Gen::pursuitescalation`,
  `AUD:PF MUSIC Stream Channel`, `AEMSMGR: async bank load buffer`…), cada una como su
  propio `$LC`;
* nadie las referencia, así que **`-strip-unused-data` se las lleva**;
* el objetivo las tiene **dentro de `lbl_*` gordos** que el troceado nombra (`lbl_803D5E94`
  son 672 B, `lbl_803D6146` son 618 B) y que `keep.lst` mantiene vivos **enteros**: allí
  sobreviven porque el bloque entero tiene dueño.

Y el estripado deja rastro medible: quita **`size & ~7`** bytes y **conserva el resto**. Por
eso la ventana enlazada está sembrada de colillas —`'ects'` (cola de
`Attrib::Gen::effects`, 21 B: se lleva 16 y deja 5), `'lation'`, `'ehicle'`, `'OICE'`,
`'ckable'`…—: **22 en `zEAXSound` (144 B) y 27 en `zEAXSound2` (187 B)**. Un símbolo muerto
de 4 B (`4 & ~7 == 0`) **no se estripa nunca**; ésa es la razón del `.bss` que queda (§5).

### La consecuencia de herramienta, que afecta a las 27 unidades

**`linkdelta` NO mide el déficit de la ventana.** Da el delta de la sección del ELF
completo, y en `.rodata` eso mezcla dos cosas:

    zEAXSound   ventana −688      linkdelta rodata−120
    zEAXSound2  ventana −496      linkdelta rodata−48

La diferencia (568 y 448 B) **reaparece más abajo**, en las bibliotecas que van detrás de
las SourceLists, porque nuestro objeto mantiene vivo allí lo que el original no. Medido
unidad por unidad: el escalón de `.rodata` es **−688 constante** desde `zEAXSound2` hasta
`zWorld2`, las 20 SourceLists siguientes. **La cifra accionable es la ventana, no la
sección**, y la ventana hay que medirla contra un enlace BASE de la misma corrida.

---

## 3. Lo que se hizo, con su cifra

Todo son `asm()` de fichero al final del SourceList. **Ni una entrada nueva en `keep.lst`**:
los nombres que emito ya están ahí desde el troceado, y por eso sobreviven al estripado.

| # | edición | fichero | efecto |
|---|---|---|---|
| 1 | **26 blobs de `.data`** (`lbl_804176*`…`gap_06_80417ABC_data`), 418 B, contenido literal del objeto original | `zEAXSound.cpp` | ventana `.data` **−408 → +8** |
| 2 | fuera `gap_06_804176A0_data` (8 B): esos 8 B ya los aportan `DEBUG_360MEM` y `g_NextStreamHandle`, que el troceado no nombra | `zEAXSound.cpp` | **+8 → 0** |
| 3 | **33 cadenas** que le faltan a la ventana enlazada, en el orden del objetivo, 704 B | `zEAXSound.cpp` | ventana `.rodata` **−688 → +16** |
| 4 | fuera `"AUD:EAXFrontEnd"` (16 B) | `zEAXSound.cpp` | **+16 → 0** |
| 5 | **15 blobs de `.data`** (`pad_06_80417DA8_data`…`lbl_8041897C`), 256 B | `zEAXSound2.cpp` | ventana `.data` **−236 → −44** |
| 6 | **`gap_06_8041A570_data`** (52 B): es nuestro `szMainMapStates`, que en nuestro árbol está MUERTO y el enlazador se lleva entero (52 B ≥ 8) | `zEAXSound2.cpp` | **−44 → +8** |
| 7 | fuera `gap_06_80418290_data` (8 B) | `zEAXSound2.cpp` | **+8 → 0** |
| 8 | **31 cadenas** que le faltan a la ventana enlazada, 572 B | `zEAXSound2.cpp` | ventana `.rodata` **−496 → +80** |
| 9 | fuera dos **duplicados reales** del bloque de la r52 —`"Attrib::Gen::gameplay"` y `"CARSFX_WindWeather"`, que el objetivo **no tiene** en esa ventana— más `"MControlPathfinder"` y `"Attrib::Gen::speech"` | `zEAXSound2.cpp` | **+80 → 0** |
| 10 | **64 B de `.bss`** anónimos (el objetivo tiene 284 B de hueco en `gap_07_8045DE68/DEE0/EC1C_bss` y nosotros cubrimos 220 con estáticas de más) | `zEAXSound2.cpp` | `.bss` **−64 → 0**, y `linkdelta` pasa a **IGUAL** |

Los recortes 4, 7 y 9 son la deuda honesta de la ronda: **tres cadenas y un hueco de 8 B que
el objetivo SÍ tiene y nosotros dejamos fuera a propósito**, para absorber los 144/187 B de
colillas de `size & ~7` que no se pueden quitar sin dejar de emitir el `$LC` muerto. Están
nombrados uno a uno arriba para que la ronda que mate las colillas los devuelva.

### El anónimo sobrevive y el nombrado no

Contra lo que parecía: un bloque `.rodata`/`.bss` **sin símbolo** no lo estripa nadie (el
bloque de 71 cadenas de la r52 lleva vivo desde entonces). Lo que se estripa es el símbolo
con nombre y sin referencias. Por eso el `.bss` de la edición 10 va anónimo y los blobs de
`.data` van con el nombre que `keep.lst` ya lista.

---

## 4. Un negativo medido y REVERTIDO: el orden de `.data` no paga a trozos

**Confirmado por construcción**: nuestro orden de `.data` es **exactamente** el orden de
definición de los globales en el fuente (GCC 2.9 llama a `assemble_variable` en el punto de
la definición). `EAXSound.cpp:72-100` sale palabra por palabra en el mismo orden en nuestro
`.o`. Y el objetivo tiene **otro** orden, que se lee entero del objeto troceado:

    objetivo:  PlayerUpgrade.14976, gap(24), AudioMemoryPool, lbl_80417654(60),
               gbHasStartNewGamePlayBeenProcessed, gnHasStartLoadFEBeenProcessed,
               gIsPauseForPause, g_pEAXSound, gap(8), g_pcsCSISAllocString, NullPointer,
               g_fMasterSFXVolume, g_iMasterSFXVolume, lbl_804176B8(36), SoundRandomSeed, ...
    nuestro:   DEBUG_360MEM, AudioMemoryPool, SoundRandomSeed, g_SliderValue,
               g_fMasterSFXVolume, g_iMasterSFXVolume, g_pcsCSISAllocString, ...

Probé el primer escalón —mover esas cuatro (`gbHasStartNewGamePlayBeenProcessed`,
`gnHasStartLoadFEBeenProcessed`, `gIsPauseForPause`, `g_pEAXSound`) delante y reordenar
`g_fMasterSFXVolume`/`SoundRandomSeed`/`g_SliderValue`— y el DOL salió **+5 B PEOR**
(62.462 → 62.467: `.data` −1, `.text` **+6**). **Revertido.**

La lección: mover globales **también mueve el pool de literales y de flotantes** de la
unidad, así que el `.data` a trozos sale negativo. **O se transcribe la secuencia entera
—globales reordenados y cada `lbl_`/`gap_` en su hueco, no al final del SourceList— o no se
toca.** Vale 6.939 B (1.237 + 5.702) y el orden objetivo está escrito en
`build/GOWE69/obj/…/zEAXSound*.o`.

---

## 5. Lo que queda, con cifra

`zEAXSound2` está a **`IGUAL` en todas las secciones**: sólo le falta CONTENIDO/ORDEN.
`zEAXSound` sólo tiene abierto el `.bss`.

1. **`zEAXSound` `.bss`: ventana +16 B** (sección `+32`). Es lo único que le impide llegar a
   `IGUAL`. Causa medida: emitimos **66 estáticas muertas de 4 B** (`k.*`, `hash.*`,
   `_.tmp_*`, del patrón `static UCrc32 hash = "…"`) contra las 26 del objetivo, y **un
   símbolo muerto de 4 B no se estripa jamás** (`4 & ~7 == 0`). No se cierra añadiendo:
   hay que dejar de instanciar cuatro de esos pares.
2. **Orden dentro de mis ventanas: 47.918 B.** `zEAXSound` `.text` 13.702 + `.rodata` 8.753
   + `.data` 1.237; `zEAXSound2` `.text` 7.798 + `.rodata` 10.826 + `.data` 5.702. Las tres
   familias son transcripción, no búsqueda: el orden objetivo está en el objeto troceado.
3. **Fuera de mis ventanas quedan 14.444 B** (eran 60.887). Lo que queda ahí es el escalón
   de `.bss` del punto 1 más las reubicaciones que apuntan a símbolos míos que siguen
   permutados; se irá con 1 y 2.
4. **`GenerateRoadNoise`** (1.240 B) sigue siendo la única función que no casa. No la he
   tocado esta ronda: era ronda de datos y el diagnóstico de la r50/r55 sigue en pie
   (índices 88–125, la carga del literal `0.1f` adelantada, candidato a pin `f10`).

### Correcciones de contenido detectadas y NO aplicadas

* **`AEMSMGR: async bank load buffer`**: el objetivo la tiene en `803D6C60`, pegada delante
  de `EAXAemsManager::m_pAsyncBuff` (`803D6C80`). El orden del pool dice que el original
  usaba **una cadena distinta en cada uno de los dos sitios** de
  `EAXAemsManager.cpp:503` (`TheTrackStreamer.AllocateUserMemory`) y `:514` (`bMalloc`), y
  nuestro árbol repite `"EAXAemsManager::m_pAsyncBuff"` en los dos. La he metido de momento
  como cadena muerta en el bloque; **el arreglo bueno es cambiar el literal de la línea
  503**, y entonces sobra la copia del bloque de la r52 (29 B, hoy duplicada de verdad).
* **`DataToPlayBack`** (`CARSFX_Engine.cpp:28`) mide **16 B** en nuestro árbol
  (`unsigned int [1][4]`) y **4 B** en el troceado. Si el troceado tiene razón son 12 B, y
  es lo que me obligó a dejar fuera `gap_06_80418290_data`. No lo toco: cambiar la
  declaración cambia código.
* **`SPECIAL_EVENTS`** (8 B) y **`LastRandom.26966`** (4 B) de `zEAXSound2` están muertas y
  el enlazador se lleva `SPECIAL_EVENTS` entera.

---

## 6. Sorpresas

### 6.1 `rodorden.py` **sí** arranca en esta unidad: quiere el nombre PELADO

El encargo avisaba de «`rodorden` no arranca — sin rango de `.rodata` en `splits.txt`».
**El rango está en `splits.txt`** (`.rodata start:0x803D5E38 end:0x803D9050`, línea 72). Lo
que pasa es que `rodorden.py` indexa por `os.path.basename(...)` sin extensión, así que
`python scripts/rodorden.py Speed/Indep/SourceLists/zEAXSound` falla y
`python scripts/rodorden.py zEAXSound` funciona. **No hay nada que proponer al jefe.**
(Y ojo: `rodorden` lee el OBJETO, así que sobre esta unidad decía «277 objetivo, 291
nuestro» cuando a la ventana enlazada le faltaban 33 — el mismo error de §2.)

### 6.2 `build_direct.py zEAXSound` reconstruye **las dos**, y `zEAXSound2.o` cambió sin que yo tocara su fuente

Ya lo avisó la r55 (empareja por prefijo). Lo nuevo: el `sha1` de `zEAXSound2.o` cambió
`f9720dfa → 7aee7aeb` en la primera reconstrucción de la ronda **con la fuente intacta**.
No es el fallo de reproducibilidad del brief: es que `zEAXSound2` incluye cabeceras de otros
(Sim, Libs/Support) y **otro agente las estaba editando**. La consecuencia práctica es que
**la línea base de una unidad caduca en cuanto otro agente toca una cabecera compartida**:
hay que re-medirla, no arrastrar la del principio de la ronda.

### 6.3 El `.data` del original guarda ceros porque GCC 2.95 no tiene `flag_zero_initialized_in_bss`

Los `gap_06_*_data` del troceado son casi todos ceros, y no son relleno: en GCC 2.95
`assemble_variable` sólo manda a `.bss` lo que tiene `DECL_INITIAL == 0`, así que un
`int x = 0;` **va a `.data` con su palabra a cero**. Eso explica por qué el objetivo tiene
418 B de ceros con nombre donde nosotros no tenemos nada.

---

## 7. Atomicidad y propiedad

Ninguna edición es atómica con otro agente y ninguna toca fichero ajeno. Los dos únicos
ficheros modificados son `src/Speed/Indep/SourceLists/zEAXSound.cpp` y `…/zEAXSound2.cpp`.
`src/Speed/Indep/Src/EAXSound/Stream/SpeechManager.cpp` ya venía modificado del árbol al
empezar la ronda; **no lo he tocado**.
