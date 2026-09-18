# Auditoría de `docs/PLAYBOOK.md` — 2-sep-2026

Estado del árbol al auditar: **91,49% matched**, `main.dol: OK`, 17.856 funciones
al 100%, `report.json` regenerado el 2-sep 13:12.

**No se tocó `src/`, `configure.py`, `config/` ni `scripts/`.** Sólo `docs/`.

## Las cifras

| | antes | después |
|---|---|---|
| entradas (`- **…**`) | **824** | **803** (−21, fundidas) |
| líneas | 4.973 | 5.223 |
| secciones numeradas | 11 | **11 (sin renumerar)** |
| subsecciones `###` nuevas | 0 | 5 (todas dentro de §6) |
| entradas sin cifra medida | 333 | 323 |

- **5 reglas falsas** reescritas con la corrección **dentro de la propia
  entrada** (antes vivían a 300–800 líneas de su corrección, o directamente sin
  ella).
- **12 vedas rotas**, marcadas como ROTAS con lo que sí funcionó.
- **8 contradicciones fundidas** en una entrada con su discriminante.
- **1 contradicción NO resuelta**, marcada como tal en el sitio (no había
  discriminante que dar).
- **21 entradas eliminadas**: todas por absorción en una entrada consolidada.
  **Ninguna medida se perdió**; cada cifra de las 21 está reproducida en la
  entrada que las sustituye.

## La numeración

**No cambió.** §1…§11 siguen donde estaban. Lo único que se movió: **§6 (Control
de flujo, 130 entradas, la mayor del manual) se partió por dentro** con
subtítulos `###`, sin mover una sola entrada de sección:

- **6.1** Ramas, bucles, guardas y `switch` (el cuerpo, sigue entremezclado)
- **6.2** Cross-jumping: quién se funde y hacia dónde
- **6.3** La saga del `cr7` (veda ROTA; se conserva por el método)
- **6.4** El banco de un solo `.cpp` (1-3 s por variante)
- **6.5** Vedas y techos medidos, con el porcentaje del 2-sep

**Por qué no se renumeró de verdad**: partir §6 en dos secciones obligaba a
correr §7…§11 y a reescribir todas las referencias cruzadas «§N» mientras ocho
agentes leen el fichero. El coste de que un agente busque en la sección
equivocada es mayor que el de una sección larga con índice. §1 y §6 llevan ahora
un párrafo de navegación al principio, y la cabecera un índice de las 11.

---

## 1. Entradas FALSAS, y cómo quedaron

### 1.1 `litpos`: el test es el MULTICONJUNTO, no el registro destino
- **Dónde estaba el problema**: §1 traía «comprueba también el REGISTRO destino
  del `lfs` antes de tocar nada» y la corrección estaba en §2, **780 líneas más
  abajo**. Un agente leyó la de §1 y avisó mal a dos subagentes.
- **Qué se hizo**: la entrada de §1 se reescribió con el test correcto y las dos
  medidas que lo demuestran (`EPlayRaceNIS` y `UpdateAdaptiveDifficulty`
  coinciden en registro destino y aun así son permutación pura;
  `FinalizeRaceStats` sí era un bug). **La entrada duplicada de §2 se borró.**

### 1.2 Las locales muertas NO reordenan el pool
- **Dónde estaba el problema**: §4 tenía la corrección («probado dos veces,
  cero cambio»), pero §5 seguía diciendo «**La local flotante MUERTA ordena el
  pool**» sin matiz.
- **Qué se hizo**: entrada única con el **discriminante**, en las dos secciones:
  - local **entera muerta** (declarada y nunca usada) → **cero efecto**
    (`Convert16To32`, `AdvanceAnimTime`);
  - **inicializador muerto sobre una local que SÍ se usa** → sí crea y coloca la
    entrada del pool (`VisualLookEffect::UpdateActive` 97,27 → 100%, 300 B;
    cuatro `= 0.0f` fundiendo dos `lis $LC@ha`, 93,44 → 97,91%; los
    `bool x = false;` de `Moment::React`, 58 de 117 líneas de diff).
  - Excepción conservada: la local muerta que **mantiene vivo un valor** (el
    «contador muerto») sí paga.

