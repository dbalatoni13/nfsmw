# Ronda 32 — `ord`: el ORDEN del `.text`

Encargo: el §4 del brief. Tres SourceLists con el `.text` del tamaño exacto,
cero funciones ausentes, cero por debajo del 100 % y **ya enlazando** desde la
r31, que no promocionan porque sus funciones **están en otro orden**.

**Resultado: 218 de las 405 funciones descolocadas, colocadas.**
`zFoundation` queda con **ORDEN PERFECTO**. Cuatro ediciones de `src/`, ninguna
en una cabecera compartida, **sin perder un byte de `matched`** y con el control
`DOL OK` (`9619ba57…`).

**Ninguna de las tres promociona todavía**, y no por el orden: a las tres les
falta `.rodata`. La cifra exacta está en el §5 para el agente de datos.

| unidad | descolocadas antes | después | saltos | `trypromo` antes | después |
|---|---|---|---|---|---|
| `zFoundation` | 68 | **0** | 3 → **0** | DOL ROTO | DOL ROTO (−1.440 B) |
| `zSim` | 209 | **123** | 42 → **33** | DOL ROTO | DOL ROTO (−1.824 B) |
| `zMisc` | 28 | 28 | 9 → 9 | DOL ROTO | DOL ROTO (−2.176 B) |

---

## 0. Las cinco frases

1. **`inline` no es una optimización: es la POSICIÓN.** En GCC 2.9 todo lo que
   sea inline se emite en `finish_file`, al final de la unidad. Y **no hace
   falta llevar el cuerpo a la clase**: basta la palabra `inline` en la
   definición fuera de clase. Eso desbloqueó la quinta causa de zFoundation, que
   la r31 dejó abierta **porque no compilaba al moverla a la cabecera** — con
   `inline` ni se toca la cabecera. Una palabra, **68 funciones**.
2. **El orden del bloque diferido lo decide el orden en que se PARSEAN las
   definiciones de clase, no el orden en que se usan.** Medido con `cc1plus` en
   un caso mínimo: definiendo `C`,`B`,`A` y usando `a`,`b`,`c` salen `C`,`B`,`A`.
   Y las **vtables salen en orden inverso** al de definición.
3. **`finish_file` va POR PASADAS.** Lo que sólo hace falta *por haber emitido*
   algo de la pasada N sale entero detrás de la pasada N — por eso los métodos de
   **una misma clase** pueden salir partidos en dos sitios lejanos. Verificado
   con el compilador, y es lo que le pasa a `RealFile::DeviceDriver` en zMisc.
4. **La palanca del `#include` está ACOTADA por el grafo transitivo, y en zMisc
   eso la mata.** `Hermes.h` incluye `AttribAlloc.h`, y `GameFlow.hpp` incluye
   `ResourceLoader.hpp` → `driver.h`. Ninguna reordenación de `zMisc.cpp` puede
   poner `HighAttribAlloc` detrás de las clases de Hermes: **el orden del
   objetivo es inalcanzable desde la SourceList**. Lo que difiere es la lista de
   `#include` de una cabecera NUESTRA (§6.1).
5. **El frente es mucho mayor que las tres unidades del encargo**: el barrido de
   la herramienta nueva da **32 unidades de 525 con el `.text` desordenado y
   9.924 funciones fuera de sitio**, middleware incluido (§4).

---

## 1. La herramienta: `scripts/textorder.py` (nueva, es el entregable duro)

No existía. Compara la **secuencia de símbolos de `.text` por dirección** entre
el objeto **extraído** (`build/GOWE69/obj/…`, que trae el orden bueno) y el
**nuestro** (`build/GOWE69/src/…`). **No enlaza nada**: es inmune a los otros
agentes y tarda milisegundos.

    python scripts/textorder.py zFoundation          # la medida
    python scripts/textorder.py zSim --porque        # cada salto -> su causa
    python scripts/textorder.py zSim --movidas       # las que mas se desplazan
    python scripts/textorder.py --todas              # las 525 unidades

Lo que hay que saber para leerla:

- **delta** = desplazamiento en bytes respecto de donde debería estar la
  función, alineando por tamaño acumulado. Un bloque entero con el mismo delta
  está *empujado*: no es una causa, es una consecuencia.
- **SALTO de delta** = el sitio donde el delta cambia. **Ahí está la causa.** 209
  funciones descolocadas en zSim eran 42 saltos, y 3 de ellos valían 86.
- **`--porque`** imprime la vecindad del salto en NUESTRO objeto con la posición
  que el objetivo le da a cada vecina. Es lo que convierte un salto en un
  diagnóstico: si emitimos algo justo antes que el objetivo pone 19.732 B más
  allá, ése es el culpable y hay que diferirlo.
