# r61 — `zEAXSound` + `zEAXSound2`

Agente `eax`. Unidades en exclusiva: `Speed/Indep/SourceLists/zEAXSound` y `…/zEAXSound2`.

**Ninguna de las dos promociona.** Lo que sí traigo:

1. `zEAXSound2` — **la mitad de DATOS de `GenerateRoadNoise` está CERRADA**: nuestra
   `.rodata` es hoy, palabra por palabra, la del objetivo. Y la mitad de `.text` baja de
   **20 a 17 instrucciones** con dos líneas de fuente.
2. `zEAXSound` — la raya de `splits.txt` **verificada contra los STT_FILE** y, sobre todo,
   la **corrección del encargo**: cerrar el `bss+32` *no* cierra la unidad. Con la sección
   `.bss` cerrada del todo quedan **24.191 B de DOL**, medidos.

Ficheros de fuente tocados: **dos, los dos míos** —
`src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp` (dos líneas de código y el
bloque de comentario) y `src/Speed/Indep/SourceLists/zEAXSound.cpp` (**sólo comentario**;
su `.o` sale con el sha1 idéntico al de antes de tocarlo). Cero cabeceras, cero
`config/`, cero `keep.lst`, cero `splits.txt`, cero `configure.py`.
**Cero correcciones de `lcfix` pendientes**, y no es una suposición: §6.

---

## 0. Estado, antes y después

| | antes | después |
|---|---|---|
| `zEAXSound2` `.o` sha1 | `689cec08e85957023035739a2b66cdc0a5d8235f` | `2a3ee786b45243e8cba971bfc9b0b15dd4556877` (5 compilaciones) |
| `fncmp zEAXSound2` | 1 de 930 — `20 insn, 73 reub` | 1 de 930 — **`17 insn`, 73 reub** |
| `.rodata` de la función | `0,01 0,1 0,15` | **`0,01 0,15 0,1` = la del objetivo** |
| `linkdelta zEAXSound2` | `.text +0  IGUAL` | `.text +0  IGUAL` |
| `trypromo zEAXSound2` | DOL ROTO | DOL ROTO (`ad0401fcc168`) |
| `dolwhere zEAXSound2` | 39.630 B | 39.610 B |
| `zEAXSound` `.o` sha1 | `bca325441ac3803c71e944264ac47d2f803ad79a` | **el mismo** (3 compilaciones) |
| `fncmp zEAXSound` | 0 de 773 | 0 de 773 |
| `linkdelta zEAXSound` | `.text +0  bss+32` | `.text +0  bss+32` |
| `trypromo zEAXSound` | DOL ROTO | DOL ROTO (`4737ec98b206`) |

Control de base: revirtiendo mis dos líneas de `CARSFX_Roadnoise.cpp` el `.o` vuelve
**exactamente** a `689cec08e85957023035739a2b66cdc0a5d8235f`. Mis ediciones son el único
cambio.

---

## 1. `GenerateRoadNoise` — la semilla de pool del decisor F: CONFIRMADA a la primera

Aplicada tal cual, como nueva `:406`:

    ftemp = bLength(this->m_pWheelCtl->m_bvTotalRightWheelSlip) * 0.01f;
    if (0) { ftemp = 0.15f; }

`powerpc-eabi-objdump -s -j .rodata` sobre los dos objetos:

    NUESTRO antes    08f0 43300000 80000000 3c23d70a 3dcccccd
                     0900 3e19999a 46fa0000 3e4ccccd 45bb8000
    NUESTRO después  08f0 43300000 80000000 3c23d70a 3e19999a
                     0900 3dcccccd 46fa0000 3e4ccccd 45bb8000
    OBJETIVO         0a00 43300000 80000000 3c23d70a 3e19999a
                     0a10 3dcccccd 46fa0000 3e4ccccd 45bb8000

`diff` de los dos volcados completos (1.125 líneas): **exactamente esas dos líneas**, cero
desplazamiento del resto de la `.rodata` ni de las cadenas. Sello con la semilla sola:
`fb570764b73d0aa112b2fe22fa4f94ca7cb45b15` — **el mismo que midió el decisor F**.

Y **cae la línea de la r60** que decía que `$LC566`/`$LC567` estaban intercambiados: con la
semilla el emparejamiento es una biyección limpia. La transposición era **una sola pareja**,
la de `0,1f`/`0,15f`, vista dos veces por dos herramientas.

