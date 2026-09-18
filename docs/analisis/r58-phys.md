# r58 · `phys` — zGameplay pasa a **IGUAL**: 413.278 → 96.137 B de DOL, un −76,7 %

**Cinco ediciones, ninguna toca una instruccion**, `fncmp` 0/768 antes y despues, y
`linkdelta` de `rodata-136 data-128` a **`.text +0 · IGUAL`**. Es la cuarta unidad del
proyecto con las nueve secciones a `IGUAL` (con zAnim, zMain y zPhysics).

    zGameplay  ec6925d6333c2fafc1acf385028c2acd778acf31   TRES compilaciones seguidas
    zPhysics   b88fe7fed0b96c9a02c24f21a54cc00a61118d50   (sin tocar, fncmp 0/718)
    zMisc      bc161c7db892495429f8163378bb146424fe88d6   (sin tocar, fncmp 0/450)
    trypromo Speed/Indep/SourceLists/zGameplay -> DOL ROTO (07a3a0b86570), 96.137 B
    lcfix.py --check: 122 correcciones pendientes, CERO mias (todas de zLua/zSpeech/zAI)

---

## 0. Lo que de verdad abrio la unidad: **keep.lst ya estaba esperando**

`config/GOWE69/keep.lst` trae `zGameplay.o:lbl_803EBE98`. Nuestro `zGameplay.cpp` escribia
el mismo bloque de 92 B llamandolo **`lbl_803EBB48`** —que es la direccion de
**zGameModes**, no la nuestra—. Como el nombre no casaba, `-strip-unused-data` se llevaba
`92 & ~7 = 88` B y del prefijo solo sobrevivia en el enlace el `"STL"` final.

**La sonda que lo encuentra son cuatro lineas**: cruzar las entradas de `keep.lst` de una
unidad contra la tabla de simbolos de NUESTRO `.o`. Lo que no definimos es deuda
localizada, con nombre y con tamano:

| unidad | entradas de keep.lst que nuestro `.o` NO define |
|---|---|
| zGameplay | `lbl_803EBE98` (92 B) · `lbl_803EC7C4` (84) · `lbl_803ECF64` (19) · `lbl_803ED330` (3) · `lbl_803ED333` (5) · los cinco `gap_06_8041D*_data` (148) · `lbl_8041D3AC` (4) |
| zPhysics | cinco `gap_07_804*_bss` (232 B) — pero ya esta en `IGUAL`, **no tocar** |
| zMisc | ninguna |

De esa lista salieron **tres de las cinco ediciones**. Es una veta de un minuto por unidad
y nadie la habia mirado.

---

## 1. Las cinco ediciones, con su medida

| # | edicion | fichero | linkdelta | DOL |
|---|---|---|---:|---:|
| 0 | (partida) | | `rodata-136 data-128` | 413.278 |
| 1 | `lbl_803EBB48` → `lbl_803EBE98` | `zGameplay.cpp` | `rodata-48 data-128` | 402.199 |
| 2 | los cinco huecos de `.data` + `lbl_8041D3AC` | `GRuntimeInstance/GRaceStatus/GManager.cpp` y `zGameplay.cpp` | `rodata-48` | 374.144 |
| 3 | `lbl_803ED288` de 4 a 16 B **+** `"Lua bytecode decompression buffer"` | `GRaceStatus.cpp`, `zGameplay.cpp` | `rodata-8` | **206.548** |
| 4 | `lbl_803ED330` + `lbl_803ED333` | `zGameplay.cpp` | **IGUAL** | **96.137** |

Desglose final: `.text` 91.320 · `.rodata` 4.342 · `.data` 258 · `803A41B8` 217 ·
`.sdata`/`.sdata2`/`.ctors`/`.dtors`/`.init` **0**.

---

## 2. Los huecos de `.data`: 148 B que keep.lst ya nombraba

El objetivo tiene 712 B de `.data` y nuestro `.o` 564. La diferencia son **seis** trozos,
todos de ceros salvo el ultimo, y `keep.lst` ya trae el nombre exacto de los cinco huecos:

    gap_06_8041D2D0_data   60 B   entre GRuntimeInstance::sRingListHead y GRaceStatus::fObj
    gap_06_8041D310_data    8 B   entre GRaceStatus::fObj y GRaceDatabase::mObj
    gap_06_8041D38C_data    8 B   entre regionTable (GRaceDatabase.cpp) y los dos TWEAK_
    gap_06_8041D3A0_data    4 B   entre GManager::mObj y la estatica local xLeft
    lbl_8041D3AC            4 B   el 220 de xWidth (ver seccion 3)
    gap_06_8041D53C_data   68 B   la cola de la seccion

Un `asm()` de fichero se emite **DONDE ESTA ESCRITO**, asi que cada hueco va entre las dos
definiciones que separa, y eso los coloca al byte. `.data` pasa de `-128` a **IGUAL** de una
sola vez.

