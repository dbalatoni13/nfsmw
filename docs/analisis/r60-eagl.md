# r60 — `eagl`: el `.bss` de zEagl4Anim estaba MAL con `linkdelta` diciendo IGUAL, y los 4 B de `Initialize` quedan VETADOS con prueba de `stmt.c`

Agente `eagl`. Unidades: `zEagl4Anim`, `zEAXSound`, `zEAXSound2`.

**Cinco ficheros tocados, los cinco MIOS.** Cero cabeceras ajenas, cero `config/`,
cero `splits.txt`, cero `keep.lst`, cero `configure.py`.

    src/Speed/Indep/SourceLists/zEagl4Anim.cpp               (quitado el asm de .bss)
    src/Speed/Indep/Src/EAGL4Anim/DeltaChan.cpp              (los DOS huecos de .bss)
    src/Speed/Indep/Src/EAGL4Anim/RawStateChan.cpp           (solo comentario)
    src/Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.cpp     (solo comentario)
    src/Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp (solo comentario)

---

## 0. Titular

| unidad | linkdelta antes | despues | `dolwhere` | DOL con la unidad sustituida |
|---|---|---|---|---|
| **`zEagl4Anim`** | `text+4` `IGUAL` | `text+4` `IGUAL` | NO COINCIDEN (.text) | **2.961.879 -> 2.833.715** (−128.164) |
| `zEAXSound` | `text+0` `bss+32` | igual | NO COINCIDEN | 34.338 (sin cambio) |
| `zEAXSound2` | `text+0` `IGUAL` | igual | 39.630 B | 24.207 (sin cambio) |

`linkdelta` de `zEagl4Anim` **no se mueve** (era IGUAL y sigue IGUAL) y aun asi la unidad
gana 128.164 bytes de DOL: **la seccion medía bien y las seis direcciones estaban mal**.
Eso es una trampa de medida nueva, y esta en §1.

`fncmp` ANTES y DESPUES, corrido tras CADA fichero: `zEagl4Anim` 2/318 (`Initialize`,
`EvalState`), `zEAXSound` **0/773**, `zEAXSound2` 1/930 (`GenerateRoadNoise`).
**Cero regresiones.**

`trypromo` con la ruta completa, unidad a unidad:

    zEagl4Anim   DOL ROTO 8f3dc0381a23
    zEAXSound    DOL ROTO 4737ec98b206
    zEAXSound2   DOL ROTO d7c5578a8320
    zEAXSound + zEAXSound2   DOL ROTO 72487037a9d4   <- EL MISMO de la r57 y la r58

Ninguna promociona. `lcfix.py zEagl4Anim zEAXSound zEAXSound2 --check`: **todas al dia**.
Global: **185 pendientes, 142 de `zFe` y 43 de `zSpeech`; NINGUNA mia y ninguna toca a mis
unidades** (son entradas `zFe.o:` / `zSpeech.o:`, y `keep.lst` indexa por objeto; el control
`BASE == DOL original` da `True` en todas mis medidas).

---

## 1. La trampa: `linkdelta` IGUAL con las seis direcciones movidas

La r59 cerro el `.bss` de `zEagl4Anim` con

```c
asm(".section .bss\n  .space 128\n.previous\n");
```

al final de `zEagl4Anim.cpp`. El TAMANO salia bien (`linkdelta` IGUAL) y **el sitio salia
mal**: GCC 2.9 drena los globales de fichero en `finish_file`, **detras** de cualquier
`asm()` de ambito de fichero (la nota de la r59 en `EcstasyE.cpp:4907` ya lo decia con
`-S`), asi que el relleno anonimo caia en el **offset 0** de la seccion.

Volcado del `.o` (`objsec.py`, instantaneo, sin enlazar):

