# r67b — lote `fe-world`: 16 barreras `asm("")` cortas, 7 retiradas, 9 irreducibles con diagnóstico

**Encargo**: retirar las barreras escritas con la palabra clave corta `asm("" ...)` que el
censo de las rondas anteriores no contaba. 10 ficheros en exclusiva, 6 unidades (zFEng y
zSim promocionadas; zFe, zWorld2, zTrack y zGameplay pendientes). Test: digests por sección
ALLOC y de `.symtab`/`.strtab` del `.o` compilado a un directorio privado, idénticos o se
revierte en el acto.

## 1. Cifras

| | |
|---|---|
| barreras cortas del lote (sin contar comentarios) | **16** |
| retiradas | **7** |
| barreras cortas al final | **9** (con nota r67b junto a cada una) |
| pines en los 10 ficheros | 20 → 20 (ninguno convertido, ninguno añadido) |
| `asm volatile` con instrucción | 2 → 2 |
| compilaciones de unidad | 16 (cada barrera quitada SOLA) + 8 formas y remedidas + 13 de aplicación (retiradas, notas, fin de línea) + 24 de sello (base y final, dos veces cada una) |
| revertidos | las 24 medidas, de forma automática. Las 11 distintas quedan fuera; las 6 idénticas (5 de la tanda 1 más `n1_fp`) se volvieron a aplicar con su nota como KEEP y se remidieron |
| regresiones | 0: las 6 unidades reproducen ALLOC y symtab base, dos veces en serie |

Censo con `scratchpad/few67b/censo2.py` (quita los comentarios antes de contar; patrón
`(?<![\w])(?:__asm__|asm)\s*\(\s*""`), HEAD contra copia de trabajo:

| fichero | HEAD | ahora |
|---|---|---|
| `FEngInterfaceFEObjects.cpp` | 2 | 2 |
| `FEngInterface.cpp` | 2 | **0** |
| `FEngine.cpp` | 1 | 1 |
| `FEPackage.cpp` | 1 | **0** |
| `WRoadNetwork.cpp` | 4 | **3** |
| `TrackStreamer.cpp` | 1 | **0** |
| `WeatherMan.cpp` | 1 | 1 |
| `TrackPath.cpp` | 1 | **0** |
| `QuickGame.cpp` | 2 | **1** |
| `GTrigger.cpp` | 1 | 1 |

## 2. Método (`scratchpad/few67b/`)

* `seal.py <unidad> [tag]` — compila con los cflags de `build_direct.parse_units()` a
  `o/`, y da el digest ALLOC (`tool.digest`, el de la r67), el de `.symtab`+`.strtab` y el
  sha1 del `.o` entero. Base compilada **dos veces**: iguales hasta en el sha1 del objeto.
* `quita3.py <spec>` — aplica reemplazos a un fichero, compila, compara ALLOC y symtab con la
  base, diffea la función contra el `.o` original y contra la base, imprime SOLO las secciones
  que cambian y revierte salvo `KEEP=True` e idéntico. Conserva el fin de línea del fichero
  (`RAW` para los de finales mixtos).
* Primera tanda: **cada barrera quitada SOLA**, dejando sus pines. Es lo que ninguna ronda
  había medido para las cortas: la r65 midió los pines («quitarlo a secas») y la r67 midió
  «sin los dos».

## 3. Resultado por barrera

