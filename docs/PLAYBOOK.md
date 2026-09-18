# Manual de decompilación matching — NFS Most Wanted (GameCube, GOWE69)

> **Son 6.240 lineas. No lo leas entero.** `docs/PLAYBOOK-MAPA.md` dice que hay
> en cada seccion y en que rango, para leer solo el trozo que toca.

Compilador: **SN ProDG 3.9.3 (GCC 2.9 PPC)**, `-O1 -gdwarf+ -ffast-math
-fschedule-insns -fschedule-insns2 -fexpensive-optimizations -fgcse
-frerun-cse-after-loop`.

**La métrica: `matched_code` es todo-o-nada.** Una función al 99,99% aporta
**cero** bytes; un near-miss vale su tamaño íntegro en cuanto cruza. Cuenta
funciones cruzadas, no puntos.

## Cómo leer este manual

1. **Una entrada CON cifra es una regla; una entrada SIN cifra es una PISTA.**
   Se auditaron **824 entradas**; quedan **803** (21 se fundieron en entradas
   consolidadas), y de ellas **323 no traen una medida explícita**. No están mal
   por eso —casi todas son hechos de codegen, no heurísticas— pero **no valen
   como argumento**: pruébalas y mide. Las cuatro que sí eran conjetura pura van
   marcadas *(pista, sin cifra)*.
2. **Una veda dice «no cede a ESTAS vías», nunca «no cede».** Están fechadas y
   con el porcentaje del día. Antes de descartar una función por una veda,
   **mira su porcentaje de hoy** (`build/GOWE69/report.json`,
   `scripts/mnemo.py <unidad>`): en la auditoría del 2-sep, **12 vedas
   nombraban funciones que hoy casan al 100%**.
3. **Cuando dos entradas se contradicen, la buena trae el discriminante.** Si
   encuentras un par sin discriminante, es un bug del manual: mídelo y dilo.
4. **Auditoría**: 2-sep-2026, `docs/analisis/auditoria-playbook.md`. Ahí está lo
   que se borró, lo que se fundió y lo que quedó marcado como dudoso.

**Secciones**: 1 Herramientas · 2 Trampas de medición · 3 Estructura y ámbitos ·
4 Locales, marco y registros · 5 Orden de emisión · **6 Control de flujo** ·
7 Aritmética y comparaciones · 8 Tipos, clases y símbolos · 9 Del frontend ·
10 Método · 11 Ronda 10.

**La numeración de las 11 secciones NO ha cambiado en esta auditoría.** Lo único
que se movió es que §6 —la más larga, 130 entradas— se partió **por dentro**:
**6.1** ramas, bucles, guardas y `switch` (el cuerpo) · **6.2** cross-jumping
(quién se funde y hacia dónde) · **6.3** la saga del `cr7` (veda ROTA, se
conserva por el método) · **6.4** el banco de un solo `.cpp` · **6.5** vedas y
techos medidos, con el porcentaje del 2-sep. §1 y §6 llevan además un párrafo de
navegación al principio.

---

## 1. Herramientas

- **`cc1.exe` DE SN TIENE OPCIONES QUE NO SALEN EN `--help`.** Se leen de su
  tabla `target_switches` dentro del binario: `safe-sda`, `sndata=`, `fast-cast`,
  `align-section-names`... **`-msafe-sda` manda el POOL DE CONSTANTES del
  compilador a `.sdata` con `@sda21`** --una instruccion en vez de `lis @ha`+`@l`--
  y eso cerro **siete unidades de libm y 10.184 B** que llevaban dos rondas
  dandose por muro. Explica de paso por que TODAS las constantes de fdlibm del
  objetivo estan en `.sdata` (son entradas del pool, no variables) y por que hubo
  que inventar el truco `SDA_DOUBLE`.
  **Es POR OBJETO, no por biblioteca**: aplicado a todo `libc/` cuesta -15.544 B.
  Y **bloquea la promocion**: el enlace responde `L1001: Failed to create small
  data reference to address 0x804`, asi que la unidad casa al 100% pero no puede
  ser `Matching` mientras el enlace no alcance esa referencia.
- **`-mstrict-align` decide el orden de operandos del `lfsx`**: un `float[2]` mide
  64 bits, `layout_type` le da `DImode` y sale `indice, base`; con el flag pasa a
  `BLKmode` y sale `base, indice`. No depende de la forma de la fuente (12 formas
  probadas), sino del modo del tipo del array.

- **CAMBIAR **UN** CFLAG SOBRE EL `.cpp` REAL IDENTIFICA EL PASE CULPABLE EN UNA
  SOLA COMPILACION.** No es para cambiar los flags --son correctos y eso esta
  medido-- sino para **saber quien te esta haciendo el dano** y buscar la forma de
  fuente que lo evita. Casos de la ronda 15: `Update__7FEnginelUi` -> el culpable
  es **`-fgcse`** (su cprop propaga el 0 porque `iIterationTicks` tiene un unico
  set constante que domina el uso); `MaybeDoJumpCam` -> **`-fcse-skip-blocks`**, y
  con `-fno-cse-skip-blocks` la funcion sale **exactamente del tamano del
  objetivo**. Un repro de 40 lineas con los cflags exactos itera despues en ~1 s.
  **Ojo: que el flag arregle la funcion NO significa que el original no lo
  llevara** -- compilar `FEngine.cpp` entero con `-fno-gcse` baja de 10.888 a
  4.424 B al 100%.
- **UNA VARIABLE DE RESULTADO SIN INICIALIZADOR, ASIGNADA EN LAS DOS RAMAS, NO ES
  LO MISMO QUE DOS `return`.** Con `return` GCC **siempre** iza el `li r3,0`
  delante de la rama y paga un `mr` para el argumento; con `int i; if (p) i =
  f(p); else i = 0; return i;` emite `mr. r3,r4` --que hace de test **y** de
  preparacion del argumento-- y el `li r3,0` al final. 12 formas con `return`
  barridas, todas izadas. **Y el orden de las ramas manda**: la rama del valor
  primero da `beq`+`li` al final, que es el objetivo.


- **EL CONTADOR DE EXPANSIONES DEL DESTRUCTOR EN EL DWARF DICE CUANTAS SALIDAS
  TENIA EL AMBITO.** El original expande `~copList()` **7 veces** y nosotros 3:
  los tres `goto` no existian, cada rama se escribe entera con su propio
  `return`, y **GCC vuelve a fundir las colas**. Cerro **1.704 B a la primera
  compilacion**. Corroborado con dos contadores mas (`GetDispatch` 8 contra 6,
  `GetHistory` 3 contra 0).
- **`dwbody.py` SE QUEDA RANCIO SIN AVISAR:** lee `build/regmap/our_<unidad>_*` y
  compara contra el `.o` de antes. Hay que borrar `our_zX_*.nothpp` **y**
  `idx_our_zX_*.pkl` y relanzar `regmap.py` tras cada build.
- **EL VOLCADO DWARF INVIERTE LOS DECLARADORES DE UNA MISMA SENTENCIA.**
  `unsigned int p, dataOffset, baseOffset;` vuelca **`baseOffset, dataOffset,
  p`**. Sentencias **separadas** no se invierten. Verificado por construccion: al
  escribirlo al reves el volcado paso a coincidir con el original **con codegen
  identico al bit**. **Sin esto se lee mal `dwbody`.**
- **LAS HERMANAS AL 100% DESMIENTEN VEDAS.** La veda de la local `times` decia
  «quitarla empeora en las tres»; pero `FnDeltaQ::EvalSQT` (4.980 B) y
  `FnDeltaSingleQ::EvalSQT` (5.188 B) **casan al 100% escribiendo el miembro
  directo**. O sea que la forma correcta existe: lo que falla es **otra** cosa en
  las tres que no cierran (los `mr` del precabezal que crea `-fforce-mem`).
- **EL PERMUTADOR CIERRA POR `move_stmt` CUANDO EL DIFF ES UN BLOQUE
  DESORDENADO.** Con `opcodes=1.0` y solo posiciones bailando: 115 variantes, 2
  rondas, **6 minutos -> IDENTICO**. Es con diferencia lo mas rentable en ese
  caso.
- **EL PERMUTADOR COGE LA FUNCION EQUIVOCADA SI EL NOMBRE ESTA DUPLICADO EN EL
  ARBOL:** eligio dos funciones de un fichero **que ni siquiera esta en la
  SourceList** y costo una tanda. **Pasale siempre `--file` y `--symbol`.**
- **NO SOLAPES `build_direct.py` CON EL PERMUTADOR:** con 26 `ngccc` en marcha
  reporta `FAILED` en unidades que compilan (`rc` 0 al reinvocar a mano). Es
  contencion, no un error de fuente.
- **`fuse.py` DELATA LAS ASIGNACIONES DEL CUERPO QUE EN EL ORIGINAL SON LA LISTA
  DE INICIALIZACION:** si los `stw` de siete miembros salen todos en **la linea de
  la declaracion del constructor** y no en lineas de cuerpo, van a la lista de
  mem-init. Eso disolvio una permutacion de 4 registros que `regmap` daba por
  «candidato real del asignador»: **94,05 -> 98,13% en 2.276 B, a la primera**.
  **Merece la pena barrer con este test todos los constructores near-miss.**
- **ARITMETICA DE LINEAS DE `fuse.py`:** mapeando nuestras lineas contra las del
  original se deduce **que sentencias comparten linea**. De ahi salio tambien que
  una funcion tiene **dos `return`** y no uno (la salida unica mide 81,81% y 140 B
  contra 144 del objetivo).
- **LA HERMANA QUE YA CASA AL 100% ES EL MANUAL DE ESTILO DEL ORIGINAL.** En un
  fichero, dos funciones al 100% escribian `ret = f(...); if (ret >= 0)` y
  **duplicaban la llamada final dentro de cada rama**; la tercera tenia
  `if (f(...) >= 0)` con la llamada factorizada detras. **Copiar el estilo de la
  hermana la cerro de golpe (93,63 -> 100%, 600 B, a la primera compilacion).**
  Antes de pelear con el asignador, **mira si hay una hermana al 100% en el mismo
  fichero**.
- **`libdiff --lines` + `libdwarf` ES EL `plan.py` DEL MIDDLEWARE.** El mapa de
  lineas del original da la estructura de sentencias directa: **que `if` protege
  que, que sentencias comparten linea, donde abre el `else`**. De ahi salieron
  cinco funciones y **6.300 B**, casi todas a la primera o segunda compilacion.
- **EL DWARF DICE CUANTAS LOCALES HAY, O SEA DICE SI TE ESTA PERMITIDO INVENTAR UN
  TEMPORAL.** Una funcion de 3.016 B solo declaraba una: eso descarto la local
  nueva y senalo **reasignar el parametro**.
- **`dwbody.py` COMPARA EL ARBOL DE EXPANSIONES INLINE DEL DWARF, QUE ES LO QUE
  `regmap` NO MIRA.** `regmap` compara locales y **registros**; nadie comparaba
  las **expansiones**. Ve un `IsEmpty()` que escribimos como `GetHead() !=
  EndOfList()`, tres `m[i][j]` de mas, un `UMath::Dot` que el original no usa, un
  `Reset()` que falta, y los `goto` que no tenemos (salen como `// Labels`). Una
  pasada sobre 32 near-miss destapo diferencias **estructurales en 13**. Cuesta
  segundos y no necesita objdiff.
*Sección larga y no ordenada por tema. Las entradas que más se consultan:
**qué mide de verdad objdiff** (las dos opciones de `objdiff-cli diff`, el 0%
del `symbol_mappings`, las secciones renombradas), **literales** (`litpos`
obligatorio / `litcheck` complementario, el pool y su orden), **el permutador**
(guiado o ciego, lo decide `regmap`), **`regmap`/`lreg`/`plan`/`fuse`** (qué
diagnostica cada uno), **accesores vacíos** (por firma, no por nombre),
**identificar el compilador de una unidad**, y **las otras versiones del juego**
(PS2 tiene nombres; Xbox 360 no). Al final hay una tabla de herramientas y la
lista de fuentes de datos.*

- **PERMUTADOR: GUIADO O CIEGO, Y LO ELIGE `regmap`. ESTE ES EL DISCRIMINANTE.**
  - **`regmap` dice «REPARTO CON UN REGISTRO» -> `--guided`.** Es lo mas
    rentable que hay. Encontro un `float x = 0.0f;` en la variante ~50 de 198
    que cerro `GinsuSynthesis::HandlePacketRelease` (**1.936 B**), y `cse_temp`
    cerro otra de **1.172 B** moviendo un puntero de r3 a r7.
  - **`regmap` dice «IDENTICO» -> `--guided` genera CERO variantes y sale en
    3 s, y eso PARECE un techo.** Ahi hay que abrir el **catalogo entero**
    (`--rounds 3 --limit 120`): en `FnPhaseChan::Eval` (644 B, 99,57%, tamano
    exacto, permutacion pura de f10/f11/f12) la **variante 23 de 97** dio
    `*** IDENTICO ***` con un `cse_temp` sobre `w`. **644 B, 0 diffs.**
  - **`--sweep --timeout` NO SIRVE en ninguno de los dos casos.** Seis funciones
    probadas asi terminaron en TIMEOUT sin acabar la ronda 1 y produjeron
    **cero**. El coste real es **8-85 s por variante x 120-320 variantes**: hay
    que lanzarlo **por funcion, sin timeout, con `-j 8`**.
  **Corolario que ya ha pagado tres veces: una permutacion pura de registros de
  *scratch* NO es techo**, aunque el volcado DWARF no liste ninguna local nueva
  (el temporal se fusiona con la variable original y no crea entrada).
- **EL PERMUTADOR NO VALIDA SEMANTICA EN `move_stmt`:** su mejor variante para una
  funcion dejaba una local **sin inicializar**. Hay que leer el cuerpo antes de
  aplicar.
- **UN SIMULADOR DE REGISTROS SOBRE EL `.s` DEL TROCEADOR SACA UN STATIC-INIT
  ENTERO DE UNA PASADA.** Reconstruye instruccion a instruccion **que global
  recibe que valor en que desplazamiento**, leyendo los literales del ELF. Saco
  **~140 inicializadores** de zCamera (26,40 -> **97,11%**, 3.604 B) incluidos
  2.464 B de floats de `.data` leidos byte a byte. `scratchpad/sim.py`.
- **`config/GOWE69/symbols.txt` ES EL MAPA EXACTO DEL ORDEN DE DECLARACION.** El
  orden de `.bss` dice entre que dos ficheros va cada TU y entre que dos objetos
  va cada global.
- **`fndiff.py` y `pct_ecs.py` NECESITAN EL MANGLADO COMPLETO** (200 caracteres en
  las plantillas). Con el nombre corto dicen `not found`, y es facil dar por
  **ausente** una funcion que esta escrita. Cuidado al concluir «falta esta
  funcion» desde una de esas herramientas.
- **UN METODO NUEVO EN UNA PLANTILLA DE CABECERA INSTANCIA TODO LO QUE LLAMA, Y ESO
  ROMPE TIPOS QUE NO LO SOPORTAN.** Anadir `assign()` a `Vector<T>` tumbo zPhysics,
  zMain y zSim: `assign` llamaba a `resize()`, `resize()` instancia `push_back()`,
  y `push_back()` hace `new (p) T()` — pero
  `GarbageNode<PhysicsObject,160>::Collector::_Node` **solo tiene `_Node(T*)` y
  copia**. La rama culpable solo podia ENCOGER, asi que se reescribio con
  `pop_back()` y desaparecio la instanciacion. **Al tocar una plantilla de
  cabecera, mira que INSTANCIA cada camino, no solo si compila en tu unidad.**
- **PARA DECIDIR SI UN AVISO DE `litpos.py` ES REAL, EL TEST ES EL
  MULTICONJUNTO — NO EL REGISTRO DESTINO DEL `lfs`.** `litpos` da «DISTINTO»
  falso cuando el diff es una **permutacion de registros**: canto tres
  constantes cruzadas (0,25 / 0,5 / 1,0) donde el multiconjunto era identico y
  el codigo el mismo, y solo cambiaba **que FPR** las tiene (f26/f27/f29
  rotados). **El test correcto: si el multiconjunto de valores de los dos lados
  es identico, es el asignador y no hay nada que arreglar; si difiere, es un bug
  real.**
  **CORRECCION MEDIDA (la regla vieja decia «comprueba el REGISTRO destino» y
  NO BASTA):** en `EPlayRaceNIS` y `UpdateAdaptiveDifficulty` los `lfs`
  **coinciden en registro destino** y aun asi eran permutacion pura. Con el
  filtro del multiconjunto, `FinalizeRaceStats` si era un bug real. La regla
  vieja hizo que un agente avisara mal a dos subagentes: no la uses.
- **EL ARBOL DE INLINES DEL VOLCADO ES TAMBIEN UN ORACULO NEGATIVO.** Si el volcado
  **no** nombra el envoltorio que nuestra fuente llama, **el original llamo a la
  funcion de debajo**. `UMath::ScaleAdd(...)` -> `VU0_v4scaleadd(...)`: **688 B a
  la primera compilacion**, tras fallar 3 variantes de ligar referencias. El
  playbook solo tenia la mitad positiva de esta regla.
- **`litpos.py` (POSICION A POSICION) ES OBLIGATORIO; `litcheck.py`
  (MULTICONJUNTO) ES EL COMPLEMENTO BARATO — Y CADA UNO VE LO QUE EL OTRO NO.**
  Los dos atacan el mismo agujero: con `function_reloc_diffs=none` **objdiff no
  ve QUE constante carga un `lfs`**, asi que un literal equivocado es
  **invisible al porcentaje** y aun asi rompe el DOL en cuanto la unidad se
  promociona.
  - **`litcheck` compara el MULTICONJUNTO**, asi que **no puede ver dos
    constantes intercambiadas entre si** — el conjunto es identico en los dos
    lados. Cuatro aciertos de cuatro en su primera pasada, y en la segunda
    `DoTunnelBloom` —cerrada al 100% dos rondas antes— cargaba `0,588235` donde
    el original tiene `0,0588235`. Pero medido en otras tres unidades dio **31
    lineas de ruido y cero senal** (cadenas, direcciones de pool y deltas de
    near-miss ya conocidos).
  - **`litpos` compara POSICION A POSICION**, y es el unico que ve el
    intercambio: `8000.0f`/`4000.0f` cruzadas en una funcion **que ya media
    100%**, y un `127.0f`/`255.0f` cruzados en otra cuyo porcentaje **no se
    movio ni un decimal**.
  **Practico: pasa `litpos` siempre y en toda unidad antes de pelear registros;
  pasa `litcheck` cuando `litpos` no diga nada y sospeches de una constante
  ausente.** *Falso positivo conocido de `litpos`*: una entrada de pool que
  contiene una DIRECCION vale 0 con reubicacion en nuestro `.o`; filtra
  `addi rX,rY,$LC@l` pero no un `lwz` de entrada con direccion.
- **EL SCORE INTERNO DEL PERMUTADOR PUEDE DISCREPAR DE objdiff VARIOS PUNTOS, Y
  CONTRADECIR A `regmap`.** Sus ganadores de una ronda (+7 puntos internos) median
  **3,8 puntos PEOR en objdiff** y ademas **borraban locales que el DWARF declara**.
  Confirma SIEMPRE con objdiff, y **veta las transformaciones que eliminan una
  local que el volcado nombra**.
- **EL «BLOQUE QUE SOBRA» DE `regmap` PUEDE ESTAR EN LA OTRA DIRECCION: reejecutalo
  tras CADA edicion de ambito.** El arbol se renumera con cada cambio, y un nivel
  de mas puede venir de un `if` mucho mas arriba.
- **UNA CONSTANTE DE UN INLINE SE ATRIBUYE POR LA `.rodata` DEL LLAMANTE, NO POR
  EL CONTEO GLOBAL.** Se cambio `KPH2MPS` de `x / 3.6f` a `x * 0.27778f` porque el
  frontend tenia la segunda, y **rompio 26 constantes de zAI en silencio**. La
  prueba correcta: las **17** apariciones de `0x3E8E38E4` estan **todas** en el
  `.rodata` de zAI —que es quien LLAMA a la macro— y las **10** de `0x3E8E392E`
  **ninguna**: viven en el frontend, donde la fuente escribe la multiplicacion **a
  mano** sin usar la macro. **Localiza la constante en la `.rodata` de la unidad
  llamante antes de tocar una cabecera.**
- **UNA MEDIDA DE +0 B EN UN CAMBIO DE CONSTANTE NO ES «INOFENSIVO»: ES «objdiff no
  puede verlo».** Es la misma trampa del «100% que miente» aplicada al que hace el
  cambio. Con literales, la verificacion es `litcheck`/`litpos` y el ELF, **nunca**
  `matched_code`.
- **TODO SIMBOLO DE DATOS POR DEBAJO DEL 100% CON `diffs = 0` EN `pct_ecs` ES UN
  VALOR EQUIVOCADO.** Es el barrido mas barato que hay y dio **4 de 4**: dos arrays
  de *tweak* con los numeros cambiados, y un `heat_cutoffs[4]` **declarado y nunca
  definido** que salia UNDEF en `symtabdiff --und` y hacia leer basura.
- **`litpos.py` CACHEA su volcado y NO lo invalida**: tras recompilar da un desfase
  de UNA entrada y reporta **todas** las referencias como distintas. Borra su
  cache antes de repetirlo.
- **`KPH2MPS(x)` ES `x * 0.27778f`, NO `x / 3.6f`.** Medido al bit: la division
  pliega a `0x3E8E38E4` y el objetivo tiene `0x3E8E392E`. **+0 B pero mueve el
  DOL**, y por tanto bloquea la promocion. La pista estaba en el arbol: otro
  fichero ya llevaba `* 0.27778f` escrito a mano en cuatro sitios.
- **`objdiff-cli diff` NO APLICA `symbol_mappings` (solo la GUI y `report`).** Por
  eso `measure.py`, `censo.py` y cualquier barrido propio dan como **MISSING** un
  simbolo que esta **escrito y al 84%**, si el troceador lo nombro distinto
  (2.456 B en `gc_interface`). **Truco para medirlo igualmente**: copiar el `.o`
  del objetivo y **poner un NUL en el `.strtab`** justo tras el nombre corto — que
  es prefijo del largo. Otra fuente de censo inflado, como las funciones anonimas.
- **`_GLOBAL_.I.<nombre>` TOMA EL NOMBRE DE LA PRIMERA FUNCION GLOBAL DEL TU.**
  Meter una funcion nueva **por encima** lo renombra y **pierde el match en
  silencio** (44 B). Respeta el orden de `.fn` del troceador al anadir codigo.
- **`if (x > 1.0f)` contra `> 0.0f` NO SOLO CAMBIA LA CONSTANTE: cambia el
  ASIGNADOR.** Con `0.0f` el pool tiene **una** entrada en vez de dos y sobra un
  allocno flotante, asi que desaparece de propina una permutacion f29/f31 que
  parecia un techo. **Un caracter, 540 B.**
- **UN PORCENTAJE BAJO EN UNA UNIDAD PEQUENA ES ARITMETICA, NO FALTA DE CODIGO.**
  zDynamics estaba al **64,51%** y parecia el peor sitio del juego. Tiene **41
  funciones para 28.976 B** (~707 B de media), y los 10.284 B que faltaban eran
  **exactamente** la suma de sus **cuatro** unicas near-miss
  (5.668+1.896+1.784+936, coincidencia al byte). Cerrando **una** subio a 84,07%.
  **Divide el hueco entre el numero de funciones antes de sacar conclusiones.**
- **LAS LISTAS DE TECHOS TIENEN ENTRADAS FALSAS, Y LA PROPORCION ESTA MEDIDA:
  EN LA AUDITORIA DEL 2-SEP, 12 DE LAS ~40 FUNCIONES NOMBRADAS EN UNA VEDA
  CASABAN YA AL 100%.** Un techo es «no cede a ESTAS formas», nunca «no cede».
  Las doce, con lo que las cerro:
  `Moment::React` (las 4 sobrecargas, 14.748 B — el temporal `__typeof__` y dos
  inicializadores muertos), `ScratchPtr::_Alloc` (216 B — `__asm__("")` entre
  dos sentencias), `ActionQueue::FetchCurrentValues` (488 B — la polaridad del
  `case` hermano), `CAnimCtrl::AdvanceAnimTime` y `CWorldAnimCtrl::AdvanceAnimTime`
  (1.516 B), `FnEventBlender::Eval` (284 B), `Articulation::Constraint::React`
  (936 B — un solo `return` en vez de tres), `MenuScreen::MenuScreen` (472 B),
  `MilestoneBoard::MilestoneBoard` (500 B), `UnlockSystem::IsEventAvailable`
  (248 B), `GinsuSynthesis::HandlePacketRelease` (1.936 B — el permutador ciego,
  variante ~50 de 198), `FnDeltaQ::EvalSQT`/`EvalSQTMasked`/
  `FnDeltaQFast::EvalSQTMask` (12.516 B — la receta del `cr7`) y
  `TestLoadStreamingEntry` (812 B).
  **El patron que las explica casi todas: se habian barrido N formas de la
  SENTENCIA equivocada.** Antes de creerte una veda: (1) mira el porcentaje de
  hoy en `report.json`, (2) comprueba que el barrido tocaba la sentencia que el
  diff senala, y (3) si `regmap` dice IDENTICO, abre el catalogo entero del
  permutador — el guiado genera cero variantes ahi y parece techo.
- **`python scripts/missinline.py unit <u>` TABULA las expansiones inline del
  volcado contra las llamadas de la fuente.** De ahi salieron el **decimo**
  `AlignPointer` de una funcion (860 B) y un accesor que faltaba (756 B).
- **`objdiff-cli` TARDA 44 s CUANDO NUESTRO LADO NO TIENE `.data` Y EL OBJETIVO SI**
  (90 kB en zPlatform), contra 0,4 s con el `.o` completo de la unidad. **Por eso
  el banco no rinde en todas partes**: en zPlatform son 0,8 s de compilacion mas
  44 s de medida contra 10 s de `build_direct`; en zCamera, 8 s contra 19.
  **Si el banco tarda mas que `build_direct`, mira el `.data` del objetivo.**
- **EL MULTICONJUNTO DE MNEMONICOS PARTE EL TRABAJO EN DOS, Y ES MAS BARATO QUE
  LEER EL DIFF.** `scripts/mnemo.py <unidad>`: **mismo multiconjunto** = las
  mismas instrucciones en otro orden o con otros registros, o sea **asignador o
  planificador, y NO falta codigo** (7 de 19 en una unidad); un delta como
  `stfs +1, lfs +1, mr -1` = **falta o sobra una SENTENCIA**, y el opcode dice
  cual. Asi se nombro en **una sola pasada** que faltaba `position.z = 0.0f;`,
  donde el diff no lo ensenaba: 95,43% -> **100% a la primera compilacion**.
- **LOS BARRIDOS MECANICOS NO TIENEN «TERRENO VIRGEN»: corren sobre TODO el
  juego.** Se mandaron dos unidades que **nunca habian tenido agente** esperando
  fruta facil y los **cinco** barridos (`censo`, `loss miss`, `deadreg`,
  `undefcheck`, `callcheck --real`) salieron **a cero**. Lo que si estaba sin
  tocar era el near-miss. **Una unidad sin agente no es una unidad sin barrer.**
- **CUIDADO CON LAS UNIDADES QUE RENOMBRAN SECCION (`zFeOverlay`, `zOnline`).** Un
  banco o una medida que **no pase el `.o` por `tools/rename_section.py`** las mide
  a **0,0000% con 0 diffs y los tamanos correctos**: el fallo silencioso perfecto.
  `bench.py` lo tenia y esta arreglado. Si una unidad entera te sale a cero con
  tamanos buenos, sospecha de la seccion antes que del codigo.
- **objdiff NO PUEDE PUNTUAR UNA FUNCION QUE EL TROCEADOR NO NOMBRO.** Empareja
  por nombre, asi que un `fn_80310750`, `gap_` o `pad_` **no puntua por bien que
  se escriba**. Antes de invertir en una unidad, cuenta cuantas de sus `.fn` son
  anonimas: en `metrotrk` son **100 de 107**, y sus «12.136 B de trabajo» son en
  realidad **596**. `censo.py` ya las filtra; el censo global cayo de 214.828 B a
  **26.668 B**, que es la cifra buena.
- **CUATRO DISCRIMINADORES DE COMPILADOR, por orden de coste:** (1) `gcc2_compiled.`
  en el `.s` = ProDG; (2) prologo con `stwu` **antes** de `mflr` = MWCC 1.3-2.7;
  (3) **`addi rD,rS,0` como copia de registro = MWCC 1.0-1.2.5n**, y `mr` =
  MWCC 1.3-2.7 (micro-banco con los **doce** compiladores del arbol);
  (4) `lis @h` + `ori @l` = asm escrito a mano, no salida de compilador.
- **`libc` SON DOS BIBLIOTECAS COMPILADAS DISTINTO, Y SE SEPARAN CON UN `grep`.**

      for f in build/GOWE69/asm/libc/*.s; do \
        echo "$(basename $f) sda21=$(grep -c @sda21 $f) ha=$(grep -c @ha $f)"; done

  **`@sda21` y cero `@ha` = viene PRECOMPILADA de SN** (su pool vive en `.sdata`,
  una sola instruccion). **`@ha` y cero `@sda21` = la compila el juego** con
  nuestros cflags (pool en `.rodata`: `lis @ha` + `@l`, dos). Prueba de que es la
  misma fuente compilada distinto: `floor` (de SN) mide **348 B** y `sn_floor`
  (del juego, dentro de `math_support`) mide **372 B** — exactamente **24 B, los
  seis `lis` de mas** de sus tres parejas de `lfd`. De doce unidades de fdlibm que
  parecian trabajo, **solo dos eran del juego** (`math_support` y `mbtowc_r`, las
  dos hoy al 100%). *(El dial en las dos direcciones —`SDA_FLOAT`/`SDA_DOUBLE` en
  las unidades de SN, `static const` en las del juego— esta en §10.)*
- **LA VERSION DE fdlibm ES NEWLIB CON `_IEEE_LIBM`** (por eso `e_pow.c` define
  `pow` y no `__ieee754_pow`), estilo `#ifdef __STDC__ static const`. Confirmado
  por los simbolos CON NOMBRE del ELF (`Zero[]`, `ln2HI/ln2LO`, `dp_h/dp_l`, `T[]`)
  y porque el pool sale en orden de primer uso del fuente de newlib.
- **TECHO DE 12,4 kB EN LA LIBM DE SN: EL SESGO DE `(float)(int)`.** El sesgo
  `0x4330000080000000` lo crea el compilador **dentro del expansor `floatsidf2`**
  (`force_reg` sobre un `CONST_DOUBLE`) y su pool va a `.rodata`, donde el
  objetivo lo tiene en `.sdata`. Medida limpia: 93,284% con **69/69
  instrucciones, tamano exacto**, y el multiconjunto de mnemonicos difiere solo
  en `lis` 3 contra 4. **La entrada completa —los ~20 flags descartados uno a
  uno, el analisis de RTL de por que escribirlo a mano tampoco vale, y las 12
  unidades afectadas— esta en §10.** *(Las constantes CON nombre si se reponen,
  con `SDA_FLOAT`/`SDA_DOUBLE`.)*
- **`-fno-expensive-optimizations` VA POR OBJETO, NUNCA POR BIBLIOTECA.** Lleva
  `sf_cos`, `sf_sin` y `sf_tan` de ~93% a **100%** (58 -> 55 instrucciones), pero
  **el mismo flag tira 14 unidades de esa misma biblioteca de 100% a 0%**. Con
  `extra_cflags=[...]` en el `Object()`: +568 B y las 14 de control intactas.
- **GUARDA ANTES DE GASTAR LA RECETA DEL `cr7`: CUENTA LOS `b* crN` DE LOS DOS
  LADOS.** Si coinciden, **el problema no es el campo de CR** y la receta no va a
  hacer nada. En `Smackable::Smackable` se atribuyo mal la causa: los dos lados
  emiten ya `cmpwi cr2` y su `beq cr2` en el mismo indice. Cuatro formas mas de la
  receta dieron **binario identico**; con las anteriores van **32 variantes**.
- **EL DESAJUSTE DE ARBOL DE `regmap` GANA AL VEREDICTO DE REGISTROS.** Tres de
  cuatro cierres de una ronda salieron de «el bloque X SOBRA/FALTA», no del
  reparto. Dos formas que lo producen: **`if (T x = f())` mete `x` en un nivel
  propio**, y **un iterador declarado en la cabecera del `for` se lleva un nivel de
  mas** (pasarlo a `while` BAJA).
- **CORRECCION: el permutador guiado CIERRA donde el playbook daba veda.**
  `cse_temp` —ligar el resultado de la llamada a un temporal y pasar **ese**,
  dejando la asignacion original como almacen muerto— cerro una funcion de
  **1.172 B** moviendo un puntero de r3 a r7; y `move_stmt` cerro otra de 132 B
  **que el playbook daba por trampa medida**. Antes de dar por cerrado un techo de
  reparto, pasa el permutador entero.
- **EL ARBOL MEZCLA FICHEROS LF Y CRLF, y las ediciones por texto fuerzan CRLF.**
  `CarLoader.cpp` es LF y `EcstasyEx.cpp` es CRLF. Un script que reescriba el
  fichero entero mete **lineas en blanco fantasma** que no se ven en el diff de
  contenido. Comprueba siempre con `git diff` contra el ultimo commit **antes** de
  dar por buena una medida.
- **CONTAR DIFFS ES MAS BARATO QUE MEDIR EL PORCENTAJE, y `scripts/pct_ecs.py` lo
  hace de todas las funciones de una unidad en UNA sola invocacion de objdiff.**
  Con la regla de «ordena por tamano, no por diffs» para elegir la funcion, pero
  «cuenta diffs» para elegir la variante, ese barrido es el bucle interior.
- **EL RACIMO `REAL` DE `callcheck` SE DISUELVE ESCRIBIENDO LA FUNCION, NO
  ARREGLANDO SIMBOLOS.** zPhysics paso de **REAL 57 a 15** con **dos funciones
  escritas y cero cambios de nombre**. El racimo y el hueco de bytes son el mismo
  trabajo: agrupa el descuadre por funcion **llamante**, no por simbolo.
- **EL SCRATCHPAD TAMPOCO ES PRIVADO ENTRE AGENTES.** No solo los `.json`: un
  agente sobrescribio el `try.py` de otro a media sesion y le mato el barrido con
  un `IndexError`. **Prefija tambien los scripts**, no solo las medidas.
- **EL DISCO LLENO NO DA ERROR: DA MEDIDAS QUE MIENTEN EN SILENCIO.**
  `objdiff-cli diff --format json` escupe **un JSON por instruccion**; en una
  unidad grande son 20-35 MB por volcado, y un solo agente acumulo **11 GB** y
  dejo C: al 100%. Entonces objdiff **no puede escribir** su JSON, el script lee
  el fichero RANCIO anterior, y sale un porcentaje plausible y falso: costo un
  informe entero con cifras equivocadas (83,2% donde la realidad era 100%) y una
  unidad que salio **en blanco sin avisar**. **Borra el volcado justo despues de
  leerlo, no al final.** Y ante un `FAILED` raro de `build_direct.py`, una unidad
  en blanco o un numero que no encaja, **mira `df -h /c` antes que el codigo**.
- **UNA UNIDAD PUEDE MEDIR 100% MIENTRAS EMITE FUNCIONES DE MAS.** `model` mide
  344/344 B al 100% y nuestro `.text` son **400 B**: emitimos `GetDepth` e
  `IsClutType` fuera de linea y el original no tiene esos simbolos en ESE objeto
  (los tiene en otro, 0x803A4180 y 0x803A41A4), asi que objdiff no los compara ni
  los cuenta. Se ve con `scripts/promote.py`, que enfrenta los tamanos de seccion.
- **FRENTE CERRADO CON MEDIDA: `ps2fn.py` NO SIRVE donde existe el `.s` de
  GameCube.** Sobre una funcion de RealShape devolvio doce llamadas en orden... que
  son **exactamente** las doce que el `.s` del troceador ya lista con nombre en sus
  `bl`. Su valor teorico —decir que esta inlinado en un lado y no en el otro—
  tampoco se materializo: lo resolvio el propio diff. **No invertir en un
  desensamblador MIPS completo.** Lo unico que PS2 aporto de verdad fue la
  IDENTIFICACION de las unidades sin fuente, que es donde conviene repetir el cruce.
- **`git checkout -- <fichero>` PARA DESHACER UN EXPERIMENTO TIRA TODO LO BUENO DEL
  MISMO FICHERO.** Se perdio un arreglo ya medido asi. **Revierte siempre con un
  Edit del bloque exacto**, nunca del fichero entero.
- **NINGUN NOMBRE DE FICHERO DE MEDIDA ES PRIVADO**: `medida.json` es compartido y
  `mi_base.json` tambien se pisa entre agentes. **Mete el nombre de la unidad**
  (`base_zEAXSound.json`) o `--cmp` dira «0 unidades cambian» mientras los totales
  discrepan.
- **UN `lbl_XXXXXXXX` EN LA FUENTE ES UN LITERAL SIN ESCRIBIR, Y EL VOLCADO DICE
  CUAL.** `Locale::GetString(msgId, lbl_804147C8, ...)` con
  `lbl_804147C8 = .rodata:0x804147C8; // size:0x2 data:string` en `symbols.txt`
  es la cadena `"s"`. Sustituirlo (mas corregir `"ssii"` por `"ssdd"`, que el DOL
  no contenia en ningun sitio) dejo el `.rodata` de `trctasks` **byte a byte** con
  el original y desbloqueo **20.176 B** de promocion. **`matched_code` no se
  movio ni un byte**: esto solo se ve en el lado de *linked*.
- **LA BASE DE UNA SECCION DE DATOS SE DEDUCE DE LAS ETIQUETAS, NO DE LOS BYTES.**
  El `.s` del troceador nombra por su direccion cada dato que el original
  referencia (`lbl_804147C8`) y nuestro `.o` trae el desplazamiento de esa misma
  referencia dentro de la seccion: **la diferencia es la base**, y con dos
  referencias ya se vota. Es exacto y no depende de que los bytes sean unicos, que
  es donde falla la busqueda directa (80 B de `gc_pad` salen en 1.260 sitios).
  Lo hace `scripts/claimdata.py`.
- **UNA SECCION QUE EMITIMOS Y NADIE REFERENCIA ES CODIGO MUERTO DELATOR.** Si
  nuestro `.o` tiene `.rodata` pero **cero reubicaciones** hacia el, esos bytes
  salen de un inline que GCC parseo y no emitio. Si el original tampoco los tiene,
  la unidad no puede promocionarse hasta quitarlos.
- **UNA UNIDAD SIN `.rodata` DECLARADO EN `splits.txt` SIEMPRE MIDE 100% EN
  DATOS**, porque `total_data` es 0 y no hay nada contra lo que comparar. **El
  100% no dice que sus datos esten bien: dice que nadie los ha mirado.** Otra
  cara del «100% que miente». 24 unidades (59.084 B) emiten un `.rodata` que **no
  esta en ninguna parte del DOL**: cadenas de un `Locale::GetString` que la
  release compila a nada, y constantes flotantes que el original pone en
  `.sdata2`. **`scripts/promote.py` lo detecta sin construir nada**; el reparto
  de datos sin duena lo resuelve `scripts/claimdata.py` por coincidencia de bytes.
- **`matched_code` y `complete_code` NO miden lo mismo.** El segundo (*linked*)
  solo cuenta las unidades marcadas `Matching`, y hay **177 al 100% sin
  promocionar** (125.764 B). Casar no basta: hace falta el mismo conjunto de
  secciones con contenido, los mismos simbolos globales, y **ninguna reubicacion
  a simbolo con sufijo de direccion** (`gcc2_compiled._80370048`), que obliga a
  promocionar el racimo entero o ninguno.
- **`scripts/ps2fn.py <simbolo>` DA EL ESQUELETO DE UNA FUNCION DESDE EL BUILD DE
  PS2, CON NOMBRES.** El Alpha 124 trae 29.142 simbolos con direccion y tamano en
  `NFS.MAP` y el codigo en `NFS.ELF`. La herramienta saca la **lista ordenada de
  llamadas directas** (`jal` de MIPS resuelto contra el mapa), que es el
  equivalente al arbol de inlines de `plan.py --dwarf-only` **para lo que el DWARF
  de GameCube no cubre**. Es MIPS: el codigo **se traduce, no se copia**.
  Ejemplo real: `SetTrafficPattern__16AITrafficManagerUi` sale con sus 14 llamadas
  en orden, incluido el `Attrib::Instance` por Key con su `_$_` y su
  `DefaultDataArea`. **Usalo antes de escribir de cero una funcion cuyo DWARF sea
  pobre**; y si el simbolo no esta en PS2, es que ahi esta inlinado o no existe.
- **LAS TRES VERSIONES, IDENTIFICADAS**: el prototipo del 21-oct-2005 es **Xbox
  360** (XEX2, PowerPC) y el PE ya descifrado esta en
  `orig/EUROPEGERMILESTONE/NFS.exe` (`machine=0x01f2`, 8,4 MB de `.text`); el
  'Alpha 124' del 20-sep-2005 es **PS2**; la USA es GameCube. El de Xbox 360
  **no tiene simbolos ni RTTI** (`-RTTI off`, igual que GameCube) — solo la ruta
  del PDB (`E:\mw\Speed\Xenon\obj\MWEuropeGerMilestone\...pdb`, que NO viene) y
  una `.pdata` de 187 KB con los limites de ~23.000 funciones. **Para saber que
  hace una funcion sirve PS2, que tiene nombres; Xbox 360 no.**
- **EL MAPA DEL ENLAZADOR DE PS2 (`orig/SLES-53558-A124/NFS.MAP`) ES UN ARTEFACTO
  NUEVO Y VALE PARA TRES COSAS.** Sale de la ISO del prototipo Alpha 124 (que es
  **PS2**, no Xbox 360) y trae 29.142 simbolos con **direccion, tamano y objeto de
  procedencia**.
  1. **Confirma que la PS2 usa las MISMAS 34 SourceLists** (`...\obj\mweuropemilestone\sourcelists\zai.cpp.obj`), asi que su estructura vale de
     referencia para la nuestra.
  2. **Da la lista de ficheros de cada biblioteca de middleware.** Cruzada con el
     arbol: a `snd` le faltan 30 nombres (casi todos `sd*`, el driver de sonido de
     PS2), a `realcore` 9 y a `realmemcard` 10 (casi todos `ps2_*`), o sea que el
     grueso de lo que 'falta' es especifico de plataforma. Los que **no** lo son
     (`author`, `slib`, `ssysserv`, `sgparse`, `sfrsf`, `eaxadecf`, `spchdata`)
     son justo objetos que `configure.py` declara **sin fuente**.
  3. **RECUPERAR EL CUERPO DE UN ACCESOR VACIO.** Si el build de PS2 emitio una
     copia FUERA DE LINEA del accesor, el mapa da su direccion y tamano y el
     cuerpo se lee del desasamblado. Cruce medido: de **546** accesores vacios con
     clase identificada, **9** tienen copia fuera de linea en PS2. Es poco, pero
     es la unica via que existe para esos. Ojo: **`NFS.ELF` es MIPS**, asi que el
     cuerpo hay que **traducirlo**, no copiarlo.
- **El prototipo del 21-oct-2005 es un XEX2 de Xbox 360 (PowerPC), y su basefile
  esta CIFRADO** (empieza en `
Ì`, no en `MZ`). Descifrarlo requiere AES-128
  con la clave de sesion del certificado; queda como via abierta. Seria la unica
  fuente PowerPC alternativa del mismo fuente.
- **`scripts/deadreg.py` AUTOMATIZA EL DETECTOR DEL REGISTRO QUE NADIE ESCRIBE.**
  Lee el JSON de objdiff de NUESTRO lado y saca, por funcion, los registros
  **leidos y nunca escritos** (excluyendo r0/r1/r2/r13, los de argumento r3-r10 y
  f1-f8). No depende de nombres, asi que no sufre la homonimia que estropea el
  barrido por nombre. **Resultado del primer barrido de todo el juego: solo
  quedaban DOS funciones** — la veta ya estaba casi cosechada, y eso tambien es
  un dato: no hay que volver a barrerla entera.
  El acierto: `EventSeqEngine` en `Libs/Support/Miscellaneous/CARP.h:253` tiene
  **cuatro metodos no-`void` con cuerpo vacio** (`GetSystemIDs`, `GetSystems`,
  `FindSystemIndex`, `FindSystem`) y las versiones **`const` de los dos primeros
  SI estan implementadas** justo debajo: el llamante no-`const` coge la vacia.
  **Cuando una sobrecarga esta vacia y su gemela `const` no, la gemela es la
  plantilla exacta del cuerpo que falta.**
- **PROMOVER UNA UNIDAD A `Matching` NO BASTA CON QUE MIDA 100%: HAY TRES
  CONDICIONES MAS, Y LA TERCERA ES ESTRUCTURAL.**
  1. **Mismo conjunto de SECCIONES.** De 178 unidades al 100% de codigo *y*
     datos, **85 tienen secciones que el objeto del troceador no tiene** — 40 por
     `.rodata`, 18 por `.sdata`. Ese contenido de mas desplaza el layout y el DOL
     sale distinto. Se comprueba sin compilar, comparando tamanos de seccion
     entre `build/GOWE69/obj/<u>.o` y `build/GOWE69/src/<u>.o`.
  2. **Sus simbolos externos tienen que resolverse.** `libc` es un grupo
     acoplado: `fopen.c` necesita `_sn_stat_g` y `_sn_iobf`, que define
     `sn_buf.cpp`. O van todas o ninguna.
  3. **LOS OBJETOS DEL TROCEADOR SE REFERENCIAN ENTRE SI POR SIMBOLOS CON LA
     DIRECCION PEGADA** (`gcc2_compiled._80370048`, `lastTick.126_804502F4`).
     Promover una unidad **retira su objeto troceado** y rompe a toda vecina que
     referenciaba esos locales. Esto acopla casi todo con casi todo: la
     promocion hay que hacerla por **racimos completos**, no unidad a unidad.
  Medido: de 178 candidatas solo **4** pasaron las tres (vp6 `decodembs`, `scale`,
  `DFrameR` y snd `saramman`), y con ellas el DOL sigue OK. Enlazado 5,2496% ->
  **5,5492%** (200 -> 204 unidades). **El resto no es trabajo de configure.py: es
  resolver los racimos.**
- **`fndiff.py` TRUNCABA A 2.000 INSTRUCCIONES Y EL AVISO IBA AL FINAL**, detras
  de 2.000 lineas, donde nadie lo veia porque todos cortamos con `head`. El
  static-init de zAI leia '2 diffs'; con limite explicito son **65**. **Ya
  corregido**: el aviso sale en la cabecera con el comando exacto para repetir.
  Afectaba a **6 funciones del juego** (las > 8.000 B), de las que solo dos
  importaban: `CarRenderInfo::Render` y el static-init de zAI.
- **UN TIPO DE MIEMBRO EQUIVOCADO QUE SOLO CAMBIA EL `_IHandle` ES INVISIBLE PARA
  OBJDIFF.** `ResetInternals` y `OnBehaviorChange` median **100%** llamando a
  `_IHandle__Q214EventSequencer7IEngine` donde el objetivo llama a
  `_IHandle__7IEngine` — el miembro era `EventSequencer::IEngine*` y es
  `::IEngine*`. Se detecta con un `grep -c` del simbolo en nuestro `.s` contra el
  del objetivo (14 contra 0). Misma familia que el `__builtin_vec_new`.
- **El desplazamiento del numero magico da el divisor sin adivinar**: `0x51EB851F`
  con `srawi 4` es `/50`; con `srawi 5` es `/100`.
- **Los ternarios de constantes se auditan con el ORDEN DEL POOL**: si el objetivo
  crea 600 antes que 375 y nosotros al reves, el ternario esta invertido. No mueve
  el porcentaje pero si el DOL.
- **`sprobe.py` REUTILIZA EL `.s` VIEJO EN SILENCIO.** Muchos `.cpp` no compilan
  solos (dependen de includes que pone la SourceList: `Attrib::Gen`,
  `g_pEAXSound`) y sprobe deja el `.s` anterior: **la medida miente sin avisar**.
  Costo una ronda entera. Lo correcto es medir sobre `build_direct` + `fndiff`
  (~25 s por variante).
- **EL BARRIDO DE CUENTA DE MNEMONICOS ES EL TRIAJE MAS BARATO QUE HAY.**
  Enfrentar el **multiconjunto de mnemonicos** por funcion nombra el opcode exacto
  que sobra: `stfs 7/8` apunto a un store duplicado (`x = A; x += B;` en vez de
  `x = A + B;`), `fmr 0/2` a temporales explicitos de mas, y `mulli 3/2 slwi 2/3`
  a una sintesis de producto mal.
- **`__builtin_vec_new` CONTRA `__builtin_new`: EL 100% QUE MIENTE, CAPITULO
  `new`.** El objetivo llama a `__builtin_vec_new` **623 veces** y a
  `__builtin_new` **19**; nuestro `zFe2.o` tiene **115 reubicaciones a
  `__builtin_new` y 33 a `vec_new`**. Con `functionRelocDiffs=none` objdiff marca
  esas funciones al **100% con el simbolo equivocado**, pero el DOL no. Medido:
  **no es el toolchain** — los cinco ProDG del arbol (3.5, 3.5b140, 3.7, 3.8.1,
  3.9.3) dan `new B` -> `__builtin_new` y `new B[1]` -> `__builtin_vec_new`
  igual. Es la FUENTE: donde el original escribio `new T[n]` nosotros escribimos
  `new T`. Para un tipo con ctor trivial `new B[1]` emite **exactamente** lo mismo
  salvo el simbolo, asi que **no es sustitucion ciega**: con ctor no trivial anade
  un bucle. Se audita contando reubicaciones a `__builtin_*` en nuestro `.o`
  contra los `bl` del `.s` del troceador. **De las 367 unidades candidatas a
  promocion a `Matching` solo UNA lo referencia, asi que no bloquea esa via.**
  **RESUELTO: el `new` del juego es un PLACEMENT-NEW SOBRE `new char[sizeof(T)]`.**
  `new (new char[sizeof(T)]) T(args)` produce **instruccion por instruccion**
  `li r3,<size>; bl __builtin_vec_new; mr r4,<arg>; bl <ctor>`, que es exactamente
  lo que emite el objetivo — prologo incluido. Descartado antes: no es el
  toolchain (los cinco ProDG del arbol coinciden con nosotros), no es `-fnew-abi`
  ni `-fcheck-new`, y no es `new T[1]` (imposible con ctor de argumentos). Y el
  objetivo emite **las dos** (623 `vec_new` y 19 `new`), asi que el front end SI
  las distingue: son dos formas de fuente distintas.
  Los dos simbolos son **envoltorios byte a byte identicos** en el ELF (ambos
  saltan a 0x8006261C), asi que la diferencia es solo a que simbolo apunta el
  `bl` — invisible para objdiff, decisiva para el enlace.
  **Es ~600 sitios de Game Code y es la razon por la que ninguna SourceList podra
  enlazar hasta que se arregle.**
  **ARREGLADO, y no era placement-new: es un `operator new` global INLINE que
  reenvia al de array.** `inline void *operator new(size_t n) { return
  ::operator new[](n); }` en `bWare.hpp` hace que **todos** los `new` del juego
  emitan `__builtin_vec_new`. Prueba definitiva: en el ELF **no hay ni un
  `__nw__FUi` global** (todos los `__nw__` son de clase), y los cuatro
  `__builtin_*` estan **consecutivos y en el mismo orden** que las cuatro
  definiciones de `bMemoryOverloads.cpp` (0x802085EC, 0x80208610, 0x80208634,
  0x80208654), con sus cuerpos exactos: `bMalloc(size, 0)` y `bFree(ptr)`.
  **GCC 2.9 NOMBRA `__builtin_new` A LA COPIA FUERA DE LINEA DEL `operator new`
  GLOBAL**, asi que el inline de la cabecera y la definicion de
  `bMemoryOverloads.cpp` chocan. En el original viven en TUs distintas; con el
  build unitario hay que desactivar el inline en esa unidad con un `#define`
  **al principio de la SourceList**, porque puesto en el `.cpp` llega tarde: la
  guarda de `bWare.hpp` ya se leyo.
  **Resultado: descuadre global 2.753 -> 2.527, `vec_delete` exacto (92=92),
  `__builtin_new` de ~115 a 1, y `main.dol: OK`.** `matched_code` no se movio ni
  una centesima, que es la prueba de que objdiff no ve nada de esto.
- **EL 86% DEL DESCUADRE DE `callcheck` ES ARTEFACTO DE MEDIDA, Y AHORA SE
  SEPARA SOLO CON `callcheck.py --real`.** `callcheck` enfrenta nuestro `.o`
  (PRE-enlace) contra el `.s` del troceador, que sale del DOL (POST-enlace), y
  el enlazador SN hace tres cosas que el `.o` todavia no. De 2.447 descuadres:
  1. **`ART_OBJ` (893, 36%): el simbolo lo DEFINIMOS nosotros, weak.** El
     enlazador original dedujo las copias weak y dejo UNA, en UN objeto; los
     demas objetos del original lo ven externo. Nuestro `.o` emite **el mismo
     `bl`**, solo que resuelto en casa, asi que no sale UNDEF y callcheck lee
     "objetivo N, nuestro 0". Caso mayor: `__8bVector3RC8bVector3`, 123 sitios
     en 23 unidades — y sin embargo `Init__19TrackCopCameraMover`, que lo llama
     6 veces, esta al **100%** en los dos lados. **No hay arreglo de fuente
     posible**: es donde el enlazador puso la copia. Misma familia:
     `SetTime__5Timerf`, `Default__Q37Physics4Info11Performance`, los
     `_GetKind__M*`, `First__...ListableSet`, `_M_increment`/`_M_decrement`.
  2. **`ART_DUP` (854, 35%): llamamos de mas desde funciones WEAK que solo
     estan en nuestro `.o`.** Son las copias fuera de linea de los inline
     in-class y las instanciaciones de plantilla; el enlazador original se
     quedo con una sola y la puso en otra unidad. Es **toda** la familia
     `FastMem::Alloc`/`Free` (693 descuadres, el 28% del total): en zFeOverlay
     el objetivo llama a `Alloc` **1** vez y nosotros **15**, y las 14 de mas
     salen de `reserve__...vector`, `clear__..._List_base` y `_M_erase__...
     _Rb_tree` — `reserve<Hermes::Handler>` vive **solo en zAI** en el
     original. Tambien los `__nw__<Clase>Ui` de las clases con `operator new`
     in-class (17 en zCamera).
  3. **`ART_STRIP` (374, 15%): llamadas desde funciones que el enlazador
     BORRO.** El test es de una linea: si el nombre de la funcion llamante
     **no esta en la symtab del ELF**, no puede estar en ningun `.s`. Los 83
     de zFe son constructores derivados (`__8COConfigb`, `__10MainCareerUiUiUi`,
     `__9RaceDatum`…) que no existen en el binario; los 18 de zLua son
     `luaL_loadfile`/`getF`/`errfile` con su `fopen`/`fread`/`strerror`; los 7
     de zDebug, `DebugGraphDataSet` entero.
  4. **`REAL` (326, 13%)**: lo unico accionable.
  **Corolario operativo: no ataques una unidad por su numero de `callcheck`;
  atacala por su columna `REAL`.** zSpeech (134), zLua (100), zEAXSound (68),
  zPlatform (33), zRender/zMiscSmall (18) y zTrack (27) tienen **REAL = 0**:
  no hay nada que arreglar ahi. La cabeza real es zMain (64), zPhysics (64),
  zFeOverlay (56), zFe2 (41), zCamera (35) y zAI (32).
- **UNA REUBICACION REL24 A UN SIMBOLO QUE NO EXISTE EN LA SYMTAB DEL ELF ES UN
  BUG SEGURO, Y SE BARRE EL ARBOL ENTERO EN UN MINUTO.** Abrir cada `.o`, sacar
  sus REL24 a UNDEF y cruzarlas contra la symtab de `NFSMWRELEASE.ELF` dio
  **71 simbolos / 87 llamadas** imposibles de enlazar, con el fichero exacto.
  Es mas fuerte que `callcheck` porque no depende de que el objetivo llame o no:
  el simbolo **no existe**. Filtra por REL24 o te ahogas en `lbl_` de datos.
  Asi salieron `ScrollView__11Scrollerinai`, `lua_dobuffer__FP9lua_StatePCcUiT1`,
  `ReInit__Q23Snd6Systemv`, `GetPosition2D__5GIconR8bVector2`,
  `FILESYS_atomic__FPFiPv_iPviT1` y los `MIX_*__F...` manglados de `snddrv`.
- **UN METODO DECLARADO Y NUNCA DEFINIDO NO DA ERROR DE ENLACE EN ESTE ARBOL, DA
  UN `bl` A LA NADA.** `Scrollerina::MoveNext/MovePrev/GetSelectedSlotIndex/
  ScrollView` y `GIcon::GetPosition2D` estaban declarados en la cabecera y sin
  cuerpo: como las SourceLists son `NonMatching` el DOL sigue casando y nadie se
  entera. El objetivo dice que **no existen**: `MoveTrack` emite
  `bl MoveSelected__11Scrollerina10eScrollDirb` con `li r4,-1/1` y `li r5,1`, y
  `UpdateIconElement` expande `lfs 0x10/0x14` -> `stfs`. O sea: eran **inline
  in-class**, y el arreglo es escribir el cuerpo dentro de la clase.
- **`extern "C"` SE COMPRUEBA SIMBOLO A SIMBOLO CONTRA EL ELF, Y LOS DOS
  SENTIDOS APARECEN EN EL MISMO ARBOL.** `filesys.h` declaraba las `FILESYS_*`
  `extern "C"` porque las TU de `snd` las necesitan sin manglar, pero `bFile.cpp`
  es C++ y el original llama a `FILESYS_close__FiiPv`, `FILESYS_read__FiiPviiT2`,
  `FILESYS_completeop__Fi`… El header ya traia el interruptor
  (`REALCORE_FILESYS_CXX_LINKAGE`) y **nadie lo definia**: definirlo en
  `bFile.cpp` valio **46 descuadres de golpe** (zMisc 94 -> 48) con
  `matched_code` clavado al byte. Al reves en Lua: `lua.h` tiene su bloque
  `extern "C"` y **`lauxlib.h` no**, asi que `GHandler.cpp` llamaba a
  `lua_dobuffer__FP9lua_StatePCcUiT1` en vez de `lua_dobuffer`.
  **Y el `Tn` del manglado GCC 2.9 numera TIPOS DISTINTOS, no parametros**:
  `FILESYS_read__FiiPviiT2` es `(int,int,void*,int,int,void*)` — `T2` es el
  segundo tipo distinto (`Pv`), no el segundo parametro. Verificado compilando.
- **UN TIPO EN EL NAMESPACE EQUIVOCADO SOLO SE VE EN EL MANGLADO DE QUIEN LO
  RECIBE.** `Shape` estaba dentro de `namespace RCMP` en `MoviePlayer.hpp` y el
  ELF dice `RealShape::Shape`: `PlatSetFirstMovieFrame__FP11TextureInfoP5Shapeb`
  y `FillInTextureInfo__11MoviePlayerPUiP11TextureInfoPQ24RCMP5Shape` contra los
  `PQ29RealShape5Shape` del objetivo. **Trampa del build unitario**: mover el
  tipo al namespace bueno choca con la definicion minima que ya tenia otro
  `.cpp` de la misma SourceList (`Movie_GC.cpp` en zPlatform) — hay que
  guardarla con el `#ifndef` de la cabecera grande.
- **`bOMalloc(SlotPool*)` Y `bMalloc(SlotPool*)` SON DOS ENVOLTORIOS DISTINTOS,
  Y `USE_SLOTALLOC` USA EL EQUIVOCADO PARA ALGUNAS CLASES.** Los dos miden 32 B
  y solo se distinguen por el destino de su `bl`, asi que objdiff los da por
  iguales. Se barre con un `awk` de `bl bMalloc__FP8SlotPool` sobre los `.s` del
  troceador: solo **6 sitios** en todo el juego, y son `AStarNode` (zWorld2),
  `TexturePack` y `TextureAnimPack` (zEcstasy) y `SkidSet` (zTrack). El resto
  del juego si usa `bOMalloc`.
- **LA CLASE-COMO-NAMESPACE VUELVE A APARECER, Y EL PARCHE ES `__asm__`.**
  `Snd::System` es un `struct` de estaticos en `ssys.cpp` y un `namespace` en
  `EAXSND8Wrapper.cpp` y en los dos `.cpp` de `egami`: `ReInit()` e `IsInited()`
  no llevan parametros, asi que salian `...Q23Snd6Systemv` y el ELF dice
  `...Q23Snd6System`. El fichero **ya usaba** el truco para `VectorToCsisMutex`;
  basta con extenderlo (`int ReInit() __asm__("ReInit__Q23Snd6System");`).
- **UN `bl` CON ARGUMENTOS DE MAS SE LEE EN EL MANGLADO ANTES QUE EN EL DIFF.**
  `FEngRender.cpp` declaraba `extern void GCDrawMovie(FEObject *, FERenderObject *)`
  y el ELF tiene `GCDrawMovie__Fv`. Al arreglarlo salio ademas la condicion:
  el objetivo hace `cmpwi r0,0x5; bne` y nosotros
  `static_cast<unsigned int>(GetStatus()-3) < 3`. Dos lineas, `RenderMovie`
  de 75,33% a 100%.
- **UNA 'REGRESION' REPORTADA POR UN AGENTE QUE VIGILA UNA UNIDAD AJENA SUELE SER
  SU PROPIO ESTADO SIN COMMITEAR.** Un agente reporto
  `TrackStreamer::DetermineStreamingSections` cayendo de 85,79% a 81,64% 'sin
  tocar el fichero'. **No existia**: la funcion llevaba 81,638% toda la sesion,
  comprobado con tres reconstrucciones (la cabecera sospechosa, las **76
  cabeceras** cambiadas de `src/Speed/`, y el `.cpp` mismo) al estado de partida —
  las tres dan el mismo numero al cuarto decimal. Y la unidad habia **subido**
  (84,61% -> 85,89%). **Antes de auditar 96 cabeceras, reconstruye el estado de
  partida y mide**: cuesta tres compilaciones y descarta el frente entero.
- **Un bucle de restauracion que revienta a mitad DEJA FICHEROS PISADOS.** El mio
  fallo en el septimo fichero (no existia, diferencia de mayusculas en la ruta) y
  los seis anteriores se quedaron en la version vieja; el segundo intento piso sus
  copias de seguridad. Se arregla con `git checkout HEAD -- <ficheros>`, pero la
  regla es: **filtrar y comprobar existencia ANTES de escribir el primero**.
- **EL REGISTRO QUE NADIE ESCRIBE DELATA EL ACCESOR VACIO, Y ES UN BARRIDO
  AUTOMATICO.** `stfs f0, 0x4c(r30)` con `r30` **jamas escrito en toda la
  funcion** es la firma inconfundible de un inline con cuerpo vacio: GCC inlina
  'nada', el pseudo nace indefinido y reload le da cualquier registro. Detector:
  por funcion, un GPR **leido y nunca destino**, excluyendo r1/r2/r13 y r3-r10.
  **704 B en una pasada.** Convierte `docs/analisis/accesores-vacios.md` de
  ranking a lista exacta: en vez de adivinar por numero de llamantes, se barre el
  desasamblado y salen las funciones REALMENTE rotas.
- **Un `int` local que copia un miembro `bool` cuesta la funcion entera, y se
  detecta contando opcodes.** `int x = s->Flag; x = !x;` da el `subfic`/`adde`
  generico; el `xori rN,rN,1` del objetivo prueba precision 1. Cambiar **solo el
  tipo declarado** llevo `COConfig::Act` de 81,26% a **100%**. Triaje: contar
  `xori rX,rX,0x1` contra `subfic rX,rY,0x0` en los dos lados de cada near-miss.
- **UNA VEDA DE 'PERMUTADOR AGOTADO' ES FALSA SI `regmap` DICE ESTRUCTURA.** En
  `GameDevice::PollDevice` las **78 variantes** del permutador corrian contra el
  **arbol de bloques equivocado**: el DWARF pone las dos locales a nivel de
  funcion y nosotros teniamos un `{ }` envolviendo el bucle. Quitarlo lo dejo en
  **2 diffs con tamano exacto**. Antes de dar una funcion por techo de reparto,
  comprobar que los BLOQUES coinciden.
- **`regmap --scan` es el triaje mas rentable, y su receta es literal**: escribir
  la local que el DWARF pone en un bloque que nos falta, **aunque sea muerta**.
  `IOModule::UpdateAllDevices` cerro escribiendo un `InputDevice *device` muerto
  dentro de un bucle vacio — cero instrucciones emitidas. Dos de cuatro cierres de
  una ronda salieron de ahi.
- **El orden de declaracion de dos locales del mismo tamano se lee de las RANURAS
  del DWARF**, y vale puntos enteros: intercambiar `offset` y `dim` (0x8 y 0x18 al
  reves) valio **13 puntos** en `Collection::AddNode`.
- **`medida.json` ES COMPARTIDO Y CUALQUIER `measure.py <unidad>` LO PISA.** Con
  varios agentes en paralelo, una instantanea global guardada ahi dura lo que
  tarde el siguiente en medir su unidad, y entonces `--cmp` dice **'0 unidades
  cambian'** mientras los totales no cuadran. Es un falso negativo silencioso.
  Para comparar de verdad hay que escribir a un fichero propio:
  `python scripts/measure.py -o <mi_instantanea>.json`.
- **`plan.py --dwarf-only` ES EL PLANO DE LA FUNCION, Y VA ANTES DE MIRAR EL
  DIFF.** La lista de locales del volcado es el **orden de declaracion del
  fuente**, y el arbol de inlines es la **lista exacta de llamadas**. Reescribir
  la funcion a esa lista cerro **8 de 13** en una ronda. Deja de ser una consulta
  puntual: es el primer paso.
- **ACCESORES VACIOS: 597 no-`void` con cuerpo `{}` en 154 cabeceras, 110 con
  llamantes — PERO EL RANKING POR NOMBRE ES RUIDO Y LA VETA ESTA CASI AGOTADA.**
  Cada accesor vacio devuelve basura y **ninguna funcion que lo llame puede
  casar**, asi que la veta es real y esta cruzada. Lo que cambio con la medida
  es **como se busca**:
  1. **NO por nombre.** Los cabezas del ranking son falsos positivos:
     `GetNext`/`GetHead`/`GetPrev`/`GetTail` de `FEList.h` son de
     `FEHashNode`/`FEHashList` y **no hay un solo uso de esas clases en todo
     `Frontend/`**; los 516 y 422 «aciertos» son `FEMinList::GetHead`,
     `bList::GetHead` e `IconPanel::GetHead`, **ya implementados**. Cruzado y
     medido contra los 88 `.cpp` de zFe2+zFeOverlay: **cero bytes por esa via**.
     Los nombres muy usados (`bMin`, `bMax`, `bLength`, `bDot`) tienen varias
     sobrecargas de las que **solo algunas** estan vacias.
  2. **SI por FIRMA (tipo de retorno).** En EAXSound salen **44** por firma, de
     los que **5 se llaman de verdad**, y uno (`SndCamera::GetWorldCamVel`)
     valio **840 B de un tiron**.
  3. **Y por el REGISTRO QUE NADIE ESCRIBE** (`deadreg.py`), que no depende de
     nombres: **704 B en una pasada**, y hay que correrlo tambien sobre
     `f9-f31` (`fabs f1,f24` con f24 jamas destino es un accesor **flotante**
     vacio: 1.788 B).
  **Estado medido de la veta**: en zEAXSound+zEAXSound2, `deadreg` sobre las 69
  funciones sub-100% da **cero**, y con `--all` sale **un** falso positivo; el
  barrido por firma da 54 accesores vacios en 13 cabeceras de los que ninguno
  paga (los `operator new`/`delete` de `EvtSystems`/`BankSlotSystem`/
  `SndDataParams` **no los usa nadie**, `EAX_CarState::GetShiftUpRPM/
  GetShiftDownRPM/GetCarType` no tienen llamante, y el unico que si —`GetYaw()`
  en `CARSFX_Skids`— esta en una funcion que ya mide 100%). **Veta cerrada con
  medida en esas dos unidades y en zFe2+zFeOverlay.** El ranking completo sigue
  en `docs/analisis/accesores-vacios.md`, y se regenera con un barrido de
  definiciones cuyo cuerpo es `{}` cruzado con `\bNOMBRE\s*\(` en los `.cpp`
  (ese barrido es el que da los falsos positivos del punto 1: usalo para
  enumerar, no para decidir).
  **Coordinacion**: casi todos viven en cabeceras compartidas, asi que esto es
  trabajo de una ronda dedicada, no de un agente entre otros seis midiendo.
- **HAY 85 OBJETOS DECLARADOS EN `configure.py` SIN FUENTE EN EL ARBOL, y ninja
  ni lo intenta: 25.800 B de codigo del objetivo en territorio frio.** No salen
  en ningun ranking de near-miss porque no hay `.o` nuestro que comparar. Los
  mayores: `creates.cpp` (3.320 B, 19 fns), `ppc2D2.tmp` (2.448 B, 22 fns, son
  helpers del runtime), `qsort.c` (2.444 B, **una** funcion, newlib),
  `interfaceimp.cpp` (896 B), `audioplayer.cpp` (780 B). Se listan con un barrido
  de `Object(...)` de `configure.py` contra `os.path.exists`. **Aviso: ninguno
  tiene DWARF**, asi que `plan.py` y `fuse.py` no sirven; se trabaja con el `.s`
  del troceador y la tabla de simbolos.
- **ANTES DE PELEARSE CON UNA UNIDAD AL 0%, COMPROBAR QUE EL COMPILADOR ES EL
  QUE CREEMOS.** Tres discriminadores, los tres validados sobre 200 unidades:
  1. **`gcc2_compiled.`**: presente en **100 de 105** unidades GCC del juego y en
     **0 de 95** Metrowerks. Un `.s` sin ese simbolo **no es GCC**, y escribir C
     con `ngccc` para el no puede casar nunca.
  2. **Los nombres del runtime**: `__div2i`/`__mod2i`/`_savegpr_N` = Metrowerks;
     `__divsi3`/`__modsi3` = GCC.
  3. **El mangling de los estaticos de funcion**: `nombre$NNN` = MWCC;
     `nombre.NNN` = GCC.
  Con eso se descubrio que **tres unidades de LibSN estaban con el compilador
  equivocado en `configure.py`**: cambiarlo dio +2.192 B y +26 funciones, con las
  15 primeras de `vmbase` casando **a la primera compilacion**.
- **La version de CodeWarrior se lee del PROLOGO**: 1.0-1.2.5n emiten
  `mflr r0; stw r0,4(r1); stwu`; **1.3-2.7 emiten `stwu; mflr r0; stw r0,frame+4`**.
  Y no tiene por que ser la misma en todo el juego: el VM es de la segunda
  familia y el resto de librerias Dolphin de la primera.
- **Las entradas `STT_FILE` del ELF dan el FICHERO FUENTE de cada objeto sin
  DWARF.** Resuelven la identidad de una unidad de middleware: confirmaron que
  toda nuestra `libc` es **newlib** (`strcpy.c`, `memcpy.c`, `qsort.c`,
  `vfprintf.c`) y dieron el nombre real detras de los temporales de SN
  (`ppc2D2.tmp` -> `ppcdown.c`, `fil2D3.tmp` -> `fileserver.c`).
- **COMPARAR EL VALOR DE CADA CONSTANTE, NO SOLO EL CONTEO DEL POOL.** El chequeo
  de conteo no ve un literal cambiado. Leyendo los `lbl_` del ELF contra nuestros
  `$LC` del `.rodata` salieron **7 literales mal** en una sola unidad (`1.0f`
  donde iba `10000.0f`, `0.0f` donde iba `1000.0f`...). Tres llevaron un ctor de
  90,36% a 99,83%; **los otros cuatro no mueven el porcentaje pero SI el DOL**.
- **UN GENERADOR DE CONJUNTOS DE `case` RESUELVE UN ARBOL EN UN MINUTO.** Compilar
  con `ngccc -S` un `switch` sintetico por conjunto candidato y comparar la
  secuencia de constantes de `cmpwi`/`cmplwi` seguidas de `bc`. Con 40 conjuntos
  localizo los DOS `case` que faltaban en `ProcessResponses`, **uno de ellos fuera
  del rango emitido** (cualquier valor > el mayor `case`, o negativo, da la misma
  secuencia y por eso no aparece en el desasamblado). Ojo al parsear: el `.s` de
  ProDG escribe `cmpwi 0,3,263` con el campo CR y `bc 12,2,.L15`, no `cmpwi r3,0x107`.
- **LA SECCION `Labels:` DEL VOLCADO DWARF DICE QUE EL ORIGINAL USA `goto`, Y DA
  EL NOMBRE DE LA ETIQUETA.** En `DeserializeTable` aparecia
  `LabelDeserializeFailed: // 0x801BE124`, y los seis saltos que parecian el
  `default` de un `switch` eran seis `goto` a esa etiqueta. Sin eso la funcion se
  reconstruye con la estructura equivocada.
- **El volcado DWARF trae el TIPO DE RETORNO, que el mangling esconde.** Nuestra
  cabecera decia `unsigned short SerializeTable(...)` y el original es
  `unsigned int`: mismo simbolo, mismo enlace, **otro codigo**. Merece un barrido
  comparando el retorno declarado con el del volcado, unidad por unidad.
- **Un `bl` a un simbolo que no esta declarado en ningun sitio de nuestro arbol
  significa que la CABECERA perdio la declaracion, no que la funcion no exista.**
  `lua_userdatalen` estaba definida en `lapi.c` y sin declarar en `lua.h`;
  `UMath::IsNaN` existe en 0x801926F8 (48 B), justo detras de `UMath::Mod`, y el
  volcado da su firma exacta.
- **EL ORDEN DE CREACION DEL POOL DE CONSTANTES ES UN DIAGNOSTICO, Y ES GRATIS.**
  El pool es **por funcion** (por eso el objetivo tiene varios `0.0f` distintos
  en el mismo TU) y se crea **en orden de aparicion en el fuente**. El del
  objetivo sale de las *direcciones* de los `lbl_` en el `.s` del troceador; el
  nuestro del *numero* de `$LCn` en el nuestro. Tres lecturas:
  1. **mismo conjunto, otro orden** -> falta una **referencia temprana**. Un
     inicializador muerto no emite ni una instruccion pero **si crea la entrada
     del pool**; eso cerro una funcion de 300 B.
  2. **al objetivo le sobra una constante** -> hay **codigo sin escribir**, y su
     **valor dice cual**.
  3. **nos sobra a nosotros** -> el original lo expresa de otra forma.
  **Trampa**: hay que mirar **todas** las referencias del rango de la funcion, no
  solo las de `lfs`/`lfd` — filtrar por opcode perdio 5 de 14 constantes.
  Barrido completo de dos unidades: solo 4 funciones tienen el orden mal, asi que
  como triaje es de una sola pasada.
  **ACOTADO, y en un punto CORREGIDO, por un barrido posterior de tres unidades:**
  1. **Arreglar el orden NO puntua.** En las tres funciones donde se dejo el pool
     perfecto el resultado fue **0 bytes**, y en dos **empeoro** (208 -> 268 diffs,
     127 -> 144). Con `functionRelocDiffs=none` objdiff **no ve** a que `$LC`
     apunta cada carga. El pool sirve para **localizar** la sentencia que falta y
     para `matched_data`; **no es una palanca de porcentaje**.
  2. **`extern const float lbl_XXXX` NO crea entrada de pool.** Contar esas
     referencias UNDEF **invierte el veredicto**: hacia parecer que al objetivo le
     sobraba una constante (lectura 2, 'falta codigo') cuando era la lectura 1
     (mismo conjunto, otro orden). Hay que filtrarlas antes de comparar.
  3. Como **triaje** si es de una sola pasada, y barato: 8 de 260 funciones en
     zEcstasy, 6 de 185 en zWorld, 1 de 204 en zWorld2.
  Donde si acerto: predijo **dos inicializadores muertos** en `eDataRender::Render`
  (los dos ciertos, binario identico) y la **linea exacta** de una sentencia que
  faltaba en `HolePunchAvoidables`.
- **El valor exacto de una constante que le sobra al objetivo dice de que
  EXPRESION viene.** `0.19999999` (`0x3E4CCCCC`) **no** sale de escribir `0.2f`
  (que es `0x3E4CCCCD`): solo sale de **`1.0f - 0.8f` plegado**. Con eso se
  identifica la sentencia que falta, no solo que falta algo.
- **Para reconstruir un `__static_initialization_and_destruction_0`, la llave es
  `debug_lines.txt`, NO el DWARF.** La init estatica lleva **la linea de fuente
  de cada store**, asi que el mapa dice directamente **en que `.cpp`/`.hpp` y en
  que linea esta declarado cada global** y **cuantas sentencias tiene cada
  constructor** — incluso cuando los siete stores de un miembro caen en una sola
  linea fisica. Con eso, los 4.544 B de un static-init dejan de ser un muro y
  pasan a ser una lista. Ejemplo trabajado en
  `docs/analisis/zEcstasy-static-init.txt`.
- **Y distingue lista de inicializacion de asignacion**: un `bVector3` temporal
  de pila materializado UNA vez para doce objetos (por CSE) prueba que el ctor
  **asigna** en el cuerpo, no inicializa en la lista.
- **Para una familia de funciones hermanas, el MAPA DE LINEAS gana a `regmap`.**
  Las cuatro `DoWheelForces` (10.908 B) las cerro `fuse.py`, no el reparto de
  registros: el original tiene el producto entero **en una sentencia** y las dos
  variables auxiliares **antes**. Partirlo en un intermedio permutaba f10/f11/f12.
- **`regmap.py` y `permuter.py` eligen mal la sobrecarga**: diagnostican la
  primera definicion del nombre. `permuter.py` acepta `--overload N` (1-based) y
  `--symbol <mangled>`; **`regmap.py` no tiene equivalente** y hay que sacar la
  sobrecarga del volcado a mano.
- **`lreg.py` da el NUMERO exacto que hay que mover, y eso ahorra la ronda de
  variantes entera.** Para una funcion atascada dio prioridad 2400 contra 833
  entre dos locales: el arreglo no era "probar formas", era **subir `n_refs` de
  la que en el objetivo va primero hasta 5, o bajar la otra a 3**. Correrlo
  *antes* de barrer formas es lo que lo hace rentable — pero solo cuando ya
  sabes que el techo es el asignador (vease la entrada sobre `regmap` arriba).
- **`scripts/libdwarf.py struct <fichero> <Struct>`** convierte el volcado DWARF
  en **C compilable**: hoista los struct anonimos a typedefs deduplicados, los
  ordena topologicamente e invierte las dimensiones de los arrays. **El volcado
  las invierte tambien en las LOCALES que imprime `plan.py`**: decia
  `int joinedfaces[2][3]` y la aritmetica del objetivo (`best*8 + pi*4`) probaba
  que era `[3][2]`; valio 2,5 puntos. `libdiff.py`
  compila y mide una unidad de biblioteca **sin darla de alta**, y con `--lines`
  da el desensamblado con la linea de fuente de cada instruccion.
- **`regmap.py` diagnostica; `lreg.py` solo confirma.** `regmap` enfrenta local a
  local el reparto del original contra el nuestro **con los nombres que el
  original les puso**, y en zDynamics cerro cuatro funciones. `lreg.py` (tabla de
  allocnos via cc1plus -dl) **sirve donde YA SABES que el techo es el asignador**;
  para diagnosticar no, y sus pseudos flotantes no salen con clase FLOAT_REGS.
- **El mapa de lineas ordena DECLARACIONES, no solo sentencias.** Reestructurar
  una funcion por el orden de `debug_lines.txt` la llevo de 80,9% a 99,93%.
- **Los cflags de cada biblioteca de middleware se LEEN del asm, no se copian.**
  `configure.py` los tenia copiados en bloque de `cflags_snd` -su propio
  comentario lo admitia- y estaban mal en dos bibliotecas. Los cuatro
  delatores:

  | lo que ves en el asm | lo que significa |
  |---|---|
  | manglado `Uw` (unsigned wchar_t) | `-fshort-wchar` |
  | `@sda21(r0)` en los estaticos | **sin** `-G0` |
  | acceso directo sin materializar la direccion en un salvado | **sin** `-fforce-addr` |
  | bucles que avanzan punteros (`addi rN,rN,stride`) en vez de `slwi`+`lwzx` | `-fstrength-reduce` |

  **Cuando sospechar:** codigo ya escrito **y** porcentaje bajo. `path` estaba
  al 17% con 14 ficheros escritos: quitarle `-G0` y `-fno-strength-reduce`
  valio 1.852 B y 10 funciones sin tocar una linea. `realmemcard` igual, y
  llevo `memcard_interface` de 67% a 100%. **Pero se mide en las dos
  direcciones**: `spch` estaba al 75% y esos mismos flags le cuestan 9.152 B.
  Una biblioteca que ya casa bien NO es sospechosa.
- **`_vt.<clase>` se lee del ELF y da el orden de vtable CON NOMBRES.** Mejor
  fuente que el volcado de PS2, que ademas puede ir corto: `Realmc::Interface`
  tiene cinco virtuales mas en GameCube. Se resuelve cada `pfn` contra la
  symtab. La ranura N esta en el desplazamiento 8*N, con la entrada 0 nula y el
  destructor en la 1 -o sea `slot = 8*(indice+1)` contando el dtor como indice
  0-. Si choca con la aritmetica COM de la seccion 8, manda el ELF.
- **El nombre del `.o` no es el del `.cpp`: cruzar por basename pierde CUs.**
  `gc/public.s`, `gc/tasks.s` y `gc/trctasks.s` parecian no tener DWARF y lo
  tienen entero, bajo `gc_public.cpp`, `gc_tasks.cpp` y `gc_trctasks.cpp`. Se
  confirma cruzando el `Code range` del CU con el rango del `.s`.
- **La lista de funciones del `.o` NO es la del DWARF.** El enlazador SN quita
  las no llamadas: las que el volcado marca `Range: 0xFFFFFFFF` **no hay que
  escribirlas** (14 de 49 en un fichero).
| herramienta | qué da |
|---|---|
| `scripts/fuse.py <unidad> "Clase::Metodo"` | desensamblado + **línea de fuente**. **Si `plan.py` no encuentra la función, prueba aquí**: hay TU de los que el volcado DWARF sólo trae los inlines, pero `debug_lines.txt` los cubre enteros |
| `scripts/plan.py <unidad> "Clase::Metodo"` | desensamblado + **línea de fuente** + **árbol de expansiones inline con rangos**; `>` `<` abren y cierran inlines, `{` `}` bloques anónimos, y lista las locales con **registro o ranura de pila**. `--dwarf-only`, `--raw` |
| `scripts/regmap.py <unidad> "Clase::Metodo"` | enfrenta local a local el registro del original contra el nuestro y **nombra la causa**. `--scan --min 90 --minsize 64` triaja la unidad |
| `scripts/permuter.py <unidad> "Clase::Metodo" --guided --sweep` | busca la forma de fuente automáticamente, **guiado por regmap**. `--timeout` mayor en unidades lentas |
| `scripts/derotate.py <unidad> "Clase::Metodo"` | lee el orden de sentencias del original desde `debug_lines.txt` |
| `scripts/missinline.py unit <u>` / `func <fn>` | inline que el original expande y no tenemos, por impacto; `func` da la receta |
| `scripts/missingcalls.py` | símbolos que el objetivo llama y no definimos = **código sin escribir, con nombre** |
| `scripts/symtabdiff.py`, `vtable_audit.py --klass X`, `unitmap.py --cmp`, `loss.py`, `rodata.py` | |

**Fuentes de datos**

- `symbols/mw_dwarfdump.nothpp` (105 MB): clases, locales con registro, **árbol
  de inlines con rangos**, bloques anónimos. 163.904 rangos, 643.106
  expansiones, 40.256 locales con registro.
- `symbols/debug_lines.txt`: línea de fuente de cada instrucción. **Sólo anota
  cuando cambia la línea**: hay que heredar la anterior.
- **`orig/GOWE69/NFSMWRELEASE.ELF` es legible**: valor de cada `lbl_803Cxxxx`,
  tamaño y bytes de cualquier símbolo. **Se acabó adivinar constantes.**
- `build/GOWE69/asm/.../<unidad>.s`: el `.data` trae las tablas estáticas.
- `symbols/PS2/PS2_types.nothpp`: **7.506 anotaciones `/* vtable[N] */`** con el
  índice de cada virtual. El volcado de GameCube trae **cero**.

**EL FRENTE MAS BARATO QUE QUEDA: LAS FUNCIONES DECLARADAS Y NUNCA DEFINIDAS.**
Tres censos de dos minutos y ninguno necesita compilar:
1. **Simbolos del objetivo sin CUERPO nuestro.** Abrir el JSON de `objdiff-cli
   diff` de la unidad y quedarse con los simbolos del lado *left* con tamano cuyo
   gemelo del lado *right* no tiene `instructions`. `fndiff` los delata tambien:
   imprime `size=NNNN/None` y `ours=None`.
2. **`grep -rn "// unfinished" src/`** — los stubs que dejaron rondas anteriores.
3. **`scripts/missingcalls.py`** por columna de `bl` ausentes.
**Rendimiento medido en una sola ronda: 5 funciones escritas, 4 al 100% en la
PRIMERA o segunda compilacion** (`CameraMover::MinGapTopology` 1.316 B,
`GameDevice::ReportCollision` 1.336 B, `ActionQueue::IO_UpdateFromDevice` 688 B, y
`ActionQueue::FetchCurrentValues` 488 B al 98,16%). La receta es siempre la misma:
`plan.py <unidad> "<Clase::Metodo>"` da el desensamblado con **la linea de fuente**
y el **arbol de inlines con las locales y su ranura**, y con eso la funcion se
*transcribe*. Comparado con pelear near-misses del 99%, el rendimiento por hora no
tiene competencia.
- **Y ESCRIBIR EL OVERRIDE QUE FALTA ARREGLA LA VTABLE DE PROPINA.** Un `_vt.X.Y`
  que mide <100% **con cero diffs** no es un problema de datos: es una ranura que
  apunta a un simbolo que no existe. `GameDevice` no declaraba el override de
  `IFeedback::ReportCollision`; escribirlo dio 1.336 B de codigo **y 136 B mas de
  `_vt.10GameDevice.9IFeedback`** sin tocar nada de la vtable.

---

## 2. Trampas de medición

- **`objdiff-cli diff` CON UN SIMBOLO Y SIN `-o` ABRE LA TUI Y SE CUELGA.** Costo
  una tanda de 120 s. Pasa siempre `-o <fichero> --format json`.
- **zFeOverlay RENOMBRA `.text` A `.over`**: sin pasarle `rename_section.py` al
  `.o` del banco de pruebas, objdiff **no empareja** y devuelve
  `match_percent: null` **sin error**. Parece un 100% cuando no ha comparado nada.

- **CON VARIOS AGENTES, UN `ninja` ENLAZA CON LOS `.o` DE MIDDLEWARE QUE OTRO
  ESTA ESCRIBIENDO EN ESE INSTANTE.** Medido: el DOL fallo el checksum con
  `d24840e8...` porque otro agente reconstruyo `build/GOWE69/src/libc/*.o`
  **cinco segundos antes** del enlace, y `k_rem_pio2.o` **si entra en el enlace
  desde `src/`**. Las 33 SourceLists entran desde `obj/` --son NonMatching-- pero
  el middleware promocionado no. **Si el DOL falla, mira la hora de los objetos
  antes de bisecar.**
- **PROMOCIONAR NO ES GRATIS AUNQUE LA UNIDAD ESTE AL 100%.** Las cinco unidades
  de libm cerraron al 100% y `trypromo.py` dice **DOL ROTO en las cinco**, una a
  una. `eaxadecf`, tambien al 100%, dice **DOL OK**. La unica forma de saberlo es
  `scripts/trypromo.py <unidad>`; editar `configure.py` a ojo rompe el arbol para
  todos los agentes en vuelo.

- **CONGELA EL DIFF, NO EL PORCENTAJE.** `pctsnap.py` solo mira el `%`, y una
  funcion puede cambiar de codigo sin moverlo. `scripts/frozen.py` guarda el diff
  ENTERO de una unidad y comprueba que sigue igual, asi que caza **cualquier**
  cambio. Es el arnes que jferr ya usaba en `scratchpad/phase13_refresh_and_check.py`.
- **AL COMPARAR DOS DIFFS, QUITA `line_number` PRIMERO.** Cambia en cuanto
  anades un comentario, y sin quitarlo cada nota que escribes hace que el objeto
  "difiera" sin que cambie una sola instruccion. `frozen.py` lo hace solo.


- **EL PORCENTAJE NO MIDE AVANCE; EL CONJUNTO DE DESPLAZAMIENTOS `(r1)` SI.**
  Medido: aplicando dos correcciones reales a la vez --el tamano del bloque muerto
  y la asociacion del producto-- **todas y cada una** de las ranuras de pila de la
  funcion pasaron a coincidir con el objetivo y todos los `fmuls` a emparejarse, y
  objdiff **BAJO** de 97,18% a 94,97%. Para juzgar si vas bien, mira `faltan/sobran`
  y la coincidencia de ranuras, no el `%`.
- **SONDEAR EL TAMANO DE UN BLOQUE MUERTO ACOTA LO QUE FALTA.** Con `char zz[N]`
  detras de la ultima local conocida: N<=8 no mueve el marco, **N=9..16 da el marco
  exacto del objetivo**, N>=17 se pasa. Eso dice **cuantos bytes** faltan y **donde**
  --y con N=12 casaron las quince ranuras--, sin tener que adivinar la declaracion.
  *(No la dejes puesta: es una local inventada.)*


- **EL BANCO DE PRUEBAS TIENE DOS FALLOS SILENCIOSOS, Y LOS DOS SE LEEN COMO
  UN ACIERTO.** (1) Contra un `.o` **suelto**, objdiff devuelve a veces
  `match_percent = None`, y entonces `mn_diff` imprime las dos columnas **sin
  marcar ninguna diferencia**: identico a un acierto. (2) `mn_repro` por stub
  reproduce el objeto de ninja en unas unidades y en otras no -- `zFe.cpp` sale
  **byte a byte** (172.044 B, 914/921 fn) y `zFeOverlay.cpp` da **0 de 141.224
  B** con el objeto real al 97,91%. **Barre con el banco, cierra SIEMPRE contra
  el `.o` de la unidad.** Misma familia que el disco lleno: si un barrido entero
  sale neutro, sospecha de la herramienta antes que de la fuente.

- **EL COSTE DE UN CFLAG HAY QUE MEDIRLO EN LA UNIDAD DONDE VIVE LA FUNCION.**
  Yo tenia apuntado el coste de `-fno-cse-skip-blocks` sobre zFe2 y
  `PushErrorPackage` vive en **zFe**. Medido donde toca: 172.044 -> 132.024 B,
  **-40.020 B**. El veto se sostenia igual, pero por el numero equivocado.


- **UN REPRO PUEDE SER REPRESENTATIVO Y AUN ASI LLEVARTE AL SITIO EQUIVOCADO: LO
  QUE HAY QUE MIRAR ES SI PRODUCE EL CODIGO DEL OBJETIVO, NO SI «SE PARECE».** Un
  agente verifico con `ngccc` que la forma *loop-and-a-half* «si produce el par
  `cror/bso` del objetivo» y se repartio como grieta en una veda de 4.396 B.
  Aplicada en la funcion real, `Add__6RBGrid` cayo de **99,301% a 88,272%** y
  perdio 48 B: no da el `b` incondicional, da el bucle **sin rotar**, que son 3
  instrucciones menos por bucle. Y el repro lo habia predicho (n=350 contra 362)
  -- nadie miro el conteo. **Antes de propagar una grieta, aplicala en la funcion
  real y mide.**
- **«ESE COMENTARIO ESTA RANCIO» ES UNA AFIRMACION QUE HAY QUE MEDIR, NO CREER.**
  Un agente reporto que el comentario de `Smackable.cpp` ya no se cumplia y se
  metio en el brief siguiente como correccion. Remedido: `if (simple_physics ==
  true)` da **99,2615%** y `if (simple_physics)` **98,0154%** con el marco de 0x150
  a 0x158 y la variable derramada a `0xd4(r1)`. El comentario tenia razon y
  «corregirlo» habria costado **1,25 pp en una funcion de 3.120 B**. Mecanismo: en
  la arista falsa de `x == 1` GCC no registra equivalencia con constante; en la de
  `x != 0` si, y esa equivalencia alarga el rango de vida.

- **EL MULTICONJUNTO DE MNEMONICOS NETEA LOS BLOQUES MOVIDOS: LA PARTICION
  DELTA/MURO TIENE PUNTO CIEGO EN LOS DOS BUCKETS.** Contadas las filas
  INSERT/DELETE reales, **51 de las 92 funciones del bucket MURO (46.568 B)
  tienen movimiento estructural** que el multiconjunto cancela --
  `Play__14cSampleWarpper` 22+22, `HolePunchAvoidables` 13+13, `RenderString`
  9+9--; y al reves, `CompositeSkin(RideInfo*)` esta en el bucket de DELTA con un
  multiconjunto de 2 **escondiendo un bloque de 5 instrucciones movido**, que es
  justo lo que la cierra. **Un delta simetrico (`n+k` y `n-k` del mismo
  mnemonico) es ruido del planificador; filas INSERT/DELETE asimetricas son
  codigo movido.** Hay que mirar las dos cosas, no solo el multiconjunto.
- **REGLA REFUTADA CON DATOS: «delta pequeno + tamano ya exacto = muro» NO
  aguanta.** Cobertura 8,7% de las funciones y 4,8% de los bytes, y **2 de sus 4
  aciertos son falsos positivos** (1.280 B que vetaria, entre ellos
  `CompositeSkin(RideInfo*)`, que esta PROBADA como arreglable). Ademas `tamano
  igual` es `n_obj == n_our` en 74 de 74: no es informacion nueva, es el numero de
  instrucciones con otro nombre.
- **LA QUE SI AGUANTA: delta compuesto SOLO de copias (`mr`, `fmr`, `mr.`) =
  muro.** 20 funciones, 17.616 B, 11 de 11 sin contraejemplo, incluida la mayor
  (`UpdateOccludedPosition`, 4.368 B). Y **solapamiento cero** con la regla del
  tamano: ninguna de las 20 tiene el tamano igual.
- **LOS FINALES DE LINEA MEZCLADOS SON POR LINEA, NO POR FICHERO.**
  `AnimPlayer.cpp` tiene unas lineas con `
` y otras con `
`. Un ancla con
  `str.replace` y `
` da **cero coincidencias en silencio** y las variantes se
  leen como «sin cambio»: a un agente se le colaron seis asi. Usa regex con
  `
?
`, o trabaja por lineas.
- **`git apply` NORMALIZA A LF SOLO LOS TROZOS QUE PARCHEA.** Deja el fichero
  mixto, y entonces las anclas con `
` casan cero veces en esa zona mientras las
  de una linea si. Trabaja por lineas, no por cadenas con salto.

- **MEZCLAR `build_direct.py` CON `ninja` HACE QUE LAS MARCAS DE TIEMPO MIENTAN, Y
  CON VARIOS AGENTES A LA VEZ EL `.o` PUEDE SALIR DE OTRO ARBOL.** Un agente
  midio 97,1662% incremental y 97,3552% tras borrar el `.o` y reconstruir, y lo
  atribuyo a que «ninja no reconstruye la SourceList cuando tocas un `.cpp` que
  incluye». **Eso es FALSO, comprobado**: tocando `AnimPlayer.cpp` y pidiendo
  `zAnim.o`, ninja lo reconstruye -- las dependencias de include las guarda en
  `.ninja_deps`, no en `build/GOWE69/dep/`, que para las SourceLists esta vacio.
  La causa real es la concurrencia: `build_direct.py` escribe el `.o` por su
  cuenta, ninja lo ve mas nuevo que las fuentes y lo da por bueno, y con ocho
  agentes editando el mismo arbol ese `.o` puede haberse compilado con los
  cambios de OTRO. **La cura es la misma que proponia el agente --borrar el `.o`
  antes de medir-- pero la razon no es la que creia.**
- **`const` EN UN PARAMETRO POR VALOR NO ES NUNCA UNA PISTA DE CODEGEN: SOLO
  CAMBIA EL DWARF.** Medido compilando el caso con `ngccc`: `f(float x)` y
  `f(const float x)` dan codigo **byte a byte identico**, tambien con argumento
  constante. Mato dos frentes de golpe --`GetPredictedZone` (`MPH2MPS(float)`
  contra `TrackMPH2MPS(const float)`) y `GenerateHorizonFogDisplayList` (los tres
  `const unsigned char` de las inline de GX)--, que parecian diferencias de
  fuente y son cosmetica.
- **UN DESAJUSTE DEL ARBOL DE BLOQUES PUEDE VALER CERO, Y LA HERMANA AL 100% LO
  PRUEBA.** A `eProgressiveScan_EURGB60DialogBox` (1.848 B) le faltaban **dos
  bloques anonimos enteros** con un `static bool once` cada uno: parecia la
  reclasificacion mas gorda del lote. Pero el mismo idioma sale **seis veces** en
  `EcstasyE.cpp`, y dos de sus hermanas --`eNTSCInterlace_PALSetMode` y
  `eNTSCInterlace_PALProceed`-- **casan al 100,00000% con exactamente la misma
  carencia**. Eran DWARF y `.bss`, no codigo. **Antes de perseguir un bloque que
  falta, mira si una hermana al 100% tiene el mismo hueco.**
- **EL SIGNO DEL DELTA DE `auditecho.py` ES `OBJETIVO - NUESTRO`: POSITIVO = NOS
  FALTA, NEGATIVO = NOS SOBRA.** Verificado midiendo (`SNDAEMSI_timerupdate`: el
  objetivo tiene `mr`=4, nosotros 0, y auditecho imprime `mr+4`). Leerlo al reves
  manda al agente en la direccion contraria, y ya paso: en la ronda 14 se
  repartieron dos encargos con `mr-6` y `mr-4` descritos como «nos faltan copias»
  cuando eran «nos sobran». **Un `mr` de mas nuestro es una local que el original
  no tiene o un valor que copiamos donde el reusa; un `mr` de menos es lo
  contrario.** Y un `stw`/`lwz` POSITIVO significa que **el original derrama y
  nosotros no**, o sea que el original tenia MAS presion de registros.
- **`bench.py` PUEDE DAR UN 100% FALSO: su comprobacion de fidelidad solo compara
  TAMANOS de funcion.** El ctor de `IconScroller` midio **384 B / 100,0000% / 0
  diffs** en un banco de 2 ficheros y **95,48% en la unidad real**. Es rapido para
  iterar (1,4 s contra el `ninja` de una TU de 71 ficheros), pero **ningun cierre
  cuenta hasta verificarlo con `measure.py`/`fndiff` contra el `.o` de la unidad.**
- **UN BARRIDO AUTOMATICO QUE NO MIRA EL CODIGO DE SALIDA DEL COMPILADOR DA
  «IGUAL» CUANDO LA COMPILACION FALLA**, porque `libdiff` no imprime nada y eso
  son cero lineas de diff. Ya colo un ganador falso en middleware. Y ojo con
  `str.replace` sin ancla unica: aplica el parche a la segunda ocurrencia.
- **`measure.py` NO VE UNA REGRESION DE NEAR-MISS, Y ESO YA HA DEJADO CODIGO MALO
  EN HEAD.** Como `matched_code` es todo-o-nada, una funcion puede caer del
  **95,49% al 75,72% y engordar 304 B** sin mover **ni un byte** la medida de su
  unidad. Paso literalmente: al rescatar el trabajo de una ronda cortada se
  verifico «ninguna unidad baja» y se commiteo `Waiting__Q26Speech12StrategyFlow`
  en el estado malo; lo destapo el agente siguiente una ronda despues.
  **Verificar trabajo ajeno o rescatado exige comparar el PORCENTAJE POR FUNCION,
  no solo los bytes.** Herramienta: `scripts/pctsnap.py -o antes.json <unidades>`
  y `--cmp antes.json despues.json`, que lista aparte las que EMPEORAN.
- **«SIN LOCALIZACION EN EL DWARF» TIENE DOS LECTURAS Y SOLO EL ASM LAS SEPARA.**
  En un caso significaba **la variable no existe en el original** y sustituirla
  valio **+1.992 B**; en otros cuatro la misma senal era **ruido** --el valor
  existe en el asm-- y quitar la local **empeora** (85,82 -> 75,88%; 99,96 ->
  99,39%). **La prueba que las distingue: mira que OTRA local del original tiene
  el registro que el asm usa para ese valor.** Si hay una, esa es la que el
  original reusaba.
- **`regmap`/`dwbody` MAPEAN NOMBRE->REGISTRO POR EL DWARF, Y EN VARIAS FUNCIONES
  ESE MAPEO CONTRADICE AL ASM.** Verifica con el asm antes de creerte la tabla.
- **UN CAMBIO DE CABECERA AJENO MUEVE EL PORCENTAJE SIN MOVER `matched`.** El % de
  una funcion paso de 96,658 a 95,163 tras un build completo por **un inline sin
  usar anadido a `UMath.h` por otro agente**, con `matched` y el total de
  near-miss identicos. Es alineamiento de objdiff. **Invalida cualquier % medido
  sin un build completo delante.**
- **`promote.py` DICE AHORA *QUIEN* REFERENCIA CADA SIMBOLO QUE FALTA**, y eso
  cambia el veredicto: si los que lo usan son objetos comodin del troceador
  (`auto_*`) o unidades **VETADAS** --que siempre salen del lado extraido-- **no
  hay dependencia real** y basta con que nuestro objeto exporte ese nombre. Todos
  los bloqueos por `lbl_804FFxxx` de `libc` son de esa clase: **~5.000 B en unas
  20 unidades**, y el arreglo es un `__asm__` con el nombre del troceador **sin
  `static`**.
- **VEDA DEFINITIVA: LOS cflags DE zEAXSound SON CORRECTOS, LOS DIEZ.** Quitados
  **uno por uno** sobre un banco de 521 funciones (76.708 B al 100%):
  `-frerun-cse-after-loop` **−42.332 B**, `-fgcse` −31.136, `-fforce-addr`
  −30.840, `-fcse-skip-blocks` −28.072, `-fforce-mem` −21.884,
  `-fcse-follow-jumps` −20.036, `-fexpensive-optimizations` −17.720,
  `-fmove-all-movables` −3.460, `-frerun-loop-opt` −152. **Ninguna explica el
  patron «el objetivo lo mantiene vivo y nosotros rematerializamos».**
- **`regmap.py` ANALIZA LA SOBRECARGA EQUIVOCADA: coge `same[0]` sin forma de
  elegir.** Segunda vez que muerde. Una funcion con **3 entradas en el DWARF**
  hacia que `regmap --scan` **no listara siquiera** el near-miss, y el «regmap
  dice IDENTICO» de una ronda anterior --**y el plan `--guided` que el permutador
  construyo con el**-- eran de **otra funcion**, la que ya estaba al 100%.
  Rodeo en `scratchpad/r11_regmap_ov.py <unidad> "Clase::Metodo"
  "<subcadena de la firma>" --all`. **Comprueba siempre cuantas sobrecargas hay
  antes de creerte su tabla.**
- **`permuter.py` NECESITA `-I <dir del .cpp>` Y A VECES UNO MAS:** un fichero
  daba «LA BASE NO COMPILA» y dejaba **2.868 B fuera del permutador** por faltarle
  un segundo `-I`.
- **`__asm__("" : "+f"(x))` *ES* BARRERA DE CSE; `__asm__("")` Y
  `__asm__ volatile("":::"memory")` NO LO SON.** Medido: el `asm("")` entre dos
  copias solo bloquea el **planificador** (98,42 -> 97,30%); el clobber de memoria
  deja el binario **identico**; el `"+f"` sube la misma funcion a 98,36%. **No
  sirve como parche --es hack y da 0 B-- pero sirve para DIAGNOSTICAR que CSE es
  el obstaculo.**
- **`litpos` NO PUEDE VER EL *ORDEN* DEL POOL: empareja por indice de
  instruccion.** Un pool **permutado** se ve exactamente igual que una permutacion
  de registros, y en cuanto una constante cambia de sitio **canta decenas o
  cientos de falsos en cascada** (medido: 3 -> 194 en una unidad tras un cambio
  correcto). **El juez es el delta de bytes, nunca la cuenta de `litpos`.**
- **EL ORACULO «QUE INLINE SE USO» DE `fuse.py` DA FALSO POSITIVO CON INLINES
  ANIDADOS:** `bMin(1.0f, bAbs(...))` atribuye el `fabs` al `.cpp` y no a
  `bMath.hpp`.
- **CUANDO EL POOL YA CASA, LOS INICIALIZADORES MUERTOS VALEN CERO.** La entrada
  que prometia «poner `= 0.0f` en cuatro declaraciones: 93,44 -> 97,91%» **no
  reproduce**: 13 combinaciones, efecto nulo. El `init_decl` solo mueve algo
  cuando el reparto o el pool estan mal.
- **VEDA MEDIDA: EL IZADO DE INVARIANTES DE MAS *NO* ES COSA DE LOS cflags.**
  Quitando banderas: zBWare 82,16% -> **77,75%** sin `-fmove-all-movables`,
  **81,09%** sin `-frerun-loop-opt`, **59,47%** sin `-fgcse`; zPlatform 79,47% ->
  **68,58%** / **47,56%**. Y sobre la funcion concreta: 94,614% con todo contra
  94,589% y 92,063%. **Es forma de fuente, no bandera.**
- **DOS REGLAS DE ESTE PLAYBOOK, DESMENTIDAS CON MEDIDA:** (1) el **orden de
  declaracion** de locales del DWARF **no** manda sobre el reparto de registros
  --`CreateCarLightFlares` no se movio un decimal--; (2) **«el original no expande
  ese inline» NO autoriza a quitar la llamada**: en `UpdateLightStateTextures`
  quitar dos accesores que el DWARF del original no lista costo **2,3 puntos**
  (98,63 -> 96,29%). Es la misma familia que «esta local no esta en el volcado».
- **`fndiff.py` MIENTE SOBRE NUESTRO TAMANO:** imprime el del objetivo en los dos
  lados. Una funcion era 944 contra 936 mientras la cabecera decia «936/936».
  **El juez es el tamano del simbolo en el ELF**, y fue lo que encontro la
  variante ganadora en dos funciones.
- **`regmap.py` PUEDE CASAR LA SOBRECARGA EQUIVOCADA:** cogio la de 5 argumentos
  de una clase para el simbolo de 4 de otra, y **toda su tabla de locales era
  falsa**.
- **PARA DESCARTAR «FALTAN TUs», BARRE `debug_lines.txt` CONTRA EL SourceList.**
  zCamera nombra **122 ficheros** y zFEng **73**, y todos estaban ya en la lista.
  Es un barrido de segundos y cierra el frente entero en vez de dejarlo abierto.
- **EL SWIZZLE EQUIVOCADO SE COMPENSA A MANO Y PARECE BUENO.** Una funcion usaba
  `bConvertToBond` (x=-v.y, y=v.z, z=v.x) con el producto escalar **permutado a
  mano**; el original usa `eSwizzleWorldVector`->`bConvertFromBond` (x=v.z,
  y=-v.x, z=v.y) y un `bDot` limpio. Delator: leia `camFwd->y` en **0x54**, no en
  0x50.
- **`debug_lines.txt` RESUELVE LO QUE EL DWARF NO.** Delato que un `Vector3 dir`
  era una **inicializacion agregada** --el objetivo copia un `{0,0,1}` de
  `.rodata` palabra a palabra-- y no cuatro asignaciones: 89,38 -> **92,71% con
  tamano exacto, con una linea**.
- **QUITAR UN LOCAL SOBRANTE PUEDE DESTAPAR UN CROSS-JUMP: MIRA EL TAMANO, NO
  SOLO EL PORCENTAJE.** Al quitar un `flags` que el DWARF no declaraba, las dos
  ramas quedaron **identicas** y GCC fusiono 29 instrucciones: el porcentaje bajo
  a 92,07% **y la funcion encogio 116 B**. Una barrera detras de la llamada lo
  deshizo.
- **`build/GOWE69/report.json` SE QUEDA RANCIO Y EL PERMUTADOR MUERE EN SILENCIO**
  con «no encuentro X en el report». Dos tandas perdidas por eso. Regeneralo antes
  de cada ronda.
- **PASA `litpos` DESPUES DE CERRAR, NO SOLO ANTES.** `CAnimCtrl::AdvanceAnimTime`
  **medi­a 100% con la constante mal** (`/3` donde el original tiene `/30`, dos
  veces). Es el unico chequeo que ve ese fallo.
- **UN `ARG_MISMATCH` MASIVO PUEDE SER UNA ASIGNACION DE STRUCT QUE SOBRA:** 12 B
  de `dst->campo = origen[i];` valian **2,5 puntos y 7 instrucciones**.
- **LA PUNTUACION DEL PERMUTADOR CONTRADICE A objdiff.** Su busqueda guiada
  declaro `score=0.9939` para una variante que objdiff mide en **98,79%** frente
  al 99,758% de la base. **Todo ganador del permutador hay que remedirlo con
  objdiff.**
- **EL SCRATCHPAD ES COMPARTIDO ENTRE AGENTES**: dos agentes se sobrescribieron
  `bdiff.py` y `nm.py` a media sesion. Los helpers de ahi necesitan nombre unico.
- **BANCO AISLADO SIN TOCAR EL ARBOL**: copiar el `.cpp` al scratchpad, generar un
  banco propio que lo incluya y anadir `-I <dir del original>` **al final**
  resuelve los includes entrecomillados y da fidelidad **al quinto decimal**.
  40+ variantes sin riesgo para los vecinos.
- **EL 0% QUE MIENTE: `symbol_mappings` NO LO APLICA *NINGUN* SUBCOMANDO DE
  objdiff.** El troceador nombra `__static_initialization_and_destruction_0_803906C0`
  (con sufijo de direccion, porque el nombre pelado se repite en todo el DOL) y
  GCC emite el pelado. objdiff empareja **por nombre**, asi que la funcion sale
  **0%**. `objdiff.json` tiene el mapeo y `configure.py` lo repone en cada
  `configure` -- **y aun asi ni `diff` ni `report generate` lo honran**: el report
  sigue dando 3.696/6.152. Comprobado comparando los cuerpos a mano: **609 de 614
  palabras identicas**, y las 5 restantes son `b`/`bc`/`stw` con desplazamiento de
  reubicacion. **2.456 B reales e incontables.**
  Regla: si una funcion sale al **0%** con **el mismo tamano** en los dos lados,
  comprueba el NOMBRE antes de escribir una linea.
- **EL TROCEADOR CANTA UN SOLAPE POR EJECUCION, Y UN RANGO GRANDE PUEDE CONTENER
  A TRES VECINOS.** Comparar solo con el rango **contiguo** deja pasar la
  contencion: hay que barrer con el **maximo acumulado**. De 40 rangos reclamados,
  cinco caian dentro de `sstvol` o `mpegl3base`, y salieron **de uno en uno, a un
  `ninja` cada uno**. Cuando pasa, el sospechoso es el rango GRANDE: se esta
  tragando los datos de su vecino.
- **`litpos.py` TIENE CACHE Y MIENTE SI NO SE BORRA:** sin borrar
  `r4_c_<unidad>.json` daba **494 falsos** en zFe2.
- **SEGUNDA CLASE DE FALSO POSITIVO DE `strseq`: cuando NUESTRA `.rodata` se
  desplaza, canta en cascada** y atribuye nuestras cadenas a la funcion vecina
  (24 -> 193 -> 317 «ausentes» segun avanzaba una ronda). **Confirma siempre
  grepeando la cadena en `src/`.** Aun asi, los tres avisos de aquella unidad eran
  REALES: `extern const char lbl_XXXXXXXX[]` que **no existe ni en nuestro arbol
  ni en el ELF original**, y habia que sustituirlo por el literal.
- **DIFF = PAREJA INSERT/DELETE DE LA MISMA INSTRUCCION REPETIDA N VECES =>
  ORDEN DE SENTENCIAS, NO ASIGNADOR.** Cuenta cuantos sitios de la fuente tienen
  cada orden: **el que no da diff manda**. `SetScreenBuffers` tiene 11 bloques, 3
  con un orden y 8 con el otro, y los 8 con diff eran exactamente los 8 del orden
  invertido: 94,88 -> **100%, 1.476 B**.
- **MICRO-BANCO A 3 s/VARIANTE, PERO VERIFICA SU FIDELIDAD.** Un `.cpp` autonomo
  con los structs a mano y los cflags de `build_direct.parse_units()`, puntuado con
  `objdiff-cli diff -1 <obj objetivo> -2 micro.o <simbolo>`: reproduce el
  porcentaje **al quinto decimal** y es 20-30x mas rapido que `bench.py`. Pero en
  una de cuatro funciones **mintio** (96,0% contra 97,25% real): comprueba la
  fidelidad antes de barrer.
- **«SOLO NUESTRA» DE `regmap` ES UNA HIPOTESIS, NO UN VEREDICTO — Y EL
  RECUENTO ACUMULADO ES DEMOLEDOR: 1 MEJORA CONTRA 8 EMPEORAMIENTOS.** Esta es
  la entrada canonica; las demas apariciones del veredicto remiten aqui.
  **La unica vez que pago**: 91,23 -> **94,04%**. **Las que costaron**, medidas
  una a una: 99,60 -> 98,85%; 97,32 -> **93,00%**; `DefragmentPool` 99,27 ->
  **92,47%** (7 puntos por tres locales); `FnStatelessF3::EvalSQT` 99,35 ->
  98,12% **y +12 B**; un multiplicador 98,78 -> 96,58% y +12 B;
  `GRaceParameters::GenerateIndex` 97,90 -> **92,94% y +64 B** (17 sitios);
  `EmitterSystem::UpdateParticles` de 31 a 42 diffs; y `DrawAmbientShadow`, que
  es el caso mas fino porque **el tamano decia +4 B, o sea que la local sobraba
  de verdad**, y aun asi las dos formas de quitarla empeoran (literal en los
  cuatro sitios 96,64% y +16 B; plegada en `scale` 97,94% y −4 B, contra 98,84%
  de partida).
  **Las cuatro razones por las que una local que el DWARF del original no lista
  SI existe**, y hay que descartarlas antes de tocarla:
  1. **barrera de `fold`** — sin el temporal con nombre, `fold` reasocia
     `(A/C)*B` en `A*(recip*B)` y el orden de los `fmuls` deja de casar;
  2. **el DWARF no lista los temporales que GCC coalesce** (§4);
  3. **particion de rango de vida de `regmove`** — el `mr` de mas del original
     no es una local;
  4. **el volcado solo trae la funcion como un inline de cuerpo vacio**, y
     entonces `regmap` es basura entera (vease la entrada siguiente).
  **Regla firme: una local de mas NUNCA se quita «porque `regmap` no la ve»;
  solo se quita cuando el TAMANO dice que sobra codigo — y ni siquiera
  entonces es seguro.**
- **LA LISTA DE INLINES DEL DWARF NOMBRA LOS ACCESORES QUE USABA EL ORIGINAL.** Es
  lo que convirtio una funcion de 896 B de muro (ESTRUCTURA: 1 local ausente, 4
  nuestras de mas, 4 desajustes) en **permutacion limpia de 3 registros**. Leerla
  siempre, no solo la lista de locales.
- **`regmap` DA FALSO POSITIVO EN LOS `static` DE FUNCION** *(sin cifra; el
  mecanismo es verificable de un vistazo en el `.s`)*: el original sale con
  su direccion enlazada (`@0x804565C0`) y el nuestro con el **offset de seccion**
  (`@0x000000A0`) mas una reubicacion. La funcion puede ser IDENTICA.
- **`regmap --obj <banco.o>` VALIDA EL ARBOL DE AMBITOS SIN RECONSTRUIR LA
  UNIDAD**: 1,4 s en vez de 3,5 min.
- **EL ORDEN DE LAS CADENAS DENTRO DE UNA FUNCION DELATA EL ORDEN DE LAS RAMAS,
  NO SOLO UN DATO MAL.** `strseq` dio `Container/Invalid/Null` contra nuestro
  `Null/Container/Invalid`, y de ahi salio la forma `if (ref != 0) {...} else
  {Null}`: **+7,8 puntos y tamano exacto**.
- **NO CUENTES INSTRUCCIONES CON `len(instructions)`: objdiff METE RANURAS
  VACIAS** para alinear los dos lados, asi que los dos cuentan igual **justo
  donde falta codigo**. `Setup__RoadblockFlow` salia `n=151/151` cuando el
  objetivo tiene **149** reales y el nuestro **142**. Corregido en `mnemo.py` y
  `auditecho.py`; si escribes otra herramienta, filtra por
  `instruction.formatted` no vacio.
- **`permuter.py` NO DISTINGUE SOBRECARGAS.** Pedirle `Clase::Metodo` le hace
  optimizar la que ya esta al 100%. Hay que darle `--symbol <mangled>
  --overload N`.
- **VEDA MEDIDA: EL POOL DE LITERALES DE `libc` (17.388 B).** El original mete
  **todas** sus constantes en small data (`lfd @sda21`); nuestro `ngccc` manda el
  pool a `.rodata` (`lis`+`lfd`, **+1 instruccion por uso**) y las tablas `const`
  a `.rodata` en vez de `.sdata2`. Probados sin ningun efecto: `-msdata=`
  eabi/sysv/data/none, `-meabi`, `-G0/8/16/64/100`, y **las CINCO versiones de
  ProDG (3.5, 3.5b140, 3.7, 3.8.1, 3.9.3) dan bytes identicos**. No es cuestion de
  flags nuestros: es el cc1 con el que SN precompilo su libc. Afecta a `vfprintf`
  (6.520 B), `vfprintf_1` (5.760), `k_rem_pio2`, `itoa`, `e_rem_pio2`, `kf_tan`,
  `sf_log10`.
  **Lo que SI funciona**: nombrar los literales como `static double` los sube a
  `.sdata` (`k_rem_pio2` 89,87 -> 93,46%). Lo que no cede es la constante magica
  `0x4330000080000000` de las conversiones `int->double`, que el compilador emite
  solo y siempre en `.rodata`.
- **MARCOS +8: 13 FUNCIONES EN 8 BIBLIOTECAS CON EL MARCO DEL OBJETIVO EXACTAMENTE
  8 B MAYOR** y ocho bytes muertos que nadie usa (`PATH_createstreamimp` al
  **99,92%**, solo los 7 inmediatos del prologo/epilogo). **19.756 B.** Barridos 20
  flags sobre `pathtrack` y **la linea base es la mejor de todas**: no son los
  cflags, es un temporal de 8 B alineado que el original reserva y nosotros no.
- **UN RANGO DE DATOS DEDUCIDO SE VERIFICA SIN RE-EXTRAER: COMPARA LOS BYTES.**
  Si la unidad casa al 100%, sus datos tambien son identicos byte a byte, asi que
  basta leer el ELF original en el rango deducido. De 47 rangos, **7 no cuadran**
  -- y se habrian escrito a ciegas en `splits.txt`. Los que fallan son casi
  siempre los mismos a los que el orden de declaracion no les cuadra.
- **LAS PROMOCIONES SE PRUEBAN SIN VENTANA: `scripts/trypromo.py`.** Reconstruye
  la lista de objetos del enlace desde `build.ninja`, sustituye el `.o` extraido
  por el NUESTRO, enlaza a un temporal y compara el DOL. No toca `configure.py`
  ni `build/GOWE69/main.elf` ni `config/`. **Solo la aplicacion necesita ventana.**
  Se pago a la primera: `realcore/gc/timerthread` sale **LIMPIA en `promote.py` y
  ROMPE el DOL** --el fallo de `systemvars` de la ronda 9, cazado antes--, y la
  receta de promocionar `libgcc2_6/_8/_9` junto al `_3` es falsa: rompen las tres,
  tambien de una en una. **Pasa siempre las dos herramientas, y prueba tambien el
  LOTE junto**, que es como se aplica.
- **EL CODIGO SIN REFERENCIAR NO SE PUEDE QUITAR EN EL ENLACE. VEDA MEDIDA.**
  Nueve SourceLists casan al **100%** y no promocionan porque exportan **143
  funciones que no referencia nadie en todo el arbol** (9.152 B: la API de Lua
  entera en zLua, `EmotionManager` completo en zGameModes). El original se enlazo
  tirandolas. Barrido: **`-strip-unused-data`, la bandera que pasa nuestro
  `configure.py`, NO EXISTE** en la lista de opciones de `ngcld` --y `ngcld`
  **acepta banderas desconocidas EN SILENCIO**, probado con `-zzz-inventada`--;
  **`-strip-unused` si existe y si funciona, pero rompe el DOL sobre el enlace
  INTACTO** (`f304e1a3f86b` contra `9619ba57c991`) porque tira simbolos que el
  original conserva y `keep.lst` no los cubre; y **con las dos a la vez la primera
  enmascara a la segunda**, que es lo que hacia parecer inerte a la opcion. O esas
  funciones no se emiten desde la fuente, o esas nueve unidades no promocionan.
- **UN POOL DE DATOS ESCRITO A MANO EN ASM NO IMPIDE QUE GCC EMITA SU PROPIO
  LITERAL PARA EL MISMO VALOR**: el ensamblador no los funde. `zOnline` llevaba
  **dos copias del mismo `1.0f`** --la escrita a mano en `.rodata+92` y el `$LC58`
  del compilador en +96--, y esos 4 B eran lo unico que la separaba de
  promocionar. Si el pool del objetivo acaba en una constante que el codigo de la
  unidad usa, hay que **dejarsela a GCC**.
- **`splits.txt` HABLA EN SECCIONES DEL OBJETIVO, NO EN LAS NUESTRAS.** Los
  `__clz_tab` de `libgcc2` viven en **`.sdata2`** alli y en `.rodata` aqui (el
  original tiene **tres copias locales**, una por unidad, que `symbols.txt` lista
  con `scope:local`). Escribir el rango con nuestro nombre de seccion lo mete en
  la seccion equivocada.
- **UN RANGO DE DATOS SOLO SE PUEDE RECLAMAR SI DEJA DETRAS <= 4 B SIN
  RECLAMAR.** Es el segundo modo de romper el DOL con `splits.txt`, y no tiene
  nada que ver con que los bytes cuadren. De 7 rangos con **bytes verificados y
  tamano EXACTO** contra nuestra seccion, **5 rompieron el DOL** y 2 no; el unico
  discriminante es el hueco que queda **detras** del rango hasta el siguiente ya
  reclamado: `smixer` (4 B) y `sfir8` (4 B) pasan; `sserver` (56), `SNDI_sin`
  (204), `sf_cosh` (332), `bigswizzler` (824) y `pathvol` (3.696) rompen. Al
  reclamar el trozo de en medio de un comodin `auto_*`, el fragmento que queda
  detras se vuelve a emitir **realineado a 8** y desplaza toda la seccion (aqui
  `.rodata` salio **256 B corta** y todo lo posterior se movio). El hueco de 4 B
  que si pasa es **el resto de nuestro propio corte de simbolo**, no datos
  ajenos. Corolario: para cobrar un rango rodeado de datos sin reclamar hay que
  reclamar **tambien** el vecino de detras, o dejarlo.
- **EL TROCEADOR NO CORTA DENTRO DE UN SIMBOLO, PERO EL SIMBOLO SE PARTE.** Los
  `lbl_` que crea son mas GRUESOS que la frontera real entre unidades: en el DOL
  no hay simbolo entre la ultima constante de una unidad y la primera de la
  siguiente, asi que funde las dos. Los 7 rangos de la ventana 12 cortaban
  **exactamente 4 B** dentro de un `lbl_`, y los 7 eran `data:float align:4`:
  partirlos en `symbols.txt` (encoger el viejo, anadir uno nuevo en el corte,
  conservando la suma) es seguro y **no mueve el DOL por si solo**. El test que
  decide si `claimlbl` se quedo corto o el troceador fundio de mas es comparar el
  rango con el **tamano de NUESTRA seccion** en el `.o`: si coinciden, el rango
  esta bien y lo que sobra es del vecino.
- **UNA CADENA EQUIVOCADA VIVE PARA SIEMPRE EN UNA FUNCION QUE MARCA 100%.**
  objdiff empareja instrucciones, no contenidos: un `addi` a `"COLUMN2_DATA"` y
  otro a `"COLUMN3_DATA"` son **la misma instruccion** para el. Y NFSMW **hashea
  en runtime**, asi que `"smackable"` por `"Smackable"` no da error: da un `Find`
  que falla en silencio. `scripts/strseq.py` compara el **multiconjunto** de
  cadenas por funcion; barrido global: **8 sustituciones reales**, entre ellas un
  `TypeName` que devolvia `"SuspensionParams"` en vez de `"SmackableParams"` y un
  `React` que abria `"RapSheetMain.fng"` en vez de `"RapSheetLogin.fng"`.
- **EN LA COMPARACION DE CADENAS SOLO UNA DIRECCION ES FIABLE.** «El objetivo la
  tiene y nosotros no» y las **sustituciones con el mismo numero de cadenas** son
  senal; «nosotros la tenemos y el objetivo no» es **ruido del extractor**, que no
  decodifica todas las formas de simbolo del lado objetivo -- **zAttribSys, que
  casa al 100%, cantaba 13 falsos**. De 85 hallazgos brutos, 8 eran reales.
- **`%$d` NO ES `%d`:** es la extension de formato de EA y ya ha aparecido dos
  veces en funciones al 100%.
- **UN SIMBOLO QUE SE MUEVE DENTRO DE SU SECCION REPATCHA A QUIEN LO REFERENCIA,
  Y ESO ROMPE EL DOL SIN MOVER NI UNA CENTESIMA DE `matched_code`.** `systemvars`
  casaba al 100% en codigo y datos, tenia las **mismas secciones, del mismo
  tamano, con los mismos globales** -- y aun asi el DOL salio con **5 bytes**
  distintos, en dos ficheros que ni siquiera habiamos promocionado. Causa: al
  extraido le precede un hueco de 8 B (`gap_08_804FF650_sdata`) delante de dos
  punteros de `.sdata` y a nosotros no, asi que sus offsets bajan 8; los objetos
  EXTRAIDOS que los alcanzan por reubicacion recibieron `0x90` donde el original
  pone `0x98`. **Comparar tamanos de seccion no basta: hay que comparar el OFFSET
  de cada simbolo.** Ya lo hace `scripts/promote.py`.
- **CINCO BYTES EN 4,5 MB SE LOCALIZAN EN UN MINUTO, Y VALE MAS QUE BISECAR.**
  Diferencia binaria del DOL contra el original -> rachas -> cabecera del DOL para
  pasar de offset de fichero a direccion -> `splits.txt` para saber la unidad. Una
  sola construccion en vez de once.
- **`@h` CON `ori` (NO `@ha` CON `addi`) ES LA FIRMA DE UNA CONSTANTE QUE EL
  TROCEADOR CONFUNDIO CON UN PUNTERO.** Barrido de una linea sobre el asm
  extraido: `grep -r '@h$' build/GOWE69/asm/`. Quedan 109 en el arbol y **96 son
  de `metrotrk`, que no esta escrito**: el frente esta agotado.
- **LA COMPARACION POSICIONAL DE LITERALES MIENTE TAMBIEN CON CADENAS; LA DE
  SECUENCIA NO.** Una instruccion de mas desplaza todas las ranuras siguientes:
  una funcion canto 3 cadenas «distintas» con listas identicas. Comparando la
  **lista ordenada** por funcion: **4 aciertos de 4, cero falsos**. Filtra los
  bytes imprimibles del pool de flotantes (`'C0'`, `'?'`) o te ahogas.
- **`matched_code` NO VE UN ULP DE DIFERENCIA EN UN FLOAT, PERO EL DOL SI.**
  `463.79202f` es `0x43E7E561` y `463.792f` es `0x43E7E560`. Los literales
  flotantes hay que sacarlos **del patron de bits del ELF**, no redondeados del
  desensamblado. (Complementa la entrada sobre el `lfs` que casa contra `$LCn`
  valga lo que valga.)
- **LA NORMALIZACION A `bool` ES PROPIEDAD DEL SITIO DE LLAMADA, NO DEL
  ACCESOR.** Cambiar el cuerpo de un accesor subio una funcion 3 puntos y **tiro
  otra de 100% a 91,5%, -600 B**. Corolario general: **un cambio en cabecera se
  mide con la UNIDAD ENTERA, nunca con la funcion objetivo** — ese cambio parecia
  ganar y perdia 600 B en silencio.
- **EL MAPA DE LINEAS PRUEBA QUE UNA LOCAL «SOLO NUESTRA» SI EXISTE EN EL
  ORIGINAL.** `regmap` decia que sobraba un multiplicador; quitarlo bajo de
  98,78% a **96,58% y +12 B**. El delator estaba en `debug_lines.txt`: la carga
  de la constante lleva **linea propia**, quince lineas antes de su uso. Es el
  chequeo mas barato antes de creerle a «SOLO NUESTRA» (§2).
- **El DWARF cuenta las copias de un bloque duplicado con el numero de linea.**
  Cinco `clear()` inlinados con lineas 502, 534, 562, 593 y 620 son **cinco
  copias en el fuente**, cuatro de ellas fundidas por cross-jumping — no un
  bloque alcanzado por `goto`.
- **`mcrf cr4,cr0` mas `mfcr` / `stw r12` en NUESTRO lado significa que CSE
  fundio dos comparaciones**, y eso arrastra el marco entero y la renumeracion de
  todos los salvados: el diff se ve enorme y la causa es una linea. Mover la
  declaracion detras del bucle llevo una funcion de **64 diffs a 4**.
- **EL PORCENTAJE ORDENA MAL LOS CANDIDATOS: cuenta los diffs, no el %.**
  objdiff penaliza mucho mas un `INSERT`/`DELETE` que un `ARG_MISMATCH`, asi que
  una variante al **98,89% con 4 diffs** esta estructuralmente MAS CERCA que la
  base al **99,92% con 6**. Medido en `FindFaceInCInst`. Cuando compares
  variantes de una misma funcion, mira el numero de instrucciones marcadas.
  **PERO contar diffs tambien engaña cuando los `INSERT` son CONTIGUOS**: una
  variante bajo de **33 a 23 diffs** y sin embargo **anadio 28 B**, porque GCC
  habia metido una guarda de bucle muerta. **El arbitro final es el TAMANO**:
  si el tuyo no es el del objetivo, no importa cuantos diffs tengas.
- **Los destinos de salto se leen del JSON, no del texto.** `--format json`
  imprime el nuestro como `sym+0xNN` y el del objetivo como direccion absoluta;
  el ancho de columna los trunca y **parecen iguales**. Restando la base salieron
  dos saltos cruzados que el diff de texto daba por identicos.
- **Y CUANDO EL VOLCADO SOLO TRAE LA FUNCION COMO UN INLINE DE CUERPO VACIO,
  `regmap` ES BASURA ENTERA, NO UN DIAGNOSTICO PARCIAL.** En
  `FnRawLinearChannel::Eval` canta **16 locales "SOLO NUESTRAS"** y **tres
  bloques que sobran** — y las 16 existen en el original. El delator es gratis y
  va ANTES de leer el veredicto: `plan.py <unidad> "<fn>" --dwarf-only` imprime
  la firma seguida de **`{}`** (cuerpo vacio) y a continuacion vuelca locales de
  **otras** funciones del TU. Si ves eso, `regmap` y `plan.py` no valen ahi:
  trabaja con `fuse.py` (que va por `debug_lines.txt`) y con el tamano.
- **`permuter.py --overload N` NO desambigua sobrecargas**: hay que pasar
  `--symbol <mangled>`. Cogio la hermana ya al 100% y tiro una tanda entera.
  `plan.py` y `regmap.py` igual: la sobrecarga se saca del volcado a mano.
- **El score de texto del permutador tiene ruido sistematico por la rodata
  anonima**: una funcion verificada al 100% con objdiff puntua 0,985 ahi. Sirve
  como generador de candidatos, **nunca como decisor**.
- **Una unidad que baja y que TU no has tocado es un `.o` rancio o una cabecera
  compartida movida.** Paso tres veces en una sesion. **Recompila la unidad justo
  antes de medir**, siempre.
- **Un cambio en cabecera compartida solo se mide con el arbol PARADO.** Una
  primera medida daba +3.648 B a un constructor de `iallocator.h`; aislado, con
  los agentes quietos y compilando las 366 unidades en los dos estados, valia
  **+0 B**. Lo otro era trabajo en vuelo.
- **Si el DOL deja de casar, el culpable es un objeto `Matching`,** porque los
  `NonMatching` se enlazan desde el objetivo. Dos pasos: comparar las **tablas de
  secciones** de los dos DOL para ver cual crecio (una sola seccion mas grande
  desplaza todo y da cientos de miles de bytes distintos), y luego comparar el
  tamano de `.rodata` de cada objeto **enlazado desde src/** contra su gemelo de
  obj/. Asi salio `device.o` con 80 B de mas.
- **objdiff empareja por NOMBRE, y el splitter renombra los locales homónimos
  con su dirección.** `__static_initialization_and_destruction_0_803906C0` no
  casa con el `__static_initialization_and_destruction_0` que emite GCC, y la
  función se lee al **0%** aunque el código case. objdiff.json admite
  `symbol_mappings` por unidad (`{nombre_del_objetivo: nombre_nuestro}`);
  `tools/project.py` conserva los que ya existan, pero objdiff.json está en
  `.gitignore`, así que hay que reponerlos desde `configure.py`
  (`OBJDIFF_SYMBOL_MAPPINGS` + `apply_objdiff_symbol_mappings`). Desbloqueó
  2.456 B en `realmemcard/gc_interface`. **Barrido completo del proyecto: ese es
  el ÚNICO símbolo así, no hay más que cosechar por esta vía.**
- **LA MÁS CARA DE TODAS: `objdiff-cli diff` necesita DOS opciones, no una.**
  `report generate` se invoca con las dos (`build.ninja`, `objdiff_report_args`):

      -c functionRelocDiffs=none -c ppc.calculatePoolRelocations=false

  Sin la segunda, objdiff **inventa reubicaciones** para los accesos al pool de
  constantes y las cuenta como diferencias: una función byte-perfecta se lee al
  **94-99,9%**. `gc/trctasks` daba 4 de 12 funciones con `diff` y **12 de 12,
  20.176 B**, con `report`. Barrido completo: **48.348 B escondidos en 28
  unidades**, 1,22 puntos. Lo caro no es el porcentaje: son las rondas de
  agentes mandados a arreglar funciones que ya estaban terminadas.
  **Si `diff` y `report` no coinciden, es esto.**
- **El comparador de texto miente.** `$LCnnn` contra `lbl_XXXXXXXX` y las
  etiquetas `.L` normalizadas a `L` **ocultan destinos de salto distintos**. Una
  función daba "IDÉNTICO" al 99,978%. **Cierra siempre con `report generate`.**
- **El `diff` de objdiff-cli miente donde el report no**: hay `REPLACE` cuyos dos
  lados imprimen la **misma** instrucción (reubicación fantasma `R_PPC_NONE` del
  objetivo) y el report dice 100%. **Filtra por `fuzzy_match_percent`.**
- **`report generate` y `objdiff-cli diff` no puntúan igual.** Usa `report`.
- **`matched_code` NO comprueba el VALOR del literal flotante.** Con
  `function_reloc_diffs=none` un `lfs` contra `$LCn` casa aunque nuestro `$LCn`
  valga otra cosa: `1.0f/65536.0f` dio 100% donde el original tenia
  `0x377FFC9C`. El porcentaje no lo ve, el DOL si. **Lee siempre el valor del
  ELF** (`scripts/rodata.py` sobre `orig/GOWE69/NFSMWRELEASE.ELF`) y escribe la
  constante exacta, aunque ya midas 100%.
- **Ningún resultado de `permuter.py --sweep` vale sin pasar por `report
  generate`.** Medido: su puntuación de texto subió de 0,984 a 0,991 mientras
  objdiff **bajaba** medio punto; de tres "mejoras" que reportó un barrido, la
  única verificada era falsa.
- **Contar líneas de diff NO ordena las variantes.** Una variante con 7 líneas de
  diff puntuaba peor que otra con 12: objdiff penaliza mucho más un par
  `INSERT`/`DELETE` que un `ARG_MISMATCH`. Hay que leer el porcentaje.
- **Un "parkeado con medida" CADUCA.** Los descartes valen mientras el catálogo
  no crezca: una función archivada como rotación pura del pool de spill cayó
  después con una transformación nueva.
- **Un veredicto REPARTO o PERMUTACIÓN de regmap NO descarta código faltante.**
  El delator es el **tamaño**.
- **`derotate.py` describe el orden del ORIGINAL, no prueba que el nuestro esté
  mal.** Aplicarlo a ciegas bajó una función de 99,96% a 98,50%. **Actúa sólo
  donde NUESTRA secuencia de stores difiera.**
- **La permutación de registros es SÍNTOMA, no techo**: sólo el **4%** de los
  near-miss del árbol es techo real; el **60%** tiene causa estructural.
- **Si el diff es sólo renumeración de ranuras y el mapeo es una rotación pura**,
  es el pool de spill de reload y **ahí sí hay que parar**: la fase depende del
  número de spills decididos **antes** en la función.
- **zFeOverlay midiendo 0%** es la trampa del renombrado de sección
  (`.text=.over`): recompílala con `build_direct.py`, que lo aplica.
- **Y `scripts/bench.py` TENIA LA MISMA TRAMPA: no renombraba la seccion, asi
  que en zFeOverlay y zOnline el banco entero medía `0,0000%` con `0 diffs`
  y tamaños correctos** —el fallo silencioso perfecto, porque las cifras
  *parecen* una medida. **Ya corregido**: `compila()` pasa el `.o` por
  `tools/rename_section.py` cuando la unidad lo pide. Comprobado: con el
  arreglo, `NotificationMessage__17FEMarkerSelection` da 97,8307% en el banco
  y 97,83069% en la unidad entera.
- **Para promover a `Matching` en `configure.py` hacen falta `matched_code` Y
  `matched_data` al 100%.** Promover con datos al 10% rompió el DOL.
- **Acotacion al disco lleno (§1): el sintoma que lo delato NO fue una medida
  rara, fue un `git checkout` fallando con "Out of diskspace".** El scratchpad
  compartido tenia **9,5 GB** en 3.844 ficheros, de los que 8,3 GB eran volcados
  `d_*.json` de mas de dos horas: son cache regenerable y se pueden borrar sin
  romper a nadie (como mucho, otro agente repite un `fndiff`).

---

## 2a. El bloque fundido cuenta las sentencias -- CON LA CORRECCION

La regla original decia: `lmap` imprime una linea de fuente por cada `return` que
el cross-jumping fundio en un bloque, asi que si el objetivo lleva N lineas y tu
fuente tiene N-1 sentencias, **falta una**. Valio 2.200 B en la ronda 15.

**Esta MAL enunciada, y su propio autor la corrigio midiendo.** Un mismo address
con N lineas no significa N sentencias fundidas. Significa eso **solo** cuando el
address es un punto de union alcanzado por saltos **y** las lineas de mas emiten
codigo en algun otro sitio de la funcion. El caso normal es el contrario: **una
sentencia que no emite nada empuja su nota de linea a la instruccion siguiente**,
y las llaves de cierre de bloques anidados hacen exactamente eso. La frecuencia
lo dice: entre el 5% y el 17% de los addresses de una funcion llevan 2 o mas
lineas.

**LA VERSION BUENA: cuenta solo las lineas que emiten codigo en OTRO address de
la funcion. Las que no aparecen en ningun otro sitio son sentencias vacias --o
llaves-- y no cuentan.** Se comprueba en 5 segundos grepeando
`symbols/debug_lines.txt` por el rango de la funcion.

Los dos falsos positivos que la corrigieron, los dos en `CarRender.cpp`:

- `UpdateLightStateTextures`: el `li r8,1` con 2 lineas esta en la **primera
  instruccion de la funcion**, que no es punto de union de nada. La 3448
  (`int left_light_state = 0;`) no emite porque queda muerta; la 3456 emite el
  `li`. No faltaban dos sentencias: faltaba que GCC no plegara la constante.
- `CullParts`: el `stw` con 3 lineas (721, 727, 732) **si** esta en un punto de
  union de seis predecesores, asi que la duda era legitima -- pero 721 y 727 no
  aparecen en ningun otro address, y el mapa va 710 -> 716 -> 721/727/732 con
  717-720, 722-726 y 728-731 vacias: es el hueco de los cinco `}` anidados.

Donde la regla nacio si vale, y ahora se sabe por que: en un epilogo las lineas
de mas eran `return`, y un `return` **si emite codigo** (el `b` al epilogo) que el
cross-jumping funde.

## 2b. Leer un diff sin que te mienta

- **N LINEAS DE CABECERA PARA LO QUE TU ESCRIBISTE EN UNA: LA SENTENCIA PARTIDA NO
  ES COSMETICA.** El objetivo tageaba `cmpw` en la linea 136 de una cabecera,
  `li r9,0` en la **137** y las cargas en la **139**, con la 138 vacia. Un `&&` de
  una linea no puede dar tres lineas con hueco. La forma que encaja --`if (a != b)
  { return false; } return c == d;`-- cerro la funcion **al 100%**, y el fallo no
  estaba en el `.cpp` sino en un `operator==` de la cabecera.
- **`dtk elf disasm` IMPRIME MAL LOS `.L_`**: el nombre de la etiqueta es
  **destino + direccion de la propia rama**. Para leer un destino hay que restar.
  Sin eso, decodificar la colocacion de bloques da resultados absurdos.


- **EL DESTINO DE LA RAMA DICTA EL ANIDAMIENTO.** `fila = (destino - inicio_fn)/4`.
  Si los dos saltos de un `||` van a la fila del test SIGUIENTE y no a la del
  cuerpo, la condicion es un `&&` con las comparaciones invertidas **y el bloque
  de detras es hermano, no anidado**. Valio **976 B** en una funcion que ya tenia
  las cuatro agrupaciones del `||` barridas: ninguna cuestionaba el anidamiento.
- **EL HUECO DE RETORNO DEL DWARF NOMBRA LA SENTENCIA.** Un inline que devuelve
  agregado (`Vector4Make(...) { Vector4 res; // r1+0x18 }`) compartiendo hueco con
  otra local, mas `lmap` mostrando **una sola linea** donde tu tienes seis
  sentencias, da la forma exacta: **inicializacion**, y las demas declaradas
  DESPUES para que el temporal coja el hueco y la otra reutilice el liberado.
  **1.100 B.**
- **LA LISTA DE INLINES DEL DWARF ES UNA CHECKLIST DE SENTENCIAS.** Si trae un
  `operator*=` entre dos que si tienes, **te falta esa sentencia** aunque se
  pliegue a nada: `vEyeDirection *= 1.0f;` arreglo **el tamano del marco** y subio
  98,86 -> 99,70%. Parte de los **3.776 B** de la mayor de la ronda.
- **GCC PLIEGA `a * K * b` A `a * (b * K)`.** El orden de operandos del `fmuls`
  del objetivo te da la asociatividad de la fuente: para obtener
  `fCurve * (impact * 0.25f)` hay que escribir **`fCurve * 0.25f * impact`**.


- **`INSERT`/`DELETE` EN CRUDO NO MIDEN NADA.** objdiff alinea sobre el **texto
  exacto** de la instruccion, asi que un tramo que solo difiere en un registro no
  empareja, y cualquier instruccion identica de dentro hace de **ancla** y parte
  el tramo entero en un DELETE mas un INSERT. **Una instruccion desplazada se
  cuenta como 2N filas movidas.** Medido: 762 filas crudas en el arbol contra
  **392 reales**, con 41 funciones (33.848 B) donde no falta ni sobra nada.
- **Y las ramas locales se formatean distinto en cada lado** --el objetivo como
  `beq 0xcf8`, nosotros como `beq Simbolo+0x5c4`--, asi que si comparas texto sin
  normalizarlas **toda rama del arbol cuenta como codigo que falta**. En una
  funcion al 99,3% con 4 instrucciones de diferencia salian 65.
- **Lo que si mide**: el multiconjunto de instrucciones **normalizadas** (sin
  registros, sin literales, sin inmediatos y sin destinos de rama local) dice
  cuanto codigo falta y cuanto sobra de verdad; y **`dmax`** --la distancia de
  una fila movida a la del tipo contrario mas cercana-- separa el jitter del
  planificador (<=20) del bloque de verdad movido (>60). Verificado en los dos
  sentidos: `RenderFlaresOnCar` con dmax 313 iza un `@ha` 330 instrucciones
  antes; `ProcessPadsForPackage` tiene sus filas a 2-5 y no mueve nada.
- **La direccion contraria esta limpia**: sin filas movidas, objdiff emparejo
  todo 1:1, luego el orden es identico por construccion y en el muro no puede
  esconderse movimiento.
- **La leccion cara**: dos versiones seguidas de la herramienta de reparto
  midieron mal, y la segunda repartio 126.192 B de trabajo que no existia. Antes
  de repartir con una metrica nueva, **contrastala contra tres funciones cuyo
  diagnostico ya conoces**: si `Add__6RBGrid` no sale con 4, la metrica miente.

## 3. Estructura y ámbitos

- **UN `if` SIN `else` EN EL ORIGINAL: LEELO EN EL DESTINO DEL `beq`.** Si el
  salto del objetivo cae 4 filas mas alla que el nuestro, es que cuando la
  condicion falla el original **no ejecuta lo que nosotros metimos en el `else`**.
  Quitar ese `else` cerro de golpe dos permutaciones de registros y el `beq`:
  99,93 -> **100%**, 2.548 B.
- **UN TERNARIO QUE EL ORIGINAL TIENE COMO `if/else`** se ve en el mapa de lineas:
  una linea para el test, otra para cada brazo. Valio 0,78 pp en la misma funcion.
- **CUANDO AL OBJETIVO LE SOBRA UN `mr` QUE COPIA LA VARIABLE DE BUCLE** y tu
  cuerpo cierra con `var = valorDerivado;`, el original ponia el **incremento**:
  `var = derivado` se fusiona en un registro, y `var++` hace que CSE reutilice el
  `var+1` ya calculado **como copia** y deja los dos vivos. 8 colocaciones
  barridas, **solo la ultima da 100%**. 252 B.


- **`T *p = &objetoDePila;` Y USARLO SOLO EN LA PRIMERA LLAMADA.** El objetivo
  materializa la direccion del marco UNA vez y la copia (`addi r3,r1,0x8` mas
  `mr r24,r3`); nosotros la rematerializamos en cada uso (`addi r3,r1,8` ...
  `addi r24,r1,8`). Con `stBankSlot *pNewSlot = &NewSlot; pNewSlot->Clear();`
  sale la forma del objetivo: **96,024 -> 99,724%**. **El signo importa**: usarlo
  en TODOS los accesos hunde la funcion a 94,000% y +4 B. Es la primera llamada,
  no el resto. (Y no confundir con `T *p = this;`, que es al reves: ahi el
  puntero local ROMPE el CSE de los miembros y hay que quitarlo.)


- **`i++` CONTRA `++i` SOLO MUERDE EN ITERADORES DE CLASE.** El postfijo
  materializa una copia del iterador en pila, y se ve como un `stw` de mas en el
  bloque del incremento. Valio tamano exacto en dos funciones (92,71 -> 93,57% y
  96,88 -> 98,53%). **En los iteradores que son punteros crudos --los de
  `UTL::Vector`-- prefijo y postfijo dan asm identico**, asi que ahi no lo
  busques.
- **`iterator i = begin();` EMITE UN STORE; `i = begin();` EMITE DOS** --el
  segundo es un store muerto en la ranura temporal--. Dos bucles de la misma
  funcion pueden tener formas distintas en el original y hay que reproducir cada
  una.
- **UN `T *p = this;` LOCAL ROMPE EL CSE DE LOS MIEMBROS.** Con el puntero local
  GCC mantiene `this->mHead` vivo en registro donde el original lo **relee**.
  Quitarlo y llamar los metodos sueltos valio **+2,86 pp** y devolvio los cuatro
  `lwz` de relectura que faltaban.
- **UN TEMPORAL FLOTANTE INTERMEDIO CUESTA UN `fmr` POR USO** cuando alimenta una
  conversion (`extendsfdf2` -> `fctiwz`): escribir `n = (int)(expr)` directo en
  vez de pasar por `float f` los quita. **El primero y el ultimo coalescen solos**,
  asi que solo sobran los de en medio.
- **`a = b = f();` CONTRA `a = f(); b = a;`**: cuando al objetivo le SOBRA un `fmr`
  justo detras de un `bl`, es una **asignacion encadenada** -- la forma en dos
  sentencias se la come la propagacion de copias.


- **UN BLOQUE ANONIMO DEL DWARF CON RANGO *VACIO* QUE DECLARA LA MISMA LOCAL QUE
  SU HERMANO = EL ORIGINAL ESCRIBIO EL CUERPO DOS VECES Y EL CROSS-JUMPING BORRO
  EL PRIMERO.** Pasar `if (A || B) {cuerpo}` a `if (A) {cuerpo} else if (B)
  {cuerpo}` da **el mismo codigo** y ademas **sube las referencias de `this`**,
  deshaciendo el intercambio de registros. **876 B de una tacada**, y el mismo
  truco cerro otra de 324 B.
- **LA INICIALIZACION DE CAMPOS REPETIDA EN DOS USUARIOS DE UNA STRUCT ES EL
  CONSTRUCTOR DE ESA STRUCT -- Y HAY QUE QUITARLA DE LOS DOS.** Escribir el ctor
  cerro un llamante (372 B) pero **dejar la copia en el otro lo tiro de 100% a
  74,37%**; quitandola tambien, vuelve al 100%.
- **GCC 2.9 DEVIRTUALIZA LAS LLAMADAS VIRTUALES SOBRE `this` DENTRO DE UN
  CONSTRUCTOR.** Una referencia local `T &self = *this;` **lo impide** y reproduce
  el `lwz vptr / lha / lwz / add / mtlr / blrl` del original. 148 B.
- **EL ARBOL DE INLINES DEL DWARF DELATA LLAMADAS DUPLICADAS que `regmap` NO VE.**
  El original expandia `RegisterLoadStreamingEntry` **dos veces** --una a nivel de
  funcion con rango cero y otra dentro del `else`-- y nosotros la teniamos **una
  sola vez detras del if/else**. Duplicandola en las dos ramas: **100% a la
  primera compilacion**, y la referencia extra deshace el intercambio de
  registros.
- **UN ARGUMENTO *MUERTO* DE UN INLINE NO ES INOCUO.** El DWARF expandia el
  `bMalloc` **de 4 argumentos**, no el de 2. Pasandole **la misma cadena que ya
  usa la llamada vecina**, CSE guarda el `high` del literal en un callee-saved y
  lo sube delante de la llamada -- que era exactamente el diff. Y **no mete
  literal nuevo en `.rodata`**, porque el inline lo descarta. 860 B.
- **`a = b = c` EN C++ RELEE EL LVALUE VOLATIL.** Escribirlo encadenado en vez de
  en dos sentencias mata una recarga.
- **SI EL DWARF DA REGISTRO A LA VARIABLE PERO EL OBJETIVO METE EL VALOR EN UN
  SCRATCH, EL ORIGINAL NO ASIGNABA: REPETIA LA EXPRESION.**
- **LLAMAR AL METODO SOBRE EL *GLOBAL*, NO SOBRE `this`.** `p->SetPrice(
  GetPartPrice(p))` contra `gManager.GetPartPrice(p)`: el objetivo emite
  `lis/addi gManager` donde nosotros poniamos `mr r3, r26`. **1.500 B en dos
  funciones.**
- **REFERENCIA LOCAL + *UNA* LLAMADA POR EL GLOBAL** (`mgr.A(); mgr.B();
  gManager.B();`) mata el pseudo en el ultimo uso de la referencia y rematerializa
  el directo, **liberando el callee-saved**. Con la referencia en las dos, el
  marco crece 8 B y entra un registro mas.
- **CONTAR UN INLINE A NIVEL DE FUNCION EN EL DWARF DICE SI EL ORIGINAL TENIA
  `goto` O DUPLICO LA COLA:** seis `Get()` = tres parejas => el bloque esta
  escrito **tres veces**, no hay `goto`.
- **UN `bl` CON UN ARGUMENTO DE MAS ES INVISIBLE AL PORCENTAJE HASTA QUE EL TAMANO
  CUADRA.** El **mangled del objetivo** delata la sobrecarga correcta.
- **UN `?:` ASIGNADO DIRECTO A UN MIEMBRO EMITE DOS `stw` Y LUEGO RECARGA;
  ASIGNADO A UNA VARIABLE (aunque sea el parametro) EMITE UN `select` EN
  REGISTRO, UN SOLO `stw`, Y ESE REGISTRO SOBREVIVE COMO ARGUMENTO DE LA
  LLAMADA.** Ese cambio solo valio **+2,7 puntos**.
- **UN MIEMBRO QUE SE LEE JUSTO DESPUES DE ESCRIBIRLO SE RECARGA SI HAY UN `stw` A
  OTRO MIEMBRO POR MEDIO** (aliasing): el original **pasa la variable, no el
  campo**.
- **EL DISCRIMINANTE P/D DE LA LLAMADA VIRTUAL, RESUELTO: `P` (pfn antes que
  delta) SALE SI Y SOLO SI EL BLOQUE BASICO DE LA LLAMADA CONTINUA CON
  INSTRUCCION REAL DESPUES DEL `blrl`.** Reproducible en un banco de 1 s:
  `void f(Cop*c){ c->A(); c->A(); }` da **P para la primera y D para la segunda**
  con el mismo fuente. Conteo sobre los objetivos: zWorld **26 P con
  continuacion / 0 sin nada detras**, zWorld2 **51 / 0**, zSpeech **197 / 5**.
  **Pero los tres sitios que fallan son de esas 5 excepciones** --el bloque
  estaba abierto en `sched2` y lo cerro un pase posterior-- y por eso **15
  variantes de fuente dan todas D**. No cae con formas de fuente.
- **`arr[i].campo` CONTRA `GetX(i)->campo` INVIERTE LOS OPERANDOS DE
  `lfsx`/`lwzx`.** El accesor devuelve puntero -> `plus(base, indice)`; el indice
  directo da `plus(indice, base)`. Cuatro sitios de una funcion: 99,17 -> 99,64%.
- **`BNEW T(args)` EMITE `bl __builtin_vec_new`, NO `__builtin_new`**, porque el
  `operator new(size,file,line)` de `bWare.hpp` hace `return new char[size]`.
- **`MX msg; msg.Post(port);` NO ES `MX(...).Post(UCrc32(hash));` -- 772 B EN
  CINCO FUNCIONES DE UNA TACADA.** Un `UCrc32 port` **con nombre** es un lvalue:
  GCC copia el argumento por valor a un temporal y pasa **esa** direccion. Un
  temporal se pasa por direccion directa, **sin copia**. Delator: un `lwz`+`stw`
  de mas y el marco **8 B menor**.
- **MIEMBRO CONTRA LOCAL CACHEADO: ES UN DIAL DE DOS DIRECCIONES, Y EL
  DISCRIMINANTE ES CUANTAS VECES LEE EL OBJETIVO.** Las dos mitades de esta
  regla se contradecian en el manual; el criterio que las separa es este, y se
  lee del asm del objetivo antes de compilar nada:
  - **El objetivo LEE EL MIEMBRO UNA VEZ (un `lwz off(this)` y luego el valor en
    un registro que sobrevive)** -> liga tu un local: `T *p = miembro;` **antes
    de un `if` iza el `lwz` del miembro por encima del test**, y ninguna forma
    del `if` lo consigue. `mnemo` diciendo `mr +9 / lwz -9` con tamano exacto es
    la firma: `GRaceParameters *parms = mRaceParms;` llevo **82,78 -> 94,39%**.
  - **El objetivo RELEE el miembro en cada uso (varios `lwz off(this)`, o el
    vptr recargado tras cada `bl`)** -> quita el local y repite
    `this->m_Campo`. Reusar una local cacheada donde el objetivo relee costaba
    16,33 puntos (83,67 -> **100%** al quitarla); repetir `x->GetPart()` en vez
    de ligarlo valio **6 puntos** (con el local gastamos un salvado y se
    desplaza todo el reparto); y **en el frontend gana REPETIR incluso con
    llamadas virtuales**: `opt = GetSelectedOption(); if (opt->GetPart())` emite
    dos cargas y el original emite **dos llamadas virtuales completas**
    (confirmado en tres funciones; delator: un `lwz r9,0x28(this)` del vptr
    repetido donde nosotros tenemos una sola carga).
  - **Caso especial que parece la segunda y no lo es**: si al objetivo le sobran
    `lwz rA,off(this)` **+ `mr rB,rA`** donde nosotros emitimos un solo
    `lwz rB,off(this)`, eso **no** es una relectura: es la particion de rango de
    vida que mete `regmove`. Borra el local y repite el miembro (fueron los
    ultimos 4 B de una funcion de 664 B). **Un `mr rA,rB` de mas en el original
    con las locales por lo demas identicas es sintoma del asignador, no de
    fuente que falte.**
- **EL ORDEN DE LOS INLINES DEL DWARF SITUA LA SENTENCIA DENTRO DEL BLOQUE.** Si
  el bloque lista `A x3` y **solo despues** `operator-` y `GetSeconds`, la resta
  va en la **ultima** linea, no en la primera. Con eso, 100%.
- **UNA LOCAL DECLARADA OTRA VEZ *DENTRO* DEL `if` EN EL DWARF DICE QUE NO HAY
  `else`: hay `return` TEMPRANO.** Si ademas vive en un registro de argumento (r4)
  es efimera, y las que estan a nivel de funcion son las que sobreviven.
- **`bool Getter() { return campo == true; }` DA `cmpwi rX, 1` EN EL LLAMANTE SIN
  ROMPER EL TERNARIO.** `fold` intercambia los brazos de `c ? CONSTANTE : f()`
  (arg1 simple, arg2 no) e invierte la condicion, asi que un ternario **nunca**
  deja la constante en el brazo de caida; y `== true` escrito con un `bool` en el
  llamante se pliega a `cmpwi 0`. Metiendo la comparacion **dentro del inline**, el
  llamante ve un `CALL_EXPR` --no invertible, sin swap-- y tras integrar sale el
  `cmpwi 1`. 400 B.
- **LISTA DE INICIALIZADORES CONTRA CUERPO DEL CONSTRUCTOR:** los `stw` salen en
  orden de **declaracion** con lista y en orden **escrito** con asignaciones en el
  cuerpo. Y **un miembro en la lista se inicializa ANTES del cuerpo**, asi que si
  el objetivo lo pone despues de un `memset`, ese miembro va **al cuerpo**. Un
  static-init subio 86,67 -> 96,15 -> 98,02 -> 99,43 -> **100%** aplicando esto
  cuatro veces seguidas.
- **CON UN INLINE QUE DEVUELVE POR VALOR, INICIALIZAR NO ES ASIGNAR.** `T x =
  Make(...)` construye **en** `x` (una copia); `T x; x = Make(...)` mete un
  temporal intermedio (una copia de mas). Coste medido: **100% -> 79,74%**.
- **UN FLOAT EN f1 DENTRO DE UN VARARGS ES LA POSICION DEL ARGUMENTO.** Solo el
  **primer** vararg flotante va a f1, asi que si el DWARF dice `x // f1` y
  nosotros lo pasamos el segundo, **los argumentos estan cruzados**. Cerro 676 B:
  `"+%0.0f %s"` recibia `(cadena, velocidad)`.
- **COMPROBACION DE NULO DUPLICADA = `mr. r3,rX; bne; lis; addi` DE MAS.** Si el
  accesor ya devuelve `""`, repetirlo en el llamante cuesta 4 instrucciones; se ve
  porque el objetivo tiene **un** `cmpwi/bne` y nosotros dos.
- **EL CAST A SU PROPIO TIPO CREA UN TEMPORAL.** Con un parametro `const int&`,
  `f((int) x)` materializa el temporal y deja `x` en registro; `f(x)` la clava en
  la pila. 116 B.
- **`plan.py --dwarf-only` ES EL PLANO LITERAL DE UN `Setup` DEL FRONTEND**: da la
  cadena exacta de accesores, los `const unsigned long FEObj_*` en bloque anonimo,
  y que `FEngSetInvisible(pkg, hash)` es un inline de **dos** argumentos. Cuidado
  con un detalle que cuesta puntos: **con el `mode` cacheado en un local, GCC
  prueba que el `& 4` interior del inline es redundante y BORRA la materializacion
  del bool** que el objetivo si emite. 95,34 -> 100%.
- **EL BLOQUE ANONIMO DEL DWARF DICE QUE SENTENCIAS ESTAN DENTRO DE UNA RAMA, Y EL
  DELATOR SIN LEERLO SON LAS RANURAS DE TEMPORAL.** Un `LoadBehavior(...)` parecia
  incondicional y estaba **dentro del `else`**: el objetivo usaba 0x98/0xa0/0xa8
  (las que el bloque **acababa de liberar**) y nosotros 0x60/0x70/0x78 (las de un
  objeto ya muerto). **92,88% -> 96,46%, de 176 a 79 diffs.**
- **UN `struct` ANONIMO DEL DWARF ES LITERAL, NO UNA AGRUPACION.** Lo que parecian
  **tres arrays paralelos** eran `struct { unsigned type; int needed; int weight; }
  currentlyActive[10]` — stride 0xC, rango 0xC8..0x140. El stride lo confirma.
- **Guard clause**: si todas nuestras locales caen en un bloque que el original
  no tiene, la causa es la forma de la guarda — `if (cond) { cuerpo }` es
  `if (!cond) return;`, y en un bucle `if (p != null) { … }` es
  `if (p == null) continue;`. **Sacar la declaración a mano EMPEORA.**
- **Guard clause con COLA DUPLICADA**: escribir la cola **dos veces** deja que
  GCC haga cross-jumping de las dos idénticas, y eso hace que el salto temprano
  aterrice **pasado** el test siguiente.
- **LA GUARDA RAPIDA DE UN INLINE DUPLICA LA COLA COMUN, Y SON 3 INSTRUCCIONES
  POR SITIO DE LLAMADA.** `EAGL4::DynamicLoader::ELFAddr` estaba escrito

      if (offset >= mDataLen) { if (mpReloc) return A; if (offset > mDataLen) return 0; }
      return &mpData[offset];

  y emite `blt` **saltando por encima** de todo el cuerpo hacia la cola comun.
  El objetivo emite `bge` **al cuerpo** y en la caida repite la cola con un `b`:
  eso es la guarda rapida escrita a mano,

      if (offset < mDataLen) return &mpData[offset];
      if (mpReloc) return A;
      if (offset > mDataLen) return 0;
      return &mpData[offset];

  con la cola **duplicada de verdad en el fuente**. Cuesta 3 instrucciones por
  expansion (`lwz` del miembro + `b`), asi que **el delator es el tamano, no el
  porcentaje**: en `DynamicLoader::Initialize`, con dos expansiones, paso de
  -40 B a -16 B y de 75,76% a 79,66% (462 -> 428 diffs). Es la version del
  «guard clause con cola duplicada» **dentro de un inline**, y el efecto se
  multiplica por el numero de sitios donde se expande.
- **Si el bloque de fallo salta al FINAL de la función**, el original escribió
  `if (ok) {grande} else {pequeño}`, no un guard clause.
- **La cláusula de guarda ordena los bloques al revés**: `if (x) return A;
  return B;` emite `B` en el fall-through. Para un `beq→A` con `B` de caída hay
  que escribir **la otra rama** como guarda.
- **El inverso también existe**: el original declara *dentro* del bloque y
  nosotros a nivel de función.
- **Bloque anónimo alrededor de `{ iterador; while }`** cuando regmap dice que
  falta un bloque nombrando un iterador.

- **RECONSTRUIR UNA FUNCION ENTERA DESDE EL ARBOL DE BLOQUES DEL VOLCADO ES UNA
  TRANSCRIPCION, NO UNA BUSQUEDA.** `Geometry::SphereVsBox` (1.896 B) llevaba
  rondas al 97,50% con `regmap` cantando **16 locales a la profundidad
  equivocada**, cuatro bloques que faltaban y dos que sobraban. Copiando literal
  del volcado (a) el **orden de declaracion** de las locales de funcion, (b) cada
  bloque anonimo con sus locales y (c) la lista de inlines de cada bloque,
  `regmap` pasa a **"MISMO conjunto de locales y mismo arbol de bloques"**: 43
  locales iguales, **2** registros distintos, y el tamano queda **exacto**. Tres
  detalles valieron la mitad:
  1. Los `goto` de una descompilacion a mano son `if / else if / else` anidados,
     y el **rango** de cada bloque del volcado da el anidamiento exacto.
  2. El volcado da el TIPO: `unsigned int count` donde teniamos `int`.
  3. **El volcado nombra `static inline` DE FICHERO que no existen en nuestro
     arbol.** `static inline float RayExitSphere(P, D, C, r)` sale con un `grep`
     en `symbols/mw_dwarfdump.nothpp` **con sus locales en orden de
     declaracion**, y el mapa de lineas lo situa en `Geometry.cpp:26-59`, o sea
     **antes** del `namespace`. Escribirlo como inline de verdad -en vez de a
     mano en el llamante- es lo que alinea el bloque `b4/b1`. Busca siempre
     `static inline` en el volcado del TU antes de dar una funcion por
     transcrita.
- **EL SITIO DE UNA SENTENCIA CUYAS INSTRUCCIONES CAEN TODAS DENTRO DE UN INLINE
  NO SE VE: SE DESPEJA.** En `SphereVsBox` los tres `Sub`/`Subxyz` iniciales no
  llevan ni una linea de `Geometry.cpp` (todo su codigo es de `UMath.h`), asi que
  su posicion en el fuente hay que deducirla de **entre que dos lineas visibles
  cae**. Poner `radius` (linea 427) y los dos `= kIdentity` (428/429) **detras**
  de las tres llamadas da 97,71% con tamano exacto; ponerlos delante -la lectura
  ingenua del numero de linea- baja a **88,61%** y descuadra el prologo entero.
  Medido en las dos direcciones.
- **DOS `if (...) return false;` SEGUIDOS QUE EL MAPA DE LINEAS PONE EN LA MISMA
  LINEA SON UN `||`.** En `SphereVsBox` la linea 621 cubre las dos comparaciones
  (`coldist > radius + v_dot` y `!(coldist > 0.000001f)`); escritas como dos `if`
  sobra un `li r3,0` y 4 B, y con `||` el tamano queda **exacto**. Es la familia
  de `AreMomentCamerasEnabled`, pero el delator no esta en el diff: es que **las
  dos comparaciones comparten numero de linea**.
- **UN BLOQUE DEL VOLCADO CUYOS HIJOS SON LOS `if` INTERIORES *ES* UN `if`, NO EL
  CUERPO DEL BUCLE — Y ESO SE LEE SIN COMPILAR.** En
  `EmitterSystem::UpdateParticles` (1.820 B) `regmap` cantaba **11 locales a la
  profundidad equivocada y 12 desajustes del arbol**. El volcado ponia las 15
  locales del cuerpo (`pvel`, `pacc`, `ppos`, `col`, `r/g/b/a`...) en un bloque
  cuyos **hijos** eran los bloques de `if (texture_animation)` y de
  `if (ed_drag > 0.0f)`: o sea que ese bloque es el cuerpo del
  `if (particle->mLife > time_step)`, no el del `for`. **Las declaraciones van
  DENTRO de la guarda.** Ademas el volcado traia **dos** bloques distintos
  declarando `to_kill`, uno hijo del bloque de las 15 locales y otro hijo del
  bloque del `for`: eso es la instruccion literal de **escribir la cola dos
  veces** —una dentro del `if`, en sus propias llaves y seguida de `continue`, y
  otra detras del `if`—. Los dos cambios juntos: **99,396% -> 99,571%, de 47 a
  31 diffs, tamano exacto**, y `regmap` de 12 desajustes de arbol a 4.
  **Regla: cuenta los HIJOS de cada bloque del volcado contra los bloques
  anonimos que abren tus `{ }` — `regmap` lo imprime en ESTRUCTURA DE AMBITOS y
  es lo primero que hay que igualar.** *(Al 2-sep la funcion va por 99,96%: los
  99,571% del texto son de la ronda en que se escribio.)*
  **Acotacion medida en la misma funcion**: el desajuste que quedaba
  (`emitters` un nivel mas hondo, con un `bTList<Emitter> &emitters` sombreando
  a otro) se puede escribir tal cual y **baja a 95,64% con 95 diffs**. No todo
  desajuste de arbol que canta `regmap` paga: el de las declaraciones si, el del
  sombreado no.

- **EL DESAJUSTE DE ARBOL DE `regmap` GANA AL VEREDICTO DE REGISTROS, Y HAY DOS
  FORMAS DE FUENTE QUE LO PRODUCEN SIN QUE SE VEAN.** En una ronda de zPhysics,
  **tres de cuatro cierres** salieron de "el bloque X SOBRA/FALTA", no del
  reparto. Las dos formas:
  1. **`if (T x = f())` mete `x` en un nivel PROPIO**, entre el cuerpo del
     bucle y el cuerpo del `if`. Si el volcado te da `max_level` en `b0/b0/b0`
     y `f_index`/`level` en `b0/b0/b0/b0`, eso es exactamente una declaracion
     en la condicion del `if`, no `int x = f(); if (x > 0) {...}`.
  2. **Un iterador declarado en la CABECERA del `for` se lleva un nivel de
     mas.** `for (T it = X.Iterator(); it.Valid(); it.Advance())` deja `it` en
     su propio bloque y el cuerpo un nivel mas hondo; el original lo declara
     antes y escribe `for (; it.Valid(); it.Advance())`. Es lo que alineo
     `PUPartNode` (73,05 -> 84,38%). **Pasarlo a `while` con el `Advance()` al
     final BAJA** (82,45%): el `for` con primera clausula vacia es la forma.
- **EL ORDEN DE LOS GRUPOS DE `case` ES EL ORDEN DE LOS CUERPOS EMITIDOS, Y EL
  `.s` DEL TROCEADOR LO DA RESUELTO.** En `Upgrades::MatchPerformance` los
  cuatro grupos estaban emparejados al reves (el cuerpo de una linea antes que
  el de dos terminos en cada par). Se lee sin adivinar: resolver los destinos
  del arbol de despacho en el `.s` (`beq .L_xxx`) y ordenar las etiquetas por
  direccion da el orden del fuente. **92,50 -> 95,50% solo con reordenar los
  cuatro grupos**; los valores de los `case` y el arbol ya casaban.
- **UNA SENTENCIA DE MATRIZ CON `fadds fX,fX,fX` (DOBLAR SUMANDO) NO ES UNA
  EXPRESION ESCRITA A MANO: ES `QuaternionToMatrix4`.** `Collection::AddNode`
  llevaba las nueve componentes `matrix[i][j] = 1.0f - 2.0f*(...)` escritas a
  mano y el objetivo tenia `VU0_quattom4` inlinado entero. El delator es que
  el objetivo dobla con `fadds fX,fX,fX` donde `2.0f * x` emite `fmuls` contra
  una constante del pool; el volcado lo confirma listando la expansion. **Un
  solo `UMath::QuaternionToMatrix4(orientation, invmat)` valio de 85,63% a
  95,05% y quito los 36 B que faltaban.**

## 4. Locales, marco y registros

- **LA FIRMA DEL MARCO: +8 B DE LOCALES Y UN SALVADO MENOS EN EL OBJETIVO = UN
  PSEUDO QUE SE DERRAMA.** Medida identica en dos funciones de la misma familia
  (`FnDeltaQFast::EvalSQT` 0x78/r17..r31 contra nuestro 0x70/r16..r31;
  `FnStatelessQ::EvalSQT` 0x60/r27..r31 contra 0x58/r26..r31). Cuando ese pseudo
  pierde el registro, reload pliega la direccion al hueco directo, el `stfs` deja
  de tener base en registro, **el CSE del slot sobrevive y desaparecen cuatro
  instrucciones de golpe**. Los bytes "sin referenciar" del objetivo **son ese
  hueco mas el relleno**, no una construccion de la fuente.
  **Y GCC 2.9 SI produce ese patron con nuestra fuente**: tres funciones de la
  misma unidad al 100% lo llevan, y sus gemelas sin la guarda `if (...)` no. Pero
  **no es forma de fuente, es presion de registros**: en sinteticos, cuatro
  conversiones con un solo bucle lo producen y con bucles anidados no.
- **UNA LOCAL MUERTA NO FUERZA NADA, DE NINGUN TAMANO.** Mi regla de que "un array
  local muerto de mas de 8 B quita el `addi`" **es falsa**: GCC elimina la local
  entera si no se referencia, den 8, 12, 16 B o un `double`. Lo que crea el hueco
  es el derrame de un pseudo VIVO.


- **UNA LOCAL SIN INICIALIZAR CUYA UNICA ESCRITURA ES UNA SALIDA DE `asm` LLEVA
  `= 0.0f` EN EL ORIGINAL.** El `lis` del literal sobrevive muerto y CSE lo
  reutiliza mas abajo. En `bAbs(float)` de `bMath.hpp` valio **5.212 B en dos
  unidades**, y el A/B por objetos sobre las 32 compilables dice que las otras 30
  salen byte a byte identicas. **El `asm` se queda; solo cambia el inicializador.**
  Y no generaliza a ciegas: la misma idea en `bMin`/`bMax` cuesta **-10.636 B**.

- **`x = x && cond;` CONTRA `if (x) { x = false; if (c) x = true; }`.** Firma en
  el objetivo: **`cmpwi rX,0` ; `li rX,0` ; `beq`** -- el reset ejecuta ANTES del
  salto, en los dos caminos; con la forma anidada el `li 0` cae dentro del if.

- **`c ? f(A) : f(B)` Y NO `f(c ? A : B)`.** Con la ternaria en el argumento sale
  `lis r9,S@ha; addi r3,r9,S@l`; con la llamada duplicada el cross-jumping funde
  el `bl` y el argumento cae directo en r3. Firma: un brazo que es exactamente
  `lis rA,S@ha; addi rA,rA,S@l; b L`. **Veta agotada: 99 apariciones en el juego,
  las 99 ya al 100%.**


- **UN `stw` CONTRA LA DIRECCION BASE EN VEZ DEL DESPLAZAMIENTO DE MARCO DELATA UN
  INDICE VARIABLE** que CSE pliega a 0. `v[0] = x;` da `stw r0, 0x8(r1)`;
  `n = 0; v[n++] = x;` da `stw r0, 0x0(rN)` -- vale **una referencia mas al pseudo
  de la direccion**, y con eso ese pseudo **gana el registro**. Fue el ultimo paso
  de una funcion de 1.992 B.
- **EL ORDEN DE DECLARACION DENTRO DE UNA TIRADA DE ASIGNACIONES GEMELAS DECIDE EL
  REPARTO DE FP.** Bajar dos `min` por debajo de sus `max` subio 97,33 -> 98,87%
  **sin tocar una operacion**. (13 ordenes barridos; ninguno otro mejora.)
- **EL ORDEN DE EMISION Y EL REPARTO ESTAN ACOPLADOS:** con una barrera en un
  orden los registros salen intercambiados y el orden casa; con el otro, al reves.
  12 formas y ninguna cierra las dos cosas a la vez.
- **LA FRONTERA DE `floor_log2(n_refs)` (8, 16, 32 REFERENCIAS) ES UN ESCALON REAL
  EN EL REPARTO.** Una **reasignacion muerta** cuesta 2 refs: al eliminarse dejaba
  una local con **14 refs (log2=3)** en vez de **16 (log2=4)**, su prioridad caia
  un 34% y otra variable le robaba r30. Escribiendo la expresion en **una sola
  llamada** la asignacion sobrevive: 98,77 -> **100%**.
- **ESTE GCC NO HACE NRV PARA `T x = f()` CON `f` INLINE QUE DEVUELVE STRUCT:**
  crea una ranura nueva (marco +0x10) y copia. El DWARF del original pone el
  resultado del inline **en la ranura de la variable**, asi que la forma es
  correcta y **el compilador no la reproduce**: cinco formas medidas, la base
  (96,09%) es la mejor.
- **UN `addi rN, base, off` IZADO A UN SALVADO Y USADO EN *UN SOLO* STORE = UN
  PUNTERO EN EL ORIGINAL USADO EXACTAMENTE AHI.** Con 17 stores identicos al mismo
  campo, declarar `unsigned int *p = &obj->campo;` y usarlo en **uno** paso de 55
  a **8 diffs**, arreglo el marco exacto (0x80 -> 0x78, `stmw r18` -> `stmw r17`)
  y elimino **12 B de ranuras caller-save muertas**. La veda anterior --«la
  referencia arregla el marco pero usa el pseudo en los 17 stores»-- fallaba por
  **usarla en todos**.
- **`char x[2]` ES HImode EN GCC 2.9, NO BLKmode:** vive en un **pseudo** y solo
  recibe ranura de pila cuando alguien **toma su direccion**. Un
  `*(unsigned short*)x = ...` fuerza `put_var_into_stack` pronto, coloca la ranura
  **antes** de la vecina (descuadrando todos los desplazamientos) y crea un pseudo
  `&x` que se iza al prologo y se recarga seis veces. Con `x[0] = 'e';` GCC emite
  **el mismo** `lbz/ori/sth` y el puntero desaparece: **+0,8 puntos y −32 B en una
  edicion**. Vale para cualquier array de 1/2/4/8 B leido con un cast de puntero.
- **`__asm__("")` ES TAMBIEN BARRERA DE PLANIFICACION, NO SOLO DE
  CROSS-JUMPING.** Entre `p[i] = v;` y `return &p[i];` mantiene el `add` detras
  del `stwx`. Cero bytes, y cerro `ScratchPtr<T>::_Alloc` (216 B, 91,11 ->
  **100%**, dos instanciaciones) **rompiendo una veda que decia «5 formas,
  salida identica»**. Moraleja: cuando cinco formas de la SENTENCIA dan el mismo
  binario, la palanca que queda no es una forma de la sentencia — es una barrera
  ENTRE dos sentencias.
- **LA DECLARACION DE UNA REFERENCIA (`const T &x = ...`) ES UNA PALANCA DE
  COLOCACION DE BLOQUE:** moverla a traves de un `if` mueve su calculo de
  direccion de bloque basico. Fue el cuarto de los cinco pasos que cerraron
  **1.784 B**.
- **EL INTERMEDIO QUE CUESTA UN CICLO DE REGISTROS.** Una local con `-` (sin
  localizacion) en el original y **con registro** en lo nuestro, mas una rotacion
  **limpia de N posiciones** en `regmap`, significa que el original **acumulaba
  sobre la variable existente** en vez de crear el intermedio:
  `x = x + d;` en vez de `float nuevo = x + d;`. La referencia de mas sube la
  prioridad de la local en `allocno_compare` (~`log2(n_refs)*n_refs`) y ahi estaba
  la rotacion entera. **1.516 B en dos funciones, las dos a la primera
  compilacion.**
- **UN `p + i` CON `i` VARIABLE DE VALOR 0 NO SE PLIEGA A `p`:** emite
  `addi rD,rS,0` y **mantiene vivo el registro con el 0** para que GCC lo reutilice
  en los `= 0` de al lado. Escribirlo como `p->campo` cuesta **tres `li 0` de**
  **mas**.
- **PARA DOS CONSTANTES INVARIANTES IZADAS AL MISMO PREENCABEZADO, LA DEFINIDA MAS
  TARDE SE LLEVA EL REGISTRO MAS ALTO**, y eso **no cambia** si inviertes el orden
  de sus usos dentro del bucle. **El orden de USO no es una palanca; el de
  DEFINICION en el preencabezado si.** (`REG_ALLOC_ORDER` descendente, confirmado
  con un micro-test de 4 funciones y 2 s.)
- **EL ORDEN DE DECLARACION MUEVE EL ORDEN DE EMISION DE LOS INICIALIZADORES PERO
  NO EL REPARTO DE REGISTROS.** Intercambiar dos locales movio sus `li 0` --
  creando 2 diffs nuevos-- y dejo los registros exactamente igual.
- **UN MARCO QUE NO CUADRA PUEDE SER PRESION DE REGISTROS, NO LOCALES MUERTAS.**
  12 B de pila **jamas referenciados** eran ranuras de *caller-save* que GCC
  reserva cuando le falta un callee-saved. Se aisla leyendo `stwu 1,-N(1)` del
  `.s` con tres sondas, sin objdiff: aparecian en cuanto existia **cualquiera** de
  dos llamadas.
- **UN BLOQUE LEXICO DE RANGO CERO EN EL DWARF SIGUE RESERVANDO MARCO.** Faltaba
  un bloque muerto con un objeto de 0xD0 B: exactamente la diferencia 0x560/0x4D0.
  **Si el marco no cuadra, mira ahi antes que al asignador.**
- **UN INLINE VACIO EN UNA CABECERA HACE QUE GCC BORRE EL ARRAY QUE LE PASAS.** Si
  la direccion no escapa porque el receptor no tiene cuerpo, desaparecen los 64 B
  de marco. Se arregla dejando el `esp*` como **declaracion sin cuerpo** -- pero es
  cabecera compartida por todo el juego: medir en todas.
- **UN LOCAL DECLARADO Y SIN USAR ES LA FIRMA DE UNA SENTENCIA QUE FALTA, Y EL
  SITIO DE LA ASIGNACION DECIDE.** `AssignCopsInCircle` tenia `float step;` muerto
  y el DWARF le daba f27. Asignarlo **en la declaracion** da 79,32%; el mismo
  `step = ...;` **justo antes del segundo bucle** da **100%**.
- **LA FORMA EN DOS SENTENCIAS COALESCE LO QUE LA ANIDADA NO.** `x = f(); x =
  bMin(x, y);` elimina un `mr` que `x = bMin(f(), y)` deja; fue el ultimo paso de
  una funcion de 840 B (99,17 -> 100%).
- **EL TAMANO DEL MARCO DA EL TAMANO DE LOS ARRAYS LOCALES.** Marco 0x28 con
  `stmw r28, 0x18(r1)` y locales desde 0x8 -> exactamente 16 bytes -> `char
  str[16]`. Nosotros teniamos 0x30.
- *(pista, sin cifra)* **UN `addi rN, r1, off` DE MAS EN EL OBJETIVO SUELE SER
  UNA REFERENCIA EN NUESTRA FUENTE.** `T &alias = obj;` obliga a guardar la direccion en un
  callee-saved; el objetivo la **recalcula** en el punto de uso.
- **UNA LOCAL MUERTA *NO* REORDENA EL POOL; UN INICIALIZADOR MUERTO SOBRE UNA
  LOCAL QUE SE USA, SI. ESE ES EL DISCRIMINANTE.** Es el par de entradas que mas
  veces se ha leido mal.
  - **Local entera muerta (declarada y nunca usada): CERO efecto.** Medido dos
    veces — cuatro `float` muertos del DWARF en `Convert16To32` y dos `bool` en
    `AdvanceAnimTime`: **cero cambio en el binario**. Declarar basura no es un
    dial.
  - **Inicializador muerto sobre una local que SI se usa: crea la entrada del
    pool y la coloca.** `float secondsElapsed;` -> `= 0.0f;` cerro
    `VisualLookEffect::UpdateActive` (**300 B**, 97,27 -> 100%) porque metio el
    `0.0f` delante del sesgo del timer; poner `= 0.0f` en cuatro declaraciones
    de una funcion fundio dos `lis $LC@ha` en un solo registro base (93,44 ->
    97,91%); y los `bool x = false;` de `Moment::React` borraron 58 de 117
    lineas de diff sin mover una instruccion.
  **Regla: la palanca es el INICIALIZADOR de una variable viva, no la
  declaracion de una variable muerta.** (La unica local muerta que paga es la
  que **mantiene vivo un valor** — vease «el contador muerto» mas abajo.)
- **EL DWARF NO LISTA LOS TEMPORALES QUE GCC COALESCE, ASI QUE «ESTA LOCAL NO
  ESTA EN EL VOLCADO» NO AUTORIZA A BORRARLA.** Medido dos veces en la misma
  ronda: quitar `wheel_y_offset` bajo de 96,78 a **95,16%**, y quitar
  `params`/`table`/`zero` de `DefragmentPool`, de 99,27 a **92,47%**.
- **UN MARCO DE PILA DISTINTO CON LOS MISMOS OFFSETS USADOS NO SE VE EN objdiff:
  SE LEE EN EL `stwu`/`stmw` DEL PROLOGO.** Puede ser pila MUERTA nuestra (un
  `Vector3` temporal de mas) o del objetivo (12 B y un GPR salvado mas).
- **UN OBJETO CON CTOR Y DTOR VACIOS DECLARADO EN UN BLOQUE RESERVA MARCO**, y el
  delator es un **corrimiento UNIFORME** en TODOS los desplazamientos. 8 B sin
  emitir una instruccion.
- **LA POSICION DE UN `bMatrix4 m;` DECIDE QUE RANURA SE LLEVAN LOS TEMPORALES DE
  BLOQUE.** Declarado antes de los bloques que usan temporales, ocupa 0x40..0x80 y
  les deja 0x80.
- **`delete p; p = 0;` SIN `if` EXPLICITO:** `delete` emite su propio test de nulo
  y el store queda **fuera**, en la junta, con el `lis` rematerializado. Con
  `if (p) { delete p; p = 0; }` GCC CSE-a `&p` a un salvado.
- **`bFill(&v, x, y, z)` EN VEZ DE TRES ASIGNACIONES DE CAMPO METE `&v` EN UN
  SALVADO** (toma la direccion explicitamente).
- **`const char *fmt = "...";` DECLARADO ANTES DE LA LLAMADA QUE LE PRECEDE LO METE
  EN UN SALVADO** (`mr r5,r30`) en vez de materializarlo en linea.
- **UN `subi rSalvado,rX,1` IZADO POR ENCIMA DE UNA RAMA ES UN LOCAL**
  (`int prev = i - 1;` antes del `if`): 97,73 -> 99,99% y arreglo 8 registros.
- **UN ARGUMENTO QUE ES EL PARAMETRO EQUIVOCADO ES INVISIBLE PARA objdiff SI CAE
  EN EL MISMO REGISTRO; LO DELATA LA PERMUTACION DE SALVADOS DEL PROLOGO.**
- **N STORES EN ORDEN DE DECLARACION Y TODOS EN LA MISMA LINEA DEL MAPA = LISTA
  DE INICIALIZACION, NO ASIGNACIONES EN EL CUERPO.** El delator secundario es
  `lwz -9`: con la lista, CSE se queda las tres palabras de una constante
  (`Vector3::kZero`) **en registro** para las cuatro copias. 344 B al 100%.
- **CSE QUE SUSTITUYE UN PARAMETRO `bool` POR LA CONSTANTE 0 DE LA RAMA `else` LE
  ALARGA EL RANGO DE VIDA Y LO DERRAMA.** `if (simple_physics)` deja que CSE use
  el parametro como el `0` de una llamada **490 instrucciones despues**; su
  prioridad en `global_alloc` se hunde y va a pila. **`if (x == true)` rompe la
  deduccion** (CSE solo aprende `!= 1`) y vuelve al salvado que dice el DWARF:
  98,02 -> 99,26%, de **65 a 26 diffs**.
- **CUANDO DOS EXPANSIONES DEL MISMO INLINE NO SE CROSS-JUMPEAN, MIRA SI EL FUENTE
  USA DOS EXPRESIONES PARA EL MISMO VALOR.** `Link(node->GetHead(), node)` contra
  `Link(mRoot->GetHead(), mRoot)` —equivalentes— decide si CSE puede fundir la
  carga con el argumento. Con la segunda, las dos expansiones acaban con el mismo
  registro y el cross-jumping post-reload funde las colas: **92,93 -> 99,30% con
  tamano EXACTO**.
- **UN `bClamp` DUPLICADO SE LEE COMO `fsel -2, fsubs -1`.** Escrito dos veces, GCC
  lo rematerializa en el uso; el objetivo lo CSE-a en el registro que el DWARF da
  a la otra local (**compartido**). Ligandolo una vez: tamano **de +8 B a EXACTO**
  y el delta de `fsel-2, addi+1, fsubs-1` a **solo `addi+1`**. **Y el porcentaje
  BAJA** (94,45 -> 93,68%): otro caso de «objdiff premia el error» — **ahi el juez
  es el TAMANO**.
- **UN `flag || (!A(X) && B(X))` CON `flag` CONSTANTE-VERDADERO DEJA UNA ESTRUCTURA
  DE BLOQUES QUE IMPIDE A CSE DEMOSTRAR QUE DOS INDICES SON IGUALES.** Plegabamos
  cuatro `arr[i]` a **dos `lwz` con desplazamiento constante** donde el objetivo
  emite **cuatro `lwzx`** con dos registros de indice. Delator exacto:
  `lwzx +4, lwz -2, li +2, addi +2`. 96,29 -> 98,63%, de 42 a 15 diffs.
- **DECLARAR EL INVARIANTE ANTES DEL `if` QUE CALCULA EL OTRO LIMITE PARTE SU RANGO
  DE VIDA.** Subir `int wrap = numPoints - 1;` por encima de `int numSegments =
  numPoints;`: **52 -> 34 diffs**, y arreglo `stmw` y marco de golpe.
- **UN `if (cond) { <cuerpo entero de la funcion> }` ES INVISIBLE PARA objdiff Y
  DESPLAZA DECENAS DE LOCALES DE AMBITO.** El original usa **early return**.
  Byte-neutro, pero: ambito equivocado 59 -> 3, desajustes de bloque 2 -> 0,
  locales iguales 21 -> 68, y el veredicto de `regmap` pasa de **ESTRUCTURA a
  REPARTO**. Es el paso previo a poder atacar el reparto de verdad.
- **`ProfileNode profile_node("TODO", 0);` ES EL PREAMBULO ESTANDAR DE EA** y falta
  en varias funciones de Ecstasy. `regmap` lo nombra como «SOLO EN EL ORIGINAL
  (struct ProfileNode)» y el arbol ya tiene decenas de usos que copiar.
- **UNA ROTACION CICLICA DE REGISTROS EN LA COLA DE UN static-init ES LA SOMBRA DE
  UN VALOR MAL, NO UN TECHO.** Un `bVector2 T(-3.0f, -3.0f)` que era `(0.0f, 0.0f)`
  cerro **9.380 B** de golpe. El mecanismo: con `-3,0` hace falta un pseudo nuevo
  cuyo rango **cruza un `bl`**, lo que obliga a un registro SALVADO y corre la
  asignacion de f27/f28/f29 en toda la cola; con `0,0` el valor **ya estaba vivo**
  en f30. **Metodo: decodifica del stream del static-init QUE VALOR escribe el
  objetivo en cada global y contrastalo con la fuente.**
- **MWCC REPARTE LOS SALVADOS POR ORDEN DE DECLARACION EN DOS GRUPOS** (r31 hacia
  abajo y luego r26 hacia abajo). Reordenar las 8 locales de una funcion cerro sus
  23 diffs **de golpe**.
- **UN INLINE DE CABECERA PUEDE NECESITAR DOS FORMAS INCOMPATIBLES.** La misma
  funcion con `if (ok) return ptr; return 0;` gana **+1.116 B** en una unidad y
  pierde **-2.432 B** en otra. **Un cambio de cabecera se mide con la BIBLIOTECA
  ENTERA, nunca con una unidad.**
- **`local_alloc` DA r30 Y `global_alloc` DA r31: UNA FUNCION DE UN SOLO BLOQUE
  BASICO NUNCA PUEDE USAR r31.** Micro-banco de 12 formas: `void A::U(){S();V();}`
  da **siempre** `stmw r30,8(1)`/`mr r30,r3`; con dos bloques (`if (f) S(); V();`)
  sale `stw r31,0xc(r1)`/`mr r31,r3`. **Delator inmediato: si el objetivo salva UN
  registro con `stw r31` y nosotros dos con `stmw r30`, al fuente le falta un
  BLOQUE BASICO, no una local.** Descartadas como generadoras de bloque vacio:
  `if (0){}`, `if (G){}`, `goto L; L:`, `{S();}{V();}`, `asm("")`, `for(;;)`.
- **UNA CONVERSION int->float DE UN CAMPO `unsigned` USA OTRO SESGO** (`0x4330...0000`
  sin `xoris`) que la con signo, y eso cuesta **un FPR salvado y 8 B de marco**.
- **UN `= false` DE MAS ROTA EL POOL DE DERRAMES.** 58 de los 60 diffs de una
  funcion de 5.668 B eran una **rotacion ciclica uniforme de -4 en 16 ranuras**, y
  se disolvieron **quitando dos inicializadores**. **Esto vuelve a corregir la
  entrada que da la rotacion pura por techo: antes de aceptarla, busca un
  inicializador que sobre.**
- **EL VOLCADO CUENTA LAS LOCALES, NO SOLO LAS NOMBRA.** Dos locales del original
  con el mismo registro donde nosotros reusabamos una = **split**. En una funcion
  de 756 B eso solo basto — y de propina aparecio el `mfcr`/`stw r12` del
  objetivo: **al acortar el rango de vida, GCC izo la comparacion al campo CR
  salvado**.
- **LIGAR EL PRIMER ELEMENTO DE UN ARRAY TEMPORAL A UNA REFERENCIA METE SU
  DIRECCION EN UN SALVADO.** `Vector4 &t0 = tmp[0];` cerro dos plantillas de
  golpe; `Vector4 *p = tmp;` vale igual, y **cuatro variables sueltas NO** (28
  diffs).
- **EL MARCO RESERVADO POR UN BLOQUE LEXICO MUERTO VALE 40 DE 75 DIFFS.** Un
  `if (RemoteCaffeinating && ...)` con `static const int RemoteCaffeinating = 0`
  es muerto en compilacion, pero GCC 2.9 lo expande y **reserva el marco**: el
  objetivo usa 0x120 y nosotros 0xe0, **64 B exactos**, con el mismo tamano y el
  mismo multiconjunto. Rellenar el bloque muerto hace casar **todos** los
  desplazamientos de pila. Hermano del truco del `if (0) { ... }` literal.
- **DECLARAR EL PUNTERO DENTRO DEL BUCLE lo convierte de cantidad MULTI-BLOQUE
  (`global_alloc`) en cantidad de UN BLOQUE (`local_alloc`)** — y `local_alloc`
  corre **antes** y se lleva **r11**, cabeza de `REG_ALLOC_ORDER`. Fue el paso
  final que cerro `eDataRender::Render` (5.980 B): de 20 diffs a **0**.
- **En MWCC un ARRAY local sin usar reserva marco; un escalar sin usar no**
  (`double d;` se elimina, `u32 pad[4];` cuesta 16 B). Y **MWCC reparte las
  locales de la direccion ALTA a la BAJA en orden de declaracion**. Sirve para
  reconstruir el marco de una funcion cuyo objetivo tiene locales muertas.
- **Un `__asm__("")` VACIO detras de una llamada es una BARRERA DE CROSS-JUMPING
  DE CERO BYTES.** GCC compara las colas de los bloques y el insn de asm las hace
  distintas. Recupero los 4 B que separaban a una funcion de su tamano exacto
  **tras fallar 14 formas de fuente**.
- **UN VLA DECLARADO CON UNA VARIABLE ENMASCARA EL TAMANO DEL `alloca`; `alloca()`
  NO.** `char buf[n]` emite `((n & 0x1FFFFFFF) + 14) & 0x3FFFFFF8`; el objetivo
  emite `((end-start) + 24) & ~7`. Escribirlo con `alloca` lo reproduce:
  89,78% -> 96,17%.
- **UNA CARGA IZADA POR ENCIMA DE UN TEST DE NULO EN EL OBJETIVO SIGNIFICA QUE EL
  FUENTE CARGA PRIMERO**, aunque sea un deref de NULL latente. El planificador
  **no puede** subir una carga por encima de un salto, asi que la posicion es del
  fuente: `int c = *t;` **antes** de `if (s == NULL) return 0;`. Cerro dos
  funciones distintas en la misma ronda.
- **EL INICIALIZADOR FLOTANTE MUERTO DECIDE CUANTOS REGISTROS DE DIRECCION `@ha`
  RECIBE UN LITERAL. Vale 4,5 puntos.** No es solo que ordene el pool: **la
  ranura del pool decide los registros base**. Emitiamos **dos** `lis $LC@ha` para
  el mismo `0.0f` (uno izado, otro rematerializado dentro del bucle), quemando un
  allocno y forzando un `andi.` **duplicado**. Poner `= 0.0f` en cuatro
  declaraciones puso el `0.0f` a la cabeza del pool —como en el objetivo— y fundio
  los dos en un solo registro base: **93,44 -> 97,91%**.
- **ARREGLA EL POOL ANTES DE BARRER EL ORDEN DE SENTENCIAS, O EL BARRIDO OPTIMIZA
  CONTRA LA LINEA BASE EQUIVOCADA.** Misma funcion, mismos tres ordenes: con el
  pool mal, subir `tex_coordX` ganaba (+4,2 pts) y el orden del mapa de lineas
  perdia; con el pool bien, **el orden del mapa de lineas gano por otros 5,5 pts**
  (93,44 -> 98,99%). **El barrido se invirtio entero.**
- **EL BARRIDO DEL «REGISTRO QUE NADIE ESCRIBE» HAY QUE CORRERLO TAMBIEN SOBRE
  f0-f31.** `fabs f1, f24` con f24 **jamas** destino es un accesor **flotante**
  vacio. 1.788 B. (`deadreg.py` ya excluye solo f1-f8, que son los de argumento.)
- **`1.0f` y `0.0f` EN LOCALES NO SE PLIEGAN**: CSE sustituye el **registro**, no
  la constante, asi que `body_blend*x + heading_blend*y` sobrevive como
  `fmuls`/`fmadds` en vez de plegarse.
- **PARTIR `x = A()*B/C` EN `x = A(); x = x*B/C;` COALESCE EL TEMPORAL CON EL
  DESTINO** (`lfs f30` / `fmuls f1,f30,f1` / `fdivs f30`).
- **`GetHead()` MUERTO DELANTE DEL BUCLE DE `RemoveHead()`: 1.748 B A LA PRIMERA.**
  El DWARF expandia `bTList<T>::GetHead()` en el **preheader** (rango cero) y
  `bList::IsEmpty()` en la cabeza del bucle: **`GetHead` ANTES que `IsEmpty` es la
  firma**. Escribir `part = lista.GetHead();` justo antes del `for` hace que el
  pseudo nazca en el preheader y deje de coalescerse con la carga del test — que
  es el `mr` que le sobraba al objetivo.
- **`static volatile int` ES LO UNICO QUE SOBREVIVE COMO CARGA MUERTA.** Un `lwz` a
  un estatico cuyo destino se pisa acto seguido —y que ademas se repite DENTRO de
  un bucle— solo sale de `volatile`; y que sea `lwz` y no `lfs` prueba que el tipo
  es **entero** aunque el nombre diga `xLeft`/`yTop`. Micro-banco de 4 formas:
  `static float` no, `static volatile float` da `lfs`, `volatile` en el local no.
- **UN BLOQUE LEXICO DE CODIGO MUERTO SIGUE RESERVANDO MARCO.** Si al objetivo le
  sobran 16 B de marco y todos sus desplazamientos van corridos, `plan.py` puede
  ensenar un **bloque de rango CERO** con temporales que GCC elimino pero cuyo
  espacio reservo. Reponerlo realinea el marco entero.
- **Y SE REPONE CON `if (0) { ... }` LITERAL: GCC 2.9 EXPANDE EL CUERPO Y LUEGO LO
  BORRA.** El front-end no salta el cuerpo de un `if` con condicion constante: lo
  expande a RTL —y ahi `assign_stack_local` ya subio `frame_offset`— y es
  `jump_optimize` quien borra despues el codigo inalcanzable. Resultado: cero
  instrucciones emitidas y el marco reservado, que es exactamente lo que hace el
  objetivo. En `CubicCameraMover::Update` (3.776 B) reponer asi el bloque de rango
  cero que `plan.py` enseñaba —`float seconds`, un bloque anidado con `float t`, y
  dentro dos `bVector3::operator*` y un `operator+`— llevo el diff de **236 a 104
  lineas con el tamano clavado**: los tres `fCurve` pasaron de 0x1B0/0x1B4/0x1B8 a
  0x1C0/0x1C4/0x1C8 y `mWorldToCamera` de 0x170 a 0x180, o sea el area entera de
  locales con nombre alineada de golpe. **El truco es que basta con reproducir los
  TEMPORALES** (cuantos `dest` de inline y de que tipo), no la sentencia real.
- **CONTAR EL MARCO: `vars = frame - 8 - 4*nGPR - 8*nFPR`, Y NUNCA MIRAR SOLO EL
  `stwu`.** Bisecando una funcion por trozos, quitar codigo que solo deja de usar
  **un registro flotante salvado** baja el `stwu` en 8 B y parece que has quitado
  una local: no has quitado nada. Hay que leer a la vez el `stwu`, el `stmw rN` (de
  ahi salen los GPR) y **cuantos `psq_st` hay** (de ahi los FPR). Con eso el
  `get_frame_size` de los dos lados es una resta, y ahi si se ve la local que
  sobra. Coste de no hacerlo: tres compilaciones de bisecion con la conclusion al
  reves.
- **UN REGISTRO «RARO» EN EL DESTINO DE UN `subi` PUEDE SER UN ARGUMENTO QUE
  FALTA.** Una funcion se quedaba al 99,71% con `subi r3,r3,0x10` donde el
  objetivo pone `subi r5,r3,0x10`; **seis formas** de la sentencia dieron el mismo
  numero. **r5 es el TERCER registro de argumento**: al `sprintf` le faltaba un
  argumento. Anadirlo cerro la funcion sin mover una instruccion de sitio.
  **Corolario: un destino de registro que no cede a ninguna forma de la expresion
  puede estar reservado para una llamada posterior.**
- **UNA COMPARACION INVARIANTE DECLARADA DENTRO DEL BUCLE SE IZA AL PREHEADER**
  (detras de la guarda); **declarada fuera, se emite delante**. Fue la diferencia
  entre 89,75% y 97,97%.
- **EL TEMPORAL ANONIMO DE UNA MACRO MATERIALIZA SU DIRECCION EN REGISTRO PROPIO.**
  `F(&g, NULLALLOCTVP)` da `addi r9,r1,off` + `mr r4,r9`; escribir el `TagValuePair
  tvp;` a mano con tres asignaciones pierde esas dos instrucciones. 73,97 -> 100%.
- **EL CONTEO DE EXPANSIONES INLINE DEL DWARF ES POR FUNCION, NO POR FAMILIA.** El
  volcado pedia **seis** `IsImmobile()` y cero locales; escribir las seis llamadas
  directas subio la hermana `Joint` de 99,35 a 99,995% y **bajo** la `Plane` de
  99,78 a 99,49%. **Copiar la forma de la funcion hermana es pista, no regla.**
- **UN INLINE QUE SOLO ES UN CAST IGUAL CAMBIA EL DIRECCIONAMIENTO.** El DWARF no
  listaba expansiones de `Vector4To3`; escribir la copia como
  `*reinterpret_cast<Vector3*>(&dst) = *reinterpret_cast<const Vector3*>(&src)`
  quito el `stwu` de auto-incremento: 94,93 -> 98,86% con tamano exacto.
- **`Singleton::Get()` ligado a una referencia contra llamado cada vez**: ligarlo
  borra las relecturas de `fObj`. **Contando los `lwz fObj` del objetivo sale el
  numero de llamadas del fuente.** 91,6% -> 99,4% con el tamano exacto.
- **`const T &x = arr[i];` (REFERENCIA) PLIEGA EL DESPLAZAMIENTO DEL MIEMBRO
  DENTRO DEL INDICE** -> `addi rIdx,rIdx,off` + `lwzx`. Un puntero, o
  `arr[i].campo` escrito directo, dan `add` + `lwz off(p)`. Tres funciones.
- **Declarar `bVector3 v(0,0,1)` con ctor en su sitio contra `bVector3 v;` arriba
  + tres stores**: mismas instrucciones, **otro reparto** en el `bCopy` posterior.
  Fue el ultimo paso de 99,95% a 100%.
- **Los `extsb` de mas delatan locales `char` donde el original tiene `int`.**
- **DOS TEMPORALES DEL MISMO TIPO EN EL CUERPO DEL CONSTRUCTOR REUSAN LA MISMA
  RANURA; EN LA LISTA DE INICIALIZACION VIVEN A LA VEZ Y CADA UNO SE LLEVA LA
  SUYA (+8 B de marco).** Dos `Hermes::Handler::Create` en el cuerpo daban el
  choque; moverlos a la lista cerro **1.400 B en tres funciones**. Medido en
  negativo: locales con nombre 83,5%, `const T &` 91,2%; **solo la lista vale**.
  Delator en el arbol: un comentario `stack issues ... reuses sp8`.
- **`bVector3` es `ALIGN_16`: `bVector3 v(0,0,0); this->m = v;` produce el
  temporal de 16 B de pila que el objetivo tiene.** `this->m = bVector3(0,0,0)`
  **no**, y escribir las componentes sueltas tampoco. 552 B.
- **UN `fmr`/`mr` DE MAS DEL ORIGINAL EN EL JOIN DE UN INLINE ES UNA LOCAL
  INTERMEDIA EN EL SITIO DE LLAMADA.** `x = f();` -> `T t = f(); x = t;`. La
  copia sobrevive porque el rango cruza bloques y **local-alloc solo coalesce
  dentro de un bloque**. Cerro 824 B a la primera, y su gemelo: **`global = f();`
  materializa el `lis` del global ANTES de la llamada** (en un salvado); partirlo
  en dos sentencias lo mueve detras.
  **ACOTADO: no vale para todo `mr` de mas.** En `bStrNICmp` el objetivo tiene un
  `mr r11,r3` de mas en `bToUpper(s1[-1]) - bToUpper(s2[-1])` y ligar los dos
  resultados a locales **empeora** (97,78% -> 96,27%, y de 248 a 244 B), y ligar
  solo el segundo tambien (96,11%). Cuando el `mr` esta dentro de la expansion de
  **dos inlines del mismo tipo en la misma expresion**, la copia es un reparto de
  registros, no una local: la palanca no existe ahi.
- **LIGAR `&local` A UN PUNTERO EXPLICITO ARREGLA EL REGISTRO BASE DE UNA COPIA
  DE STRUCT.** GCC elegia `r11` de base y lo pisaba con la primera carga,
  forzando las otras tres a `r1`-relativas; `T *p = &x; ... dst = *p;` deja la
  base en `r10` y la pisa la ULTIMA. **Ese solo cambio cerro 1.880 + 412 B.**
- **Ligar a un local el argumento que es una llamada mueve la evaluacion de los
  OTROS argumentos detras de ella.** Pago cuatro veces en una sola ronda.
- **Una local de tipo ALINEADO rematerializa su direccion**: un tipo normal
  CSE-a `&local` en un salvado; uno con `ATTRIBUTE_ALIGN(32)` emite
  `addi r3,r1,off` en cada uso. Y **un tipo alineado como destino de un retorno
  `sret` cuesta 16 B de marco y una copia entera**; el tipo base elide el sret.
- **Array local con inicializador contra `static const`**: si el objetivo copia N
  palabras de `.rodata` a pila **y su marco es mayor exactamente el tamano del
  array**, es `auto`.
- **El inicializador muerto flotante es del SITIO DE LLAMADA, y meterlo en la
  cabecera cuesta el doble de lo que gana.** Anadir `float zero = 0.0f;` dentro
  de `bSqrt` conseguia la forma correcta **y tiraba 4.024 B en otras siete
  funciones de la misma unidad**. Lo que vale es `float v = 0.0f; v = bLength(x);`
  **en el llamante**: el pseudo nace antes de la rama y CSE lo reusa. **2.188 B,
  coste cero fuera.**
- **`regmap` puede dar veredicto limpio y la funcion seguir a 92 diffs**: si las
  25 locales tienen el mismo registro en los dos lados, lo que queda es reparto
  de *scratch* y planificacion, que `regmap` no ve.
- **Un `asm` que REASIGNA SU PROPIO OPERANDO DE ENTRADA fuerza un derrame.** Dos
  `asm("fsel" : "=f"(v) : ..., "f"(v), ...)` encadenados mantienen vivo el `v`
  original y GCC lo derrama a pila — `stfs`/`lfs` de mas **dentro del bucle
  caliente**. Encadenar por un temporal (`r` para la salida, `t1 = r - vmax`) lo
  quita: 80,10 -> 83,06% y el tamano paso de +8 B a **exacto**.
- **El `extern const float lbl_XXXX` es un dial que funciona en LAS DOS
  DIRECCIONES, y hay que aplicarlo a los SITIOS EXACTOS.** Convertir **dos**
  `0.0f` concretos quito 40 de 62 lineas de diff; convertir **uno solo de esos
  dos**, cero efecto; convertir **los 30** de la funcion, 133 diffs (mucho peor,
  aunque el tamano casara exacto). Un barrido sitio a sitio sobre otra funcion
  (13 sitios): ninguno mejora, dos neutros, once empeoran. **No es global: es un
  ajuste por sitio.**
- **PARTIR LA ASIGNACION COMPUESTA ARREGLA PERMUTACIONES DE REGISTRO SALVADO EN
  TODA LA FUNCION.** `x = Max(x - a, b);` contra `x -= a; x = Max(x, b);` llevo
  una funcion de 98,85% a **100%**, arreglando un intercambio f29/f30 que
  `regmap` daba por permutacion del asignador. No es un efecto local: **sube
  `n_refs` de la variable y le cambia la prioridad en todo el rango**.
- **Un `fmr` de mas del original entre una llamada y una reasignacion de la misma
  variable es una LOCAL INTERMEDIA**, aunque el DWARF no la liste: el original
  guarda el valor viejo para usarlo despues.
- **Un iterador con nombre cuesta ranura de marco Y un store.** Cuando `regmap`
  marca SOLO NUESTRA sobre un iterador **con ranura de pila**, es esto (a
  diferencia de las locales sin ubicacion, que no valen nada).
- **`const` EN UNA LOCAL ES UN DIAL DE ROTACION DEL POOL DE DERRAMES.** Un solo
  `const float penetration` quito una rotacion de **cinco ranuras** y cerro
  `SuspensionRacer::DoWheelForces` (3.156 B) en una funcion con veredicto
  `IDENTICO` de `regmap`. **Esto corrige la entrada que daba la rotacion pura del
  pool por techo: SI cae.** Lo encontro el permutador guiado en 484 variantes y
  una ronda.
- **Los inicializadores muertos (`bool x = false;`) no emiten ni una instruccion
  y quitan la rotacion del pool**: en `Moment::React` borraron 58 de 117 lineas
  de diff sin mover una instruccion, y dejaron el tamano exacto.
- **DOS CONTADORES DE BUCLE QUE DEBERIAN SER UNO: un allocno de mas desplaza
  TODOS los salvados.** Delator: el mapa de registros es un **corrimiento
  uniforme de +1** (nuestro = objetivo + 1 en una tirada larga, r19..r24) y en
  UN sitio el objetivo **reusa el mismo registro para dos bucles disjuntos**
  mientras nosotros gastamos dos. La causa es que el fuente declara dos
  variables (`for (j = ...)` y `for (i = ...)`) donde el original usa **la
  misma**. Cambiar `j` por `i` en el primer bucle de `learn()` (NeuQuant) lo
  cerro de 98,86% y 33 diffs a **100%, 700 B**, sin mover una instruccion de
  sitio. Se detecta en segundos: si el corrimiento es uniforme, cuenta cuantos
  registros salvados usa cada lado (`stmw rN` del prologo); si nos sobra uno,
  busca dos locales del mismo tipo con rangos de vida disjuntos.
- **Una local reusada como intermedio en una rama anterior alarga su intervalo**
  — local-alloc mide nacimiento..muerte **sin huecos** — y bloquea que un
  temporal posterior comparta su registro. Fundir dos usos en una sola expresion
  libero `f31` y cerro `Joint::Resolve` (1.300 B).
- **`break` en vez de un `return X;` duplicado baja el conteo de referencias de X
  y le cambia la prioridad en local-alloc**; el codigo emitido es identico tras el
  cross-jumping, pero el reparto no.
- **La posicion de la declaracion fija el SLOT del literal en el pool, y el slot
  fija el registro de direccion.** Cuando dos `lfs` con la **misma base**
  (`@l(rN)`) difieren solo en el registro destino, la causa no es el asignador:
  es el orden del pool. Regla confirmada: **los registros de direccion (`lis
  @ha`) se reparten en orden de CREACION en el pool; los flotantes, en orden de
  USO dentro del bloque.** Bajar una declaracion **una linea** cerro
  `CheckLoadingBar` (812 B). Un `const float` no vale: pliega el literal y `fold`
  reasocia.
- **El inicializador muerto con `+=` DUPLICA la constante.** `float x = K;` da un
  pseudo; `float x = 0.0f; x += K;` da **dos**, y el planificador iza la carga del
  primero por encima del `bl`, dejando un `fmr` para la copia. Delator: al
  original le sobra un `lfs` mas un `fmr` de una constante que ya esta en
  registro. Nueve variantes en micro-banco, solo esa reproduce los tres roles.
  **3.024 B.**
- **`REG_ALLOC_ORDER` de rs6000 reparte los salvados en orden DESCENDENTE**
  (f31..f14, r31..r14), y `allocno_compare` ordena por
  `floor_log2(n_refs)*n_refs*size / live_length` con desempate por orden de
  creacion. Con eso **se predice** que constante se lleva cada registro sin
  compilar: la de vida mas corta, el registro mas ALTO libre. Sirve para separar
  techo del asignador de causa estructural.
- **Una local que `regmap` da como SOLO NUESTRA puede ser una particion de rango
  de vida de `regmove`, no una local que sobre**: quitarla bajo una funcion de
  99,35% a 98,12% **y anadio 12 B**, porque GCC rehizo el `lwz` dentro de los
  bucles. Y **una local solo del original con guion en las dos columnas no vale
  nada**: son `bool` muertos, y seis formas de reponerlos dan el mismo binario.
- **Los locales direccionables se llevan las ranuras BAJAS del marco** (se
  reservan en `expand_decl`, ascendiendo) **y los derrames de reload y los
  temporales del compilador las ALTAS.**
- **El idiom del temporal explicito (`__typeof__(x) t = expr; x = t;`) CUESTA una
  copia entera de 6 instrucciones.** Quitarlo cerro `Moment::React(const Plane&,
  int)` de golpe (97,62 -> 100%). *(Corrige una entrada anterior que decia lo
  contrario: aquella medida mezclaba dos cambios.)*
- **`size_t` es `unsigned int`, no `unsigned long`.** El bucle `while (n--)` con
  `unsigned long` emite SIEMPRE un `mr` sobrante y ningun flag lo quita. Un
  `typedef` valio **672 B**.
- **La declaracion `extern const float` con el almacenamiento aparte es la llave
  de fdlibm**: un `static float` con `__asm__` del nombre real, mas la
  declaracion `extern const`, da a la vez `@sda21` de UNA instruccion **y**
  semantica `RTX_UNCHANGING`, que es lo que hace que CSE lo mantenga en un
  salvado a traves de un `bl` y que gcse lo ice por encima de una rama. Un
  `static float` pelado da `@sda21` pero es memoria viva: ni se reusa ni se iza.
- **Para un array, la declaracion `extern const` tiene que MENTIR sobre el
  tamano**: solo los objetos de 8 B o menos reciben `SYMBOL_REF_FLAG`.
- **Un VLA se reserva DONDE ESTA LA DECLARACION, no al entrar** (65,6 -> 95,0%).
  Y **`sizeof` de un VLA no es la constante que lo dimensiona**: usa el tamano
  enmascarado que el `alloca` ya calculo, y eso vale un registro salvado.
- **`extern const float lbl_XXXX` CAMBIA EL REPARTO DE REGISTROS; el literal no.**
  El truco de declarar la constante agrupada como `extern const float` para
  forzar la referencia al simbolo exacto sale gratis en opcodes pero **no en
  registros**: GCC crea el pseudo en otro punto que con un literal, y el
  asignador reparte distinto. Sintoma inconfundible: *misma cuenta de
  instrucciones, mismos opcodes, solo cambian los numeros de registro*.
  Sustituir `lbl_803D3E24` por `0.0f` cerro `Friction::GetForce` de golpe
  (99,35 -> 100%). **`python scripts/pool2lit.py <fichero.cpp> --apply`** lo hace
  entero: lee el valor del ELF y busca el texto mas corto que reproduce el
  patron de bits. Pero **es una palanca local, no un barrido**; medido en los
  cinco ficheros que usan el truco: Collision.cpp +336 B, Geometry.cpp +0,3 pt,
  Articulation.cpp NEGATIVO, PhysicsUpgrades.cpp y UVectorMath.cpp cero. El
  literal casa donde el original **tenia** un literal; donde tenia otra cosa,
  empeora, y dentro de un mismo fichero puede repartirse en las dos
  direcciones.
  **AMPLIACION MEDIDA: el ajuste por sitio incluye los ARGUMENTOS DE LLAMADA, y
  a medias EMPEORA.** En `CarRenderInfo::RenderFlaresOnCar` (2.908 B) el
  objetivo iza **un solo** `lis lbl_8040AD04@ha` a un salvado (r16) y hace
  **tres** `lfs` por el; nosotros teniamos dos sitios como
  `extern const float lbl_8040AD04` y el tercero —la comparacion
  `if (intensity > 0.0f)`— como literal, o sea **dos registros base**. Convertir
  **solo** la comparacion lo dejo **peor** (98,46 -> 97,50%, +20 B): ese mismo
  `lfs` de 0.0f alimenta ademas el **septimo argumento** de dos llamadas a
  `eRenderLightFlare(..., 0.0f, ...)`, y al cambiar de simbolo CSE dejo de
  compartirlo y salieron dos `lis $LC@ha` nuevos. Convirtiendo la comparacion
  **y los dos argumentos**: **98,46 -> 98,62%, tamano de +8 B a +4 B y de 22 a
  18 diffs**. **El delator es que un unico `lfs` alimenta el `fcmpu` Y el
  registro de argumento de la llamada siguiente: los tres sitios son uno solo.**
  El negativo, tambien medido: convertir ademas los **seis** `= 0.0f` de las
  intensidades de antes del bucle sube a **134 diffs**. El truco es local al
  racimo que comparte registro base, no al fichero ni a la funcion.
  Con `function_reloc_diffs=none` el `lfs` casa igual contra el
  literal agrupado que contra el simbolo, y los datos del objetivo no se mueven
  (medido: 596/596 B). Lo unico que crece es nuestro .rodata con `$LC` de mas,
  que no se puntua.
- **Doce funciones con opcodes IDENTICOS y registros permutados no son doce
  problemas, es uno.** En zDynamics las 12 near-miss (24.248 B) tenian la misma
  cuenta de instrucciones que el original y la misma secuencia de mnemonicos.
  Cuando el diff entero es `ARG_MISMATCH` sobre registros, la causa esta en
  algo que comparte todo el TU, no en la forma de cada funcion. Se comprueba en
  segundos comparando las listas de mnemonicos de los dos lados.
- **Anadir la local que falta puede costar mas de lo que arregla.** El DWARF del
  original tenia una `UVector3 Vv` a `r1+0xE8` que desplaza 0x10 todas las de
  abajo. Tres formas de reponerla (copia, uso como velocidad de cierre, local
  muerta) engordaron el marco 96-116 B y BAJARON la funcion de 97,62% a 90-93%.
  La local existe, pero la forma que la produce sin codigo de mas sigue sin
  aparecer: reponer el hueco no es reponer la sentencia.

- **El contador muerto NO es inocuo**: `if (!cond) n++;` con `n` que nadie lee —
  GCC borra el store pero **alarga el rango de vida de `cond` a registro
  salvado**. Llevó una función de 99,855% a 100%. **Una local del DWARF que
  parece sin usar hay que escribirla.**
- **Quitar una local que el DWARF del original no tiene EMPEORA** (medido dos
  veces, ahora tres). En `FnStatelessF3::EvalSQT` regmap cantaba
  `times (unsigned short *) <-- SOLO NUESTRA` y el original emitia dos `mr
  r11,r8` que a nosotros nos faltaban; quitar el puntero local y repetir
  `statelessF3->mTimes[...]` bajo de 99,35% a **98,12% y +12 B**, porque GCC
  vuelve a hacer el `lwz` del miembro dentro de los bucles. **El `mr` sobrante
  del original NO es una local: es una particion de rango de vida que mete
  `regmove`.** Un `mr rA,rB` de mas en el original con las locales por lo demas
  identicas es sintoma del asignador, no de fuente que falte.
- **Una local "SOLO EN EL ORIGINAL" con guion en las DOS columnas no vale
  nada.** Sin ubicacion no emite codigo ni crea pseudo, y anadirla no mueve un
  byte: GCC la elimina antes de asignar. En
  `CWorldAnimCtrl::AdvanceAnimTime` el original declara dos `bool` muertos
  (`range`, `print_out_here`); se probaron seis formas -`false`, `m_flags &
  0x40`, sin inicializar, con comparacion flotante, en dos posiciones- y las
  seis dan **exactamente el mismo binario**. El veredicto "hay N solo en el
  original" de regmap **hay que filtrarlo por si tienen ubicacion**; solo paga
  la local muerta que mantiene vivo un valor (§ contador muerto).
- **Rotacion ciclica pura de los flotantes SALVADOS = misma prioridad, otra
  base.** En `AdvanceAnimTime` las 8 cantidades ocupaban `f23..f30` en los dos
  lados con el mapa `nuestro = original + 2` (ciclico): el orden por prioridad
  del asignador es identico, solo arranca dos registros mas alla. No es la forma
  de ninguna sentencia; es cuantos pseudos flotantes se reparten antes.
  **AVISO (auditoria 2-sep): esto se leyo durante rondas como un techo y NO lo
  era.** `CAnimCtrl::AdvanceAnimTime` (660 B) y `CWorldAnimCtrl::AdvanceAnimTime`
  (856 B) **casan hoy las dos al 100%**. Una rotacion ciclica pura es un
  **sintoma** —cuantos pseudos nacen antes—, no un veredicto: busca lo que crea
  un pseudo de mas (un inicializador que sobra, un `= false`, una constante mal)
  antes de aceptarla. Vease la entrada del `= false` de mas, que disolvio 58 de
  60 diffs de una rotacion de -4 en 16 ranuras.
- **Una local que sobra no se arregla inlinando la expresión: se arregla
  REASIGNANDO una local ya muerta.** Reasignar el parámetro es la forma canónica.
- **El inicializador muerto vence a la propagación de constante.** Si el original
  hace `mr rX,rY` donde nosotros `li rX,<const>`:
  `int j = 0; for (j = n; …)` — la local **nace viva** y CSE ya no sustituye la
  copia por la constante. **Pruébalo antes de dar nada por techo del asignador.**
- **GCC 2.9 reparte las ranuras de las locales por ORDEN DE PRIMER USO**, no de
  declaración.
- **El orden de declaración de las primeras locales que lista el DWARF es
  literal.**
- **Los locales declarados nunca reutilizan ranuras de temporales** y van a la
  cima del marco. **Ranura alta = temporal, ranura baja = local con nombre.** Un
  hueco en el marco sin local que lo explique es el pool de temporales.
- **Locales fantasma que sólo ocupan marco**: un objeto con ctor y dtor vacíos
  ocupa marco y cero instrucciones. `plan.py` lo canta literal.
- **Pero un marco 8-12 B mayor en el original NO prueba que falte una local**:
  GCC 2.9 con estos cflags deja **siempre** un hueco de 8 B entre la última local
  con memoria y el primer temporal DFmode (conversión int↔float), más 4 B de
  relleno bajo los guardados. Se comprueba en micro-banco antes de buscar nada.
- **Y AL REVES: SI EL OBJETIVO NO TIENE ESE HUECO Y NOSOTROS SI, ES QUE NOSOTROS
  TENEMOS UNA LOCAL DIRECCIONABLE QUE EL NO.** Cuando el temporal DFmode del
  objetivo esta en la **primera** ranura (0x8) no hay ninguna local con memoria en
  la funcion; si el nuestro esta 8 B mas arriba, la cuenta es
  `[4 B de local direccionable][8 B de hueco][8 B del temporal DFmode][4 B de
  relleno]` = 20 B contra los 8 del objetivo. La local no aparece en el DWARF ni se
  referencia en el asm final: es una que GCC promociono a registro despues de
  reservarle sitio. En `SteeringWheelDevice::UpdateForces` (2.140 B) eso es lo
  unico que queda entre 96,60% y el 100%.
- **`const T& x = f()` contra `T x = f()`**: una local de pila en el DWARF prueba
  la **copia** (seis instrucciones y todo el reparto).
- **`const` y ámbito actúan juntos** (una función pasó de 98,84% a 100% con tres
  cambios simultáneos, ninguno suficiente por separado), pero **`const` es dial
  de doble filo**: puesto en un `float` plegó una constante y bajó otra de 99,95
  a 96,8.
- **`split_decl`** (partir declaración y asignación) es el dial del rango de vida
  y la transformación más rentable del permutador.

- **UNA REFERENCIA `T *&` EN UN INLINE BLOQUEA LA INFERENCIA DE ALINEACION, Y ESO
  ES LO QUE CONSERVA LOS `AlignPointer` DEL ORIGINAL.** Escrito a mano,
  `p = (unsigned char *)(((unsigned int)p + 15) & ~15);` se BORRA en cuanto GCC
  puede probar que `p` ya esta alineado (tras `p += n<<4`), y se pliega a
  `+0x10` cuando viene de `p += 4`. Con
  `static inline void AlignPointer(unsigned char *&ptr, unsigned int bound)`
  —la forma que nombra el volcado DWARF— los dos sitios sobreviven. En
  `GManager::SaveGameplayData` valio de 85,39% a 98,86%, y con el sexto
  `AlignPointer` que pedia el DWARF, **al 100% (700 B)**. Corolario de conteo:
  el numero de expansiones `AlignPointer` del volcado incluye las que se
  pliegan; hay que escribirlas todas y dejar que GCC borre las que sobran.
- **UN LOCAL MUERTO CON VALOR 0 DECLARADO *ANTES* DE UNA LLAMADA METE EL `li` EN
  UN REGISTRO SALVADO.** Si el objetivo tiene `li rSalvado,0` **antes** del `bl`
  y el `stw` **despues**, la constante no se escribio detras de la llamada:
  `unsigned int shift = 0;` antes del `new` y `mCampo = shift;` despues lo
  reproduce (97,66 -> 99,91% en `FindKeyReductionShifts`). **La posicion importa
  y es estrecha**: la misma declaracion subida a la cabecera de la funcion baja
  a 96,35%.
- **`static T v(a,b,c);` CONTRA `static T v; static int inited; if (!inited) {...}`:
  el guardia `_.tmp_N` DEL COMPILADOR *ES* el `inited`.** Un estatico de funcion
  con inicializador dinamico emite un unico guardia; escribirlo a mano deja
  **dos** estaticos y **dos** comprobaciones. Delator: al objetivo le sobra un
  `_.tmp_N` y a nosotros nos sobra un `prev_inited.NNNN` con nombre. 24 B en
  `UpdateCameraMovers`.
- **Ejemplo trabajado del veredicto «SOLO NUESTRA» (§2): en
  `GRaceParameters::GenerateIndex` el acumulador `flags` no esta en el DWARF del
  original, y sustituirlo por `index->mFlags |= X` en los 17 sitios bajo de
  **97,90% a 92,94% y anadio 64 B**.** *(Al 2-sep la funcion sigue abierta al
  98,10%.)*

## 5. Orden de emisión

- **UN `lwz` REPETIDO DE UN PUNTERO GLOBAL DETRAS DE UN `stb`/`stw` A TRAVES DE EL
  = EL ORIGINAL LEIA EL OTRO CAMPO ANTES DE ESCRIBIR.** Intercambiar las dos
  sentencias deja que el planificador las mezcle y **borra la recarga**. 340 B con
  dos lineas.
- **EL `beq` QUE SALTA *AL* `stw` DICE QUE LA SENTENCIA ESTA FUERA DEL `if`.** Si
  el destino del salto condicional es el propio store, ese store no va dentro de
  las llaves. Sacarlo arreglo **los 17 destinos de salto de una funcion a la vez**
  (72 -> 55 diffs). Se ve en `plan.py` por la etiqueta `.L_<addr>`; **`fndiff` lo
  enmascara** porque pinta los `beq` como `ARG_MISMATCH` generico.
- **CUATRO `addi` QUE FALTAN PUEDEN SER UN EFECTO POSICIONAL DE CSE, Y SE
  DEMUESTRA QUITANDO ALGO 30 INSTRUCCIONES ANTES.** Si al borrar un inicializador
  anterior **desaparecen los cuatro**, y el codigo hasta ahi es identico byte a
  byte, no hay nada legal que quitar: el techo es real.
- **EL NOMBRE DE `_GLOBAL_.I.` LO DECIDE EL PRIMER GLOBAL *CON INICIALIZADOR*.**
  GCC 2.9 usa `first_global_object_name`, y una **definicion tentativa** (`int X;`)
  se difiere al final de la TU y **nunca gana el nombre**. Cambiarla a `int X = 0;`
  basta. Y como objdiff empareja **por nombre**, mientras no coincida la funcion
  entera cuenta **0%** aunque los bytes sean identicos: **44 B por unidad,
  gratis**. Barrido: `scripts/globalini.py`.
- **EL SIMBOLO `_GLOBAL_.I.<nombre>` DICE QUE DEFINICION VA PRIMERO, Y COMPARARLO
  ENTRE LOS DOS `.o` CUESTA SEGUNDOS.** En zAnim el nuestro se llamaba
  `AnimBankSlotPool` porque esa variable estaba **antes** del constructor;
  bajandola detras, el simbolo paso a `__9CAnimBank` y **+44 B**.
- **EL ORDEN DE DIRECCIONES DEL OBJETIVO ES EL ORDEN DE DECLARACION DEL
  ORIGINAL.** GCC 2.9 emite `.sdata`/`.rodata`/`.bss` en orden de declaracion, asi
  que no hay que deducirlo: se lee. `scripts/claimlbl.py` lo imprime. Medido:
  fdlibm declara los coeficientes **al reves** (`S6..S1`, `C6..C1`) y `scalbn`
  pone `tiny` antes que `huge`; `sfxrevc` declara `sndfx` **antes** de `modlist` y
  `conlist`.
- **LA DIRECCION DEL `.rodata` DE UNA UNIDAD NO SE ADIVINA: SE LEE DEL
  EMPAREJAMIENTO.** Si la unidad casa al 100%, cada instruccion nuestra que
  referencia un `$LCn` tiene enfrente la del objetivo referenciando un
  `lbl_XXXXXXXX`, **y el troceador ya calculo esa direccion**. Buscar los bytes
  (`claimdata.py`) deja 25 de 30 ambiguas -- un `.rodata` de 4 B con `1.0f`
  aparece en cientos de sitios; leer la pareja resuelve **47**.
- **UN `asm(".section ...")` INTERCALADO DESCOLOCA LO QUE VENGA DETRAS.** GCC 2.9
  lleva su propia cuenta de la seccion activa y no reemite la directiva si cree
  estar ya en ella: **cerrar el bloque con `.section .text` mando los dos punteros
  siguientes a `.text`**. Hay que cerrarlo dejando a GCC en la seccion en la que
  el cree estar.
- **UN STORE MUERTO QUE GCC BORRA Y EL OBJETIVO CONSERVA SIGNIFICA QUE LA
  INICIALIZACION ES DE UN CONSTRUCTOR, no asignaciones duplicadas.** Escribir
  `options.allocator = 0; ... = alloc;` hace que GCC borre el primero; un **ctor
  con 3 inicializadores** conserva los 7 stores: **71,81 -> 99,31% con tamano
  exacto**.
- **CONTRADICCION ABIERTA Y SIN DISCRIMINANTE (auditoria 2-sep): LA «CABEZA» DE
  UNA TIRADA DE STORES ES EL PRIMER STORE EN UNA MEDIDA Y EL ULTIMO EN OTRA.**
  Las dos lecturas estan medidas y **no se ha encontrado que las separa**:
  - **cabeza = ULTIMO store en orden de fuente de cada registro de valor
    distinto**, cabezas entre si en orden de fuente, detras el resto en orden de
    fuente (verificado 3/3, cerro dos constructores, un tercero 90,36 -> 99,83%
    y `FEQuaternion::GetMatrix` 196/196 a la primera). Entrada larga mas abajo,
    «LA TIRADA DE STORES ES UN DESPEJE».
  - **cabeza = PRIMER store de cada registro de valor distinto**, cabezas en
    **orden ASCENDENTE DE OFFSET**, y detras el **cuerpo** y **despues** la
    lista de inicializacion: con esa lectura, **73,82% -> 93,45%**.
  **Como se trabaja mientras tanto**: prueba la del ULTIMO primero (tiene mas
  confirmaciones), y si el despeje da un orden que no baja los diffs, prueba la
  del PRIMERO. Con **4 stores o menos no razones: enumera las 4! = 24
  permutaciones** — cuestan ~10 min y ya valieron 704 B donde la regla predijo
  `pnco` y el bueno era `ncop`. Y recuerda las dos acotaciones medidas: la regla
  se lee mal cuando **una sola sentencia escribe DOS registros de valor
  distintos**, y **se rompe cuando uno de los stores es una llamada a
  constructor** (ahi la permutacion inversa bajo 96,65 -> 87,32%).
- **LA TIRADA DE STORES DE UNA MALLA SE ESCRIBE EN ORDEN NATURAL DE INDICE.** Lo
  que veniamos transcribiendo (`V3.x, V0.x, V3.y, V0.y...` y
  `Colours[2],[0],[1],[3]`) era el orden **EMITIDO** —la rotacion del
  planificador—, **no el del fuente**. Con `V0.x, V0.y, V1.x...` y `Colours[0..3]`:
  tres funciones de 49->25, 80->56 y 37->30 diffs. **Cuidado con transcribir el
  orden del asm en una tirada regular: ahi el planificador SI reordena.**
- **EL PUNTERO EXPLICITO VA SOLO EN EL ARGUMENTO DE LA LLAMADA, NO EN LOS STORES.**
  `T *pp = &p; ... f(pp, ...)` con los stores escritos `p.` reproduce el
  `addi rN,r1,off` izado al preencabezado; poner `pp->` **tambien** en los stores
  enruta las 12 escrituras por el puntero y **empeora 4 puntos**.
- **EL BLOQUE DE STORES DE UN CONSTRUCTOR SEPARA LISTA DE INICIALIZACION Y CUERPO,
  Y SE LEE AL REVES.** GCC emite primero las **cabezas** (el ultimo store de cada
  registro de valor distinto, en orden de fuente = las asignaciones del **cuerpo**)
  y detras **los miembros de la lista en orden de DECLARACION**. En un ctor el
  objetivo emitia cinco offsets de valor distinto y luego ocho en orden de
  declaracion exacto: mover esos ocho a la lista fue **80,69% -> 100% con un solo
  cambio**. **Seis constructores cerrados con esta lectura.**
- **Corolario: un miembro que el objetivo escribe ANTES que otro declarado despues
  de el solo puede estar en la LISTA** (93,05 -> 100%).
- **EL CTOR IMPLICITO DEL MIEMBRO YA HACE EL TRABAJO.** Cinco casos medidos donde
  la llamada explicita era codigo de mas: `InitList()`, `X.Init()` (emitiamos
  cuatro `bl Init` y el objetivo dos), `Default()`, `Timer = 0`, y
  `m = FEGeometryModels()`, que ademas materializa un temporal de 0x14 B (+72 B).
- **EL ORDEN DEL POOL DE LITERALES DELATA QUE RAMA DEL `if/else` ESCRIBIO EL
  ORIGINAL, y es invisible para `matched_code`.** Diez parejas `"x"` /
  `"x_in_pursuit"` estaban **invertidas**: el `.rodata` del ELF lista siempre el
  nombre pelado primero, o sea `if (c) key = "x"; else key = "x_in_pursuit";`.
  **1.636 B.**
- **ACOTACION: la regla de la permutacion inversa de stores SE ROMPE cuando un
  store es una llamada a constructor** y otro usa un registro de valor distinto.
  Aplicarla ahi **bajo** de 96,65% a 87,32%.
- **UN OBJETO CONSTRUIDO COMO ARGUMENTO ANIDADO RESERVA SU RANURA DESPUES DEL
  TEMPORAL EXTERIOR; COMO LOCAL CON NOMBRE, ANTES.** El objetivo tenia el
  `Resource` en 0x88 y el `pvehicle` en 0x98, luego era **una sola expresion**, no
  un local — y que una local declarada despues reutilice el 0x88 lo confirma.
  **99,34% -> 100% en 1.588 B con un solo cambio.**
- **UN `sret` A UN LOCAL RECIEN DECLARADO SE ELIDE; PARTIDO EN DOS SENTENCIAS, NO.**
  `Matrix4 m = f();` borra la copia; `Matrix4 m; m = f();` reproduce el temporal
  mas los 0x40 B de copia (17 instrucciones) del objetivo.
- **LA LINEA HERMANA QUE YA CASA DICTA EL ORDEN DE DOS STORES GLOBALES.** Los
  `lis @ha` salen en orden de fuente; si la lista del objetivo es la inversa de la
  nuestra, hay dos asignaciones intercambiadas. En el arbol habia **5 sitios** con
  `Showcase::FromArgs` primero y **uno** con `FromPackage` primero: ese era el
  roto. **692 B.** Es un `grep`, no un diff.
- **En el despeje de stores, prueba el CUERPO COMPLETO antes que la lista de
  inicializacion.** En un constructor la lista **no** era la respuesta: mover los
  dos miembros de la lista al **final del cuerpo** dio el orden exacto.
- **LOS ARGUMENTOS ESCRITOS EN LINEA IZAN EL ACCESOR A UN SALVADO** (128 B): quitar
  el local y meter la llamada dentro de la llamada exterior.
- **PASAR LA BANDERA POR VALOR A UN SETTER METE EL `lwz` DEL ARGUMENTO DELANTE DEL
  `&= ~mask`, Y ESO DECIDE QUE `stw` SOBREVIVE.** Con las ocho sentencias escritas
  en linea GCC borra el store equivocado y la funcion se queda al 69%; con setters,
  100%. **Desbloqueo 1.076 B de golpe** en cuatro clases.
- **La regla del despeje de la tirada de stores (cabeza = ultimo store del fuente)
  vale tambien para las CARGAS y para colas de 3.** Una funcion de 200 B cerro
  reordenando tres asignaciones; otra, reordenando cuatro locales.
- **El ternario `cond ? MASK : 0` en un `|=` sale con RAMA; a traves de un
  envoltorio inline sale como MASCARA** (`xori`/`subic`/`subfe`/`rlwinm`/`or`),
  que es lo que emite el objetivo al escribir un bitfield.
- **CUANDO EL DESPEJE DE STORES TE DEJA EN 2 DIFFS, FUERZA BRUTA LAS `n!`.** Para
  4 stores finales la regla del playbook predijo `pnco` (99,93%) y el bueno era
  **`ncop`**. Las 24 permutaciones cuestan ~10 min de compilaciones y valieron
  **704 B**. Con 4 o menos stores, no razones: enumera.
- **INTERCAMBIAR DOS STORES ADYACENTES MUEVE LA CARGA DEL PARAMETRO PASADO POR
  PILA**, y eso decide que parametro se lleva el registro alto: alarga la vida de
  uno y le baja la prioridad en local-alloc. El orden EMITIDO de los stores ya era
  identico; lo unico que cambio fue el orden en la FUENTE. **316 B con dos lineas.**
- **LOS ARGUMENTOS ESCRITOS EN LINEA IZAN LA DIRECCION DEL OBJETO A UN SALVADO.**
  Con los argumentos ya calculados en locales, el `@ha`/`@l` del objeto se
  materializa justo antes del `bl`; escritos en linea (`f(GetA(), GetB()->C())`)
  GCC evalua la direccion **primero** y, como hay llamadas en los argumentos, la
  iza a un salvado. Delator: dos instrucciones `@ha/@l` que en el objetivo van
  **antes** de las llamadas y en el nuestro despues. 92,47% -> 97,84%.
- **`stfsux`/`stwu` ES EL AUTO-INCREMENTO DE GCC, Y SOLO SE FORMA SI LA PRIMERA
  REFERENCIA DE MEMORIA TRAS EL `add` LLEVA DESPLAZAMIENTO 0.** Si el objetivo
  hace `add` + `stfs` y nosotros `stfsux`, se rompe **invirtiendo el orden de los
  campos del inline que escribe** (`MaxTime` antes que `Time`): el planificador
  vuelve a emitir los stores en 0 y 4. Delator: sobran 4 B por sitio.
- **Ligar `x.z` a una local ADELANTA su carga** por delante de otras. Es la
  palanca opuesta a inlinar la expresion; hay que probar las dos.
- **`a*b + Max(...) + c` en una sola sentencia impide que GCC ice `a*b + c`** como
  invariante de bucle.
- **El INICIALIZADOR flotante ordena el pool**: `float sign = 1.0f;` no emite
  nada pero mete el `1.0f` delante del `0.0f` (verificado en el ELF), y eso
  mueve el `lbz` delante del `lis`. Fue el ultimo diff de una funcion.
  **Precondicion medida (§4): la variable tiene que USARSE.** Una local
  enteramente muerta no mueve el pool ni un byte — probado dos veces con cero
  cambio (`Convert16To32`, `AdvanceAnimTime`).
- **LA TIRADA DE STORES ES UN DESPEJE, NO UNA BUSQUEDA.** Para una tirada de
  stores de constantes **ya materializadas**, GCC 2.9 emite **a la cabeza el
  ULTIMO store en orden de fuente de cada registro de valor distinto** (las
  cabezas entre si en orden de fuente) y **detras el resto en orden de fuente**.
  Invirtiendo esa regla se calcula el orden del fuente **sin probar nada**.
  Verificado 3/3; cerro dos constructores y llevo un tercero de 90,36% a 99,83%.
  **Corolario medido: lo que veniamos transcribiendo era el orden EMITIDO del
  original, no el de su fuente.**
  **PRIMERA APLICACION, y cerro a la primera**: en `FEQuaternion::GetMatrix` los
  seis ceros salian emitidos `0x2c,0x30,0x34,0x38,0xc,0x1c`; quitando la cabeza y
  devolviendola al final queda `m41,m42,m43,m14,m24,m34`, que **solo sale de
  escribir la matriz columna a columna**. 196/196 sin probar una sola variante.
  **ACOTACION medida en el intento siguiente**: la regla se lee mal cuando **una
  sola sentencia de la fuente escribe DOS registros de valor distintos**
  (`stScale = FEPoint(1.0f)` escribe `f0` en `0x4` y `f13` en `0x8`). Ahi el
  despeje da un orden falso: reordenar por el bajo `InitializeCell` de 87,64% a
  75,12%.
- **`.x` antes de `.y` en cada par** cierra funciones de vectores; y **bajar la
  declaracion de una local justo antes de su bucle** la mueve de un salvado a un
  scratch y quita 8 B de marco.
- **QUE EL OBJETIVO NO IZE UNA CONSTANTE FUERA DE UN BUCLE Y NOSOTROS SI ES
  `move_movables`, NO LA FUENTE.** El objetivo dejaba `lis`/`addi`/`lfs` DENTRO
  del bucle; nosotros la subiamos a `f14` y eso **corria un registro todos los
  flotantes salvados**. Seis formas de la sentencia (literal, `1.0f-0.8f`,
  `const`, orden invertido, declaracion partida, autoasignacion) dan **binario
  identico**. Es techo.
- **La asignacion encadenada cierra una tirada de stores donde la permutacion
  inversa falla.** Con la rotacion calculada `S' = [E(2)...E(n),E(1)]` no cayo;
  `a = b = c = ... = 0;` **en orden de declaracion** dio la secuencia exacta del
  objetivo a la primera.
- **Mover globales entre `.cpp` de la misma SourceList sale gratis y es lo que
  ordena el static-init.** Basta con dejar un `extern` en el origen y una
  declaracion adelantada donde el tipo no sea visible.
- **`x->f() = g(... x->f() ...)` evalua la direccion ANTES de la llamada y la
  CSE-a**; el original la parte en dos sentencias y la recalcula despues. Un solo
  `INSERT`, y era lo ultimo que separaba una funcion de 1.108 B del 100%.
- **Ligar a un local el argumento que es una llamada cambia donde cae el despacho
  virtual.** `obj->Set(otro->Get() < k)` iza el despacho de `obj` **por encima**
  de la llamada interna; `bool v = ...; obj->Set(v);` lo deja detras. 1,2 puntos
  en una linea.
- **Un local usado UNA SOLA VEZ como argumento cambia el orden de evaluacion de
  la llamada**: `float d = X - Y; f(g(), d)` evalua `Y` antes que `g()`; inlinar
  la expresion restaura `g()` primero.
- **La permutacion inversa del bloque de stores, como tiro unico**: leer
  `E = pi(S)` del build actual, resolver `S'[pi(i)] = objetivo[i]` y escribir
  `S'`. Cerro un ctor de seis stores a la primera (96,80 -> 99,98%). **Pero no
  siempre vale**: en otro ctor la misma cuenta BAJO, porque uno de los stores
  cuelga de una carga y no es libre de moverse.
- **Un `fmuls` con el destino como SEGUNDO operando solo sale del producto
  escrito DENTRO del argumento de la llamada.** `x = k * x;` da siempre
  `fmuls fX,fX,fK`; `f(..., k * x, ...)` da `fmuls fX,fK,fX`.
- **Un envoltorio `inline` materializa el argumento por referencia en un pseudo
  NUEVO**: da `addi r3,r1,off` donde nosotros dabamos `mr r3,rN`. **El delator es
  que la llamada lleva la linea de la CABECERA, no la del `.cpp`.** Reponerlo
  como `static inline` libre cerro `SetNISPosition` (1.964 B).
- **Cuando una cabecera compartida bloquea una funcion, la salida suele ser el
  SITIO DE LLAMADA, no la cabecera.** `SetVehicleOnGround` (1.908 B) llevaba
  rondas bloqueada: mover `fYOffset = yOffset;` dentro del ctor de `WWorldPos` la
  cerraba **pero costaba ~10.000 B en seis unidades**. Un ctor por defecto
  anadido mas `SetTolerance()` **en el sitio de llamada** hace el mismo store al
  final con **coste cero fuera de la unidad**. El `stfs` lleva la linea de
  `SetTolerance`, no la del ctor.
- **`x = expr; x = a + x;` INTERCAMBIA los operandos del `add`.** `expand_binop`
  prefiere que el primer operando sea el registro destino, asi que reusar la
  variable destino como sumando emite `add x,x,a` en vez de `add x,a,x`. Con una
  **variable distinta** para la subexpresion sale el orden escrito. Seis formas
  en micro-banco, solo esa. **792 B.**

- **EL LOCAL INTERMEDIO CON ASIGNACION COMPUESTA ADELANTA SU OPERACION POR
  ENCIMA DE LA EXPANSION DEL OTRO OPERANDO. 1.300 B EN UNA LINEA DE CABECERA.**
  `eagl4supportdlopen.h` tenia el intercambio de bytes escrito asi:

      unsigned int ul;
      ul = htotus((unsigned short)(l & 0xFFFF));
      ul <<= 16;
      return htotus((unsigned short)(l >> 16)) | ul;

  y el `slwi ul,ul,16` salia **pegado** al calculo de `ul`; el objetivo lo emite
  **detras** de la expansion entera del segundo `htotus`. Escribirlo como **una
  sola expresion**

      return htotus((unsigned short)(l >> 16)) |
             (htotus((unsigned short)(l & 0xFFFF)) << 16);

  lo pone donde el objetivo: `DynamicLoader::Resolve` (1.300 B, 325
  instrucciones) paso de **98,80% con 35 diffs a 100% con 0**, y el resto de los
  diffs -que parecian permutacion pura de registros en dos byte-swaps- se
  disolvieron solos: **una sola instruccion mal planificada arrastraba 34
  `ARG_MISMATCH`**.
  Medidas de las otras cinco formas (todas 98,80%, todas 1.300/1.300): dos
  locales (`ul` y `uh`), el `<<= 16` al final, el `<<` dentro de la expresion
  pero conservando `ul`, el `|` invertido (98,54%). **Solo la expresion unica.**
  **Corolario del `regmap` limpio:** el veredicto era "MISMO conjunto de locales
  y mismo arbol de bloques, solo cambia el reparto en 3" -o sea, el que manda al
  permutador guiado-. La causa no estaba en ninguna local: estaba en el ORDEN de
  una instruccion dentro de un inline de otra cabecera. **Cuando `regmap` dice
  reparto pero el diff tiene UNA instruccion desplazada, arregla el
  desplazamiento antes de tocar los registros.**
  Y el volcado DWARF **lista `unsigned int ul` como local de `htotul`** -sin
  ubicacion-, lo que confirma la regla de §4: una local sin ubicacion no dice
  nada sobre la forma del fuente.
- **Reusar una variable de puntero para dos objetos distintos cambia el
  planificado de TODA la tirada de stores siguiente.** Ocho `stb` y dos `lwz` se
  reordenaron solo por declarar un segundo puntero. Es el gemelo de `split_decl`
  pero sobre la variable, no sobre la declaracion. **1.000 B.**
- **Dos `lwz` por la MISMA base a +0 y +4 seguidos de dos `stw` en orden inverso
  son una ASIGNACIÓN DE STRUCT, no campo a campo.** El delator es que CSE no
  funde esas lecturas con las escalares vecinas de la misma memoria: el original
  relee lo que acaba de comparar. `fp->_bf = _sn_IO_buf_ptr[i];` en vez de
  `_base`/`_size` por separado cerró `_sn_sfp` (324 B) desde 91,1%. Ni el
  `volatile` ni reordenar los campos se acercan.

- **Un ctor fuera de clase con `inline` contra dentro de clase cambia el ORDEN DE
  LOS STORES**, no solo el simbolo. Comprobado en micro-banco con cuerpo
  identico.
- **El orden de declaracion DENTRO de la clase decide el inlining**: un
  `operator delete` declarado *despues* del destructor hace que el destructor
  emita una llamada. Y **GCC 2.9 no anida inlines a tres niveles**.
- **La POSICION DE LA DECLARACION decide cuando corre el ctor de un miembro**:
  mover una llamada por encima de la declaracion de un objeto local puso la
  llamada antes del constructor y cerro una funcion de 696 B. Es el gemelo de la
  lista de inicializacion: alli manda el orden de declaracion, aqui el punto del
  texto.
- **El incremento fundido con el store contra el incremento en sentencia aparte**
  cambia el orden de emision del bucle; el mapa de lineas lo canta porque el
  `addi` lleva la linea del store.
- **La rotacion del planificador es INVERTIBLE**: si la permutacion emitida es
  E=[S2,S1,S4,S5,S6,S3], invertirla da el orden del fuente. 1.680 B.
- **Escribir los campos a mano en vez de llamar al helper inline** cambia el
  planificado de los inmediatos aunque el codigo inlinado sea identico.
- **Los resets van ANTES de la llamada, y el argumento relee el campo ya
  asignado.** `t->vol = fadeto; <4 resets>; SetVolume(t->vol * ...)`. Con los
  resets detras la funcion se quedaba en 87,95%; delante, 100% (676 B).
- **El orden del pool de literales fija el orden de las sentencias** antes de
  compilar nada: sbias, -1.0f, 0.0f, ubias, 1.0f, 0.04f daba las seis primeras
  sentencias de un vistazo.
- **Partir la asignacion compuesta cambia el reparto de registros:**
  `v *= 10; v += *p - '0';` caso donde `v = v*10 + (*p - '0');` fallaba.
  Misma familia que `split_decl`, pero sobre la expresion.
- **La lista de inicializacion decide DONDE cae la llamada al ctor del
  miembro**: con lista corre en orden de declaracion, o sea en medio de los
  stores escalares; con cuerpo corre la primera. Un ctor de 72% a 100%.
- **El planificador ROTA una tirada de stores**: emite el store de la **última**
  sentencia **primero** y el resto en orden de fuente, `E = [S(n), S(1)…S(n-1)]`.
  La cantidad varía. **Desrotando se predice el orden del original** (ver §2 para
  el filtro).
- **Los stores a la MISMA base SE REORDENAN; a bases distintas NO.** El orden
  emitido de un bloque de `p->campo = ...` **no es** el del fuente, así que
  leerlo del asm y transcribirlo falla. Pero dos bases distintas (`pups->` y
  `pupip->`) sí conservan su orden relativo, y eso **ancla** el bloque: las
  sentencias que el asm emite antes del store a la otra base van antes en el
  fuente. Con eso el espacio a probar baja de `n!` a trozos de 4-6.
- **El orden del bloque de stores se BUSCA, no se adivina.** Es la
  transformación que más veces cerró una función en la ronda de `snd`: 15 de
  las 28 unidades. Receta: dejar cada store en su línea, permutar el bloque y
  medir. Casi siempre cae en <30 pruebas de las `n!`; los 4 punteros nulos de
  una lista enlazada van SIEMPRE juntos, así que se agrupan en una sola línea
  y `6!` se queda en `5!`. Cuando `n>6`, escalada por movimientos simples
  (`n*(n-1)` por ronda) llega al 97-99% y el último salto sale de **invertir
  la permutación observada** (§5, «la rotación es invertible»): se calcula qué
  orden de fuente produciría el orden del objetivo y se prueba de una vez.
  `SNDplaysetdef`, 13 stores, cerró así.
- **El orden canónico del reset de un nodo de lista** (`SFILTERNODE` de `snd`,
  8 unidades): `next, next2, prev1, prev2` (los cuatro juntos), `clean`, los
  campos propios del filtro, `restorefn`, `filterfn`, y el ÚLTIMO campo propio
  al final. Una vez encontrado en `sfamplf` valió 7 unidades más a la primera
  compilación.
- **Coloca las funciones nuevas en el orden del `.s` original**: el sitio de la
  definición cambia el inlining de **otras** funciones del mismo TU. Reordenar un
  fichero al orden del DWARF destapó dos funciones que nadie atacaba.
- **La misma expresión escrita DOS VECES produce un `fmr` que GCC no coalesce**;
  un `b = a;` explícito sí lo coalesce.
- **Un argumento que es una simple carga de miembro se sube a un registro salvado
  antes de la llamada**; ligar el *resultado de la llamada* a un `const T&` en la
  sentencia anterior deja la carga después (35 discrepancias de una vez).
- **Ligar a un local el argumento que es una llamada** hace lo mismo.
- **El `this` de la llamada ligado a una local antes del test deja UNA sola
  carga**: `T *p = m_pX; if (cond || p != 0) p->F();` contra usar `m_pX` dos
  veces, que la duplica.
- **`a *= b` entre dos posiciones de memoria no produce `mult(a,b)`**: sólo
  `float px = a.x; a.x = px * b.x;`, con el valor a local **primero** e
  intercalado por componente, reproduce el `fmuls`.
- **La recarga del puntero global entre stores es un ORÁCULO DE TIPOS**: GCC
  recarga el `lwz` sólo tras un store a un campo `char`/`unsigned char`. Contar
  los grupos entre recargas da el orden del fuente **y** qué campos son byte.

- **EL DESPEJE DE LA TIRADA DE STORES IDENTIFICA LA LISTA DE INICIALIZACION.**
  Si al aplicar la regla (cabeza = ULTIMO store del fuente, resto en orden de
  fuente) el orden deducido pone **primero** un miembro que en nuestro fuente se
  asigna en el CUERPO, ese miembro esta en la **lista de inicializacion**: alli
  corre en orden de declaracion, o sea *entre* la llamada al ctor de la base y
  los ctores implicitos de los miembros siguientes. En `GTrigger::GTrigger` los
  siete `stw r0` salian `0xc8,0x74,0x78,0x7c,0x84,0xc0,0xc4` y solo
  `: ..., mWorldTrigger(), mTriggerEnabled(0)` los produce. Un solo miembro
  movido a la lista, siete stores en su sitio.

## 6. Control de flujo

- **UN BUCLE HECHO CON `goto` HACIA ATRAS NO LLEVA `NOTE_INSN_LOOP_BEG`.** GCC
  2.9 saca invariantes al preencabezado **solo en bucles marcados por el
  front-end** (`for`/`while`/`do`), asi que con `goto` **`loop_optimize` ni lo ve**
  y el invariante se rematerializa dentro. Delator en el diff: **un `lis …@ha`
  NUESTRO justo antes de la cabecera del bucle que en el objetivo aparece
  repetido en el cuerpo**, con callee-saved de mas y marco mas grande. Cerro dos
  funciones (90 -> 100% y 87,56 -> 100%). Vale al reves tambien: si al objetivo
  le sobra el izado y a nosotros no, el original tenia un bucle de verdad.
  *(Solo muerde con `-fmove-all-movables` en los cflags de la unidad.)*
- **`*(array + i)` CONTRA `array[i]`**: `fold` manda `&array` --que es
  `TREE_CONSTANT`-- al SEGUNDO operando, asi que la forma con puntero sale con
  el `lwzx` con los operandos al reves. Valio el ultimo 0,74% de una funcion.

- **`balance_case_nodes` SOLO PARTE LISTAS DE MAS DE DOS NODOS** (`if (i > 2)`).
  Con dos nodos no parte, el rango queda de hoja, se emite como prueba mas
  `b default`, y **`jump.c` borra la condicional porque las dos salidas son la
  misma etiqueta**. Ese es el borrado que costo nueve pruebas sin explicacion en
  una ronda anterior.
- **Un `case` extra ADYACENTE (con las dos cotas conocidas) cuesta CERO
  instrucciones**: sale un `emit_jump(label)` que `jump.c` funde con el `bgt` del
  padre. Sirve para forzar la forma del arbol sin pagar bytes.
- **`group_case_nodes` vuelve a fusionar dos `case` vacios adyacentes si comparten
  salida**, y se rompe poniendo `continue` en uno y `break` en el otro.

- **`if (c) { X = A; } else { X = B; }` -> `X = c ? A : B;` CUANDO EL LVALUE TIENE
  DIRECCION NO TRIVIAL.** Con el `if/else`, el movimiento de invariantes iza el
  `addi rN,r1,K` de la direccion al preencabezado y arrastra una renumeracion de
  registros; con el ternario la direccion se calcula **una sola vez antes de la
  bifurcacion**. Delator: `lmap` da **una sola linea** donde tu tienes cinco.
  Cerro **440 B**.
  **NO GENERALIZA, hay que medirlo**: barridas las 85 apariciones del patron en
  seis unidades y probadas las 3 que caian en una near-miss -- una identica al
  bit, otra **empeora** 97,21 -> 95,48%, la tercera identica. **El discriminador
  es que el lvalue sea indexado o por puntero, no un local plano.**


- **`do/while` CON GUARDA `if` CONTRA `for`.** Si el objetivo hace `li r5,0` y
  luego `mr r7,r5` donde tu emites **dos** `li`, tenias un `do/while` precedido de
  un `if` y el original tiene un `for`: el `for` inicializa una vez y copia.
- **UN `while` CON `&&` NO ES LO MISMO QUE `while` + `if/else` + `break` PARA
  COMPARACIONES FLOAT.** Con `-ffast-math`, `for/while (p && a < b)` emite
  `fcmpu + blt`; la forma con `if (a < b) { ... } else { break; }` emite
  `fcmpu + cror un,eq,gt + bso` (sale por GE ordenado; sigue en LT y en NaN) y
  el fondo del bucle en `b` en vez de en `blt`. Delator: el objetivo trae
  `cror + bso` donde tu tienes `blt`, repetido en cada expansion del mismo
  bucle inline. Medido en micro-fichero con los cflags exactos de la unidad
  (`for&&` y dos variantes de `while`+`break` con `>=` dan `blt`; el
  `if/else` da `cror+bso`) y cerro **4.396 B a la primera compilacion**
  (`RBGrid::Add` 1.488 B + `AIAvoidable::UpdateAllAvoidables` 2.908 B, el
  mismo bucle del ctor `Node` de `SAP.h`, las dos auditadas). Limite: el
  micro-banco aplana el bucle (el contexto del inline manda); la prueba que
  vale es en la unidad real.
- **`return x;` AL FINAL CONTRA `goto done;`.** Un `return` final se fusiona con
  los `return false` de arriba por cross-jumping; con `goto done;` sobrevive el
  salto. Las dos cosas juntas: 99,194 -> 99,960%.


- **EL BLOQUE FUNDIDO DEL EPILOGO CUENTA LAS SENTENCIAS: `lmap` IMPRIME UNA LINEA
  POR CADA `return` QUE EL CROSS-JUMPING METIO AHI.** Si el `li r3,0` del objetivo
  lleva N lineas y tu fuente tiene N-1 `return false`, **FALTA UNA SENTENCIA**, y
  el sitio se deduce contando huecos hacia atras. Valio **2.200 B en una sola
  ronda** (`CheckCollideRB` cerrada anadiendo un unico `return false;` dentro de un
  `else`, y `CheckCollideSRB` +0,4 pp con el mismo). **Generaliza a cualquier
  bloque fundido**, no solo al epilogo: el `stw r24,0x18(r29)` de `CullParts` lleva
  3 lineas y el `li r8,1` de `UpdateLightStateTextures` lleva 2.


- **GCC 2.9 BORRA LOS `case` DE LOS EXTREMOS DE LA LISTA ORDENADA CUYO DESTINO ES
  EL DEL `default`; UNO EN MEDIO SOBREVIVE.** Medido sobre ~3.246 variantes
  compiladas. `{2:a, 3:b, 8:vacio}` -> el 8 desaparece; anadiendo `9:c` el 8
  **sobrevive** porque queda en medio. Con dos vacios al final se borran los dos,
  y luego el siguiente que queda expuesto. Solo sobrevive si su destino difiere en
  expansion: `default:` con cuerpo, cuerpo propio, o compartir etiqueta con un
  case que lo tenga. **Fallan**: `continue` en vez de `break`, `goto`, `default`
  escrito primero, `default: ;`, y no poner `default`.
- **DELATOR BARATO DE UN `case` BORRADO**: dentro de un bucle, la constante izada
  para la comparacion que se borro **se queda en el precabezal como codigo
  muerto** (`lis 29,0x7000; ori 29,29,6` sin ningun `cmplw` que la use). Si ves
  una constante izada que nadie compara, ahi habia un `case`.
- **`cmplwi K; ble DEFAULT` ES LA PRUEBA DE QUE **NO** HAY UN `case` EN K.** Es el
  chequeo de limite inferior del nodo siguiente, y GCC solo lo emite cuando ese
  valor no esta en la lista. Sirve para descartar «falta el case K» sin compilar
  nada: costo una hipotesis mia que el agente refuto con 3.246 variantes.


- **`if (A || B) { cuerpo }` CON UN SOLO CUERPO SE EMITE EN LINEA Y CON LA SEGUNDA
  CONDICION INVERTIDA; CON EL CUERPO DUPLICADO EN DOS `if`, EL CROSS-JUMPING LOS
  FUNDE FUERA DE LINEA Y LA SEGUNDA CONDICION CONSERVA SU `beq`.** Probado con
  `ngccc -S` y los cflags reales sobre `CompositeSkin(RideInfo*)`: con el cuerpo
  duplicado, `.L20` y `.L21` colapsan en un solo bloque colocado **exactamente
  donde lo pone el objetivo**. Es el patron del `case A: case B:` generalizado del
  `switch` al `||`, y **la firma para reconocerlo es `bne-1, beq+1` con el mismo
  tamano**.
- **EL ARBOL DE SALTOS COMPLETO DICE SI FALTAN `case`, Y CUANTOS.**
  `arboljump.py --todas` cruzado con el delta de mnemonicos deja el trabajo hecho:
  en `DynamicLoader::Initialize` (79,66%) el objetivo tiene `bgt0x3` y `ble0x1`
  --juntos, un nodo de rango 1..3-- que nosotros no emitimos, el pivote 0x8
  invertido y `0x9`/`0x3` en orden cambiado. Eso **resolvio una contradiccion que
  costo 696 compilaciones**: no era que el `switch` no pudiera emitir esa raiz,
  era que con los `case` que faltan el arbol es OTRO. Y en el otro sentido, un
  nodo de mas en NUESTRO arbol es **un `if` defensivo que sobra**
  (`TestToLicensed`).

- **UN `b` A UN `beq` COMPARTIDO = EL ORIGINAL TENIA DOS `if` CON `return` PROPIO,
  NO UN `?:`.** El ternario en condicion hace que GCC emita `bcc drop_through; b
  falso` para el **primer** brazo --tres instrucciones, imposibles de fundir--;
  dos `if (...) return;` terminan cada brazo en `bcc fin` y el cross-jumping funde
  ese `bcc`. 224 B.
- **UN `&&` CUYA RAMA FALSA SALTA A LA *SALIDA* DE LA FUNCION EN VEZ DE AL TEST
  DEL BUCLE NO ES UN `&&`: ES UN `if (...) break;` ANIDADO.** El `blt` lo delata.
- **`if (A) { if (B) X } else { if (B) Y }` CON EL TEST INTERNO EN LA MISMA LINEA
  QUE EL `if` EXTERNO ES `if (A && B) X; else if (B) Y;`** -- invierte el orden de
  los bloques. 74,4 -> **100%**.
- **LA ASIGNACION Y EL TEST EN LINEAS DISTINTAS DEL MAPA** significan
  `x = f(); if (x != C) ...`, **no** `if ((x = f()) != C) ...`. Mueve el bloque
  compartido del cross-jump.
- **AVISO: ESTRUCTURA CORRECTA NO IMPLICA MISMO CODIGO.** Reescribiendo `Waiting`
  con la receta que cerro a su hermana, el **cuerpo DWARF queda EXACTO** (mismo
  conteo de inlines, cero etiquetas) y el codigo sale **304 B peor**: los siete
  destructores del original suman 220 B y los nuestros 424. Ahi el cross-jumping
  no reproduce lo del original, y en la hermana si.
- **UN `li rX,0` DE FUSION DETRAS DE UN `Search` ES UNA GUARDIA EXPLICITA QUE
  FALTA.** El objetivo iza `li r31,0` **antes** de la busqueda y encamina el caso
  nulo directo al `return false`; nosotros emitimos un bloque de fusion.
  Envolviendo la llamada en `if (handle) { ... }`: 83,02 -> **100%, 232 B**. Y la
  hermana al 100% **si** tiene el bloque de fusion: **son formas distintas en el
  original**, no un techo. Extrapolable a cualquier near-miss con esa firma.
- **VEDA (40 B, `eathread_thread`): el `mtctr`/`bdnz` del objetivo contra nuestro
  `cmpwi`/`subi`/`bne` en el bucle de constructores de un array de 32 objetos NO
  sale de la fuente.** Probado: literal en vez de la constante con nombre,
  dimensiones del array vecino invertidas, y el array con ctor declarado primero.
  **Las tres: 82,40%, sin mover un byte.**
- **VEDA (264 B, `Csis::System::Init`): dar nombre al temporal del macro
  `NULLALLOCTVP` lo EMPEORA.** El objetivo iza `addi r11,r1,0x8` y guarda los tres
  campos **a traves de ese puntero**; nosotros escribimos relativo a r1. Con
  `EA::TagValuePair tvp = NULLALLOCTVP;` y pasar `tvp`: **95,73 -> 86,71%**.
- **`||` ENCADENADO CONTRA `if` SEPARADOS CAMBIA EL ALCANCE DE CSE, AUNQUE EL
  CODIGO DEL GUARDIAN SEA IDENTICO.** Partir un guardian de 6 condiciones en
  **seis `if (...) { return 0; }`** (el mapa de lineas los pone a 8 lineas cada
  uno) hizo que GCC guardara un valor en un salvado y **rematerializara la resta**
  en el quinto peldano: exactamente los 8 B que faltaban. **95,0 -> 99,55% con
  tamano exacto.**
- **UN `bgt` DETRAS DE UN `cmpwi` EN UNA CADENA DE IGUALDADES = EL ORIGINAL ERA UN
  `switch`.** `if (e==A||e==B||e==C)` era `switch(e){case A: case B: case C: ...}`
  --el `bgt` es el arbol de `emit_case_nodes` y las comparaciones sobrantes viven
  en cr3/cr4 izadas del bucle--. Y el `switch` interior **no lleva
  `default`/`else`**: eso es el `li r30,0` vivo que una cadena `if/else` borra.
  **90,90 -> 99,41% de un golpe.**
- **EL CROSS-JUMPING SE PARA JUSTO ANTES DEL REGISTRO DEL *SEGUNDO* ARGUMENTO.**
  Escribir `return f(K, a, b);` en **cada** `case` --en vez de `fmt = K; ...;
  return f(fmt, a, b);`-- reproduce la cola compartida `mr r5,r30; crclr; bl; b`
  dejando el `mr r4,r31` **repetido por caso**. 86,01 -> **100%, 556 B**.
- **EL PLANIFICADOR POST-RELOAD SIEMPRE IZA EL `++` DE LA VARIABLE DE INDUCCION
  QUE ALIMENTA EL TEST, SEA CUAL SEA EL ORDEN DE LA FUENTE.** `a++; b++;`,
  `b++; a++;` y `for(...; ...; a++, b++)` dan salida **byte a byte identica**: el
  orden fuente es **irrecuperable** reordenando. Lo unico que restaura el del
  objetivo es `__asm__("")` **entre los dos incrementos**: 99,93 -> **100%,
  1.348 B**.
- **SI TODAS LAS RAMAS DE FALLO SALTAN A UN PUNTO DE CONTINUACION Y NO A UN
  `li r3,0`, EL ORIGINAL NO TIENE `return false`.** Y **anidar no basta: GCC no
  elimina el test redundante** (98,42%, +8 B); con `goto` a esas etiquetas,
  98,77 -> **99,51% con tamano exacto**.
- **`while (x-- != 0.0f)` CONTRA `x -= 1.0f` EN EL CUERPO.** La firma del objetivo
  es `fmr fT,fX; fcmpu fT,0; fsubs fX,fX,1` **al fondo del bucle** mas un
  decremento en el precabezal: eso es **post-decremento en la condicion**, no un
  decremento como sentencia. 86,08 -> **94,61%**, y de paso arreglo el intercambio
  de FPR que `regmap` senalaba.
- **UNA CONDICION DE BUCLE INVERTIDA SE VE EN EL ASM:** `limit > i` da
  `cmplwi limit,0 / ble` en la guarda y `bgt` en el latch; `i < limit` da
  `cmplw i,limit / bge` y `blt`.
- **`} while (*p++ != 0);` CONTRA `p++; } while (*p != 0);` CUESTA UN REGISTRO
  SALVADO** y rota el reparto entero. Valio **1.076 B a la primera**.
*La sección más larga del manual (130 entradas) y la peor ordenada: ramas,
bucles y `switch` van entremezclados en **6.1** porque cada ronda añadió al
final. **Lo que la auditoría del 2-sep dejó consolidado en una sola entrada, y
conviene leer antes que nada**: «EL `case` VACÍO SE BORRA SIEMPRE, Y EL QUE
FALTA VA POR DEBAJO DEL MÍNIMO» (funde siete entradas que se contradecían) y,
en §7, «UN LÍMITE SIN CANONICALIZAR TIENE TRES ORÍGENES». Las cuatro
subsecciones siguientes sí son autocontenidas: **6.2** cross-jumping, **6.3** la
saga del `cr7`, **6.4** el banco de un solo `.cpp` y **6.5** las vedas medidas
con su porcentaje de hoy.*

### 6.1 Ramas, bucles, guardas y `switch` (el cuerpo de la sección)

- **VEDA MEDIDA Y *EXPLICADA*: DONDE CAE EL PAD DE LIMPIEZA DE UN `goto`.**
  Reproducido en un **sintetico de 40 lineas** con los cflags exactos: con **un**
  `goto` hacia delante que sale de un ambito con destructor, GCC 2.9 emite el pad
  **en linea en el sitio del salto**; con **dos**, el cross-jumping los funde y
  **sobrevive la copia ULTIMA**, pegada justo antes de la etiqueta destino. Para
  que caiga donde el objetivo lo pone haria falta que el ultimo sitio de fixup
  estuviera al final de otro bloque -- y el DWARF confirma que ese bloque **acaba
  en medio del pad**. `ResolveCarBanks` (7.508 B): **320 variantes del permutador,
  cero**; mover la etiqueta dentro o fuera de las llaves, **igual**; invertir el
  `if` final, **codigo identico**.
- **DOS `if` CON UN `return true` CADA UNO PUEDEN SER UN `if/else if/else` CON UN
  SOLO `return`.** La etiqueta de junta que queda detras de la segunda rama es lo
  que deja al cross-jumping fundir `mtlr; blrl` **a traves de la llamada
  indirecta** en la direccion correcta. De 24 combinaciones de donde poner los
  `return`, cuatro dan el tamano exacto pero funden **al reves**. Cerro
  `Articulation::Constraint::React` (936 B, 99,12 -> **100%**) **rompiendo la
  veda de 15 formas que hay en §6.5**: la palanca no era donde poner el `return`,
  era CUANTOS habia.
- **MOVER UNA ASIGNACION POR ENCIMA DE UN `if` IMPIDE QUE GCC PLIEGUE LA CONDICION
  DEL `do-while`.** Subir la asignacion justo detras de la primera llamada
  recupera el `mr.`+`bne` que faltaban.
- **EL AVANCE DEL BUCLE SE COMPARTE O SE DUPLICA, Y ESO DECIDE TAMBIEN LA
  PROPAGACION DE CONSTANTES.** Con `while` y el avance duplicado en cada
  `continue`, +36 B; con `for(...;...;avance)`, −188 B; el objetivo comparte
  **solo dos** de los tres `continue`. Y el bloque compartido, al tener **≥2
  predecesores, pierde la equivalencia** que `record_jump_equiv` propaga: por eso
  el objetivo emite `mr r6,r28` donde nosotros poniamos `li r6,0x4d`. Un `goto` a
  una etiqueta comun arregla las dos cosas: 85,22 -> 90,90%.
- **EL `case` VACIO SE BORRA SIEMPRE, Y EL QUE FALTA VA POR DEBAJO DEL MINIMO.
  ESTE PAR ES EL DISCRIMINANTE Y RESUELVE SIETE ENTRADAS QUE SE CONTRADECIAN.**
  - **Dentro o por encima del rango emitido, un `case` vacio se BORRA y no
    mueve nada.** Cuatro variantes con `ngccc -S` (sin `default`, reordenado,
    `case A: case B:` en vez del rango, `continue` en vez de `break`): **las
    cuatro pierden el nodo**; solo un cuerpo **no** vacio lo conserva. Y en una
    funcion con 20 nodos y 4 rangos, **47 variantes de `case` vacios no movieron
    el pivote ni un sitio**. Si la formula falla por un indice y esta palanca no
    responde, la palanca no existe ahi.
  - **POR DEBAJO DEL MINIMO (o negativo), SI cuenta**, porque es lo que le da al
    nodo de rango su **limite inferior**. Un `switch` de 2 casos da cadena lineal
    (`beq;beq;b`); anadiendo un tercer valor **menor** que el mas bajo con cuerpo
    vacio sale exactamente `beq c1; cmplw <mismo reg>; ble default; beq c2;
    b default`. **Por encima**, la cola sale `bne default` y no casa. Se
    comprueba con `ngccc -S` en 1 s antes de tocar el arbol.
  - **El caso extremo es el NEGATIVO.** Cuando el pivote del objetivo es
    *imposible* con los valores visibles, el nodo que falta esta **fuera del
    rango emitido**: un `case -1: break;` no emite ni una instruccion, corre el
    indice y **reordena el arbol entero** (movio un pivote de 6 a 4 con tamano
    exacto; el pivote de `epCalculate` solo cuadra con `case -1`). **Un barrido
    de `case` vacios que solo mire 0..63 da CERO aciertos.**
  - **Un GRUPO CONTIGUO pegado al `default` cuenta igual**, aunque comparta
    etiqueta con el: micro-banco de **12 formas**, seis `case` contiguos pegados
    al `default` mueven el pivote del indice 3 al 4 y **no emiten ni una
    comparacion** (se funden en un nodo de rango cuyas dos aristas van al
    default). Un `cmplwi 8;bgt / cmplwi 4;bge` con limites sin canonicalizar era
    imposible con 4 nodos: eran seis `case` pegados al default. **672 B.**
    **Un `case` SUELTO pegado al default SI emite su `beq`**, y con
    `case 8: case 0x70000005: case 0x70000006: break;` **solo el 8 (el pivote)**
    la emite — micro-banco de 6 formas. Con cuerpo no vacio sobreviven las tres.
  - **Con CUERPO, aunque sea identico al del `default`, la comparacion SI se
    emite** (cuatro variantes).
  **Lectura del pivote**: a la derecha = sobra un nodo; a la izquierda = falta un
  `case`. Y **un valor de `case` > 127 apaga `use_cost_table`**, lo que cambia el
  equilibrado del arbol entero.
- **UN TERNARIO QUE ASIGNA UN MIEMBRO NO ES `if/else`:** el ternario deja **un**
  store tras la junta; el original guardaba **dentro de cada rama**.
- **`__asm__("")` JUSTO *DESPUES* DE LA LLAMADA, DENTRO DE LA RAMA, ES UNA
  BARRERA DE CROSS-JUMPING DE CERO BYTES.** 90,90 -> **97,83%**. **Delante de la
  llamada no sirve** (medido).
- **DECLARAR LAS LOCALES DESPUES DEL EARLY-RETURN**, con su propia constante en el
  `return`.
- **UN LIMITE SIN CANONICALIZAR TIENE TRES ORIGENES (`case LO ... HI:`, un bucle
  con cota VARIABLE, o la constante en un LOCAL) — no solo el `switch`.** La
  entrada consolidada, con las medidas de las tres vias y las seis formas de
  `if` descartadas, esta en **§7** («GCC 2.9 canonicaliza `x>=C` a `x>C-1`...»).
  **Antes de reescribir una cadena de `if` como `switch`, mira si la comparacion
  esta en la arista de salida de un bucle.**
- **VEDA MEDIDA: EL `b` DE `expand_end_loop` NO SE ALCANZA DESDE LA FUENTE
  (4.396 B).** El objetivo copia el test de entrada al final del bucle y salta al
  `cror/bso` compartido; nuestro GCC **invierte** ese salto y emite `blt` al
  cuerpo. Probadas **17 formas de fuente** (while, for, do-while, goto explicito,
  break antes y despues, temporal, accesores, condicion negada...) -- **todas dan
  el mismo asm byte a byte** -- y **24 combinaciones de flags**. En todo el juego,
  el patron `cmpwi,beq,lfs,lfs,fcmpu` va seguido de `b` **8 veces y de `blt` 0**:
  es sistematico, no una casualidad. Banco aislado de 1 s en `scratchpad/iso/`.
- **LA RAMA `else` QUE COMPARTE CUERPO CON UN `case` SOLO SE FUNDE SI SE ESCRIBE
  ANTES DEL SWITCH.** `if (a) A0; else if (c < end) {switch} else A1;` da el
  layout equivocado; invertir a `else if (c >= end) A1; else {switch}` funde el
  `case` con el `else`. Corolario: con 2 cases y un `default:` **con cuerpo**, GCC
  parte el despacho; solo emite `beq;cmpwi;beq;b default` si el cuerpo del
  default **no** es el bloque siguiente.
- **DOS `return 0` TEMPRANOS SE COLOCAN EN LINEA; ANIDANDOLOS EL BLOQUE COMPARTIDO
  SE VA AL FINAL.** `if (ok) { ... return 1; } return 0;`
- **`if (f() == 0) return ERR;` NO ES `result = ...` CON `if/else`.** La firma en
  el diff: con la variable, izamos el `li rSalvado,-0x12` **antes** de la rama; el
  objetivo emite `li r3,-0x12; b` **en linea** en el camino de error. 308 B.
- **`do { ... } while (c)` CONTRA `while (c) { ... }` DECIDE SI UN BLOQUE SE
  DUPLICA.** Con la prueba **arriba**, GCC comparte por cross-jumping el bloque
  del destructor virtual entre las dos llamadas; con `do/while` lo duplica. 168 B
  en una unidad que ya marcaba 100%.
- **DOS `if (A && B) x = true;` SEGUIDOS NO SON `if ((A&&B) || (C&&D)) x = true;`.**
  Con dos `if`, GCC sabe que `x` sigue siendo `false` en el primero y lo convierte
  en `x = cond` (`mfcr`/`extrwi`); con el `||` sobreviven los dos saltos.
- **`if (A && !B) {grande} else {pequeno}` CONTRA `if (!A || B) {pequeno} else
  {grande}`:** solo la primera deja el bloque pequeno AL FINAL, y sin eso la
  ultima cola grande no tiene `b` propio y no funde con sus hermanas.
- **MOVER LA ETIQUETA DE UN `goto` FUERA DE LA COLA ARREGLA DOS CROSS-JUMPS DE
  GOLPE.** Al final de la funcion mete un bloque entre el ultimo `return` y el
  epilogo; dentro del `switch` el ultimo bloque cae al epilogo y deja de ser
  candidato a fundirse.
- **QUITAR LAS LLAVES DE UN OBJETO CON DESTRUCTOR NO BASTA:** con el destructor
  vivo hasta el final, cada `return <expr>;` genera su propia cola. Hay que
  escribir `if (cond) return false; return true;` en **todos** los `case`.
- **UN `andi.` CON DOS SALTOS SOBRE EL MISMO `cr0`, EL SEGUNDO MUERTO, ES UN
  ACCESOR EXPLICITO QUE FALTA** (el segundo reusa el bit por CSE).
- **INVERTIR UNA COMPARACION PARA QUE ESPEJE A SU HERMANA ROMPE `thread_jumps`.**
  Dos `case` con la misma prueba en polaridad opuesta dejan que GCC enhebre los
  saltos y salgan `cror`/`bso` de mas. Escribir el segundo con el **mismo** signo
  y los cuerpos cambiados lo impide: `ActionQueue::FetchCurrentValues`
  **488 B al 100% con una linea**.
- **UNA GUARDA REDUNDANTE CON EL INDICE A 0 SE PLIEGA, Y ESO ES LO QUE EMITE EL
  OBJETIVO.** `while (p->name != 0 && i <= 36)` con `i = 0`: GCC pliega el
  `i <= 36` de **la primera vuelta** y deja la prueba solo en el avance. Una
  cadena de `goto` descompilada a mano casi nunca es un `goto`.
- **`if (a <= b) {cuerpo} else {return;}` DA BINARIO IDENTICO A `if (a > b)
  return;`.** No es un frente: si el diff no baja, no era eso.
- **UN DESTRUCTOR DUPLICADO MAS UN `b` AL AVANCE DEL BUCLE ES UN `continue`, NO UN
  `&&`.** `if (a==b && count!=0)` -> `if (a!=b) continue; if (count!=0)`. El
  `continue` emite los destructores en esa salida, que son las instrucciones que
  faltaban. **84,38 -> 87,36% con tamano exacto.**
- **EN UNA FAMILIA DE MAQUINAS DE ESTADOS, LA LISTA DE `case` DE LA HERMANA QUE YA
  CASA ES LA RESPUESTA.** Una de seis `Update()` era la unica **sin
  `case kTransition:`** (= -1). Se lee **sin compilar**: el objetivo despacha el
  valor 1 con un **`bgt` pelado** y nosotros con `cmpwi 1; beq`, y `emit_case_nodes`
  solo emite el `bgt` cuando el nodo 1 esta **acotado por los dos lados** — o sea
  que existe un `case` por debajo de 0. Micro-banco de 4 conjuntos, 1,5 s.
- **`b = (x == K);` SALE SIN SALTO; EL ORIGINAL LO ESCRIBE COMO `if`.** El objetivo
  emite `cmpwi K; bne; li r30,1` con el `bool` inicializado a 0 antes; nosotros
  `xori/subfic/adde`. **Delator: `subfic`+`adde` en NUESTRO lado con `li`+`bne` de
  mas en el objetivo.**
- **LA ARITMETICA DEL DELTA DE MNEMONICOS PREDICE EL NUMERO EXACTO DE BUCLES Y SU
  POLARIDAD, ANTES DE COMPILAR.** `for (;;) { i++; if (i > end) return; cuerpo; }`
  es en realidad `i++; if (i <= end) { do { cuerpo; i++; } while (i <= end); }
  return;`. **Cada bucle `++` aporta `+1 addi / +1 ble` y cada `--` `+1 subi /
  +1 bge`**, asi que de `cmpw+4, b+4, addi+2, subi+2, bge+2, ble+2` se lee «cinco
  bucles, dos ascendentes y dos descendentes, todos guarda + `do/while` escrita a
  mano». **Una sola compilacion: 93,01% -> 99,88% con tamano exacto**, 3.456 B.
- **LA GUARDA DE UN `duplicate_loop_exit_test` ESCRITO A MANO TIENE QUE SER
  TEXTUALMENTE LA CONDICION DEL BUCLE.** Con `if (iStartPosX != ... && iStartPosY
  != ...)` la guarda usa **otros pseudos** y `thread_jumps` no puede redirigirla
  por encima del `if` posterior; con `if (iCurPosX != ... && iCurPosY != ...)` si.
  **99,994% -> 100%.**
- **UN LOCAL QUE GUARDA LA CONSTANTE ROMPE LA CANONICALIZACION `x>=C` -> `x>C-1`
  (tercera via de §7).** Vale tambien para la comparacion materializada como
  valor: en *expand* el operando es un **REG**, no un `CONST_INT`. 116 B a la
  primera tras 7 formas fallidas.
- **UN `case` CUYO CUERPO ES SOLO `break;`/`return;` HACIA EL FINAL DE LA
  FUNCION: SOBRA CUANDO *FUNDE* DOS NODOS Y HACE FALTA CUANDO LOS *SEPARA*.** No
  es una regla de una direccion; se decide **contando las comparaciones del
  objetivo**, no leyendo el fuente.
  - **Sobra**: en `EAXSound::SetSFXBaseObject` (268 B) sobraban
    `case eMM_COPCAR: break;` y `case SFXOBJ_COMMON: return;` — con ellos
    `group_case_nodes` los funde con el grupo vecino y el nodo de rango pierde
    su prueba de limite inferior (`cmplwi 6; ble` contra el `cmplwi 5; blt` +
    `cmplwi 6; ble` del objetivo). Borrando los dos, **90,61% -> 100%** y los
    dos arboles casan enteros. Probado y descartado antes: intercambiar
    `break`/`return` en los seis cuerpos (**8 combinaciones, binario
    identico** — con la funcion acabando ahi son la MISMA etiqueta) y un
    `default:` explicito.
  - **Hace falta**: en `SFXCTL_AccelTrans::UpdateTRQ` (124 B) el
    `if (state > NONE && state < 5)` habia que escribirlo como `switch` con
    **`case FX_ACCEL_STATE_NONE: break;` delante** de los cuatro del rango — ese
    `case` pegado al default no emite cuerpo pero **parte el nodo** y produce el
    `cmpwi 0; beq; blt; cmpwi 4; bgt` del objetivo. 89,52% -> **100%**.
- **UN NODO DE RANGO VACIO Y UN `case` SUELTO NO CUENTAN IGUAL PARA EL PIVOTE: el
  rango resta 2 y el singleton 1.** De un pivote observado se **despeja** cuantos
  nodos faltan y **de que clase**: con un `case 4:` suelto no caia; con
  `case 0 ... 4:` caia el raiz pero el rango se fundia con el nodo 5; con **un
  grupo contiguo NO adyacente (`case 0: case 1: case 2: break;`) MAS el `case 4:`
  suelto** casan los dos pivotes. **100%, 384 B.**
- **LA ASIGNACION DEL FLAG HAY QUE ESCRIBIRLA DUPLICADA EN LAS DOS RAMAS.** Antes
  del `if/else` 99,45%, detras 98,83%, dentro del `erase` 99,37%, **en las dos
  ramas 100%** (2.012 B).
- **`expand_end_case` GENERA EL ARBOL DEL `switch` DESPUES DEL CUERPO**, asi que
  sus pseudos CC llevan numero **mas alto** que todo lo de dentro — y eso es lo
  que ordena el **pool de derrames de CR**. Si las ranuras de derrame rotan por
  bloques, mira **que estructura envuelve a las comparaciones**, no los registros.
  Reescribir un arbol de `if` como `switch` **con `default:` PRIMERO** arreglo de
  golpe las 17 ranuras del pool **y** la polaridad de un nodo: 58 -> 30 -> 13
  diffs en una funcion de 11.132 B.
- **`if (cond) { cuerpo } else break;` DENTRO DE `for(;;)` NO ES
  `if (!cond) break; cuerpo;`.** El primero deja el cuerpo como **caida** y las
  salidas como saltos —lo que emite el objetivo—; el segundo invierte los bloques
  y **rota el bucle**. Ese solo cambio valio **83,20% -> 97,60%**. Delator: los
  `bXX` de salida apuntan al bloque de tienda en vez de al de salida.
- **`li 0/beq/li 1` CONTRA `li 1/bne/li 0` NO LO DECIDE LA EXPRESION, LO DECIDE LA
  ESTRUCTURA DE SENTENCIAS.** **Nueve formas** de `return (*t != 0)` —ternario,
  `!(==)`, local, `&&`, `||`, cast, `t[0]`— dan **todas** `li 1/bne`. Solo la
  guarda pelada **mas** el `if/return` final da la del objetivo.
- **UNA `b` A UN CUERPO DE `case` EN VEZ DE AL `default` DELATA UN NODO DE RANGO
  INVISIBLE.** Un `b VINYL` **sin comparacion previa** era un `case A ... B`
  acotado por sus ancestros del arbol: faltaban cuatro `case` contiguos, y con
  ellos el pivote pasa de 0x5b a 0x53 y la funcion casa entera.
- **`while (w)` ROTA EL BUCLE y se lleva la comparacion al fondo; `for (;;) { if
  (!w) break; ... }` no.** Fue el paso 2 de tres para aplicar la receta del `cr7`
  en otra unidad (86,4 -> 91,6 -> 98,9 -> 100%), y **ninguno de los tres bastaba
  solo**: (1) sacar la accion del bucle a un `if (w)` posterior sobre la misma
  variable, (2) el `for(;;)`, (3) **una sola declaracion de `w` compartida por las
  dos ramas**, como la pone el volcado. Subir la declaracion **con inicializador**
  rompe el `goto` del arbol.
- **UN FALLO DE POLARIDAD PUEDE ESCONDERSE DETRAS DE UN CR DERRAMADO.** Habia
  `|| x == 0` donde el original tiene `!= 0`, pero **los dos lados restauran el
  mismo CR derramado** (`lwz 0x618(r1)` + `mtcrf`), asi que la unica diferencia
  visible es **`beq` contra `bne` al mismo destino**. Se resuelve leyendo el
  significado del bit eq **en un uso hermano de la misma ranura que YA casa**.
- **AFINADO de `thread_jumps`: el local NO tiene que estar ligado al resultado de
  una llamada.** Basta **una copia de un local existente**, usada *solo* en la
  guarda posterior: es un pseudo distinto cuando corre `thread_jumps` y local-alloc
  la coalesce despues, asi que **cuesta cero instrucciones**. Cerro `SpawnParticles`
  (2.780 B) **a la primera compilacion**, tras 110 formas fallidas entre dos agentes.
- **LA REGLA DE POLARIDAD APLICADA A UNA GUARDA REESCRIBE LA FUNCION ENTERA.**
  `cror un,eq,gt` + `bso` es `jumpifnot(a < b)`: si el `bso` va al `return NULL`
  **compartido**, la fuente NO es `if (t >= 0) return NULL;` sino
  `if (t < 0) { TODO EL RESTO }`. Eso llevo una funcion de 98,29% a **100%**,
  porque ademas deja el `return NULL` como **ultimo** bloque, que es el que
  sobrevive al cross-jumping. Descartados antes: `goto` explicito (92,62%),
  `else`, y tres guardas sueltas.
- **LA COLA COMPARTIDA SE CONSIGUE DEJANDO EL `return false` COMO CAIDA, NO COMO
  BLOQUE CON SALTO PROPIO.** `return f(x) != 0;` -> `if (f(x)) return true;` con el
  `return false;` **fuera** del `if`: 97,90% -> 99,95%. Y un `break` en vez del
  `return nullptr;` duplicado cierra el mismo patron en un bucle.
- **ACOTACION: LA COLA DUPLICADA NO SIEMPRE SE FUNDE.** Dos ramas de un
  `if/else if` que caen a una junta **conservan las dos copias**. Reescribir
  `if (A||B) { X; if(A){..}else{..}; <cola> }` como dos ramas con la cola duplicada
  **BAJO** una funcion de 87,53% a 83,60% y anadio 144 B. **Para que el
  cross-jumping fusione, cada copia necesita control de flujo propio.**
- **VEDA ROTA (auditoria 2-sep): `ActionQueue::FetchCurrentValues` CASA HOY AL
  100% (488 B).** Estaba aqui como «TECHO ABIERTO: `thread_jumps` funde dos
  `case` cuyo `if/else` tiene la misma forma con otro umbral, y cuesta 8 B». Lo
  que la cerro **con una sola linea** fue **no invertir la comparacion del
  segundo `case` para que espejara a la del primero**: dos `case` con la misma
  prueba en **polaridad opuesta** dejan que GCC enhebre los saltos y salgan
  `cror`/`bso` de mas; escrito el segundo con el **mismo signo** y los cuerpos
  cambiados, no.
  **El diagnostico original sigue siendo bueno como sintoma** y merece
  guardarse: el `case kPress` es `if (v < 0.5f) x=0; else x=1;` y el
  `case kAnalogPress` es `if (v >= dz) x=1; else x=0;`; el objetivo emite el
  primero como `fcmpu` + `cror un,eq,gt` + `bso` + `b`, y nosotros emitiamos
  `fcmpu` + `b` al `blt` del segundo porque GCC reconoce que su `blt` prueba lo
  mismo sobre el `cr0` recien puesto. **Lo que NO valia: un `break;` explicito
  dentro de cada rama** — la regla de «cada copia con control de flujo propio»
  no aplica cuando lo que se funde es el SALTO y no la cola. **Moraleja: un
  techo de `thread_jumps` cede a la POLARIDAD del hermano antes que a la forma
  del bloque.**
- **ENVOLVER LA LLAMADA A UN INLINE CON GUARDA EN `if (handle)` FUNDE LA GUARDA DEL
  INLINE** y convierte su cola `b; li r3,0` en un acumulador en registro salvado.
  `T *p = NULL; if (h) p = Find(h); if (p)` da `li rSalvado,0` + dos `beq` directos
  al test del llamante; `T *p = Find(h);` da la cola compartida en `r3`. **Partir
  solo la declaracion no basta (CSE lo propaga).** Delator: al objetivo le sobra un
  `li rN,0` temprano **con la linea del `.cpp`, no la de la cabecera**. 400 B.
- **LA GUARDA INVERTIDA DECIDE LA POLARIDAD DEL TERNARIO DE UN `GetNext()`.**
  `if (off != 0) return this+off; return 0;` deja el `add` en la caida;
  `if (off == 0) return 0; ...` deja el `li 0`. **Las dos formas conviven en la
  misma funcion del objetivo** y hay que elegir cada una por separado.
- **LA POLARIDAD DE LA GUARDA DECIDE SI GCC IZA LA ASIGNACION DUPLICADA DE LAS DOS
  RAMAS.** `if (c) { x = false; } else { x = false; <cuerpo grande> }` iza el
  `li 0` **delante** del test y funde las dos copias; escrito al reves —la rama
  grande primero— el original conserva las **dos**. 96,83 -> 99,31% con ese solo
  cambio. **Delator: un `li rX,C` que en el objetivo aparece DENTRO de una rama y
  en el nuestro antes del salto.**
- **`if (n > 0) { for (i=0;i<n;i++) ... }` ES SOLO EL `for`.** La guarda redundante
  anade `mr. rN,rArg`+`ble` y **permuta los parametros de registro**. El arbol del
  DWARF lo canta antes de compilar: sin `if`, el `for` deja `n` en `b0` y el cuerpo
  en `b0/b0`; con `if` sobra un nivel. 85,18 -> 100%.
- **LA GUARDA POSITIVA CONTRA EL `return` TEMPRANO DECIDE `bgt` PELADO CONTRA
  `cror`+`bso` EN FLOTANTE.** `if (t > 1.0f) return false; <cuerpo>` da
  `cror un,eq,lt; bso` con los bloques invertidos; `if (t <= 1.0f) { <cuerpo>;
  return true; } return false;` da un `bgt` pelado al bloque de fallo. Un solo
  cambio dejo `BoxVsBox` en tamano exacto.
- **EL `return` EXPLICITO DEL ULTIMO BLOQUE DECIDE QUE COPIA SOBREVIVE AL
  CROSS-JUMPING.** Invertir dos guardas fundio las colas de llamada virtual, pero
  **sobre una tercera copia al final de la funcion**; anadir `return true;` dentro
  del ultimo bloque (para que su cola caiga al epilogo en vez de saltar) lo corto:
  95,51 -> 99,12%.
- **NINGUNA FORMA DE `if` REPRODUCE UN LIMITE SIN CANONICALIZAR.**
  Micro-banco de **seis** formas (`>=C`, `>C-1`, `!(<C)`, `<C`, `-1 < x-C`, y la
  version sin signo): **las seis canonicalizan**. *(Lo que si lo da: `case LO ...
  HI:`, un bucle con cota variable, o la constante en un local — §7.)* Un `switch` de un solo rango sustituyendo un `if` anidado dio a
  `CarRenderInfo::Render` (11.132 B) **tamano exacto** y el `stmw r14` identico.
  **Corolario: la canonicalizacion sobrevive aunque la comparacion se ice fuera
  del bucle y se derrame con `mfcr`/`stw` — es de `fold`, no de `combine`.**
- **UN OBJETO CON DESTRUCTOR DECLARADO ANTES DE UNA GUARDA DE SALIDA TEMPRANA
  EMITE EL DESTRUCTOR ENTERO EN ESA SALIDA** (11 instrucciones de mas). Moverlo
  detras de la guarda: 71,45% -> 82,27%. **Y la posicion fina importa**: hay que
  ponerlo ademas detras de los stores al objeto que la guarda comprueba, porque
  los stores a bases distintas no se reordenan y su orden relativo es literalmente
  el del fuente. Con eso, 100%.
- **LA CADENA DE `goto` DESCOMPILADA A MANO CONTRA EL `switch` REAL: LOS CUERPOS
  VAN DESPUES DEL ARBOL.** El objetivo emite el arbol entero y luego cada cuerpo
  terminando en `b <continuacion>`; la cadena a mano intercala cuerpo y test con
  `bne`. Reescribir `ShapeMemoryAllocator::Alloc` como `switch` fue **76,79%
  (27 diffs) -> 97,39% (4 diffs) en una compilacion**. **El delator en NUESTRA
  fuente son etiquetas `next:`/`tag_gt_2:` sobre un entero — un `grep`.**
- **ACOTACION: el bloque de cola compartido NO siempre es un `goto`.** En
  `WGrid::FindNodes` las cinco copias de `clear(); FindNodes(...); return;`
  parecen compartidas, pero escribirlo con `goto` bajo de 93,01% (3.416 B) a
  **86,17% y 3.096 B**: el objetivo las duplica las cinco. **Manda el tamano.**
  *(La funcion cerro despues al 100% —3.456 B— por la via de la aritmetica del
  delta de mnemonicos: cinco bucles, dos ascendentes y dos descendentes. La
  acotacion del `goto` sigue siendo buena.)*
- **`for (i=0;i<n;i++)` CONTRA `while (i<n) { ...; i++; }` CON LLAMADAS DENTRO:**
  el `for` deja el contador en un *scratch* (regmove parte el rango:
  `addi rSalvado,rScratch,1` arriba y `mr rScratch,rSalvado` abajo); el `while` lo
  clava en un salvado e **iza la comparacion a `cr4`**, y eso cuesta `mfcr` +
  `stw r12`.
- **El avance del bucle DENTRO del cuerpo corre ANTES de los destructores del
  ambito; en la tercera clausula del `for`, DESPUES.** Delator: `bl ~X` y el
  avance intercambiados. 93,5 -> 99,99%.
- **`return a == b;` con objetos que destruir da UNA cola de limpieza;
  `if (a==b) return true; return false;` da DOS.** La limpieza duplicada en el
  objetivo es el delator.
- **GCC 2.9 solo tiene patron SIN SALTO para `== 0`.** Si el objetivo es
  branchless ahi, el fuente comparaba contra cero — **aunque eso invierta la
  polaridad que dabas por buena**.
- **Guarda cuyo `return 0` se cross-jumpea con el `return 0` final**: la guarda
  sale de DOS instrucciones (`bne cuerpo; b ret0`) y el bloque de retorno queda
  **entre la guarda y el cuerpo**, alcanzado hacia atras.
- **UN `beqlr` (rama y retorno fundidos) DICE QUE R3 YA TRAE EL VALOR DE RETORNO,
  y con eso se lee cual de los dos `return` es la CAIDA.** Si el objetivo carga
  algo en `r3` y acto seguido hace `cmpw; beqlr`, ese valor es el retorno del
  camino que **cae fuera del `if`**: la forma es
  `if (cond) { ...; return A; } return B;`, no `if (!cond) return B;`. Escribirlo
  al reves cuesta **tres instrucciones** (`bne`/`mr r3,rN`/`blr`) en vez de una.
- **ACOTADO: pasar un `do/while` a `while` no siempre es gratis.** En
  `FindConditionBranchTarget` la forma `while (A && B)` reproduce el `b` a la
  prueba y el `if (!B)` en la cabeza del cuerpo que tiene el objetivo, pero cuesta
  **8 B** y baja de 66,81% a 62,64%. El tamano exacto lo da el `do/while`. Medido
  en las cuatro combinaciones de forma del `if` y forma del bucle.
- **`case 0:` + `default:` ESCRITO PRIMERO mueve el pivote Y coloca el cuerpo del
  default delante.** Con casos {1,2,3} sobre un `uint8` la forma normal pivota en
  2; `case 0: default: X; break;` **antes** de los casos da
  `beq 1 / ble default / beq 2 / beq 3` con el default de caida. Micro-banco de 8
  variantes, solo esa. 252 B.
- **DOS `case` ADYACENTES CON EL MISMO CUERPO SON DOS NODOS SI EL CUERPO SE
  ESCRIBE DOS VECES** (etiquetas distintas -> `group_case_nodes` no funde -> dos
  `beq`; el cross-jumping funde los bloques despues). **Ese es el dial para
  separar un rango en singletons y mover el pivote.** 752 B en dos funciones.
- **Tres `case` vacios adyacentes con `default: goto` se funden en UN NODO DE
  RANGO** y emiten `cmplwi hi; bgt def; cmplwi lo; blt def` — limites sin
  canonicalizar que ningun `if` produce. Firma de un switch con agrupacion previa.
- **`cmpwi rX, -0xNN` contra una DIRECCION no es un `case`: es un test de nulo
  plegado.** `&arr[1]` con `sizeof = 0x68` se compara `cmpwi r3,-0x68` porque
  `fold` mueve el desplazamiento al otro lado. Es el falso positivo tipico de
  `casetree.py`.
- **`goto` explicito a la rama compartida contra escribir la llamada dos veces**:
  hay sitios donde GCC **no** cross-jumpea las dos copias y el `goto` si da el
  salto directo al bloque comun. +3,5 puntos. Es lo contrario de la entrada de
  los tres `case` duplicados: hay que probar las dos.
- **EN MWCC, LA FORMA DEL INDICE CAMBIA EL FACTOR DE DESENROLLADO.**
  `for (i=0;i<N;i++) base[i]=0;` desenrolla **x16** y precomputa 8 offsets;
  `for (i=0;i<N*sizeof(T);i+=sizeof(T)) *(T*)((u32)base+i)=0;` desenrolla **x8**
  con un unico offset corriente. **328 B en `vm` y 560 B en `vmbase`.**
- **Un `bl` a una funcion pequena del MISMO TU prueba `-inline off`** en MWCC.
  Cambiar `-inline auto` por `-inline off` paso `vm` de 204 a 616 B en un paso.
- **LA GUARDA + `do/while` CON LA QUE SE RECONSTRUYE UN `for` AL DESCOMPILAR
  ROMPE EL CSE.** `if (i < N) { do { ... } while (i < N); }` tiene **dos**
  lecturas de `N` en la fuente donde el `for` tiene **una**. Si `N` es un miembro
  y dentro del cuerpo hay una llamada, esa lectura de mas obliga a **recargar**:
  el objetivo reusa el valor que ya trae de la condicion y nosotros emitimos un
  `lwz` extra. GCC genera igualmente la guarda + `do/while` a partir del `for`, o
  sea que **la forma corta no cuesta nada y el CSE sale gratis**. Cerro
  `AllocateStrings` (472 B) cambiando solo eso.
  **PRECONDICION, medida despues: no basta con que haya guarda + `do/while`.**
  Tiene que darse **la condicion leyendo un MIEMBRO** *y* **una llamada en el
  cuerpo** *y* **ese mismo miembro leido otra vez dentro**. Sin las tres, la
  conversion da binario identico, porque GCC ya genera la guarda + `do/while` a
  partir del `for`. El arbol tiene **76 sitios con el patron pero solo 5** que
  cumplen la precondicion: no es un barrido, es una lista corta.
  Y no es mecanico: `for` y `while` diseminan distinto el `continue` (ver la
  entrada del destino del `continue`), asi que hay que medir cada uno.
- **EL DESTINO DEL `continue` DISTINGUE `while` DE `for`.** En un `while` salta
  a la condicion; en un `for` salta al **tercer termino**. Si al objetivo le salta
  a una sentencia que parece la ultima del cuerpo, esa sentencia **es el
  incremento del `for`**. Y GCC 2.9 expande el incremento con el `lineno` de la
  **llave de cierre**, que es por lo que el mapa de lineas lo situa 200 lineas mas
  abajo que la cabecera del `for` — no es un bug del mapa. Confirmado dos veces en
  la misma unidad; llevo `SerializeTable` de 99,90% al 100%.
- **El bloque lexico del volcado DWARF que termina ANTES de la ultima sentencia
  del cuerpo de un bucle es la firma de ese `for`**: el incremento queda fuera del
  cuerpo, y el cuerpo de un `while` lo incluiria. Se lee sin compilar nada.
- **EL AGRUPAMIENTO DE UNA CONDICION SE LEE DEL DESTINO DEL SALTO, NO DE LA
  FORMA.** Un `bgt` que salta al **tercer** test y no al cuerpo significa
  `if ((A || (B && C)) && D)`, no `if (A || (B && C && D))`.
- **`(x & A) || (x & B)` tambien se pliega a `& (A|B)`** — la entrada anterior
  solo lo daba para `&&`. Dos `if` separados conservan los dos `andi.`.
- **`use_cost_table` SE APAGA si algun `case` sale de [-1,127]**, y eso cambia el
  pivote del arbol entero. Es lo que explica los pivotes que la formula no
  predice. Un generador que compile 200 conjuntos de `case` de golpe y compare la
  secuencia (constante, mnemonico) lo resuelve en un minuto.
- **Dos grupos de `case` vacios ADYACENTES EN VALOR se funden en UN nodo** aunque
  esten en grupos distintos (`group_case_nodes` los une si el primer insn real de
  las dos etiquetas es el mismo salto). Se separan poniendo `return;` en uno:
  mismo binario, otro destino.
- **`case A ... B:` COMPILA CON ProDG 3.9.3, y es la via mas limpia de las tres
  para un limite sin canonicalizar (§7).** **La extension GNU de rangos reproduce
  el arbol exacto**, con los `bge`/`ble` contra el literal que un `if` nunca
  emite.
  **Cinco funciones cerradas** reescribiendo una cadena de `if` como `switch` con
  rangos, una de ellas instruccion por instruccion a la primera. **El delator en
  NUESTRA fuente es `if (x != C) { if (x > C) {...} }`** con la misma variable y
  constante — artefacto de descompilar el arbol a mano; un `grep` lo localiza.
- **La formula del pivote, completa:** el bucle arranca en
  `i = (n + rangos + 1)/2` y luego **resta 2 por cada nodo de rango y 1 por cada
  singleton**, parando en `i <= 0`. Con eso se predicen el pivote **y los
  subarboles** sin compilar (verificado con 16 nodos y 2 rangos). Y si el pivote
  del objetivo no cuadra con los casos visibles, **despeja `n`**: en una funcion
  con 8 casos que pivotaba en el indice 2, `n` tenia que ser 10, y poner dos
  `case X: break;` hizo casar el arbol entero.
- **El bloque que se emite DESPUES del codigo posterior al `switch` no es un
  `default:`**: es un `goto` a una etiqueta que va detras. Sacar la asignacion
  fuera del switch llevo una funcion de 62,36% a **98,94% con el tamano exacto**.
- **El cuerpo compartido por tres `case` se escribe DUPLICADO, no con `goto`**: el
  `goto` dejaba 36 B de menos; escrito en los tres, el cross-jumping lo funde
  solo y la funcion cierra.
- **`thread_jumps` SE ROMPE CON UNA COPIA LOCAL. NO es incondicional.** Una
  funcion a **99,995% con un solo diff** (nuestro `bne cr7` saltaba 3
  instrucciones mas lejos porque GCC enhebro un re-test redundante del mismo
  miembro) cedio al ligar el valor a un local usado **solo** en la guarda
  interior: `bool a0 = o0->IsImmobile();`. El pseudo se mantiene distinto y el
  enhebrado no ocurre. **4.384 B.** **Pruebalo SIEMPRE antes de dar por vetado un
  salto enhebrado** — incluidas las funciones que ya estan en esta lista.
- *(matizado)* Con todo, hay saltos que no se arreglan con
  la forma del fuente.** En una funcion a **un solo diff** se probaron **110
  formas** entre dos agentes (18 + 92 en micro-banco: variantes del `if/else if`,
  del `if` final, separadores sin codigo, formas exoticas y pseudos distintos) y
  **todas** dan el mismo salto. Cuando el enhebrado no cede a nada, el techo esta
  en una diferencia de RTL anterior al planificado.
- **GCC 2.9 PODA LAS ULTIMAS COMPARACIONES DE UNA CADENA `&&` LARGA CON CUERPO
  VACIO.** Doce `!=` escritas emiten ocho `fcmpu`; para que salgan las **doce**
  del objetivo hay que escribir **dieciseis** (anadiendo una fila que el original
  no compara). **El delator es contar los `fcmpu` de los dos lados**, y la cuenta
  no es la del fuente.
- **LA ARITMETICA DEL PIVOTE, confirmada por dos agentes el mismo dia y con el
  matiz que la hace usable:** GCC 2.9 elige el pivote con
  `cost = (n_nodos + n_rangos + 1)/2` y avanza `cost-1` posiciones en la lista
  ordenada. **Contando los `cmpw` del objetivo se deduce cuantos `case` tiene el
  original**, aunque no emitan codigo. Cerro tres funciones (732 + 1.068 + 316 B)
  con `case` vacios y con `case` que comparten etiqueta con el `default`.
  **Matiz: un `case` con cuerpo propio identico al `default` SI emite su
  comparacion; compartiendo etiqueta con el `default` no emite nada pero cuenta
  igual en el arbol.**
- **El ORDEN de los `case` decide la forma de los bloques, no la forma del `if`.**
  Con dos cuerpos semanticamente identicos, el primero emitido sale `beq A; b B`
  y el segundo `bne B; caida`; con el mismo texto en otro orden GCC los funde.
  **Empareja cada `case` con la POSICION, no con el texto.**
- **`(x & A) == 0 && (x & B) == 0` lo pliega `fold` a `(x & (A|B)) == 0`**; los
  dos `andis.` separados del original salen solo con `if`s anidados.
- **`balance_case_nodes` PERMITE CONTAR LOS `case` QUE FALTAN.** Elige el indice
  `(nodos + rangos + 1)/2 - 1`, donde **dos `case` adyacentes con la misma
  etiqueta cuentan como UN nodo pero DOS en `rangos`**. Con esa aritmetica se
  deduce cuantas etiquetas ocultas tiene el original: asi salieron los tres
  `case` que le faltaban a una funcion, y su arbol paso a casar entero. **Un
  pivote raro se resuelve con cuentas, no probando.**
  **Limite medido**: la formula **valida siempre NUESTRO lado**, pero no
  siempre explica el del objetivo. En una funcion con 9 casos y 0 rangos
  predice el pivote que emitimos (indice 4) y el objetivo pivota en el 3 **con
  los mismos 9 valores presentes en su arbol**; los cinco `case` ocultos
  posibles dan **binario identico entre si** (lo que confirma la otra mitad de
  la regla) y **todos empeoran**. Cuando la cuenta no cuadra, el pivote viene
  de otra parte: no sigas probando etiquetas.
- **El arbol de bloques del DWARF dice DONDE ESTA LA GUARDA.** Siete locales que
  `regmap` da en `fn` y el original en `b1` significan que el original **envuelve
  todo el cuerpo** en `if (x != 0) { ... }` en vez de usar un `return` temprano.
  Un solo cambio realineo 12 de 14 ambitos.
- **`if (!A) { if (B) X else Y } else { X }` es `if (A || B) X else Y`.** Y un
  `else if` en el nivel de anidamiento equivocado mueve una rama entera: el
  delator son **dos destinos de salto INTERCAMBIADOS**.
- **El comparador de saltos NORMALIZADO POR DESPLAZAMIENTO (`dest - addr`, no
  `dest`) es el detector estructural mas rapido**: en una pasada senalo cuatro
  funciones con el arbol mal. Queda en `scratchpad/spc_br.py`.

### 6.2 Cross-jumping: quién se funde y hacia dónde

- **LA DIRECCION DEL CROSS-JUMPING DE DOS `case` CON CUERPO IDENTICO LA DECIDE LA
  DISTANCIA ENTRE ELLOS EN EL FUENTE.** Delator: un diff formado **solo** por
  tripletes `cmpw rX,r0` / `beq cuerpo` en DELETE de un lado e INSERT del otro,
  con el arbol del switch identico. Intercambiar las dos etiquetas **no hace
  nada** (los cuerpos son identicos, el objeto sale byte a byte igual). El dial
  es la distancia: **casos ADYACENTES** hacen que la hoja anterior salte hacia
  adelante; **separados por al menos otro `case`**, salta la posterior hacia
  atras. Mover un `case` una ranura cuesta **cero instrucciones**. En una funcion
  de 138 casos y 30 grupos duplicados fallaban cuatro grupos: **3.784 B**.
  **ACOTADO por el estudio posterior (entrada siguiente): esto vale SOLO para
  el despacho del `switch`.** La cola duplicada en si no la decide la
  distancia, sino **como termina el bloque de cada copia**. Si el diff no son
  tripletes `cmpw`/`beq` sino una tirada de N instrucciones que sale `DELETE`
  en un sitio e `INSERT` en otro, la regla que aplica es la de abajo.
- **Regenerar el `switch` entero en un micro-banco** desde la lista de casos
  reproduce exactamente nuestro conjunto de saltos (9 de 9 comprobados) y baja la
  iteracion de 50 s a 1,5 s. Es lo que hace buscable un switch de 138 casos.
- **LA COLA DUPLICADA FUERA DE UN `switch`: LO QUE DECIDE SI UNA COPIA SE FUNDE
  ES COMO TERMINA SU BLOQUE, Y LA DIRECCION LA DECIDE DONDE ESTA LA ETIQUETA
  COMPARTIDA.** Dos reglas independientes, las dos medidas en micro-banco:
  1. **Quien se funde.** Una cola identica repetida se funde **solo si su bloque
     termina con un control de flujo propio** (`return`, `goto`, `break` que
     emita salto). Si en vez de eso el bloque **cae a una junta** -el
     `if/else` con un unico `return` al final, o el ternario- la copia
     **se queda**, y ademas **no sirve de destino** para las demas. Medido con
     seis formas sobre la misma cola: `if (c) { X; return v; } Y; return w;`
     funde; `if (c) { X; } else { Y; } return v;` NO; `if/else` con `return` en
     **las dos** ramas funde; la guarda invertida funde; el ternario no.
     **Es local a cada copia**: en un mismo `switch` puedes elegir cual sobrevive
     escribiendo esa y solo esa en la forma `if/else`.
  2. **Hacia donde.** Todas las copias que se funden colapsan sobre **la mas
     LEJANA a la etiqueta compartida**, y los saltos resultantes apuntan **en
     sentido contrario a la etiqueta**. Con la etiqueta *detras* (el caso
     normal: el epilogo, el `return` comun) sobrevive la **primera** y las
     posteriores saltan **hacia atras**; con la etiqueta *delante* (una cabecera
     de bucle, un `goto` hacia arriba) sobrevive la **ultima** y las anteriores
     saltan **hacia adelante**. Comprobado con 2, 3 y 4 copias.

  **Como se reconoce en el diff**: mismo tamano en los dos lados y el diff son
  solo un `DELETE` de N instrucciones en un sitio y un `INSERT` de **las mismas
  N** en otro, mas los `ARG_MISMATCH` de los destinos de salto que eso arrastra.
  **Que se escribe para invertirlo**: cambiar `if (c) { …cola; return v; } …` por
  `if (c) { …cola; } else { … } return v;` en la copia que quieres conservar, y
  al reves en la que quieres fundir. **Cuesta cero instrucciones.** Cerro
  `NFSMixShape::GetCurveOutput` (**584 B**): cuatro copias del clamp
  `if (n < -0x2580) n = -10000;`, y bastaba con **intercambiar las formas** de
  `SHAPE_DWN_EQPWR_SQ` y `SHAPE_DWN_ONE_MIN_EQPWR_SQ`.
- **Las colas de limpieza de destructores obedecen la misma regla, pero el
  BLOQUE DE FIXUP de un `goto` hacia adelante no.** Un `goto` hacia **atras**
  ejecuta los destructores **en linea** en el sitio del `goto`; uno hacia
  **adelante** deja un bloque de fixup aparcado **justo antes de su etiqueta**,
  y varios fixups a la misma etiqueta se cross-jumpean entre si formando un solo
  bloque con **dos puntos de entrada** (`mr r3,rA` / `b` / `mr r3,rB` / cola
  comun). En `CSTATEMGR_CarState::ResolveCarBanks` el objetivo tiene ese bloque
  de 36 B **antes de `BeginRule3`** y nosotros antes de `LoadRemainingEngines`;
  **no se ha encontrado la palanca de fuente que lo mueve** -probadas 36
  combinaciones de forma del `goto` y 8 de ambito, todas dan el mismo tamano-.
  El micro-banco que lo reproduce entero en 63 instrucciones esta en el
  scratchpad (`cj/h0.cpp`, `cj/g0.cpp`).
- **`if (!A || B) x=0; else x=1;` da el arbol de saltos; `if (A && !B) x=1; else
  x=0;` NO** — GCC 2.9 lo pliega a `x = A && !B` y lo expande **como valor**
  (`li 0` + `jumpifnot` + `subfic/adde`, o `mfcr/extrwi`). Micro-banco con 25
  formas: solo la ORIF con el `0` en el *then* conserva las ramas. Y **el orden
  de operandos de la segunda condicion decide que se recarga**.
- **`(A && B) || (C && D)` tiene firma propia**: `!A -> L2`, `B -> SI`,
  `L2: !C -> NO`, `D -> SI`. En flotante son `cror un,eq,lt` + `bso`, salto
  directo, `cror un,eq,gt` + `bso`, salto directo, **con el segundo `cror`
  reusando el `fcmpu` del primero**. Si ves ese `cror` doble sobre un solo
  `fcmpu`, **no es De Morgan**: no lo reescribas como `(a<=0||A>=a) && ...`,
  porque ademas difiere en el caso cero.
- **Cola bajo condicion negada contra `return` temprano.** `beq epilogo; b cola`
  sale de `if (!cond) x = ...;` **con la cola escrita en las dos ramas**
  (cross-jumping); `bne cola; b epilogo` sale del `return` temprano. Lo que
  decide el *sentido* del salto no es la guarda, sino si el cuerpo es una
  asignacion o un return. 204 B.
- **`if (A && B) {X} else {Y}` NO es `if (A) { if (B) {X} } else {Y}`** en cuanto
  existe un tercer caso: en la segunda forma el fallo de `B` cae **pasado** el
  bloque `else`, no dentro. El delator es un `bne` cuyo destino está unas pocas
  instrucciones **más allá** del destino del `beq` de `A` — un solo salto de
  diferencia en todo el diff. Cerró `GcPad::Update` (**1.076 B**) desde 99,98%.
  Y el guardián exterior puede hacer el tercer caso imposible sin que el
  compilador lo sepa: la forma sigue mandando.
- **La selección condicional de una constante de `enum` ligada a una LOCAL emite
  un solo store**; escrita como `campo = A; if (c) campo = B;` emite dos **y**
  ademas iza las dos constantes a registros salvados fuera del bucle. El delator
  son dos `li` de mas vivos en todo el bucle. 97,77% -> 99,98%.

- **El ternario decide la POLARIDAD de la rama, no solo la canonicalizacion.**
  `if (a <= b) {A} else {B}` emite `cror un,eq,lt; bso A; b B` (tres
  instrucciones, salta al *then*); `return a > b ? B : A;` emite `fcmpu; bgt B;
  b A` (dos, salta si falso). Es `do_jump`: el COND_EXPR entra por `jumpif` y el
  `if` por `jumpifnot`. **Un `bgt` o `blt` pelado donde nosotros emitimos
  `cror`+`bso` con los mismos operandos es un ternario.** Probadas las nueve
  alternativas en micro-banco; ninguna otra lo da.
- **La cola `return 0;` compartida va DESPUES del ultimo `return` real**, y se
  llega a ella con un `goto` explicito. Delator: el original tiene un `b` de mas
  justo antes de su `li r3,0`. Intercambiar las etiquetas a secas cambia la
  semantica y cuesta un registro salvado (99,01% -> 98,46%, marco 0x58 -> 0x60).
- **`cmp cr7` solo sale si el pseudo CC se referencia en MAS DE UN bloque
  basico**, y eso lo producen TRES cosas (la lista estaba corta): la comparacion
  invariante de bucle **izada** fuera del bucle, la materializada como valor (que
  sale con `mfcr`), y **una sola comparacion que alimenta DOS saltos en bloques
  distintos** — CSE (`-fcse-follow-jumps`/`-fcse-skip-blocks`) funde los dos
  tests y luego `jump_optimize` borra el segundo salto, dejando un `cmpw cr7`
  con **un unico** `bXX cr7` y sin `mfcr`. Fundida con su salto en el mismo
  bloque, **siempre `cr0`**. Corolario: **no se arregla reescribiendo la
  expresion**. Ver la entrada larga de la seccion 7.
- **El ternario resiste la canonicalizacion que el `if` sufre.** `if (x < 1) x =
  1;` se pliega SIEMPRE a `cmpwi 0; bgt`; el ternario da `cmpwi 1; bge`. Y **la
  seleccion condicional de un valor es un TERNARIO, no un `if`**: si al diff le
  faltan `mr` de un valor recien cargado que tambien se usa en la comparacion, la
  fuente tenia un ternario. **Valio 3.988 B de golpe.**
- **La guarda invertida**: un `bne` al cuerpo con el retorno de fallo en la caida
  es `if (x != FALLO) { cuerpo; return r; } return 0;`, no un guard clause.
- **Un salto incondicional a un bloque de cola compartido es un `goto`,** no una
  llamada duplicada: duplicar da `bl X` mas `b fin`, el `goto` da el `b` pelado.
- **Rotacion de bucle, las formas medidas**: `if (r >= 0) acc += r; else break;`
  NO rota; `if (r < 0) break; acc += r;` SI. El `for(;;)` con el decremento
  dentro deja un solo `subi` en la cabeza; el `for` con decremento en la
  actualizacion lo duplica. Con una **llamada** en el test, el `while` con
  asignacion rota y el `do/while` no; sin llamada, al reves.
- **Pre-test y arista de retorno con limites DISTINTOS son dos condiciones**, no
  un `break`: un `bge` arriba y un `ble` abajo delatan un `if` envolviendo al
  `while`.
- **El pivote del arbol de un switch es la mediana de los `case`.** Un pivote raro
  puede no ser un case que falta sino un case **equivocado**.
- **El fallthrough de un `switch` se escribe DUPLICADO.** `case A: x; case B:
  y; break;` con caida literal rematerializa la constante del cuerpo (8 B de
  mas). El original escribia el cuerpo entero en los dos casos y dejaba que GCC
  hiciera cross-jumping. **Valio 2.916 B de golpe**, cuatro funciones de 580 B.
  **CORREGIDO, y el matiz decide**: el fallthrough literal **si existe**. En
  una funcion, duplicar la cola midio **88,46%** y el fallthrough literal
  **100%**. La regla de duplicar vale cuando los dos cuerpos **no** son
  adyacentes en el orden de emision; cuando si lo son, el original cae de
  verdad. Se decide mirando el orden de emision, no el texto.
- **La guarda de bucle es un `continue` negado.** `srw; xori 1; andi.; bne`
  (tres instrucciones) es `if (!(x & 1)) continue;`; `andi.; beq` (dos) es
  `if (x & 1) { ... }`. Y el desplazamiento es `srw`, no `sraw`, o sea
  `((unsigned)x >> t) & 1`.
- **`if (!p) return 0; return p->x;` contra el ternario NO son intercambiables,**
  y cual gana depende de si la funcion llama a alguien: **sin llamadas**
  (epilogo `blr` pelado) GCC fusiona con `beqlr` y hay que usar el **ternario**;
  **con llamadas**, la **guarda negada**. Una funcion 26%->100%, otra 68%->100%.
- **Un `enum` con un valor `0xFFFFFFFF` es UNSIGNED** y el arbol del switch usa
  `cmplwi`; escrito `-1` sale `cmpwi` y otro arbol distinto.
- **Una cadena de `else if` sobre un entero era un `switch`**: árbol contiguo
  `cmpwi 1;beq;cmpwi 2;beq;b`. **El orden de los `case` en fuente es el orden de
  emisión de los cuerpos.**
- **Un `switch` de DOS casos sobre hashes no es un `switch`**: dos hashes dan
  `beq A; beq B; b def`; un `beq A; bne def; b B` **sólo** sale de
  `if (d==A || d==B) { if (d==A) … else … }`.
- **`return X;` en un `case` donde el original tenía `break;`** genera un bloque
  propio; con `break` cae al `return` común y GCC lo comparte.
- **El despacho de un `switch` lleva la línea de su LLAVE DE CIERRE.**
- **`if (A) { X; return; } Y; return;` invierte el bloque**: hay que escribir
  **la otra rama** como cuerpo.
- **Si varios bloques `if` saltan todos a la cola, son UNA cadena `else if`.**
- *(pista, sin cifra)* **Guardas compuestas partidas**: `if (A && B)` puede ser
  `if (A) { if (B) … else if (C) … } else if (C) …`; **el retest muerto del árbol
  delata el conjunto que falta**.
- **`while (c) {...}` rota el bucle; `for (;;) { if (!c) break; ... }` no.**
- **Hay TRES formas de bucle, no dos, y la tercera es la que más aparece.**
  `while (c) {...}` emite `b` al test del fondo (el `for` es igual); pero si el
  objetivo prueba la condición **arriba y abajo** (`cmpwi; ble` de entrada y
  `bgt` al final, sin el `b` inicial) hay que escribir la guarda a mano:
  `if (c) { do { ... } while (c); }`. Es `duplicate_loop_exit_test`, y GCC no lo
  hace solo con estos cflags. Cerró `suppf`, `supmutlf`, `supxalf` y `sfecho`.
- **`while ((p = f()) != 0)` borra el test nulo que `delete p` emite**;
  partirlo en `p = f(); while (p) { …; p = f(); }` lo conserva.
- **Un `do { } while(dupe);` con la variable declarada FUERA** permite izar los
  `lis` invariantes al preheader.
- **El cross-jumping deja huella en el DWARF**: un bloque léxico de rango cero
  que declara constantes ausentes del código emitido es una cola fusionada.

- **TRES `if (X) return false;` SEGUIDOS DEJAN DOS COLAS; `if (A) return false;
  if (B || C) return false;` DEJA UNA.** El cross-jumping funde la segunda copia
  con la primera pero **no siempre la tercera**: el delator es que a nosotros nos
  sobran exactamente `li r3,0` + `blr` al final de la funcion y el ultimo `bne`
  del objetivo apunta al bloque de la PRIMERA copia. Fusionar las dos ultimas
  condiciones con `||` cerro `AreMomentCamerasEnabled` (**84 B**, 90,24 -> 100%).
- **UN BLOQUE `if` QUE CAE A UNA JUNTA NO SE CROSS-JUMPEA; CON SALTO PROPIO SI.**
  Delator: al objetivo le sobran el `bl` y su `stw` justo antes de un `b` comun, y
  nuestro `b` salta a una copia compartida de esas dos instrucciones. Se separa
  metiendo una variable de bloque para el cuerpo y dejando la sentencia comun
  DETRAS del `if`: `iconType` dentro, `checkBinIndex = 0;` como junta. Lo canta
  el volcado, que declara la variable interna en su propio bloque anonimo. 8 B
  en `GTrigger::GTrigger`.

### 6.3 La saga del `cr7` (VEDA ROTA — se conserva por el método)

- **LA VEDA DEL `cr7` DE `FnDeltaQ` ESTA **ROTA** — LA RECETA ESTA DOS ENTRADAS
  MAS ABAJO («`if (c) X; else X;` en un bloque POSTERIOR»), Y ESTO ES EL CAMINO
  QUE LLEVO HASTA ELLA.** *(No lo leas como trabajo pendiente: `FnDeltaQ::EvalSQT`
  4.980 B, `FnDeltaQ::EvalSQTMasked` 5.412 B y `FnDeltaQFast::EvalSQTMask`
  2.124 B casan hoy al 100%. Se conserva por el metodo, que es el mejor ejemplo
  del manual de como se acota una veda: 46 + 15 formas medidas antes de dar con
  la buena.)* La causa #3 es la buena y se reproduce en la funcion real; lo que
  faltaba era borrar el segundo salto. Contra las 46 formas
  de UNA sola comparacion que ya se habian medido (todas `cr0`), **escribir la
  comparacion DOS VECES en bloques distintos da `cmpw cr7` a la primera**. En
  `FnDeltaQ::EvalSQT` la variante
  `if (mPrevKey == -1 || ... || (preventReverse && floorKey < mPrevKey))`
  emite `cmpw cr7, r19, r7` + `bge cr7` **exactamente como el objetivo** y con el
  tamano intacto (4.980/4.980). Lo unico que sobra es el **segundo** `bge cr7`.
  Medido:
  - **Solo cuenta si el segundo test esta VIVO.** Con cuerpo vacio, con una
    asignacion muerta, con el test anidado redundante (`if (c) { if (c) … }`) o
    recomputando un valor que CSE sabe igual, `jump_optimize` lo borra **antes**
    de CSE y vuelve `cr0`: ocho formas, las ocho dan el binario de partida.
  - Las que SI dan `cr7` -`if (c) {cuerpo vivo}` suelto detras,
    `(preventReverse && c)`, `(c && preventReverse)`- dejan **dos** `bXX cr7`.
  - **El sitio importa**: como ultimo termino del `||` el segundo salto no puede
    morir nunca, porque su destino (el `else`) no coincide con su caida (el
    `then`). Hay que buscarle un sitio donde el salto muera en el
    `jump_optimize` FINAL (el de despues de reload, con cross-jumping), no en el
    de despues de CSE.
  - **Y el truco de las DOS RAMAS IDENTICAS tampoco vale**: `if (c) x = 0; else
    x = 0;` con la misma comparacion sale **binario identico al de partida**
    (4.980/4.980, los mismos 9 diffs), o sea que GCC lo pliega **antes** de CSE
    igual que los demas; duplicando el bucle entero en las dos ramas se va a
    5.320 B. Contadas con estas, van **quince** formas medidas sobre la funcion
    real.
  Vale para `FnDeltaQ::EvalSQT` (4.980 B) + `FnDeltaQ::EvalSQTMasked` (5.412 B) y
  ademas es la mitad de lo que separa a las dos `FnDeltaQFast`.
  **Aviso**: `Hermes::System::RemovePortMessage`, que este playbook citaba como
  ejemplo vivo de la causa #3, tiene su `cmpw cr7` **pegado a su unico `bne cr7`
  dentro de un solo bloque basico** y sin `mfcr`. O sea que el modelo "un bloque
  -> `cr0`" **no lo explica**: ahi hay una cuarta causa sin identificar, y es la
  pista mas barata que queda (esta al 100% en nuestro arbol: se puede volcar el
  RTL con `cc1plus -dl` y leerla).
  **NO HAY CUARTA CAUSA: ese aviso era un error de lectura.** El asm FINAL de la
  causa #3 es *exactamente* eso -un `cmpw cr7` pegado a su unico `bXX cr7`-
  porque el segundo salto ya lo borro `jump_optimize`. Volcado el RTL de
  `RemovePortMessage` (`cc1plus -dl/-dg/-dJ` sobre el `.i`), el `(reg:CC 284)`
  tiene **un `set` en el bloque 39 y DOS `jump_insn`**, en los bloques 39 y 42:
  en `.lreg` los dos existen y el pseudo cruza bloques -> `global_alloc` -> cr7;
  en `.greg` siguen los dos con `cr7` ya asignado; y en `.jump2` el segundo ha
  desaparecido. Es la causa #3, literal.

- **LA VEDA DEL `cr7` ESTA ABIERTA: `if (c) X; else X;` EN UN BLOQUE POSTERIOR.
  +10.392 B EN `FnDeltaQ` Y +2.124 B EN `FnDeltaQFast`.** La forma que hace vivir
  el segundo salto hasta CSE y morir en el `jump_optimize` FINAL es **duplicar la
  comparacion como un `if/else` de DOS RAMAS IDENTICAS colocado en un bloque
  POSTERIOR al de la primera comparacion** -no en la sentencia que la contiene-.
  En `FnDeltaQ::EvalSQT` la primera comparacion es
  `bool preventReverse = floorKey < mPrevKey && !IsReverseDeltaSumEnabled();`
  y la segunda va en la rama `else` del `if` que la consume:

      } else {
          int mask = binLenModMask;
          if (floorKey < mPrevKey) { prevDeltaIdx = mPrevKey & mask; }
          else                     { prevDeltaIdx = mPrevKey & mask; }
      }

  **El mecanismo, y por que las quince formas anteriores fallaban.** El primer
  `jump_optimize` de GCC 2.9 corre **sin cross-jumping**, asi que las dos ramas
  identicas sobreviven como dos bloques; CSE (`-fcse-follow-jumps`/
  `-fcse-skip-blocks`) funde la comparacion con la de arriba y el pseudo CC pasa
  a referenciarse en dos bloques -> `global_alloc` -> `cr7`; y el
  `jump_optimize` **de despues de reload**, que SI cross-jumpea, funde las dos
  ramas y borra el salto. Las quince formas medidas antes fallaban por sitio, no
  por forma: dentro de la propia sentencia (`(preventReverse && c)`, ultimo
  termino del `||`, `if (c) x=0; else x=0;` junto a la declaracion) el front end
  pliega o el destino no coincide con la caida.
  - **`if (c) X; else X;` SI; `X; if (c) X;` NO** (esta segunda la pliega antes
    de CSE). El ternario `c ? X : X` tampoco: lo pliega el front end.
  - **Si sobra 1 instruccion, es presion de registros, y se quita con una copia
    local del invariante**: `int mask = binLenModMask;` delante del `if/else`
    convirtio 1.246 -> 1.245 instrucciones (`srw 0,0,9; mr 8,0` -> `srw 8,0,9`).
    En `FnDeltaQFast` no hace falta y ademas **estorba** (+3 instrucciones): la
    forma buena alli es el `if/else` pelado.
  - Da igual escribirlo como `} else if (c) { X } else { X }` o como `if/else`
    anidado, y da igual la polaridad (`<` o `>=`): las cuatro miden identico.
  - Resultado medido: `FnDeltaQ::EvalSQT` (4.980 B) y `EvalSQTMasked` (5.412 B)
    al **100%**, y `FnDeltaQFast::EvalSQTMask` (2.124 B) al **100%**.
    `FnDeltaQFast::EvalSQT` se queda en 98,41% porque ademas le sobra un allocno
    (`addi rN,r1,8` = `&deltaf`), pero su `cmpw cr7` ya casa.
  - **Barrido: no queda ni un `cr7` descuadrado en zEagl4Anim ni en zDynamics.**
    Se comprueba en un minuto contando `cr7` por funcion en los dos lados del
    JSON de objdiff.
  - **ACOTACION: LA RECETA NO APLICA SI LOS DOS LADOS YA EMITEN EL MISMO CAMPO
    DE CR.** `Smackable::Smackable` estaba catalogada como caso de la causa #3
    ("el `cmpwi cr2` va 445 instrucciones por delante de su `beq cr2`") y **no
    lo es**: nuestro lado emite tambien `cmpwi cr2` y su `beq cr2` en el mismo
    indice; el CC ya cruza bloques en los dos. Lo unico que separa la funcion
    del 100% es que el objetivo guarda el parametro `simple_physics` en **r17**
    (`mr r17,r9` en el prologo, muerto en el `cmpwi`, y r17 reutilizado 10
    instrucciones despues) y nosotros lo **derramamos a `0xd4(r1)`**: un
    allocno de mas, marco 0x158 contra 0x150, y los otros 64 diffs son el
    arrastre. Probadas **cuatro formas mas** de la receta (`if (c) X; else X;`
    sobre la sentencia viva de al lado, la misma detras de `smack_class`, el
    contador muerto `if (!c) n++;`, y subir el calculo de `active` a la cabecera
    del cuerpo): las cuatro dan **el mismo binario** que la base. Con las 28 de
    la ronda anterior van **32 variantes**. **Antes de gastar la receta,
    cuenta los `cmpw*`/`b* crN` de los dos lados: si coinciden, el problema no
    es el campo de CR.**

### 6.4 El banco de un solo `.cpp` (1-3 s por variante)

- **EL BANCO DE PRUEBAS DE UN SOLO `.cpp` CUESTA 1 SEGUNDO POR VARIANTE Y SALE
  BYTE A BYTE IGUAL QUE LA SourceList.** No es `sprobe.py` (que reusa el `.s`
  viejo) ni `build_direct`+`fndiff` (25 s). Se hace un `.cpp` en el scratchpad
  que `#include`a **los `.cpp` que la SourceList mete ANTES** mas una copia
  editable del que interesa, y se le pasan los cflags EXACTOS de la unidad
  (`build_direct.parse_units()` los da) con `-S`. Verificacion obligatoria:
  el tamano de la funcion en el banco tiene que ser el mismo que en la unidad.
  Ejemplos medidos: `FnDeltaQ.cpp` solo -> 4.980 B, igual que la unidad;
  `FnDeltaQFast.cpp` **solo** da 2.732 B y con `FnDeltaQ.cpp` delante da los
  2.800 B de la unidad (el inline `FastPolarizedQuatBlend` vive alli).
  Con eso, barrer 128 variantes es **dos minutos** en vez de una hora, y ahi si
  compensa generar los subconjuntos por fuerza bruta.
  **Aviso**: si el experimento toca una CABECERA hay que editar la de verdad y
  restaurarla en un `finally` - y **no puede haber otro build en marcha**: le
  revento una tanda del permutador a mitad.

- **AMPLIACION DEL BANCO: COMPILALO A `.o` Y MIDELO CON OBJDIFF, NO CON TEXTO.**
  En vez de `-S` y comparar el `.s`, se compila el banco con `-c -o banco.o` y se
  enfrenta al `.o` del troceador con `objdiff-cli diff -1 obj/<u>.o -2 banco.o
  -c function_reloc_diffs=none -c ppc.calculatePoolRelocations=false --format
  json <simbolo>`. objdiff empareja por NOMBRE, asi que le da igual que el banco
  tenga menos simbolos que la unidad. Sale el **porcentaje real, el tamano y el
  numero de diffs en 2-3 s** —contra 25 s de `build_direct`+`fndiff`— y coincide
  con el de la unidad **al quinto decimal** (verificado en 6 funciones de
  zEagl4Anim y zDynamics). Con eso, un barrido de 13 variantes cuesta 25 s y deja
  de hacer falta el score de texto del permutador, que es ruido.
  **Y la cabecera se barre sin tocar el arbol**: se escribe una copia modificada
  en el scratchpad y se sustituye la linea `#include "x.h"` de la copia editable
  del `.cpp`. Cero riesgo para los demas agentes.
  **Segunda acotacion, medida**: para muchos `.cpp` el banco **no necesita ningun
  `.cpp` anterior** (`RawStateChan.cpp`, `eagl4supportdlopen.cpp`,
  `FnStatelessQ.cpp`, `FnStatelessF3.cpp`, `FnRunBlender.cpp`, `Articulation.cpp`
  dan los tamanos exactos ellos solos, 2-3 s); otros si
  (`MemoryPoolManager.cpp` no compila sin los suyos). La regla sigue siendo la
  misma: **si el tamano no cuadra con el de la unidad, te falta un `.cpp`
  anterior**; empieza por cero y sube.

- **EL LOCAL QUE EL DWARF NO TIENE NO SE QUITA ENTERO: SE QUITA A MEDIAS.**
  Septima confirmacion de que borrarlo cuesta 12 B (`FnStatelessF3::EvalSQT`:
  1.460 -> 1.472 con el objetivo en 1.468), pero **sustituir SOLO ALGUNAS de sus
  apariciones por la expresion original da el tamano exacto**. En
  `FnStatelessF3::EvalSQT` el local sobrante es `unsigned short *times =
  statelessF3->mTimes;` con tres usos; sustituir **solo el de la condicion del
  primer `while`** por `statelessF3->mTimes[...]` deja 1.468/1.468. Cada
  sustitucion vale +4 B (una copia `mr` que el objetivo tiene y nosotros no:
  es `-fmove-all-movables` izando el invariante al preheader). El barrido de los
  2^N subconjuntos con el banco de un solo `.cpp` es de un minuto.
  **Y no es universal**: en `FnPoseAnim::EvalPose` (mismo patron, 7 usos, faltan
  3 copias) el subconjunto que da el tamano exacto mete recargas de `mTimes` que
  el objetivo no tiene y el porcentaje BAJA de 98,82% a 96,15%. Hay que mirar el
  diff, no solo el tamano.

### 6.5 Vedas y techos medidos (con el porcentaje del 2-sep)

- **VEDA ROTA (auditoria 2-sep), Y LA MAS INSTRUCTIVA:
  `Articulation::Constraint::React` (936 B) CASA HOY AL 100%.** Estaba aqui como
  «la direccion del cross-jumping de una cola `mtlr r0; blrl` no cede al `return`
  interior: ocho combinaciones medidas» — y la palanca **no era el `return`, era
  la ESTRUCTURA**: los dos `if` con un `return true` cada uno **no son dos
  `if`**; el original es `if / else if / else` con **UN SOLO `return`**, y la
  etiqueta de junta que queda detras de la segunda rama es lo que deja al
  cross-jumping fundir `mtlr; blrl` **a traves de la llamada indirecta** en la
  direccion correcta. De 24 combinaciones de donde poner los `return`, cuatro
  dan el tamano exacto pero funden **al reves**. (Y el `__asm__("")` **no hace
  nada aqui**: +8 B, ni un byte de mejora.)
  **Regla que deja: cuando la direccion del cross-jumping no cede al `return`,
  el siguiente candidato es CUANTOS `return` hay, no donde estan.**
  El barrido que se hizo antes se conserva porque describe bien el paisaje —
  99,12%, 3 diffs, `mtlr r0` + `blrl` de mas (8 B), el objetivo fundiendo la
  cola de la PRIMERA llamada virtual sobre la SEGUNDA (`b` hacia **adelante**, a
  mitad del despacho de la otra) y nosotros al reves. Las 8 combinaciones de
  `return true;` interior en los tres bloques que acaban en esa cola:

  | b1 | b2 | React | tamaño | diffs |
  |---|---|---|---|---|
  | 0 | 0 | 1 | 944 (+8) | **3** |
  | 1 | 0 | 0 | **936 exacto** | 8 |
  | 0 | 1 | 1 | **936 exacto** | 8 |
  | 1 | 1 | 0 | **936 exacto** | 10 |
  | 0 | 1 | 0 | 924 (-12) | 6 |
  | 1 | 0 | 1 | 924 (-12) | 6 |
  | 0 | 0 | 0 | 916 (-20) | 7 |
  | 1 | 1 | 1 | 916 (-20) | 11 |

  **Con TRES `return` el tamano exacto se alcanza pero siempre con la copia
  equivocada sobreviviendo** — y ese era el sintoma de que sobraba un `return`,
  no de que faltara una palanca. Descartadas ademas 7 formas mas (guarda
  invertida del `if (av.y*deltaA<0)` en cada bloque y en los dos, bloques
  intercambiados, y el puntero de la entidad ligado a un local antes de la
  virtual en cada bloque y en los dos): las tres del puntero dan **binario
  identico**, las guardas bajan a 10-19 diffs, e intercambiar los bloques se va
  a 99 diffs.
  **Y la «contradiccion con la regla 2 de §6» que esta entrada denunciaba queda
  RESUELTA**: el `.s` del objetivo tiene su etiqueta compartida `.L_80089114` en
  la cola del bloque SEGUNDO y el primero salta hacia adelante hasta ella, con
  el epilogo detras de las dos — que es lo que sale **cuando hay un solo
  `return`**, no una excepcion a la regla. **No queda ninguna «segunda variable
  sin identificar» aqui.**

- **TECHOS MEDIDOS DE zEagl4Anim EN ESTA RONDA (no los reintentes por la via
  que dice cada uno).**
  - **`FnRawStateChan::FindTime`** (232 B, 99,57%, **4 diffs, tamano exacto,
    cero opcodes de diferencia**): el producto `i * GetKeySize()` se lleva `r0`
    en el nuestro y `r9` en el objetivo, y con el los tres usos siguientes.
    Barridas **12 formas de fuente** (keyTime inlinado, `startTime < keyTime`,
    `i` sin signo, `kMaxSize` como `int`, array con literal, inicializador
    muerto, `while` en vez de `for`, `continue` negado, orden de declaracion,
    `state` como `char`) y **6 de la cabecera** (`&keyData[...]`, `mKeySize`
    directo, offset en local, `GetKeySize()` devolviendo `int`, `keyData +=`,
    el tamano leido antes del `if`): **las 18 dan el mismo binario**. Es
    `local_alloc` puro dentro de un solo bloque; `lreg.py` no lo cubre porque
    solo lista allocnos de `global_alloc`.
  - **`FnRunBlender::FindMatchTime`** (720 B, 97,36%, 8 diffs, tamano exacto):
    el `lis`/`lfd` del sesgo `0x4330000080000000` (la conversion int->float de
    `n`) lo emite el objetivo **antes del primer `bso`** y nosotros dentro del
    bloque de la segunda condicion del `&&`. Ocho formas (condicion invertida,
    `if` anidado, `float fn = n` delante, cast explicito, `searchLength` en
    local, `0.0f` en local, y dos ordenes de las sentencias de arriba): las
    cinco que no empeoran dan **binario identico**.
  - **`FnStatelessF3::EvalSQT`** (1.468 B, 99,35%, 10 diffs, **-8 B**): lo unico
    que falta son **dos `mr r11, r8`** en los preheaders de los dos `while` (una
    copia del puntero `mTimes`). **No salen del local `times`**: el volcado DWARF
    dice que el original **no tiene ese local** en el bloque, y se han medido los
    **8 subconjuntos** de sustituirlo por `statelessF3->mTimes[...]` (cada
    sustitucion mete un `lwz` DENTRO del bucle, no la copia: +4/+8/+12 B y de 10
    a 39-42 diffs) y **7 colocaciones** del local (arriba del bloque, `const`,
    una copia `t2` por rama, uno por rama desde el miembro, puntero movil,
    `do/while`): las siete dan **el mismo binario**. GCC coalesce la copia
    siempre.
  - **`FnStatelessQ::EvalSQT`** (1.480 B, 95,39%, 139 diffs, tamano exacto):
    escribir los DOS `if (floorKey >= statelessQ->mNumKeys - 1)` como
    `floorKey >= (unsigned short)(statelessQ->mNumKeys - 1)` sube a **96,79% con
    126 diffs y el tamano intacto** -mueve el `clrlwi` de delante del `subi` a
    detras, pero no lo quita-. **NO se aplico**: son 0 bytes y ademas cambia la
    semantica si `mNumKeys` vale 0. Lo que queda de verdad es que el objetivo
    salva **5** registros (r27-r31, marco 0x60) y nosotros **6** (r26-r31, marco
    0x58) porque `dataBuf` se nos va a un salvado (r27) donde el objetivo lo
    tiene en `r12` -el registro que el propio DWARF le asigna-. 40 variantes
    medidas entre las dos sesiones.
  - **`DynamicLoader::Initialize`** (2.352 B, 79,66% tras arreglar `ELFAddr`;
    **al 2-sep, 80,40% — sigue abierta**):
    el nodo `case SHT_LOPROC + 5 ... SHT_LOPROC + 6` **nunca emite sus dos
    comparaciones** y al objetivo le sobran 9 instrucciones del arbol del
    `switch`. Descartado con medida: quitar el `default`, separar el rango en
    dos `case`, poner el `default` primero, `case 8` suelto, y **`continue` en
    vez de `break`** en cualquiera de los tres (las seis dan **binario
    identico**); y un micro-banco de **20 conjuntos de `case`** (anadiendo 0,
    0..1, 4..7, 10, 11, 10..11, HIPROC, LOUSER..HIUSER, LOPROC completo)
    tampoco reproduce la secuencia del objetivo. Lo que si mueve el arbol es dar
    **cuerpo no vacio** al rango LOPROC: entonces sus dos `cmplw` aparecen, pero
    **detras** del `cmpwi 9` y no delante. La causa de fondo es otra: el
    objetivo mantiene `pHP` en **r31** y derrama `e` a pila, y nosotros al reves
    -eso son 8 B de marco (0xf0 contra 0xe8) y una ranura de pila de mas-.
    Se llega a **tamano exacto (2.352/2.352, 417 diffs)** leyendo `h.e->e_shnum`
    en el primer `for` y en el `shstrndx` y `e->e_shnum` en el bucle de
    simbolos, pero es un truco para bajarle las referencias a `e`, no la forma
    del original, y no puntua: **no se aplico**.
  - **`Collision::Moment::React` — VEDA ROTA (auditoria 2-sep): LAS CUATRO
    SOBRECARGAS CASAN HOY AL 100%** (5.668 + 4.384 + 2.588 + 2.108 B). Esta era
    la veda mas citada del manual («8 variantes a mano + ~1.650 del permutador,
    cero mejora»), y cayo con **cambios de FUENTE**, no de reparto:
    1. **quitar el idiom del temporal explicito** (`__typeof__(x) t = expr;
       x = t;`), que cuesta una copia entera de 6 instrucciones — eso solo llevo
       la sobrecarga `(const Plane&, int)` de 97,62 a **100%**;
    2. **quitar dos inicializadores muertos** (`bool x = false;`), que borraron
       **58 de 117 lineas de diff** sin mover una instruccion y dejaron el
       tamano exacto — la rotacion del pool de derrames era su sombra.
    **El barrido que se hizo antes sigue siendo cierto y por eso se conserva**:
    con la funcion a 99,80% y tamano exacto, de sus 66 diffs solo DOS eran
    estructurales (al objetivo le sobra `mr r11, r0`, copia de
    `o0->IsImmobile()`, y a nosotros `lwz r0, 0xc8(r31)`, relectura de
    `o1->IsImmobile()` que el objetivo CSE-a contra `r9`); las **64
    combinaciones** de los seis usos de `IsImmobile()` entre local y llamada
    directa mejoran a 99,80029% y ninguna quita el par, y las **7 formas de la
    copia local** dan **el MISMO binario** (5.660 B, 76 diffs), o sea que la
    receta de «`thread_jumps` se rompe con una copia local» **no aplica ahi**.
    **La leccion: 71 formas medidas sobre el ARREGLO EQUIVOCADO no son una
    veda.** El par de diffs era arrastre de dos sentencias que sobraban 400
    instrucciones antes.
  - **`MemoryPoolManager::NewBlockAux`** (84 B, 88,57%, 8 diffs, tamano exacto):
    dos cosas y solo se consigue una a la vez. Escribir
    `gMemoryPoolFree = gMemoryPoolFree + (idx + 1) * 0x10 + 4;` **sin
    parentesis** (en vez de `+=`) pone el `stw` del global **detras** del
    `addi r3` del valor devuelto, como el objetivo (88,57 -> 89,05%, 7 diffs);
    pero entonces el `+4` se agrupa con el puntero (`add`; `addi 4`) en vez de
    con el tamano, y el objetivo hace lo contrario. Con parentesis, con un local
    para el avance o con `+=` se agrupa bien y el `stw` se adelanta. **19 formas
    medidas**, ninguna consigue las dos.

## 7. Aritmética y comparaciones

- **EL PLEGADO `a * K * b` -> `a * (b * K)` DISPARA PORQUE `-ffast-math` ESTA EN
  LOS CFLAGS** (confirmado: con `-fno-fast-math` la funcion cambia). Asi que la
  cadena del objetivo se reproduce **poniendo los parentesis en la fuente**:
  `(90-af) * (sf * K) * K2` da la cadena del objetivo instruccion a instruccion
  donde `(90-af) * K * sf * K2` no.


- **COMPARAR `unsigned int` CON UN CAMPO `long long` PROMOCIONA A 64 BITS:** en
  nuestro lado sobran `li rX,0 / cmpw / bgt / bne` alrededor de un `cmplw`. El
  original **trunca**: `(unsigned int)amount`. 404 B con **dos casts**.
- **`pass = true;` SOBRE UN `short` SE PLIEGA SIEMPRE A `li rX,1`; `pass++` DA EL
  `li` MAS EL `extsh`.** El `extsh` solo sale si el RHS es una **expresion
  entera** que cprop reduce a 1 **despues** de haber generado la conversion
  int->short. Cerro 2.236 B, y era el diagnostico que llevaba una ronda abierto.
- **`xoris` ANTES DEL `stw` = CONVERSION A FLOAT *CON* SIGNO; SIN `xoris` = SIN
  SIGNO** (las magicas `0x4330000080000000` y `0x4330000000000000`). Un solo
  `xoris` de menos en `(float)campo` costo **6 puntos y 8 B**. Se fuerza con
  `static_cast<float>(static_cast<int>(campo))`.
- **`*++p` DA `lhau`/`lhzu`; `p++; x = *p;` DA `addi`+`lha`.**
- **`((x>>n)&C1) | ((x>>n)&C2)` SE PLIEGA A UN SOLO AND; `((x&C1)>>n) |
  ((x&C2)>>n)` NO.** El remapeo de botones del original tiene diez terminos y
  nosotros plegabamos cuatro en `(x>>8)&0xF`. Con la forma **mascara y luego
  desplazamiento** volvieron los 6 `or` y 6 `rlwinm` que faltaban: 82,89 ->
  **87,66%** y el tamano paso a cuadrar.
- **UN `extsh` DETRAS DE UN `li rX,1` DICE QUE EL RHS DEL ORIGINAL NO ERA UNA
  CONSTANTE LITERAL.** Con `short pass; pass = true;` GCC pliega a `li r24,1`;
  para que emita el `sign_extend` la asignacion tiene que venir de una expresion.
- **`(x >> 16) & 1` NO ES `(x & 0x10000)`:** el desplazamiento **estrecha la carga
  a `lhz`** (la mitad alta en big-endian).
- **LA ARITMETICA DEL OBJETIVO ES LA DEL FUENTE AUNQUE SEA REDUNDANTE:**
  `(index/5 - 1)*5 + 3`, no `page - 2`.
- **CUANDO `fold` REASOCIA `a * (b * K)` Y NINGUN PARENTESIS LO IMPIDE, EL DIAL ES
  `extern const float lbl_XXXX`.** Cinco asociaciones fallaron (izquierda,
  constante delante, plana, temporal con nombre, `const float` local) y
  `extern const float k __asm__("lbl_803D27F4")` cerro la funcion: 2.060 B.
- **`UMath::Clamp(a,min,max)` NO ES `Max(min, Min(a,max))`.** `Clamp` es un inline:
  sus argumentos se evaluan **antes** del cuerpo, asi que el `lfs` de `amin` sale
  **delante** de las ramas del `Min`. Anidado, la constante se carga en el punto de
  uso, en el bloque de la junta. **Delator: un `lfs` de pool que el objetivo pone
  antes de la comparacion y nosotros despues.** 2.204 B.
- **`if (int x = f())` da `beq`; `int x = f(); if (x > 0)` da `ble`.**
- **`subfic`/`adde` EN EL OBJETIVO DELATA LA POLARIDAD, no solo la forma sin rama.**
  El bool sin salto probo que nuestro `!= 0` estaba **invertido**, y el mismo diff
  destapo una segunda negacion equivocada en la misma funcion.
- **`static_cast<float>(unsigned char)` EMITE EL SESGO SIN SIGNO
  `0x4330000000000000`; dejar que actue la PROMOCION ENTERA (`float_expr *
  unsigned_char`) emite el CON signo `0x4330000080000000` + `xoris`, que es lo que
  tiene el original.** Y **objdiff PUNTUA MAS ALTO LA VERSION ROTA**: 90,2886% con
  el sesgo malo contra **89,3691% con el bueno**. Casi un punto de premio por el
  error. **Solo `litpos` lo vio.** Es el caso mas claro de que el porcentaje no es
  el juez cuando hay constantes de por medio.
- **`(x >= 0) << 1` DA EL ATAJO DE DESPLAZAMIENTO** (`nor`+`rlwinm`, 2
  instrucciones); **el ternario sobre el bit de signo enmascarado da la via del
  CR** (`cmpwi cr7`+`cror`+`mfcr`+`clrlslwi`, 4). Ocho formas barridas (`?:`,
  `2*(x>=0)`, suma, `bool`, `(x<0)?0:2`, `if`, `!(x<0)`): **solo
  `(x & 0x80000000) ? 0 : 2`** da la del objetivo.
- **CALCULA CON LAS VARIABLES SIN MUTAR, ANTES DEL `-=` EN SITIO.** Las **16
  permutaciones** del orden de seis `-=` dan todas >=2 diffs; lo que cierra es
  mover el calculo **delante** y dejar las restas detras:
  `float a = pow2(x2-x1) + pow2(y2-y1);` y **luego** `y2 -= y1; x2 -= x1;`. CSE
  comparte la resta y el planificador deja de rellenar el hueco de latencia.
  Delator: **dos `fsubs` independientes transpuestos** con todo lo demas igual.
- **`-(a-b)` CONTRA `b-a` ES UN DIAL POR ARGUMENTO.** En un ctor de dos
  argumentos, negar **los dos** bajo de 12 a 6 diffs y negar **solo el segundo**
  cerro a 0: cada mitad elige por separado, y **la que ya casa hay que dejarla**.
- **EL `crclr cr1eq` DECIDE EL PROTOTIPO, PERO LA EXTENSION DE SIGNO DEL ARGUMENTO
  LA DECIDE EL CAST.** Tres llamadas `isspace/isdigit(*p)` **sin** `extsb` aunque
  `p` sea `char *` y el llamado no tenga prototipo: el fuente es
  `(unsigned char)*p`. 98,52% -> 100%.
- **EL FRONTEND USA `|`, NO `||`.** Toda la familia `UnlockSystem` estaba
  transcrita con cortocircuito. `answer = answer | X;` sobre un `bool` **distribuye
  el `!= 0` en los dos operandos**, y **`answer |= (cond);` es la forma corta del
  idiom de 12 lineas** que ya estaba escrito en el arbol. No son intercambiables:
  **1.960 B en dos compilaciones.**
- **`i <= 6` e `i < 7` DAN BINARIO IDENTICO** (6 formas en micro-banco): un `blt 7`
  del objetivo **no sale de la forma de la condicion**, hay que buscarlo en otro
  sitio.
- **`(((i >> 31) + i) >> 1)` NO ES `i / 2`.** La cadena escrita a mano emite
  `srawi rX,rY,31` (aritmetico); la expansion propia de GCC para `i / 2` emite
  **`srwi rX,rY,31`** + `add` + `srawi ...,1`. **Un `srwi ...,31` encabezando ese
  trio es la firma de `/ 2` escrito como division**, y delata que un descompilador
  escribio la cadena a mano. *(Barrido el arbol entero: cero casos mas.)*
- **`fold` REASOCIA `(y * K) * b` EN `y * (K * b)`; solo un temporal CON NOMBRE lo
  impide.** `y + y / 255.0f * rot` da `y*(rot*K)+y`; el original tiene
  `(y*K)*rot+y`. `float rot_scale = y / 255.0f;` lo reproduce exacto.
- **CON DOS CONVERSIONES int->float ALIMENTANDO UN `fmadds`, EL OPERANDO CONVERTIDO
  PRIMERO ES EL QUE SE ESCRIBE PRIMERO** — aunque sea el sumando.
  `color_base + red * color_mul` gano a `red * color_mul + color_base`; el delator
  es el orden de los dos `xoris ...,0x8000`.
- **`mfcr` + `extrwi` DONDE EL OBJETIVO TIENE `li 1 / bXX / li 0` ES UN `if/else`
  EXPLICITO**, no `x = (a < b)`. GCC funde los dos stores. 96,65 -> 99,85%.
- **`_SimRandom_FloatRange(K)` SE LEE DE DOS CONSTANTES DEL POOL** (el rango y
  `1/65536`): asi salio que `FloatRange(15)+15` era `FloatRange(4)+8` y que
  `FloatRange(1)` era `FloatRange(100)`.
- **UN BUCLE DE CTORES `li rN, 7` CON `cmpwi` ANTES DEL `subi` EJECUTA 8 VECES**,
  no 7. Cuenta el orden, no el inmediato.
- **`MACRO(x * k)` CONTRA `(x * k) * C` NO SON LO MISMO CON `-ffast-math`.** `fold`
  reasocia la forma larga y **funde los dos `fmuls`**; la llamada al inline los
  conserva. **484 B** — y la funcion hermana que ya casaba tenia la forma buena
  escrita en el mismo fichero.
- **DOS LOCALES EVITAN LA REASOCIACION DE `(a<b) + 1`.**
  `unsigned branch = (t[i] < key) + 1;` da salto (`li 1`/`bge`/`li 2`);
  `unsigned less = t[i] < key; unsigned branch = less + 1;` da la forma sin salto
  `subfc`/`subfe`/`subfic` del objetivo. **10 variantes probadas, solo esa.**
- **`fold_truthop` FUNDE DOS COMPARACIONES DE MIEMBROS ADYACENTES EN UNA CARGA
  ANCHA CON MASCARA.** `lwz r0,0x10(rX)` + `and. rY,r0,0x10FFFF` sobre
  `uint8 A(0x10); uint8 B(0x11); uint16 C(0x12)` **no es un campo de 32 bits ni un
  bitfield**: es `(B & 0x10) == 0 && C == 0`. **La mascara se lee por bytes en
  big-endian** (byte 0x11 -> bits 16-23; 0x12-0x13 -> bits 0-15). Recupera la
  condicion exacta sin adivinar.
- **`li rN,-0x1` contra `li rN,0xff` delata COMO llega el 255 a un campo
  `unsigned char`**: mismo byte almacenado, distinto inmediato. Una constante
  escrita en el sitio se pliega a QImode (`-1`); un valor que viene de una `int`
  local se queda en SImode (`0xff`). Micro-banco de 5 formas.
- **TECHO: `bMax(const, const)` no se puede desplegar.** El objetivo emite la rama
  sin plegar y nosotros plegamos siempre: **8 formas** (literal, `int size = ...`,
  cast, y **4 cuerpos distintos de `bMax`**) pliegan las ocho. Pero el dial existe
  y no esta en el cuerpo: `_c(a)` con `int a = 255` **si** conserva la rama.
- **`x = expr; x = Max(K, x);` CONTRA `x = Max(expr, K)`**: el orden del pool
  prueba el orden del fuente y el operando **por defecto** del max prueba cual es
  el segundo argumento. 91,86% -> 100%.
- **Polaridad `>` contra `<=` en la seleccion de dos tablas**: `if (a > b) {A}
  else {B}` da `cror un,eq,lt; bso <else>` con A en la caida; tanto
  `if (a<=b){B}else{A}` como el ternario dan `bgt`.
- **Duplicar la llamada en cada rama en vez de seleccionar la tabla y llamar una
  vez**: el cross-jumping funde `bl`+`fmuls` y deja duplicado el `fmr` del
  argumento, **y ademas impide el `fmadds`** porque producto y suma acaban en
  bloques basicos distintos. +3,7 puntos.
- **`x = a*K; x += C;` SOBRE UNA LOCAL SE FUNDE EN `fmadds`; SOBRE UN MIEMBRO
  NO.** El store a memoria intermedio separa el producto de la suma, que es lo que
  da el `fmuls`+`fadds` del original. **340 B a la primera.**
- **Ligar el multiplicador a una local `int` da `mulli` en vez del desplazamiento
  sintetizado.** `Vol * 0x7FFF` sintetiza `slwi 15; subf` (mas barato que el coste
  3 de MULT en rs6000); `int scale = 0x7FFF; Vol * scale` llega a *expand* como
  no-constante y *combine* la pliega despues en `mulli`.
- **`Clamp` SOBRE LA MISMA VARIABLE mete DOS `fmr`** que la variable nueva no
  mete. Ni el temporal explicito ni partirlo en `Min`/`Max` los reproducen.
- **`VU0_floatmin(a,b)` es `if (a<b) a else b`**: el `fcmpu` lee los argumentos en
  orden y el valor por defecto es el **segundo**. Con eso se decide entre
  `Max(lo, Min(hi, x))` y `Clamp(x, lo, hi)` sin compilar.
- **`b == true` conserva el `cmpwi 1` y ROMPE la CSE** con un `if (!b)` posterior;
  `if (b && ...)` la comparte. Delator: al objetivo le sobra un `cmpwi rX, 1`.
- **AMPLIACION de la entrada de `!bool`: se arregla EN EL SITIO DE LLAMADA.**
  `x == false`, `!(int)x`, una local `int` y el ternario valen los cuatro; **no
  hace falta retipar la virtual**.
- **`cmpwi` contra `cmplwi` da el SIGNO del global**: dos funciones cerraron solo
  cambiando `u32` por `int` en el `extern`.
- **`!bool` da `xori rN,rN,1` SOLO si el operando es de precision 1.** Si el
  objetivo tiene la secuencia generica `subfic rD,rA,0` + `adde`, la fuente **no**
  es un `!x` sobre un `bool`: es `(x == false)`, `!(int)x`, o un `int` intermedio.
  Micro-banco de 8 formas: `!a` es la unica que da el `xori`, las otras siete dan
  la generica. 4 B en `SerializeTable`.
- **`(unsigned)(x-2) > 3` contra `(x-2) > 3`**: el sin signo sale **sin salto**
  (`subfic`/`subfe`/`neg`); el con signo **materializa el CC** (`cmpwi cr7` +
  `mfcr`). **REVISADO Y ACOTADO**: se creyo que era la via para el `cr7` de la
  familia `FnDeltaQ` de zEagl4Anim y **no lo es**. Ese caso es OTRO: alli el
  objetivo **no** materializa (no hay `mfcr`) y emite `cmpw` **con signo** mas su
  `bge`. La palanca del tipo solo aplica cuando el objetivo **no tiene salto**;
  en cuanto el objetivo emite `cmpw` ya te esta diciendo que los dos operandos
  son `int` con signo, porque cualquier tipo sin signo sale `cmplw` y ni siquiera
  es el mismo mnemonico. **El mnemonico del objetivo cierra la pregunta del tipo
  sin compilar nada.**
- **`bAdd(dest, v1, v2)`: el operando escrito primero se carga primero** — la
  misma regla que `b - a` contra `-a + b`, confirmada en el helper de 3
  argumentos. Y **`bCopy`/`bFill` agrupan las cargas antes de los stores**
  mientras tres asignaciones campo a campo los intercalan.
- **`x |= (cond)` contra `x = (cond) || x`** sobre un `bool`: el `|=` da
  `ori rX,rX,1` mas la normalizacion; el `||` materializa un temporal
  preinicializado a 0. Cerro una funcion de 356 B — **pero no es global**:
  aplicarlo a otra empeoro y hubo que revertirlo.
- **LA CONVERSION ENTERO->FLOAT SIN SIGNO NECESITA UN CAST EXPLICITO A `unsigned
  int`, y ya ha pagado dos veces.** Un `unsigned char` o `unsigned short`
  promociona a `int` y GCC emite la secuencia **con** signo (`xoris rX,rX,0x8000`
  mas el sesgo `0x4330000080000000`); el objetivo usa la sin signo
  (`0x4330000000000000`, sin `xoris`). Un cast llevo una funcion de 93,31% a
  99,99% y otra **de 106 a 63 diffs de un golpe**. Delator: un `xoris` de mas y
  el sesgo equivocado en el `lfd`.
- **`if (x) v = 0;` contra `v &= (x == 0);`**: un `clrlwi rN,rN,31` (AND con 1)
  en el objetivo solo sale de la segunda forma.
- **El orden de operandos de una comparacion flotante se lee del `cror`**:
  `cror un,eq,lt` es `a > b` y `cror un,eq,gt` es `a < b`. `x > f()` **no** es lo
  mismo que `f() < x`.
- **En flotante, el operando escrito PRIMERO nace antes como pseudo y se lleva el
  registro bajo.** Si el diff es solo un par de `lfs` con los registros
  **intercambiados** alimentando un `fsubs` —misma cuenta, mismos opcodes, mismo
  orden de cargas— reescribe `b - a` como `-a + b`. Medido que `-=`, `-(a-b)`,
  `= x + (...)` y el temporal explicito NO valen. **Y el delator suele estar en
  la sentencia hermana que ya casa: las lineas vecinas dictan el estilo del
  original.**
- **`b - a` contra `-a + b`: MISMO codigo, REGISTRO FLOTANTE DISTINTO.** El
  operando escrito **primero** se evalua primero, nace antes como pseudo y se
  lleva el registro bajo (`f0`); el segundo se lleva el alto. Si el diff es solo
  un par de `lfs` con los registros **intercambiados** alimentando un `fsubs`
  (misma cuenta de instrucciones, mismos opcodes, mismo orden de cargas),
  reescribe `b - a` como `-a + b`. Cerro
  `CBasicCharacterAnimEntity::RenderEffects` de 99,87% a 100% (**948 B**) con
  **un solo caracter movido**: `right0.x += perpendicular.x - parallel.x` era
  `right0.x += -parallel.x + perpendicular.x`. El delator estaba en la linea
  vecina: `right1.x += -parallel.x - perpendicular.x` ya casaba, o sea que el
  autor escribia `-parallel` primero en TODO el grupo. **Mira siempre las
  sentencias hermanas que ya casan: dictan el estilo del original.**
  Las otras formas NO valen (medidas): `right0.x -= parallel.x -
  perpendicular.x` y `+= -(parallel.x - perpendicular.x)` bajan a 99,63%;
  `= right0.x + (...)` y el temporal explicito no cambian nada.
- **`cmpw cr7` (entero) = el valor CC cruza un bloque basico**; `cmpw` sin campo
  (cr0) = comparacion fundida con su salto en el mismo bloque. **El mecanismo,
  confirmado con el volcado RTL (`cc1plus -dl`)**: el pseudo CC nace con
  `pref CR0_REGS`, asi que si `local_alloc` lo coge -y lo coge siempre que este
  referenciado en UN solo bloque- sale `cr0`, sin excepcion. Solo si el pseudo
  se referencia en **mas de un bloque** pasa a `global_alloc`, que al no tener
  `cr0` libre en todo el rango cae a la clase alternativa y reparte `cr7`
  primero. **Las tres formas de conseguirlo**, todas verificadas en el binario
  del objetivo:
  1. **comparacion invariante de bucle izada** al preheader, con el salto dentro
     del bucle (`WRoadNav::IsSegmentInCookieTrail`, `WRoadNetwork::ResetBarriers`);
  2. **materializada como valor** (sale con `mfcr`);
  3. **una comparacion con DOS saltos** en bloques distintos: CSE los funde y
     `jump_optimize` borra despues el salto redundante, de modo que en el asm
     final se ve **un solo** `bXX cr7` pegado al `cmpw` y sin `mfcr`
     (`Speech::Cache::TossSample` y `Hermes::System::RemovePortMessage`, los dos
     al 100 % en nuestro arbol; el volcado RTL enseña el `(reg:CC 323)` con un
     `set` y **dos** `jump_insn` en bloques 44 y 47).
  Tambien lo fuerza que un **`cmpwi`/`fcmpu`/`andi.` de por medio pise `cr0`**
  dentro del rango del pseudo (`GRaceStatus::SortCheckPointRankings`,
  `CarCustomizeManager::MaxOutPerformance`).
  **Corolario: NO se arregla reescribiendo la expresion.** En la familia
  `FnDeltaQ`/`FnDeltaQFast` se probaron 35 formas de
  `A && B` (`if`, ternario, `if/else`, bool intermedio, `&` bit a bit, orden
  invertido, `== false`, tipo `int`/`unsigned`, condicion inlineada en el `if`,
  `!(x >= y)`, `(a-b) < 0`, `if` anidado) y **GCC 2.9 las canonicaliza todas al
  mismo RTL**: identico byte a byte, cr0 siempre.
- **LA VIA DEL TIPO NO ABRE EL `cr7`: 11 VARIANTES DE TIPO Y 25 ESTRUCTURALES,
  TODAS `cr0`.** *(Esta entrada decia «la veda sigue en pie» y **eso ya es
  falso**: la veda cayo por otra via —el `if (c) X; else X;` en un bloque
  posterior, §6— y las tres funciones casan al 100%. Lo que sigue siendo cierto,
  y es lo que se conserva, es que **el TIPO no es la palanca**.)* Sobre la
  funcion REAL (no en banco reducido) se barrieron 11 variantes
  de tipo -`unsigned` en los dos operandos, `(a-b)<0`, `(unsigned)(a-b) >
  0x7fffffff`, `short`, `long`, el destino `int`/`unsigned`/`char`, copia a local
  `int`, lectura por `int*`, lectura `volatile`- y 25 estructurales; **todas dan
  `cr0`**. Y hay una prueba que se puede hacer **sin compilar**: el objetivo
  emite `cmpw` (con signo) **con** su salto; cualquier tipo sin signo daria
  `cmplw` y cualquier forma de resta cambia la cuenta de instrucciones, asi que
  el tipo ya esta fijado por el mnemonico del objetivo. **La prueba definitiva
  de que la causa no esta en esta sentencia**: `FnDeltaF1` y `FnDeltaF3` tienen
  la **linea identica** (`bool preventReverse = floorKey < mPrevKey &&
  !IsReverseDeltaSumEnabled();`) y el objetivo les da **`cr0`** en las cuatro,
  todas al 100 %. Ocho sitios, misma sentencia, cuatro `cr0` y cuatro `cr7`: el
  campo lo decide el contexto de la funcion, no la forma de la expresion.
  **Y la familia no son cinco funciones, son dos**: solo `FnDeltaQ::EvalSQT`
  (4.980 B) y `FnDeltaQ::EvalSQTMasked` (5.412 B) fallaban **unicamente** por el
  `cr7` (9 lineas de diff cada una, tamano exacto) — **y las dos estan hoy al
  100%**. `FnDeltaQFast::EvalSQT` fallaba por **marco distinto** (0x78 contra
  0x70) y un registro salvado de mas -eso es una local que falta, no el `cr7`- y
  **sigue abierta al 98,41%**; `FnDeltaQFast::EvalSQTMask` arrastraba ademas una
  permutacion r28/r29 en toda la funcion y **cerro al 100%**; y
  `FnStatelessF3::EvalSQT` es el `mr r11,r8` de la particion de rango de vida y
  **sigue al 99,35%**. **Agrupar por sintoma sin mirar el diff de cada una
  cuesta rondas enteras**, y esta familia lo demuestra en las dos direcciones:
  tres de las cinco cayeron por vias distintas.
- **Barrido completo del juego: solo hay 37 `cmpw`/`cmplw` con campo != cr0 y un
  unico salto**, y de ellos ya reproducimos 12 al 100 %. Es un patron raro; si
  te sale uno, mira primero si el objetivo lo tiene dentro de un bucle o con un
  segundo salto, antes de tocar la expresion.
- **`x * 2` emite `add rD,rS,rS`; el desplazamiento emite `slwi`, y GCC 2.9 no
  los unifica.** Con 48 apariciones en un fichero, cambiarlos subio cuatro
  funciones de 82-94% a 96-97%. (En cambio `3*p1` si sale `add;add`.)
- **`fold` reasocia `a * (b * K)` en `(a * K) * b`; con una suma o un
  desplazamiento en vez del producto interior, no.** Cuando el objetivo dobla el
  operando equivocado, ahi esta el arreglo.
  **Y el arreglo NO son parentesis: es escribir el producto IZQUIERDA-ASOCIADO.**
  `fCurve * (vCameraImpcat.x * 0.25f)` se pliega a `(fCurve*0.25f)*impcat` (el
  objetivo carga `impcat` primero y multiplica por la constante); escribirlo
  `vCameraImpcat.x * 0.25f * fCurve` da el orden de cargas y el `fmuls` del
  objetivo. En `CubicCameraMover::Update` valio 10 lineas de diff en dos sitios y
  ademas **quito la local intermedia** que `regmap` daba como SOLO NUESTRA.
- **`static_cast<int>` contra `static_cast<unsigned int>` de un `double`:** la
  version sin signo emite la secuencia de sesgo 2^31 (`fcmpu`, `cror un,eq,gt`,
  `bso`, `fsub`, `xoris`), unas 9 instrucciones mas. **Una sola letra valio
  1.148 B.** El delator es el tamano, no el porcentaje.
- **Un temporal de bloque bloquea la reasociacion de `(a - C) - b`,** que GCC 2.9
  pliega a `a - (b + C)`. Ni parentesis ni reescribir la expresion sirven.
- **`(S + (T-1))/T` y `(S-1+T)/T` NO son lo mismo**: `fold` mueve la constante al
  otro operando.
- **Un `bool` xor 1 normaliza; la negacion da `xori`.** Y `cmpwi; li 1; bne; li 0`
  es **normalizacion de int a bool**, no una permutacion.
- **Tabla de comparaciones flotantes**: `a<b` → `cror un,eq,gt`+`bso`; `a>b` →
  `cror un,eq,lt`+`bso`; `a>=b` → `blt` pelado; `a<=b` → `bgt` pelado. **Sin
  `cror` la condición es no estricta.** Como salto-**fuera**, `cror un,eq,gt` +
  `bso` es `<` estricto y un `bgt` pelado es `<=`.
- **`fabsf` ≠ `VU0_fabs`**: `fabs f1,f1` suelto es el builtin;
  `fmr`+`fcmpu`+`cror`+`bso`+`fneg` es `VU0_fabs`.
- **`bMath.hpp:230` es `bMax`, `:231` es `bMin`**: tras `fsubs t,a,b`,
  `fsel d,t,b,a` es min.
- **`bMax(int,int)` es correcto; lo que cambia es el ORDEN DE ARGUMENTOS.**
  `bMax(x,1)` da `li b; cmpwi x,1; ble`; `bMax(1,x)` da
  `mr r9,x; cmpwi x,0; bne; li 1`, porque GCC canonicaliza `1>x` a `x==0` cuando
  el `lbz` prueba el rango 0..255.
- **`a = b = c = 0;` encadenado emite los `lis @ha` en orden ESCRITO y los stores
  en orden INVERSO.** Es la única forma de producir esa combinación.
- **`X = expr; bool b = (X == 1);` produce DOS `mr` que GCC no coalesce**, donde
  `bool b = expr; X = b;` no produce ninguno: el store intermedio a memoria
  mantiene vivos los dos pseudos.
- **`u = Max(Min(u,hi),lo)` anidado contra dos sentencias**: la forma en dos
  sentencias mete un par `fmr` de ida y vuelta.
- **`xor` + `subfic rD,rA,0` + `adde` es `==`, no `!=`** (calcula `(a^b)==0`), y
  sólo para igualdad **entera**. **Sin el `xor`, `subfic rD,rA,0` + `adde
  rD,rD,rA` es `== 0`**: GCC 2.9 sólo tiene patrón sin salto para la igualdad
  **con cero**, así que escribir `!= 0` sale con rama y baja la puntuación.
- **`x *= C * y` NO es `x = x * C * y`.** Con `-ffast-math` `fold` reasocia la
  forma larga a `x * (C*y)` —constante y variable multiplicadas primero— y no hay
  paréntesis, cast ni división que lo impida. La asignación compuesta conserva
  `(x*C)*y`. Cinco variantes de paréntesis dieron el mismo 90,77%; `*=` dio 100%.
- **`a || b` da `or.` sólo con los operandos ya materializados a 0/1**: `|` lo
  consigue, dos `bool` locales no.
- **`m = (A||B||C);` emite UN store**; `m=false; if(...) m=true;` emite dos.
- **El orden de operandos de `==` sobrevive al compare**: constante a la
  izquierda da `li`+`cmpw`, a la derecha da `cmpwi`.
- **GCC pliega `A - (i + 1)` en `(A - 1) - i`**: hacen falta **dos** locales para
  evitarlo. **`fold` reasocia `a - (b - 1)` en `(a + 1) - b`.**
- **`2*i + 1 + (id<t)` sale sin salto; `2*i + ((id<t)+1)` sale con salto.**
- **GCC 2.9 canonicaliza `x>=C` a `x>C-1`, Y UN LIMITE SIN CANONICALIZAR TIENE
  TRES ORIGENES POSIBLES — NO SOLO EL `switch`.** (La regla vieja decia «sólo
  puede venir de un `switch`»; es falsa.) Comprobado que **ninguna forma de
  `if` lo produce**: micro-banco de seis (`>=C`, `>C-1`, `!(<C)`, `<C`,
  `-1 < x-C`, sin signo), las seis canonicalizan; y la canonicalizacion
  sobrevive aunque la comparacion se ice fuera del bucle y se derrame con
  `mfcr`/`stw`, porque es de `fold`, no de `combine`. Los tres origenes reales:
  1. **`case LO ... HI:`** — la extension GNU de rangos compila con ProDG 3.9.3
     y reproduce el arbol exacto con sus `bge`/`ble` contra el literal.
     **Cinco funciones cerradas** reescribiendo una cadena de `if` como `switch`
     con rangos; en `CarRenderInfo::Render` (11.132 B) dio **tamano exacto** y
     el `stmw r14` identico.
  2. **Un bucle con cota VARIABLE** (`i < num - 1`): `fold` canonicaliza en el
     parseo y la propagacion de constantes llega despues, asi que la arista de
     salida sale cruda. El `blt 7` contra nuestro `ble 6` era exactamente eso.
     **Corolario para `casetree.py`: filtra los nodos cuyo `cmpwi` este en una
     arista de retorno.**
  3. **La constante guardada en un LOCAL.** En *expand* el operando es un
     **REG** y no un `CONST_INT`, asi que `simplify_relational_operation` no lo
     toca y CSE mete el valor despues.
     `return GetCurGear() >= SECOND_GEAR;` da siempre `cmpwi 2`+`mfcr`+`extrwi`
     (siete formas probadas); con `Gear second = SECOND_GEAR; ... >= second;`
     sale el `cmpwi 3`+`cror un,eq,gt`+`clrlwi 31` del objetivo.
     `SFXCTL_AccelTrans::ShouldPlayEngOffSweet` **116 B a la primera**. Misma
     familia que el `_c(a)` con `int a = 255` de la nota de `bMax(const,const)`.
  **El delator en NUESTRA fuente del caso 1 es `if (x != C) { if (x > C) {...} }`**
  con la misma variable y constante — artefacto de descompilar el arbol a mano;
  un `grep` lo localiza.
- **`slwi N+16` + `srawi 16` es extensión de signo CON DESPLAZAMIENTOS**, no un
  cast: `(int(a)<<16)>>16`.
- **`xoris rX,rX,0x8000` es conversión con signo.** **`cmplwi` ⇒ contador
  `unsigned`.** **`a | b` emite primero `b`.**

- **`a * (a * K)` SE REASOCIA A `(a * K) * a` Y SOLO UN TEMPORAL CON NOMBRE LO
  IMPIDE.** Micro-banco de catorce formas —literal, `a/4.0f`, `0.5f*0.5f`,
  `a*a*K`, `K*a*a`, parentesis de todo tipo, dos temporales de parametro—:
  **todas** dan `fmuls t,a,K` + `fmuls r,t,a`, con la variable como SEGUNDO
  operando. Solo `float h = a * K; ... a * h` (o un `float q = K;` y escribir
  `a * (a * q)`) conserva `fmuls r,a,t` con la variable delante, que es lo que
  emite el objetivo. **Ojo: con los operandos en REGISTRO (parametros) ni eso
  vale; hace falta que vengan de MEMORIA.** 1,3 puntos en `GTrigger::GTrigger`.
- **EL ACCESO POR PUNTERO O REFERENCIA A UN STRUCT `packed` CAMBIA `rlwimi` POR
  `rlwinm`+`or` AL ESCRIBIR UN BITFIELD.** `this->m.campo = v` conserva la
  alineacion y GCC usa `insv` (`rlwimi`); `T *p = &this->m; p->campo = v;` la
  pierde y cae a `store_fixed_bit_field`, que emite `rlwinm` (limpiar) + `or`
  (meter). **Y el valor tiene que venir de una VARIABLE**: un literal se pliega
  en `oris`/`ori` sobre la palabra ya enmascarada. Las dos condiciones a la vez;
  micro-banco de diez formas. `GTrigger::GTrigger` 99,32 -> 99,57%.
  **Techo medido encima**: el REGISTRO DESTINO del `or` (el del valor en el
  objetivo, el de la palabra en el nuestro) no cede a `register`, ni al orden de
  las dos declaraciones, ni a envolverlo en un `static inline` con el puntero y
  el valor como parametros.

## 8. Tipos, clases y símbolos

- **EL ENSAMBLADOR DE ProDG *TIRA* LOS ALIAS `.set` DE LA SYMTAB.**
  `__asm__(".globl X
	.set X,Y")` compila, sale en el `.s`... y el simbolo **no
  llega al ELF**. Lo que si funciona es **una etiqueta de verdad**:
  `__asm__(".section .text
	.globl X
X:
	.previous")`. Y **la etiqueta cae al
  FINAL de lo ya emitido en esa seccion**, no en el offset 0: para datos hay que
  usar el `__asm__("nombre")` **sobre la propia declaracion** y quitarle el
  `static`.
- **UN SIMBOLO DEFINIDO CON ENLACE INTERNO DONDE EL OBJETIVO LO EXPORTA BLOQUEA EL
  ENLACE IGUAL QUE SI FALTARA.** `promote.py` lo cantaba como «no definimos», y no
  era eso: lo definiamos **LOCAL**. Quitar el `static` --el nombre mangled ya lo
  fija el `__asm__("...")`-- dejo `snd/srandom` **LIMPIA con una palabra**.
- **LAS INSTANCIACIONES DE PLANTILLA `WEAK` DUPLICADAS NO ROMPEN EL ENLACE NI LAS
  VE `matched_code`, PERO BLOQUEAN LA PROMOCION.** `zMisc` casa al **100%** y
  exporta **106 simbolos de mas**, +8.360 B de `.text`: son instanciaciones que en
  el original viven en **zAI** (40 de ellas estan en el DOL, en el rango de
  `zAI.cpp`) mas 66 que el original **nunca hizo**. Es el mayor bloqueo de `linked`
  del arbol: **78.008 B**.
- **UN GLOBAL QUE SALE `UND OBJECT` EN `symtabdiff` Y UN STATIC-INIT POR DEBAJO
  DEL 100% SON EL MISMO BUG.** Un global **que no esta definido en ningun sitio**
  (solo el `extern` de la cabecera) se come su constructor del static-init: 208 B
  en un caso, 44 B en otro. **Buscalos con `symtabdiff.py <unidad>` antes de
  atacar el static-init.**
- **`float m[2][4]` Y `float m[4][2]` OCUPAN LO MISMO Y GENERAN CODIGO DISTINTO.**
  El objetivo iza **cuatro** bases (`+0x40`, `+0x44`, `+0x60`, `+0x64`) con **un**
  indice `i*8`; con `[2][4]` sale **una** base mas un `mr`+`ori 4` en tiempo de
  ejecucion. **Solo el acceso directo a un miembro `[4][2]` lo reproduce**: de 20
  grafias medidas, `(m[0] + 1)[i*2]` y `(&m[0][1])[i*2]` **pliegan** a
  `lfs 68(rX)`, y los punteros `float*` o el cast a `float(*)[2]` dan las cuatro
  bases pero **invierten el orden de operandos del `lfsx`**. 95,09 -> **99,45% con
  tamano exacto**.
- **LOS CUERPOS DE `Modify`/`GenerateUniqueKey` DE LAS CLASES GENERADAS DE
  AttribSys DEBEN ESPEJAR LOS DE `Attrib::Instance`.** A `gameplay::Modify` le
  faltaba el `LocalAttribCount() +`, y por eso `LocalAttribCount()` se evaluaba en
  **el llamante**, antes de `ClassKey()`, en vez de dentro del inline.
  **Delator general: un argumento evaluado en el llamante en vez de dentro del
  inline dice que al cuerpo generado le falta un termino.** Con el cuerpo
  arreglado, el llamante pasa a `Modify(key, 0)` y GCC pliega la suma. Medido:
  **+908 B**. La plantilla del generador la comparten 59 clases: hay mas.
- **CORRECCION MEDIDA: `ATTRIB_NO_INLINE_CLASSKEY` NO ES UN FRENTE DE BYTES.**
  Contando en los 17 candidatos cuantos `bl ClassKey__` emite nuestro `.o` frente
  al objetivo -- **incluyendo las reubicaciones a `.text+0xNNNN` de una copia
  local, que `objdump` no nombra y hacen parecer que faltan llamadas** -- salen
  **0 de menos y 0 de mas**. GCC 2.9 ya se niega a inlinar `ClassKey()` a la
  profundidad del ctor `Instance(FindCollection(ClassKey(), key), ...)` y emite el
  `bl` por su cuenta. Lo que la macro cambia **no son las llamadas**: es donde
  vive la copia y con que binding.
  **Y en seis unidades EMPEORA**, porque la guarda solo puede QUITAR inlines,
  nunca ponerlos: zEAXSound **−9.284 B** (su static-init cae de 100 a 99,01% al
  meterle dos `bl ClassKey` que el objetivo no tiene), zPhysics −1.820, zWorld
  −1.144, zEAXSound2 −1.080, zGameplay −936, zMain −880.
  **Regla: antes de encender la guarda en una unidad, cuenta las llamadas. Si ya
  emitimos tantas como el objetivo, la macro solo puede empeorar.**
  **DE DONDE VENIA EL ESPEJISMO, y por que la extrapolacion era falsa.** La
  guarda faltaba de verdad en cuatro cabeceras generadas de zFe2 y ahi valio
  **~1.780 B**; de ahi se generalizo a «17 unidades llaman ClassKey fuera de
  linea y solo zFe2/zFeOverlay definen la macro — zPhysicsBehaviors 77 llamadas,
  zGameplay 65, zSpeech 24, zWorld 19, zEAXSound2 16, zPhysics 12, zEAXSound 8,
  zSim/zCamera/zAI 4 — de ~20 cabeceras implicadas solo 9 tienen la guarda», y
  se lanzo como «el mayor frente abierto del proyecto». **El conteo estaba mal
  porque `objdump` no nombra las reubicaciones a `.text+0xNNNN` de una copia
  local**, y son justo las que hacen parecer que faltan llamadas. Con esas
  contadas: 0 de menos y 0 de mas. **El caso de zFe2 era real y aislado; la
  extrapolacion, no.** Y el interruptor de zFe2 **esta bien puesto**: apagarlo
  ahi pierde 1.376 B.
  Lo que si quedo de aquel trabajo, y es fidelidad de simbolos aunque
  `matched_code` no lo vea: las **59** cabeceras que definen `ClassKey` inline
  llevan ya la guarda (eran 9), 21 definiciones fuera de linea colocadas en el
  `.cpp` exacto donde el objetivo las pone, copias debiles espurias 13 -> 6, y
  unidades exactas 2 -> 11 de 17.
- **LOS DTOR Y `Callback` DE CLASES LOCALES LLEVAN EL SUFIJO `.NNNNN` DE
  `DECL_UID`, Y NUESTRO NUMERO NUNCA CASARA CON EL SUYO.** No son trabajo
  pendiente. Pero **si son un bug real de enlace**: nuestras vtables apuntan a
  simbolos UNDEF.
- **UNA TU QUE FALTA EN EL SourceList SE VE EN `debug_lines`.** `CopView.cpp` y
  `Still.cpp` tenian **6 entradas cada uno dentro del rango del static-init** y no
  estaban en la lista. Son TUs de **solo datos**, asi que no aparecen como
  funciones ausentes en ningun censo.
- **HAY FUNCIONES QUE NO SON EXPRESABLES EN C Y SE TRANSCRIBEN CON `asm()` DE
  AMBITO DE FICHERO.** `setjmp`/`longjmp` de la libc de SN son ensamblador puro
  (guardan r1, r2, r13..r31, LR, CR y f14..f31 en el `jmp_buf` realineado a 8).
  Hace falta `.type X,@function` **y** `.size` para que objdiff los empareje:
  **384/384 B al 100%, y `promote.py` la da LIMPIA**.
- **UN GLOBAL DEFINIDO DOS VECES METE SU CONSTRUCTOR EN LA UNIDAD EQUIVOCADA.**
  Si en el `.s` del objetivo el simbolo solo aparece **leido** (`lwz`) y nunca
  construido, y en el nuestro sale un `stw` a un campo suyo dentro del
  static-init, es que un `.cpp` de ESA unidad repite la definicion que vive en
  otra. Borrarla: la cabecera ya la declara `extern`.
- **UN GLOBAL CON `stfs` DE UN `.float 0` EN EL STATIC-INIT LLEVA CONSTRUCTOR
  EXPLICITO.** `bVector3 g;` no emite nada; `bVector3 g(0.0f, 0.0f, 0.0f);` emite
  los tres stores. Que el valor sea cero **no** significa que no haya
  inicializador: significa que el troceador nombro el literal `lbl_XXXX` y ahi
  esta el `.float 0`.
- **UN `static` DUPLICADO A MANO EN UN `.cpp` TAPA EL DE LA CABECERA.** Ocho
  `WAM_*` repetidos en `GenericNISControlScenario.cpp` impedian encender la guarda
  de la cabecera; borrandolos, el static-init de zAnim paso de **46,86 a 100%**.
- **UNA VIRTUAL EQUIVOCADA SE VE EN EL DESPLAZAMIENTO DE LA VTABLE**, no en el
  nombre: `lha 0x48` contra `0x50` son dos ranuras distintas. `AverageAir`
  llamaba a `GetWheelCenterPos` (2 ranuras) y `GetOOMass` (1) de mas: 98,17 ->
  **100%**.
- **NUESTRO GRAFO DE INCLUDES NO ES EL DEL ORIGINAL, Y EL DWARF LO DICE: CUENTA
  EN CUANTAS CU SALE CADA CLASE.** `WorldAnimNamedRange` sale en **4 CU**
  (zMain, zAnim, zFe2, zSim) y nuestra cabecera la incluyen **6** —- y **no**
  zFe2. Poner los inicializadores a pelo habria metido 8 constructores en zRender
  (80 B) y **zWorld (11.168 B), que hoy casan al 100%**. La solucion es un
  `#ifdef` encendido **solo en la SourceList que toca**; encenderlo en otra es
  una linea. **La causa de fondo se ve igual: `AnimPlayer.hpp` incluye
  `AnimWorldScene.hpp` y el original no** (`CAnimPlayer` en 8 CU,
  `CAnimWorldScene` en 4).
- **SI EN EL ELF NO HAY NI UN `_vt.` DE LA INTERFAZ NI UN CUERPO DE SUS VIRTUALES,
  ESAS VIRTUALES SON `= 0`.** Sin virtual pura GCC 2.9 **elige una llave** y emite
  los inlines como externos, y aparecen simbolos de mas. Cuatro interfaces, 48 B.
- **UN `static const X k(...)` A NIVEL DE FICHERO SE DELATA EN EL `.s`: SALE
  LOCAL.** Y su sitio en `.bss` dice **entre que dos objetos** va declarado.
- **TRES ESTADOS DE UN ARRAY ESTATICO DE PLANTILLA, Y SOLO UNO DA EL BUCLE DE
  CONSTRUCTORES.** `template <> T X<A>::m[N] = {}` **desenrolla** (64 `bl` + 63
  `addi` por array); `template <> T X<A>::m[N];` GCC lo trata como **declaracion**,
  el simbolo sale **UNDEF** y no construye nada; y **`T X<A>::m[N];` SIN
  `template <>` define y emite el BUCLE** con cuenta atras y avance de puntero.
  Eso solo quito los **159 `bl` y 153 `addi` de mas** de un static-init: 49,51% ->
  **100%, 4.464 B**.
- **UN INICIALIZADOR ESTATICO CON LLAMADA INLINE OBLIGA A INIT DINAMICA, Y LA
  CONSTANTE PLEGADA IDENTIFICA LA FUNCION.** El factor era `0x3C8EFA36` =
  `2*PI * (1/360.0f)` —el plegado de `bDegToRad`— y **no** `PI/180`
  (`0x3C8EFA35`, **a un ULP**). Si el array tiene los radianes ya calculados, el
  original escribia la llamada.
- **`Attrib::Instance` POR KEY TAMBIEN FUERA DEL FRONTEND, y hay que BORRAR el stub
  `asm` que tapaba el UNDEF.** Dentro del ctor por Key, `ClassKey()` cae a una
  profundidad donde GCC 2.9 **no** inlina y emite el `bl` mas la copia fuera de
  linea que el objetivo tiene. Dejar el `asm(".globl ClassKey__...")` da
  `Label ... multiply defined`.
- **UN ARGUMENTO DE CONSTRUCTOR IGUAL A `sizeof` DE LA CLASE ES EL TAMANO DEL
  MENSAJE MAL ATRIBUIDO.** En `M<X>(false, 0x20, intensidad, 0)`, el `0x20` es
  `sizeof(M<X>)` y pertenece a `Hermes::Message(_GetKind(), _GetSize(), 0)`, no al
  segundo argumento (que es **0**). **El delator es de CONTEO, no de valor**:
  escribimos la constante en **dos** ranuras y el objetivo en una, asi que se nos
  va a un registro salvado de mas y el marco crece 16 B. **Barrido: todo
  `M<X>(a, K, ...)` cuya `K` coincida con `sizeof(M<X>)`.**
- **UN ACCESOR DE CLASE ARREGLA EL ORDEN DE OPERANDOS DE UN `lwzx` QUE NO CEDE A
  NINGUNA FORMA DE FUENTE.** Con un local `T **p = (T**)((char*)&Obj + OFF);`
  alimentando `p[i]`, CSE propaga la definicion y la regla de `EXPAND_SUM` («el
  sumando con constante va el ultimo») **invierte** el `plus`: sale
  `lwzx rD, idx, base` en vez de `lwzx rD, base, idx`. Probadas tres formas
  (puntero local, `i[traps]` —binario identico, `fold` canonicaliza— y `__asm__("")`,
  que empeora). **El accesor de clase no invierte.**
- **UN CTOR POR DEFECTO QUE SOLO INICIALIZA PARTE DE LOS MIEMBROS ES CORRECTO.**
  Completarlo cerraba 88 B y **tiraba 3.132 B en seis funciones de otra unidad**.
  La salida es el **sitio de llamada** (`SndParams(0,0x7FFF,0x1000,0,0,0)`), con
  coste cero fuera.
- **`Attrib::StringKey::GetString()` YA DEVUELVE `""`**: la guarda de nulo del
  llamante son **16 B de mas**.
- **CUANDO DOS CAMPOS GUARDAN EL MISMO VALOR, DE CUAL LEE EL OBJETIVO FIJA LA
  FUENTE**: un ctor releia `mParams.other_object` y no `mActee`, con el actor en
  un local.
- **`bool` MIDE 4 B EN ESTE ProDG C++** (`stw`, no `stb`). Imprescindible para leer
  offsets de campo del asm sin equivocarse.
- **DOS FUNCIONES QUE DISCREPAN EN EL OFFSET DEL MISMO CAMPO = CAMPO EQUIVOCADO, NO
  LAYOUT EQUIVOCADO.** Una escribia 0x110 y otra 0x114: era el **campo vecino**.
  Intercambiar los dos campos arreglo una y **rompio la otra** (-304 B).
- **El cast `(u8)` decide el opcode**: `x &= ~3` sobre un `u8` **se estrecha** a
  `rlwinm 0,24,29`; `(u8)((x & ~3) | f)` conserva la mascara de 32 bits
  (`clrrwi`). **584 B.**
- **ESCRIBIR A MANO UN INLINE QUE EL VOLCADO NOMBRA CUESTA LA FUNCION.** El
  `nlen = Sqrt(LengthSquare)` a mano en vez de `Normalize()` valia **55 diffs**
  (66 -> 11) en una sola funcion. Igual con `MPH2MPS()`, `Vector4To3()` y
  `GetPosition()`. Si el volcado lo nombra, **llamalo**.
- **UN METODO QUE EL DWARF NOMBRA COMO INLINE LIBRE NO ES EL METODO DE LA CLASE.**
  `bSlerp(a, b, t)` (inline libre, que es lo que pedia el volcado) contra
  `a.Slerp(b, t)`: 128 -> 58 diffs, porque **libera el registro salvado** que el
  objetivo dedica a otra cosa. Primer paso de los cinco que cerraron
  `CarRenderInfo::Render`.
- **Los stores de pares de flotantes se escriben con `__builtin_psq_st(...)`**, no
  con la macro de union: 39 -> 20 diffs, porque el planificador deja de intercalar
  la preparacion de la llamada siguiente.
- **LA SINTAXIS `objeto : direccion` DE MWCC (el `AT_ADDRESS` del Dolphin SDK) NO
  ES COSMETICA.** `volatile u32 __EXIRegs[15] : 0xCC006800;` da `addi rN,rBase,
  0x6800` + `stwu/lwzu` con desplazamiento corto; `((vu32*)0xCC006800)[10]` pliega
  la direccion y da `stw r0,0x6828(rBase)`. Un solo cambio llevo una unidad de
  **628 a 980 B**.
- **En MWCC, `asm { ... entry <simbolo> ... }` reproduce los puntos de entrada
  alternativos byte a byte**, y `sym@ha`/`@l` **solo** funciona sobre simbolos
  declarados en C (una etiqueta local del propio asm da `illegal use of label`).
  ~800 B en una unidad.
- **GCC 2.9: UN CUERPO QUE SOLO ES `__asm__` SIEMPRE SE LLEVA UN `blr` DETRAS.**
  Micro-banco de 4 formas: `noreturn` no lo quita, `for(;;)` lo cambia por `b .`,
  y meter el `bl` en C anade prologo. **La unica forma sin prologo NI epilogo es
  `__asm__` de nivel de fichero con `.type nombre,@function` + etiqueta +
  `.size`** — y objdiff la puntua como funcion.
- **`static` NO SE INLINA EN C CON GCC 2.9 A -O2; `static inline` SI.** `qsort`
  paso de **36,97% a 100%** con **dos palabras**: `med3` y `swapfunc` salian como
  `bl`. Es el gemelo en C de la regla «en clase = inline» de C++.
- **DOS UNIDADES DEL TROCEADOR CON EL MISMO `.file` Y UNA FUNCION `local` BYTE A
  BYTE IDENTICA SON EL MISMO FUENTE COMPILADO DOS VECES.** `vfprintf.s` y
  `vfprintf_1.s` comparten `_vfwrite`; **la lista de `bl` separa las dos
  compilaciones** y da el `#define` que las distingue (`INTEGER_ONLY`). Escribir
  un solo fichero cobra la funcion compartida **dos veces**.
- **UN CTOR CON PARAMETROS Y CUERPO `{}` ES UN MIEMBRO SIN INICIALIZAR, y se ve
  como stores que faltan.** `FEngMovieStarter(FEPackage*) {}` y
  `FEngTransferFlagsToChildren(int32) {}`: 92,04% -> **100%** con dos listas de
  inicializacion. Barrido: solo **8 ctores asi** en `FEng/` + `Frontend/`.
- **`static_cast<A>(b)` PASA POR `operator f32*()` Y CREA UN TEMPORAL DE PILA; el
  cast de REFERENCIA no.** `*reinterpret_cast<const A *>(&b)` quito el temporal,
  3 registros salvados y 0x18 de marco: 86,24% -> **100%**.
- **UN LITERAL DE CADENA PROPIO DONDE NOSOTROS INDEXAMOS UN ARRAY DELATA EL CUERPO
  DEL ACCESOR.** El objetivo referencia `"16.2.1"` en `.rodata` y no
  `sDDayRaces+0x20`: el accesor **devuelve el literal**, como su vecina.
- **EL VALOR DE UN `enum` EN EL ARGUMENTO SI LO VE objdiff**: `li r4,-1` contra
  `li r4,4`. Un `grep` de `li rN,<const>` contra el enum es triaje **gratis**.
- **UN `operator new` CON CUERPO VACIO ES UN `bl` QUE FALTA, y en el diff se ve
  como un BUCLE DE COPIA sobre un registro que nadie escribe.**
  `FECustomizationRecord::operator new(size_t) {}` hacia que `new T(*c)` copiase
  0x198 B **sobre basura**. **El cuerpo correcto lo dicta el `operator delete`
  gemelo de la misma clase**: si el delete hace `gFastMem.Free(mem,size,NULL)`,
  el new es `return gFastMem.Alloc(size, NULL);`.
- **`IsNotEmpty()` SE ESCRIBE CON TERNARIO, NO CON `if`+`return`.** Micro-banco de
  4 formas: el `if/return` mete `mr r0,r9 / b / li r0,0` de mas porque el
  `return false` no se cross-jumpea con el `li 0` del calculo; `p && p[0]` da la
  forma sin salto; **solo `return p ? p[0] != 0 : false;`** reproduce el objetivo.
- **ESCRIBIR EL CTOR DE UNA CLASE PAGA DOS VECES**: cerrar `PVehicle::PVehicle`
  arrastro `map<UCrc32,UCrc32,...>::map` (116 B) al 100%, porque la plantilla no
  se instanciaba mientras el miembro no se construyera.
- **`Ctor() { Clear(); }` con `Clear()` declarado DESPUES emite `bl`; declarado
  antes, se expande.** La regla del orden en clase, confirmada sobre el ctor de un
  miembro.
- **LA ARITMETICA `slot = 0x10 + 8*indice` IDENTIFICA LA VIRTUAL EQUIVOCADA SIN
  COMPILAR.** Cuatro aciertos en una ronda (`IsStaging`->`GetDriverStyle`,
  `IsActive`->`IsDestroyed`, `rigid_body->GetPosition`->`simable->GetPosition`,
  `BackupArrives`->`Quadrant`). **Y si el slot cae FUERA de la vtable de la base,
  el objeto es de la derivada.**
- **`UCrc32(key)` y `UCrc32(key.GetString())` SON CTORES DISTINTOS**: el primero
  lee `mHash32` (`lwz 0x8`), el segundo hace `lwz 0xc` + test de nulo +
  `stringhash32`. Y **un `bl stringhash32` FUERA del bucle prueba que hay un
  local**: GCC no iza llamadas.
- **`bRadToDeg` tenia el CUERPO VACIO en `bMath.hpp`.** El patron de bits
  `0x42652EE0` solo sale con `PI` **float**, no double: `radians * (360.0f / (2*PI))`.
  Bloqueaba una funcion de 1.788 B. Enesima confirmacion de «cabeceras que mienten».
- **TECHO DURO DEL COMPILADOR: GCC 2.9 NO EMITE LOS VIRTUALES DE UNA CLASE LOCAL A
  UNA FUNCION.** Los 16 simbolos de las clases locales de `FEPlayerCarDB`
  (`Callback__CQ3...`, `_._Q3...`) salen **UNDEF** en nuestro `.o` aunque la fuente
  los defina, y **la vtable —que si emite— los referencia**. Micro-banco de 15
  lineas lo reproduce; probados los **cinco ProDG del arbol** (identicos), 6 formas
  de fuente y 8 flags: solo `-fkeep-inline-functions` emite el destructor, y ni
  asi el `Callback`. El objetivo los tiene LOCAL con la vtable GLOBAL; nosotros
  vtable WEAK y los virtuales UNDEF. **624 B inalcanzables en zFe2.**
- **LA MATERIALIZACION DE BOOL QUE LE SOBRA AL OBJETIVO (`li 1; cmp; bne; li 0;
  cmpwi 0`) ES UN ACCESOR INLINE QUE DEVUELVE BOOL.** Se repone con el accesor
  (`!IsImpounded()`, 60 B) o con un `bool` local (168 B). **Y al reves: si nos
  sobra a NOSOTROS, la condicion del original era `(x & M) == M`, no `!= 0`.**
- **LA `const`/NO-`const` EQUIVOCADA EMITE UN `bl` A UN ACCESOR VACIO, Y `deadreg`
  LO CANTA.** Un miembro `T *m` leido desde un metodo `const` da `T *const`: **el
  puntero NO es const**, asi que se coge la sobrecarga **no-`const`**. Si esa esta
  vacia, sale basura sin ningun aviso.
- **`UTL::COM::QueryInterface<T>(p)` (LIBRE) CONTRA `p->QueryInterface(&x)`
  (MIEMBRO) SE DISTINGUEN EN EL DIFF POR CUATRO INSTRUCCIONES DE NORMALIZACION.**
  El miembro devuelve `bool`, asi que dentro de una cadena `||` GCC materializa el
  valor: `mr. rN,r3` + `li r0,1` + `bne` + `li r0,0` + `cmpwi r0,0`. La libre
  devuelve `T*`, y **si el argumento se acaba de comprobar contra nulo** su guarda
  `if (pUnk)` se pliega y no queda mas que `mr. rN,r3` + `beq`. **Delator: un
  `mr. rN,r3; beq` pegado al `bl Find__Q43UTL3COM6Object6_IListPv` significa
  funcion LIBRE; con normalizacion, miembro.** En `SteeringWheelDevice::
  UpdateForces` eran 32 B y 8 lineas de diff, y el mismo fuente usa **las dos**:
  libre en la cadena de guardas, miembro en el `if (vehicle->QueryInterface(&input))`
  suelto.
- **`_GLOBAL_.I.<simbolo>` SE LLAMA COMO EL PRIMER OBJETO GLOBAL DEL TU, y un
  estatico de clase solo DECLARADO le cambia el nombre.** `_GLOBAL_.I.__6Camera`
  (nuestra primera funcion) contra `_GLOBAL_.I._6Camera.StopUpdating`: definir
  `int Camera::StopUpdating = 0;` antes del primer metodo del primer `.cpp` de la
  SourceList arregla **el nombre, el UND y 44 B** de golpe. Se detecta con
  `symtabdiff --und` mirando los «estatico-de-clase».
- **EL ORDEN DE DECLARACION EN CLASE DECIDE QUE INLINE ANIDA.** GCC 2.9 compila
  los cuerpos en clase **en orden de declaracion**: si `IsTextureType` va ANTES
  que `IsClutType`, dentro de la primera sale `bl` a la segunda, y desde una
  funcion normal se inlina. Es lo que reproduce un `bl` a un accesor trivial que
  en todos los demas sitios va inline. 75% -> 100% en 204 B.
- **`unsigned int mFlags : 24` ES lo que produce el par `clrlwi rX,r0,8` /
  `rlwimi r0,rX,0,8,31`.** Ese par es la firma de un **bitfield de 24 bits**, no
  de una estructura *packed* (probado: `packed` da `rlwimi` sobre la palabra
  entera).
- **`flag == ENABLED` sobre un accesor con ternario se pliega a `andi.`/`beq`.**
  Para conservar el `li 1`/`andi.`/`bne`/`li 0`/`cmpwi 1` del objetivo hay que
  escribir el accesor como `f = ENABLED; if (!(mFlags & M)) f = DISABLED;
  return f;`.
- **DOS PUNTEROS DISTINTOS AL MISMO ACCESOR NO SON INTERCAMBIABLES.** `m_pEAXCar`
  y `m_pStateBase` apuntan al mismo objeto (herencia), pero el original **reusa el
  puntero que acaba de comprobar**. Delator: `lwz r9,0x18(r31)` seguido de
  `lwz r9,0x34(r9)` — el 0x34 cuelga del 0x18, no del 0x14.
- **Dos `stw rN,0x8(r1)`/`stw rN,0xc(r1)` justo antes de un `bl bFree` son
  `lista.InitList()`**: el original vacia la lista porque sus nodos viven dentro
  del bloque que va a liberar.
- **`crclr cr1eq` delante de `memset` se repone sin tocar `string.h`**:
  `extern "C" void *X(void *dst, ...) __asm__("memset");` y llamar a `X`. Da el
  `crclr` exacto y no cambia el manglado ni afecta a otros llamantes.
- **DECLARAR UNA CLASE COMO `namespace` EN UN `.cpp` CAMBIA EL MANGLING DE SUS
  METODOS SIN ARGUMENTOS.** Un miembro sin parametros mangla `f__Q26Speech7Manager`;
  una funcion libre en el namespace equivalente mangla `f__Q26Speech7Managerv`,
  porque para la funcion libre la lista de parametros **siempre** se codifica.
  **Con argumentos NO se nota** (los dos codifican la lista), asi que el bug se
  esconde hasta que topas con un metodo sin parametros. Medido: `static` contra no
  estatico y `()` contra `(void)` manglan **igual**, asi que no es eso.
  `uiSMSMessage.cpp` tenia dos casos (`Speech::Manager` y `MiscSpeed`, que ademas
  se llama `MiscSpeech`): 5 sitios llamando a simbolos inexistentes.
- **LAS INSTANCIACIONES HUERFANAS DE PLANTILLA SE PIDEN A MANO, Y BASTA CON
  DECLARAR EL TIPO ADELANTADO.** `namespace Sim { class IEntity; }` mas
  `template Sim::IEntity **find<Sim::IEntity**, Sim::IEntity*>(...)` en la
  SourceList: **704 B en cuatro simbolos, al 100% a la primera**. Acota la entrada
  de 'plantillas sin llamante': faltan porque nadie las instancia, **y se pueden
  instanciar**.
- **`static const` frente a `static` en un estatico de funcion**: solo con `const`
  la lectura es `RTX_UNCHANGING` y GCC la iza fuera del bucle. +2,2 puntos sin
  mover una sentencia.
- **UN ACCESOR QUE RECONSTRUYE EL STRUCT (`return bVector2(m.x, m.y);`) EMITE
  `lfs`/`stfs` MIEMBRO A MIEMBRO; DEVOLVER EL MIEMBRO (`return m;`) EMITE LA COPIA
  POR PALABRAS `lwz`/`stw`.** Corregirlo pago **+580 B en una unidad que nadie
  estaba tocando**.
- **QUE GCC 2.9 NO ANIDE INLINES A TRES NIVELES ES UNA PALANCA, NO SOLO UN
  LIMITE.** El original se apoya en `ctorA -> ctorB -> Clear()`: en el nivel 3
  `Clear()` **no** se inlina y emite `bl`. La llamada explicita equivalente esta
  en el nivel 1, se inlina, y anade una copia entera (+80 B). **Borrar la llamada
  explicita cerro la funcion.**
- **ACOTACION a la regla de `bVector3` con ctor en su sitio: para `bVector2` es al
  REVES.** El temporal **anonimo** (`this->m = bVector2(0,0);`) es el que produce
  la ranura de pila que el objetivo tiene; la local con nombre y las componentes
  sueltas, no. 972 B.
- **LA VIRTUAL EQUIVOCADA: si UNA funcion usa un slot de vtable distinto y sus
  hermanas casan con el viejo, el error esta en la FUENTE, no en la clase.** La
  tentacion es declarar `virtual` el accesor inline para insertar el slot que
  falta: **eso costo -6.512 B** porque desplaza la vtable para todas las hermanas.
  Lo correcto era llamar a **otra virtual que ya existia**. Detector: barrer pares
  `lha rX,0xNN(r9)` / `lwz rX,0xMM(r9)` con la misma base y offset distinto.
  **452 + 1.136 B.**
- **LA LLAMADA VIRTUAL REPETIDA CONTRA EL LOCAL: en el frontend gana REPETIR.**
  Caso particular de la entrada «miembro contra local cacheado» de §3, con el
  vptr en el papel del miembro: el delator es un `lwz r9,0x28(this)` repetido
  donde nosotros tenemos una sola carga. Confirmado en tres funciones.
- **UN `bl` CUYO RESULTADO NADIE LEE ES UN `if` VACIO, NO CODIGO MUERTO.** El
  objetivo tenia `bl bStrICmp` **sin `cmpwi` detras**: es
  `if (bStrICmp(x, "...") == 0) { }` con el cuerpo vacio. GCC no puede borrar la
  llamada (externa) pero si el test.
- **UN MIEMBRO SIN INICIALIZAR EN EL CTOR SE VE A LA VEZ COMO UN STORE QUE FALTA
  Y COMO UNA CONSTANTE QUE EL OBJETIVO IZA.** El objetivo tenia `li r14,1` izado
  fuera del bucle y `stw r14,0x10(r28)`, y rematerializaba la vtable *dentro*;
  nosotros izabamos la vtable y no escribiamos 0x10. Era un `bEnabled = true` que
  faltaba. **Corolario: si el objetivo iza una constante entera pequena a un
  salvado y nosotros izamos una direccion, busca el campo que no escribimos.**
  928 B.
- **`FEngMemCpy(this, &m, sizeof)` en un `operator=` contra la copia miembro a
  miembro**: 16 `lfs`/`stfs` alternando f0/f13 contra un `bl`. El mapa de lineas
  lo confirma — cada par lleva **linea propia**. 42% -> 100%.
- **LLAMAR A LA SOBRECARGA EQUIVOCADA BORRA UN SIMBOLO QUE YA ESTABA AL 100%.**
  `bQuaternionToMatrix` debe llamar a `GetMatrix(bMatrix4 *)`, no a
  `GetMatrix(bMatrix4 &)`: la version por puntero esta definida EN clase **antes**
  que la de referencia, asi que no puede inlinarla y emite `bl`. Llamar a la de
  referencia inlina 48 instrucciones y **hace desaparecer**
  `GetMatrix__C11bQuaternionR8bMatrix4` (192 B).
- **`x.size() != 0` no es `x.empty()`**: `empty()` compara `begin==end` (`cmpw`);
  `size()` hace `(end-begin)>>2` con `srwi.`.
- **`bClamp(int,int,int)` contra el clamp escrito a mano**, y con `int`, no `char`.
- **EL BUCLE EXPLICITO `while (!lista.IsEmpty()) delete lista.RemoveHead();` AL
  FINAL DE UNA FUNCION DUPLICA EL DESTRUCTOR DE `bTList`** — 17 instrucciones,
  68 B. Borrarlo llevo una funcion de 91,5% a 97,3% de un golpe. **Esta en todo
  el frontend**: cualquier limpieza manual de un `bTList` local sobra.
- **El bucle sobre una lista de la que se van quitando nodos RELEE `GetHead()`, no
  camina con `next`.** `while (l.GetHead() != l.EndOfList()) { ... }` ligando el
  puntero solo justo antes de `Remove()`. Con `node = next` sobran salvados y
  saltos. Y `node->Remove()` carga Next y Prev **antes** de los dos stores, al
  reves que escribir los campos a mano.
- **Un ctor que ya inicializa (`RideInfo ride;` llama a `Init(-1,0,0,0)`) hace que
  el `ride.Init(...)` explicito sea una llamada de mas.** Mismo patron que el
  `SetDefaultLayout` de Attrib::Gen.
- **UNA CLASE VACIA EN EL VOLCADO (`total size: 0x1`, ni un miembro) ES UNA CAPA
  ANULADA, Y SUS ACCESORES DEVUELVEN CONSTANTES.** Delator en el asm: **el retorno
  de una llamada se descarta y el argumento siguiente es `li rN,0`**. Toda la capa
  online de GameCube es asi (`OnlineRacer`, 22 accesores, cero miembros); cerro
  `Game_AddPlayer` a la primera.
- **`Attrib::StringKey("")` en lista de inicializacion emite dos `bl
  StringHash64/32`; `StringKey()` almacena 0/0/"" como constantes.** Delator: al
  objetivo le sobran `li 0` donde nosotros llamamos. **Un parentesis vacio = 768 B.**
- **`IsNotEmpty()` y `!IsEmpty()` NO son intercambiables**: `IsEmpty()`
  (`s[0] == 0`) sale sin salto (`subfic`+`adde`); `IsNotEmpty()` mete rama mas
  materializacion del bool, 4 instrucciones de mas.
- **La negacion tiene que estar DENTRO del inline.** `if (!X.IsOn())` hace que
  `do_jump` invierta la rama; `if (X.IsOff())` —inline cuyo cuerpo ya es la
  condicion negada— materializa el valor y salta `beq`. **Ni un `bool` local ni
  un `static inline` que llame al primero valen: GCC ve a traves de los dos.**
- **`GetList()` inline contra `const List &` es POR SITIO, y lo decide si el
  accesor lleva argumento.** Sin argumento y **con una llamada dentro del
  bucle**, el original la repite inline en `begin()`/`end()`; ligarla a una
  referencia es lo contrario. **Con argumento (`GetList(idx)`) el original SI
  liga la referencia.** 832 B en cinco funciones.
- **Un destructor DUPLICADO mas un salto al avance del bucle es
  `for(...; ...; advance())` con `continue`**, no un `while` con el avance al
  final: GCC emite una copia del cleanup en cada salida temprana del ambito.
- **`T arr[N];` contra `T arr[N] = {};`**: con lista vacia GCC **desenrolla** el
  bucle de constructores; sin ella emite el bucle.
- **El ctor `Attrib::Gen::<clase>` YA llama a `SetDefaultLayout`.** Uno explicito
  detras emite **una segunda copia entera** del bloque `DefaultDataArea` mas un
  retest nulo (6 instrucciones por sitio). Borrarlo cerro una funcion de 396 B.
- **Un `bl Metodo` SIN `mr r3,<this>` delante significa que el metodo es
  `static`**, y GCC 2.9 mangla igual un miembro estatico y uno no estatico sin
  parametros: **la conversion es gratis y no toca ningun llamante**.
- **El ID de un chunk se lee del `lis`, no se copia del hermano**: uno resulto
  ser `0x00135200` (sin el bit 31) mientras sus dos hermanos eran `0x8013xx00`.
  Delator: `lis r4, 0x13` contra `lis r4, 0x8013`.
- **Estar al FINAL del TU, detras de las instanciaciones de plantilla, es la
  firma de un cuerpo definido DENTRO de la clase.** Mover la definicion a un
  `.cpp` que va antes en la SourceList y escribirla in-class dio **96,75% a la
  primera compilacion** en una funcion de 1.495 instrucciones.
- **Un accesor de cabecera vetada con cuerpo vacio Y miembros privados no se
  arregla leyendo el miembro**: se repone como `static inline` libre en el `.cpp`
  **leyendo por offset**, y se envuelve el que si es publico.
- **Un inicializador que es una LLAMADA INLINE es lo que obliga a la init
  dinamica.** Dos `static const unsigned short` cuyos `li` llevan la linea de
  `bMax(int,int)` en `bMath.hpp`: el front-end no puede plegar la llamada, asi
  que emite init dinamica. **Un literal pelado se plegaria y no emitiria nada** —
  si al objetivo le sobran stores de constantes en el static-init, busca una
  llamada inline en el inicializador.
- **Un array que el enlazador borro (`address 0xFFFFFFFF`) puede seguir teniendo
  su bucle de ctores en el static-init.** Si ves un bucle de N ctores vacios sin
  simbolo que le corresponda, el array existe en la fuente aunque no en el
  binario.
- **`__asm__` en una DEFINICION rompe el ensamblador de SN** (`Unrecognised
  opcode`). La salida es declarar la funcion con **el manglado como nombre
  propio** dentro de `extern "C"`. Y **un puente de constructor no se escribe
  como constructor**: uno devuelve `this` y cuesta salvar r3 (49,5% -> 100%).
- **Los miembros `inline` definidos FUERA de la clase en un `.cpp` no se
  inlinan** con GCC 2.9, ni poniendo `inline` en la declaracion: hay que meter el
  cuerpo dentro de la clase.
- **`plan.py` da la firma real de un accesor por su EFECTO**: uno resulto ser
  `mOp |= cmd & 0xf0` y su pareja guardaba ademas el valor anterior. Escribir el
  par suelto costo **808 B en tres funciones vecinas que ya estaban cerradas**, y
  solo medir el arbol entero tras tocar la cabecera lo destapo.
- **Un envoltorio `inline` decide si la llamada sale VIRTUAL o desvirtualizada.**
  `obj.IsHead(obj.GetCurrentOption())` desvirtualiza porque `obj` es concreto;
  `obj.AtHead()`, que por dentro hace `this->IsHead(...)`, sale **virtual**, que
  es lo que el objetivo tiene. 360 B mas 1,7 puntos en una hermana.
- **La referencia local al global va ACOTADA AL BLOQUE, no a la funcion.** Si el
  objetivo hace `addi rN, r9, gGlobal@l` una vez por `case` y `mr r3,rN` en cada
  llamada, la fuente tenia **una referencia por bloque `case`**; a nivel de
  funcion se pasa de largo. 24 funciones de una unidad ensenan este patron.
- **UNA DECLARACION LOCAL CON EL TIPO DE RETORNO EQUIVOCADO INVIERTE EL ORDEN DE
  LOS ARGUMENTOS CONSTANTES.** Micro-banco: `void f(int,int); f(0,8)` da
  `li r3,0; li r4,8`; `int f(int,int); f(0,8)` da `li r4,8; li r3,0`. El manglado
  no lleva el retorno, asi que **retipar la declaracion es gratis**. Valio 344 B,
  y **es un frente entero**: cualquier `.cpp` que redeclare a mano funciones de
  libreria como `void` es sospechoso.
  **Pero el frente es mucho mas pequeno de lo que parecia**: barrido el arbol
  cruzando toda declaracion `void` a nivel de fichero contra el retorno que da el
  volcado, salen 65 candidatos y **casi todos son homonimos** (`Update`,
  `Render`, `Reset`, `Play`). De los cinco inequivocos, dos ya estaban al 100% y
  los otros tres valen **0 B**. Sirve caso a caso cuando el diff senala el orden
  de los `li`, no como barrido. El script queda en el scratchpad.
- **El valor de retorno DESCARTADO que el DWARF nombra como local no es
  cosmetico**: ligarlo cambia el orden de emision de los argumentos de esa misma
  llamada. 296 B.
- **El delator del vararg que falta tambien es `lwz`, no solo `lbz`**: un `lwz
  rN, off(base)` cuyo registro es el siguiente argumento y que ademas se compara
  justo despues significa que la llamada lleva un argumento mas, CSE-ado con el
  de la comparacion. 280 B.
- **El `crclr cr1eq` se lee en las DOS direcciones y por fichero.** Su presencia
  dice «llamado sin prototipo»; su AUSENCIA dice «con prototipo», y el mismo
  símbolo puede ir de las dos formas en TU distintos: `_sn_sinit` lleva `crclr`
  desde `fclose`/`fflush`/`refill` (declarado K&R en la cabecera interna) y NO
  desde `fopen.c` (que añade el prototipo). Un `crclr` sobrante en `fflush` era
  `_fwalk` sin declarar.
- **`if (x > n) return -1; return x;` mete un `mr` por retorno**; la guarda
  negada (`if (x <= n) return x; return -1;`) no. Cerró `vsnprintf`. Es la misma
  familia que «acumulador contra guard clause», pero sobre el valor de retorno.

- **Un `lbz` en el registro de argumento justo antes de una llamada de un
  argumento significa que la llamada tiene un argumento MAS.** Unico cambio que
  cerro una funcion de 1.384 B desde 99,79%.
- **`crclr cr1eq` antes de un `bl` = el llamado se declaro SIN prototipo**, y se
  reproduce declarandolo variadico. Delante de `memset` es ademas oraculo del
  argumento: solo aparece con **valor 0 literal Y longitud constante**.
- **Un `operator delete` con cuerpo VACIO borra la rama `__in_chrg` entera** del
  destructor (16 B en vez de 36).
- **Una funcion `inline` definida DESPUES de su llamante no se inlina, pero su
  copia fuera de linea se emite igual al FINAL del `.o`.** El orden del `.s`
  engana.
- **Un accesor con cuerpo vacio en una cabecera vale bytes de verdad**: reponer
  uno valio 184 B. Y **un `Set()` inline que llama a un `Clear()` fuera de
  linea**: si el objetivo emite el `memset` inline pero conserva el simbolo
  `Clear`, la fuente escribia el `memset` directo en el inline. Una linea,
  1.644 B.
- **Un puntero local al estatico** reproduce el calculo unico de direccion;
  repetir el nombre del estatico lo rematerializa.
- **`s32` es `long`, no `int`**: se pasa con cast; declararlo `s32` cambiaria el
  manglado.
- **La API pública de las bibliotecas C sale SIN manglar aunque el fichero se
  compile con `-x c++`.** En `snd`, `MIX_*`, `SFILTER_add/remove/connect`,
  `rsflc`, `SNDover`, `SNDPLAYSETDEF`… están en el ELF sin manglado: la
  definición necesita `extern "C"`. El síntoma es inconfundible —la unidad mide
  **0%** y el lado nuestro del diff sale **vacío**, porque objdiff empareja por
  nombre. Y al reves: si el objetivo trae `__Fii`, la declaracion debe estar
  FUERA del bloque `extern "C"` de la cabecera.
- **En SN ProDG `char` es CON signo**: `(char)p` da `extsb`, `(unsigned char)p`
  da `clrlwi rX,rY,24`. En PPC EABI estandar es al reves.
- **`_4Path.xxx` es un `namespace`**, no un estatico de clase; eso seria
  `xxx__4Path`.
- **`lhz` contra `lhax` sobre un `short*` delata un cast**: `(unsigned
  short)off[i]` da `lhz`, `off[i]` da `lhax`. Conviven en la misma unidad.
- **Contador `unsigned` para `cmplw`**: con reduccion de fuerza, el bucle sobre
  un array compara punteros con `cmpw` si el indice es `int` y con `cmplw` si
  es `unsigned int`. Una instruccion, 168 B.
- **`mBuf[0] = 0` no es `memset(mBuf, 0, sizeof)`**: reponer el memset+strncpy
  en los `Clear()`/`Init()` inline llevo una funcion de 69% a 100%.
- **Un local `T *p = &base[i];` en vez de repetir `base[i].campo`** reproduce el
  calculo unico de direccion del original (90% -> 100%).
- **Dos structs homonimos en namespaces distintos salen FUNDIDOS en el
  volcado**; hay que separarlos por el manglado.
- **En clase = inline y no emite símbolo**; **fuera de clase sin `inline` =
  llamada real**. Dos excepciones: GCC 2.9 no inlinea recursivamente a `-O1`, y
  las virtuales declaradas en clase **sí** emiten símbolo.
- **Un inline que falta en una cabecera vetada se repone como `static inline`
  libre en el `.cpp`** y genera el mismo código.
- **Y un método PRIVADO de una cabecera vetada se alcanza por nombre de
  ensamblador**, sin tocarla:
  `int F(T*, int) __asm__("GetCombinedSectionNumber__13TrackStreameri");`
- **Un destructor virtual vacío DECLARADO por el usuario emite su propio store de
  la vtable; el sintetizado no.** Borrar el destructor vacío llevó cuatro dtors
  de 83,5% a 100%.
- **El constructor de copia declarado convierte un struct pequeño (≤8 B) en local
  direccionable con ranura BAJA del marco**; sin él GCC lo deja en pseudo y lo
  derrama al final. Un destructor vacío da el mismo efecto.
- **Una llamada REAL a un inline in-class delata un WRAPPER inline**: un
  `bl IsFlagSet` en el cuerpo significa que el fuente decía `GetIsActive()`, que
  es un inline llamando a otro inline, y **GCC 2.9 no anida a `-O1`**.
- **Un constructor declarado y nunca definido impide que se emita la vtable**, y
  sin vtable no salen las copias fuera de línea de ningún virtual de la clase.
- **`_STLP_NO_FORCE_INSTANTIATE` sólo funciona en el PRIMER `.cpp` de la
  SourceList**; en cualquier otro llega tarde.
- **Un tipo declarado `bool` en un sitio y definido `int` en otro** hace que todas
  las llamadas apunten a un símbolo manglado que no existe.
- **El orden de declaración de dos sobrecargas decide si GCC inlinea una en
  otra**: los cuerpos en clase se compilan en orden de declaración.
- **Los métodos definidos en clase que necesitan copia out-of-line salen en
  `.gnu.linkonce.t`, al final del TU y como `weak`.** Es la firma que delata un
  cuerpo dentro de la clase.
- **La compuerta de la vtable**: `_vt.` —y las copias fuera de línea de los
  virtuales declarados inline— **sólo se emite en el TU que DEFINE el primer
  virtual no-inline declarado**. Valió ~1.400 B por clase. **Comprueba antes en
  `report.json` que figuran como MISSING**: los `_vt.` son **rodata** y
  `matched_code` no cuenta datos.
- **LA OTRA MITAD DE LA COMPUERTA: SI TODOS LOS VIRTUALES SON PUROS NO HAY
  METODO LLAVE, Y ENTONCES EL TU QUE LOS USA SI EMITE LA VTABLE *Y* EL
  DESTRUCTOR.** Tres destructores de interfaz (`_._17IPlaceableScenery`,
  `_._17ITriggerableModel`, `_._Q214EventSequencer8IContext`, **280 B**) salian
  al 0% porque nuestro `.o` tenia `_vt.<clase>` **UNDEF**: las cabeceras
  declaraban `virtual void PickUp();` sin `= 0`, GCC elegia esa como metodo
  llave y daba la vtable por externa. **La prueba esta en el `.s` del
  troceador: los `pfn` de la vtable son `__pure_virtual`.** Poner `= 0` a los
  cinco virtuales cerro los tres destructores, y el control de arbol entero
  (33 SourceLists reconstruidas) dio **+280 B en UNA unidad y cero movimiento
  en las otras 32**.
  **Trampa del arreglo**: al emitir la vtable, GCC emite tambien la copia fuera
  de linea del `_IHandle()` de `DECL_INTERFACE`, y choca con los stubs
  `asm(".globl _IHandle__...")` que alguien escribio para tapar el UNDEF —
  `Label ... multiply defined` del ensamblador de SN. **Hay que borrar los
  stubs en el mismo cambio**; si no, la unidad no compila.
- **El delator de la herencia está en la symtab**: la ausencia de
  `_vt.<clase>.<base>` prueba que no hereda de ella.
- **EL VOLCADO DA LA FIRMA DEL INLINE, NO SOLO SU NOMBRE, Y UN PARAMETRO MAL
  TIPADO SE PAGA EN EL MARCO DEL LLAMANTE.** `BoundsPack::Table::Add` estaba
  declarado `void Add(const Pair &entry)` y sin definir, asi que el ctor hacia
  `upper_bound`+`insert` a mano con dos locales (`entry`, `position`) que el
  original no tiene. El volcado dice `inline void Table::Add(struct Collection *
  collection)` **con su propia local `Pair pair` en `r1+0x20`**: los
  temporales viven DENTRO del inline, no en el llamante. Escribir la firma
  buena y el cuerpo (`Pair pair(c->fNameHash, c); insert(upper_bound(begin(),
  end(), pair), pair);`) cerro `BoundsPack::BoundsPack` (**956 B**) de golpe.
  **Sintoma sin leer el volcado: nuestro marco 8 B MAS PEQUENO y un registro
  salvado de menos que el objetivo.**
- **`extern` contra `static` también se lee en la symtab del objetivo**: un
  símbolo con manglado de namespace que sale **UNDEF** prueba `extern`. Y un
  `static` de tipo con vtable arrastra construcción dinámica a **toda** unidad
  que incluya la cabecera, así que quitarlo del `.hpp` paga en varias a la vez.
  Un carácter en una cabecera valió 3.040 B en otra unidad.
- **`lha rX, N(r9)` entre 8 da el índice del virtual llamado.** Y la **aritmética
  de ranuras COM**: `slot = 8 · (posición_en_la_lista_de_virtuales + 2)`.
- **Leer el miembro en vez de llamar al getter**: GCC 2.9 **no** desvirtualiza el
  getter propio de la clase.
- **`sizeof(bool)` es 4** y un miembro `bool` se lee con `lwz`, indistinguible de
  `int` por layout. **El tipo del parámetro decide dónde ocurre la conversión.**
  **`bool` de retorno normaliza (`li 1`/`li 0`/`cmpwi`), `int` no**, y un
  accesor que devuelve `bool` sobre un miembro **`int`** normaliza igual:
  declarar el miembro `bool` (mismo tamaño, 4 B) borra las instrucciones.
- **Las cabeceras mienten, y sobre todo en la ALINEACIÓN DE BASE**: una base con
  un `uint64_t` fuerza alineación a 8 y desplaza toda la clase.
- *(pista, sin cifra)* **La local "sólo en el original" que nombra regmap suele
  ser el VALOR DE RETORNO DESCARTADO de una función declarada `void`.** El mangling no lleva el
  tipo de retorno: **re-tipar una declaración sin definición es gratis.**
- **La lista de inicialización contra el cuerpo decide `lwz/stw` o `lfs/stfs`**
  si el tipo declara `operator=`. Y decide **cuándo corren los ctores de
  miembro**: con lista, en **orden de declaración**; con cuerpo, **los primeros**.
- **Acumulador contra guard clause**: `T *p = 0; if (ok) { … p = f(); } return p;`
  gasta un registro salvado de más y un `mr` por retorno;
  `if (!ok) return 0; … return f();` casa. **El opcode de la copia dice el tipo**:
  `lwz/stw` ×3 = asignación de struct del mismo tipo; `lfs/stfs` ×3 = ctor u
  `operator=` miembro a miembro.
- **La cadena de copias delata el tipo declarado**: `UVector3 d = a-b` elide el
  sret; `UMath::Vector3 d = a-b` conserva la copia extra.
- **Un estático de plantilla sólo declarado apaga la instanciación entera**, y
  **hace falta inicializador** (`= T()`). Sólo pagan los de tipo contenedor con
  vtable. Es **por unidad**, en el `SourceLists/z<Unidad>.cpp`.
- *(pista, sin cifra)* **La instanciación que falta puede ser un CAST que
  falta.**
- **`BNEW` = `new (__FILE__, __LINE__)`** → `__builtin_vec_new`; el `new` pelado
  da `__builtin_new`.
- **`GX_WRITE_U8/U16/U32` son macros de `dolphin/gx/GXPriv.h`**, que `dolphin.h`
  **no** arrastra: sin el include compilan **en silencio** como varargs.

- **UN `bl` A UN METODO QUE ES `inline` EN CLASE PRUEBA QUE EL LLAMANTE USO EL
  ENVOLTORIO, NO EL METODO.** GCC 2.9 no anida inlines a tres niveles: si el
  objetivo emite `bl SetFlag__5GIconUi`, el fuente decia `mIcon->Show()`
  (inline que llama a `SetFlag`), no `mIcon->SetFlag(1)`. Lo confirma el volcado,
  que lista `GIcon::Show()` y `GIcon::ShowOnMap()` como expansiones inline. 0,7
  puntos en `GTrigger::GTrigger`.
- **EL MISMO MECANISMO EXPLICA EL `bl Reset__3MD5` DE `MD5 md5;`.** El
  constructor `MD5() { Reset(); }` se inlina (nivel 2) pero su llamada a
  `Reset()` queda a nivel 3 y sale como `bl`. **Un `md5.Reset()` explicito
  detras esta a nivel 2, se inlina, y anade quince instrucciones** (los cuatro
  `lis`/`ori` del estado inicial mas seis stores). Borrarlo es lo que llevo
  `SaveGameplayData` de 85,4% a 98,2%. **Corrige la nota "MD5::MD5() NO es
  vacio"**: el ctor esta bien; lo que sobraba era la llamada explicita.
  Mismo patron pendiente en `FEDatabase.cpp` (zFe2), que tiene dos.
- **UNA CABECERA QUE DECLARA `float` DONDE EL NOMBRE DICE `long` VALE 180 B DE
  CODIGO FANTASMA.** `struct LongVector { float x, y, z; }` hace que
  `v.x = static_cast<int>(f * 65536.0f)` emita la conversion entera->flotante de
  vuelta (`xoris 0x8000`, `stw`, `lfd`, `fsub`, `frsp`) **por componente**.
  Delator: al nuestro le sobran tripletes `stw`/`lfd`/`fsub`/`frsp` que el
  objetivo no tiene en ningun sitio. `UpdateCameraMovers` 63,9 -> 77,8% con
  `long x, y, z;`.

## 9. Del frontend

- **EL FUENTE DEL FRONTEND USA `|`, NO `||`, Y ESO VALE UNIDADES ENTERAS.** La
  familia `UnlockSystem` estaba transcrita con cortocircuito y el original
  acumula con OR bit a bit. Tres formas, todas medidas:
  1. **`answer = answer | X;` sobre un `bool`** distribuye el `!= 0` en LOS DOS
     operandos (`cmpwi/li 1/bne/li 0` por cada uno) y deja el `or` **sin
     normalizar** detras. Es lo que emite el objetivo; el `||` da ramas.
  2. **`answer |= (cond);` es la forma CORTA del idiom de 12 lineas**
     (`if (c) { answer |= true; bool tmp = true; if (!answer) tmp = false;
     answer = tmp; } else if (!answer) answer = false; else answer = true;`) que
     ya estaba transcrito en este arbol. **No son intercambiables**: el largo
     deja `cmpwi rTmp,0` donde el objetivo pone `mr rAnswer,rTmp`.
     `DoesCategoryHaveNewUnlock` paso de 95,33% a **100% (976 B)** solo con eso,
     y las cuatro `IsUnlockableUnlocked`/`IsCarPartUnlocked` de 40-57% a 100%.
  3. **`return (a != 0) | (b != 0);`** reproduce el `or r3,r9,r0` con las dos
     normalizaciones **independientemente del tipo de retorno declarado**, asi
     que no hace falta tocar la cabecera para probarlo.
  **984 + 976 + 276 B en una ronda.**
- **UN `mr` DE COPIA DEL VALOR INICIAL SE REPONE CON UN INLINE QUE LO DEVUELVE.**
  El objetivo materializaba `UnlockAllThings != 0` en un **scratch** y hacia
  `mr rSalvado,rScratch`; nosotros lo materializabamos directo en el salvado.
  `bool answer = AllThingsUnlocked();` -con el `static inline bool
  AllThingsUnlocked() { return UnlockAllThings != 0; }` que YA estaba en el
  fichero, solo que definido 150 lineas mas abajo- lo repone. **Cerro cuatro
  funciones de golpe (984 B) que estaban a un solo diff.** Un `bool t = expr;
  bool answer = t;` da el mismo binario: lo que cuenta es que el valor nazca en
  un pseudo distinto del destino.
- **EL BUCLE DE BUSQUEDA DEL ORIGINAL LLEVA LA LLAMADA PELADA EN EL PREHEADER.**
  `while (true) { p = &Get().At(i); if (cond) break; ++i; }` emite **una** copia
  de la llamada y **un** `lis` del global; el objetivo emite **dos** de cada una
  (la del preheader con el indice constante `li r4,0`). La forma es
  `p = &Get().At(i); while (!cond) { p = &Get().At(i++); }` -ojo al `i++`, que
  usa el valor VIEJO y por eso el primer indice se consulta dos veces: es asi en
  el binario-. `PostRaceResultsScreen::SetupResults` (1.088 B) de 93,60% a
  99,19% con ese solo cambio en sus dos bucles.
- **`int rank = x + 1; ...; x = rank;` REPRODUCE LA PARTICION DE RANGO DEL
  CONTADOR** (`addi rNuevo,rViejo,1` arriba y `mr rViejo,rNuevo` abajo) donde
  `++x` al principio del cuerpo emite `addi rViejo,rViejo,1` y nada mas. **Y la
  POSICION del write-back decide si gcse iza una direccion invariante AJENA**:
  con `x = rank;` justo tras el bucle interior, el `lis FEDatabase@ha` del
  segundo bucle dejo de izarse (99,19% -> 97,96%); moviendolo detras del bloque
  siguiente volvio a izarse y la funcion cerro al **100%**. Es el mismo cambio
  en dos sitios y solo uno de los dos ordenes vale: **hay que probar los dos**.
- **UNA CADENA `if (x == C) ... if (x > C) ...` CON ETIQUETAS `goto` ES UN
  `switch`, Y EL ORDEN DE LOS `case` ES EL ORDEN DE EMISION DE LOS CUERPOS.**
  `DoesCategoryHaveNewUnlock` tenia los tres cuerpos en el orden equivocado: el
  primero emitido leia `TheUnlockData[11..15]` y el nuestro `[4..10]`. Reescrito
  como `switch` con los `case` en orden de valor (1, 2, 3) el arbol de despacho
  casa entero. **74,14% -> 95,33% en una compilacion**, y el `|=` cerro el
  resto. **El delator es el OFFSET del primer `lbz` de cada cuerpo**, no el
  arbol de saltos.
- **EL ARGUMENTO ESCRITO EN LINEA IZA TAMBIEN EL PUNTERO GLOBAL, Y SE BARRE CON
  UN ESCANER DE 20 LINEAS.** `RaceSettings *s = FEDatabase->GetX(...);
  FEDatabase->FillY(c, s);` recarga `FEDatabase` tras la llamada;
  `FEDatabase->FillY(c, FEDatabase->GetX(...))` lo deja en un salvado y lo reusa
  -que es lo que emite el objetivo-. **Firma exacta en el diff**: el objetivo
  hace `lwz rSalvado, GLOBAL@l(rX)` **antes** del `bl` y `mr r3,rSalvado`
  despues; nosotros `lwz r3, GLOBAL@l(rX)` en el sitio. Barrido de las dos
  unidades del frontend con ese patron: **3 aciertos, los tres cerrados**
  (`UIQRTrackOptions::NotificationMessage` 640 B, `AddRace` 184 B y
  `CustomizeMain::NotificationMessage` +1,3 pt).
- **`for (i = 0; i < Lista.CountElements(); i++)` REPITE LA LLAMADA CADA VUELTA**
  y ahorra el salvado del `count`; el `int count = ...` fuera del bucle lo gasta.
  Delator: el `b` de la cola del bucle salta **por encima** del `bl` de la
  cuenta en el objetivo y por debajo en el nuestro. Parte de los 640 B de
  `UIQRTrackOptions`.
- **VEDAS DE UNA RONDA ANTIGUA — DOS DE LAS TRES ESTAN ROTAS (auditoria
  2-sep).** Se conservan porque las MEDIDAS siguen siendo ciertas; lo falso era
  la conclusion.
  - `CarCustomizeManager::GetUnlockFilter` (92 B, 2 diffs, tamano exacto) —
    **sigue abierta, hoy al 91,30%**: el `li r3,4` del objetivo va **detras**
    del `cmpwi` y a nosotros nos lo mete el planificador delante; **diez
    formas** de fuente (ternario, guarda invertida, local, `!=` en vez de `==`,
    `else` explicito, local del `GetGameMode()`) dan las diez el mismo hueco.
  - `MilestoneBoard::MilestoneBoard` (500 B) — **VEDA ROTA: hoy al 100%.** Lo
    medido sigue valiendo como acotacion de §5: la **permutacion inversa** de la
    tirada de stores BAJA de 96,65% a 87,32% porque uno de los stores es una
    llamada a constructor (`mScrollTimer = Timer(0)`) y otro usa un registro de
    valor distinto — **esa acotacion es la buena; «la funcion es techo» no lo
    era**.
  - `UnlockSystem::IsEventAvailable` (248 B) — **VEDA ROTA: hoy al 100%.** Es
    cierto que invertir la polaridad de la guarda para conservar las dos copias
    del `li r3,1` baja 0,7 pt; lo que la cerro fue otra cosa —**una guarda
    `== 19.8.31` envolvente mas un `return true` de cola**— que llevo 96,60 ->
    98,23% y de ahi al 100%.
- **EN EL FRONTEND, `plan.py --dwarf-only` ES EL PLANO LITERAL DE UN `Setup` O DE
  UN CONSTRUCTOR, Y TRAE TRES COSAS QUE NO ESTAN EN EL DIFF.**
  1. **La cadena de accesores exacta**: el volcado lista `IsCarLotMode()`,
     `IsDDay()`, `IsQuickRaceMode()`, `IsSplitScreenMode()`, `IsOnlineMode()`,
     `IsLANMode()`, `IsCustomizeMode()`, `IsCareerMode()` en orden. Nuestra
     transcripcion tenia `unsigned int mode = GetGameMode();` y una cadena de
     `(mode & 0x8000)`: **con el local, GCC prueba que el `& 4` interior del
     inline es redundante y borra la materializacion del bool** que el objetivo
     si emite (`xori/subfic/adde`). Con los accesores no.
  2. **Los `const unsigned long FEObj_<NOMBRE>` en bloque anonimo**: cada hash
     de objeto FEng del original es una constante con nombre declarada en su
     propio `{ }`, no un literal en la llamada. El volcado los nombra uno a uno
     (`FEObj_carlot`, `FEObj_2players`, `FEObj_PLAYER_TEXT_GROUP`...).
  3. **`FEngSetInvisible(pkg_name, hash)` es un INLINE de dos argumentos**
     (`FEngInterfaceFEObjects.hpp`), no `FEngSetInvisible(FEngFindObject(...))`.
  Transcribir las tres llevo `UIQRCarSelect::Setup` de 95,34% a **100% (688 B)**
  en dos compilaciones. **Es el frente mas rentable que queda en zFeOverlay**:
  casi todos los `Setup`/`NotificationMessage`/ctor de pantalla estan escritos
  con literales y con el `mode` cacheado.
- **EL `if (puntero)` DEFENSIVO QUE EL ORIGINAL NO TIENE CUESTA 8 B Y LO DELATA
  EL BLOQUE ANONIMO DEL DWARF**: si el volcado declara `stable` y `car` en el
  **mismo** bloque anonimo, entre las dos llamadas no hay `if`. Ultimos 8 B de
  `UIQRCarSelect::Setup`.
- **`(x >> 8) & 0xFF` SE CARGA COMO BYTE; `x >> 8` NO.** El objetivo hacia
  `lbz r5, 0xa(rSd)` sobre un `int` en `0x8` y nosotros `lwz 0x8` + `srawi 8`.
  Es el gemelo del `lbz 0xb` que ya casaba para `x & 0xFF`.
- **EL `cr7` DEL FRONTEND SALE SOLO CON EL BUCLE SIN ROTAR MAS LA CONDICION
  REUTILIZADA DESPUES.** `UIWidgetMenu::SetInitialOption` (492 B) tenia el
  `cmpwi cr7, rW, 0` del objetivo y nosotros `cr0`. Dos cambios, los dos
  necesarios y ninguno suficiente:
  1. **Sacar la accion del cuerpo del bucle a un `if` posterior sobre la MISMA
     variable**: `while (w) { if (c) { A; break; } w = w->next; }` ->
     `while (w) { if (c) break; w = w->next; } if (w) { A; }`. Ahora la
     comparacion `w != 0` la usan DOS saltos en bloques distintos, CSE las funde
     y el pseudo CC pasa a `global_alloc`, que reparte `cr7`. 86,38 -> 91,59%.
  2. **`for (;;) { if (!w) break; ... }` en vez de `while (w)`**: el `while`
     ROTA el bucle y se lleva la comparacion al fondo, con lo que la de arriba
     deja de ser la misma; el `for(;;)` la mantiene en la cabecera. 91,59 ->
     98,86%.
  3. El ultimo `mr` lo dio **una sola declaracion de `w` compartida por las dos
     ramas** (el volcado la pone en el MISMO bloque anonimo que
     `need_first_avail`), no dos `FEWidget *w` locales. **100%.**
  **Ojo al `goto` del arbol**: mover la declaracion arriba con inicializador da
  `jump to label ... crosses initialization`; hay que dejar
  `bool x; x = false;` como estaba.
- **EL SCRATCHPAD ES COMPARTIDO ENTRE AGENTES.** Un `nm.py` propio aparecio
  reescrito por otro agente a mitad de ronda. **Pon un prefijo tuyo a cada
  script y a cada `.json` que dejes ahi**, igual que a los ficheros de medida.
- **`Attrib::Instance` es constructor por `Key`, nunca `FindCollection`.**
- **`GetName()` devuelve `const Attrib::StringKey &`**, sin buffer sret.
- **UN RACIMO `REAL` DE ATTRIB ENTERO PUEDE SER UNA SOLA LINEA: UN CONSTRUCTOR
  STUB.** En zPhysics los ~31 descuadres de la familia Attrib
  (`DefaultDataArea`, `~Attribute`, `RefSpec::Clean`, `GetAttributePointer`) no
  eran 31 arreglos **ni** un tipo mal nombrado:
  `BehaviorSpecsPtr(ISimable *, int)` en `Physics/Behavior.h` era un **stub con
  `// TODO` que pasaba `0`** al padre en vez de llamar a
  `LookupKey(owner, index)` — y la sobrecarga gemela `(Behavior *, int)`, tres
  lineas mas arriba, **si** la llamaba. Su `LookupKey` es quien expande
  `Attribute::Get(index, RefSpec&)` mas los tres `~RefSpec()`, o sea **todo** el
  racimo. Una linea: **REAL 64 -> 57** y `Smackable::Smackable` (3.120 B) de
  **63,72% a 74,44%**, con `matched_code` clavado al byte y `zAI.o` **byte a
  byte identico** (control, misma cabecera compartida). Generaliza la regla de
  `EventSeqEngine` al eje de `callcheck`: **cuando una sobrecarga esta vacia y
  su gemela no, la gemela es la plantilla exacta del cuerpo que falta.**
- **`callcheck --where` AGRUPADO POR FUNCION LLAMANTE CONVIERTE UN RACIMO EN UNA
  LISTA DE CUATRO SITIOS.** Cruzar cada simbolo `REAL` contra la funcion que lo
  llama **en los dos lados** concentro los 31 descuadres de Attrib de zPhysics
  en **cuatro** constructores, y de paso enseño que tres de ellos son **codigo
  sin escribir**, no simbolos mal: `PVehicle::Construct` es un `return NULL;`
  (1.588 B en el objetivo) y `PVehicle::PVehicle` **no esta definido en ningun
  sitio del arbol** (1.764 B). O sea que **el racimo de `callcheck` y el hueco
  de bytes eran el mismo trabajo**. Ojo al rendimiento: el `--where` de serie es
  O(n·m) sobre el `.o` y tarda **mas de dos minutos por simbolo** en unidades
  grandes; con un `bisect` sobre las funciones ordenadas por offset salen
  **todos** los simbolos a la vez en segundos.
- **EL `>>> CIERRA` DEL PERMUTADOR APARECE SOBRE UNA FUNCION QUE OBJDIFF PONE AL
  99,24%.** En `PhysicsObject::PhysicsObject` la **base** ya puntuaba
  `score=1.000000` con `1a divergencia=353` de 353 instrucciones mientras
  objdiff media **99,24234%**: el diff real son **tres stores reordenados**
  (el objetivo hace `stw r30,0x40(r31)` **antes** de `stw r31,0x3c(r31)`), que
  su comparador de texto normaliza y no ve. Refuerza la veda ya escrita: el
  `--sweep` es **generador de candidatos**, y su `CIERRA` no es un veredicto
  **ni cuando afirma que la base ya esta perfecta**.
- **EL DICCIONARIO DE NOMBRES DEL ALPHA 138 DE XBOX NO TENIA NADA QUE CORREGIR EN
  `Attrib` (resultado negativo, medido).** Las 29 clases `Attrib::*` del volcado
  —incluidas las trampas `attribute_data` y `layout` en minuscula, y
  `CollectionHashMap` **ademas** de `HashMapTable`— **ya existen en el arbol con
  la grafia exacta**. Descarta de un `grep` la hipotesis del «tipo en el
  namespace equivocado» para todo el racimo de Attrib: si el manglado no cuadra,
  la causa esta en el cuerpo que falta, no en el nombre.
- **`QueryInterface` devuelve bool y el original lo TESTEA**:
  `if (!x->QueryInterface(&p)) return;`.
- **Llamar al constructor en vez de repetir su cuerpo a mano** desatascó tres
  near-miss sin escribir código nuevo.

- **EL BLOQUE DE STORES DE UN CONSTRUCTOR SEPARA LA LISTA DE INICIALIZACION DEL
  CUERPO, Y ESO SE LEE SIN COMPILAR. ES EL FRENTE MAS RENTABLE DEL FRONTEND:
  +7.488 B y 13 funciones en una ronda, 6 constructores cerrados a la primera o
  segunda compilacion.** GCC 2.9 emite primero las **cabezas** —el ULTIMO store
  de cada registro de valor distinto, en orden de fuente— y detras **el resto en
  orden de fuente**; la lista de inicializacion corre ANTES del cuerpo y **en
  orden de DECLARACION**. Corolario que se aplica al reves: si el objetivo emite
  un prefijo de stores que **no** esta en orden de miembro y a continuacion una
  tirada que **si** sigue el orden de DECLARACION, el prefijo es el CUERPO y la
  tirada es la LISTA. Ejemplo trabajado, `GarageMainScreen::GarageMainScreen`
  (632 B, 80,69% -> **100%** con un solo cambio): el objetivo emitia
  `0x70,0x74,0x7c,0x88,0x8c` (cinco registros de valor distintos = cinco cabezas
  = las cinco asignaciones del cuerpo) y luego
  `0x30,0x34,[ctor del miembro],0x50,0x58,0x78,0x80,0x84`, que es **exactamente**
  el orden de declaracion de los ocho miembros restantes -> a la lista.
  Cerrados asi: `UIQRCarSelect` (732 B), `UIQRBrief` (324 B),
  `CustomizationScreenHelper` (176 B), `CustomizationScreen` (160 B),
  `GarageMainScreen` (632 B) y `UIWidgetMenu` (492 B).
- **Y LO QUE DECIDE EL ORDEN NO ES SOLO QUE MIEMBROS ESTAN EN LA LISTA, SINO
  CUALES SE QUEDAN EN EL CUERPO.** En `UIWidgetMenu` los siete `const char *`
  (0xb8..0xd0) se emitian DESPUES de los `bVector2` de la lista y el objetivo al
  reves; como en la clase van **antes**, meterlos en la lista los pone primeros
  en RTL y la funcion casa (93,05% -> **100%**, 492 B). **Regla practica: un
  miembro que el objetivo escribe ANTES que otro declarado despues de el solo
  puede estar en la lista.**
- **EL CTOR IMPLICITO DE UN MIEMBRO YA HACE EL TRABAJO: LA ASIGNACION EXPLICITA
  ES CODIGO DE MAS Y SE VE EN EL TAMANO.** Cinco casos medidos en una ronda:
  `FilteredCarsList.InitList()` cuando `bTList::bTList()` hace los dos mismos
  stores; `LoadingRideInfo.Init(...)` cuando `RideInfo::RideInfo()` llama a
  `Init` (GarageCarLoader emitia **cuatro** `bl Init` y el objetivo dos: 55,97%
  -> **100%**); `raceSettings.Default()` cuando `RaceSettings::RaceSettings()`
  ya lo llama; `ScrollTime = 0` sobre un `Timer` miembro; y
  `mGeometryModels = FEGeometryModels()`, que ademas **materializa un temporal
  de 0x14 B en pila y lo copia** (+72 B). **El delator es siempre el mismo: el
  objetivo escribe el miembro y NO hay `bl` ni copia.**
- **`mMapDefaultPos = FEVector3(0.0f);` CONTRA EL CTOR POR DEFECTO DEL MIEMBRO:
  el orden de los tres `stfs` lo decide.** `FEVector3()` hace `x = y = z = 0.0f`
  (asignacion encadenada), que emite **z, y, x**; el temporal + `operator=` emite
  x, y, z mas la copia. En `Minimap::Minimap` borrar la linea valio 91,10% ->
  96,22%.
- **`FEngGetCenterX/Y` EXISTEN Y SON LA FORMA DEL ORIGINAL.** Si el objetivo
  llama **dos veces** a `FEngGetCenter` con ranuras de pila distintas y se queda
  la `x` de la primera y la `y` de la segunda, la fuente no es
  `FEngGetCenter(o, m.x, m.y)`: son los dos inlines de
  `FEngInterfaceFEObjects.hpp`. `Minimap::Minimap` 96,22% -> 99,52%.
- **UN `struct` LOCAL COPIADO ENTERO CONTRA TRES ASIGNACIONES DE CAMPO.** En
  `FEMarkerSelection::FEMarkerSelection` (668 B, 83,56% -> **100%**) el objetivo
  ponia a cero **tres** ranuras contiguas antes de la llamada y luego copiaba
  **los tres campos** al array: es `Selection sel; ...; TheMarkers[i] = sel;`, no
  `TheMarkers[i].Marker = marker; .Param = param; .Selected = false;`. Los otros
  dos detalles que hicieron falta: el indice ligado a un local
  (`int index = NumVisibleMarkers; TheMarkers[index] = sel;
  NumVisibleMarkers = index + 1;`) porque `NumVisibleMarkers++` **recarga** el
  miembro tras escribir en el array, y el contador del bucle `unsigned`.
- **`cmplwi` CONTRA `cmpwi` EN UN CONTADOR DE BUCLE ES `unsigned int`, Y SUELE
  SER EL ULTIMO DIFF.** Dos `for (int ...)` a `unsigned int` cerraron
  `Minimap::Minimap` (996 B) desde 99,52%. Es un diff de un solo `REPLACE` y se
  ve de un vistazo.
- **LA CADENA `if (A==0 || B==0 || ...) {} else if (!f()) X;` DEL FRONTEND ES UN
  `bool` CON `&&`.** `MenuScreen::~MenuScreen` (376 B) paso de 94,34% a **100%**
  escribiendo
  `bool ok = bStrCmp(...) != 0 && ... && !cFEng::Get()->IsPackagePushed(...);
  if (ok) { ... }`. **El delator es la materializacion (`li rN,0` temprano +
  `subfic`/`adde` al final + un `cmpwi rN,0` compartido al que saltan todas las
  comparaciones)**; con `if/else if` sale `cmpwi r3,0` + salto y faltan tres
  instrucciones. El ctor de la misma clase gana 8 puntos con el mismo cambio.
- **`int x = A > K ? A : 0;` FUERA DEL `if` CONTRA `if/else` CON DOS
  ASIGNACIONES.** `LeaderBoard::Update` (1.360 B) estaba a **4 diffs** con
  `int n; if (m>1) { ...; n = m; if (m<=1) n = 0; } else n = 0;`; el ternario
  detras del `if` lo cerro al **100%**. El delator: el objetivo tiene la variable
  en un **salvado** (r30) y nosotros en `r0`.
- **PARTIR LA ASIGNACION COMPUESTA CIERRA UNA PERMUTACION DE FLOTANTES SALVADOS,
  Y VALE PARA `*=` IGUAL QUE PARA `Max`.**
  `GarageMainScreen::UpdateRenderingCarParameters` (1.188 B) llevaba 9 diffs con
  `float height = average_wheel_radius * 0.25f - average_wheel_z + K;`; escribir
  `average_wheel_radius *= 0.25f;` antes y `height = average_wheel_radius -
  average_wheel_z + K;` lo dejo al **100%**. Descartado antes: orden de
  declaracion, inicializador muerto, orden dentro del bucle, `bVector4` campo a
  campo (todos dan el mismo binario o peor).
- **VEDAS DEL FRONTEND, CON EL PORCENTAJE DE HOY (auditoria 2-sep). DOS DE
  CINCO ESTABAN ROTAS.**
  - `FEMarkerSelection::NotificationMessage` (756 B) — **abierta, 97,83%**: el
    arbol del `switch` funde las dos hojas de `case 0xbb3e313d`/`case
    0xf0966d46` hacia el lado contrario que el objetivo; **seis formas**
    (etiquetas invertidas, cuerpo duplicado, literales, `return` en vez de
    `break`, y separar los dos `case` por otro) dan **binario identico** o peor.
  - `MilestoneBoard::MilestoneBoard` (500 B) — **VEDA ROTA: hoy al 100%.** Las
    **24 permutaciones** del bloque de stores estaban barridas con minimo de
    **7 diffs** (orden `mScrollTimer, mPlayerBinNumber, mNumMilestones,
    mMilestoneSetVisible`), y meter los miembros en la lista de inicializacion
    empeoraba a 30. **La permutacion no era la palanca.**
  - `PostRacePursuitScreen` (460 B) — **abierta, 98,22%**: corrimiento uniforme
    de registro en el contador del bucle; cuatro formas del `for`/`while` dan lo
    mismo.
  - `MenuScreen::MenuScreen` (472 B) — **VEDA ROTA: hoy al 100%.** Llevaba
    97,80% con 3 diffs y tamano exacto (el objetivo relee `sd->PackageFilename`
    una vez y copia con `mr`, nosotros dos veces) y **doce formas** medidas
    (local, referencia, asignacion dentro del argumento, lista contra cuerpo,
    orden de las dos asignaciones) sin moverlo. Doce formas del sitio
    equivocado; el hermano `MenuScreen::~MenuScreen` cerro con la cadena
    `if (A==0 || B==0 || ...)` reescrita como un `bool` con `&&`, y el ctor
    gana 8 puntos con el mismo cambio — esa es la pista que se dejo sin tirar.
  - `zFe2` **con `ATTRIB_NO_INLINE_CLASSKEY` desactivado pierde 1.376 B** aunque
    el objetivo inlinee `pvehicle::ClassKey` en `FEPlayerCarDB::Default`: el
    interruptor esta bien puesto, la diferencia es **por sitio**. *(Sigue
    vigente; vease la entrada consolidada de ClassKey en §8.)*

---

## 10. Método

- **`bool x = cond;` contra `bool x = false; if (cond) x = true;` es LOCAL A LA
  FUNCION, no del fichero.** Un metodo hermano casa al 100% con la segunda forma
  y copiarla al vecino lo **empeora de 1 a 16 diffs**. La forma del hermano es
  una pista, no una regla.
- **Cuando una familia de funciones hermanas no cede, mira cuantos REGISTROS
  SALVADOS usa cada lado.** Las tres `CarRenderInfo` (16,5 kB) fallan porque el
  objetivo **mantiene mas direcciones vivas en salvados**: 18 GPR contra 17 y
  12 B mas de marco. Eso no se arregla reordenando sentencias; hay que darle al
  compilador una razon para izar esa direccion (el `extern const float` lo
  consiguio en una de las tres).
- **`pct.py` (scratchpad `wld/`): porcentaje Y numero de diffs de varias
  funciones en UNA sola invocacion de objdiff.** Es lo que hace baratos los
  barridos ahora que la regla es contar diffs, no leer el porcentaje.
- **EL ORDEN DE CREACION DEL POOL DE CONSTANTES ES UN DIAGNOSTICO, Y ES GRATIS
  — ejemplos trabajados** *(la entrada canonica, con la acotacion de que
  arreglar el orden NO puntua, esta en §1)*. El pool es **por funcion** y se crea
  en **orden de aparicion en el fuente**. El del objetivo se lee de las
  DIRECCIONES de los `lbl_` en el `.s` del splitter; el nuestro, del NUMERO de
  `$LCn` en nuestro `.s` (`ngccc -S`). Las tres lecturas:
  - **mismo conjunto, otro orden** -> al fuente le falta una referencia
    temprana a esa constante. `float secondsElapsed;` -> `= 0.0f;` cerro
    `VisualLookEffect::UpdateActive` (**300 B**, 97,27 -> 100%) porque metio el
    `0.0f` delante del sesgo del timer. **El inicializador muerto no emite
    codigo pero SI crea la entrada del pool.**
  - **al objetivo le sobra una constante** -> es codigo que no hemos escrito, y
    su VALOR dice cual: en `HolePunchAvoidables` el objetivo tiene `0.19999999`
    (0x3E4CCCCC) **y ademas** `0.2` (0x3E4CCCCD); el primero solo sale de
    `1.0f - 0.8f` plegado, no de escribir `0.2f`.
  - **a nosotros nos sobra una** -> constante que el original expresa de otra
    forma.
  **Ojo al filtrar**: hay que mirar TODAS las referencias `lbl_`/`$LC` del rango
  de la funcion, no solo las de `lfs`/`lfd`; filtrando por opcode se pierden
  constantes (en `HolePunchAvoidables`, 9 de 14). Barrido completo de zWorld y
  zWorld2: solo 4 funciones tienen el orden mal, asi que como triaje es de una
  pasada.
- **Cuidado con `sed -i` en este arbol**: convierte el fichero entero a LF y
  rompe cualquier script que parta por CRLF.
**El stdio de la libc de SN es TRANSCRIPCIÓN, no arqueología.** Es newlib 1.8 con
tres retoques (`__sfp`/`__sinit` renombrados a `_sn_sfp`/`_sn_sinit`, sin
`sfmoreglue` ni `_free_r`, y buffers preasignados vía `_sn_IO_buf_ptr`), y la
cabecera REAL está en el árbol: `src/Speed/GameCube/bWare/GameCube/SN/include/`
(`sys/reent.h` da `sizeof(FILE)=0x60`, `_flags` short en +0xC, `__sglue` en
+0x1D8 de `struct _reent`; `sys/stat.h` da `sizeof(struct stat)=64`). Trece
funciones de trece cerraron a la primera o segunda compilación: **3.188 B en
`stdio.c`, `fwalk`, `fclose`, `fflush`, `makebuf`, `wsetup`(refill), `sn_buf`,
`fopen`, `sprintf`, `vsprintf`, `printf`, `fprintf`**. Lo único que no cae es
`fseek` (996 B), atascado en una permutación pura r30/r31 entre `fp` y `ptr`
(81 líneas de diff, todas `ARG_MISMATCH`; probados orden de declaración de las
seis locales, `register`, tipos y formas de retorno).

- **De las unidades de la libm solo `math_support` y `mbtowc_r` son del juego, y
  las dos cierran al 100%** (2.324 + 796 B, cinco funciones, la mayoria a la
  primera compilacion). *(El `grep` de una linea que separa la libc de SN de la
  del juego —`@sda21` contra `@ha`— y la prueba de `floor`/`sn_floor` estan en
  §1.)*
- **EN LAS UNIDADES DE SN, LAS CONSTANTES CON NOMBRE SE REPONEN CON `SDA_FLOAT`
  / `SDA_DOUBLE` DE `src/libc/fdlibm.h`; EN LAS DEL JUEGO NO, VAN `static
  const`.** Poner las macros SDA en una unidad del juego (o `static` a secas,
  que tambien va a `.sdata`) da `@sda21` de UNA instruccion donde el objetivo
  tiene DOS. Es el mismo dial en las dos direcciones.
- **TECHO MEDIDO Y CERRADO: EL SESGO DE `(float)(int)` BLOQUEA TODA LA LIBM DE
  SN — 12,4 kB en 12 unidades.** El expansor `floatsidf2` de GCC hace
  `force_reg (DFmode, CONST_DOUBLE 4503601774854144)`, o sea que el sesgo
  `0x4330000080000000` sale **del pool del compilador**, y el pool de este ngcc
  vive en `.rodata`: `lis @ha` + `lfd @l` donde el objetivo tiene un solo
  `lfd bias@sda21`. **El multiconjunto de mnemonicos de `sf_log10` difiere solo
  en `lis` 3 contra 4**, con 69/69 instrucciones y el tamano exacto (93,284%).
  Lo descartado, medido uno a uno:
  1. **Ningun flag mueve el pool**: `-msdata={data,eabi,sysv,none}`, `-G` 4/8/16/
     32/64/1024, `-meabi`, `-mrelocatable(-lib)`, `-mtoc`/`-mminimal-toc`/
     `-mfull-toc`/`-mfp-in-toc`/`-msum-in-toc`, `-mps-float`, `-fpic`,
     `-fwritable-strings`. Los **cinco ProDG del arbol** dan lo mismo. (`cc1.exe`
     tiene las cadenas `".sdata2.lit%d"` y `".rodata.lit%d"`, pero ese camino
     esta muerto.)
  2. **Escribir la conversion a mano no vale, y el motivo es estructural.** Con
     una `union` el valor vive en un **pseudo DImode** que reload asigna a un
     PAR de GPR y guarda con un unico `movdf`: los dos `stw` salen SIEMPRE en
     orden de direccion creciente (msw primero) y el objetivo los tiene al reves
     (lo dice el volcado `-da`, fichero `.greg`). Haciendo el objeto
     **direccionable** (puntero o `volatile`) el orden sale bien, pero entonces
     se lleva la ranura BAJA del marco y empuja la union de `GET_FLOAT_WORD` a
     la alta — justo al reves que el objetivo. **Las dos mitades no se pueden
     tener a la vez.** (Regla de reparto, util aparte: **los objetos
     direccionables van al arena BAJO y las uniones de type-punning no
     direccionables al ALTO**, y dentro de cada arena manda el orden de
     aparicion.)
  3. Un `__asm__ __volatile__("" ::: "memory")` entre los dos stores **no** los
     ancla: la union no es direccionable, asi que el planificador no la ve
     alcanzable por el clobber y mueve los dos stores detras del `asm`.
  Unidades afectadas: `e_exp`, `e_pow`, `e_rem_pio2`, `ef_pow`, `ef_rem_pio2`,
  `k_rem_pio2`, `kf_rem_pio2`, `kf_tan`, `sf_exp`, `sf_expm1`, `sf_log`,
  `sf_log10`. **No las ataques por la fuente: el tope no esta ahi.**
- **`-fno-expensive-optimizations` CIERRA `sf_cos`, `sf_sin` Y `sf_tan` (568 B)
  Y ROMPE 14 UNIDADES DE LA MISMA BIBLIOTECA.** Con el flag las tres pasan de
  93,1/93,1/89,8% a **100%** (de 58 a 55 instrucciones y de 35 a 32); sin el, no
  hay forma de fuente que lo consiga. Pero el mismo flag tira `e_acos`, `e_sqrt`,
  `ef_atan2`, `ef_sqrt`, `math_support`, `mbtowc_r`, `memcmp`, `s_floor`,
  `s_scalbn`, `sf_asin`, `libgcc2_3/6/8/9`… de 100% a 0%. **Es `extra_cflags`
  POR OBJETO, nunca de la biblioteca.**
- **`while (n--)` PELADO PIERDE EL `mr` DEL CONTADOR; `while ((i = n--) != 0)`
  LO REPONE.** Si al objetivo le sobra `mr rTmp,rN` + `cmpwi rTmp,0` justo antes
  del `subi rN,rN,1` (o sea que el test usa una COPIA del valor viejo y no la
  variable), el fuente ligaba el post-decremento a una local ya muerta. Cinco
  formas medidas sobre `sn_fmod` (860 B): `while(n--)`, `while(n-- != 0)` y
  `for(n=…;n--;)` dan las tres 99,465%; `while(n-- > 0)` baja a 98,302%; solo
  `while ((i = n--) != 0)` da **100%**.
- **UNA CARGA QUE EL OBJETIVO EMITE ANTES DE UNA RAMA QUE NO PUEDE CRUZAR ES UN
  INICIALIZADOR ANTES DE LA GUARDA.** En `_mbtowc_r` el objetivo hace
  `lbz r9,0(r31)` **antes** del `beq cr4` que comprueba `s == NULL`; el
  planificador no puede subir una carga por encima de un salto, asi que la
  posicion es del FUENTE: newlib escribe `int char1 = *t;` **antes** de
  `if (s == NULL) return 0;` (deref de un puntero que puede ser nulo — es un
  quirk real de newlib). Mover las dos declaraciones cerro la funcion entera
  (**796 B**, de 97,99% a 100%).

- **El DWARF dice que inline llamo el ORIGINAL, no que el nuestro lo optimice
  igual.** El volcado pedia `GetPtrAtIndex` + `DeleteIndex`; reponerlos
  literalmente da **95,46%** porque el `ValidIndex` redundante no se colapsa como
  en el original, mientras el inline "incorrecto" da 99,65%. **Medir siempre
  antes de creerle al volcado.**
**Escribir desde cero rinde más que cosechar.** Medido: el permutador guiado
cerró **cero** en unidades con ceros, mientras escribir a mano rindió ~40 veces
más. La búsqueda automática sirve para **funciones ya escritas y atascadas por
encima del 99%**.

**UNA FUNCION DE 3.708 B ESCRITA DE CERO SALE A 926 DE 927 INSTRUCCIONES EN LA
PRIMERA COMPILACION, Y LOS DOS ULTIMOS CAMBIOS SON DE AMBITO.**
`CollisionGeometry::CreateJoint` (927 instrucciones, la mayor funcion a 0% de
zPhysics) se transcribio con `plan.py --dwarf-only` (arbol de bloques + locales
en orden de declaracion) mas `fuse.py` (la linea de fuente de cada instruccion,
que da los `if`, los `break` y los `continue`). Receta de colocacion: **el
fichero y el sitio salen de cruzar el `.s` del troceador con `debug_lines.txt`**
—por funcion, el rango de lineas del fichero de EA— y ahi CreateJoint aparecia
entre `Collections::Find(UCrc32)` (lineas 20-103) y `Collection::GetRoot`
(270-272). Lo que faltaba tras la primera compilacion:
1. **Los objetos `Vector3` gemelos de dos ramas `if/else if` son UNA sola
   declaracion en el cuerpo del bucle**, aunque el volcado los liste como dos
   locales homonimas en dos bloques distintos (comparten ranura). Con dos
   declaraciones el tamano sale 4 B corto y `&post` nace tarde; con una
   local con nombre EXTRA el tamano casa pero el marco crece 0x10. Lo que vale
   es **una sola**, y ademas hay que…
2. **…partir las declaraciones del cuerpo en DOS GRUPOS, uno por mitad.** El
   volcado las lista todas seguidas (`q, constraint_mat, constraint_dim, …,
   post_mat, post_dim, …`) y la lectura ingenua es declararlas juntas arriba;
   el original declara el primer grupo, escribe la primera mitad, y **declara
   el segundo grupo justo antes de la segunda mitad**. Eso conserva el orden
   del volcado y ademas hunde `post_mat`, que es lo que iza `&post` al
   preheader. **Con ese solo cambio: 97,94% -> 100%, 3.708/3.708 B, 0 diffs.**
   El permutador guiado, corriendo en paralelo, habia senalado la misma
   transformacion (`sink_decl:post_mat`) en su primera ronda.

**EL PERMUTADOR SIN `--guided` CIERRA LO QUE EL GUIADO NI SIQUIERA INTENTA.**
`FnPhaseChan::Eval` (644 B, 99,57%, **tamaño exacto**, y el diff entero era una
**permutación pura de f10/f11/f12**): `regmap` daba veredicto IDÉNTICO, así que
`--guided` generó **cero** variantes y salió en 3 s. Con el catálogo entero
(`--rounds 3 --limit 120`, 97 variantes de un solo cambio) la **23ª** dio
`*** IDENTICO ***`, y era `cse_temp` sobre `w`. En fuente limpia:

```c
frame = FloatToInt(w) / mSampleRate;
float t = (w - frame * mSampleRate) / mSampleRate;
w = t;                       // w no se vuelve a leer; el store muere
value->mAngle = a->GetAngle(frame);
if (frame < l / mSampleRate + 1)
    value->mAngle = (1.0f - t) * value->mAngle + t * a->GetAngle(frame + 1);
else
    value->mAngle = (t + 1.0f) * value->mAngle - t * a->GetAngle(frame - 1);
```

**644 B, verificado con objdiff: 100,0%, 0 diffs.** Dos lecciones: **cuando
`regmap` dice IDÉNTICO, `--guided` es inútil y hay que abrir el catálogo**; y una
**permutación pura de flotantes de *scratch* NO es techo** — es un rango de vida
que se parte con un temporal, aunque el volcado DWARF no liste ninguna local
nueva (el temporal se fusiona con la variable original y no crea entrada).
**Y el score de texto sigue siendo ruido**: en la misma tanda `init_decl:a_lp=0`
subió el score de `SphereVsBox` de 0,867 a 0,871 y con objdiff da **exactamente
el mismo binario**. Verifica siempre.

**La raíz importa más que el tamaño.** `missingcalls.py` antes de elegir qué
escribir: una clase escrita arrastró gratis 3.796 B de instanciaciones; una
familia entera de catorce funciones colgaba de un método que estaba fuera de
clase.

**Buscar hermanas ya casadas gana a descifrar desde el asm.**

**Media función no es neutra: RESTA.** Desplaza el inlining y baja funciones
vecinas ya casadas, y una cabecera tocada arrastra a unidades que nadie vigila.
Cerrar y medir cada función antes de pasar a la siguiente.

```
python scripts/build_direct.py [unidad]      # segundos; NO uses `python -m ninja`
./objdiff-cli-windows-x86_64.exe report generate -o build/GOWE69/report.json -f json
python scripts/unitmap.py --cmp <tu instantanea>
```
**Guarda tu propia instantánea del `report.json` al arrancar**: con varios
agentes el report se mueve. `build_direct.py` da `FAILED` transitorios por
contención de ficheros: recompila esa unidad sola antes de concluir nada.
Usa **tu propio** `.s`: `tools/scratch/out.s` es compartido y se pisa en
silencio. **Monta un micro-banco**: un `.cpp` de diez líneas con los cflags de la
unidad responde una pregunta de codegen en 1,5 s en vez de 30.

**RONDA zEcstasy/zWorld: EL MAPA COMPLETO DE `CarRenderInfo::Render` (11.132 B,
98,877%, tamano exacto, 128 diffs) Y CUATRO TECHOS MEDIDOS.**  *(`Render` ya
esta CERRADA al 100%: el diagnostico de abajo era correcto y las cinco causas se
resolvieron en la ronda siguiente; ver el bloque "RONDA zEcstasy/zWorld (r5)"
al final de esta seccion. Se conserva por el metodo, no como trabajo pendiente.)*

- **`Render`: las causas 1 y 2 son UNA SOLA — hay seis huecos de salvado (r14-r19)
  y siete candidatos a izar.** El objetivo iza `NISCopCarDoorClosedMarkers`(r14),
  `NISCopCarDoorOpenMarkers`(r15), `extra_render_flags`(r16), el contador del
  bucle(r17), `&blend_translation`(r18) y una copia de r24(r19). Nosotros izamos
  los mismos **menos ClosedMarkers y mas `&blend_quaternion`** (r17), y por eso
  rematerializamos ClosedMarkers dos veces (+4 insn) y gastamos un `addi`+dos
  `mr` en el quaternion (+1). `&blend_quaternion` se iza porque sus **dos** usos
  —el argumento de salida de `Slerp` y el `this` de `GetMatrix`— estan separados
  por dos `bl`, asi que el pseudo cruza llamadas; `&open_quaternion` y
  `&closed_quaternion`, con los usos juntos, se rematerializan solos en los dos
  lados. **Liberar ese salvado es el arreglo, no tocar los marcadores.**
- **`Render`: el pool de derrames de los CR es una ROTACION PURA DE 5 SOBRE 17
  RANURAS, y las 5 que rotan son exactamente las del PRIMER arbol de `if`.**
  Los 17 `cmpwi r27,K` izados al preencabezado se derraman con `mfcr`+`stw` a
  0x5c4..0x604. El conjunto y el orden de las comparaciones son **identicos**;
  lo unico que cambia es que al objetivo las cinco del primer arbol
  (24 FRONT_BRAKE, 34 REAR_BRAKE, 44 SPOILER, 69 LICENSE_PLATE, 66 FRONT_WHEEL)
  le tocan las ranuras **altas** (0x5f4..0x604) y a nosotros las **bajas**.
  Reload reparte las ranuras en orden de numero de pseudo, o sea de creacion en
  la fuente: al objetivo esos cinco CC nacen **detras** de los doce de los dos
  `switch` posteriores. Hay tambien un segundo pool de 3 ranuras
  (0x628/0x62c/0x630) rotado en 1.
- **`Render`: el arbol del `switch` esta INVERTIDO en el nodo `cmpwi 62`.** El
  objetivo emite `beq continue; bgt <rama de las ruedas>` y deja la rama de
  `SPOILER` de **caida**; nosotros `ble <SPOILER>` con las ruedas de caida.
  Nuestra fuente ya dice `if (part < CARSLOTID_HOOD) {spoiler} else {ruedas}`,
  que es la forma que deberia dar el orden del objetivo: lo invierte algo de
  `jump_optimize`, no la forma escrita.
- **`Render`: el objetivo conserva DOS puntos de entrada a `RenderPart`**
  (`lwz r0,0x5ac(r1)` + `mr r5,r30` con `b` a la cola, y `lwz r0,0x5ac(r1)` +
  `add r5,r25,r24` de caida) donde nosotros fundimos tambien el `lwz`. Es la
  regla de §6 «quien se funde»: la copia que sobrevive es la que **cae** a la
  junta. Vale 3 instrucciones, justo las que pagan el exceso de las causas 1-2.
- **La receta del `cr7` NO aplica en zEcstasy ni en zWorld (negativo medido).**
  Barridas las **40** funciones near-miss de las dos unidades contando diffs que
  toquen un campo de CR: **cero** tienen el campo equivocado. `Render` reproduce
  cr2, cr3, cr4, cr6 y cr7 exactamente; los unicos `cmpw crN` del objetivo que
  no producimos estan en `TestLoadStreamingEntry` (812 B, 87,7%, 67 diffs y
  tamano mal), donde falta mucho mas que el campo.
- **TECHO NUEVO: `GenerateHorizonFogDisplayList` (796 B, 98,995%, tamano exacto)
  es UNA TRANSPOSICION del planificador post-reload** — `andi. r11,r31,1` y
  `srawi r0,r0,1` intercambiados. `regmap` dice IDENTICO (27 locales, 0
  registros distintos). Medidas: subir el ternario `(i & 1) ? …` por encima del
  producto **91,23%**; subirlo ademas con `multiple` detras **91,48%**; sacar
  `i & 1` a un `int odd` **binario identico**; y **140 variantes** del catalogo
  completo del permutador sin una sola mejora. Mismo patron, misma cuenta, en
  `CarLoader::SetMemoryPoolSize` (304 B, 97,37%, tamano exacto): sobra que el
  `stw` del global se planifique **antes** del `addi rX,rX,@l` del literal.
- **TECHO NUEVO: `DrawAmbientShadow` (2.548 B) — una local de mas que el tamano
  confirma y que AUN ASI no se quita.** `regmap` da `scale_mult` como SOLO
  NUESTRA (se lleva f13, que en el original es de `scaleW`) y el tamano dice
  +4 B, o sea las dos condiciones que el playbook pedia para retirarla. Las dos
  formas de hacerlo empeoran: literal en los cuatro sitios **96,64% y +16 B**;
  plegarla en `scale` (`scale = 1.05f` / `heliScale * 1.05f`) **97,94% y -4 B**
  contra el 98,84% de partida. **Sexta confirmacion de la regla, y la primera
  con el tamano a favor.**
- **El volcado da `bool success` en `SetMemoryPoolSize` recogiendo el retorno de
  `TrackStreamer::MakeSpaceInPool`, y escribirlo da objeto BYTE A BYTE IDENTICO.**
  O sea que la palanca «ligar el valor de retorno descartado» **no siempre
  mueve nada**; el volcado lo pide, cuesta cero y no paga.

**RONDA zEcstasy/zWorld (r5): `CarRenderInfo::Render` Y `eDataRender::Render`
CERRADAS — +17.112 B y 2 funciones. Cinco patrones nuevos, todos medidos.**

- **EL ORDEN DE LOS GRUPOS DE `case` DECIDE LA POLARIDAD DE UN NODO INTERIOR DEL
  ARBOL, Y `default:` VA PRIMERO.** Micro-banco de siete formas sobre el mismo
  conjunto de `case` {24,33,34,42,44,62,66..69}: con el grupo que tiene cuerpo
  colocado **antes** de `default:` el nodo 62 emite `bgt <subarbol derecho>` y
  deja el izquierdo de **caida** (lo que hace el objetivo); colocado **detras**
  de `default:` emite `ble <izquierdo>` y saca el derecho de caida (+1 `b`). Y
  con el cuerpo escrito como **`goto` pelado** sale la forma mala aunque el
  grupo vaya delante. La combinacion que casa es `default:` **primero**, luego
  los `case` de `continue`, y al final el grupo con cuerpo y `break` — asi el
  cuerpo del `default` se emite pegado al arbol y el del ultimo grupo al final,
  que es exactamente el orden del objetivo. **Cambia 17 lineas de diff.**
- **UN `switch` CUYO `default:` CONTIENE EL CUERPO CREA SUS PSEUDOS DE
  COMPARACION AL FINAL, Y ESO ES LO QUE ORDENA EL POOL DE DERRAMES DE LOS CR.**
  `expand_end_case` genera el arbol de despacho **despues** de expandir los
  cuerpos y lo mueve delante con `reorder_insns`: los pseudos CC del despacho
  llevan por tanto numeros MAS ALTOS que todo lo que haya dentro del `switch`.
  En `CarRenderInfo::Render` las 17 comparaciones izadas se derraman en orden de
  numero de pseudo, y las cinco del primer arbol tocaban ranura baja en vez de
  alta: metiendo el resto del cuerpo dentro del `default:` de ese primer
  `switch`, las 17 ranuras casan de golpe (**58 -> 30 diffs**). Es el detector
  al reves de la entrada «el pool de derrames es una rotacion pura»: si las
  ranuras rotan por bloques, mira **que estructura envuelve** a las
  comparaciones, no la forma de cada una.
- **DECLARAR EL PUNTERO DENTRO DEL BUCLE —SOMBREANDO LA VARIABLE DE BLOQUE— LE
  DA EL PRIMER REGISTRO DE `REG_ALLOC_ORDER`.** Una variable de bloque
  (`unsigned char *mixed_index_08;` declarada arriba y asignada en cada `case`)
  se referencia en muchos bloques basicos, asi que la reparte `global_alloc`;
  escrita como `unsigned char *mixed_index_08 = &ib8[jj];` **dentro** del bucle
  pasa a ser cantidad de UN solo bloque, la coge `local_alloc` —que corre
  ANTES— y se lleva r11, que es el primero de `REG_ALLOC_ORDER` para GPR
  (11,10,9,8,...,3, luego 31..12). Dos declaraciones cambiadas cerraron
  `eDataRender::Render` (**5.980 B**, 20 diffs -> 0) sin mover una instruccion.
  **Delator**: el diff es un intercambio limpio de dos registros de *scratch* en
  bucles hermanos donde los demas `case` ya casan; el que deberia ir en r11 es
  el que en el objetivo NACE en el bloque.
- **`__builtin_psq_st(&x, v, 0, 0)` CONTRA `*(sn_ps *)&x = v` NO ES LO MISMO
  PARA EL PLANIFICADOR.** Con el cast, el planificador post-reload intercala la
  preparacion de los argumentos de la llamada siguiente entre los ocho `psq_st`;
  con el intrinseco los ocho stores salen seguidos y el `lwz` del global que
  alimenta el primer argumento se queda **detras**, como en el objetivo.
  `eDataRender::Render` 98,72% -> 99,93% (39 -> 20 diffs) con ocho lineas.
  El fichero ya usaba `__builtin_psq_st` en otro sitio: **si el TU mezcla las dos
  formas, la del objetivo es la del intrinseco.**
- **EL ENVOLTORIO `inline` LIBRE QUE NOMBRA EL VOLCADO LIBERA UN REGISTRO
  SALVADO.** El DWARF de `CarRenderInfo::Render` pedia
  `inline bQuaternion *bSlerp(bQuaternion *qdest, const bQuaternion *q1, const
  bQuaternion *q2, float t)` donde nosotros llamabamos al metodo
  `closed.Slerp(blend, open, t)`. Escribirlo (cuerpo: `q1->Slerp(*qdest, *q2,
  t);`) hace que `&blend_quaternion` se **rematerialice** en cada uso en vez de
  vivir en un salvado, y ese salvado pasa a `NISCopCarDoorClosedMarkers`, que es
  quien lo tiene en el objetivo. **98,88% -> 99,78%, de 128 a 58 diffs.** Es la
  entrada «un envoltorio inline materializa el argumento por referencia en un
  pseudo NUEVO» aplicada a un caso donde el sintoma era un **hueco de registro
  salvado**, no un `addi` de mas.
- **LA GUARDA CONTRA EL `else` PARA UN `return` DENTRO DE UN BUCLE VALE UNA
  INSTRUCCION.** `if (A) { <bloque grande> } else { f(); return 1; }` emite el
  bloque del `else` detras del grande **mas un `b` para saltarlo**; escrito
  `if (!A) { f(); return 1; } <bloque grande>` el bloque acaba en `blr`/salto
  propio y el `b` desaparece. `CompositeSkin(RideInfo *)` paso de +4 B a
  **tamano exacto** (18 -> 16 diffs). **El delator es el mapa de lineas**: el
  volcado situaba la llamada 34 lineas despues del `return 0` y nosotros 69.
- **UN `case` PUEDE CAER EN EL CUERPO DEL `default`.** En `Render` el `break` de
  `case CARSLOTID_HOOD` saltaba en el objetivo a la instruccion `clrlwi` del
  `default:` (`draw_part &= 1`) y en el nuestro por encima de ella: el `case`
  del original **tambien hace `draw_part &= 1`**, y su cola se cross-jumpea con
  la del `default`. Escribirlo cerro los dos ultimos saltos. Delator: el destino
  del `break` de un `case` cae **una instruccion antes** que el nuestro.
- **VEDAS DE zEcstasy/zWorld, CON EL PORCENTAJE DE HOY (auditoria 2-sep): LAS
  CUATRO SIGUEN ABIERTAS** — `BlendVisualLookAttribute` 95,97%,
  `RenderFlaresOnCar` 98,06%, `CullParts` 99,33%, `UpdateParticles` 99,96% (subio
  desde el 99,571% que dice el texto; los 31 diffs de abajo son de entonces).
  **La unica que se movio de esta lista es `TestLoadStreamingEntry`, que el
  bloque de arriba daba por muy lejos (812 B, 87,7%, 67 diffs, tamano mal) y hoy
  casa al 100%: cuando el diagnostico dice «falta mucho mas que el campo», falta
  CODIGO — y el codigo se escribe.**
  `IVisualTreatment::BlendVisualLookAttribute(bMatrix4&,...)` (456 B, 95,97%,
  tamano exacto): el objetivo guarda el `0.0f` en un salvado (f29, tres
  psq_st) y nosotros lo rematerializamos; barridas **las 8 combinaciones** de
  literal contra `extern const float lbl_8040D00C` en los tres sitios, las ocho
  dan **el mismo binario**, y ligarlo a un `float zero` local baja a 92,28%.
  `CarRenderInfo::RenderFlaresOnCar` (2.908 B): el objetivo iza un
  `lis lbl_8040AD04@ha` a r16 en el prologo y nosotros no; **14 variantes** de
  conversion a los simbolos agrupados (los 10 sitios `= 0.0f` de uno en uno,
  los tres de dentro del bucle, y los 13 juntos) — el mejor sigue siendo
  `intensity = 0.0f` + `sizescale = lbl_8040ACF0`, con **tamano exacto** y 62
  diffs; convertirlos todos sube a 125. `CarPartCuller::CullParts` (836 B, 9
  diffs, -4 B): al objetivo le sobra un `mr r9,r0` (el `debug_print =
  NumPlanes` izado al preencabezado por `move_movables` como *movable* pareado);
  las tres posiciones de esa asignacion —cabeza del bucle, antes del bucle, y
  donde esta— dan 59, 62 y 9 diffs: la instruccion aparece pero **con un allocno
  de mas** (`stmw r14` contra `stmw r15`). `EmitterSystem::UpdateParticles`
  (1.820 B, 31 diffs): quitar `rot_scale` —que `regmap` da como SOLO NUESTRA—
  sube a 42 diffs (**septima confirmacion**), y calcularlo antes de `pangle`, a
  33.

**RONDA zEAXSound/zEAXSound2 (r4): +2.256 B y 8 funciones. Seis patrones nuevos,
todos medidos, y la lectura de los DOS barridos que se daban por agotados.**

- **EL BARRIDO DE LITERALES HAY QUE HACERLO POSICION A POSICION, NO POR
  MULTICONJUNTO: `litcheck.py` NO PUEDE VER UN INTERCAMBIO.** Si dos constantes
  del pool estan **cambiadas entre si** el multiconjunto es identico en los dos
  lados y `litcheck` calla; con `function_reloc_diffs=none` el porcentaje
  tampoco lo ve, asi que la funcion mide **100%** con el DOL mal. Se caza
  emparejando **instruccion a instruccion** las referencias `lbl_XXXXXXXX@l` del
  objetivo con nuestros `$LCn@l` y leyendo los bytes (el ELF por direccion,
  nuestro `.o` por simbolo). Acierto: `SFXCTL_AIPhysics::UpdateRPM` tenia
  `smooth(rpm, 8000.0f, dt)` donde el original pone **4000.0f** y
  `PhysicsRPM = 4000.0f` donde pone **8000.0f** — dos literales cruzados en una
  funcion que ya media 100%. **Dos filtros obligatorios o te ahogas en falsos
  positivos**: (1) comparar solo ranuras que objdiff marque SIN diferencia (si
  hay `INSERT`/`DELETE` la correspondencia por indice no vale), y (2) **saltar
  los `addi rX,rY,$LC@l`**: en un `.o` una entrada de pool que es una DIRECCION
  vale 0 y lleva reubicacion, mientras el ELF ya la tiene resuelta.
  Barrido completo de las dos unidades: 643 + 574 referencias alineadas, **un
  solo acierto**. Queda como **`scripts/litpos.py <unidad>`**, complemento de
  `litcheck.py`: pasa los dos, no uno.
- **EL BARRIDO DE ACCESORES VACIOS SIGUE AGOTADO AQUI, Y AHORA CON LOS FPR
  INCLUIDOS.** `deadreg.py` ya excluye solo `f1-f8`, asi que su barrido cubre
  `f9-f31`: sobre las **69 funciones sub-100%** de zEAXSound + zEAXSound2 da
  **cero**; con `--all` (las 1.641 al 100% incluidas) sale **una** y es un falso
  positivo (`CSIS_UpdateHOnk`, ya al 100%). El barrido **por FIRMA** —no por
  nombre— da 54 accesores no-`void` con cuerpo `{}` en 13 cabeceras de
  `Src/EAXSound`, y cruzados contra los `.cpp`: los `operator new`/`delete` de
  `EvtSystems`/`BankSlotSystem`/`SndDataParams` **no los usa nadie** (nadie hace
  `new` de esas clases), `EAX_CarState::GetShiftUpRPM/GetShiftDownRPM/GetCarType`
  **no tienen llamante en EAXSound**, y el unico que si se llama (`GetYaw()` en
  `CARSFX_Skids`) esta en una funcion que ya mide 100%. **Veta cerrada con
  medida en las dos unidades.**
- **UN LOCAL QUE GUARDA LA CONSTANTE ROMPE LA CANONICALIZACION `x>=C` ->
  `x>C-1`.** *(Esta fue la medida que desmintio el «solo puede venir de un
  `switch`»; la entrada consolidada con los tres origenes esta en §7.)* Vale
  tambien cuando la
  comparacion se materializa como VALOR. `return GetCurGear() >= SECOND_GEAR;`
  da siempre `cmpwi 2` + `mfcr` + `extrwi` (probadas siete formas: `!(<)`,
  operandos invertidos, `> FIRST_GEAR`, casts, ternario, `if/return`); con la
  constante en un local —`Gear second = SECOND_GEAR; return ... >= second;`— sale
  el `cmpwi 3` + `cror un,eq,gt` + `clrlwi 31` del objetivo. El motivo es que en
  *expand* el operando es un REG y no un `CONST_INT`, asi que
  `simplify_relational_operation` no lo toca, y CSE mete el 3 despues.
  `SFXCTL_AccelTrans::ShouldPlayEngOffSweet` **116 B a la primera**. Es la misma
  familia que la nota de `bMax(const,const)` («el dial no esta en el cuerpo:
  `_c(a)` con `int a = 255` SI conserva la rama»).
- **El `case` con cuerpo solo `break;`/`return;`: vease la entrada consolidada de
  §6 («sobra cuando FUNDE dos nodos y hace falta cuando los SEPARA»), que ya
  recoge las dos medidas de esta ronda (`SetSFXBaseObject` 90,61 -> 100% y
  `UpdateTRQ` 89,52 -> 100%).
- **REUSAR LA MISMA VARIABLE PARA EL VALOR LEIDO Y PARA EL RESULTADO ES LO QUE
  HACE QUE GCC REAPROVECHE `r0`.** `SFXCTL_Tunnel::UpdateCityVerb` (296 B) tenia
  `int zone; if (GetSndGameMode() == SND_FRONTEND) zone = 0; else zone = 9;` y
  emitia `li r10,0x9` **izado por delante** del `lwz` del modo; el objetivo pone
  `li r0,0x9` **detras** del `cmpwi`, reutilizando el registro del valor que
  acaba de morir. La forma que lo reproduce es
  `int zone = GetSndGameMode(); if (zone == SND_FRONTEND) zone = 0; else zone = 9;`
  — **una sola variable para las dos cosas**: 97,16% -> **100%**. Descartadas
  antes (mismo binario o peor): `int zone = 9;` + `if` que lo pisa, el ternario,
  la polaridad invertida, y escribir el miembro directamente.
- **UN CONSTRUCTOR POR DEFECTO QUE SOLO INICIALIZA PARTE DE LOS MIEMBROS ES
  CORRECTO: EL SITIO QUE LOS QUIERE TODOS USA EL CTOR DE N ARGUMENTOS.**
  `SND_Params()` inicializa **solo** `Vol` y `Pitch` (el `.hpp` lo marcaba con un
  `// TODO it's sus`), y `cStichWrapper::cStichWrapper` necesitaba los seis.
  Completar el ctor por defecto **cierra esa funcion (88 B) y tira 3.132 B en
  SEIS de zEAXSound2** (`CARSFX_PreColWoosh`, `BottomOut`, `TrafficWoosh`,
  `SFXObj_Collision::ProcessUpdate`): medido en las dos direcciones. La salida es
  el **sitio de llamada**: `: SndParams(0, 0x7FFF, 0x1000, 0, 0, 0)` da el mismo
  100% con coste cero fuera. Corolario de metodo: **un cambio en cabecera se mide
  con las dos unidades enteras, y si pierde, la respuesta esta en el llamante**.
- **`Attrib::StringKey::GetString()` YA DEVUELVE `""`: LA GUARDA DE NULO DEL
  LLAMANTE ES CODIGO DE MAS.** `CollisionEvent::InitAsScrape` tenia
  `const char *s = key.GetString(); if (!s) s = "";` y emitia **dos** veces el
  idiom `cmpwi; bne; lis/addi ""` (16 B de mas). Borrar la guarda: 91,43% ->
  **100%** (196 B). Es un `grep` sobre `GetString()` seguido de un test de nulo.
- **EL LOCAL QUE HACE VIVIR UN MIEMBRO A TRAVES DE LAS LLAMADAS SE VE EN QUE
  CAMPO SE RELEE.** `CollisionEvent::CollisionEvent` (824 B, 97,13%) leia
  `this->mActor` (0xdc) y `this->mActee` (0xe0) despues del bucle de
  `DESCRIPTION`; el objetivo tiene el actor en un **salvado** desde el principio
  y relee `mParams.other_object` (0x48), **no** `mActee`. O sea: un local
  `unsigned int actor = this->mActor;` al entrar en el cuerpo (CSE lo funde con
  el store de la lista de inicializacion, asi que cuesta cero) mas
  `IsPrimaryTarget(actor) || IsPrimaryTarget(this->mParams.other_object)`.
  99,97% con 2 diffs; el ultimo paso fue `if (impact == true)` en vez de
  `if (impact)` (`cmpwi 1; bne` contra `cmpwi 0; beq`). **100%.**
  **El delator general: cuando dos campos guardan el MISMO valor (uno copia del
  otro), mira de cual de los dos lee el objetivo — eso fija la fuente.**
- **VEDAS DE zEAXSound/zEAXSound2, CON EL PORCENTAJE DE HOY (auditoria 2-sep).
  UNA ESTABA ROTA Y LAS DEMAS SIGUEN EN PIE**: `InitSFX` 99,97%,
  **`HandlePacketRelease` 100% — VEDA ROTA**, `cStichWrapper::Play` 99,69%,
  `ShouldBeginAccelTrans_Idle` 92,36%, `inbetween` 87,50%,
  `CreateSubMixChannels` 98,69%, `CreateMixCtls` 97,32%, `MsgBarrier` 94,29%,
  `CARSFX_Turbo::ProcessUpdate` 97,50%, `TestToLicensed` 98,26%.
  **`GinsuSynthesis::HandlePacketRelease` (1.936 B) cayo con el PERMUTADOR
  GUIADO por funcion, sin `--timeout` y con `-j 8`: un `float x = 0.0f;` en la
  variante ~50 de 198.** Las «siete formas de declarar `blend`/`blendstep`» que
  se habian probado a mano eran siete de las 198 — **una veda de siete formas no
  es una veda; es un barrido a medias**.
  `SFXObj_Collision::InitSFX` (1.304 B, **2 diffs, tamano exacto**): solo cambia
  el registro del `li 0xd` (r10 en el objetivo, r11 en el nuestro) de la tirada
  `AzimSlot/PitchSlot/ReverbSlot`; barridas las **6 permutaciones** de las tres
  sentencias, **6 colocaciones** del grupo respecto a los cuatro
  `Assign*Vector`, un store muerto de `VolSlot` delante, y **120 variantes del
  permutador** (catalogo de un solo cambio): ninguna mueve el registro.
  `GinsuSynthesis::HandlePacketRelease` (1.936 B, 14 diffs, tamano exacto): el
  objetivo reutiliza **un solo** registro base para dos entradas de pool
  consecutivas (`lis r9` dos veces en serie) y a nosotros el planificador nos
  solapa las dos `lis` y gasta r9 **y** r11; **siete formas** de declarar
  `blend`/`blendstep` dan **binario identico**.
  `cStichWrapper::Play` (384 B, 4 diffs, tamano exacto): permutacion r30/r31
  entre el indice `i*4` y la base `&ActiveSamplesRefs`; cinco formas del bucle
  (indice sin signo, contador fuera, puntero a la base, local para el `new`,
  orden de los campos de `SampleQueueItem`) no la mueven.
  `SFXCTL_AccelTrans::ShouldBeginAccelTrans_Idle` (280 B, 11 diffs, -4 B): el
  objetivo emite `cror un,eq,lt`+`bso` para `GetPhysRPM() > 1500.0f` con el
  `return false` **de caida y en medio de la funcion**, y a nosotros
  `jump_optimize` nos lo invierte a un `bgt` al bloque compartido; siete formas
  (guarda propia, `if/else`, `!(>)`, local para el RPM, guarda fuera del `&&`)
  dan el mismo binario.
  `inbetween` de `realstream` (64 B, 2 diffs, tamano exacto): la posicion del
  `li r3,0` compartido; cinco formas del `if` anidado no la cambian.
  `NFSMixMapState::CreateSubMixChannels` (336 B) y `CreateMixCtls` (448 B): pura
  permutacion r0/r9/r10/r11 mas el sitio del incremento del `for`; siete formas
  (orden de las dos primeras sentencias, indexado contra `ptr + n`, base en un
  local, reutilizar `offset` para el cero) — la mejor baja de 16 a 15 diffs.
  `CARSFX_PreColWoosh::MsgBarrier` (140 B, 2 diffs): el `li r4,0x1` del argumento
  cambia de sitio con un `stfs`; cinco ordenes de las tres sentencias, ninguno.
  `CARSFX_Turbo::ProcessUpdate` (672 B, 8 diffs, tamano exacto) y
  `SFXObj_PFEATrax::TestToLicensed` (876 B, 8 diffs): cuatro formas cada uno.

**Nunca**: commit, push, ni tocar los bloques `#if defined(__ANDROID__)` de
`Support/Utility/UVectorMath.hpp` (tiene 2 y debe seguir teniendo 2). Deja el
árbol compilando. **Si usas un tipo, comprueba que su cabecera está incluida.**

---

## 11. Ronda 10 — zSpeech/zWorld/zWorld2

### El `extsh` que delata un `++` donde escribimos `= true`

`short pass = false; … pass = true;` **se pliega siempre a `li rX,1`**. Si el
objetivo emite en su lugar **`li rX,1` izado fuera del bucle + `extsh rDst,rX`**
en cada sitio, el RHS del original **no es una constante literal**: es una
expresión entera que la propagación de constantes (`-fgcse`) reduce a 1 **pero
que ya había generado la conversión int→short**. La forma que casa es `pass++`:

- `(set tmp (plus (reg pass) 1))` con `pass` probado 0 → cprop lo vuelve
  `(set tmp (const_int 1))`, que `loop.c` (`-fmove-all-movables`) **iza fuera
  del bucle** como invariante;
- la asignación a la `short` sigue siendo `(sign_extend (subreg:HI tmp))` →
  **`extsh` sobrevive**.

Medido en `Speech::Manager::PostValidate` (2.236 B): `pass = true` → `pass++`
subió 96,74 → **98,02%** y el tamaño pasó de 2.228 a 2.236 B **exactos**.
El mismo mecanismo explica `li r8,1; slwi r8,r8,2` donde nosotros ponemos
`li r10,4` (índice sin plegar) — ahí `++` **no** basta (medido en
`CarRenderInfo::UpdateLightStateTextures`, 98,63% sin cambio).

### El ORDEN de las expansiones inline del DWARF sitúa la sentencia

El volcado no sólo lista locales: lista **cada inline expandido, en orden de
fuente**. En `PostValidate`, el bloque del bucle traía
`SpeechID ×3` … `Timer::operator-` … `GetSeconds`, o sea las **tres** llamadas
antes de la resta. Nuestro `Timer elapsed = WorldTimer - GetTime(...)` hacía la
resta en la primera línea; el original es

```c
Timer elapsed = mGlobalHistory.GetTime(dependency.SpeechID());
int  count    = mGlobalHistory.GetCount(dependency.SpeechID());
bool playing  = IsCopSpeechPlaying(dependency.SpeechID());
float t       = (WorldTimer - elapsed).GetSeconds();
```

Con eso **PostValidate cerró al 100%** (+2.236 B). Regla: cuando el diff mueve
un `subf`/`lwz` unas ranuras, **mira el orden de los inlines antes que el
planificador**.

### Un `{}` con locales propias que el DWARF declara DOS veces

`Manager::ScheduleSpeechPartII` declaraba `eventkey`/`eventID` a nivel de
función y los usaba en las dos ramas; el DWARF los declara **otra vez dentro
del `if`**, y ahí viven en `r4` (efímero) en vez de en una callee-saved: por eso
el original hace `mr r4,r3` donde nosotros `mr r29,r3` + `mr r4,r29`.
Además el `else` **no existe**: es un `return 0;` temprano, y por eso
`event_atr`/`event` salen a nivel de función y `pool` no existe (el original
escribe `gSpeechCache.GetEventPool()->IsFull()`). Y el `if (interrupt())` va
con la rama del `+100` **primero**. Todo junto: 95,70 → **96,76%** y el tamaño
de 1.488 a 1.472 B **exactos**.
**Cómo se lee**: locales de función en el DWARF que en tu fuente están dentro
del `else` ⇒ el original **no tiene `else`**, tiene `return` temprano.

### `__asm__("# id")` corta el cross-jumping en LOS DOS sentidos

- **Sobramos código compartido** (el objetivo duplica): un `__asm__` con
  **cadena única** justo después de la llamada, DENTRO de la rama, rompe el
  emparejamiento de colas. `RoadblockFlow::Setup` 92,52 → **98,26%** con tres
  barreras (`# a`, `# b`, `# c`) y tamaño exacto 596 B.
- **Faltamos código** (nosotros compartimos y el objetivo no): pasa cuando dos
  ramas quedan **idénticas** al arreglar una de ellas. En
  `StrategyFlow::ReqBackup`, quitar el local `flags` (el DWARF no lo tiene) hizo
  las dos ramas idénticas y GCC fusionó **29 instrucciones** (1.704 → 1.596 B,
  92,07%). Una sola barrera detrás del `NegativeBackupReply()` lo deshizo:
  **98,84%** y 1.712 B. **Corolario: quitar un local sobrante puede DESTAPAR un
  cross-jump; hay que medir el tamaño, no sólo el porcentaje.**

### El orden de los `addi` izados delata el orden de las sentencias del bucle

Los `addi rN, this, off` que `-fmove-all-movables` saca de un bucle se numeran
por **orden de primer uso dentro del cuerpo**. En `SoundAI::SoundAI` el objetivo
izaba `0x1ac, 0x1bc, 0x1b0, 0x1c0` y nosotros `0x1ac, 0x1b0, 0x1bc, 0x1c0`: la
fuente original intercala los dos arrays (`mPlayerCurrent[i].direction`,
`mAICurrent[i].direction`, `mPlayerCurrent[i].roadID`, …). Reordenar casó los
cuatro `addi` (el orden de los `stwx` sigue siendo del planificador).

### Vedas medidas en esta ronda

| cambio | antes | después |
|---|---|---|
| `Cache::Init`: quitar el temporal `speech_pool` (el DWARF NO lo declara) | 96,98% | **96,58%** |
| `Cache::Init`: asignación anidada en la llamada | 96,98% | **96,58%** |
| `HolePunchAvoidables`: quitar `nav_cross` (el DWARF NO lo declara) | 95,28% | **95,19%** |
| `Assess180`: `if (!cop) return;` → `if (cop) {…}` | 98,96% | **96,59%** |
| `UpdateLightStateTextures`: `left_light_state++` | 98,63% | 98,63% |
| `Setup__RoadblockFlow`: `__asm__` ANTES de la llamada del 4º sitio | 98,26% | 98,26% |

### Diagnosticado y SIN resolver: el orden pfn/delta de la llamada virtual

La expansión de `obj->Virtual()` tiene dos formas y **las dos existen en el
mismo binario** (en zSpeech: 193 y 375 en el objetivo, 190 y 375 en el nuestro):

```
DELTA:  lwz r9,0(rX) ; lha r3,D(r9)   ; lwz r0,D+4(r9) ; add r3,rX,r3 ; mtlr r0 ; blrl
PFN:    lwz r9,0(rX) ; lwz r0,D+4(r9) ; lha r3,D(r9)   ; mtlr r0 ; add r3,rX,r3 ; blrl
```

**Sólo tres sitios de zSpeech eligen distinto** que el objetivo, y son los tres
diffs que quedan en `Observer::Assess180` (1.004 B, 98,96%),
`RoadblockFlow::Setup` (596 B, 98,26%) y el último de
`StrategyFlow::Waiting` (1.904 B). Descartado: no es la última llamada del
cuerpo (en el objetivo la última es P 45 veces y D 102), no es el registro base
ni si es callee-saved, no es lo que sigue al `blrl`, y **`__asm__` delante de la
llamada no lo mueve**. Vale ~3.500 B en zSpeech si alguien lo resuelve.

### 2026-09-08: una ranura explicita rompe el muro de conversiones de MWCC

`VDevice_RecalcGammaTable` cerrada: **216 B, 54/54 instrucciones, marco 0x20**.
El diagnostico antiguo era correcto para las conversiones C implicitas: MWCC
les asigna tres ranuras distintas. No demuestra que el almacenamiento explicito
de fuente tampoco pueda compartirlas.

Una union de 8 B reutilizada por las dos conversiones entero->float y por el
resultado de `fctiwz` reproduce la ranura unica del objetivo. La union sola
emite `fsub; frsp` donde el objetivo usa `fsubs`: dos instrucciones MW inline
ASM `fsubs` y una `fctiwz` completan el cierre, con operandos C `register`
inicializados y sin fijar registros fisicos. El resto de la funcion sigue en C.
El orden de escribir la palabra baja y luego la alta decide el ultimo empate
r0/r3. Ver `docs/analisis/resume-20260908-steering.md` y su gate de unidad.

Limite importante: **MWCC tambien optimiza las copias dentro del ASM**.
`asm { mr out, mag }` en `Effect_PerformEnvelope` se fusiona igual que la
asignacion C: 192/196 B, 93,265305 %, identica a la base. Una instruccion ASM
no garantiza por si sola ni dos pseudos distintos ni la conservacion del `mr`.

### 2026-09-08, iter2: la mitad alta de un global si puede tener productor

`CompletePCreadAsync` cerrada: **388 B, 97 instrucciones; FSasync 13/13 exactas**.
La veda de R46 era valida para barreras sobre valores C y punteros completos,
pero no para un productor real `lis %0,global@ha` con salida inicializada.
Accesos inline `lwz/stw global@l(base)` conservan esa mitad alta sin introducir
el `addi` de un puntero completo. En esta funcion dos bases en r30 y recargas
de condicion en r0 reproducen los rangos del objetivo; el resto queda en C.

Los accesos llevan clobber `memory` y el global es volatile. Poner un operando
`m` o `X` adicional sobre la lvalue fue contraproducente: reload materializa
otras bases y sube a 404 B. Se validaron las 2 relocaciones HA y 12 LO contra
la direccion original, las trece funciones y el DOL completo. Esto no autoriza
usar valores sin inicializar ni convertir toda una funcion en ASM.

Limite: requiere un simbolo con identidad estable. No copiar la receta con
un ordinal automatico `$LC917` para un literal: su nombre puede cambiar al
editar otra parte de la unidad. `UpdateLoaded` queda fuera de este cierre.
Ver `docs/analisis/iter2-20260908-fsasync.md` y
`docs/analisis/iter2-20260908-fs-link.md`.

### 2026-09-08, iter3: un productor MW puede devolver r0 al store C

`__VMBASESetupExceptionHandlers` cerrada: **380 B, 95 instrucciones, vmbase
30/30 exactas**. Las 21 formas de fuente agotadas no cubrian productores MW
reales. Para los saltos de entrada, `asm { oris r0, branchN, 0x4800;
mr instruction, r0 }` fija el papel del temporal y devuelve un valor definido
a la variable C register `instruction`. MW fusiona el mr y conserva los
stores C y su informacion de direccion. Para los dos saltos de retorno,
`neg; clrlwi; oris; stw` expresa la cadena real con branchN y p definidos.

El mismo coalescing que impidio mantener un mr extra en PerformEnvelope aqui
es necesario: no basta que el ASM escriba la instruccion deseada, hay que
medir lo que MW conserva. Hacer los cuatro stores en ASM con otra base C
definida anadio dos lis (388 B/95,89474 %) y se descarto. Solo se retiene la
version100; datos, raw relocs y bytes de las otras29 funciones identicos.
Ver `docs/analisis/iter3-20260908-vmbase.md`.

### 2026-09-08, iter3: conservar una copia MW sin reservar un hardreg

`__VMAllocVirtualToARAMLUT` cerrada: **168 B, vm18/18 exactas**. La secuencia
real `mr saved_base,base; addis base,saved_base,1; stw saved_base,global`
mantiene la copia porque el store necesita la base vieja despues de producir
la nueva. Ambos son operandos C register definidos; no hace falta fijar r4.
Usar r4 fisico cerraba la cabecera pero desplazaba los registros del bucle:
168 B/95,95238 %. Dejar que MW lo asigne/libere conserva el mr y cierra todo.
No contradice el coalescing anterior: importa que exista un uso real de los
dos valores distintos y solapados. Ver `docs/analisis/iter3-20260908-vm.md`.

### 2026-09-08, iter4: el ownership original decide una promocion linked

DebuggerDriver no era una TU con un compilador casi correcto: los marcadores
STT_FILE originales prueban tres propietarios distintos. Separarlos conserva
los cuerpos C y permite seleccionar GC/1.2.5n para DB y GC/1.2.5 para las dos
Amc. Resultado integrado: 31/31, +416 B exactos y +5988 B linked. No basta
partir .text: cada propietario debe recibir sus .sdata/.sbss/.data/.bss y
sus valores iniciales, respetando el padding de alineacion entre objetos.
Los dos Ecb LOCAL (192 B EXIBios y 24 B AmcExi) no se pueden compartir.

vmbase ilustra el caso complementario: sus 28 B sbss no se pueden reabsorber
sin mover otras direcciones porque el orden de texto y datos es distinto.
Usar los siete aliases ya exportados por su auto-objeto permite enlazar el
codigo exacto manteniendo intacto el almacenamiento. Solo es valido tras
verificar identidad, binding/ABI, relocaciones y DOL; no generalizar a aliases
inventados ni a ordinales inestables de literales.

La promocion exige el grafo real: verificar que desaparecen de la lista los
autos absorbidos, no borrar sus ficheros como sustituto de una reextraccion.
Guardar baselines inmutables antes de modificar configure/splits; los gates
anteriores pasan a historicos, no se actualizan sus hashes para simular PASS.
Ver `docs/analisis/iter4-20260908-cierre.md` y sus dos gates posintegracion.

### 2026-09-08, iter5: VM demuestra por que recuperar datos exige revisar el orden

El monolito vm agrupaba VM.c, VMPageReplacement.c y VMMapping.c. Repartir sus
18 funciones exactas y el estado segun STT_FILE permite +2304 B linked y
+176 B de datos matched/linked, con DOL original. Las fronteras de texto se
contrastan con funciones/usos; STT_FILE prueba ownership de los locales, no
por si solo un intervalo de codigo.

MW invierte el orden declarativo de globals .sbss, pero mantiene el de .sdata.
La politica de reemplazo inicial es 1, no cero. El estatico inerte original
nextPageToCheck conserva sus 4 B reales y cero referencias; el siguiente
cursor ARAM conserva LOCAL, cero-init, .sbss+12 y sus siete usos de VMAlloc.
Subir esa declaracion de block-static a file-static reproduce almacenamiento
y comportamiento; no prueba el ambito lexico ni el sufijo privado original.

El split recorta tres autos y crea una cola de datos: verificar el grafo
generado completo (616 -> 619), no limitarse al numero de TUs Matching. La
promocion no aporta funciones nuevas si sus cuerpos ya estaban al 100 %.
Ver `docs/analisis/iter5-20260908-vm.md` y los gates POST/snapshots actuales.

### 2026-09-08, iter6: cruzar un umbral de referencias sin mover los argumentos

`SetupNextLoad` llega en sombra privada a **1008 B / 100 %**. La corrección
de CFG de DeleteRefToAsset deja un ciclo de tres registros. El RTL muestra
que `this` tiene 12 referencias y debe llegar a 16 para cruzar el siguiente
escalón de floor_log2 en la prioridad de global_alloc.

Cuatro entradas adicionales reales de `this` consiguen el reparto, pero las
dependencias sobre nextBankIndex y currentLoad alteran instrucciones ajenas.
Dos asm vacíos separados y encadenados sobre endIndex y otros dos sobre
bankIndex conservan las cuatro referencias sin esa deuda. Ambos valores
están inicializados y se consumen después. Repetir una entrada dentro del
mismo asm no equivale a añadir dos instrucciones RTL separadas.

Medir a la vez prioridad, asignación y scheduling: cerrar el ciclo no basta
si el nuevo sitio de la dependencia desplaza cargas o argumentos. No se
necesitan operandos indefinidos ni pines. La receta requiere también el
reinicio correcto sobre el mismo SndBase tras eliminar una referencia.

Estado actualizado: **integrado con autorización de cabecera**, +1008 B y
una función exacta. La compilación real conserva todos los otros 996 cuerpos
de zEAXSound, sus datos/ctors y el objeto zEAXSound2 entero (1112 funciones).
Las 43 relocs del cuerpo real se aplican antes de exigir sus 1008 bytes
idénticos al ELF original. Ver `docs/analisis/iter6-20260908-setupload-refs.md`
y el gate POST de `codex_20260908_iter6_setupload_integration`.

### 2026-09-08, iter6: retener el constructor no retiene su entrada

eathread ya tenía 84 B exactos, pero el storage sintético de fuente impedía
enlazar. La auditoría del grafo probó que las tres definiciones ausentes del
original no tienen usuarios; el helper C conserva el efecto del constructor
sin crear ese BSS. Aun así, el linker elimina el wrapper de nombre LOCAL
distinto al generado originalmente.

Una regla keep calificada por objeto conserva los 84 B en sus direcciones
correctas, pero no su entrada `.ctors`, un símbolo independiente. Retener
también esa entrada cierra el DOL entero: **+84 B código linked, +4 B datos
linked**, sin cambiar splits, flags ni identidad/binding de las funciones.
El mapping de objdiff asocia el nombre LOCAL distinto solo después de probar
los bytes y destinos, y conserva los 84 B matched ya existentes.

No exigir que un nombre privado de compilador sea idéntico si la equivalencia
de código, binding, layout, orden de inicialización y DOL completo ya está
demostrada. Tampoco asumir que keep de una función conserva referencias
entrantes desde datos: la dirección de la dependencia importa.
Ver `docs/analisis/iter6-20260908-eathread-linked.md` y su gate POST.