| | nuestro ANTES | objetivo extraido |
|---|---|---|
| 0 | **[HUECO 128 B]** | `kFloatScaleUp` / `kFloatScaleDown` |
| 128..324 | los 8 B de locales + los 6 globales | los 6 globales, y **dos huecos** |
| | sin `gap_07_8045B1B8_bss` (8 B) | offset 168 |
| | sin `gap_07_8045B1DC_bss` (140 B) | offset 204 |
| tamano | 324 | **344** |

Y en el ENLACE (comparando la DIRECCION de cada simbolo con nombre, `bssmulti.py`):

    _Q29EAGL4Anim19AttributeDictionary.mReservedAttributeMetaData   +128
    _Q25EAGL413DynamicLoader.gSymbolPool / gConsPool / ...          +128
    _9EAGL4Anim.qt0                                                 +120
    gAudioMemoryManager (ya dentro de zEAXSound)                    +104

**La leccion, en una linea: `linkdelta` compara TAMANOS de seccion; un relleno del tamano
correcto en el sitio incorrecto le sale IGUAL.** El chequeo barato es `objsec.py` sobre los
dos `.o`, que tarda milisegundos y no depende del enlace ni de otros agentes.

### La correccion

Los dos huecos ya estan en `keep.lst` (lineas 188 y 189, `zEagl4Anim.o:gap_07_8045B1B8_bss`
y `zEagl4Anim.o:gap_07_8045B1DC_bss`), asi que van **como variables de C++ con su nombre**
—la receta de la r59— y no como `asm()`:

```c
// DeltaChan.cpp, alrededor de qt0
char gapAnteQt0[8]   asm("gap_07_8045B1B8_bss");
float qt0[7];
char gapTrasQt0[140] asm("gap_07_8045B1DC_bss");
```

Tres detalles que costaron una iteracion cada uno y que valen para la proxima unidad:

1. **El `asm("nombre")` es obligatorio**: dentro de `namespace EAGL4Anim` el simbolo saldria
   como `_9EAGL4Anim.gap_...`, `keep.lst` no lo reconoceria y `-strip-unused-data` se lo
   llevaria (140 & ~7 = 136 B recuperados).
2. **Hay tres colas, no dos.** Medido: los estaticos **DE CLASE** (`mScratchBuffers`,
   `mReservedAttributeMetaData`, `gConsPool`...) salen primero y en un orden propio; detras
   los globales de **namespace** en orden de PARSEO; y los de **ambito global** delante de
   los de namespace. Por eso los dos huecos tienen que ir los dos en `DeltaChan.cpp` y
   dentro del namespace: uno delante de `qt0` y otro detras.
3. Un `.balign` en la seccion sube la alineacion y el enlazador anade 32 B (aviso de la r59).
   Con variables de C++ no hace falta ninguno.

Resultado: **el `.bss` de nuestro `.o` es BYTE A BYTE el del objeto extraido**, simbolo a
simbolo, offset a offset, tamano 344.

---

## 2. Los 140 B son de `zEAXSound`, y la raya de `splits.txt` esta 136 B TARDE

La r58 §0 propuso mover la raya de `.bss` de `0x8045B268` a `0x8045B1F0`. **La direccion
estaba mal y el diagnostico bien.** La prueba es de una linea y no admite discusion:

> el PRIMER simbolo de `.bss` del `zEAXSound` extraido se llama **`_.tmp_17.30901`**.

El contador de `DECL_UID` es **por unidad de traduccion**, asi que `tmp_0..tmp_16`
—**17 pares de 8 B = 136 B**— existieron en esa TU y estan justo delante. No tienen nombre
porque el enlazador borra el simbolo de una estatica muerta pero solo recupera `size & ~7`,
que para 4 B es **cero**: el espacio se queda y el nombre no. Con los 4 B de alineacion a 8
salen los **140** exactos del hueco `0x8045B1DC..0x8045B268`.

Confirmado por dos medidas mas:

* `refscan.py` sobre el ELF original: ese rango tiene **CERO referencias** desde todo
  `.text` (control: el rango de al lado da 11 funciones y el de arriba 8). Es `.bss` muerto.
* nuestro `zEAXSound.o` emite exactamente **17 pares** (`k.21804` .. `_.tmp_17.22484`, 136 B)
  antes del par que en el objetivo es `k.30900`/`_.tmp_17.30901`. La estructura es la misma;
  lo unico que cambia es donde `splits.txt` pone la raya.

**La raya buena es `0x8045B1E0`**, no `0x8045B268` ni `0x8045B1F0`.

### Y aun asi el hueco hay que emitirlo HOY, en `zEagl4Anim`

Porque `trypromo` sustituye **una** unidad contra los objetos extraidos, y los extraidos
parten la imagen sin solaparse: **casar con el extraido es siempre lo correcto**. Medido con
el DOL, sustituyendo solo `zEagl4Anim` y barriendo el tamano del hueco:

| hueco | bytes distintos del DOL | secciones |
|---:|---:|---|
| 0 | 2.835.941 | `text+4 bss-128` |
| 20 (la propuesta de la r58) | 2.835.856 | `text+4 bss-96` |
| **140** | **2.833.715** | `text+4` |

### Lo que le toca al jefe, con la cifra

    splits.txt:  zEagl4Anim .bss end   0x8045B268 -> 0x8045B1E0
                 zEAXSound  .bss start 0x8045B268 -> 0x8045B1E0
    keep.lst:    gap_07_8045B1DC_bss pasa de zEagl4Anim.o a zEAXSound.o
    y entonces:  en DeltaChan.cpp, `char gapTrasQt0[140]` -> se BORRA (una linea)

**No es un adorno: sin esa raya `zEAXSound` no puede llegar nunca a `.bss` IGUAL.** Sus 17
pares muertos de cabeza son esos mismos 136 B, y sus CADENAS si estan dentro de su ventana
de `.rodata` (la r58 §3 las localizo una a una) — o sea que desde la fuente hay que emitir
las dos cosas o ninguna, y con la raya actual haria falta emitir la cadena y **no** la
estatica. Hoy la unidad marca `bss+32` porque la cabeza le sobra 112 B y la cola le falta
otros tanto y casi se cancelan; anadir solo los huecos de cola (`gap_07_8045DDC8_bss` 4 B y
`gap_07_8045DDF4_bss` 108 B, los dos ya en `keep.lst`) la dejaria en `bss+128`. **Es un
paquete atomico y la mitad que falta es del jefe.** Por eso no he tocado `zEAXSound`.

---

## 3. `Initialize` (4 B): VETADA, y la causa es `stmt.c`, no `jump.c`

Los 4 B de `.text` de `zEagl4Anim` son **una sola instruccion** del arbol del `switch`, y
valen **~2,9 MB de DOL** (todo lo que va detras se desplaza 4 B).

La r47 dejo identificada la condicion que falla en `jump.c:1831`
(`JUMP_LABEL(range1end) == label2`). Esta ronda dice **por que** falla, y con eso el frente
queda cerrado con prueba en vez de con barrido.

El objetivo emite la hoja del nodo `{2}` (`case SHT_SYMTAB`) asi:

    cmplwi r0, 1 ; ble <default>     <- `LT 2 -> default`  (LTU C se canonicaliza a LEU C-1)
    <CAIDA al cuerpo de SHT_SYMTAB>

y en `emit_case_nodes` (`stmt.c:5875`) esa forma sale de **un solo sitio**: la rama de
**RANGO** (`node->low != node->high`), que termina en `emit_jump (node->code_label)`. Ese
`emit_jump` es justo lo que convierte `range1end` en un salto simple a `label2` y dispara la
permutacion de `jump.c`; despues `jump.c` se lo come por ser salto al siguiente. **Una sola
causa explica las DOS diferencias** (el `ble` en vez del `bgt`, y la instruccion de mas).