- **`--movidas`** ordena por desplazamiento **de rango**, no de bytes: caza
  directamente las funciones definidas fuera de su clase que deberían estar
  dentro. Cuando esa lista se queda plana (±72 rangos en zSim ahora), ya no
  quedan casos baratos y lo que resta es permutación fina.

El docstring lleva escrito el modelo de emisión medido, para no volver a
descubrirlo.

## 2. El modelo de emisión de GCC 2.9, medido con el compilador

No es lectura de la fuente de GCC: son tres casos mínimos pasados por
`build/compilers/ProDG/3.9.3/cc1plus.exe -O1 -S`.

| ensayo | fuente | salida |
|---|---|---|
| lab1 | clases `A`,`B`,`C` con virtuales en clase; objetos `a`,`b`,`c` | `f__1A g__1A f__1B g__1B f__1C g__1C` |
| lab3 | **definiciones** `C`,`B`,`A`; **usos** `a`,`b`,`c` | `f__1C g__1C f__1B g__1B f__1A g__1A` |
| lab2 | `A` sólo se menciona dentro de un inline de `C` | `f__1B g__1B mk__1C h__1C` **y luego** `_vt.1A f__1A g__1A` |

- lab1 + lab3 → **manda el orden de DEFINICIÓN de la clase, no el de uso**.
- lab3 → las **vtables salen en orden inverso** (`_vt.1A _vt.1B _vt.1C`).
- lab2 → **hay pasadas**: `finish_file` repite la lista hasta que no nace nada
  nuevo, y lo que nace por haber emitido la pasada N sale detrás de toda ella.

Los `.cpp` están en el scratchpad (`c32ord_lab1..3.cpp`). Reproducir cada uno
cuesta un segundo.

## 3. Los cinco ensayos

### o1 — `zFoundation`: `inline` en `CarpResolver::StartGroup` → **ORDEN PERFECTO**

Era la quinta causa de la r31, la única que quedó abierta. La r31 intentó llevar
el cuerpo a `CARP.h` y **no compila**: `gResolving`, `gDeltaAddress` y
`ResolveData` están declarados en `Carp.cpp`, no en la cabecera.

