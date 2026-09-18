# r64 -- ai: zAI y zFe2

Agente `ai`. Unidades en exclusiva: `zAI`, `zFe2`.
Ficheros tocados: `src/Speed/Indep/SourceLists/zAI.cpp` y
`src/Speed/Indep/SourceLists/zFe2.cpp` (codigo y comentarios; en zFe2 ademas los
16 alias numerados que reescribe `mangfix.py`).
**Nada commiteado. `config/GOWE69/keep.lst` NO se ha tocado y `lcfix.py` NO se ha
corrido**: las correcciones de `$LC` estan CONTADAS y listadas abajo, y se
midieron con copias privadas en `scratchpad/ai64/`.

---

## 0. LO PRIMERO, Y NO ES MIO: LAS TRES LINEAS DE `keep.lst` DE LA r63 SIGUEN SIN APLICAR

`gapchk.py zAI` las canta hoy igual que hace una ronda, y `zAI.cpp` lleva la nota
en la cabecera desde la r63. Medido HOY, en el mismo arbol:

| estado | `dolwhere zAI` |
|---|---:|
| como esta el arbol ahora (solo el `pad` 0xAC del codigo) | **237.800 B** |
| anadiendo las tres lineas a `keep.lst` | **53.874 B** |

Son **-183.926 B de DOL por tres lineas de texto**. Es, con diferencia, el mayor
bloque de bytes pendiente que conozco, y esta a una edicion de distancia.
Las tres, detras de `zAI.o:gap_06_80415498_data` (linea 73 hoy), en CRLF:

```
zAI.o:gap_06_80415260_data
zAI.o:gap_06_804152A8_data
zAI.o:gap_06_804152C8_data
```

Recordatorio de por que son multiplicativas con el `pad ... 0xAC` que YA esta en
`zAI.cpp`: son las dos mitades del mismo deficit de 20 B de `.data` (8 del pad,
12 de los tres huecos), y con una sola mitad los 2.596 simbolos ajenos siguen
desplazados. Todo lo que sigue en este informe esta medido CON las tres puestas.

---

## 1. Resultado de la ronda

Las dos unidades bajan, las dos con las secciones del DOL CUADRANDO y sin ni una
regresion de codigo. Medido por PAREJAS (base y candidato seguidos, en la misma
ventana: ver el aviso de la seccion 5).

| unidad | `dolwhere` base | `dolwhere` con lo de esta ronda | delta |
|---|---:|---:|---:|
| zAI  | 53.928 B | **53.469 B** | **-459** |
| zFe2 | 46.908 B | **46.105 B** | **-803** |

* `fncmp Speed/Indep/SourceLists/zAI`  : **0 de 1030** con el codigo distinto.
* `fncmp Speed/Indep/SourceLists/zFe2` : **0 de 1307** con el codigo distinto.
* Secciones del DOL: **coinciden** en las dos (ni una linea de
  `LAS SECCIONES NO COINCIDEN`).
* Sello triple, tres compilaciones seguidas identicas:
  * `zAI.o`  `91b310efba4be0ff1a2d082e77c5cfdda14613a1`
  * `zFe2.o` `2b910ea96faeb2e2550a30a6c2b619696afcbe50`
* `trypromo`: **ninguna promociona**. Siguen a 53 kB y 46 kB del DOL.

---

## 2. La palanca: la de `parseord` de la r63, estrenada

La r63 dejo escrita la receta y no le dio tiempo a estrenarla. Es esta:
**adelantar CABECERAS -- nunca `.cpp` -- para que las clases se COMPLETEN en el
orden del objetivo**, porque el orden de vtables es el inverso del de completado.
Los `.cpp` no se tocan: su `.text` va a delta 0 en 255 kB (zAI) y 236 kB (zFe2) y
moverlos solo puede empeorar.

### 2a. zFe2: 13 cabeceras, -803 B

