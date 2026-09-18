# r57 — `zEAXSound` + `zEAXSound2`: −380.819 B de DOL, `zEAXSound2` a `IGUAL`, y la causa era una duplicación entre `keep.lst` y el fuente

Agente `eax`. Unidades: `Speed/Indep/SourceLists/zEAXSound` y `…/zEAXSound2`.
Ficheros tocados: **los dos `.cpp` de SourceList y nada más**. Cero cabeceras (ni mías ni
ajenas), cero `config/`, cero `keep.lst`, cero `splits.txt`, cero `configure.py`.
**Cero correcciones de `lcfix` pendientes por mi culpa** (§7).

    DOL, bytes distintos con las dos unidades sustituidas:  443.067 → 62.248   (-380.819)
    zEAXSound   .text +0   .rodata 0   .data 0   .bss +32
    zEAXSound2  IGUAL en las nueve secciones

---

## 0. El encargo partía de una medida caducada, y eso era el hallazgo

El encargo decía «le faltan 136 B de `.rodata` y 416 B de `.data`». Al medir, la r57 empezaba
en el otro signo:

| | al empezar la r57 | al cerrar la r56 (informe) |
|---|---|---|
| `zEAXSound` `linkdelta` | `.text +0  **rodata+568**  bss+32` | `.text +0  bss+32` |
| `zEAXSound2` `linkdelta` | `.text +0  **rodata+448**` | `IGUAL` |
| DOL bytes distintos (las dos) | **443.067** | 62.462 |

(La cifra de cierre es **62.248**: 214 B por debajo del mejor de la r56, ademas de deshacer
el desplazamiento de 380.819 B.)

El `.o` daba el **mismo `sha1`** que dejó la r56 (`694f17bb…` / `d66c11df…`), así que no era
el fuente: era el **enlace**. Y `lcfix.py --check` decía «todas las entradas al día», así que
tampoco era `keep.lst` rancio.

**Los 568 y los 448 B son exactamente las cifras que la r56 dio por «reaparecen más abajo, en
las bibliotecas»** (§2 de `r56-eax.md`). No reaparecían más abajo: estaban **dentro de mi
ventana**, y lo dice una medida por objeto que antes no existía:

    python scripts/agent_eax57_ventanas.py .rodata zEAXSound
      objeto                                          base    promo   delta
      build/GOWE69/obj/…/zEAXSound.o                 12824    13392    +568
      suma de deltas de ventana: +568          <- ni un byte fuera

La `.rodata` había crecido +1.016 B entre las dos unidades, y **un cambio de tamaño de
sección desplaza todo lo que va detrás en el DOL**: de ahí 443.067 bytes distintos donde la
r56 medía 62.462. Es la trampa de medida de la ronda: *el número de bytes distintos del DOL
no es comparable entre rondas si el tamaño de una sección cambió*.

---

## 1. La causa: **`keep.lst` y el bloque `asm()` resuelven el MISMO problema, y estaban los dos puestos**

Hay **dos** maneras de que una cadena muerta del objetivo sobreviva en nuestro enlace:

1. **`keep.lst`** con una entrada `zEAXSound.o:$LCnnn` y su comentario `# @lc zEAXSound "…"`:
   cc1plus emite el `$LC` y `-strip-unused-data` no se lo lleva porque está en la lista;
2. un **`asm()` de fichero** con `.asciz "…"` anónimo, que nadie estripa nunca.

La r52 y la r56 escribieron 98 cadenas a mano en `zEAXSound.cpp` y 74 en `zEAXSound2.cpp`.
`keep.lst` lleva 66 entradas `@lc` de `zEAXSound` y 67 de `zEAXSound2`. **La intersección son
31 cadenas (656 B) y 27 cadenas (517 B): cada una salía DOS VECES en el enlace.**

    python scripts/agent_eax57_dup.py
      zEAXSound    a mano 98   keep.lst @lc 66   DUPLICADAS 31, 656 B
      zEAXSound2   a mano 74   keep.lst @lc 67   DUPLICADAS 27, 517 B

Y la contabilidad de la ventana enlazada, cadena a cadena, lo confirmaba sin margen:

    python scripts/agent_eax57_cad.py zEAXSound
      SOBRA en PROMO (38 cadenas, 721 B) ... FALTA en PROMO (0 cadenas, 0 B)

