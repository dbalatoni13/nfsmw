# Ronda 31 — ub: zSpeech / zFe / zTrack (las seis funciones que faltan)

**CERO funciones cerradas, y el hallazgo que manda sobre el encargo: NINGUNA de
las tres unidades puede promocionar esta ronda aunque cierre las seis funciones.
Medido con el enlazador delante: `zSpeech` y `zFe` NI SIQUIERA ENLAZAN (318 y 12
símbolos indefinidos) y `zTrack`, que sí enlaza, sale con el DOL corto **−2.464 B
en `.data2`/`.data3`**. Los 416.832 B del encargo están detrás de los frentes de
`lk`/`und`, no detrás de mis seis funciones.**

Lo que sí traigo: **49 ensayos numerados**, dos funciones llevadas al borde
(`UIMemcardKeyboard` 92,98 → **99,4186 %** y `FoundEntry` 97,97 → **98,4375 %**,
las dos con barrera declarada y **revertidas** porque no cierran), **una veda
confirmada con prueba nueva** (`Setup`), **el hueco de pila de `GetLoadingPriority`
localizado al byte** en el DWARF, y **una veda nueva y general**: el orden de
carga de los ARGUMENTOS de una llamada es un empate de `INSN_UID` en
`rank_for_schedule` y no se toca desde el fuente — bloquea las dos funciones que
más cerca se quedaron.

**Ficheros del árbol tocados: NINGUNO.** Todo medido con parche temporal +
restauración. Sin commits.

---

## 0. Verificación del encargo — reproduce ENTERO

`build_direct.py zSpeech zFe zTrack` → 5 ok (el filtro arrastra zFe2 y
zFeOverlay). `triage.py … --muro` da **las seis funciones con las cifras exactas
del encargo**, al dígito:

| bytes | % | función | unidad | `triage` |
|---|---|---|---|---|
| 708 | 97,181 | `TrackStreamer::GetLoadingPriority` | zTrack | 67 registros |
| 596 | 98,255 | `Speech::RoadblockFlow::Setup` | zSpeech | reorden local, dmax 4 |
| 512 | 97,969 | `MemcardCallbacks::FoundEntry` | zFe | reorden local, dmax 5 |
| 316 | 95,316 | `Speech::Manager::LoadSpeechBank` | zSpeech | **faltan 1, sobran 0** (`mr`) |
| 172 | 92,977 | `UIMemcardKeyboard::UIMemcardKeyboard` | zFe | reorden local, dmax 2 |
| 116 | 95,517 | `SubTitler::GetElapsedTime` | zFe | **faltan 1, sobran 0** (`fmr`) |

`audit.py Speed/Indep/SourceLists/{zSpeech,zFe,zTrack}`: **0 FALLA en las tres**,
en las dos pasadas (apertura y cierre). No hubo nada que confirmar.

Repetido el `triage` al cerrar la ronda tras reconstruir: **las seis cifras
idénticas** — mis parches están todos revertidos.

---

## 1. EL HALLAZGO: las tres unidades están bloqueadas ANTES que las funciones

Antes de tocar una línea pasé el enlazador a las tres (con
`scratchpad/c31ub_link.py`, un `trypromo.py` que además imprime **todos** los
errores y, si enlaza, la comparación del DOL sección a sección).

### `zSpeech` — **NO ENLAZA**: 318 errores

| veces | símbolo indefinido | quién lo referencia |
|---|---|---|
| **266** | `_Q33UTL11Collectionst9Singleton1Z7SoundAI.mInstance` | **nuestro** `zSpeech.o` |
| 36 | el mismo | los `.o` extraídos de otras unidades |
| 10 | `_7SoundAI.mRefCount` | **nuestro** `zSpeech.o` |
| 4 | `SPEECHFLOW_DISPLAY` | **nuestro** `zSpeech.o` |
| 2 | `MUSICFLOW_DISPLAY` | `zEAXSound2.o` (extraído) |

Es el frente §2 del brief: **estáticos de plantilla sin definir**
(`UTL::Collections::Singleton<SoundAI>::mInstance` y `SoundAI::mRefCount`) más
dos `int` que probablemente sean la regla «`int X = 0;` va a `.data`, `int X;` a
COMMON». Los 266 vienen de NUESTRO objeto: **no es que falte el dato, es que
nadie lo define en C++**.