Van justo detras de `WorldConn.h`, en el orden en que el objetivo completa sus
clases: `FeHudElement.hpp`, `FEMenuScreen.hpp`, `IFengHud.h`, `FeGetawayMeter.hpp`,
`FeRadarDetector.hpp`, `FeHeatMeter.hpp`, `FeCostToState.hpp`, `FeReputation.hpp`,
`Slider.hpp`, `feWidget.hpp`, `FEIconScrollerMenu.hpp`, `feArrayScrollerMenu.hpp`
y `FEPkg_Hud.hpp`.

`parseord.py zFe2` pasa de **87 de 89 mal** a **52 de 89**, y la distancia de
posicion de 1.340 a 144. Las descolocadas de `.text` por DIRECCION ENLAZADA
bajan de 215 a 210.

**El optimo NO es un prefijo del orden del objetivo.** Barrido completo, todo con
`dolwhere` (14 compilaciones):

```
N= 8 46.814 | N= 9 46.814 | N=10 46.754 | N=11 46.658 | N=12 46.442
N=13 46.313 | N=14 46.476 | N=16 46.547 | N=18 46.547 | N=20 46.547
N=24 46.603 | N=28 46.603 | N=30 46.603 | N=31 46.603 | N=32 233.122 (TAMANO)
{0..12} + FEPkg_Hud.hpp = 46.047   <-- el elegido
```

### 2b. zAI: doce cabeceras en dos bloques, -459 B

`parseord.py` da CONTROL FALLIDO en zAI (invertir nuestro parseo reproduce 18 de
73 vtables), asi que la herramienta no vale tal cual. **Lo que si vale es sacar
el orden objetivo de las DIRECCIONES del ELF original**: los 171 simbolos `_vt.`
de nuestro `zAI.o` que existen en `NFSMWRELEASE.ELF`, ordenados por direccion e
invertidos, dan 74 clases; el mapeo clase -> cabecera sale del preproceso. Con
eso el orden objetivo deja de ser adivinanza (aunque siga siendo hipotesis: el
juez es `dolwhere`).

Bloque 1, delante de `#include ".../AITrafficManager.cpp"` -- **detras de todas
las instanciaciones explicitas, para no mover la region A de
`pending_templates`**: `AIAvoidable.h`, `IAI.h`, `ITrafficMgr.h`,
`AISpawnManager.h`, `Behavior.h`, `IVehicleCache.h`, `WRoadNetwork.h`,
`ICopMgr.h`.
Bloque 2, delante de `#include ".../AICopManager.cpp"`: `AIVehiclePursuit.h`,
`AIVehicleCopCar.h`, `AITarget.h`, `AIRoadBlock.h`.

Escalones medidos: 53.874 -> 53.670 (dos cabeceras) -> 53.535 (bloque 1 entero)
-> 53.453 (los dos bloques).

---

## 3. Lo que hay que aplicar en `keep.lst` (PROPUESTA), y cuanto vale

Adelantar cabeceras mueve literales y **renumera los `$LC`**. Las entradas `@lc`
identifican la cadena por CONTENIDO, asi que la correccion es mecanica:
`python scripts/lcfix.py zAI zFe2`. **Sin ella el DOL rompe en silencio**:
`linkdelta` lo canta -- `zAI: rodata-176`, `zFe2: rodata-184` -- mientras
`keepchk` sigue dando las entradas por buenas.

**Ninguna entrada se anade ni se pierde: son las MISMAS cadenas con otro numero.**
Son 13 en zAI y 21 en zFe2, y estas son, por si se prefiere aplicarlas a mano:

zAI (identificadas por la directiva `# @lc` que llevan encima):

```
"Attrib::Gen::gameplay"            $LC301 -> $LC232
"19.8.31"                          $LC302 -> $LC233
"Attrib::Gen::engineaudio"         $LC408 -> $LC410
"SMS_MESSAGE_%d"                   $LC442 -> $LC444
"SMS_MESSAGE_%d_FROM"              $LC443 -> $LC445
"SMS_MESSAGE_%d_VOICE"             $LC444 -> $LC446
"SMS_MESSAGE_%d_SUBJECT"           $LC445 -> $LC447
"Attrib::Gen::speechtune"          $LC470 -> $LC472
"16.2.1"                           $LC303 -> $LC234
"1.2.3"                            $LC304 -> $LC235
"Attrib::Gen::rigidbodyspecs"      $LC854 -> $LC856
"Attrib::Gen::collisionreactions"  $LC972 -> $LC974
"Attrib::Gen::smackable"           $LC973 -> $LC975
```

