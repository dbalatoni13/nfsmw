# Ronda 27 — rescate del paquete de `splits.txt`

Base: `linked` 429/590, `main.dol: OK` (`9619ba57`).

**Resultado: los 13 grupos de la r26 pasan, más DOS nuevos (P y Q), y con ellos
promocionan CUATRO unidades — 16.512 B.** `linked` 429 → **433**.

| unidad | B | qué necesita |
|---|---|---|
| **`gc_memcard_interface_impl`** | **10.984** | grupos N+Q **y** la reescritura de fuente del §4 (ya hecha y verificada) |
| **`pathvol`** | **4.124** | grupo M **y** los 8 B de fuente del §4-bis (ya hechos) |
| `interfaceimp` (realmemcard) | 896 | grupo M |
| `systask` | 508 | grupo B **más** el parche de `locale.cpp` de la r26 §5 |

Las cuatro sueltas y **las cuatro juntas: DOL OK**.

---

## 1. Por qué el paquete de la r26 rompió el DOL: faltaban DOS LÍNEAS de `keep.lst`

**Reproducido al dígito.** Con las 28 líneas de `splits.txt` aplicadas y el
`keep.lst` del árbol sin tocar, el enlace da exactamente el hash del encargo:

    A,B,E,F,F2,G,H,I,J,K,L,M,N  --keepold      ROTO 6a1ed4c83221
    A,B,E,F,F2,G,H,I,J,K,L,M,N  (keep nuevo)   DOL OK

Las dos líneas son las que la r26 §3 ya daba por **obligatorias**:

    -zFEng.o:gap_06_8041D01C_data      +zFe2.o:gap_06_8041D01C_data
    -sdspmix.o:gap_07_804C03A4_bss     +snddrv.o:gap_07_804C03A4_bss

Mover una frontera mueve el `gap_` de un objeto a otro; sin actualizar
`keep.lst`, `-strip-unused-data` se come el relleno y todo lo que va detrás se
desplaza. **El control de sensibilidad**: el grupo A solo con el `keep.lst`
viejo da `ROTO f1d598d4fea4`, el mismo hash que anotó la r26.

### El control de la r26 SÍ era el reparto real — la premisa del encargo era falsa

El encargo daba por hecho que el control «todo extraído» ponía todas las
unidades como `NonMatching`. **No es así**: `c26lk_link.py` y `c26lk_fin.py`
leen el edge de `main.elf` de `build.ninja` **tal cual está** (429 objetos
nuestros + 161 extraídos) y sustituyen **sólo los extraídos**. El `f.rsp` de la
última ejecución de la r26 lo demuestra: 430 objetos de `build/GOWE69/src`.

Mi `c27lk_go.py` hace lo mismo de forma explícita y lo he vuelto a verificar:
el **control puro** (sin ningún grupo, reparto real, 590 objetos) da **DOL OK**.

### Y las falsificaciones no bloquean, porque están muertas

La razón por la que los grupos B, G, H y N pasan **sin tocar la fuente** aunque
una de las dos unidades de la frontera sea `Matching`: los símbolos fabricados
(`locale.cpp` con `statInfo`/`sMsg`, `memcard_memvectors.cpp` con
`slotA.384_804FF668`/`slotB.385_804FF66C`) **no los referencia nadie** y
`-strip-unused-data` los tira. Ocupan crédito falso en `matched_code`, pero para
el enlace son inertes.

**Corolario medido, y es una trampa:** si se retira la falsificación de
`memcard_memvectors.cpp` **sin** aplicar el grupo N, el enlace **rompe** —
el objeto extraído de `gc_memcard_interface_impl` importa `slotB.385_804FF66C`
de esa unidad. Retirada y grupo N van **juntos o ninguno**. Igual que
`locale.cpp` ↔ grupo B (r26 §5). Lo probé, lo revertí, y el árbol queda con la
falsificación puesta.

---

## 2. El paquete verificado — 15 grupos, uno a uno y todos juntos

Todo contra el **reparto real** (`configure.py` tal como está), **dos pasadas**:

| grupo | frontera | estado de las unidades | control |
|---|---|---|---|
| **A** | `zFe2`/`zFEng` `.data` → 0x8041D040 | las dos `NonMatching` | **DOL OK** |
| **B** | `locale`/`gc_driver`/`gc_interface` `.bss` | `locale` es **NUESTRA** | **DOL OK** |
| **E** | `sdspmix`/`snddrv` `.bss` → 0x804C03A0 | las dos `NonMatching` | **DOL OK** |
| **F** | `ssysinit`/`stagpat` `.data` | las dos `NonMatching` | **DOL OK** |
| **F2** | `stpparse`/`svecreal` `.data` | **las dos NUESTRAS** (cambio inocuo) | **DOL OK** |
| **G** | `srandom`/`sserver` `.data` | `srandom` es **NUESTRA** | **DOL OK** |
| **H** | `dering`/`criticalpath` `.bss` | `dering` es **NUESTRA** | **DOL OK** |
| **I** | `zMain`/`zMisc` `.bss` | las dos `NonMatching` | **DOL OK** |
| **J** | `zDebug`/`zDynamics` `.bss` | las dos `NonMatching` | **DOL OK** |
| **K** | `mpegl3base`/`snddrv` `.rodata` | las dos `NonMatching` | **DOL OK** |
| **L** | `pathsnd` `.rodata` (comodín) | `NonMatching` | **DOL OK** |
| **M** | `pathvol`/`avplayer`/`vfprintf`/`interfaceimp` `.rodata` | todas `NonMatching` | **DOL OK** |
| **N** | `memcard_memvectors`/`gc_mci` `.sdata` | `memcard_memvectors` es **NUESTRA** | **DOL OK** |
| **P** *(nuevo)* | `csis` `.rodata` 0x804130A0..0x80413110 | `NonMatching` | **DOL OK** |
| **Q** *(nuevo)* | `gc_mci` `.rodata` 0x80414398..0x804143F8 | `NonMatching` | **DOL OK** |

**Los quince juntos: DOL OK.** Ninguno queda fuera.

### Que una unidad sea `Matching` no basta para descartar el grupo

Es el paso 1 que pedía el encargo, y la respuesta es que **el criterio no
discrimina**: cinco grupos (B, F2, G, H, N) tocan una unidad promocionada y los
cinco pasan igual. Lo que decide es si el símbolo que cambia de dueño está
**vivo**. La prueba barata es el enlace, no la lista de `Matching`.

### El grupo P: `csis` no necesitaba «mover el objeto» a mano

La r26 §4 dejó `csis` fuera porque su rango empieza justo en el principio del
comodín, así que carvarlo **renombra** `auto_05_804130A0_rodata` a
`auto_05_80413110_rodata` en vez de partirlo, y el comodín viejo está en la
posición 437 del enlace, antes de `csis` (439).

**La regla general es la misma que para los demás**: el objeto comodín nuevo va
**detrás de la unidad que carvó el rango**. Aquí eso significa *quitar*
`auto_05_804130A0_rodata` de la posición 437 y *añadir* `auto_05_80413110_rodata`
detrás de `csis` — que es exactamente «reordenar», pero sale solo de la misma
regla. `c27lk_go.py` lo hace por sí mismo (empareja el `auto_XX_<dir>_<sec>` con
el cambio cuyo `end` es esa dirección). **DOL OK.**

`csis` **sigue sin promocionar** con el grupo puesto: le sobran 52 B de `.text`
(5.780 contra 5.728) y 56 B de `.rodata` (168 contra 112), y emite 28 B de
`.data` y 28 de `.bss` que la unidad no tiene. El grupo P es necesario pero muy
lejos de suficiente.

---

## 3. Qué hay que aplicar, exactamente

### `config/GOWE69/splits.txt` — 30 cambios
El fichero listo está en `…/scratchpad/c27lk_try/splits_R27.txt`; el diff sale
con `diff config/GOWE69/splits.txt c27lk_try/splits_R27.txt`. Es exactamente el
de la r26 §3 **más dos líneas nuevas**:

    Speed/Indep/Libs/csis/dev/source/library/cmn/csis.cpp:
        .rodata     start:0x804130A0 end:0x80413110          (grupo P)
    Packages/…/lib/gc/gc_memcard_interface_impl.cpp:
        .rodata     start:0x80414398 end:0x804143F8          (grupo Q)

### `config/GOWE69/keep.lst` — 2 líneas, **NO SE PUEDEN OMITIR**
Fichero listo en `c27lk_try/keep_R27.lst`.

    -zFEng.o:gap_06_8041D01C_data      +zFe2.o:gap_06_8041D01C_data
    -sdspmix.o:gap_07_804C03A4_bss     +snddrv.o:gap_07_804C03A4_bss