---

## 2. Lo que el decisor F dejaba abierto: las 16 filas de `.text`. Bajan a 17 insn

El defecto que quedaba no era «una rotación»: era que **la constante `0,1f` de `slipBoost`
se cargaba en el BLOQUE EQUIVOCADO** (el de antes del `bso` del clamp derecho; el objetivo
la carga en el bloque de unión). Dos ediciones, y las dos hacen falta:

    if (ftemp > 0.15f) {
        ftemp = 0.15f;
    }
    register float slipBoost asm("fr10") = 0.1f;   /* antes iba DELANTE del if, sin pin */

Métrica **POSICIONAL** (posiciones con codificación distinta, sin resincronizar; los cuatro
casos miden 1240/1240 B):

| variante | posiciones |
|---|---:|
| base + semilla (decl delante del `if`) | 20 |
| decl detrás del `if`, **sin** pin | 51 |
| decl detrás del `if` + `asm("fr10")` | **17** |
| igual con `fr8` / `fr9` | 18 / 20 |

Y en la herramienta honrada, `fncmp`: **`20 insn` → `17 insn`**. El bloque anterior al `bso`
casa ahora **entero** con el objetivo (la primera fila marcada pasa de la 91 a la 95).

**AVISO DE MEDIDA, y es la trampa de `nfsmw-medidas-que-enganan`**: `fndiff` **sube** de 16
a 20 filas marcadas y el fuzzy **baja** de 97,70968 % a 96,80322 %. No es una regresión: su
alineador resincroniza con INSERT/DELETE y cuenta el mismo hueco dos veces. Las tres medidas
que sí son monótonas aquí son `fncmp` (20→17), el conteo posicional (20→17) y la estructura
(el `lis/lfs` del `0,1f` deja de estar en el bloque que no toca).

### Lo que queda, acotado al insn

Quince instrucciones del **mismo** bloque básico, en otro orden, y de ahí cuelga la única
pareja de registros mala (el `3,0f` del `rsqrt`: objetivo `f8`, nosotros `f10`, porque
nuestro `fmadds` de `slipBoost` sale antes y libera `f10`):

    objetivo: lis 0,5 | lis 0,1 | lwz 0x28 | fmaddsA | lfs 0,5 | lfs 0,1 | lfs 0x54 |
              lis eps | lfs 0x50 | lis 3,0 | fmuls | lfs eps | fmadds | lfs 3,0 | fmaddsB
    nuestro:  lwz 0x28 | lis 0,1 | lfs 0,1 | lis eps | lfs 0x54 | fmaddsA | lfs 0x50 |
              lis 0,5 | fmuls | lfs eps | fmadds | lis 3,0 | fmaddsB | lfs 0,5 | lfs 3,0

`schedtrace` sobre la mini-TU lo pone negro sobre blanco (`basic block 8`, sched2):

    ;;  Ready list (t = 1):    420  384  416  377  393
    ;;      --> scheduling insn <<<393>>> on unit lsu     (lwz 0x28)
    ;;      --> scheduling insn <<<377>>> on unit iu2     (high 0,1f)

Mejor = el último. Gana `393`; el objetivo empieza por `420` (`high` del `0,5f`), que es el
**penúltimo** de nuestra lista. O sea: **al objetivo le sobra `INSN_PRIORITY` en la pareja
del `0,5f`**, y `422` (`lfs 0,5`) no tiene ningún dependiente dentro del bloque. Eso NO se
mueve con formas de sentencia — es el nivel 1 de `rank_for_schedule`. Quien lo retome: hay
que darle a `422` un dependiente en el bloque, o quitárselo a `393`.

Controles de pasada: `-fno-schedule-insns` → 281 posiciones y **1228 B**;
`-fno-schedule-insns2` → 69; las dos juntas → 192 y 1232 B. **Las dos pasadas hacen falta**,
y el orden final lo deja sched2 sobre lo que dejó sched1.

---

## 3. Los NEGATIVOS de la ronda (todos a 1240/1240 B, métrica posicional; base 20, mejor 17)

Anotados **junto a la función** para que `previo.py` los encuentre.