### 1.3 Un límite sin canonicalizar NO viene sólo de un `switch`
- **Dónde estaba el problema**: §7 cerraba con «un límite sin canonicalizar
  **sólo** puede venir de un `switch`», y las tres correcciones estaban
  repartidas por §6 (dos «ACOTACIÓN/CORRECCIÓN» y una entrada de zEAXSound).
- **Qué se hizo**: una entrada consolidada en §7 con **los tres orígenes** y las
  seis formas de `if` descartadas en micro-banco:
  1. `case LO ... HI:` (extensión GNU, compila con ProDG 3.9.3);
  2. **bucle con cota VARIABLE** (`i < num - 1`) — con el corolario para
     `casetree.py`: filtrar los nodos cuyo `cmpwi` esté en una arista de retorno;
  3. **la constante guardada en un LOCAL** (en *expand* el operando es un REG,
     no un `CONST_INT`): `ShouldPlayEngOffSweet`, 116 B a la primera.
  Las cuatro entradas dispersas de §6 pasaron a punteros de dos líneas.

### 1.4 `ATTRIB_NO_INLINE_CLASSKEY` no es un frente de bytes
- **Dónde estaba el problema**: la corrección medida y la entrada vieja («**vale
  miles de bytes**», con las 17 unidades y las 77/65/24/19… llamadas) estaban
  **adyacentes y sin fundir**. Leídas en orden se contradicen.
- **Qué se hizo**: una sola entrada. Primero el resultado (0 llamadas de menos y
  0 de más; empeora seis unidades: zEAXSound −9.284 B, zPhysics −1.820, zWorld
  −1.144, zEAXSound2 −1.080, zGameplay −936, zMain −880), después **de dónde
  venía el espejismo** (el conteo no veía las reubicaciones a `.text+0xNNNN` de
  la copia local) y qué sí quedó (fidelidad de símbolos: 59 cabeceras con la
  guarda, copias débiles espurias 13 → 6, unidades exactas 2 → 11 de 17).

### 1.5 El `case` vacío: siete entradas que se contradecían → una
- **Dónde estaba el problema**: convivían «EL `case` VACÍO SE BORRA SIEMPRE (las
  cuatro variantes **pierden el nodo**)», «un `case` con cuerpo VACÍO desaparece
  del código pero **cuenta** en el árbol», «los `case` pegados al `default` **NO**
  se borran, CUENTAN» y «los `case` pegados al `default` **SE BORRAN** antes de
  `balance_case_nodes`». Cuatro afirmaciones incompatibles, las cuatro con
  micro-banco.
- **El discriminante que las reconcilia** (y que ya estaba medido, sólo que en
  otra entrada): **dentro o por encima del rango emitido el `case` vacío se
  borra y no mueve nada; por debajo del mínimo —o negativo— sí cuenta, porque es
  lo que le da al nodo de rango su límite inferior**. Los 47 intentos que no
  movieron el pivote eran todos dentro del rango; todos los éxitos (`case -1:`,
  `case NONE: break;`, `case 0: case 1: case 2:` + `case 4:`) están por debajo.
- **Qué se hizo**: una entrada con las cinco medidas (incluido el grupo contiguo
  pegado al `default`, que sí cuenta, y el `case` suelto pegado al `default`, que
  sí emite su `beq`). Las **siete** entradas dispersas se borraron.

---

## 2. Vedas ROTAS

Barridas **55 funciones** nombradas en una veda o lista de techos contra
`build/GOWE69/report.json` (2-sep). **17 casan hoy al 100% — 38.128 B — y 12 de
ellas seguían escritas en el playbook como veda viva.**