### `configure.py` — 7 objetos comodín que añadir, 2 que quitar

| añadir | detrás de |
|---|---|
| `auto_05_8040FC48_rodata` | `libc/vfprintf.c` |
| `auto_05_8040FF40_rodata` | `avplayer.cpp` |
| `auto_05_80413110_rodata` | **`csis.cpp`** |
| `auto_05_80413770_rodata` | `pathsnd.cpp` |
| `auto_05_80413960_rodata` | `pathvol.cpp` |
| `auto_05_804143F8_rodata` | **`gc_memcard_interface_impl.cpp`** |
| `auto_05_804149E8_rodata` | `interfaceimp.cpp` (realmemcard) |

Quitar **`auto_05_804147EC_rodata`** (sus 4 B pasan a ser el relleno de
alineación de la `.rodata` de `interfaceimp`) y **`auto_05_804130A0_rodata`**
(lo sustituye `auto_05_80413110_rodata`, ya en otra posición). Total 590 → 595.

### `locale.cpp` — sólo hace falta para `systask`
Medido esta ronda, unidad por unidad:

    ...paquete... promo:gc_memcard_interface_impl   DOL OK   (sin locale)
    ...paquete... promo:interfaceimp                DOL OK   (sin locale)
    ...paquete... promo:systask                     ROTO 82c9bbd2baa5  (sin locale)
    ...paquete... promo:las tres  +locale           DOL OK

El parche es el de la r26 §5: borrar las líneas 16-23 de
`realmemcard/…/lib/cmn/locale.cpp` (las dos definiciones fabricadas y el
`__asm__`) y poner `__attribute__((aligned(32)))` en `gTrcMsgBuffer`.
**Va junto con el grupo B.** No lo he aplicado al árbol.

---

## 4. `gc_memcard_interface_impl` (10.984 B) — CERRADA

El encargo daba dos pasos (definir `ALL_ENTRIES` y aplicar `REALMC_LCFMT_BASE`).
**El segundo estaba mal**: los 88 B de `.rodata` **no sobran**, son de la unidad
—su `.text` referencia `lbl_80414398` ("s") y `lbl_8041439C` ("ss")— y lo que
faltaba era el **rango** (grupo Q). Y había un tercer bloqueo que nadie había
visto.

### 4.1 El bloqueo que faltaba: el ORDEN de las funciones dentro de la TU

`libdiff` daba `gc_memcard_interface_impl` al **100 %** con la `.text` del mismo
tamaño… y el DOL salía con **7.700 bytes** distintos en 1.089 tramos. La causa:
objdiff empareja **por nombre**, así que un orden de emisión distinto dentro de
la unidad lee 100 % perfecto. Nuestras 35 funciones estaban en otro orden que el
objetivo desde el índice 6 (`Load` movía **+6.864 B**, `_CalcSignature`
**−7.264 B**).

**Es una comprobación general que hay que añadir al repertorio**: ordenar los
símbolos de `.text` del objeto nuestro y del extraído por desplazamiento y
compararlos. Vale para cualquier unidad al 100 % que no promociona.

### 4.2 Las reglas de emisión de GCC 2.9 que salieron de aquí, todas medidas

1. **Las funciones normales se emiten en orden de fuente.** Reordenar el `.cpp`
   basta y **no cambia el código**.
2. **Una función `inline` se emite fuera de línea al FINAL**, detrás de todas
   las normales, y sólo si algún llamante la llama de verdad.
3. **El orden entre las diferidas es el de sus CUERPOS** (dónde se parsean), no
   el de su primer uso ni el inverso. Por eso `~GCMessage`, con el cuerpo dentro
   de la clase en la cabecera, salía siempre **antes** que `FileHeader::Clear`.
4. **Un cuerpo dentro de la clase no emite símbolo** (confirma la nota de
   memoria): al meter `FileHeader::Clear` en el `struct`, su símbolo desapareció
   y el constructor engordó 60 B porque pasó a inlinearlo.