| variante | posiciones |
|---|---:|
| decl detrás del `if` SIN pin | 51 |
| `0,1f` como literal, sin variable, **con la semilla puesta** | 51 |
| decl delante de ACC1 / delante de ACC2, con pin `fr10` | 19 / 19 |
| ACC2 detrás del `bLength` izquierdo | 33 (y el `fmadds` se va al bloque SIGUIENTE) |
| ACC2 detrás del `ftemp = ftemp * 0.01f;` | 33 |
| `asm("" : "+f"(fRightVol))` entre ACC1 y ACC2 | 26 |
| el mismo dos veces | 29 |
| el mismo con clobber `"r0"` (barrera de ranura) | 26 |
| `asm("" : "+f"(slipBoost))` delante de ACC2 | 18 |
| `asm("" : "+f"(fLeftVol))` / `("+f"(ftemp))` delante de ACC1 | 19 / 19 |
| `asm("" : : "f"(slipBoost))` delante del `bLength` izq. | **16 en el número y PEOR** |
| lo mismo detrás del `bLength` izq. | 210, y 1244 B |
| `ftemp = ftemp*0.01f` fundido en una sola sentencia | 17 (objeto idéntico) |
| variable propia (`rslip`) para el clamp derecho | 21 |
| pin `fr11` sobre `ftemp` (con y sin mover la decl) | 162 |

El de 16 merece su propia línea: **baja el número y adelanta el `fmaddsB` a la ranura 100**,
o sea que estructuralmente aleja. Es el segundo caso de la ronda en que el recuento con
resincronización miente en el sentido bueno; no me fío de ninguna cifra de alineador que no
venga acompañada del bloque.

### Banco, si alguien retoma la función

* `scratchpad/eax61/h.py` — mini-TU de `CARSFX_Roadnoise.cpp` con los cflags reales de
  `zEAXSound2`, compara **CODIFICACIONES** (no texto) contra `build/GOWE69/asm`. **7 s** por
  variante contra los 25 s de la SourceList; reproduce la función byte a byte (1240/1240 y el
  mismo defecto). `h.positional()` es la métrica sin resincronizar; `h.window(a,b)` imprime
  el tramo lado a lado.
* `scratchpad/eax61/vp.py` — aplica una edición sobre la fuente, mide y **revierte siempre**.

---

## 4. `zEAXSound` — la raya, con la prueba de los STT_FILE

`rangechk.py` sobre las 3.845 entradas locales con STT_FILE del ELF original:

    zEagl4Anim  .bss  locales 0x8045B110-0x8045B118   declarado 0x8045B110-0x8045B268
    zEAXSound   .bss  locales 0x8045B268-0x8045DDF0   declarado 0x8045B268-0x8045DE60

Y el volcado directo de la `.symtab` del ELF en ese tramo dice **de quién es cada byte**:

    8045B110   4 LOCAL   kFloatScaleUp           <- STT_FILE = zEagl4Anim.cpp
    8045B114   4 LOCAL   kFloatScaleDown         <- STT_FILE = zEagl4Anim.cpp
    8045B118  24 GLOBAL  _Q29EAGL4Anim19AttributeDictionary.mReservedAttributeMetaData
    8045B130  20 GLOBAL  _Q25EAGL413DynamicLoader.gSymbolPool
    8045B144  40 GLOBAL  _Q25EAGL413DynamicLoader.gConsPool
    8045B16C  40 GLOBAL  _Q25EAGL413DynamicLoader.gRuntimeAllocConsPool
    8045B194  36 GLOBAL  _Q29EAGL4Anim19ScratchBufferHelper.mScratchBuffers
    8045B1C0  28 GLOBAL  _9EAGL4Anim.qt0                       -> acaba en 0x8045B1DC
       ...  NADA con nombre entre 0x8045B1DC y 0x8045B268 ...   (140 B anónimos)
    8045B268   4 LOCAL   k.30900                 <- STT_FILE = zEAXSound.cpp

O sea: **la raya puede caer en cualquier punto de `[0x8045B1DC, 0x8045B268]` sin contradecir
un solo STT_FILE**, y hoy está en el extremo que no es. El hueco es
`gap_07_8045B1DC_bss` (140 B) y **está en `keep.lst`**, así que sobrevive al estripado.

