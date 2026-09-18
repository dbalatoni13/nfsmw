# r53-cerca2 — zFe y zEAXSound: el «vocabulario» era el argumento de `new`

**AVISO, LÉELO ANTES DE MEDIR NADA.** El paquete es **ATÓMICO**: fuente +
`keep.lst`. Las 55 entradas nuevas de `keep.lst` **no están puestas** (la regla 3
me lo prohíbe) y **sin ellas el árbol está peor que antes**:

| unidad | r52 | hoy, sin las entradas | hoy, con las entradas |
|---|---|---|---|
| zFe | `rodata−192 data−32` | **`rodata−624`** | **`rodata−48 data−32`** |
| zEAXSound | `rodata−104 data−384 bss+32` | **`rodata−208`** | **`rodata−104 data−384 bss+32`** |

Se regeneran solas y verificadas:

    python scripts/deadlink.py zFe --keep        >> config/GOWE69/keep.lst   # 49 entradas
    python scripts/deadlink.py zEAXSound --keep  >> config/GOWE69/keep.lst   #  6 entradas
    python scripts/lcfix.py --check

`fncmp` sigue en **0** en las dos (921 y 773 funciones). Base del enlace
verificada `DOL OK` con el `keep.lst` del repo y con el ampliado.

---

## Lo que encontré: el «vocabulario» no son literales muertos, es un ARGUMENTO

El bloque de `asm(".asciz ...")` que la r52 pegó al final de cada SourceList
—117 cadenas y 1.846 B en `zFe`— **no es vocabulario compartido de bWare/STL**.
La mayoría son el **primer argumento del `operator new(size_t, const char*, int)`
de placement** que usa todo el frontend, y del `bMalloc(size, name, line, flags)`
de bWare.

Nuestro árbol escribe

```cpp
mInstance = new (__FILE__, __LINE__) FEManager;          // FEManager.cpp:63
SMSSortNode *node = new (__FILE__, __LINE__) SMSSortNode(msg);
FEScrollyBookEnd *b = new ("", 0) pm_RestartRace;        // uiPause.cpp
ScrollerSlot *pSlot = BNEW ScrollerSlot;                 // BNEW == new (__FILE__, __LINE__)
```

y el original escribía

```cpp
mInstance = new ("FEManager", __LINE__) FEManager;
SMSSortNode *node = new ("SMSSortNode", __LINE__) SMSSortNode(msg);
... new ("pm_RestartRace", 0) pm_RestartRace;
... new ("ScrollerSlot", __LINE__) ScrollerSlot;
```

Tres consecuencias, las tres medidas:

1. **El código NO cambia.** Sólo cambia el destino de la reubicación del
   `lis/addi`. `fncmp` se quedó en `0 de 921` tras convertir **99 sitios**.
   (El argumento va a un registro de argumento, que la llamada machaca, así que
   GCC lo rematerializa en cada sitio: no hay CSE que perder.)
2. **La cadena aparece SOLA en el sitio correcto del pool** —justo delante de la
   función que la usa—, que es exactamente donde el objetivo la tiene. El bloque
   al final del `.cpp` la ponía 10 kB más allá.
3. **Mata el literal de `__FILE__`**, que es puro sobrante: el objetivo **no
   tiene ni una sola ruta `.cpp` en su `.rodata`** (`obj src/ = 0`, `.cpp = 0`;
   lo único con barras es el `d:/mw/speed/indep/bware/inc/bware.hpp` del prefijo
   escrito a mano). En zFe eran 20 literales de ruta; quedan 7.

### La prueba

De los 101 sitios `new(__FILE__|"", …)` del TU de `zFe`, **42 nombres de clase
distintos**, y **los 42 están en la `.rodata` del objetivo**, uno por sitio, en
el orden de la fuente. Los dos que NO están (`JukeboxEntry`, `UIEATraxScreen`)
no se convirtieron: en el objetivo esos sitios llevan otro texto
(`"JukeboxEntry[] - backup copy"`).

### Cómo se aplica a otra unidad

```
grep -rnE 'new *\( *(__FILE__|"") *,' <los .cpp del TU>
```

- el texto es el **nombre de la clase que se construye** (comprobable: tiene que
  estar en la ventana de `.rodata` del objetivo, `rodorden.py`/`prefijotu.py`);
- hay excepciones con texto propio: `EAX_HeliState` → `"Aud: EAX_HeliState"`,
  `EAXS_StreamChannel` → `"AUD:PF MUSIC Stream Channel"` / `"AUD:NIS/SFX Stream
  Channel"` / `"AUD:Speech Stream Channel"`, `EAXCommon` → `"AUD:EAXCommon"`;
