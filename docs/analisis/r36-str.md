# r36 · grupo `str` — el frente de las cadenas: qué era de verdad

## 0. Verificación del encargo, y en qué falla la premisa

**Lo que sí reproduce, exacto:**

    grep -rn '"TODO"' src/ --include=*.cpp --include=*.h --include=*.hpp   ->  104
    de ellos ProfileNode                                                  ->   75
    resto (bMalloc / placement-new / AttribAlloc)                         ->   29

Y el hueco de `.rodata` también: midiendo el objeto extraído contra el nuestro,
**23.982 B de cadenas ausentes**, que es la misma cifra que el brief da como
22.746 B (la diferencia es el filtro de qué cuenta como cadena).

**Lo que NO reproduce, y cambia el plan de las 20 unidades:**

1. **En el objetivo no hay cadena en 95 de los 104 sitios.** El objeto extraído
   **no contiene `"TODO"` en ninguna unidad**, pero tampoco contiene un nombre de
   ámbito para casi ningún marcador. Barrido el árbol entero buscando cadenas
   ausentes con pinta de ámbito (`()`, `->`, `::`), **salen 29 en todo el juego**,
   repartidas en 8 unidades, y sólo **3** caen en un sitio con marcador `"TODO"`.
   En `zTrack` los 16 marcadores dan **0** cadenas; en `zEAXSound2` los 6 dan **0**;
   en `zAI` los 10 dan **0**.

   La forma que el original tenía es `ProfileNode profile_node;` **sin argumento**
   (el árbol ya la tiene escrita así en `CarLoader.cpp:1845`, `profile_node2`).
   Los `"TODO"` no son un hueco: son un argumento inventado.

2. **Escribir la cadena buena desde la fuente NO acerca el enlace ni un byte**,
   porque **todas estas cadenas están MUERTAS**. Medido de dos maneras
   independientes:

   - Por reubicaciones del objeto EXTRAÍDO: de las cadenas que nos faltan,
     **19.075 B no están referenciadas por nada** y sólo **280 B** lo están.
   - Por experimento directo (`c36str_probe2`, enlace base contra enlace con
     nuestro `zWorld.o`): `"CarLoaderPool"`, `"TestAlloc"` y `"CarLoaderDefragment"`
     **están hoy en nuestra fuente** como argumento de `bMalloc` y **no aparecen en
     nuestro enlace**. `-strip-unused-data` se las lleva enteras.

   La causa es la de siempre y ahora está cerrada: `bMalloc(size, name, line, pool)`
   es **inline y descarta `name`** (`bWare.hpp:97`), igual que `AttribAlloc::Allocate`
   y que `ProfileNode`. En el objetivo esas cadenas sobreviven **por vecindad**: dtk
   las mete dentro de un símbolo grande (`lbl_803D6758`, 264 B) cuyo primer byte sí
   está referenciado. En nuestro objeto cada literal es su propio `$LCnn` y muere solo.

**Conclusión operativa: el hueco de `.rodata` de las 20 unidades no se cierra
escribiendo cadenas en C++.** Se cierra con el pool escrito a mano de `r35-att`, y
lo que hay que escribir a mano es **casi todo el pool**, no sólo las que faltan
(nuestras cadenas muertas también se estripan). Cuánto, por unidad, medido:

| | `.rodata` objetivo | muerta en el objetivo | muerta en NUESTRO objeto (se estripa) |
|---|---|---|---|
| zWorld | 17.040 | 1.618 | 910 |
| zFe2 | 25.704 | 1.057 | 1.074 |
| zGameplay | 6.128 | 1.131 | 718 |
| zPlatform | 6.712 | 1.172 | 759 |
| zEAXSound | 12.824 | 890 | 1.038 |
| zTrack | 3.488 | **18** | 508 |

(`scripts/` nuevos en el scratchpad; ver §5.)

---

## 1. Las cadenas de ámbito que SÍ faltan — las 29 de todo el juego

Éstas son las únicas que existen. Ninguna de las 20 primeras está en un sitio con
marcador: **los sitios no existen en nuestro árbol** y hay que reconstruirlos.

