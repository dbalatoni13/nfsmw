# r63 / spch -- `spchdata.c` escrita, medida y con el DOL en verde

**Resultado: la unidad esta LISTA. El enlace simulado con `spchdata.o` dentro y
los dos comodines partidos da el DOL de referencia
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`, byte a byte, y los 29 simbolos de la
zona caen en su direccion exacta.** Falta solo aplicar 2 lineas de `splits.txt` +
1 de `configure.py` y re-extraer, que no me toca.

Techo `544 -> 545`. `linked 523 -> 524`.

---

## 1. La fuente

`src/Speed/Indep/Libs/spch/dev/source/library/cmn/spchdata.c` (4.164 B, ASCII
puro, LF). Sello del objeto, **tres compilaciones seguidas**:

```
sello1 785eb5bfde880c8f315ed79ad3f4178f7e521ffc
sello2 785eb5bfde880c8f315ed79ad3f4178f7e521ffc
sello3 785eb5bfde880c8f315ed79ad3f4178f7e521ffc
```

El objeto que sale (`cflags_spch` tal cual, sin tocar nada):

```
.text       0 B      .rodata     0 B
.data      48 B  al4        .bss      520 B  al4
```

**`.data`, contenido contra el ELF original, los 48 B:**

```
nuestro : 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001
original: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001
```

**Los 20 simbolos, desplazamiento contra `symbols.txt`, 20 de 20 exactos:**

| `.data` (base 0x80451ED0) | off | | `.bss` (base 0x804CC6F8) | off |
|---|---:|---|---|---:|
| gMemAlloc | 0 | | gCallbacks | 0 |
| gMemFree | 4 | | gEventDats | 20 |
| gExtVecs | 8 | | gVoxEvents | 84 |
| gSPCH_Initialized | 20 | | gVoxInGame | 404 |
| gVoxBanks | 24 | | gGameNum | 468 |
| gUniqueBankHandle | 28 | | gDataRate | 472 |
| gNumBanks | 32 | | gFilterSetting | 476 |
| gBankCount | 36 | | gPreLoadTicks | 508 |
| gSPCH_AddEvent | 40 | | gLastTick | 512 |
| gClearCycle | 44 | | gLastSubTick | 516 |

---

## 2. Los DOS mecanismos que deciden esta unidad, medidos

### 2.1 `.data` contra `.bss`: manda el inicializador (confirma `nfsmw-inicializador-roba`)

Con `-x c++` una definicion **sin** inicializador va a `.bss`; **con**
inicializador --aunque valga cero-- va a `.data`. GCC 2.95 solo manda a `.bss`
los `VAR_DECL` con `DECL_INITIAL` nulo. Por eso las diez de arriba llevan `= 0`
explicito y las diez de abajo no llevan nada. Quitar un `= 0` cambia de seccion
y descuadra las dos.

### 2.2 EL ORDEN DE `.bss` NO ES EL DE DEFINICION: es el de PRIMERA DECLARACION

**Hallazgo nuevo, y sirve para todo el proyecto.** GCC 2.95 emite `.data` en el
momento de la definicion, pero **difiere `.bss` a `finish_file`, recorriendo la
cadena de declaraciones globales**. Un `extern` previo --el de una cabecera
incluida antes, por ejemplo-- **adelanta el simbolo al principio de `.bss`**,
conservando su posicion original en la cadena.

Prueba de un renglon (`scratchpad`, 3 s):

```c
extern int e1;  extern int e2;
int d1;  int e2;  int d2;  int e1;  int d3;
```
```
.bss:  e1(0)  e2(4)  d1(8)  d2(12)  d3(16)
```

o sea, **el orden de los `extern`, no el de las definiciones**.

**Y aqui costo el primer intento.** La version 1 de `spchdata.c` incluia
`spchi.h`, que trae `extern SPCHType_ExtVecs gExtVecs;` y
`extern EventDatInfo gEventDats[8];`. Resultado: `gEventDats` salio en el
desplazamiento **0** de `.bss` y `gCallbacks` detras, al reves que el original.
`.data` salio perfecta ya en ese intento -- el mecanismo solo toca `.bss`.

La version 2 **no incluye `spchi.h`**: incluye `spch/spch.h` (que no declara
ninguna variable) y define localmente los seis tipos que hacen falta
(`EventDatInfo`, `SPCH_Callbacks`, `VoxEventItem`, `VoxPendingEvents`,
`VOXINGAME`, `VoxBankInfo`), con declaraciones adelantadas de `VoxData`,
`VoxEvent` y `VOXBANKHDR`. Cero cambios en cabecera compartida.

### 2.3 Corroboracion: el DWARF del original dice lo mismo

`python scripts/libdwarf.py cu spchdata.c` **existe** y trae las 20 variables.
Su orden de DIE es:

```
gSPCH_AddEvent (suelto, entre los typedef de spch.h)
gCallbacks gExtVecs gSPCH_Initialized gMemAlloc gMemFree
gVoxBanks gNumBanks gBankCount gUniqueBankHandle
gEventDats gVoxEvents gVoxInGame gGameNum gDataRate gFilterSetting
gPreLoadTicks gClearCycle gLastTick gLastSubTick
```

**Ese orden NO es el de las direcciones en `.data`, pero SI es exactamente el de
`.bss`.** Con 2.2 en la mano se lee solo: en el original ese bloque son los
`extern` que estaban al final de `spchi.h`, y las definiciones de `spchdata.c`
van en orden de direccion. Es decir: **el orden de `.bss` del original lo puso la
CABECERA, no `spchdata.c`**. Nota de metodo para el DWARF-1: **un DIE de variable
con direccion puede venir de un `extern`, no de una definicion** -- la direccion
sale de la reubicacion, no de que la unidad la defina.

---

## 3. La prueba del DOL sin re-extraer

No se puede usar `trypromo.py` porque `spchdata.o` no esta en la lista de enlace.
Se simulo la re-extraccion **partiendo los dos comodines a mano** (cirugia ELF en
`scratchpad`, sin escribir en `build/GOWE69/obj`):

```
auto_06_80451EA4_data.o  224 B  ->  head_data.o  44 B  (pad_06_80451EA4_data, _4Path.pfstates, seedPATH)
                                    tail_data.o 132 B  (gAddEventStatus_80451F00 ... gap_06_80451F81_data)