Las 38 son las 31 duplicadas, la copia de más de `EAXAemsManager::m_pAsyncBuff` (29 B, la que
la propia r56 dejó anotada como «hoy duplicada de verdad»), `GAMECUBE` (§6) y cinco
**colillas** de `size & ~7` (`uction`, `stems`, `ssis`, `done`, `Cop`, 27 B), que no se pueden
quitar sin dejar de emitir el `$LC` muerto.

**Quitar el duplicado del FUENTE y no de `keep.lst` es lo correcto por dos motivos**: no toca
un fichero del enlace mientras hay seis agentes midiendo, y deja viva la copia que está en el
sitio del pool (la del `$LC`), no la del final del SourceList.

---

## 2. Lo que quedaba después: contenido NO-cadena que nadie había contado

Quitados los duplicados, la ventana no cerraba en 0: se pasaba de largo.

    zEAXSound   +568 → −120     zEAXSound2   +448 → −72

Porque la ventana tiene dos poblaciones y sólo se había contado una. Restando las cadenas de
las dos ventanas y **filtrando las entradas de tabla de saltos** (`0x800C….`, que son ORDEN y
no contenido: aparecen en las dos, permutadas) queda una lista corta y accionable:

    python scripts/agent_eax57_nocad.py zEAXSound
      PALABRAS que FALTAN en promo (20, 80 B):
         x11  00000000     x3  3F800000 (1,0f)   x2  43300000   x2  80000000
         x1   46FFFE00 (32767,0f)                x1  44000000 (512,0f)
      PALABRAS que SOBRAN en promo (4, 16 B)   <- colillas de cadena mal partidas

`0x43300000`/`0x80000000` es la constante de conversión `int → double` de GCC 2.9. Son
**constantes de pool que el objetivo tiene y nuestro árbol no referencia**, así que cc1plus no
las emite. Se escriben igual que los blobs de `.data` de la r56: un `asm()` con `.4byte`.

En `zEAXSound2` la lista es 19 palabras / 76 B (dos `0,5f`, dos parejas `43300000/80000000`,
dos `1000,0f`, `1,0f`, `2,0f`, `70,0f`, `18,8f` y siete ceros).

---

## 3. Las cuatro ediciones, con su cifra

Todas son `asm()` de fichero. **Ni una entrada nueva ni retirada en `keep.lst`, y ningún
`$LC` renumerado** (quitar líneas `.asciz` de un `asm()` no mueve el pool de cc1plus).

| # | edición | fichero | ventana `.rodata` |
|---|---|---|---|
| 1 | fuera las **31 cadenas duplicadas** con `keep.lst` **+ la copia sobrante de `EAXAemsManager::m_pAsyncBuff`** (685 B) | `zEAXSound.cpp` | **+568 → −120** |
| 2 | **20 palabras NO-cadena** que le faltan (80 B) | `zEAXSound.cpp` | **−120 → −40** |
| 3 | **40 B más de ceros**: el histograma de bytes de la ventana da 75 ceros de MENOS contra ninguno de más | `zEAXSound.cpp` | **−40 → 0** |
| 4 | fuera las **27 cadenas duplicadas** (517 B) **+ 18 palabras NO-cadena** (72 B) | `zEAXSound2.cpp` | **+448 → −72 → 0** |

Resultado por sección, con `linkdelta`:

    Speed/Indep/SourceLists/zEAXSound2   +0   IGUAL
    Speed/Indep/SourceLists/zEAXSound    +0   bss+32

`fncmp` **idéntico antes y después, y comprobado tras CADA fichero**: `zEAXSound` 0 de 773,
`zEAXSound2` 1 de 930 (`GenerateRoadNoise`, la de siempre).

`sha1` del `.o`, tres compilaciones seguidas iguales:
`f06ad54f4188da714e1ea66703ec7286d2bf1bd4` y `40f74ccebef053a152af7ebb41c51335725656ef`.
`trypromo` con la ruta completa: **DOL ROTO (`72487037a9d4`)** — no promociona, y lo digo
claro.

### Un aviso de higiene que costó una recompilación

El bloque nuevo de `zEAXSound2.cpp` salió con **saltos de línea de verdad dentro de los
literales** en vez de `\n` escapado (un heredoc que se comió la barra). **Compiló igual y dio
el mismo `.o`** —cc1plus 2.95 acepta la cadena multilínea—, pero está arreglado. Y el
`.cpp` de `zEAXSound2` es **CRLF** mientras el de `zEAXSound` es **LF**: las dos ediciones se
hicieron en bytes para no convertir el fichero entero y ensuciar el diff.