```
zEAXSound   (bloque contiguo en +0x092C..+0x0A27, dentro de lbl_803D6758)
   gAEMSMgr.Update()            m_pEAXSND8Wrapper->Update()
   m_pSTICH_Playback->Update(t) Speech::Manager::Update()
   GetFrontEnd()->Update()      m_pStateMgr[n]->UpdateParams()
   m_pNFSMixMaster->ProcessMixMap(t)  m_pStateMgr[n]->ProcessUpdate()
   m_pCmnSnd->Update()
   -> son, EN ORDEN, las nueve sentencias de EAXSound.cpp:1026..1064.
zGameplay   GRaceStatus SplitChars()   GRaceStatus ParseArray()
            GManager::InitializeVaults()   AddVault()   CalcSpaceRequired()
zEagl4Anim  EAGL4Anim::BankStat::mBankFileName   DynamicLoader::AddPatchAddress
            ScratchBuffer::mBuffer   @EAGL4::dynamic destructor list
zFEng       FEngine::Render()   SortObjects()   RenderObjectList()
zFe         cFEngJoyInput::ActionQueue   Realmc::IMutex   Realmc::IThread
zFoundation StringPool Attrib::StringKey[]   StringRegistry::CreateLowerCaseKey
zCamera     ICE::Cubic3D   ICE::Cubic1D
zFe2        Sound::stSongInfo
```

**Las 3 que sí caen en un marcador** (todas en `CarLoader.cpp`, todas verificadas
por posición contra el objeto extraído) están **aplicadas**:

| sitio | cadena recuperada | prueba de posición |
|---|---|---|
| `CarLoader.cpp:396` `profile_node.Begin` | `"Remove Duplicates"` | +0x2CA8, entre `"WHEELS"` (+0x2CA0, línea 283) y `"%s(%d)"` (+0x2CBC, línea 476) |
| `CarLoader.cpp:1847` | `"Defragment Textures"` | +0x2DC4, tras `"TestAlloc"` (línea 1720) |
| `CarLoader.cpp:1852` | `"Defragment Solids"` | +0x2DD8, antes de `"Main Pool"` (línea 1878) |
| `CarLoader.cpp:1858` | `"Defragment Textures"` | tercera `Begin`, mismo literal: el objetivo tiene **dos** cadenas para **tres** sitios, y cuadra |

---

## 2. Lo que sí es un hallazgo: `Hermes.h` mete `"TODO"` en 20 unidades

`Speed/Indep/Src/Misc/Hermes.h:16,20` tenía

    inline void *DefaultTableAllocFunc(size_t bytes)  { return AttribAlloc::Allocate(bytes, "TODO"); }
    inline void DefaultTableFreeFunc(void*, size_t b)  { AttribAlloc::Free(ptr, b, "TODO"); }

Hermes.h entra por las cabeceras generadas de `Messages/`, así que esas dos
funciones `inline` —que casi nunca se emiten— **internan `"TODO"` en 20 de las 28
SourceLists**, justo detrás del bloque `Attrib::Gen::*`. Es el mecanismo de
`r35-att §1` funcionando en contra.

Cambiado a `nullptr` (el nombre lo descarta `AttribAlloc::Allocate`, así que no
cuesta una instrucción). Efecto **aislado** en cinco unidades que no toco de
ninguna otra forma:

| unidad | `.rodata` antes | después | `.text` |
|---|---|---|---|
| zSpeech | −1.408 | −1.416 | −4 → −4 |
| zPhysics | −1.576 | −1.584 | −8 → −8 |
| zCamera | −1.912 | −1.920 | +16 → +16 |
| zLua | −2.056 | −2.064 | +0 → +0 |
| zMain | −2.912 | −2.920 | +0 → +0 |

**Exactamente −8 B por unidad y `.text` intacto.** Los 8 B son los 5 de `"TODO"`
más el relleno: la cadena **estaba sobreviviendo al enlace** y el objetivo no la
tiene en ninguna parte (comprobado sobre la `.rodata` del ELF base).