### `zFe` — **NO ENLAZA**: 12 errores, y sólo TRES símbolos

| veces | símbolo | quién |
|---|---|---|
| 6 | `theMarker` | **nuestro** `zFe.o` |
| 4 | `MovieTextureInfo` | **nuestro** `zFe.o` |
| 2 | `_13MemoryCardImp.gEntryType` | `zPlatform.o` (extraído) |

**zFe está a tres definiciones de enlazar.** Es el caso más barato de las tres.

### `zTrack` — **ENLAZA**, pero el DOL sale CORTO −2.464 B

```
DOL ROTO  (4.539.424 B contra 4.541.888 B)   sha1 13bdc17467c7
  sec data2  CABECERA distinta: siz 0004bd20/0004c520   (d = -2048)
  sec data3  CABECERA distinta: siz 00041240/000413e0   (d =  -416)
  sec data4  off 00452bc0/00453560  adr 804fe380/804fedc0  siz igual (d = +0)
```

Y en `.text` salen **39.818 B distintos en 20.054 rachas**, casi todas de 1-2
bytes: **son punteros corridos por los 2.464 B que faltan**, exactamente lo que
avisa el §4 del brief. Las rachas largas (300 B en `vaddr 802c5380`, 122 B en
`8035f426`) caen en el rango de zTrack y son código, pero **la causa es el
acortamiento de datos, no `GetLoadingPriority`**.

**Veredicto para `lk`/`und`: `zTrack` necesita ceder al comodín 2.048 B en
`.data2` y 416 B en `.data3`.** No he tocado `splits.txt` ni `keep.lst`.

### Consecuencia para el encargo

**Cerrar las seis funciones era NECESARIO y no es SUFICIENTE en ninguna de las
tres.** Con eso el premio de cada función deja de ser «la unidad entera» y pasa a
ser sus propios bytes, así que reordené el trabajo por probabilidad de cierre en
vez de por bytes desbloqueados.

---

## 2. `UIMemcardKeyboard::UIMemcardKeyboard` — 92,977 → **99,4186 %** (11 ensayos)

La que más se movió, y la que deja la palanca más útil de la ronda.

### Diagnóstico

Somos 172 B contra 172 B con **19 filas**, y son **dos** cosas:

1. `this` está en **r30** y la base de `gButtonIDs` en **r29**; en el objetivo
   están **al revés** (`this` en r29 — lo confirma el DWARF: `// this: r29`).
2. El objetivo usa **el mismo registro** para el `@ha` y para la base
   (`lis r30` / `lwz r4, gButtonIDs@l(r30)` / `addi r30, r30, gButtonIDs@l`);
   nosotros gastamos un scratch (`lis r9` / `addi r29, r9, @l` / `lwz r4, @l(r9)`),
   porque nuestro `addi` sale **antes** del `lwz` y el `@ha` sigue vivo.

### Ensayos

| ensayo | qué | cifra | filas |
|---|---|---|---|
| `m1` | `__asm__("")` delante de `m_pCancel` (sólo) | 93,02326 | 19 |
| **`m2`** | **`__asm__("")` delante de `m_pOK`** | **98,90698** | **4** |
| `m3` | `*(gButtonIDs + 0/1)` | 92,97675 | 19 |
| `m4` | barrera al final del cuerpo | 92,97675 | 19 |
| **`m5`** | **`m2` + barrera delante de `m_pCancel`** | **99,41860** | **2** |
| `m6` | `ok`/`cancel` a locales | 89,51163, **176 B** | 20 |
| `m7` | `m2` + `this->GetPackageName()` | 98,90698 | 4 |
| `m8` | dos barreras seguidas delante de `m_pOK` | 99,41860 | 2 |
| `m9` | la primera con `"memory"` | 99,41860 | 2 |
| `m10` | `m5` + `*gButtonIDs` | 99,41860 | 2 |
| `m11` | `m2` + barrera al final | 98,90698 | 4 |

### Lo que enseña `m2`, y es nuevo