---

## 4. Dónde están ahora los bytes distintos

`python scripts/agent_eax57_atrib.py` reparte 62.237 de los 62.248 por ventana y sección
(los 11 que faltan son cabecera del DOL, que ese reparto no cubre):

| | bytes |
|---|---:|
| `zEAXSound` `.text` | 13.702 |
| `zEAXSound2` `.rodata` | 10.756 |
| `zEAXSound` `.rodata` | 8.583 |
| `zEAXSound2` `.text` | 7.825 |
| `zEAXSound2` `.data` | 5.702 |
| `zEAXSound` `.data` | 1.233 |
| **fuera de mis ventanas** | 14.436 |

**Ya no queda un solo byte de TAMAÑO: todo lo que queda es ORDEN DE EMISIÓN.** Y el perfil
dice dónde empieza:

    python scripts/agent_eax57_perfil.py zEAXSound .text
      ventana 800A6128  base 151092  promo 151092  distintos 13513
      tramos IGUALES de >=32 B: 919, suman 122890 B

    python scripts/agent_eax57_perfil.py zEAXSound .rodata
      primer byte distinto: +0x60      tramos IGUALES de >=32 B: 1 (los 96 B del prefijo)

O sea: **el `.text` está colocado en un 81 %** y sus 13.513 bytes distintos son en su mayoría
campos de reubicación `lis/addi` apuntando a `.rodata`; **la `.rodata` diverge en el byte
0x60 y ya no vuelve**. Arreglar el orden de la `.rodata` se lleva por delante buena parte del
`.text` y de los 14.436 de fuera.

Y el primer escalón está a la vista, cadena a cadena:

    OBJETIVO  …Blob, simsurface, TAttrib, ecar, camerainfo, effects, audioimpact, audioscrape…
    NUESTRO   GAMECUBE(dup), Audio Memory Pool, …Blob, pvehicle, TAttrib, simsurface, engine…

Dos bloques de basura de 32 B por delante (§6) y luego un orden distinto de los
`Attrib::Gen::*`, que es orden de **primer uso en el parseo**. Es el frente de
`nfsmw-orden-de-emision`, no un frente de datos.

---

## 5. `GenerateRoadNoise`: mirada y NO tocada, con la razón

`previo.py` da cuatro bloques (r27/r29/r46/r47/r49/r50) y un `UNSOLVED` explícito. La medida
de hoy coincide con el diagnóstico viejo y **no aporta razón nueva**: las 91 primeras
instrucciones casan al byte y la divergencia arranca en `+0x16C` con
`EFBFFAFA`→`EFBDEABA` (rotación de FPR) y `C00A0054`→`C00B0054` (otro registro base). Es
reparto y planificador, exactamente lo que la r47 y la r50 barrieron. **No la he tocado: era
ronda de datos y volver a atacarla sin palanca nueva es repetir cuatro negativos.**

---

## 6. Dos propuestas para el jefe, las dos MEDIDAS

### 6.1 Tres entradas de `keep.lst` que mantienen viva una cadena que el prefijo ya trae

`deadstr.py` metió `GAMECUBE` y `bad_alloc` como `$LC` a mantener, pero **esas cadenas ya
están dentro del blob de prefijo de bWare** (`lbl_803D5E38`, 92 B, el que empieza por
`GAMECUBE`). Por eso salen dos veces en la ventana enlazada:

    # @lc zEAXSound  "GAMECUBE"    ->  zEAXSound.o:$LC61
    # @lc zEAXSound2 "bad_alloc"   ->  zEAXSound2.o:$LC58
    # @lc zEAXSound2 "GAMECUBE"    ->  zEAXSound2.o:$LC146

Medido con una **copia privada** de `keep.lst` en el scratchpad (control `BASE == DOL
original` = `True`, y no toqué el fichero compartido):

    con las tres fuera:  zEAXSound .rodata −8 ,  zEAXSound2 .rodata −16

Así que **la edición es atómica**: quitar las tres entradas **y** añadir 8 B en
`zEAXSound.cpp` y 16 B en `zEAXSound2.cpp`, o el DOL se desplaza otra vez. Su valor real no
es el tamaño (ya está a 0) sino que **adelanta nuestro pool 12 B y alinea las diez primeras
cadenas de la ventana con las del objetivo**.