**Aviso de método: el número de `linkdelta` EMPEORA al quitar dato equivocado.**
Es un delta neto: un byte que sobra compensa a uno que falta. Quitar `"TODO"`
resta 8 B a un delta ya negativo y parece un retroceso; es lo contrario. Para
juzgar hay que mirar *falta/sobra* por separado, no el neto.

---

## 3. Lo que queda aplicado en el árbol (sin commitear)

**Base y medida seguidas; `main.dol` base verificado `9619ba57…` con todos los
cambios dentro.** `zFe2` y `zFeOverlay` no enlazan, y **no es mío**: comprobado
revirtiendo `Hermes.h` y reconstruyendo `zFe2`, sigue fallando igual
(`Callback__CQ313FEPlayerCarDB…` indefinidos; en el árbol están vivos
`AttribHashMap.h`, `AttribSys.h`, `AttribPrivate.h` y `attribuserinclude.h` de
otro agente). `zFeOverlay` falla por `_overlay_end` del `zMisc.o` **extraído**.

### 3.1 Marcadores retirados: 82 de 104

- **64 `ProfileNode <var>("TODO"|"TODO2"|"TODO3", 0|1)` → `ProfileNode <var>;`**
  en 33 ficheros (62 de ellos son de los 104; los otros 2 son el `"TODO2"` y el
  `"TODO3"` de `AIGoal.cpp`, que el grep del encargo no cuenta), más los 17 de
  `CarLoader.cpp` (13 constructores + 4 `Begin`). 62 + 17 + 2 (`Hermes.h`) + 1
  (`SuspensionTraffic`) = **82 de los 104**.
- **`Hermes.h` ×2** → `nullptr` (§2).
- **`SuspensionTraffic.cpp:352`** `EnableProfile("TODO")` → `"SuspensionTraffic"`.
  No es adivinar: las otras nueve `EnableProfile` del árbol llevan su nombre de
  clase, y el objetivo tiene `SuspensionTraffic` en +0xF14, **entre**
  `SuspensionRacer` y `SuspensionSimple`; el nuestro estaba al final (+0x7618).
- **4 nombres INVENTADOS retirados** — el objetivo no los tiene en ninguna parte:
  `AITrafficManager::OnTask` (AITrafficManager.cpp:601),
  `AIPerpVehicle::Update` (AIVehicle.cpp:1835),
  `EventManager::RunEvents` (Event.cpp:29),
  `SoundAI::OnTask` (SoundAI.cpp:1246).
  Los dos de `zAI` estaban **vivos** en nuestro objeto: costaban bytes de enlace.
  Los 16 nombres de `Main.cpp`/`zMisc` (`MainLoop()`, `Scheduler::Run()`,
  `eDisplayFrame()`…) **sí están en el objetivo: no se tocan.**

Quedan **22** marcadores, todos `bMalloc`/`new`/placement-new, en 4 unidades
(`zAnim`, `zEcstasy`, `zFe`, `zMisc`); ver §4.

### 3.2 Cadenas recuperadas o corregidas

| fichero | antes | ahora | prueba |
|---|---|---|---|
| `World/CarLoader.cpp` ×4 | `"TODO"` | `Remove Duplicates` / `Defragment Textures` ×2 / `Defragment Solids` | §1 |
| `Animation/AnimEntity.cpp:10` | `new ("CPopAnimEntity", 0) CPropAnimEntity()` | `new ("CPropAnimEntity", 0)` | el objetivo tiene `CPropAnimEntity` en +0x800, detrás de `CBasicCharacterAnimEntity`; era una errata |
| `EAXSound/SND_GEN/ENGINES_AEMS2.cpp:11` | `CAR_SWTNId = {"CAR_SWT", …}` | `{"CAR_SWTN", …}` | el objetivo tiene `CAR_SWTN` (+0x1930) y no `CAR_SWT`; **viva** en los dos lados |
| `Animation/AnimEngineManager.cpp:42` | `bMalloc(size, name, 0, …)` | `bMalloc(size, name ? name : "Unnamed MyEAGLNewOverride", 0, …)` | el objetivo emite `EAGL4Anim Memory Pool` y **luego** `Unnamed MyEAGLNewOverride` en el mismo símbolo (+0x7B0, 96 B). La rama `EA_BUILD_A124` no era una alternativa: el original tenía **las dos cadenas en la misma sentencia**. `.text` sin cambio (el nombre lo descarta el `bMalloc` inline) |