5. **GCC sólo inlinea lo que ya ha visto.** El objetivo tiene el constructor
   (#2) llamando a `FileHeader::Clear` **fuera de línea** y `Load` (#6)
   **inlineándola**: el cuerpo tiene que estar **entre los dos**. Con eso el
   constructor vuelve a 584 B y `Load` a 256 B, los dos exactos.

### 4.3 La fuente resultante (aplicada al árbol)

`src/Packages/realmemcard/3.04.01-layer2/source/lib/gc/gc_memcard_interface_impl.cpp`

- **las 35 definiciones reordenadas** al orden del objeto original;
- `inline void FileHeader::Clear()` movida **detrás del constructor y delante de
  `BootupCheck`**;
- `inline GCMessage::~GCMessage() {}` **fuera de línea**, justo detrás de la
  anterior (cierra `RealmcIface`, abre `Realmc`, define, y vuelve a abrir
  `RealmcIface`), con una guardia nueva en `gc_interface_impl.h`:

      #ifdef REALMC_GC_MESSAGE_DTOR_OUTOFLINE
          virtual ~GCMessage();
      #else
          virtual ~GCMessage() {}      // igual que antes: las otras 5 TU no cambian
      #endif

- `Init`, `_SetMsgOptions`, `_LcGetSlotString` y `Clear` de `GCMessage` pasan a
  `inline` (tienen que salir **detrás** de las dos diferidas);
- `const char *ALL_ENTRIES = "*";` **al final del fichero**, porque su `.sdata`
  va detrás de `slotA`/`slotB` de `_LcGetSlotString`.

**Resultado, verificado símbolo a símbolo:**

    .text    10.984 = 10.984, las 35 funciones en el mismo orden y con el mismo tamaño
    .rodata      96 = 96, byte a byte (el "*" cae en +28, como el original)
    .sdata       12 = 12, en el orden bueno: slotA, slotB, ALL_ENTRIES
    libdiff      100.00 %  (10984/10984)

### 4.4 La deuda que hay que decir

Para que el literal `"*"` quede internado como el **sexto** de la TU (detrás de
las cinco cadenas `LC_msg` de la cabecera y **delante** del `""` de
`SetAutosave`) mientras la variable se ensambla al final, el literal y la
definición tienen que estar **separados**. He puesto un

    inline const char *_AllEntriesLiteral() { return "*"; }

al principio de `namespace RealmcIface` (no emite nada: es una `inline` sin
llamantes). **Esa función no estaba en el original**; el original tendría el
`"*"` en algún otro sitio parseado temprano que no genera código —no hay ninguna
reubicación de `.text` a `lbl_804143B4`, así que no se puede saber cuál—. El
objeto sale idéntico, pero la forma es mía y queda anotada como deuda.

Lo mismo, en menor grado, con el bloque `namespace Realmc { inline
GCMessage::~GCMessage() {} }` incrustado en medio de `RealmcIface`: es feo y
seguramente el original lo escribía de otra forma; lo que está medido es que
**esa posición de emisión** es la que hace falta.

---

## 4-bis. `pathvol` (4.124 B) — CERRADA con OCHO BYTES

Con el grupo M puesto le faltaban 8 B de `.rodata` (176 contra 184): el literal
**`"%s  %s"`** de 0x80413958. Es una cadena **muerta** —ninguna reubicación del
DOL apunta ahí; las de `pathvol.o` a `lbl_80413954` son al flotante que hay 4 B
antes— así que la `.text` ya casaba al 100 % sin ella.

Internándola **la última** de la TU, el bloque pasa a medir 184 B **byte a byte
iguales** y la unidad **promociona**. La forma que he usado es la misma deuda
del §4.4:

    static inline const char *PATHI_volfmt() { return "%s  %s"; }

al final de `pathvol.cpp` (una `inline` sin llamantes no emite código: `.text`
sigue en 4.124 = 4.124 y `libdiff` en 100,00 %).

**Esta es la tercera unidad de la ronda en la que el bloqueo es un literal que
el objetivo emite y nosotros no.** Merece un barrido: comparar el `.rodata`
nuestro con el del reparto es barato y delata el hueco exacto.

---

## 5. El siguiente, con el diagnóstico ya cerrado: `gc_driver` (8.952 B)

**Es el mismo patrón que `gc_mci`, y ya está contado:**

1. **Sobran 8 funciones = 192 B exactos** (`.text` 9.144 contra 8.952): los
   `operator new/delete` de `GCDriver`. `gc_driver.h:66-73` los declara
   `static inline` y los define **fuera de la clase** en la línea 136, así que
   GCC emite las ocho copias fuera de línea. **Metiendo los cuerpos dentro de la
   clase dejan de emitir símbolo** (regla 4 del §4.2).
2. **El orden de las funciones también está mal** desde el índice 5. El orden
   del objetivo está volcado entero en el §5-bis.
3. **Emite 184 B de `.rodata` sin rango.**
4. Su `.bss` ya la arregla el grupo B.

### Y el punto 3 está BLOQUEADO POR `symbols.txt`, que yo no puedo tocar

El bloque de `gc_driver` es **0x80414408..0x804144C0** (192 B menos los 8 de
delante). Al pedirle ese rango, `dtk` lo rechaza con un mensaje exacto:

    Split auto_05_804130A0_rodata .rodata (0x804130A0..0x80414408)
    ends within symbol 'lbl_80414400' (0x80414400..0x80414424)

`lbl_80414400` es **un solo símbolo de 36 B** que junta dos cosas de dueños
distintos: los 8 B de `"&|!~^"` (0x80414400, que referencia
**`memcard_utilities.o`**) y los 28 B del bloque `LC_msg` de `gc_driver`
(0x80414408). **Para desbloquearlo hay que partirlo en `symbols.txt`**:
`lbl_80414400` con `size:0x8` y un `lbl_80414408` nuevo con `size:0x1C`.
Es el único caso de la ronda en el que el reparto tropieza con `symbols.txt`.

**No he tocado la fuente de `gc_driver`**: sin ese rango la unidad no puede
promocionar, y dejar el reordenado a medias no aporta nada.

### 5-bis. El orden de emisión del objetivo en `gc_driver` (46 funciones)

Los cinco primeros ya casan. Del 5 en adelante, el objetivo va:
`CardExists`, `GetFileBlocks`, `GetSectorSize`, `CardRemovalCallback`, `Mount`,
`Unmount`, `OpenFile`, `WriteHeaderData`, **`GetBannerSize`**, **`GetIconSize`**,
`WriteFile`, `FlushWriteBuffer`, `SetHeaderInfo`, `ReadFile`, `CloseFile`,
`GetOpenFileSize`, `GetFreeCardSpace`, **`DeleteFile`**, `FormatCard`,
**`IsCardPresent`**, **`WasCardPresent`**, **`ResetWasCardPresent`**,
**`IsOurFile`**, `FindFirst`, `FindNext`, `Seek`, `SetAttributes`,
`FindFileNumber`, los cinco `NGCSportsBio*`, **`RecordIplDataChecksum`**,
**`VerifyIplDataChecksum`**, y luego el grupo diferido —`~DeviceDriver`,
`CmnFileDescriptor::Clear`, `CmnFileDescriptor::Init`, `GcFileHeader::Clear`,
`GcFileDescriptor::Clear`, `GcFileDescriptor::Init`— que **ya está bien**
en cuanto desaparezcan los ocho operadores de en medio.

## 6. Los demás, con la medida hecha contra el paquete completo

| unidad | B | qué le falta EXACTAMENTE |
|---|---|---|
| `snddrv` | 11.920 | sigue siendo `sfxrevc.c(952): L0039: undefined lbl_80412B78`. Los 4 B del `1.0f` son de `sfxrevc`, que ya está promocionada |
| `pathsnd` | 8.452 | `Stop__Q24Path10IPathTrack…` y las otras tres virtuales, declaradas y sin escribir |
| `gc_interface` (rmc) | 6.152 | `asd2` referencia `__static_initialization_and_destruction_0_803906C0`: hay que fundir `asd2` en `gc_interface` |
| `csis` | 5.728 | §2: sobran 52 B de `.text` y 56 de `.rodata`, y emite `.data`/`.bss` de más |
| `avplayer` | 4.224 | dos cosas contadas: emite un `$LC0` **cadena vacía** de 4 B al principio que el objetivo no tiene, y su segunda constante doble es `0x4330000000000000` donde el objetivo repite `0x41E0000000000000`. `.rodata` 72 contra 68 |
| `vfprintf` | 6.948 | emite 128 B de `.bss` y 8 de `.sdata` sin rango (r25 §2.2) |
| `zFEng` | 71.460 | sin cambios respecto a la r26 §6: con el grupo A deja de dar `L0039` y el bloqueo pasa a ser suyo |

---

## 7. Lo que NO he probado

1. **`gc_driver`** (§5): diagnóstico completo, **no ejecutado**, y bloqueado por
   los dos símbolos de `symbols.txt` que no puedo partir. Con eso hecho es lo
   que atacaría primero: 8.952 B, mismo patrón que acabo de cerrar dos veces.
2. **`avplayer`** (4.224 B): las dos diferencias están contadas en el §6 pero no
   he buscado de dónde salen en la fuente.
3. **El `1.0f` de `sfxrevc.c:952`** y las cuatro virtuales de `pathsnd`: no los
   he tocado; siguen como los dejó la r26.
4. **`audit.py`/`frozen.py` sobre las tres verdes**: no los he pasado. El
   reparto propuesto no está aplicado, así que leerían el `build/` con el
   reparto viejo. **Hay que pasarlos al aplicar.**
5. **La corrección limpia `systask`/`timerthread`** (r26 §5, última nota) y la
   de `vfprintf`/`vfprintf_1`.
6. **El barrido de orden de funciones sobre las otras 55 candidatas**: sólo lo
   he mirado en `gc_mci` y `gc_driver`, y en las dos había desajuste. Es un
   barrido barato (comparar las dos listas de símbolos de `.text`) y muy
   probablemente hay más.
7. **El barrido de literales muertos**: comparar `.rodata` nuestra contra la del
   reparto, unidad por unidad. Ha decidido `gc_mci` y `pathvol` en esta ronda y
   tampoco lo he pasado a las demás.
8. **De dónde sale de verdad el `"*"` temprano** de `gc_mci` (§4.4) ni el
   `"%s  %s"` de `pathvol` (§4-bis).

---

## 8. Estado del árbol y herramientas

**Ficheros de fuente tocados (aplicados, y el DOL sigue OK con ellos):**

- `…/lib/gc/gc_memcard_interface_impl.cpp` — reordenada y con `ALL_ENTRIES`.
- `…/impl/gc_interface_impl.h` — la guardia `REALMC_GC_MESSAGE_DTOR_OUTOFLINE`,
  con el `#else` idéntico a lo que había, así que las otras cinco TU que
  incluyen la cabecera **no cambian**.
- `…/Libs/path/5.01.04/source/cmn/pathvol.cpp` — el literal muerto `"%s  %s"`.

Los tres son inocuos hoy: `gc_memcard_interface_impl` y `pathvol` son
`NonMatching`, así que el enlace usa los objetos extraídos. **Control
comprobado: el árbol tal cual, sin ningún cambio de `splits.txt`, sigue dando
DOL OK**, y las dos unidades siguen en 100,00 % (`libdiff`).

`memcard_memvectors.cpp` y `locale.cpp` están **como los encontré** (sus
falsificaciones puestas): retirarlas rompe el enlace si no van los grupos N y B.

**Nada de `config/` ni `configure.py` está tocado.** Sin commits.

| script (scratchpad, `c27lk_`) | qué hace |
|---|---|
| **`c27lk_go.py`** | la herramienta de la ronda: `<grupos> [--promo U] [--locale] [--keepold] [--scan lista] [--reuse] [--dump]`. Genera `splits`, llama a `dtk dol split`, recalcula `keep.lst`, coloca los comodines nuevos **por sí mismo** y enlaza contra el reparto real |
| `c27lk_dolcmp.py` | dónde difiere el DOL del original: tramos, bytes y direcciones |
| `c27lk_objcmp.py` | secciones nuestras contra extraídas (tamaño, alineación, contenido) |
| `c27lk_reord.py` | el reordenador de `gc_memcard_interface_impl.cpp` |
| `c27lk_ae2.py` / `c27lk_ae3.py` | parchear + compilar + volcar/enlazar en un paso |

**Trampa de la herramienta, y costó tres intentos:** `dtk` resuelve **todas** las
rutas relativas del `.yml` (`splits`, `symbols`, `object_base`) contra el **CWD**,
no contra el directorio del `.yml`. Hay que lanzarlo con `cwd` en el sandbox, y
el sandbox necesita sus copias de `config/GOWE69/symbols.txt` y
`orig/GOWE69/sys/main.dol`. Si no, `dtk` no encuentra el `splits` y falla con un
mensaje que no lo dice (`auto_02_803A4234_over … crosses logical ReadOnlyData
range`).