| función | B | qué decía la veda | qué la cerró de verdad |
|---|---|---|---|
| `Moment::React` ×4 | 14.748 | «8 variantes a mano + ~1.650 del permutador; 71 formas medidas; el par de diffs no cede» | quitar el idiom `__typeof__(x) t = expr; x = t;` (97,62 → 100%) **y** dos inicializadores muertos `bool x = false;` (58 de 117 líneas de diff) |
| `FnDeltaQ::EvalSQT` + `EvalSQTMasked` + `FnDeltaQFast::EvalSQTMask` | 12.516 | «la veda del `cr7` **sigue en pie**» (11 variantes de tipo + 25 estructurales + 15 de forma) | `if (c) X; else X;` con **dos ramas idénticas en un bloque POSTERIOR** al de la primera comparación |
| `WGrid::FindNodes` | 3.456 | (acotación del `goto`: 93,01 → 86,17%) | la aritmética del delta de mnemónicos: cinco bucles, dos ascendentes y dos descendentes |
| `GinsuSynthesis::HandlePacketRelease` | 1.936 | «siete formas de declarar `blend`/`blendstep` dan **binario idéntico**» | el permutador **ciego**, por función, sin `--timeout`, `-j 8`: `float x = 0.0f;` en la variante ~50 de **198** |
| `CAnimCtrl` + `CWorldAnimCtrl::AdvanceAnimTime` | 1.516 | «rotación cíclica pura de f23..f30; **no es la forma de ninguna sentencia**» | (rotación = síntoma, no techo) |
| `Articulation::Constraint::React` | 936 | «8 combinaciones + 7 formas; la palanca, si existe, no es el `return`» | **sí era el `return`, pero CUÁNTOS**: `if/else if/else` con **un solo** `return`, no dos |
| `TestLoadStreamingEntry` | 812 | «falta mucho más que el campo (87,7%, 67 diffs, tamaño mal)» | código sin escribir |
| `MilestoneBoard::MilestoneBoard` | 500 | «las **24 permutaciones** barridas; mínimo 7 diffs» | (la permutación no era la palanca) |
| `ActionQueue::FetchCurrentValues` | 488 | «TECHO ABIERTO: `thread_jumps` funde dos `case`, cuesta 8 B» | **no invertir** la comparación del `case` hermano: mismo signo y cuerpos cambiados |
| `MenuScreen::MenuScreen` | 472 | «97,80%, 3 diffs, tamaño exacto; **doce formas** no lo mueven» | (el hermano `~MenuScreen` cerró con la cadena `if (A==0 \|\| …)` escrita como un `bool` con `&&`) |
| `FnEventBlender::Eval` | 284 | veda anotada en el `git log`, no llegó al playbook | — |
| `UnlockSystem::IsEventAvailable` | 248 | «invertir la polaridad de la guarda baja 0,7 pt» | guarda `== 19.8.31` envolvente + `return true` de cola |
| `ScratchPtr<T>::_Alloc` ×2 | 216 | «5 formas, salida idéntica» | `__asm__("")` **entre** dos sentencias (barrera de planificación) |

Las dos últimas (`ScratchPtr`, y la receta del `cr7`) ya estaban corregidas en el
playbook; se han **nombrado las funciones**, que faltaba, para que el siguiente
lector pueda cruzarlas con `report.json`.

**El patrón que explica casi todas**: se habían barrido N formas de la
**sentencia equivocada**. Está escrito ahora en la entrada madre de §1 («LAS
LISTAS DE TECHOS TIENEN ENTRADAS FALSAS»), con las tres comprobaciones previas:
porcentaje de hoy, ¿el barrido tocaba la sentencia que señala el diff?, y si
`regmap` dice IDÉNTICO abre el catálogo entero del permutador.

### Vedas que SIGUEN en pie (verificadas, con el % del 2-sep)