auto_07_804CC6F8_bss.o  1928 B  ->  (se disuelve entero en spchdata)
                                    tail_bss.o  1408 B (gEventChoice_804CC900, gRandArray)
```

y armando la `.rsp` como quedaria: `head_data` en el sitio del comodin viejo
(detras de `csis.o`), y `spchdata.o` + `tail_data.o` + `tail_bss.o` detras de
`spchbank.o`. Mismo `ngcld`, mismos `-strip-unused-data -keep config/GOWE69/keep.lst`,
mismo `ldscript.ld`, mismo `dtk elf2dol`.

```
CONTROL   enlace sin tocar nada          9619ba57c9919f95f7f2ac951a2166a3517f91e3   (= referencia)
PRUEBA    enlace con spchdata.o dentro   DOL OK
SIMBOLOS  29 de 29 en su direccion exacta (los 20 de spchdata + los 9 vecinos de los dos comodines)
```

---

## 4. Lo que hay que aplicar (yo NO lo he tocado)

### `config/GOWE69/splits.txt` -- bloque NUEVO delante de la linea 1739 (`spchevnt.c`)

TAB de sangria, nombre de seccion a 12 columnas, **LF puro**:

```
Speed/Indep/Libs/spch/dev/source/library/cmn/spchdata.c:
	.data       start:0x80451ED0 end:0x80451F00
	.bss        start:0x804CC6F8 end:0x804CC900

```

`checksplits` sobre el `splits.txt` con el bloque metido: **1.117 rangos,
0 solapes, 0 rangos que cortan un simbolo. LIMPIO.**

**Sin linea de `.text`.** El original tiene `.text 0x8037D988..0x8037D988`, de
longitud cero; hay siete precedentes sin rango de `.text` (`OSUtf.c`, `DEMOPad.c`,
`device.cpp`, `systemvars.cpp`, `prodg_fixes.cpp`, `AXComp.c`, `DSPCode.c`).

### `configure.py` -- en la libreria `spch`, detras de `spchbank.c` (linea 1079)

```python
            Object(Matching, "Speed/Indep/Libs/spch/dev/source/library/cmn/spchdata.c"),
```

**`Matching` directamente**, sin fase de sondeo: la fuente esta escrita y medida.

### `symbols.txt`: NADA. `keep.lst`: NADA.

Las cuatro rayas caen en inicio de simbolo con nombre (`gMemAlloc`,
`gAddEventStatus`, `gCallbacks`, `gEventChoice`). **Cero huecos `pad_` nuevos.**

---

## 5. Los tres riesgos de r62/p3, ahora MEDIDOS

### 5.1 La colocacion de los comodines de cola: era el riesgo real, y esta cerrado

r62/p3 lo dejo como "indicio fuerte, no medida". Las dos mitades:

**(a) La regla del troceador, verificada sobre los comodines de hoy.** Cada
`auto_*` va **inmediatamente detras de la unidad cuya seccion TERMINA en su
direccion de inicio**:

```
csis.cpp     .rodata end:0x80413110 / .data end:0x80451EA4
             -> auto_06_80451EA4_data (446) y auto_05_80413110_rodata (447), justo detras de csis (445)
pathreal.cpp .bss    end:0x804CC6F8 / .sdata end:0x804FF608
             -> auto_08_804FF608_sdata (452) y auto_07_804CC6F8_bss (453), justo detras de pathreal (451)