**Una barrera total (`__asm__("")`) delante de una sentencia no sólo reordena:
CAMBIÓ LA ASIGNACIÓN DE REGISTROS DE TODA LA FUNCIÓN.** Con ella `this` pasa de
r30 a **r29** y la base de `gButtonIDs` de r29 a **r30** — el reparto del
objetivo — y el par `lis`/`addi` se coalesce en un solo registro. Es coherente
con lo que ya está en memoria («sobre una variable de vida larga sí nace la insn:
+1 al `live_length` de TODOS los pseudos vivos en ese punto»), pero **hasta ahora
la barrera se usaba sólo para mover insns, no para arreglar `global_alloc`**.

### Lo que queda, y por qué es muro

Las 2 filas de `m5` son **el orden de carga de los dos argumentos** de
`FEngFindObject(GetPackageName(), gButtonIDs[0])`:

```
objetivo   lwz r4, gButtonIDs@l(r30)   /  lwz r3, 0xc(r29)
nuestro    lwz r3, 0xc(r29)            /  lwz r4, gButtonIDs@l(r30)
```

Las dos son cargas (coste 2), las dos alimentan sólo el `bl`, las dos tienen un
único dependiente: **empatan en prioridad y en `depend_count`, y el desempate de
`rank_for_schedule` es `INSN_UID`** — o sea el orden en que `expand_call` las
emitió. **No hay frontera de sentencia entre dos argumentos de la misma llamada,
así que no hay dónde poner nada.** La misma llamada de `m_pCancel` (índices
33/34) SÍ casa con la barrera puesta: el problema es sólo la que lleva el
`@l(r30)` en la dirección.

**REVERTIDA**: 99,4186 % no aporta un byte (`matched_code` es todo-o-nada) y dos
barreras que el original no tenía son deuda que no paga.

---

## 3. `MemcardCallbacks::FoundEntry` — 97,969 → **98,4375 %** (15 ensayos)

Las 3 filas son el par `lis`/`addi` del literal `""` de
`GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault)`:

```
objetivo   lwz r4  /  lis r5  /  mr r6  /  mr r7  /  addi r5
nuestro    lis r5  /  lwz r4  /  addi r5  /  mr r6  /  mr r7
```

`dwbody.py` da **cero diferencias** (mismas locales, mismo árbol de inlines): no
falta fuente.

| ensayo | qué | cifra | filas |
|---|---|---|---|
| `g1` | `__asm__("")` DELANTE de la llamada | 97,96875 | 3 |
| `g2` | `const char *empty = "";` | 96,86719, **516 B** | 15 |
| `g3` | `UIMemcardBase *scr = GetScreen();` | 97,96875 | 3 |
| `g4` | `const char *pName = info->mName;` | 95,77344, **516 B** | 15 |
| `g5` | barrera detrás + cast en `fDefault` | **98,43750** | **2** |
| **`g6`** | **`__asm__("")` DETRÁS de la llamada (sola)** | **98,43750** | **2** |
| `g7` | sólo el cast | 97,96875 | 3 |
| `h1` | `__asm__("" : : "r"(gs),"r"(fd))` detrás | 98,24219 | 7 |
| `h2`…`h6` | `g6` + `scr` · dos barreras · delante y detrás · `gs`/`fd` a locales · `&""[0]` | **todas 98,43750** | 2 |
| `k2` | `const char *empty; empty = "";` | 96,86719, **516 B** | 15 |
| `k3` | barrera detrás con `"memory"` | 98,43750 | 2 |

**`g6` confirma la regla direccional al dígito: la barrera va DETRÁS**
(`g1`, delante, no hace nada). Arregla el `lis`.

**Y lo que queda es exactamente el mismo muro que en §2**: el `addi` (el `lo_sum`
del literal) empata en prioridad con `mr r6` y `mr r7` — todos coste 1, todos con
el `bl` como único dependiente— y pierde el desempate por `INSN_UID`. En el
objetivo el par `lis`/`addi` tiene UID **más alto** que los `mr` (o sea que nació
*después* de los movimientos de argumento); en el nuestro nace antes. Once formas
de fuente distintas no lo mueven.

**REVERTIDA** por la misma razón que §2.