| # | sitio | función (unidad) | quitada sola | resultado |
|---|---|---|---|---|
| 1 | `TrackPath.cpp` | `TrackPathManager::FindZone` (zTrack) | IDÉNTICO | **retirada** |
| 2 | `TrackStreamer.cpp` | `TrackStreamer::GetPredictedZone` (zTrack) | IDÉNTICO | **retirada** |
| 3 | `FEngInterface.cpp` rama 1 | `cFEng::PushErrorPackage` (zFe) | IDÉNTICO | **retirada** |
| 4 | `FEngInterface.cpp` rama 2 | idem | IDÉNTICO | **retirada** |
| 5 | `WRoadNetwork.cpp` `"=r"(guard)` | `WRoadNav::InitAtSegment` (zWorld2) | IDÉNTICO | **retirada** |
| 6 | `FEPackage.cpp` | `FEPackage::IssueScriptMessages` (zFEng) | 21 filas, 95 % | **retirada** con la forma natural (§4.2) |
| 7 | `QuickGame.cpp` fantasma | `QuickGame::OnManageTime` (zSim) | `.rodata` (orden de pool) | **retirada** con asignación redundante (§4.3) |
| 8 | `WRoadNetwork.cpp` `"+f"(endOffset) : "r"(guard)` | `InitAtSegment` | 49 filas (`2e2f76ec54345668`) | irreducible |
| 9 | `WRoadNetwork.cpp` `"+f"(offset_change)` | `HolePunchAvoidables` | 46 filas (`b15dd01508c59f70`) | irreducible |
| 10 | `WRoadNetwork.cpp` `"+f"(mx__)` | `CookieTrailCurvature` | 18 filas (`5d227cc0ebbe2e6f`) | irreducible |
| 11 | `WeatherMan.cpp` `"+m"(DistFogStart)` | `RegionQuery::CalculateRegionInfo` (zTrack) | 32 filas (`ff5f025d91536a9e`) | irreducible |
| 12 | `FEngine.cpp` | `FEngine::Update` (zFEng) | 9 filas (`98d0bf8686a32b4b`) | irreducible (§5.1, datos nuevos) |
| 13 | `FEngInterfaceFEObjects.cpp` X | `FEngSetScaleX` (zFe) | 2 filas (`a359156c72581295`) | irreducible (§5.2) |
| 14 | `FEngInterfaceFEObjects.cpp` Y | `FEngSetScaleY` (zFe) | 2 filas (`d3a94598fec3765a`) | irreducible |
| 15 | `QuickGame.cpp` `"+r"(collisionPos)` | `QuickGame::CreateCars` (zSim) | 2 filas (`028fac37ecf30e83`) | irreducible |
| 16 | `GTrigger.cpp` | `GTrigger::GTrigger` (zGameplay) | 4 filas (`c119a351b0605d66`) | irreducible, 3 formas más (§5.3) |

«Filas» = filas distintas de la función contra el `.o` original extraído (objdiff,
`calculatePoolRelocations=false`).

## 4. Hallazgos

### 4.1 Regla nueva: una barrera sobre una variable PINEADA es sospechosa de muerta

Cinco de las siete retiradas son el mismo caso: la barrera lleva como operando una variable
que ya es `register T x asm("rN")`. Esa variable no es un allocno: es el registro duro, así
que la referencia de más no sube la prioridad de nadie. Las notas r67 de `FindZone` y
`GetPredictedZone` decían «pin y barrera son una sola cosa: UNA referencia de más»; el
diagnóstico del pin es bueno (sin los dos, 5 y 10 filas), pero la mitad de la barrera nunca
se midió sola y no hacía nada.

En `InitAtSegment` el caso es más fino: la primera barrera `"=r"(guard)` *definía* r28 y la
segunda lo consume. Sin la definición, la segunda lee r28 sin que nadie lo escriba y r28 queda
vivo desde la entrada: el objeto es idéntico, así que antes de ese punto nada pedía r28. La
segunda sí es necesaria (49 filas sin ella).

**Uso**: antes de clasificar una barrera como irreducible, medirla sola con sus pines puestos.
Cuesta una compilación. En este lote: 5 de 16.

### 4.2 `IssueScriptMessages`: tres locales inventadas y un `for (;;)` que el mapa de líneas no tiene