**Aviso de medida**: `linkdelta` decia `-128` donde el `.o` decia **148**. La `.data` del
enlace tambien se **alinea a 32 por el final** (267.232 − 152 = 267.080 → ALIGN32 = 267.104
= −128). La cifra exacta es la aritmetica del objeto, no `linkdelta`.

---

## 3. CORRECCION a la r57: en `.data`, un simbolo muerto de 4 B se va ENTERO

La r57 dejo escrito que «`-strip-unused-data` se lleva `size & ~7` de cada simbolo muerto;
de un literal de 4 B eso es CERO». **Eso vale para los literales de `.rodata`, no para
`.data`.** Medido aqui:

    en el .o:   xWidth.25379   .data +0x00A4   sz=4   valor 0x000000DC
    en el ELF:  xWidth.25379   NO EXISTE; line.25380 esta pegado a yTop.25378

La r56 habia anadido `static volatile int xWidth = 220;` y la habia dado por buena mirando
el **objeto**. En el enlace no estuvo nunca. Se arregla sin tocar el codigo con el nombre de
ensamblador que `keep.lst` ya conoce:

    static volatile int xWidth __asm__("lbl_8041D3AC") = 220;

Es `nfsmw-promocion-sin-su-cierre` en pequeno: **una edicion validada contra el `.o` puede
no existir en el enlace**.

Y su reciproca, tambien medida: **los datos ANONIMOS de un `asm()` no se estripan**. El
bloque de 100 cadenas del final de `zGameplay.cpp` (1.577 B, sin un solo simbolo) sobrevive
entero, y por eso el `"Lua bytecode decompression buffer"` de la edicion 3 cuenta. Lo que se
estripa es lo que tiene **nombre** y nadie referencia: `$LC678` (8 B, `43300000 80000000`)
se iba entero, y esos eran los ultimos `-8` de la unidad. Con los nombres `lbl_803ED330` y
`lbl_803ED333` —los de `keep.lst`— se quedan.

---

## 4. La ventana de 32 compra `.data`; solo el tamano EXACTO compra la cola de `.rodata`

Son **dos premios distintos con dos precisiones distintas**, y la r57 solo describia el
primero. Con `rodata-8` —dentro de la ventana— el DOL ya daba:

    .rodata  105.205      <- la cola de .rodata SIGUE desplazada 8 B
    .data        865      <- COBRADO
    .sdata         2      <- COBRADO
    .sdata2        0      <- COBRADO

y con el tamano exacto la `.rodata` cayo a **4.342**. O sea: la ventana de 32 vale
**137.025 B** (todo lo que va detras de `.rodata`) y el byte exacto vale **otros ~101.000**
(la cola de la propia `.rodata` y los inmediatos de `.text` que la apuntan). Una unidad en
`rodata -8` **no esta terminada**, aunque `linkdelta` diga `siz +0`.

---

## 5. Lo que le queda a zGameplay: es ORDEN de FUNCIONES, no de pool

`.text` son 91.320 B, de los que **89.878 caen dentro de su propia ventana**, y **la
distribucion de longitudes NO es la de zPhysics**: 1.198 rangos de 1 B, 1.653 de 2, 2.499 de
3, 1.252 de 7, 931 de 11... Eso no son mitades `@ha`/`@l`: son instrucciones enteras
distintas, o sea **funciones en otro sitio**.

`permorden.py Speed/Indep/SourceLists/zGameplay` lo confirma:

    768 funciones en el objetivo, 767 comunes
    en su sitio 549 de 767 · DESPLAZADAS 218 · 15 ciclos (231, 151, 98, 57, 15, 10, ...)

y los ciclos grandes son **el bloque diferido**: `SetTaskTime__11GRaceStatusf` esta en el
objetivo en el puesto **729** y en el nuestro en el **163**; `SetIsLoading__11GRaceStatusb`
en el 727 contra el 161. Es exactamente la palanca del dossier —**cuerpo EN CLASE = va a la
COLA; cuerpo FUERA = sale en el punto del parseo**— aplicada a `GRaceStatus.hpp` y
`GManager.hpp`. **Es el mayor renglon suelto de mis tres unidades.**

De los 258 B de `.data` que quedan sale ademas un limite conocido: `gap_06_8041D53C_data`
cae **80 B antes** de su sitio porque los `kObjectTemplateKey.271xx` —estaticas locales de
instanciaciones de plantilla— se emiten en `finish_file`, y **un `asm()` de fichero no puede
pasar de ahi**.

---

## 6. zMisc: el `+2840` tiene UNA causa con nombre