- **quita el nombre del bloque `asm()`** o sale dos veces (el `asm` es texto
  crudo, no dedupa con el `$LC` de cc1plus);
- la cadena queda **muerta** (la función que la referencia la estripa el
  enlazador), así que necesita entrada de `keep.lst`: `deadlink.py <u> --keep`;
- y después **`lcfix.py <u>`**, porque los `$LC` se desplazan en bloque.

---

## zFe: `rodata−192 → −48`, y el orden de 142 a 101

| medida | antes | después |
|---|---:|---:|
| `linkdelta` (con las entradas) | `rodata−192 data−32` | **`rodata−48 data−32`** |
| `rodorden` cadenas fuera de sitio | 142 / 581 | **101 / 581** |
| `fncmp` | 0 de 921 | **0 de 921** |
| literales de ruta `__FILE__` en `.rodata` | 20 | **7** |
| líneas `.asciz` del bloque | 117 | **74** |

Cambios (todos los `.cpp` tocados son **exclusivos de zFe.cpp**, comprobado):

- 99 sitios convertidos en 13 ficheros: `uiOptionsMain`, `uiPause` (48),
  `uiEATraxJukebox`, `uiRapSheetUS`, `uiRapSheetVD`, `uiRapSheetCTS`,
  `uiRapSheetRankingsDetail`, `uiRepSheetMilestones`, `uiRepSheetBounty`,
  `uiSMS`, `uiCareerMain`, `FEngInterface`, `FEManager`.
- 8 sitios más, que son las **8 cadenas que de verdad faltaban** (103 B) y que
  `prefijotu` daba por presentes porque **lee el OBJETO y manda el ENLACE**:
  `FEManager`, `MoviePlayer`, `MemoryCard`, `FEAnyMovieScreen`,
  `FEAnyTutorialScreen`, `uiRepSheetRivalFlow`, `RaceDatum`, `ScrollerSlot`.
- `FEAnyTutorialScreen.cpp:25`: `FEAnyTutorialScreenName` era
  `"FEAnyTutorialScreen.fng"`; el objetivo tiene `"FEAnyTutorial.fng"` (24 B de
  sobra menos, y una línea menos en el bloque).
- `MFlowReadyForOutro` salía **dos veces**: la cabecera generada del mensaje ya
  la emite. Fuera del bloque.
- **Un bloque de 80 B de `.4byte` al final de `zFe.cpp`**: el pool de la función
  de fecha del memcard —tabla de días por mes `1F 1C 1F 1E …` (48 B) más
  `43300000/3B336D84/43300000 80000000/43B6A000` (32 B)— que el objetivo tiene
  en `803E1DCC`, justo detrás de `"2000/1/1 00:00:00"`. Ese patrón **no aparece
  en ningún sitio de nuestro `.o`**: la función no existe en nuestro árbol.

### Lo que queda de `.rodata` en zFe: −48 B, con el inventario completo

Medido partiendo la ventana `803DF568..803E4380` en cadenas y trozos crudos:

```
ventana obj 19992  nue 19960   cadenas +100 B    resto −132 B
```

- **+100 B de cadenas que SOBRAN**, y ninguna que falte. Son casi todo el
  **residuo `size & 7`** de literales muertos que el objetivo no tiene:
  `'uction'` (de `Attrib::Gen::induction`), `'stems'`
  (`EventSequencerSystems`), `'ssis'` (`Attrib::Gen::chassis`), `'bosfx'`,
  `'icMode'`, `'ox.cpp'`, `'ap.cpp'`, `2×'rd.cpp'`, `'.hpp'`, `'ted'`, `'tor'`,
  `'done'`. Salen de los `#include` de cabeceras `Generated/AttribSys` que la
  c34dat2 metió a propósito para que naciera otro texto: **no se pueden quitar
  sin perder lo que sí hace falta.** Aparte: un `GAMECUBE` duplicado (9 B, el
  prefijo escrito a mano más el que emite cc1plus) y `"TODO"` (5 B), que es
  `FEGameInterface.cpp:82  bMalloc(256, "TODO", __LINE__, 0)` — **pista: en esa
  posición el objetivo tiene `"Movie filename"`.**
- **−132 B de pool crudo que FALTA**: 80 B = **cinco copias más** de
  `3F800000 3DCCCCCD 43200000 00000000` (`1.0f, 0.1f, 160.0f, 0.0f`); 16 B en el
  tramo `3F000000 3F800000 3C8EFA36 42B40000 …`; 3 B `4479C0`; y un reorden de
  `43300000 80000000 45098000 45898000`.