### 3.3 La medida, unidad por unidad

A/B en la misma ventana (revertidos mis ficheros → construir → medir → restaurar →
construir → medir), para que no la contaminen los otros agentes:

| unidad | `.rodata` A | `.rodata` B | Δ | objeto: hueco A → B | cadenas que faltan A → B |
|---|---|---|---|---|---|
| zWorld | −4.720 | −4.712 | **+8** | −3.456 → **−3.400** | 3.351 → **3.295 B** |
| zAnim | −1.560 | −1.568 | −8 | −728 → **−704** | 1.235 → **1.193 B** |
| zEAXSound2 | −1.760 | −1.760 | 0 | −424 → **−416** | 702 → **693 B** |
| zAI | −1.736 | −1.752 | −16 | −688 → −704 | 811 → 811 (sobra 310 → **298**) |
| zTrack | −2.128 | −2.136 | −8 | −1.336 → −1.344 | sobra 194 → **189** |
| zSim, zFe, zMisc, zEcstasy, zWorld2, zPhysicsBehaviors, zFe2 | — | — | 0 | | |
| **neto** | | | **−24 B** | **+98 B de dato correcto** | **−107 B ausentes** |

Más el barrido de `Hermes.h` (§2): **−8 B de neto × 20 unidades**, todos ellos
dato equivocado retirado, y las dos cadenas inventadas de `zAI` (−16 B más).

O sea: **al enlace, cero.** Al objeto, 98 B de cadena buena escrita y ~200 B de
cadena inventada retirada, y `.text` **idéntico en las 28 unidades** (comprobado
en las dos medidas: ninguna fila de `linkdelta` cambia su `.text`).

---

## 4. Lo que dejo medido para el que siga

### 4.1 Cadenas VIVAS del objetivo que NO están en nuestro objeto — **son 4**

Comprobado por contención literal (no por multiconjunto, que da falsos):

```
zEAXSound2   FX_ROADNOISE_TRANS
zFe          FEAnyTutorial.fng      (tenemos FEAnyTutorialScreen.fng,
                                     FEAnyTutorialScreen.cpp:25)
zWorld       QUEUED     LOADED      (vivas en el objetivo, luego la funcion que
                                     las imprime NO esta estripada del todo)
```

**Esto es lo único del frente de cadenas que mueve el enlace por sí solo.** El
resto de «FALTA-VIVA» que canta un diff por multiconjunto es ruido: `zSpeech`
cantaba `AUD:Relocated speech headers` y `AUD: SED_NISSFX events` y **las dos
están en nuestro objeto** (`SpeechManager.cpp:786` y `:2176`); el objetivo tiene
dos copias y nosotros una.

### 4.2 Cadenas VIVAS nuestras que el objetivo no tiene (quitados los `__FILE__`)

35, y son dos frentes de otro:

- **`zGameModes` (15)** y **`zAnim` (12 `*.fng` de Customize)**: son tablas
  enteras en la unidad equivocada — frente de composición de SourceLists.
- `zFe`: `AOInteractiveMusicMode`, `FEAnyTutorialScreen.fng`.
- `zDebug`: `default`. `zMisc`: `%s - %s`.

### 4.3 Los 22 marcadores que quedan, y por qué no los toco