zFe2 (numeros de linea sobre el `keep.lst` de hoy):

```
1072 $LC403 -> $LC416    2482 $LC488 -> $LC501
1074 $LC406 -> $LC419    2484 $LC489 -> $LC502
1076 $LC475 -> $LC488    2486 $LC530 -> $LC542
1078 $LC717 -> $LC723    2488 $LC531 -> $LC543
1080 $LC730 -> $LC375    2490 $LC859 -> $LC861
1082 $LC732 -> $LC377    2492 $LC873 -> $LC875
1084 $LC994 -> $LC996    2494 $LC884 -> $LC886
1086 $LC995 -> $LC997    2496 $LC949 -> $LC951
1088 $LC997 -> $LC999    2498 $LC983 -> $LC985
                         2500 $LC993 -> $LC995
                         2502 $LC998 -> $LC1000
                         2516 $LC387 -> $LC401
```

`lcfix.py zAI` seguira sacando su FALLO de siempre --`'GAMECUBE' no tiene simbolo
$LC propio`, la cadena esta dentro del prefijo escrito a mano-- y no es nuevo.

---

## 4. NEGATIVOS MEDIDOS (los tres estan escritos JUNTO AL CODIGO)

### 4a. zFe2: el orden ENTERO del objetivo rompe el TAMANO, y no es un fallo de orden

Con las **62 cabeceras** del orden del objetivo el parseo casi casa
(`parseord` 87 mal -> 52; distancia de posicion 1.340 -> 144) y la distancia de
DIRECCIONES ENLAZADAS **mejora mucho**: de 910.419 a 590.908, y solo en `.rodata`
de 162.635 a 18.508 --con los deltas concentrados (65 simbolos a -32, 26 a 0, 17
a +8) en vez de dispersos--. **Y aun asi `dolwhere` sube de 46.850 a 233.931 B.**

La causa esta medida y no es el orden: la `.rodata` de la unidad sale **8 B corta
EN EL OBJETO** --y 192 B corta EN EL ENLACE-- porque **el relleno de alineacion
entre cadenas depende del orden del pool**: el histograma de huecos pasa de
`{1:183, 2:181, 3:186, 4:26, 5:3, ...}` a `{1:185, ..., 5:1, ...}`, o sea dos
huecos de 5 B que se quedan en 1 B. Ocho bytes mueven la `.rodata` de TODO el
juego. Los simbolos son los MISMOS y los tamanos tambien: 1.180 `$LC` y 13.277 B
en los dos objetos, 136 simbolos no-`$LC` y 8.824 B en los dos.

**La leccion, y es general: en una unidad con escalera de `.rodata` el orden de
parseo no se puede optimizar solo; hay que optimizarlo SUJETO a que la suma de
rellenos no cambie.** Por eso el optimo es un subconjunto de 13 cabeceras y no
las 62. La primera cabecera que rompe el tamano al anadirla es `PhotoFinish.hpp`
(indice 31 del orden del objetivo), pero no es la unica: quitandola del bloque
completo el tamano sigue roto.

Intente compensar con 192 B de relleno de `.rodata` al final de la unidad mas una
linea de `keep.lst`: **no sirve**, la seccion se va a +192 en vez de a 0 (el
bloque no cae al final del todo, cae delante de los literales de los inline
diferidos, y mueve el relleno otra vez). Descartado con una compilacion.

### 4b. zAI: anadir `AIPursuit.h` al segundo bloque rompe el tamano

Arrastra las siete `Formation`, que el objetivo completa en los puestos 65..71 y
no en el 23. La seccion `0x803A41B8` sale 8 B corrida. Una compilacion.