### La veda nueva, y vale para todo el árbol

> **El orden en que se cargan los ARGUMENTOS de una llamada (y el sitio del
> `lo_sum` de una dirección constante dentro de ese bloque) es un empate de
> `INSN_UID` en `rank_for_schedule`: sale del orden de emisión de `expand_call`,
> no del fuente. Ninguna barrera lo toca porque no hay frontera de sentencia
> entre dos argumentos.** Confirmado en DOS funciones independientes, 26 ensayos.
> Si un diff se reduce a «dos cargas de argumento permutadas» o «el `addi` del
> literal una ranura antes», **no gastes la ronda ahí**.

---

## 4. `SubTitler::GetElapsedTime` — 14 ensayos, la copia sigue al revés

La cabecera del fichero ya dejaba escrito el diagnóstico de la r25 y **lo
confirmo**: el DWARF del original dice que **`thetime_ms` NO TIENE REGISTRO**
(nuestro volcado le da `f1`), y lo que falta es un `fmr`:

```
objetivo   fmadds f0, f1, f13, f12  /  fmr f1, f0  /  stfs f0, 0x14(r31)
nuestro    fmadds f1, f1, f0, f13   /               stfs f1, 0x14(r31)
```

| ensayo | qué | cifra | B |
|---|---|---|---|
| base | `thetime_ms = expr; … timeElapsed = thetime_ms;` | 95,51724 | 112 |
| `v1` | **`timeElapsed = expr;` + `thetime_ms = timeElapsed;` en las DOS ramas** | 94,82758 | **120** |
| `v2`,`v3`,`v9`,`v11`,`v12` | `return timeElapsed;` en las dos ramas (5 formas) | **67,58620** | 112 |
| `v4` | base + `thetime_ms = timeElapsed;` detrás del store | 95,51724 | 112 |
| `v5`,`v6` | temporal `float t` explícito, dos órdenes | **idénticas** 95,51724 | 112 |
| `v10` | `thetime_ms` sin usar + `return timeElapsed;` | 95,34483 | 112 |
| `w1` | `v1` + barrera entre el store y la copia | 95,51724 | 112 |
| `w2`,`w3`,`w4` | `v1` + barrera al principio · al final · en las dos ramas | 94,82758 | 120 |

**`v1` es la forma correcta y da la secuencia del objetivo insn por insn**, pero
con los registros **cambiados**: nosotros `fmadds f1` + `fmr f0,f1` + `stfs f1` y
una copia extra `fmr f1,f0` en el bloque de unión (de ahí los 120 B).

**Diagnóstico afinado, con el reparto de FPR delante:** el objetivo usa
**f12, f13, f0** para los tres pseudos locales del bloque y deja **f1** para el
pseudo global del retorno; nosotros le damos **f1** al resultado del `fmadds`
porque `local_alloc` le llega la sugerencia de copia hacia el registro duro de
retorno, y entonces el global se tiene que conformar con f0 **y hace falta una
copia en la unión**. Hay que impedir que la sugerencia de f1 alcance al pseudo
del `fmadds`; **las cuatro posiciones de barrera no lo consiguen** (`w1` incluso
lo vuelve a plegar a 112 B).

**Veda nueva:** en `GetElapsedTime` la barrera no vale en ninguna de las cuatro
posiciones, y las **cinco** formas con `return` en las dos ramas **invierten los
bloques** (sale `beq` en vez de `bne`) y se hunden a 67,59 %.

---

## 5. `Speech::Manager::LoadSpeechBank` — 9 ensayos, de 17 a **7 filas**

Somos 312 B contra 316: **falta un `mr`**. El objetivo mantiene **dos** pseudos
con `index->item`: `addi r4, r12, 0x58`, luego `mr r11, r4` en la precabecera del
`while` y `mr r4, r11` **dentro** del bucle, con unos accesos por r4 y otros por
r11. Nosotros llevamos uno solo (r7) y una única copia.
DWARF: locales del original **`key` (r7), `lower` (r5), `upper` (r8), `i` (r10)** —
exactamente las nuestras, **sin ninguna local de puntero**.