`ResolveCarBanks` 98,44% · `FnRawStateChan::FindTime` 99,57% ·
`FnRunBlender::FindMatchTime` 97,36% · `FnStatelessF3::EvalSQT` 99,35% ·
`FnStatelessQ::EvalSQT` 95,39% · `FnDeltaQFast::EvalSQT` 98,41% ·
`DynamicLoader::Initialize` **80,40%** (el texto decía 79,66%) ·
`NewBlockAux` 88,57% · `Smackable::Smackable` 99,26% · `GetUnlockFilter` 91,30% ·
`FEMarkerSelection::NotificationMessage` 97,83% · `PostRacePursuitScreen` 98,22% ·
`BlendVisualLookAttribute(bMatrix4&)` 95,97% · `RenderFlaresOnCar` 98,06% ·
`CullParts` 99,33% · `UpdateParticles` **99,96%** (el texto decía 99,571%) ·
`GenerateHorizonFogDisplayList` 98,99% · `SetMemoryPoolSize` 97,37% ·
`DrawAmbientShadow` 98,87% · `InitSFX` 99,97% · `cStichWrapper::Play` 99,69% ·
`ShouldBeginAccelTrans_Idle` 92,36% · `inbetween` 87,50% ·
`CreateSubMixChannels` 98,69% · `CreateMixCtls` 97,32% · `MsgBarrier` 94,29% ·
`CARSFX_Turbo::ProcessUpdate` 97,50% · `TestToLicensed` 98,26% ·
`Assess180` 98,96% · `RoadblockFlow::Setup` 98,26% · `StrategyFlow::Waiting` 95,49% ·
`HolePunchAvoidables` 95,28% · `fseek` 98,27% · `PATH_createstreamimp` 99,92% ·
`bStrNICmp` 97,78% · `SteeringWheelDevice::UpdateForces` 96,66% ·
`GRaceParameters::GenerateIndex` 98,10% · `FnRawLinearChannel::Eval` 96,88% ·
`Geometry::SphereVsBox` 97,71% · `GameDevice::PollDevice` 99,93% ·
`FindConditionBranchTarget` 84,44%.

Los porcentajes se han anotado **en el playbook**, en las tres listas de vedas
(§6.5, §9 ×2, §10 ×2), para que la próxima auditoría sea un `diff` y no un
barrido.

---

## 3. Contradicciones fundidas (8)

1. **`T *p = miembro;` iza el `lwz`** (§4) contra **«usar el MIEMBRO, no una
   local cacheada»** (§3). Cinco entradas repartidas por §3, §4 y §9.
   **Discriminante**: *cuántas veces lee el objetivo*. Una lectura → liga el
   local (`mnemo` da `mr +9 / lwz −9`; 82,78 → 94,39%). Varias → quita el local
   y repite el miembro (83,67 → 100%; repetir `GetPart()` vale 6 puntos; en el
   frontend gana repetir incluso la llamada virtual). **Trampa añadida**:
   `lwz` + `mr` de más en el original **no** es una relectura, es `regmove`.
2. **`case` compartiendo etiqueta con el `default`: «NO se borran» vs «SE
   BORRAN»** → ver §1.5 arriba.
3. **`case` vacío «se borra siempre» vs «cuenta en el árbol»** → ver §1.5.
4. **`case` cuyo cuerpo es sólo `break;`: «no existe en el original» vs «hay que
   añadirlo»**. Dos entradas idénticas en secciones distintas.
   **Discriminante ya medido, ahora explícito**: *sobra cuando **funde** dos
   nodos, hace falta cuando los **separa***; se decide contando las
   comparaciones del objetivo. (`SetSFXBaseObject` 90,61 → 100% borrando dos;
   `UpdateTRQ` 89,52 → 100% añadiendo uno.)
5. **`litpos` obligatorio / `litcheck` opcional** vs **«hay que pasar los dos»**
   vs una tercera entrada de `litcheck`. Tres entradas → una, con lo que ve cada
   uno: `litcheck` (multiconjunto) **no puede ver dos constantes intercambiadas**;
   `litpos` (posición a posición) es el único que las ve. Práctico: `litpos`
   siempre; `litcheck` cuando sospeches de una constante **ausente**.