**Qué son esos 132 B, y por qué `fncmp` no los ve.** Son los **pools de
constantes de funciones MUERTAS** que el TU original tenía y el nuestro no
emite: `-strip-unused-data` se lleva la función pero **conserva su pool**
(ya está escrito en el docstring de `dolrod.py`). Y el objeto extraído sólo
contiene las funciones que SOBREVIVIERON al estripado, así que `fncmp` no puede
declarar `AUSENTE` ninguna. **Es un frente de «faltan funciones/ficheros», no de
datos**, y se detecta contando repeticiones de pool, no comparando símbolos.

## zEAXSound: el orden mejora, el tamaño no

| medida | antes | después |
|---|---:|---:|
| `linkdelta` (con las entradas) | `rodata−104 data−384 bss+32` | `rodata−104 data−384 bss+32` |
| `rodorden` | 115 / 277 | **108 / 277** |
| `fncmp` | 0 de 773 | **0 de 773** |

- 7 sitios convertidos (`SoundConn.cpp` ×2, `EAXS_StreamManager.cpp` ×5) y 3
  cadenas del código corregidas (`EAXCommon`/`EAXFrontEnd` → `AUD:…`), con sus
  7 líneas fuera del bloque.
- Añadidas al bloque `"CarState"` y `"Part"`, las dos únicas cadenas que
  faltaban de verdad, en su sitio relativo (junto a `CarID` y a `PartID`).
- `zEAXSound2` comprobada: sigue con **una** función abierta
  (`GenerateRoadNoise`, que no es mía) y no la he tocado.

### El `.data −384` de zEAXSound NO es relleno de alineación

Esto **corrige** `r52-jf-data.md` para esta unidad. Emparejando por nombre
**en todo el ELF enlazado** (no por dirección: la ventana se desplaza y el
emparejamiento por rango da 32 falsos ausentes):

- ventana `.data` del objetivo `80417634..80417DA8` = 1.908 B;
- **92 símbolos con nombre = 1.490 B, y los 92 están en nuestro enlace con el
  tamaño exacto**; sólo faltan `PlayerUpgrade.14976` y `counter.33705` (8 B),
  que son renombres del contador de declaración;
- **quedan 418 B en 26 huecos SIN nombre**, y **no son ceros**: llevan
  `FFFFFFFF`, `3F7FFFEF`, `3F800000`, `42480000 0000AC44 … 00000086 0000006E`,
  `1555EAAA`, `00007FFF`, `3FC00000 3F800000 FFFFFFFF`…

O sea que son **estáticos de fichero que dtk no supo nombrar y que nuestras
fuentes no definen** (o definen sin inicializador, y entonces GCC 2.9 los manda
a `.bss` — de ahí el `bss+32`). Los 384 B que faltan están ahí. Es trabajo de
escribir dato, con su posición, no de conseguir alineación.

Los huecos, por si sirven de mapa (dirección, tamaño, primeros bytes):

```
80417638  24  ceros          80417654  60  ffffffff...
804176A0   8  ceros          804176B8  36  ....3f7fffef....
80417750  28  ceros          80417770   8  ceros
80417790  40  ceros          804177BC   4  ceros
804177C4  16  ceros          80417810  12  ceros
80417820  16  00000000 3f800000 00000000 00000000
80417840   4  ceros          80417884  12  ceros
80417898  40  00000000 00000000 42480000 0000ac44 00000000 00000086 0000006e..
804178E4   4  ffffffff       804178F0   4  ceros
804178F8   4  ceros          80417910   4  ffffffff
80417980  24  00000000 00000000 00000000 3fc00000 3f800000 ffffffff
804179AC  12  ceros          804179E8   4  ceros
804179FC  12  ceros          80417A0C   4  ceros
80417A40   4  00007fff       80417A9E  26  00000000 1555eaaa ... 3f800000
80417ABC   8  ceros
```

---

## Por qué NO llegué a `DOL OK`, con el número

El encargo decía «código al 100 % a menos de 200 B». Es verdad del tamaño, pero
**el obstáculo dominante de las dos unidades no es el tamaño: es el ORDEN DE
EMISIÓN de las vtables**, que `linkdelta` no puede ver porque no cambia ningún
tamaño.

    python scripts/vtord.py zFe          143 de 158 fuera de sitio
    python scripts/vtord.py zEAXSound     58 de  67 fuera de sitio

Y no es artefacto del objeto: comparando las direcciones **del ENLACE**, las
vtables del objetivo caen en el nuestro con deltas de signo y magnitud
distintos —`_vt.17FEngGroupFEPrintf` +7.160, `_vt.15FEGameWonScreen` −3.240,
`_vt.12ScrollerSlot` +4.400, `_vt.16FEAnyMovieScreen` −2.000—, que es
permutación pura ([[nfsmw-orden-de-emision]]). Con eso abierto, `trypromo` no
puede dar `DOL OK` aunque las secciones midan igual, así que no lo he lanzado.