### 6.2 DUEÑO: tres `_GetKind` que definimos y el objetivo no

    _GetKind__10MMiscSound          objetivo: UNDEF en zEAXSound y en zEAXSound2
    _GetKind__15MGamePlayMoment     objetivo: UNDEF en las dos
    _GetKind__18MControlPathfinder  objetivo: UNDEF en las dos

y en el ELF original **sólo un objeto define cada uno**: `zCamera.o` los dos primeros y
`zAI.o` el tercero. Nosotros los definimos **en las dos unidades**. Son
`static UCrc32 _GetKind() { static UCrc32 k("…"); return k; }` con el cuerpo **en clase** en
`src/Speed/Indep/Src/Generated/Messages/M*.h`: al llevar una estática local no se pliegan, y
GCC 2.9 emite el cuerpo en cada TU que las usa.

El coste medido es **24 B de los 32 que le sobran a `.bss` en `zEAXSound`** (dos estáticas de
4 B por función: `k.N` y `_.tmp_N`, y **una estática muerta de 4 B no se estripa jamás**,
`4 & ~7 == 0`). El resto de la familia (`_GetKind__12MAIEngineRev`, los cuatro de
`SoundConn.h`) **sí** está en el objetivo, así que el arreglo es **por clase, no global**.

**No lo he hecho**: `Generated/Messages/*.h` no es mío y lo incluyen decenas de unidades; la
regla 5 obliga a medirlas todas y eso es trabajo de ventana, no de agente. Queda con su
prueba y su cifra.

`.bss` no mueve el DOL (no va en él), así que esos 32 B valen ~0 bytes hoy: sólo impiden que
`zEAXSound` ponga `IGUAL`.

---

## 7. Herramientas nuevas (todas con control `BASE == DOL original` impreso)

| script | qué mide |
|---|---|
| `agent_eax57_ventanas.py <sec> <u>` | **ventana de CADA objeto del enlace**, base contra promo. Es la que dice si el crecimiento está dentro de mi unidad o aguas abajo — la pregunta que la r56 respondió mal. |
| `agent_eax57_cad.py <u> [sec]` | contabilidad de cadenas de la ventana enlazada: SOBRA / FALTA, y el reparto cadena / no-cadena. |
| `agent_eax57_nocad.py <u>` | lo mismo para lo que **no** es cadena, filtrando las tablas de saltos (`--todo` para no filtrar). |
| `agent_eax57_dup.py` | qué `.asciz` del SourceList duplican una entrada `@lc` de `keep.lst`. |
| `agent_eax57_quita.py <u> [--extra X] [--salvo Y] [--aplicar]` | las quita, en bytes, respetando CRLF/LF. |
| `agent_eax57_perfil.py <u> <sec>` | dónde empieza a divergir la ventana y qué tramos siguen iguales. |
| `agent_eax57_hist.py <u> <sec>` | histograma de bytes de la ventana (fue lo que identificó los 40 B de ceros). |
| `agent_eax57_atrib.py` | reparte los bytes distintos del DOL entre mis ventanas y «fuera». |
| `agent_eax57_duenos.py` | quién referencia cada estática de 4 B de `.bss`, y si su función existe en el objetivo. |
| `agent_eax57_probe.py` | la sonda general (secciones, ventanas, DOL). |

**`lcfix.py --check` deja 83 correcciones pendientes: 70 de `zSpeech` y 13 de `zAI`. NINGUNA
es mía** — mis ediciones no renumeran ningún `$LC`. `checksplits` y `prefijochk` siguen
`LIMPIO`.

---

## 8. Regresiones y propiedad

**Ninguna.** Los dos únicos ficheros tocados son `src/Speed/Indep/SourceLists/zEAXSound.cpp`
y `…/zEAXSound2.cpp`, que **no los incluye ninguna otra unidad**: no hay unidad ajena que
medir (regla 5 no aplica).

Al revés sí pasó, y conviene anotarlo: **el `sha1` de mis dos `.o` cambió a mitad de ronda sin
que yo tocara sus fuentes** (`e4d5dc9b…` → `f06ad54f…`), porque otros agentes editaron
cabeceras compartidas (`ICause.h`, `SoundAI.h`, …). `linkdelta` y `fncmp` salieron idénticos
después, así que no hubo daño; pero **las cifras de este informe son sobre el árbol de este
momento**, y la línea base de una unidad caduca en cuanto un vecino toca una cabecera.