**No hacía falta tocar la cabecera.** `inline` en la definición fuera de clase
consigue exactamente lo mismo:

    src/Speed/Indep/Libs/Support/Miscellaneous/Carp.cpp
    -bool CarpResolver::StartGroup(const UGroup *group) {
    +inline bool CarpResolver::StartGroup(const UGroup *group) {

`Carp.cpp` sólo lo incluye `zFoundation.cpp`: **radio de explosión cero**, y
`CARP.h` queda intacta (`git diff` vacío).

`zFoundation` **68 → 0 descolocadas, 3 → 0 saltos**, `36.000/36.000 = 100 %`,
188 funciones (idéntico a la r31). Efecto secundario: desaparece también
`__Q24CARP12CarpResolverUi`, el constructor que emitíamos de más y el objetivo
no tiene — **−36 B de `.text` muerto**.

### o2 — `zSim`: `inline` en los dos `Gen::…::ClassKey` → 209 → 191

`zSim.cpp` define `ATTRIB_NO_INLINE_CLASSKEY`, así que la cabecera generada apaga
su copia inline y `SimModel.cpp` da la definición fuera de línea. El objetivo los
tiene en el bloque diferido (0x1637C), nosotros a mitad de unidad, y eso empujaba
todo desde `hash32` con +24 B.

    src/Speed/Indep/Src/Sim/Common/SimModel.cpp
    -Key Gen::system::ClassKey() {           +inline Key Gen::system::ClassKey() {
    -Key Gen::controller::ClassKey() {       +inline Key Gen::controller::ClassKey() {

**Comprobado que siguen enlazando**: pasan a binding **WEAK**, no `LOCAL`, así
que los objetos de otras unidades los siguen resolviendo. `zSim` sigue
`96.400/96.400 = 100 %`, 402 funciones.

### o3 — `zSim`: reordenar cuatro funciones en `QuickGame.cpp` → 191 → 187

Código normal, no diferido. El objetivo las tiene `OnManageTime`(0xF630),
`OnBeginState`(0xF864), `IsStateDone`(0xFA70), `CanSimulate`(0xFDDC); nosotros
`OnBeginState`, `CanSimulate`, `IsStateDone`, `OnManageTime`. Reordenadas tal
cual, con una nota en el fuente para que nadie las vuelva a ordenar «bien».

### o4 — `zSim`: `~CAnimMomentScene` al cuerpo de la clase → 187 → **123**

`NISActivity.cpp` la declaraba en clase y la definía fuera, 52 B a mitad de
unidad; el objetivo la tiene en 0x16F38. Cuerpo vacío `{}` dentro de la clase.
**64 funciones de una línea.**

**Coste declarado**: `zSim` deja de emitir `$LC704` y `$LC705` (dos flotantes
del pool anónimo, 8 B en el objeto) y su `.rodata` del DOL baja **32 B**
(−1.568 → −1.600). Atribuido por A/B (revertida sola, vuelve a −1.568). Lo dejo
puesto porque **los objetos del original no tienen ni un `$LC`** (§3 del brief):
emitir menos pool anónimo va en la dirección correcta, y 32 B de pool muerto no
compensan 64 funciones colocadas. **El agente de datos tiene que contar 1.600, no
1.568.**

### o5 — `zMisc`: forzar el orden de parseo con `#include` → **REVERTIDA** (§6.1)

---

## 4. El barrido: el frente son 32 unidades, no tres

`python scripts/textorder.py --todas` (525 unidades comparables, middleware
incluido). Las que no salen tienen el orden perfecto.

| desc | total | saltos | unidad | | desc | total | saltos | unidad |
|---|---|---|---|---|---|---|---|---|
| 1159 | 1308 | 153 | `zFe2` | | 273 | 319 | 78 | `zEagl4Anim` |
| 921 | 922 | 172 | `zFe` | | 272 | 358 | 18 | `zWorld2` |
| 817 | 1121 | 50 | `zPhysicsBehaviors` | | 166 | 538 | 79 | `zLua` |
| 800 | 1031 | 191 | `zAI` | | 135 | 137 | 85 | `zPlatform` |
| 730 | 774 | 125 | `zEAXSound` | | **123** | 403 | 33 | **`zSim`** |
| 701 | 719 | 224 | `zPhysics` | | 58 | 75 | 1 | `realcore/file/cmn/filesys` |
| 701 | 704 | 104 | `zSpeech` | | 47 | 197 | 18 | `zAttribSys` |
| 694 | 768 | 253 | `zGameplay` | | **28** | 451 | 9 | **`zMisc`** |
| 513 | 931 | 39 | `zEAXSound2` | | 27 | 316 | 7 | `zAnim` |
| 501 | 1381 | 194 | `zMain` | | 17 | 23 | 3 | `rcmp_vp6_codec` |
| 454 | 583 | 25 | `zWorld` | | 15 | 16 | 7 | `ppc2D2` |
| 429 | 453 | 113 | `zCamera` | | 15 | 19 | 1 | `file/gc/dvd_device` |
| 315 | 540 | 40 | `zEcstasy` | | 10 | 31 | 8 | `realmemcard/gc_interface` |

Más `zTrack` 6, `LibSN/steering` 5, `path/pathnode` 4, `zBWare` 2, `madidct` 1,
`LibSN/vm` 1, `coda/lbmpeg` 1.

**32 unidades, 9.924 funciones fuera de sitio.** `zFoundation` ya no aparece.

Dos avisos para no leer mal esta tabla:

- En las unidades que **todavía no están al 100 %** (zFe2, zAI, zPhysics…) parte
  del desorden es *consecuencia* de las funciones que faltan o sobran, no causa.
  La tabla vale como cribado; el diagnóstico es `--porque`.
- **`ppc2D2`, `filesys`, `dvd_device`, `pathnode`, `steering`, `lbmpeg`,
  `madidct`, `vm` son middleware con 1-7 saltos**: son los casos más baratos del
  proyecto para probar la receta, y ninguno estaba mirado.

## 5. Lo que les falta a las tres, para el agente de datos

Enlazadas de verdad y comparadas contra un control **verificado**
(`c32ord_control.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, o sea el DOL
del objetivo byte a byte). La cuenta de la r31 estaba hecha contra
`build/GOWE69/main.dol`, que no es el control.

| unidad | `.rodata` (sec. 9) | `.data` (sec. 10) | otras | DOL |
|---|---|---|---|---|
| `zFoundation` | **−1.248** | −192 | 0 | −1.440 |
| `zSim` | **−1.600** | −224 | 0 | −1.824 |
| `zMisc` | **−2.208** | +32 | 0 | −2.176 |

`data4` y `data5` salen del mismo tamaño en las tres: sólo están desplazadas
porque `.rodata` va corta. **El `.text` no aporta ni un byte de diferencia de
tamaño en ninguna de las tres**, ni antes ni después de mi trabajo. Los 1.312 B
que le faltan a `zFoundation` están desmenuzados bloque a bloque en el §4.2 del
informe de la r31.

## 6. Vedas

### 6.1 `zMisc`: el orden del objetivo es INALCANZABLE reordenando `#include` (o5)

Ensayo: seis `#include` explícitos al principio de `zMisc.cpp`, antes del primer
`.cpp`, en el orden que el objetivo pide (Hermes, GameFlow, AttribAlloc, Rumble,
QueuedFile, driver.h). **Saltos 9 → 8, descolocadas 28 → 28: cero.** Revertida
(`git diff` vacío).

Y no es que faltara afinar, es que **no se puede**, con dos hechos medidos:

- `Hermes.h:9` incluye `Misc/AttribAlloc.h` → `HighAttribAlloc` se parsea
  **siempre** antes que las clases de Hermes. El objetivo lo tiene al revés.
- `GameFlow.hpp:8` → `ResourceLoader.hpp:5` → `realcore/file/driver.h` →
  `RealFile::DeviceDriver` se parsea **siempre** antes que `RegionLoader`. El
  objetivo lo tiene al revés.

Orden del objetivo: `map`, `RebuildTable`, `RegionLoader`, `HighAttribAlloc`,
`tShaker`, `QueuedFile`, `QueuedFileBundle`, `DeviceDriver::GetName` +
`GetOptimalReadSize`, `CachedRealFileHandle`, `OpenDisculatorFile`,
`DisculatorDriver`, `FileGarbageCollector`, `VaultGarbageCollector`,
`DefaultAttribAllocator`, **‖ pasada 2 ‖** `~DeviceDriver`, `Init`, `Restore`,
`Write`, `QueryLocation`, `Remove`, `Getspace`.

**La conclusión es una pista, no un callejón**: si el orden del objetivo exige
que `AttribAlloc.h` se parsee después de Hermes, es que **la lista de `#include`
de nuestro `Hermes.h` no es la del original**. Eso es comprobable y arreglable, y
es una cabecera de 11 `#include`. Lo mismo con `ResourceLoader.hpp`. **No lo he
tocado**: son cabeceras compartidas y el brief pide A/B sobre todas las unidades
que las incluyen.

Y hay un segundo requisito que ningún `#include` da: el objetivo parte
`RealFile::DeviceDriver` en **dos pasadas** (§2, lab2). Eso pide que su vtable
nazca *durante* la pasada 1, no antes.

### 6.2 `zSim`: el bloque diferido no tiene ya ninguna causa barata

Tras o2/o3/o4, `--movidas` se queda plana: el mayor desplazamiento de rango es
`_._4INIS` con −72 sobre 403, y detrás vienen los `ClassKey` (+35) y los
`WorldConn::Pkt_Effect_*` (+24..+28). **No queda ninguna función definida fuera
de su clase que debiera estar dentro.** Los 33 saltos restantes son:

- **3 saltos, ~12 funciones**: el bloque de plantillas 0x13CB4..0x14934. Las
  cuatro `find_if<GarbageNode<…>>` el objetivo las pone antes de `reserve`, y
  nosotros después de todo el grupo de `CDispatcher`. Es orden de
  **instanciación**, o sea de primer uso: hay que moverlo en un `.cpp` anterior.
- **~30 saltos, ~111 funciones**: permutación fina del bloque diferido
  0x15AC8..0x17840 entre unas 15 cabeceras de interfaces (`Sim::IEntity`,
  `Sim::IServiceable`, `Sim::ITaskable`, `Sim::Object`, `IPlayer`…). Misma
  naturaleza que el §6.1 y misma cura probable: **el orden de parseo lo fija el
  grafo de `#include` de nuestras cabeceras**, no el `.cpp` de la SourceList.

### 6.3 Lo que NO he probado

1. **Corregir la lista de `#include` de `Hermes.h` y `ResourceLoader.hpp`** —
   la pista del §6.1, y probablemente la del §6.2. Es cabecera compartida:
   pide A/B por objetos sobre todas las unidades que las incluyen.
2. **Las 29 unidades restantes del §4.** Ni una mirada. Las de middleware con
   1-7 saltos son el ensayo más barato que hay ahora mismo en el proyecto.
3. **El bloque de plantillas de zSim** (§6.2, ~12 funciones).
4. **Los datos.** Es el encargo de otro agente; sólo he medido (§5).

---

## 7. Verificación

- `build_direct.py` de las unidades tocadas: **4 ok, 0 fallidas**, repetido tras
  cada ensayo y tras añadir los comentarios.
- `measure.py zSim zMisc zMiscSmall zFoundation`: **211.176/211.176 B, 100 %,
  1.048 funciones**. Igual que la r31 (188 + 402 + 458).
- `audit.py` en las tres: **0 FALLA**.
- `keepchk.py`: **758 ok, 19 RANCIAS, 0 sin objeto** — exactamente igual que al
  empezar; no he movido ninguna frontera.
- `globalini.py`: las dos unidades con nombre distinto son `rcmp_vp6_codec` y
  `realmemcard/gc_interface`, **ninguna mía y las dos ya lo eran**.
- `config/GOWE69/{splits.txt,symbols.txt,keep.lst,config.yml}` y `configure.py`:
  **no los he tocado**; copia de seguridad en el scratchpad. **No propongo
  ningún cambio de configuración.**
- Los `#if defined(__ANDROID__)`: `src/types.h` **2**, `bMath.hpp` **4**,
  `UVectorMath.hpp` **2**. Intactos.
- **Control del enlace, con todas mis ediciones de código puestas: `DOL OK`**,
  `9619ba57c9919f95f7f2ac951a2166a3517f91e3`. Pasado **dos veces**: al medir el
  §5 y otra vez al terminar los cuatro ensayos, ya con o1-o4 en su estado final.
  **No dejo ningún fichero que rompa el enlace.**

  **Aviso de convivencia, no es mío:** el tercer control, lanzado 10 minutos
  después, **falla el enlace**:

      trctasks.cpp(90) : L0039: Reference to undefined symbol
      _SetMsgOptions__Q26Realmc9GCMessagei in ... gc/trctasks.o

  Es de **otro agente**, y está comprobado: `_SetMsgOptions` se declara en
  `Packages/realmemcard/…/impl/gc_interface_impl.h`, que `git status` da
  **modificado** junto a `gc_memcard_interface_impl.cpp`, y `trctasks.o` se
  recompiló a las 21:16 — yo no he tocado `realmemcard` en toda la ronda. Es la
  trampa 3 de `HERRAMIENTAS.md` («con agentes en paralelo tu medida no es tuya»).

  Entre el segundo control (limpio) y ahora, lo único que he cambiado en `src/`
  son **comentarios**. Que no tocan el código lo prueba la propia medida:
  `measure.py` sigue dando **211.176/211.176 B**, o sea que **cada byte de
  `.text` de las cuatro unidades sigue casando con el ELF original**, y
  `textorder.py` da las mismas cifras de orden antes y después.

### 7.1 Ficheros de `src/` que dejo modificados

| fichero | +/− | por qué |
|---|---|---|
| `Speed/Indep/Libs/Support/Miscellaneous/Carp.cpp` | +7 −1 | o1 (§3), 1 palabra + 6 de comentario |
| `Speed/Indep/Src/Sim/Common/SimModel.cpp` | +5 −2 | o2 (§3), 2 palabras + 3 de comentario |
| `Speed/Indep/Src/Sim/Activities/QuickGame.cpp` | +4 −0 y reorden de 4 funciones | o3 (§3) |
| `Speed/Indep/Src/Sim/Activities/NISActivity.cpp` | +4 −3 | o4 (§3) |

Los cuatro `.cpp` los incluye **una sola SourceList** cada uno (`Carp.cpp` →
zFoundation; los otros tres → zSim), comprobado con `grep`: **ninguna cabecera
compartida tocada y radio de explosión cero**. `CARP.h`, `UGroup.hpp` y
`zMisc.cpp` quedan **byte a byte como estaban**.

**Deuda declarada:** los tres `inline` de o1 y o2 se ponen **por la posición, no
por la optimización**. Es una forma de fuente legítima y el objetivo emite esos
símbolos exactamente donde los deja, pero si algún día se descubre que el
original los tenía en la clase (o en otro fichero), la palabra sobra. Está dicho
en un comentario junto a cada uno.

## 8. Guiones del scratchpad (`c32ord_`)

| guion | qué hace |
|---|---|
| `c32ord_lab1.cpp` / `lab2` / `lab3` | los tres casos mínimos del §2. Un segundo cada uno con `cc1plus -O1 -S` |
| `c32ord_barrido.py` | el barrido del §4 antes de plegarlo en `textorder.py --todas` |
| `c32ord_bak/` | copia de seguridad de los 4 `.cpp`, de `CARP.h`, de `zMisc.cpp` y de los cuatro ficheros de `config/GOWE69` + `configure.py` |
| `c32ord_control.dol` | el control verificado del §5 |

Reutilizados de la r31: `c31und_keep.py` (enlaza dejando el `.dol`),
`c31und_doldiff.py`, `c31und_elfsym.py`.