**Corrección al planteamiento de la ronda para la próxima tanda:** de las diez
unidades del cuadro de «menos de 300 B», al menos estas dos tienen el frente
real en `vtord`, no en `rodorden`. Merece la pena pasar `vtord` a las diez antes
de repartir.

## `keep.lst` — lo que ya está y lo que falta

**Ya aplicado** (y jf ya lo tiene commiteado, porque HEAD se movió durante la
sesión):

- `python scripts/lcfix.py zFe` → **44** entradas re-resueltas por contenido.
- `python scripts/lcfix.py zEAXSound` → **2** entradas.
- dos directivas renombradas a mano, porque el texto cambió en la fuente:
  `# @lc zEAXSound "EAXCommon"` → `"AUD:EAXCommon"`, y
  `# @lc zEAXSound "EAXFrontEnd"` → `"AUD:EAXFrontEnd"`.

**Falta, y es obligatorio** (regenerar, no copiar: los `$LC` se desplazan):

    python scripts/deadlink.py zFe --keep        >> config/GOWE69/keep.lst
    python scripts/deadlink.py zEAXSound --keep  >> config/GOWE69/keep.lst

Son 49 (zFe) + 6 (zEAXSound). Las de zFe, hoy:

```
$LC501 OMAudio          $LC502 OMVideo        $LC503 OMGameplay
$LC504 OMPlayer         $LC505 OMController   $LC506 OMEATrax
$LC507 OMCredits        $LC575 pm_RestartRace $LC576 pm_QuitRaceToFE
$LC577 pm_QuitRaceToFreeRoam  $LC578 pm_QuitMainMenu  $LC579 pm_QuitQuickRace
$LC580 pm_ResumeFreeRoam      $LC581 pm_SwitchToOptions
$LC582 pm_SwitchToTuning      $LC583 pm_ResumeRace
$LC615 JukeBoxScrollerSlot    $LC618 JukeBoxScrollerDatum
$LC638 RapSheetUSArraySlot    $LC639 RapSheetUSDatum
$LC647 RapSheetVDArraySlot    $LC648 RapSheetVDDatum
$LC652 RapSheetCTSArraySlot   $LC653 RapSheetCTSDatum
$LC665 RapSheetRankingsTimerArraySlot  $LC666 RapSheetRankingsArraySlot
$LC689 RapSheetRankingsDatum  $LC728 RaceDatum   $LC743 UITrackMapStreamer
$LC771 uiRepSheetRivalFlow    $LC836 SMSSortNode $LC837 SMSDatum
$LC841 SMSSlot                $LC850 CResumeFreeRoam  $LC851 CTop15
$LC852 CCarSelect             $LC853 CRapSheet   $LC854 CSave
$LC971 cFEng                  $LC972 FEngine     $LC976 FEManager
$LC977 cFEngGameInterface     $LC978 cFEngRender $LC979 cFEngJoyInput
$LC993 FEAnyMovieScreen       $LC1007 FEAnyTutorialScreen
$LC1049 MoviePlayer           $LC1066 MemoryCard $LC1119 ScrollerSlot
```

y las de zEAXSound: `$LC649 EAX_CarState`, `$LC657 Aud: EAX_HeliState`,
`$LC685/686/687` los tres *Stream Channel*, `$LC1108 C0`.

### Aviso de daño colateral

En una prueba temprana `import lcfix` **ejecutó su `main()`** (el módulo no tiene
guarda `__main__`) y corrigió de paso una línea que no es mía:
`zSpeech.o:$LC915 → $LC930`. **La revertí a `$LC915`**, que es lo que hay en
HEAD. Pero `lcfix.py --check` la sigue marcando, junto con seis de `zPhysics` y
una de `zMain`: **son de `speech` y de `cerca1`, que tienen que correr su propio
`lcfix.py <unidad>`.** (Y `scripts/lcfix.py` debería llevar
`if __name__ == '__main__':` para que se pueda importar sin efectos.)

## Ficheros tocados

`src/Speed/Indep/SourceLists/zFe.cpp`, `…/zEAXSound.cpp`, y 16 `.cpp` bajo
`src/Speed/Indep/Src/Frontend/` y `src/Speed/Indep/Src/EAXSound/`, **todos
exclusivos de su SourceList** (comprobado uno a uno). Ninguna cabecera
compartida. `zFe2`, `zFeOverlay` y `zEAXSound2` no cambian.