6. **El permutador: guiado vs ciego.** §1 decía «guiado cuando `regmap` dice
   reparto»; §10 decía «el ciego cierra lo que el guiado ni intenta».
   **Discriminante**: lo elige `regmap` — reparto-con-un-registro → `--guided`;
   **IDÉNTICO → el guiado genera CERO variantes y parece techo**, hay que abrir
   el catálogo (`FnPhaseChan::Eval`, variante 23 de 97, 644 B, 0 diffs).
   `--sweep --timeout` no sirve en ninguno de los dos.
7. **`regmap` «SOLO NUESTRA»**: siete entradas repartidas, cada una anotando su
   propio empeoramiento sin sumar. Fundidas en una con el **recuento acumulado:
   1 mejora contra 8 empeoramientos**, y las cuatro razones por las que la local
   existe aunque el DWARF no la liste. Incluido el caso más fino
   (`DrawAmbientShadow`: el tamaño decía +4 B, o sea que la local sobraba, **y
   aun así las dos formas de quitarla empeoran**).
8. **Accesores vacíos**: «la veta más barata que queda» (597 en 154 cabeceras)
   contra «cero bytes por esa vía» y «veta cerrada con medida». Fundidas en una
   con el método correcto: **no por nombre** (los cabezas del ranking son falsos
   positivos), **sí por firma** (44 en EAXSound, 5 reales, uno de 840 B) y **sí
   por `deadreg`** (704 B en una pasada; hay que correrlo también sobre f9-f31,
   1.788 B), más el estado medido: agotada en zEAXSound/zEAXSound2 y en
   zFe2+zFeOverlay.

### Contradicción que NO se pudo resolver (marcada como tal)

**La «cabeza» de una tirada de stores: ¿el PRIMER store o el ÚLTIMO?**
- «cabeza = **ÚLTIMO** store en orden de fuente de cada registro de valor
  distinto» — verificado 3/3, cerró dos constructores, un tercero 90,36 →
  99,83%, `FEQuaternion::GetMatrix` 196/196 a la primera.
- «cabeza = **PRIMER** store, en orden **ascendente de offset**, y la lista de
  inicialización **después** del cuerpo» — 73,82 → **93,45%**.

Las dos están medidas y **no encontré el discriminante**. Se dejó una entrada
que **declara la contradicción abierta**, dice cuál probar primero (la del
ÚLTIMO, tiene más confirmaciones), recuerda que con ≤4 stores conviene enumerar
las 24 permutaciones (ya valió 704 B donde la regla predijo `pnco` y el bueno era
`ncop`), y conserva las dos acotaciones medidas (una sentencia que escribe dos
registros de valor distinto; un store que es llamada a constructor).

---

## 4. Duplicados retirados

Cuatro entradas estaban **duplicadas casi literalmente** entre §1/§10 y dentro
de §4 — que es exactamente el mecanismo por el que nació la contradicción de
`litpos`. Se dejó una completa y la otra como puntero de dos líneas, sin perder
ninguna cifra:

- «EL ORDEN DE CREACIÓN DEL POOL DE CONSTANTES ES UN DIAGNÓSTICO» (§1 y §10) —
  canónica en §1, que es la que lleva la acotación importante («arreglar el
  orden NO puntúa; sirve para localizar la sentencia, no como palanca»).
- «`libc` SON DOS BIBLIOTECAS» (§1 y §10) — canónica en §1 (el `grep`).
- «EL SESGO DE `(float)(int)`» (§1 y §10) — canónica en §10 (lleva los ~20 flags
  descartados, el análisis de RTL y las 12 unidades afectadas).
- «DOS CONTADORES DE BUCLE QUE DEBERÍAN SER UNO» (§4, dos veces) — se quedó la
  versión larga, que trae el caso `learn()` de NeuQuant (98,86% y 33 diffs →
  100%, 700 B).