```

Con el bloque nuevo, `spchdata.c` es la unica unidad cuya `.data` acaba en
0x80451F00 y cuya `.bss` acaba en 0x804CC900, asi que los dos comodines nuevos
caen detras de ella. **Ya no es conjetura: es la misma regla que produjo los
cuatro comodines de hoy.**

**(b) Que pasa si me equivoco.** Medido: comodines de cola en el sitio del
comodin VIEJO en vez de detras de `spchdata` ->

```
DOL ROTO (2d953fe9be4b)
```

### 5.2 `end:0x804CC900` contra `end:0x804CC8FE`: confirmado, y el motivo es OTRO

r62/p3 razonaba por la entrada latente `keep.lst:874 gap_07_804CC8FE_bss`. El
motivo real es mas simple y mas duro: **nuestra `.bss` mide 520 B**, no 518
(GCC rellena la seccion a `al4` detras de `gLastSubTick`). Con
`end:0x804CC8FE` el troceador dejaria un `pad_` de 2 B al principio del comodin
de cola y todo se desplazaria +2. Medido:

```
end:0x804CC900   DOL OK
end:0x804CC8FE   DOL ROTO (71aeda7e0d13)
```

### 5.3 `keep.lst:874` es INDIFERENTE -- el aviso de r62/p3 sobra

Medido en las dos direcciones sobre el enlace completo:

```
con la linea gap_07_804CC8FE_bss     DOL OK
sin la linea gap_07_804CC8FE_bss     DOL OK
```

El simbolo deja de existir tras la re-extraccion (queda dentro de la `.bss` de
`spchdata.o`, que no lo nombra) y **ngcld ignora en silencio un nombre de
`-keep` que nadie define**. La linea se convierte en FANTASMA: `keepchk2.py` la
cantara, y quitarla es cosmetica. **No hay que tocar `keep.lst` para promocionar.**

---

## 6. La ventana de colocacion, por si el troceador sorprende

Barrido de la posicion de `spchdata.o` en la lista de enlace (con las colas
siempre detras de el):

```
detras de csis.o       (445)  DOL ROTO (a19966602f99)
detras de pathreal.o   (451)  DOL OK
detras de pathrand.o   (467)  DOL OK
detras de spchbank.o   (468)  DOL OK      <- la que propongo
detras de spchcsis.o   (478)  DOL OK
detras de filesys_c.o  (479)  DOL OK
detras de exit.o       (496)  DOL ROTO (78b668ea52b7)
```

**La ventana valida es `(pathreal.o, filesys.o)`, y son 29 posiciones.** El
motivo: entre el indice 446 y el 483 **ningun objeto aporta un solo byte de
`.data`**, y entre el 453 y el 481 ninguno aporta `.bss` (los once `.o` de `spch`
y los catorce de `path` son codigo puro). Es un margen comodo: aunque el
troceador coloque `spchdata.o` en otro punto de la libreria, sigue casando.

---

## 7. Negativos, para que `previo.py` los encuentre

Los dos estan anotados **en el comentario de cabecera de `spchdata.c`**, junto a
las definiciones que gobiernan:

1. **Incluir `spchi.h` rompe el orden de `.bss`.** Sus dos `extern` (`gExtVecs`,
   `gEventDats`) adelantan `gEventDats` al desplazamiento 0 y empujan
   `gCallbacks` a 64. `.data` sale bien igual. Si alguien "arregla" la fuente
   metiendo el include natural, la unidad deja de casar sin que `.data` se queje.
2. **Quitar un `= 0`** manda el simbolo de `.data` a `.bss` y descuadra las dos
   secciones a la vez.

Y el tercero, que no esta en la fuente sino aqui: **meter los 19 `extern` en
`spchi.h`** --que es lo que tenia el original-- **no se puede hacer hoy**:
`SPCH_Callbacks`, `VoxPendingEvents`, `VOXINGAME` y `VoxBankInfo` estan definidos
LOCALMENTE en `spchevnt.c`, `spchinit.c`, `spchpick.c`, `spchrule.c` y
`spchbank.c`; declararlos en la cabecera obliga a mover los tipos alli y convierte
esas cinco definiciones locales en redefiniciones. Cinco unidades `Matching` en
juego por un cambio cosmetico: **no compensa**.

---

## 8. Lo que este encargo aporta fuera de `spch`

- **La palanca de orden de `.bss`** (sec. 2.2). Es una tercera via, distinta del
  orden de emision de `.text` y del de `.rodata`: en `.bss` manda la cadena de
  DECLARACIONES, y un `extern` en una cabecera incluida antes ADELANTA el
  simbolo. Cualquier unidad con `.bss` descolocada y sin explicacion deberia
  mirarse con esto en la mano. Se prueba en 3 s con el renglon de 2.2.
- **Un DIE de DWARF-1 con direccion puede venir de un `extern`** (sec. 2.3), asi
  que el orden de DIE de una unidad NO es el orden de sus definiciones: es el de
  sus declaraciones, cabeceras incluidas. Vale para leer cualquier volcado de
  `libdwarf.py`.
- **La regla de colocacion de los comodines** (sec. 5.1a), verificada sobre los
  cuatro `auto_*` de esta zona. Convierte en prediccion lo que hasta ahora se
  probaba re-extrayendo.
- **Cirugia de comodines para probar una promocion sin re-extraer.** Partir un
  `auto_*` a mano y enlazar cuesta un minuto y responde la pregunta que hoy exige
  una re-extraccion completa. Es aplicable a las diez fronteras `SIN CEDENTE` de
  r62/p3-C, que son todas de este tipo.

*Sondas borradas. Nada en `scripts/`. No hay commit.*
