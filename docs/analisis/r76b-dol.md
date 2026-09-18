# r76b `DOL OK`: seis líneas de `keep.lst`, y dos de los cuatro puntos no eran nada

    main.dol   9eed2ca9e3bd -> 9619ba57c9919f95f7f2ac951a2166a3517f91e3   byte a byte
    ninja build/GOWE69/ok   [3/3] CHECK config\GOWE69\build.sha1  -> OK
    cambios: config/GOWE69/keep.lst, +6 símbolos (y 7 líneas de comentario)
    src/ sin tocar. Ni un asm, ni un pin, ni una barrera. Ninguna degradación revertida.

La r76b degradó siete unidades a `NonMatching` (zMain, zFeOverlay, zSim, zFEng,
zAnim, zBWare, zDynamics). Degradar cambia el objeto que se enlaza — el extraído
en vez del nuestro — y **de las siete, sólo dos movieron el DOL**. Las dos por el
mismo mecanismo, y las dos se arreglan sin tocar el código.

## 0. El perfil de desplazamiento tenía cuatro puntos; dos son ruido

`scratchpad/r76_dol/shift.py` daba esto:

| a partir de | delta | símbolo | veredicto |
|---|---|---|---|
| `803C6A72` | `+2` | `lbl_803C6A72` | **no es un desplazamiento** (§4) |
| `803C6A92` | `+0` | `lbl_803C6A92` | vuelve: cierra el falso positivo |
| `80416444` | `-24` | `gap_06_80416444_data` | **zBWare `.data`** (§2) |
| `80439490` | `-40` | `lbl_80439490` | −24 arrastrado **−16 de zFeOverlay `.data`** (§3) |
| `80451EA4` | `-32` | `pad_06_80451EA4_data` | **no es una causa**: alineación (§5) |

`shift.py` sólo ve los símbolos que llevan su dirección en el nombre, y son
dispersos: dice «a partir de X» cuando el cambio ocurrió en algún punto entre el
símbolo anterior y X. Para localizar al culpable al byte hace falta otra cosa.

## 1. La herramienta que faltaba: el mapa contra `splits.txt`

`splits.txt` ya tiene la verdad del original: para cada unidad, la dirección de
inicio y de fin de cada sección **en el DOL bueno**. El mapa del enlazador tiene
lo mismo para el enlace de hoy. Restar una de otra localiza la unidad culpable
sin adivinar (`scratchpad/r76b_dol/mapcmp.py`):

```
80415938  zBWare       .data   dstart=   +0  dsize=  -24
80416488  zCamera      .data   dstart=  -24  dsize=   +0     <- ya sólo arrastra
...
804390E4  zFeOverlay   .data   dstart=  -24  dsize=  -16
80439490  crt0         .data   dstart=  -40  dsize=   +0     <- ya sólo arrastra
```

**Dos unidades con `dsize` negativo, y ninguna más.** Ni `.text`, ni `.rodata`,
ni `.bss`, ni `.sdata`/`.sdata2`/`.sbss`, ni las otras cinco degradadas. Con eso
la búsqueda pasa de «cuatro puntos en 4 MB» a «dos secciones de 2.896 y 940 B».

La segunda herramienta (`scratchpad/r76b_dol/falta.py`) cruza los símbolos del
`.o` extraído con los del mapa y saca el desplazamiento **símbolo a símbolo**,
marcando dónde cambia. Ahí el culpable se lee directamente.

## 2. zBWare `.data`: −24 B, y son cinco variables con nombre

`falta.py` sobre `zBWare.o .data` (0xB50 en el objeto, 0xB38 enlazado):

```
  +000C 8      gap_06_80415944_data      (no aparece en el mapa)  GLOB
  +0014 4      bPNodeSlotPool         -> 80415944  d=  -8   <<< CAMBIA
  ...
  +0AE4 20     lbl_8041641C              (no aparece en el mapa)  GLOB
  +0AF8 4      bMemoryRandomFillPattern -> 80416418  d= -24   <<< CAMBIA
```

Y la causa se ve poniendo los dos objetos uno al lado del otro. Lo que el
extraído llama `gap_06_80415944_data` (8 B anónimos) **en nuestro objeto son dos
variables con nombre**, y las dos están en `keep.lst` desde hace rondas:

| extraído | nuestro objeto | ¿en `keep.lst`? |
|---|---|---|
| `gap_06_80415944_data` (8 B) | `SendPacketFunction`, `ServiceMonitorFunction` | **sí, las dos** |
| `lbl_8041641C` (20 B) | `bMemoryPrintEachAllocation`, `bMemoryPrintAllocationRangeLow`, `bMemoryPrintAllocationRangeHigh`, `EnableCleanupBorrowedMemoryBlock`, `BorrowMemoryBlockMinSize` | **sí, las cinco** |
| `gap_06_80415954_data` (4 B) | `bPListAllocationNumber` | sí |

Ése es el mecanismo entero: **`keep.lst` protege por NOMBRE, y el objeto extraído
llama a esos bytes de otra manera.** Mientras enlazábamos el nuestro, los siete
nombres mantenían vivos los 32 B; al enlazar el extraído, esos nombres quedan
`UNDEFINED` y `-strip-unused-data` se lleva los bytes.

No se lleva todos: se lleva `size & ~(align-1)` de cada dato muerto. El `.data`
extraído de zBWare tiene `2**3`, así que:

    gap_06_80415944_data    8 B   8 & ~7 = 8   se va entero        -> -8
    lbl_8041641C           20 B  20 & ~7 = 16  deja un cadáver de 4 -> -16
    gap_06_80415954_data    4 B   4 & ~7 = 0   sobrevive            ->  0
    gap_06_804161E8_data    2 B                sobrevive            ->  0
                                                              TOTAL   -24

## 3. zFeOverlay `.data`: −16 B, y aquí sí caen los de 4 B

Lo mismo, con una diferencia que confirma la fórmula: el `.data` extraído de
zFeOverlay tiene alineación `2**2`, no `2**3`. Con `align = 4`, un símbolo muerto
de 4 B cumple `4 & ~3 = 4` y **se va entero**:

```
  +0070 4   gap_06_80439154_data   (no aparece)  ->  -4
  +0150 4   gap_06_80439234_data   (no aparece)  ->  -8
  +0158 4   lbl_8043923C           (no aparece)  -> -12
  +03A8 4   gap_06_8043948C_data   (no aparece)  -> -16
```

Cuatro símbolos de 4 B, −16 B. En zBWare los de 4 B sobrevivieron; aquí no. **La
alineación de la sección decide, no el tamaño del dato.** Es la misma fórmula.

## 4. El `+2` de `803C6A72` no es un desplazamiento: es un nombre de `dtk`

`shift.py` decía `lbl_803C6A72 -> 803C6A74`. No hay nada que arreglar: los
símbolos del objeto extraído lo dicen solos.

```
  +0032  size=2   GLOB gap_02_803C6A72_over
  +0034  size=30  GLOB lbl_803C6A72
```

`dtk` puso el nombre `lbl_803C6A72` a un símbolo que arranca en `803C6A74`,
porque los 2 B de `803C6A72` se los quedó el `gap_02_803C6A72_over` de delante.
Es un artefacto del nombre, no del enlace: la sección entra en `803C6A40` con
tamaño `0x2118`, exactamente lo que dice `splits.txt`.

**Control**: el `+2` sigue ahí en el enlace que da el sha1 bueno. Con el DOL
correcto byte a byte, `shift.py` imprime igualmente `803C6A72 delta +2`.

## 5. El `-32` de `80451EA4` tampoco es una causa: es `CARDUnlock`

De `-40` a `-32` no hay nada que se haya encogido: hay 8 B que **aparecen**. El
mapa lo explica en una línea:

    8044E5B0  CARDBios     .data   dstart=  -40
    8044E620  CARDUnlock   .data   dstart=  -32        al = 32

`CARDUnlock.o` aporta su `.data` con alineación 32. Con `-40` acumulado tocaría
`8044E5F8`, que no es múltiplo de 32, y el enlazador rellena hasta `8044E600`:
se come 8 de los 40. **Control**: arreglando sólo zBWare (quedan `-16`
pendientes), la misma alineación se los come **enteros** y el perfil vuelve a
cero en ese mismo punto — el `-16` desaparece en `CARDUnlock` sin que nadie lo
arregle. Por eso `shift.py` señalaba `80451EA4` en los dos casos: es el primer
símbolo con dirección en el nombre **después** de `CARDUnlock`.