El DWARF del original sólo declara `pEvents` (sin registro), `i` r27 y `Count` r22, más dos
bloques anónimos con `pTargetPtr`. Nuestro fuente tenía `eventOffset`, `savedOffset` y
`pEvent`, y la salida del bucle duplicada a mano. `regmap` daba ESTRUCTURA (4 locales sólo
nuestras). El mapa de líneas da un solo bucle: la cabecera en `FEPackage.cpp:342` con la
condición doble (`i < Count` y `tTime < tTo`) y el `i++` con la condición copiada en `:395`.
El `mr r28,r4` del objetivo no es una local: lo pone GCSE, que guarda el `i*12` en un
registro que sobrevive a `FindObjectByGUID`.

Forma natural, a la primera: `while (i < Count && pEvents[i].tTime < (u32)tNewTime) { switch
(pEvents[i].Target) { ... } i++; }` con `pEvents[i]` en cada uso → zFEng **IDÉNTICO**. Las
tres locales inventadas y la barrera fuera.

### 4.3 `OnManageTime`: el cebador de pool flotante SÍ existe, pero tiene que ir donde no domine

La barrera `{ float phantom; asm("" : "=f"(phantom) : "f"(1.0f), "f"(0.0f)); }` creaba la
entrada de `0.0f` del pool antes que la de `0.01f` (pool del objetivo `{1.0f, 0.0f, 0.01f}`).
Desde la r67 el `1.0f` ya nace en la primera comparación, así que sólo faltaba el `0.0f`.

* `float delta_speed = 0.0f;` en la declaración: **28 filas** y un FPR preservado de más
  (`14debecdd270994e`). La carga está en el bloque de entrada, DOMINA las de más abajo y
  GCSE las sustituye por su registro, que tiene que cruzar llamadas.
* `camera_time = 0.0f;` delante de `camera_time = camera->GetSimTimeMultiplier();`, dentro
  del `if (camera)`: **IDÉNTICO**. Crea la entrada en ese punto, flow borra la asignación
  pisada (cero instrucciones) y, como ese bloque no domina las cargas de abajo, GCSE no tiene
  nada que compartir.

Esto matiza la nota «el primer de pool no vale para floats»: la `static inline` muerta no
vale, pero una asignación pisada en un bloque que no domina a los usos sí. El
`.set lbl_80404864, $LC523` no se ha tocado: `.rodata`, `.rela.*` y symtab idénticos.

## 5. Irreducibles: lo nuevo

### 5.1 `FEngine::Update`: tres datos del DWARF que la r27/r28 no tenía

Sin la barrera, 9 filas: el 0 se propaga y desaparecen `li r27,0` y `mr. r28,r27`. Los datos
nuevos están escritos junto a la barrera:

1. el original lista `iIterationTicks` **sin registro** (el nuestro: r27). El `li r27,0`
   (`FEngine.cpp:760`) y el `mr. r28,r27` (`:798`) son de un temporal, no de esa local;
2. el bloque de `iIterationTicks` acaba en `0x801861B4`, **antes** de `bRenderedRecently =
   false` (`:797`) y de la condición. El nuestro llega al final del bucle, 12 B más. En el
   original esas dos sentencias están fuera del ámbito de `iIterationTicks`;
3. el 0 de `:760` no tiene línea propia: va detrás del `GetFirstPackage` inline.

Hipótesis sin medir: un temporal con dos sets (el de un `?:` cuyos brazos se funden, p. ej.)
no lo propaga cprop, que exige `REG_N_SETS == 1`. Ninguna forma encaja con los tres datos en
tres intentos, así que queda la barrera y el problema es de estructura.

### 5.2 `FEngSetScaleX/Y`: es sched1, no reparto

Sin la barrera, con los pines puestos, salen 2 filas: el `lwz r0,0x18(r30)` del `switch` se
coloca delante del `fmr f13,f31`. Lo más probable es que el clobber de `"memory"` actúe como
escritura pendiente para sched1, y que las dos lecturas de memoria de detrás (`Type` y
`Size.x`) dependan de ella. Probado también `float size = data->Size.x;` delante del bloque
de `scale`: las mismas 2 filas. El mapa de líneas del objetivo (848 `fmr`, 851 `Type`,
849 `Size`) no muestra ninguna sentencia intermedia que pueda hacer de escritura.