---

## 5. Entradas sin evidencia

De las 803 entradas, **323 no traen una medida explícita**. La inmensa mayoría
**no son heurísticas**: son hechos de codegen comprobables de un vistazo en el
`.s` («`*++p` da `lhau`/`lhzu`», «`sizeof(bool)` es 4», «`s32` es `long`»).
Marcarlas todas habría añadido 323 líneas de ruido.

**Lo que se hizo en su lugar**: una regla en la cabecera —«una entrada CON cifra
es una regla; una entrada SIN cifra es una PISTA: pruébala y mide»— y marcar
*(pista, sin cifra)* sólo las que además usan lenguaje de conjetura, que son
**cuatro**:

- §4 «un `addi rN, r1, off` de más en el objetivo **suele ser** una referencia»
- §6 «guardas compuestas partidas: `if (A && B)` **puede ser** …»
- §8 «la local sólo-en-el-original **suele ser** el valor de retorno descartado»
- §8 «la instanciación que falta **puede ser** un CAST que falta»

Y una quinta anotada como «sin cifra, pero el mecanismo es verificable de un
vistazo»: §2, `regmap` da falso positivo en los `static` de función.

**Dudosas y NO tocadas** (plausibles, sin medida, y no conviene borrarlas):
- §1 «CUATRO DISCRIMINADORES DE COMPILADOR» — el (4) `lis @h` + `ori @l` = asm
  escrito a mano no trae contraejemplo.
- §6 «el pivote del árbol de un switch es la mediana de los `case`» convive con
  la fórmula exacta del pivote tres entradas más abajo; la fórmula manda, pero
  la frase de la mediana se dejó porque es la aproximación mental útil.
- §5 «Coloca las funciones nuevas en el orden del `.s` original» — cita «dos
  funciones destapadas» sin cifra.
- La sección §11 (Ronda 10) es un anexo de una ronda concreta y no se
  reorganizó; sus tres vedas (`Cache::Init`, `HolePunchAvoidables`, `Assess180`)
  se verificaron y siguen en pie.

---

## 6. Avisos de herramienta (no arreglados, como pedía el encargo)

1. **Alguien escribió el playbook a través de una ruta que expande `\b`.** La
   entrada de accesores vacíos contenía un **byte 0x08 literal** donde el texto
   quería decir `` `\bNOMBRE\s*\(` ``. Corregido en el documento, pero **el
   script o el `echo`/heredoc que lo escribió sigue por ahí**: cualquier regex
   con `\b`, `\t`, `\n` que se añada al playbook por esa vía se corrompe en
   silencio. Merece un `grep -P '[\x00-\x08\x0b\x0c\x0e-\x1f]'` sobre `docs/`
   antes de commitear.
2. **`report.json` no trae `functions` en 70 de 590 unidades** (520 sí). Es
   consistente con la nota del playbook sobre el report rancio, pero conviene
   saber que un barrido por función sobre esas 70 devuelve vacío, no error.
3. Sigue vigente y sin arreglar lo que ya documentan el playbook y
   `ventana-pendiente.md`: **ningún subcomando de objdiff aplica
   `symbol_mappings`** (2.456 B incontables en `gc_interface`).

---

## 7. Lo que la próxima auditoría debería hacer primero

1. **Volver a cruzar las vedas contra `report.json`.** Ahora que los porcentajes
   están anotados en el playbook, es un `diff`, no un barrido. 12 de 55 en tres
   rondas es la tasa de caducidad observada.
2. **Resolver la contradicción del PRIMER/ÚLTIMO store** con un micro-banco: es
   la única que quedó abierta y toca una de las transformaciones más usadas.
3. **§6.1 sigue entremezclada** (≈110 entradas de ramas, bucles y `switch` en un
   solo bloque). Reordenarla por tema es trabajo de ventana, con el árbol
   parado, no de una auditoría en caliente.