El nuestro es de **valor unico**: `add_case_node` hace `r->high = r->low` cuando los limites
coinciden, asi que emite `do_jump_if_equal` + el `b default` de `emit_jump_if_reachable`:
**3 instrucciones contra 2**.

Para que `{2}` sea rango hace falta `low == 2` (lo fija el `cmplwi 1`) y `high > 2` con
`node_has_high_bound`, o sea un **antecesor con `low == high+1`**. Los antecesores de `{2}`
son `{3}` (low 3) y la raiz `{8}` (low 8):

* `high = 2` -> seria valor unico otra vez. **Imposible.**
* `high = 7` -> se solapa con `{3}` y con `[4,7]`; `add_case_node` da error. **Imposible.**

La otra forma que da el mismo codigo es que `{2}` cuelgue de un **rango acabado en 1**
(arbol `{3}` -> izq `[0,1]` -> der `{2}`): entonces `[0,1]` emite `LE 1 -> su cuerpo` y `{2}`
es `node_is_bounded`, o sea `emit_jump` que `jump.c` borra. **Pero `balance_case_nodes` no
construye ese arbol**: con la lista `[0,1],{2},{3},[4,7]` sale
`i = (4 nodos + 2 rangos + 1)/2 = 3` y parte en `{2}`, no en `{3}`. Para partir en `{3}`
harian falta **tres** rangos entre esos cuatro nodos, y `{2}` y `{3}` no pueden serlo (no
quedan valores libres). Y no vale meter nodos: el hijo **derecho** de `{3}` tiene que ser
`[4,7]` **solo**, porque es el `node_is_bounded` que le da el `bgt` directo.

**Barrido de control: 161 combinaciones** de `case` anadidos *semanticamente neutros*
(`SHT_NULL` / `SHT_PROGBITS` / `[0,1]` cruzados con `SHT_SHLIB` / `SHT_DYNSYM` / `[10,11]` /
`SHT_LOPROC+k` / `[12, LOPROC-1]`, con `break` y con `continue`). **Ninguna** baja de 28
filas ni de 2.356 B, y las que restauran el pivote `{8}` vuelven a **28 exactas** — que es
como he comprobado la aritmetica de `balance_case_nodes`.

Dos medidas que valen para el que vuelva:

| variante | tamano | filas |
|---|---:|---:|
| base | 2.356 | 28 |
| `case SHT_PROGBITS ... SHT_SYMTAB:` (nodo `[1,2]`) | **2.352 EXACTO** | **15** |
| `case SHT_NULL ... SHT_SYMTAB:` (`low == TYPE_MIN`) | 2.344 | 16 |

La del medio deja el arbol entero casando **salvo una instruccion** (`blt` contra `ble`,
porque `low` vale 1 y no 2) y da el `.text` al byte. **NO APLICADA**: cambia la semantica
(un `SHT_PROGBITS` entraria por el cuerpo de `SHT_SYMTAB`) y aun asi no casa, asi que seria
un `.text +0` falso — de los que la nota «trabajo a medias regresa» describe.

Todo esto queda escrito en `eagl4supportdlopen.cpp`, donde `previo.py` lo encuentra.
Aunque el `switch` se resolviera, a `Initialize` **le quedan 14 filas** de reparto y
asociatividad que la r47 ya documenta ahi mismo.

---

## 4. `EvalState` (18 filas): las tres palancas nuevas, todas negativas

La r48/r49/r50 barrieron formas de fuente y banderas. El **pin de registro**, la **barrera
selectiva** y la **cantidad fantasma** son posteriores; ninguna estaba probada aqui.