| ensayo | qué | cifra | filas |
|---|---|---|---|
| `n1` | `goto found;` sin llaves | 95,31645 | 17 |
| `n2` | `for(;cond;)` en vez de `while` | 95,31645 | 17 |
| `n3` | `key == item[i].key` (operandos al revés) | 95,25317 | 17 |
| `n4` | `__asm__("")` delante del `while` | 95,31645 | 17 |
| `n5` | `CLUMP_ITEM *item = index->item;` a nivel de función | 95,69621 | 8 |
| **`n6`** | **`CLUMP_ITEM *item` DENTRO del `if` exterior, usado sólo en el bucle** | **95,82278** | **7** |
| `n7` | `(type << 24) + number` | 95,31645 | 17 |
| `n8` | `n6` + `item[...]` también en los tests posteriores | 96,51899, **308 B** | 16 |
| `n9` | `n6` con `index->item` en el `<` del bucle | 92,65823, **320 B** | 19 |

`n6` **mete `key` en r7 como el objetivo** (era la mitad de las filas) pero deja
el `addi r11, r12, 0x58` rematerializado detrás del bucle en vez de las dos
copias, y sigue en 312 B. **Y la local `item` no existe en el DWARF del
original**, así que no es la forma buena aunque puntúe mejor: no la dejo puesta.
`n8` es el caso número diez de la trampa del §5 de HERRAMIENTAS — **menos filas
con MENOS tamaño** (308 contra 316).

**Lo que falta**: reproducir las dos copias del objetivo, que son la firma de
`loop.c`/`move_movables` sacando el invariante `index->item` con una copia en la
precabecera. **No lo he encontrado desde el fuente en 9 formas (van 15 con las 6
de la r28).** Las 6 vedas de la r28 siguen en pie, y la de la etiqueta también
(no la he tocado).

---

## 6. `Speech::RoadblockFlow::Setup` — VEDA CONFIRMADA con prueba nueva (0 ensayos)

La r29 la dio por cerrada por construcción tras 16 formas. **Lo confirmo con dos
pruebas que no se habían hecho, sin gastar un ensayo:**

1. **El mapa de líneas dice que NO FALTA NINGUNA SENTENCIA.** El original emite
   `RoadblockFlow.cpp:414/415` (el `bRandom` + `GetRandomActiveCop`), `416` (el
   `if (primary == 0)`), `417` (el `return`), **418** (`RBUpdate`) y **419** (la
   llamada virtual). Alineando con nuestro fuente (283→414 … 288→418) sale que la
   única diferencia es que **el original escribe `if (primary == 0)` y `return;`
   en dos líneas sin llaves y nosotros en tres con llaves**. Cosmético.

2. **Las 3 filas son DOS empates independientes dentro de la MISMA expansión de
   llamada virtual**, y los dos caen del mismo lado:

   ```
   objetivo   lwz r9,0(r31) / lwz r0,0x2e4(r9) / lha r3,0x2e0(r9) / mtlr r0 / add r3,r31,r3 / blrl
   nuestro    lwz r9,0(r31) / lha r3,0x2e0(r9) / lwz r0,0x2e4(r9) / add r3,r31,r3 / mtlr r0 / blrl
   ```

   `lwz pfn` y `lha delta` empatan (coste 2, prioridad 3+P); `mtlr` y `add`
   empatan (coste 1, prioridad 1+P). En el objetivo gana la cadena del **pfn** en
   los dos; en el nuestro la del **delta**. O sea: **el front-end del original
   emitió el `pfn` antes que el `delta`, y nosotros al revés.** Es el mismo
   desempate por `INSN_UID` de §3, dentro de una expansión donde **no cabe
   ninguna sentencia**.

**Veda: `Setup` está cerrada por construcción. No gastéis más rondas ahí** —
van 16 formas y ahora la razón está medida, no supuesta.

---

## 7. `TrackStreamer::GetLoadingPriority` — el hueco de 16 B, LOCALIZADO (0 ensayos)

El comentario del fuente decía «faltan 16 B de una local del original sin
identificar». **El DWARF los sitúa exactamente.** `dwbody.py` da UNA sola
diferencia estructural en toda la función:

| objeto | original | nuestro |
|---|---|---|
| `layer_name[32]` | r1+0x08 | r1+0x08 (igual) |
| `pos` (12 B) | r1+0x28 | r1+0x28 (igual) |
| `face[4]` exterior | r1+0x38 | r1+0x38 (igual) |
| **`face[4]` del bucle** | **r1+0x48** | **r1+0x68** |
| `bScale::dest` | r1+0xA8 | r1+0x98 |
| `predict_pos` / `direction` / `v` / `bNormalize::dest` | 0xB8 / 0xC0 / 0xC8 / 0xD0 | 0xA8 / 0xB0 / 0xB8 / 0xC0 |

`FloatVector` mide **12 B** (`Espresso.hpp`), así que `FloatVector face[4]` = 48 B.
Nuestra zona de locales va **0x38 → 0x98 = 96 B** perfectamente empaquetada
(48+48). La del original va **0x38 → 0xA8 = 112 B**, y **el `face[4]` del bucle
empieza sólo 0x10 después del exterior**.

O sea: **en el original el objeto de r1+0x38 ocupa 16 B (no 48), y hay 48 B de
pila sin nombre entre el `face[4]` del bucle (0x48..0x78) y `bScale::dest`
(0xA8)** — o, equivalente, el array del bucle mide 96 B en vez de 48. Marco
0x120 contra nuestro 0x110.

**Ése es el cabo, y es de FUENTE, no de asignador**: el bloque muerto de
`RemoteCaffeinating` del original no declara lo mismo que el nuestro. Los 67
diffs de registros FP (f30↔f31, f11/f12/f13/f9 permutados) cuelgan de eso: todos
los desplazamientos de pila del diff son exactamente **±0x10**. **No he hecho
ningún ensayo aquí** — el hallazgo vale más entero que a medias, y quien lo siga
tiene el número exacto que buscar.

---

## 8. Ensayos numerados (49)

- **`UIMemcardKeyboard` (11)**: `m1`…`m11`. §2. Mejor: `m5` **99,41860 %**.
- **`FoundEntry` (15)**: `g1`…`g7`, `h1`…`h6`, `k2`, `k3`. §3. Mejor: `g6` **98,43750 %**.
- **`GetElapsedTime` (14)**: `v1`…`v6`, `v9`…`v12`, `w1`…`w4`. §4. Ninguna mejora.
- **`LoadSpeechBank` (9)**: `n1`…`n9`. §5. Mejor: `n6` **95,82278 %** (no aplicable).
- **`Setup` (0)** y **`GetLoadingPriority` (0)**: diagnóstico por DWARF y `lmap`, §6 y §7.

## 9. Vedas nuevas y vedas que siguen

**Nuevas:**

1. **El orden de carga de los ARGUMENTOS de una llamada no se toca desde el
   fuente** (empate de `INSN_UID` en `rank_for_schedule`; sin frontera de
   sentencia donde poner una barrera). Bloquea `FoundEntry` y
   `UIMemcardKeyboard` con 26 ensayos entre las dos.
2. **El sitio del `addi` (`lo_sum`) de una dirección constante dentro del bloque
   de argumentos: lo mismo.** En el objetivo el par `lis`/`addi` nace DESPUÉS de
   los `mr` de argumento; en el nuestro antes.
3. **`GetElapsedTime`: la barrera no vale en ninguna de 4 posiciones**, y las 5
   formas con `return` en las dos ramas invierten los bloques (67,59 %).
4. **`Setup`: cerrada por construcción**, ahora con el mapa de líneas y los dos
   empates identificados (§6).
5. **`LoadSpeechBank`: la local de puntero base NO es la forma del original**
   (el DWARF sólo tiene `key`, `lower`, `upper`, `i`), aunque `n6` puntúe +0,5 pp.

**Caducadas: ninguna.** Comprobé la de la etiqueta de `LoadSpeechBank` (r28,
−9,8 pp) indirectamente: ninguna de mis 9 formas mejora el fuente por debajo, así
que no había motivo para reabrirla.

**Palanca nueva (y no es veda):** **una barrera total delante de una sentencia
puede arreglar `global_alloc` de la función entera**, no sólo el orden de insns
(`m2`: `this` pasa de r30 a r29 y el par `lis`/`addi` se coalesce, +5,9 pp de
golpe). Hasta ahora el proyecto la usaba sólo como herramienta de orden.