### 5.3 `GTrigger::GTrigger`: tres formas más, las tres negativas

La r36b ya había probado tres. Ahora, sin la barrera:

| forma | filas |
|---|---|
| sin la barrera (base de la medida) | 4 (`c119a351b0605d66`) |
| una sola sentencia `MultYRot(boxmat, -Rotation() / 360.0f, boxmat)`, sin `rot` (el mapa de líneas lo pone todo en `GTrigger.cpp:113`) | 4, el mismo objeto |
| `-Rotation() * (1.0f / 360.0f)` | 4, el mismo objeto |
| `-(Rotation() / 360.0f)` | 8 (`a46db81bc0d3cebc`) |

### 5.4 Los demás, remedidos solos y sin forma nueva

* **`HolePunchAvoidables`**, 46 filas: la r48 cerró la rama con la cifra (hace falta un insn
  menos en el bucle).
* **`InitAtSegment`, segunda barrera**, 49 filas: igual que quitar pin y barreras (r65). La
  cantidad no tiene nombre en el DWARF.
* **`CookieTrailCurvature`**, 18 filas: se pliega la copia al preservado y vuelve el ciclo
  de flotantes de la r36b.
* **`CalculateRegionInfo`**, 32 filas: el original no declara locales en el bloque y la
  retícula r65 ya está barrida.
* **`CreateCars`**, 2 filas: el segundo store del constructor se adelanta al `mr r4`. Es una
  ranura de sched1 de la misma clase que el pin (r67).

## 6. Sellos: antes y después, las 6 unidades

Digests de la r67 (`tool.digest`: `.text`, `.rodata`, `.data`, `.bss`, `.sdata*`, `.ctors` y
sus `.rela.*`; sin `.line`/`.debug*`/`.comment`/`.stab*`) más el de `.symtab`+`.strtab`. Base
compilada dos veces al empezar y final dos veces en serie con todas las notas puestas: las
cuatro iguales. Los sha1 del `.o` entero cambian (`.line`/`.debug`), como debe ser.

| unidad | estado | ALLOC antes | ALLOC después | symtab antes | symtab después |
|---|---|---|---|---|---|
| zFEng | promovida | `3869f721cf92634e` | `3869f721cf92634e` | `30950ecf29503427` | `30950ecf29503427` |
| zSim | promovida | `6913d4cebd1c34a7` | `6913d4cebd1c34a7` | `2f56601845f99e8f` | `2f56601845f99e8f` |
| zTrack | pendiente | `904e61b404a8ea8b` | `904e61b404a8ea8b` | `1b281638832b5535` | `1b281638832b5535` |
| zWorld2 | pendiente | `8e7ee9e293166fa6` | `8e7ee9e293166fa6` | `8988e465e3c1c922` | `8988e465e3c1c922` |
| zGameplay | pendiente | `8608d060553a271a` | `8608d060553a271a` | `1f622edb02ea7139` | `1f622edb02ea7139` |
| zFe | pendiente | `5a12f894633da873` | `5a12f894633da873` | `56f0bf475e754b0a` | `56f0bf475e754b0a` |

Por sección (sha1[:12]:tamaño; iguales antes y después en las seis):

* **zFe**: `.text` 681582830fe9:185212, `.rela.text` c27d042ced53:167052, `.rodata`
  e574107ba4da:20584, `.rela.rodata` 4a36de9408b3:9780, `.data` 3e2c041a973c:1684,
  `.rela.data` 30283ac03e83:1332, `.ctors` 9069ca78e745:4, `.rela.ctors` a9df38974b1d:12,
  `.bss` nobits:1620.
* **zFEng**: `.text` fbd60c5d2253:73756, `.rela.text` 7dcf5b1adb36:43416, `.rodata`
  1d4a592b6d77:2632, `.rela.rodata` 32939450b90c:648, `.data` 82101053fe83:384,
  `.rela.data` 5a80c32f1024:72, `.ctors` 9069ca78e745:4, `.rela.ctors` d4a140d381cc:12,
  `.bss` nobits:332.