**Qué valor**: nuestra cabeza de `.bss` mide `0x108` y la del objetivo `0x90` (hasta
`gAudioMemoryManager`): **+120 B exactos**, la misma cifra que la r58 y remedida hoy.
Para que `gAudioMemoryManager` caiga en su `0x8045B2F8` hay que empezar en
`0x8045B2F8 − 0x108 = 0x8045B1F0`. Es el mismo valor de la r58, deducido por otro camino.

### PROPUESTA (líneas exactas, `config/GOWE69/splits.txt`)

    Speed/Indep/SourceLists/zEagl4Anim.cpp:
    	.bss        start:0x8045B110 end:0x8045B1F0        (antes end:0x8045B268)

    Speed/Indep/SourceLists/zEAXSound.cpp:
    	.bss        start:0x8045B1F0 end:0x8045DE60        (antes start:0x8045B268)

Y la otra mitad, que es **mía** y que **no he aplicado** porque suelta empeora: al final de
`src/Speed/Indep/SourceLists/zEAXSound.cpp`, un `asm()` de fichero con
`.section .bss` / `.space 104` / `.previous`.

`checksplits.py` → `SOLAPES: 0`, `RANGOS QUE CORTAN UN SIMBOLO: 0`, `LIMPIO`.
`prefijochk.py` → `LIMPIO`. Las dos unidades son **adyacentes en el orden de enlace de
`.bss`**, así que la imagen no se mueve: sólo cambia quién emite esos 120 B (no es el caso de
`nfsmw-rango-no-basta`, que era un rango huérfano cambiando de sitio de enlace).

---

## 5. Y la corrección al encargo: el `bss+32` **no** es lo único que separa a `zEAXSound`

Se puede medir el paquete completo **sin tocar `splits.txt`**: se simula la raya encogiendo
la `.bss` del objeto **EXTRAÍDO** de `zEagl4Anim` en 120 B (`sh_size` 344→224 y
`gap_07_8045B1DC_bss` 140→20, que es exactamente lo que produciría re-extraer) y se enlaza
sustituyendo además nuestro `zEAXSound` con N bytes de relleno.
Guiones: `scratchpad/eax61/shrink.py` y `scratchpad/eax61/sim.py`.
**Control en cada corrida: el enlace base reproduce el DOL original byte a byte.**

    sin raya   pad   +0   secciones: bss+32     DOL 34.338 B distintos   <-\
    sin raya   pad  +16   secciones: bss+32     DOL 36.229                  |  los tres
    sin raya   pad  +24   secciones: bss+64     DOL 36.578               <-/   de la r58,
    sin raya   pad +104   secciones: bss+128    DOL 39.191                     clavados
    RAYA-120   pad   +0   secciones: bss-96     DOL 36.061
    RAYA-120   pad  +96   secciones: IGUAL      DOL 31.906
    RAYA-120   pad +104   secciones: IGUAL      DOL 24.191   <- mínimo
    RAYA-120   pad +108   secciones: IGUAL      DOL 24.191
    RAYA-120   pad +112   secciones: bss+32     DOL 34.152

**La sección `.bss` cierra**, y aun así quedan **24.191 B de DOL**. Diferencia por sección y
por dueño (`scratchpad/eax61/donde2.py`, sobre los dos ELF enlazados):

    .text     14.275 B    de los cuales 13.292 son de zEAXSound
    .rodata    8.651 B    de los cuales  8.587 son de zEAXSound
    .data      1.259 B    de los cuales  1.237 son de zEAXSound
    .over          6 B

`fncmp` da `0 de 773` porque compara el **OBJETO** y **por nombre de símbolo**; lo que falla
es el **ORDEN de la `.rodata`**. `rodorden zEAXSound`: **277 cadenas del objetivo contra 291
nuestras, y sólo 169 en secuencia (108 fuera)**, con la primera divergencia en el índice 5–6.
Al moverse las cadenas se mueve cada `@ha/@l` que las nombra, y eso es el `.text`.

La raíz es la que ya midió la r58 §4 y **sigue en pie**: `Interfaces/Simables/IVehicle.h`
incluye `pvehicle.h` **antes** que `ISimable.h`, y por eso abrimos
`Generated/AttribSys/Classes/pvehicle.h` como cabecera **161** y `simsurface.h` como la
**202**, mientras el objetivo las tiene en **31** y **13**. **Es una cabecera compartida por
media docena de unidades: me paro aquí y lo propongo, no lo toco** (regla 6, y es
exactamente lo que hundió unidades sin agente en la r56/r57/r58).