Esto es una trampa que merece quedar escrita: **un punto donde el desplazamiento
SUBE hacia cero no es una unidad culpable, es una alineación que absorbe.** Sólo
los puntos donde BAJA son trabajo.

## 6. El arreglo: seis líneas en `keep.lst`

```
zBWare.o:gap_06_80415944_data
zBWare.o:lbl_8041641C
zFeOverlay.o:gap_06_80439154_data
zFeOverlay.o:gap_06_80439234_data
zFeOverlay.o:lbl_8043923C
zFeOverlay.o:gap_06_8043948C_data
```

Sólo se añade; no se quitó ni se movió ninguna línea. El fichero sigue siendo
CRLF puro (3.501 CR, 3.501 LF, termina en CRLF).

Las líneas viejas que nombran los símbolos de NUESTRO objeto
(`zBWare.o:SendPacketFunction`, etc.) **se quedan**: hoy caen en la lista
`UNDEFINED` del mapa, que es inofensiva — ya hay 2.905 entradas ahí, la mayoría
de unidades que llevan rondas en `NonMatching` — y volverán a hacer falta el día
que la unidad se promocione.

### Control línea a línea

`scratchpad/r76b_dol/control.py` enlaza seis veces, cada una sin UNA de las seis
líneas, y dice dónde rompe:

| línea quitada | resultado | primer punto de desplazamiento |
|---|---|---|
| `zBWare.o:gap_06_80415944_data` | ROMPE | `8041641C` delta `-8` |
| `zBWare.o:lbl_8041641C` | ROMPE | `80416444` delta `-16` |
| `zFeOverlay.o:gap_06_80439154_data` | ROMPE | `80439234` delta `-4` |
| `zFeOverlay.o:gap_06_80439234_data` | ROMPE | `8043923C` delta `-4` |
| `zFeOverlay.o:lbl_8043923C` | ROMPE | `8043948C` delta `-4` |
| `zFeOverlay.o:gap_06_8043948C_data` | **DOL OK** | — |
| ninguna | DOL OK | — |

Cinco de las seis son imprescindibles y cada una rompe **en su propia dirección**.
La sexta se declara medida y honestamente: **no mueve el DOL**, porque son los
últimos 4 B de la sección y se los come la alineación de la `.data` siguiente.
Se deja porque devuelve la sección a su tamaño original exacto (`0x3AC`) y cuesta
una línea; queda anotada como tal en el propio `keep.lst`.

## 7. Lo que se descartó, con su medida

- **Alineación de sección.** Ya se había probado en la r76b declarando `align:`
  en `.bss`/`.ctors` de las siete degradadas: el perfil no se movió ni un byte.
  Confirmado aquí: el censo `mapcmp` da `dsize=+0` en `.bss` y `.ctors` de las
  siete.
- **Las otras cinco degradadas** (zMain, zSim, zFEng, zAnim, zDynamics) **no
  aportan nada al problema**. Ninguna aparece con `dsize` negativo en ninguna
  sección. Medido, no supuesto.
- **Revertir una degradación no hacía falta**, y además no habría funcionado:
  enlazando NUESTROS objetos en las siete (`rl.py --ours`) el DOL sigue sin
  casar (`b203d459…`), con un `-8` propio a partir de `8030EA7C`. La degradación
  no era el error; era el destapador.
- **Los `dsize` de −1 a −7** que salen por todo el SDK de Dolphin
  (`OSAlloc -6`, `SIBios -7`, …) son cadáveres de cola preexistentes: todos con
  `dstart=+0`, todos absorbidos por la alineación del siguiente. No son trabajo.

## 8. Herramientas nuevas

En `scratchpad/r76b_dol/`:

- `rl.py` — relink con sustituciones **en los dos sentidos** (`--ours` / `--ext`)
  y con `keep.lst` alternativo (`--keep`). Lo que faltaba en `relink.py`, que
  sólo sabía ir hacia el extraído.
- `mapcmp.py` — mapa del enlace **contra `splits.txt`**: `dstart` y `dsize` por
  unidad y sección. Es el que localiza la unidad culpable en una pasada.
- `falta.py` — cruce símbolo a símbolo del `.o` con el mapa: qué desapareció y
  cuánto desplazó cada desaparición.
- `secsyms.py` — símbolos de una sección de un `.o`, ordenados y con los huecos.
- `control.py` — la sonda: enlaza N veces quitando una línea cada vez.