* **zWorld2**: `.text` f3ba20f9dd3c:148268, `.rela.text` b615654521d2:92520, `.rodata`
  706a40b1fcc4:4808, `.rela.rodata` 913f1096f471:816, `.data` 450285c58b58:396,
  `.ctors` 9069ca78e745:4, `.rela.ctors` 6768e810a915:12, `.bss` nobits:1408.
* **zTrack**: `.text` 53c6c4a7645e:67280, `.rela.text` 5a3baada94e8:45000, `.rodata`
  a7dc0c28d36d:3624, `.data` cd56439f089a:4864, `.rela.data` d3e26dd11be1:72, `.ctors`
  9069ca78e745:4, `.rela.ctors` 39c945641691:12, `.bss` nobits:47300.
* **zSim**: `.text` 43193f3b6672:109324, `.rela.text` 02cc67571158:77952, `.rodata`
  ccd9b60ffe97:8384, `.rela.rodata` 176b0cc3a681:5604, `.data` 4c8f8036143b:348,
  `.rela.data` 82e53f430d98:12, `.ctors` 9069ca78e745:4, `.rela.ctors` 768ffafc4592:12,
  `.bss` nobits:8164.
* **zGameplay**: `.text` f83a7bb7e86a:153376, `.rela.text` 1c72252a0982:110724, `.rodata`
  fdc0b66e46d5:5680, `.rela.rodata` 277881c1c18c:660, `.data` 822b7b6f6c8e:712,
  `.rela.data` 5f95cd6929fb:276, `.ctors` 9069ca78e745:4, `.rela.ctors` 7582b51d44cf:12,
  `.bss` nobits:248.

Ninguna tiene `.sdata`/`.sdata2`. zFEng y zSim están promovidas: con ALLOC y symtab idénticos
el DOL no se mueve, pero el enlace completo lo tiene que confirmar el cierre (este lote no
enlaza).

## 7. Controles

* `git diff -U0` de los 10 ficheros: las líneas `-` que no son comentario son exactamente las
  7 barreras retiradas, el bloque `{ float phantom; ... }`, y el cuerpo viejo de
  `IssueScriptMessages` (las tres locales inventadas y el `for (;;)`). Ningún pin ni ninguna
  otra sentencia ha desaparecido.
* `__LINE__`/`BNEW`: sólo `FEngInterface.cpp` (líneas 20 y 27, antes de cualquier cambio).
* Fin de línea: cada fichero conserva el suyo. `FEPackage.cpp` ya venía con 46 líneas LF
  sueltas dentro de `IssueScriptMessages` y `UpdateGroup`: el bloque reescrito va en CRLF y
  las 8 que quedaban LF se han normalizado a CRLF (zFEng idéntico tras ese cambio). Ahora es
  CRLF puro.
* Ni cabeceras, ni `configure.py`, ni `config/GOWE69/*`, ni `keep.lst`, ni ficheros de
  otros lotes.

## 8. Propuestas

Ninguna de `configure.py`, `config/GOWE69/*` ni cabeceras.

**Auditoría sugerida (otros lotes)**: aplicar la regla §4.1 en todo el árbol, es decir, medir
SOLA cada barrera cuyos operandos sean variables ya pineadas. Da 5 de 16 aquí y cuesta una
compilación por barrera.

## 9. Veredicto

**PARCIAL: 7 de 16 retiradas y 9 irreducibles con el diagnóstico r67b junto al andamio.**
Objetos ALLOC y symtab idénticos en las 6 unidades. Cinco de las retiradas eran barreras
muertas detrás de un pin, y nadie las había medido solas. Las otras dos salen con C
legítimo: la forma natural del bucle de `IssueScriptMessages`, que el DWARF y el mapa de líneas
describen, y una asignación pisada que sustituye al cebador de pool por `asm`.