El brief decia «zMisc sigue sin merecer la pena». Sigue siendo verdad para esta ronda, pero
ya no es «sin palancas»: **el pool escrito a mano de `zMisc.cpp` y cc1plus emiten LAS MISMAS
cadenas**. Contado por contenido sobre los dos objetos:

    objetivo .rodata 7.864 B · nuestro 11.256 B
    cadenas que tenemos y el objetivo NO: 2.512 B en mas de 40 entradas
    cadenas que el objetivo tiene y nosotros no: 0 B
    ejemplo: "RenderConn::UpdateServices()"  objetivo x1  nuestro x2 (+0x007E4 y +0x01E90)

La copia de `+0x007E4` es la del bloque a mano (`$LC2151632928`, en la posicion del
objetivo) y la de `+0x01E90` es el `$LC437` que cc1plus emite porque **`Main.cpp:299` sigue
teniendo el literal**: `profile_node.Begin("RenderConn::UpdateServices()", 0)`.

La receta ya existe en el arbol —`ATTRIB_TAG(s) → NULL` de `AttribSys.h`— y se generaliza
asi: **poner una etiqueta dentro del pool a mano en el limite de cada cadena y hacer que la
fuente la referencie** (`extern const char lbl_XXXX[];`). El `lis`/`addi` es el mismo; solo
cambia el destino de la reubicacion, y cambia **hacia el correcto**, porque el objetivo
apunta a esa copia.

**Pero no cierra sola**: 2.840 − 2.512 = **328 B** que no son cadenas (flotantes
duplicados), asi que hace falta el barrido completo. Lo que hay detras: `.data` 182.162 +
`.sdata` 2.816 + `.sdata2` 3.420 = **188.398 B**, mas casi toda la `.rodata` (95.889).

---

## 7. Regresiones

**Ninguna.** Los cuatro ficheros tocados —`SourceLists/zGameplay.cpp`,
`Gameplay/GRuntimeInstance.cpp`, `Gameplay/GRaceStatus.cpp` y `Gameplay/GManager.cpp`— los
incluye **solo `zGameplay.cpp`** (`grep` sobre `SourceLists/` y `GameCube/`). **No he tocado
ninguna cabecera** ni `config/GOWE69/*`. zPhysics y zMisc recompiladas al cerrar con el
`sha1` de partida y `fncmp` 0/718 y 0/450.

---

## 8. Propuestas (no las aplico)

1. **`keep.lst`: `zGameplay.o:$LC3` ("bad_alloc") y `:$LC59` ("GAMECUBE") sobran** — el
   prefijo `lbl_803EBE98` ya trae las dos cadenas, asi que ahora las tenemos DUPLICADAS y
   `keep.lst` salva 16 B que el objetivo no tiene. Hay que quitarlas **a la vez** que se
   anaden `lbl_803EC7C4` (84 B) y `lbl_803ECF64` (19 B), que siguen en `keep.lst` sin
   emisor. Paquete atomico: hoy la unidad esta en `IGUAL` y cualquiera de los tres por
   separado la saca de ahi.
2. **La sonda de la seccion 0 merece subir a `scripts/`** (`keepfalta.py`): entradas de
   `keep.lst` que nuestro `.o` no define, por unidad. Ha valido dos de las cinco ediciones
   de esta ronda y se corre en un segundo sobre las 27 unidades.
3. **Repito la propuesta 3 de la r57**: `p8_where.py` (bytes de DOL por seccion, emparejando
   POR INDICE) es la herramienta que ordena la cola de trabajo y sigue en el scratchpad.

---

## 9. Sorpresas

1. **`keep.lst` ya sabia la respuesta.** Dos ediciones de cinco fueron poner el nombre que el
   fichero de configuracion llevaba esperando. La deuda estaba **escrita**, no oculta.
2. **La regla `size & ~7` de la r57 no vale para `.data`**: alli un simbolo muerto de 4 B se
   va entero. Y su reciproca: **lo anonimo no se estripa nunca**.
3. **Una edicion de la r56 validada contra el `.o` no existia en el enlace.** `xWidth = 220`
   llevaba una ronda dando por pagado un hueco que seguia abierto.
4. **La `.data` tambien se alinea a 32 por el final**, asi que `linkdelta` redondea: decia
   `-128` donde faltaban 152.
5. **`rodata -8` no es «casi»**: compra `.data` entera (137.025 B) y no compra ni un byte de
   la cola de `.rodata` (~101.000). Dos premios, dos precisiones.
6. **zGameplay y zPhysics estan rotas por cosas DISTINTAS**, y la distribucion de longitudes
   de los rangos de diferencia de `.text` lo dice antes de mirar una instruccion: zPhysics
   son 3.150 rangos de exactamente 2 B (pool), zGameplay son 11.412 rangos de 1, 2, 3, 7 y
   11 (funciones permutadas).
7. **Un `asm()` de fichero no llega detras de `finish_file`**: la cola de `.data` se queda 80
   B corta de sitio porque las estaticas locales de las instanciaciones de plantilla salen
   despues de cualquier posicion del fuente.