### 4c. zAI y zFe2: lo que queda del orden YA NO SE ARREGLA CON `#include`

`AIVehicleEmpty` y `AIVehicleHuman` (objetivo 44 y 45), los tres
`AdaptivePIDController` (47..49) y las siete `Formation` (65..71) salen donde
salen porque estan escritos ANTES **dentro de** `AIVehicle.h` y `AIPursuit.h`.
Una cabecera se puede adelantar; **atrasar una clase exige moverla dentro de su
cabecera**, y las dos son COMPARTIDAS. Lo mismo en zFe2 con cuatro casos, y todos
valen posiciones enteras del orden:

* `feScrollerina.hpp` la arrastra `FEIconScrollerMenu.hpp`: `Scrollerina` nos sale
  en el puesto 16 y el objetivo la tiene en el **80**.
* `FEIconScrollerMenu.hpp` define `FEScrollyBookEnd` antes que `IconPanel`; el
  objetivo los tiene al reves.
* `feArrayScrollerMenu.hpp` define `ArraySlot, ArrayScroller, ArrayScrollerMenu,
  ImageArraySlot`; el objetivo tiene `ArraySlot, ImageArraySlot, ArrayScroller,
  ArrayScrollerMenu`.
* `PhotoFinish.hpp` incluye `FEPkg_PostRace.hpp` antes de definir
  `PhotoFinishScreen`; el objetivo completa `PhotoFinishScreen` ANTES que los 16
  de PostRace.

**Son cuatro ediciones de cabecera compartida y no las he hecho** (regla 5). Si
el jefe las quiere probar, el metodo es el de esta ronda: `parseord`/`-E` para
elegir y `dolwhere` con copia privada de `keep.lst` para juzgar; pero hay que
medir tambien las unidades ajenas que parsean esas cuatro cabeceras.

---

## 5. AVISO DE METODO: LAS MEDIDAS ABSOLUTAS SE MUEVEN CON LOS DEMAS AGENTES

A mitad de ronda `dolwhere zFe2` paso de 46.850 a **511.611 B** sin que yo tocara
nada, y `dolwhere zAI` a 207.143. No era mio: `build/GOWE69/src/*.o` es
COMPARTIDO, el enlace usa el objeto NUESTRO de todas las unidades ya
promocionadas, y varios agentes estaban recompilando las suyas (zEAXSound2 23:44,
zLua 23:42, zTrack 23:40, zEAXSound 23:36...). Media hora despues la base de zFe2
media 46.908 en vez de 46.850: **58 B de deriva ajena**.

**Por eso todas las cifras de la seccion 1 son PAREJAS base/candidato tomadas
seguidas.** El delta (-803 y -459) salio identico en las dos ventanas; el
absoluto no. Una medida absoluta de `dolwhere` tomada al principio de una tanda y
comparada con otra tomada al final **no vale**, y una sola lectura anomala no es
una regresion: hay que repetir la pareja.

---

## 6. Herramientas (en `scratchpad/ai64/`, no promovidas a `scripts/`)

* `dw2.py` -- `dolwhere` con `--keep` parametrizable y **resumen por seccion**.
  Sin lo primero no se puede medir una propuesta de `keep.lst` sin editar el
  fichero compartido, que es justo lo que la regla 3 prohibe.
* `ordlink.py` -- compara **DIRECCIONES ENLAZADAS** de los simbolos de una unidad
  contra `NFSMWRELEASE.ELF`, por seccion, con **histograma de deltas**. El
  histograma es lo que distingue "el orden esta mal" de "el bloque entero esta
  corrido N bytes", y es lo que destapo el 4a.
* `lcfix_priv.py` -- `lcfix` que lee y escribe copias privadas de `keep.lst`.
* `po.py` -- `parseord` sobre un FICHERO candidato, sin tocar el arbol.
* `prueba.py` / `pruebaU.py` -- el ciclo entero: instala, compila, itera `mangfix`
  hasta punto fijo (con una sola pasada NO converge y el enlace falla), rehace el
  `keep.lst` privado y mide.