## 10. Estado del árbol, herramientas y convivencia

- **Ficheros del árbol tocados por mí: NINGUNO.** Los `git diff` que quedan en
  `SubTitle.cpp` (`gCurrentSubtitler_ = nullptr`), `SpeechManager.cpp`
  (`SPEECH_DISPLAY_HISTORY` / `TRACKSTREAMER_BACKLOG_THRESH` de `extern` a
  definición) y `TrackStreamer.cpp` (`SeeulatorToolActive`, `ScenerySectionToBlink`,
  `ForceHoleFillerMethod`, `ShowSectionBoarder`) **son de OTROS agentes** (el
  frente §2 del brief). Los he respetado; mi runner restaura byte a byte.
- **CARRERA MEDIDA, y avisa a los demás**: dos compilaciones mías fallaron con
  errores en `CARP.h` (`gResolving` sin declarar) que **no reproducen** — otro
  agente estaba editando una cabecera compartida en ese instante. Es el §3 de
  HERRAMIENTAS al pie de la letra. **Repetidas, salieron bien.**
- **Herramientas nuevas en el scratchpad** (prefijo `c31ub_`):
  - `c31ub_run.py <unidad> <src> <sym> <fichero> <tag> <parche>` — parche
    anclado **línea a línea** (`splitlines(True)`, respeta CRLF/LF mezclados),
    compila con `mn_repro.py`, mide con `mn_diff.py` y **restaura siempre**
    (`finally`). 8-15 s por variante.
  - `c31ub_link.py <unidades…>` — enlace como `trypromo.py` pero imprime **todos**
    los errores agrupados y, si enlaza, **el DOL sección a sección con las rachas
    de bytes distintos**. Es lo que produjo el §1.
- **Stubs de `mn_repro.py`** (para el siguiente): `SubTitle.cpp`,
  `MemoryCardCallbacks.cpp` y `SpeechManager.cpp` compilan **sin `PRE`**;
  `uiMemcardBase.cpp` necesita
  `PRE='#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"'`.
  Los cuatro reproducen el porcentaje del `.o` de la unidad **al dígito**.
- **Finales de línea medidos**: `SubTitle.cpp`, `MemoryCardCallbacks.cpp`,
  `uiMemcardBase.cpp` y `SpeechManager.cpp` son **LF** en las zonas que toqué
  (git los convierte a CRLF al hacer checkout: por eso el parche tiene que
  anclarse línea a línea, no por bloque).
- **Disco**: entré con 15 GB libres y salgo con **18 GB**. Mis temporales
  (`stub_*`, `r_*.o`, `mnd_*.json`, `d_z*.json`, `c31ub_*`) ocupan poco; los
  `.json` grandes de objdiff que generé son tres (`d_zFe`, `d_zSpeech`,
  `d_zTrack`) y los dejo por si el siguiente reusa `REUSE=1`.
- **No he tocado** `configure.py`, `config/GOWE69/*`, `splits.txt`,
  `symbols.txt`, `keep.lst`, `build.ninja` ni los bloques `__ANDROID__`.
  **Sin commit.**

## 11. Lo que haría el siguiente, por orden

1. **`zFe`: tres definiciones y enlaza** (`theMarker`, `MovieTextureInfo`,
   `_13MemoryCardImp.gEntryType`). Es la unidad más cerca de promocionar de las
   tres y son 174.200 B.
2. **`zTrack`: ceder 2.048 B en `.data2` y 416 B en `.data3`** al comodín. Enlaza
   ya; sólo le falta eso y `GetLoadingPriority`.
3. **`GetLoadingPriority`: el objeto de 16 B en r1+0x38** (§7). Es el único de los
   seis muros que es fuente y no asignador.
4. **`zSpeech`: los estáticos de plantilla** (`Singleton<SoundAI>::mInstance`,
   `SoundAI::mRefCount`) — 276 de los 318 errores.
5. **NO volver a** `Setup`, ni al orden de argumentos de `FoundEntry` /
   `UIMemcardKeyboard` (§9.1, §9.2, §9.4).