| palanca | filas | tamano |
|---|---:|---:|
| base | **18** | 456 |
| `register unsigned char *k asm("r4")` | 18 | 456 — **OBJETO IDENTICO** |
| cantidad fantasma en `r12` (dos `asm` no volatiles) | 18 | 456 — **OBJETO IDENTICO** |
| `register int i asm("r31")` | 50 | 464 |
| barrera `asm("" : "+r"(keyData))` tras el `if` | 43 | 460 |
| barrera sobre la BASE antes del `if` | 64 | 408 |
| barrera solo en la rama IMPAR / PAR | 43 | 464 / 456 |
| pin de `k` + barrera impar; fantasma + barrera impar | 43 | 464 |

El pin de `k` y la cantidad fantasma dan el `.o` **bit a bit igual que la base**: no es que
empeoren, es que GCC 2.9 no las ve aqui (`k` ya esta en `r4`, y el rango de vida de cero
bytes no llega a bloquear `r12` en `find_reg`).

Y **siete formas mas de `GetKeyData`** en la cabecera —asignar la base antes del `if`,
`if`/`else` con temporal, ternario del desplazamiento, `unsigned short *d`, `return` doble,
`+= 2` con las dos polaridades—: **todas peores (69 a 97 filas) y todas mas CORTAS**
(404-424 B). Eso cierra una duda que quedaba abierta: **el objetivo mide 456 B igual que
nosotros, luego NO calcula la base una sola vez en la fuente**; el `mr r4,r11` se lo pone el
optimizador sobre nuestra misma fuente, y sigue en pie el diagnostico de la r50 (`loop.c` no
puede mover la insn 170 porque `reg147` tiene dos asignaciones dentro del bucle).

---

## 5. `GenerateRoadNoise` (16 filas): el eje queda ACOTADO

Con `function_reloc_diffs=none` quedan **16 filas** y todas cuelgan de lo mismo: subimos la
carga del `0,15f` del segundo clamp **por encima del `bso`** del `sqrt` del segundo
`bLength` (filas 91 y 94, `lis $LC572@ha` + `lfs $LC572@l`, y la 99, `lwz r11,0x28(r31)`), y
el objetivo las tiene **detras**, agrupadas en 101-105 con las otras dos del pool. Los tres
`INSERT` y los tres `DELETE` se compensan: mismo tamano, mismo numero de instrucciones,
**otro bloque basico**. No es rotacion de FPR: es un adelanto especulativo.

Emparejamiento del pool, medido (util para quien ataque el orden):

    $LC566 <-> lbl_803D9A40      $LC567 <-> lbl_803D9A3C
    $LC568 <-> lbl_803D9A44      $LC572 <-> lbl_803D9A60

o sea que **566 y 567 estan intercambiados**: el objetivo crea primero el de `803D9A3C`.

Probado y negativo (base 16 filas / 1.240 B):

    barrera selectiva `asm("" : "+f"(ftemp))` tras el bLength ... 23 filas / 1.244 B
    `float clampSlip = 0.15f;` local delante del if ............. 16 (OBJETO IDENTICO)
    `ftemp = ftemp > 0.15f ? 0.15f : ftemp;` .................... 16 (OBJETO IDENTICO)

**La barrera selectiva no sirve aqui aunque el sintoma sea un adelanto de `sched`**: lo que
se adelanta es una carga de CONSTANTE, que no depende de nada que la barrera pueda nombrar.
La palanca que queda sin probar es la **semilla de pool** de la r59
(`if (0) { x = ...; x = ...; }`), que desacopla el orden del pool del de DEF y es justo lo
que separa 566 de 567. Escrito en `CARSFX_Roadnoise.cpp`.

---

## 6. Higiene: dos ficheros mios estaban en latin-1

`eagl4supportdlopen.cpp` y `CARSFX_Roadnoise.cpp` tenian `0xAB`/`0xBB` (`«»`) en comentarios
de la r47. Es exactamente la familia del `§` que reventaba `configure.py` entero
(commit `7ba4d59e`). Sustituidos por comillas ASCII; **los dos ficheros son ya UTF-8 valido**
y el `.o` sale con el mismo aporte al DOL (`trypromo` del par `zEAXSound + zEAXSound2` sigue
dando `72487037a9d4`, el hash de la r57 y la r58).