**Lectura para el jefe**: la raya de `splits.txt` sigue siendo correcta y hay que aplicarla —
vale 10.147 B de DOL (34.338 → 24.191) y es requisito para todo lo que venga después—, pero
**no promociona `zEAXSound` por sí sola**, y el encargo de esta ronda («lo único que la
separa es `bss+32`») subestimaba el trabajo restante en un factor grande. El siguiente frente
de `zEAXSound` es el ORDEN de la `.rodata`, o sea el orden de parseo de cabeceras, y su
palanca vive en un fichero que no es mío.

---

## 6. `lcfix`: CERO pendientes, y medido

La semilla mueve dos palabras del pool. Pregunta obligada: ¿se ha desplazado algún `$LC`?
Comparadas las tablas de símbolos `$LC` (nombre → sección → offset) del `.o` base y del `.o`
de hoy: **ni uno solo cambia de sitio**.

    $LC570 .rodata 0008F0   $LC571 .rodata 0008F8   $LC572 .rodata 0008FC   $LC573 .rodata 000900

idénticos en los dos. Lo que cambia es el **valor** que hay en `$LC572` (era `0,1f`, ahora
`0,15f`) y en `$LC573` (al revés). **Ninguno de los dos está en `keep.lst`** — los 67 `$LC`
que `keep.lst` guarda de `zEAXSound2.o` son
`58 146…154 158 159 167 219 236…239 266 268…271 304…307 387…390 397 414 416…426 430…433 435 463 484 498…501 577 667 679 794 805 846 1054…1057 1110 1112`.
`keepchk.py` confirma que **ninguna** entrada de `zEAXSound.o` ni de `zEAXSound2.o` sale
`RANCIA` (las 129 rancias del árbol son de `zLua`, `zFe` y compañía, y no son mías).

**Nada de lo que dejo es venenoso para otra unidad**: la numeración `$LC` es por objeto y
`keep.lst` indexa por objeto.

---

## 7. Regresiones

* `fncmp zEAXSound` 0 de 773 **antes y después**; su `.o` sale con el **mismo sha1**
  (`bca325441ac3803c71e944264ac47d2f803ad79a`, tres compilaciones) porque sólo le he añadido
  comentario al final del fichero.
* `fncmp zEAXSound2`: 1 de 930 antes y después, **`20 insn` → `17 insn`**. Las otras 929 no
  se mueven.
* `linkdelta`: `zEAXSound2` `.text +0 IGUAL` (igual que antes); `zEAXSound` `.text +0 bss+32`
  (igual que antes).
* `checksplits.py` LIMPIO, `prefijochk.py` LIMPIO, `keepchk.py` sin rancias mías.
* `mangfix`: no aplica, no he tocado ninguna clase local.
* `gapchk` sale con su lista global de siempre (747), ninguna entrada nueva de mis unidades.
* Sellado: `zEAXSound2.o` = `2a3ee786b45243e8cba971bfc9b0b15dd4556877` en **cinco**
  compilaciones consecutivas; `zEAXSound.o` = `bca325441ac3803c71e944264ac47d2f803ad79a` en
  tres.

## 8. Propuestas para el jefe (nada aplicado)

1. **`splits.txt`**, las dos líneas de §4. Vale 10.147 B de DOL medidos, y es requisito.
2. **Junto con (1)**, y sólo junto con (1): el `asm()` de `.section .bss / .space 104 /
   .previous` al final de `src/Speed/Indep/SourceLists/zEAXSound.cpp`. Suelto deja `bss+128`
   (medido). Lo aplico yo en cuanto la raya esté.
3. **`keep.lst` / `configure.py`**: nada.
4. **Para otra ronda y OTRO agente** (cabecera compartida, no mía):
   `src/Speed/Indep/Src/Interfaces/Simables/IVehicle.h` incluye `pvehicle.h` antes que
   `ISimable.h`. Es lo que descoloca 108 de las 277 cadenas de la `.rodata` de `zEAXSound` y,
   por arrastre, 13.292 B de su `.text`. Hay que censar antes cuántas unidades la incluyen.