Todos son el nombre de una asignación (`bMalloc`, `new(tag)`), todos MUERTOS, y
en las cuatro unidades donde viven **el diff de cadenas no acusa ninguna ausencia**
en el sitio correspondiente (`zEcstasy` y `zMisc` tienen **0** cadenas ausentes y
cientos de sobrantes: su problema es qué ficheros lleva la SourceList, no las
cadenas). Comprobado además por vecindad: donde nuestro objeto pone `"TODO"`, el
objetivo **no tiene nada**.

    EcstasyE.cpp        1899, 1953
    EcstasyEx.cpp       286, 293, 300, 617
    eStreamingPack.cpp  300, 372, 430, 569, 619, 646, 730, 749, 781
    ResourceLoader.cpp  341, 721, 767
    FEGameInterface.cpp 82
    AnimInternal.cpp    6      (new ("TODO") EAGL4::DynamicLoader)
    Hermes.cpp          51, 107

El candidato con más pinta es `AnimInternal.cpp:6`, pero el nombre que le tocaría
—de la familia `EAGL4Anim::BankStat::mBankFileName`, `ScratchBuffer::mBuffer`—
está en `zEagl4Anim`, no en `zAnim`, así que no se puede fijar por posición.

---

## 5. Herramientas (en el scratchpad, borradas al acabar; el código está aquí)

Cuatro medidas que no existían y que valen para cualquier unidad:

| | |
|---|---|
| `c36str_strdiff.py` | diff de **multiconjunto** de cadenas objetivo/nuestro por unidad, con bytes que faltan y que sobran. A diferencia de `datacmp.py` **no mide posición**, así que no se hunde cuando metes 16 B correctos en medio |
| `c36str_live.py` | separa las cadenas ausentes en **VIVAS** (referenciadas por una reubicación del objeto extraído) y **MUERTAS**. Es la medida que decide si una cadena se puede arreglar desde la fuente o hay que escribirla a mano. Total del juego: **280 B vivas contra 19.075 B muertas** |
| `c36str_dead.py` | cuánta `.rodata` **muerta** tiene cada unidad, en el objetivo y en el nuestro: lo primero es lo que hay que escribir a mano, lo segundo lo que el enlazador nos quita |
| `c36str_probe2.py` | enlaza base y sustitución y **busca cadenas concretas en la `.rodata` del ELF**. Es lo que convirtió «se estripan» de hipótesis en medida |

---

## 6. Vedas de esta ronda

- **VEDA: rellenar los marcadores `"TODO"` no desbloquea ninguna unidad.**
  Barrida la sentencia «faltan 104 cadenas de ámbito y son 22 kB»: en el objetivo
  hay **29** cadenas de ámbito ausentes en todo el juego, sólo **3** coinciden con
  un marcador, y las **104** son MUERTAS, así que ni escribiéndolas todas cambia
  un byte del enlace. Lo que sí valía de ese frente es lo contrario: **retirar**
  el `"TODO"` inventado, que sí estaba llegando al DOL.
- **VEDA: `bMalloc(size, "nombre", línea, pool)` no hace viva la cadena.** El de
  cuatro argumentos es `inline` y reenvía al de dos (`bWare.hpp:97`); el nombre se
  descarta. Igual `AttribAlloc::Allocate/Free` y `ProfileNode::ProfileNode/Begin`.
  Probado sobre el enlace: `CarLoaderPool`, `TestAlloc` y `CarLoaderDefragment`
  están en nuestra fuente y **no** en nuestro DOL.
- **AVISO: el neto de `linkdelta` premia el dato equivocado.** Quitar una cadena
  que sobra empeora el número y mejora el estado. Contrástalo siempre con el
  falta/sobra del objeto.
- **AVISO: un diff de cadenas por multiconjunto da falsos «FALTA-VIVA»** cuando el
  objetivo tiene dos copias de un literal (las dos vueltas del pool de
  `finish_file`, `r35-att §1`). Verifica por contención literal antes de tocar:
  de 30 candidatas, **4** eran reales.
- **`ProfileNode profile_node;` y `ProfileNode profile_node("x", 0);` generan el
  mismo código** (clase vacía, cuerpos vacíos). Medido: `.text` idéntico en las 28
  unidades tras retirar 82 argumentos.

**No se ha tocado `configure.py`, `config/GOWE69/*` ni `splits.txt`. No hay
commits. Temporales `c36str_*` borrados.**