---

## 7. Sellos

    zEagl4Anim.o   3d9a8749e8ad (inicio)  ->  ef9c883af5db (fin), TRES compilaciones iguales
    zEAXSound.o    ca5b0acf27d9 (inicio)  ->  bca325441ac3 (fin), TRES compilaciones iguales
    zEAXSound2.o   cec2c6a3ea35 (inicio)  ->  689cec08e859 (fin), TRES compilaciones iguales

El `sha1` de `zEAXSound.o` cambia **sin que yo toque nada suyo** (mismo fenomeno que la r57
§8 y la r58 §7: otro agente edito una cabecera compartida a mitad de ronda). La prueba de
que su aporte es identico es el hash de `trypromo` del par: `72487037a9d4`, el mismo con que
cerraron la r57 y la r58. Y los comentarios que he escrito son neutros: `trypromo` de
`zEagl4Anim` da `8f3dc0381a23` **antes y despues** de escribirlos (el `sha1` del `.o` si
cambia, porque el arbol lleva `-gdwarf+`).

**Ninguna de mis medidas depende de que se aplique `lcfix`**: mis tres unidades estan al dia
y las 185 pendientes son de `zFe` y `zSpeech`.

---

## 8. Herramientas (todas en el scratchpad, ninguna en `scripts/`)

| script | que hace |
|---|---|
| `objsec.py <o> <sec>` | los simbolos de una seccion de un `.o` por OFFSET, **con los huecos**. Es el chequeo que le falta a `linkdelta`: milisegundos y sin enlazar. |
| `bssmulti.py <sec> <u>…` | compara la DIRECCION de cada simbolo con nombre entre el enlace base y el enlace con VARIAS unidades sustituidas, e imprime los saltos de delta. |
| `dolmulti.py <u>…` | bytes distintos del DOL con varias unidades sustituidas a la vez (el `doldelta` de la r58 solo admite una). |
| `micro.py <sim> [-v] [cpp]` | compila **un solo `.cpp`** con los cflags de la unidad y lo diffea contra el objeto extraido: 2 s por iteracion en vez de 4-28 s. Control: reproduce las 18 filas de `EvalState` y las 28 de `Initialize` exactas. |
| `symrange.py <lo> <hi>` | los simbolos del ELF original en un rango, **con el STT_FILE de cada local**. |
| `refscan.py <lo> <hi>` | quien REFERENCIA un rango desde `.text`, reconstruyendo `lis`+`addi/lwz/...`. Es la prueba de propiedad que los STT_FILE no dan para un hueco anonimo. |
| `sweep_cases.py`, `sweep_sw.py`, `sweep_gkd.py`, `sweep_eval.py`, `sweep_road.py`, `sweep_gap.py`, `tryone.py` | los barridos de §3, §4, §5 y §2, con restauracion garantizada del arbol. |

---

## 9. Lo siguiente, por valor

1. **La raya de `.bss` a `0x8045B1E0`** (§2). Desbloquea `zEAXSound` —que no tiene ni una
   funcion pendiente, `fncmp` 0/773— y cuesta dos lineas de `splits.txt`, una de `keep.lst`
   y borrar una linea de `DeltaChan.cpp`. Detras quedan sus dos huecos de cola y el orden
   de su `.bss`, que ya son mios.
2. **La semilla de pool en `GenerateRoadNoise`** (§5): es la unica palanca sin barrer de las
   16 filas, y el emparejamiento del pool ya esta hecho.
3. `zEagl4Anim` **no promociona esta serie**: `Initialize` esta vetada con prueba y a
   `EvalState` le quedan 18 filas con seis rondas de negativos. Conviene sacarla de la lista
   de «a dos funciones» del dossier.
