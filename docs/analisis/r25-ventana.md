# Ronda 25 — ventana: lo libre, mientras trabajaban los ocho agentes

## 0. Qué estaba libre, y por qué nadie lo veía

Los ocho encargos cubren **82.372 B en 13 unidades**. Quedaban **20.192 B en 27
unidades** sin agente, y no aparecían en ningún reparto por una razón concreta:

**`triage.py` sólo barre las SourceLists por defecto.** Las 18 unidades de
middleware con pérdida —13.268 B en 38 funciones— no salen en su salida, así que
llevaban rondas sin que nadie las mirara.

Y hay una trampa de lectura encima: `sfir` y `srender` figuran al **0,0000 %** en
`report.json`. No es código sin escribir —`censo.py` dice cero— sino la regla de
**todo-o-nada**: sus funciones están al 89,25 % y al 93,30 %, y una función que
no llega al 100 % aporta **cero** bytes casados.

## 1. NOVENO caso del patrón «la herramienta falla en silencio», y era mío

Escribí un barrido para sacar el % por función de las 19 unidades libres. Dio
**cero funciones en las diecinueve**. No dio un dato malo: dio **menos datos de
los que hay**, que es lo que hace caro este patrón.

La causa: en el JSON de objdiff **los símbolos NO cuelgan de las secciones**, van
planos en `left.symbols` con `kind: SYMBOL_FUNCTION`, y **el tamaño viene como
cadena**. Lo cacé sólo porque tenía la verdad de `sfir` (89,25 %) de una tirada
directa anterior — es decir, por la regla de validar la métrica contra un
diagnóstico ya conocido antes de repartir con ella.

## 2. CERRADA: `FEMessageResponse::FindConditionBranchTarget` (zFEng)

**144 B, 65,27778 % → 100 %. La unidad entera pasa a 100 % (71.460 B).**
`audit.py`: **343 ok, 0 FALLA**; la función audita entera (144 B, 12 ramas,
12 reubicaciones verificadas).

Casó **a la primera compilación**, y no por buscar formas: el fichero llevaba un
comentario con tres formas medidas (`goto` 74,9 %, `while` con la comprobación al
final 72,2 %, directa 62,6 %) y **ninguna tenía la estructura del original**.
Aquí hubo además cinco pines y un `asm("b .+12")` retirados en una sesión previa.

Lo que lo cerró fue `dwbody.py`, en una sola pasada:

```
ORIGINAL                          NUESTRO
  unsigned long Nest;   // r10      unsigned long count;  // r11
  unsigned long Result; // r4       comprobar:            (etiqueta)
                                    /* bloque anonimo */ {
                                      unsigned long Result;             // r9
                                      struct FEResponse * responseList; // r10
                                    }
```

O sea: **dos locales a nivel de función y nada más.** Ni `count`, ni
`responseList`, ni etiqueta, ni bloque anónimo. Y la llave está en que
**`Result` comparte r4 con el parámetro `Index`**: `Result` es el índice que
corre, y lo que nosotros llamábamos `Result` es su `Nest`.

La consecuencia se ve instrucción a instrucción. Con `count` como local, **el
parámetro se llevaba r3** (el registro de retorno) y el `return this->Count` de
salida costaba `bne`+`mr`+`blr`; el objetivo, con `count` en r3, lo hace con un
solo **`beqlr`**. La forma que casa:

```c
if (Index == this->Count - 1) return this->Count;
Nest = 1;  Result = Index;
do { Result++; switch (this->pResponseList[Result].ResponseID) { ... } }
while (Result < this->Count && Nest != 0);
return Result;
```

Las dos lecturas de `this->` sin local son las que `loop_optimize` iza al
precabezal (`mr r0,r3` + `lwz r3,0x14(r11)`, las dos en la línea del `do {` del
mapa de líneas). **La regla general: una local nuestra que el DWARF no tiene le
roba un registro duro a la que sí.**

## 3. Dos correcciones fieles al DWARF que cuestan cero

Mismo patrón en las dos, y el mismo que zFEng: **nosotros abrimos un bloque
anónimo donde el original declara a nivel de función.** El codegen no se mueve,
pero el fuente deja de contradecir al volcado y el siguiente que llegue
diagnostica sobre algo verdadero.

- **`inittmr::TIMER_init`** (268 B, 97,01492 %): `now`/`Period`/`tmp` a nivel de
  función (nosotros: `time`/`period`/`period64` dentro del `if`). Los nombres
  también son los del original. `now` comparte r27 con el parámetro `hz`.
  Lo único que queda es **un empate de planificador de una instrucción**: el
  `addi r3,r3,TIMER_restore@l` va antes del `stb bIsTimerInited` en el objetivo
  y después en el nuestro.
- **`srender::SNDI_validrendermode`** (188 B, 93,29787 %): `globalplayloc` y
  `globalspatialization` a nivel de función. `validrendermode` y `globalplayloc`
  comparten r3 en el original. Lo que queda **no es estructura**: el objetivo
  mete `pph->rendermode` en **r9** y pisa `*prenderindex`, así que tiene que
  recargarlo en las dos ramas —de ahí el `lwz`+`b` que a nosotros nos falta—.
  Es reparto.

## 4. Diagnósticos cerrados que valen para la ronda 26

### `zDynamics::Geometry::SphereVsBox` — 1.896 B, 99,02531 %, a **106 puntos**

Es la única función que le falta a zDynamics: cerrarla lleva la unidad de
93,4567 % a 100 %. **Dos problemas independientes**, no uno:

1. **f30/f31 intercambiados** entre `b_dim` y `p_dot` (8 filas). `lreg.py` lo da
   al dígito: `b_dim` 10 refs / vida 39 → **7692**; `p_dot` 6 refs / vida 15 →
   **8000**. Faltan **308 puntos**, y hay tres caminos con el umbral calculado:
   vida de `b_dim` ≤ 37 (→8108), una referencia más a `b_dim` (12 refs → 9230),
   o vida de `p_dot` ≥ 16 (→7500).

   **Verificado contra la verdad del compilador, y hace falta porque el agente de
   zWorld2 acaba de encontrar el TERCER fallo de `lreg.py`** (lista pseudos que
   nunca llegan a `global_alloc`, omite el factor `size` y casa el nombre por
   subcadena). Aquí no muerde ninguno de los tres: los dos pseudos son `float`,
   así que `size` vale 1 en los dos y **no altera su orden relativo**, y la línea
   `;; 91 regs to allocate:` del `.greg` —que es `allocno_order` ya ordenado— los
   trae **adyacentes, en las posiciones 39 y 40, con 157 (`p_dot`) delante de 152
   (`b_dim`)**. O sea: hay que adelantar `b_dim` **una sola plaza**.
2. **`sched1` nos sube `a_lp++` y el `cmpwi` diez ranuras**; el objetivo los deja
   al final del cuerpo.

**El `__asm__("")` del bucle NO es un adorno: sostiene 0,55 pp.** Quitarlo baja a
98,47890 % **y rompe `in` (r20→r19)** — o sea que lo que compensa es la posición
del contador, no el par de flotantes. Pero **quitarlo también acorta la vida de
`b_dim` de 39 a 38** (7692 → 7894): deja el hueco en **106 puntos**, una sola
ranura. Ahí está la ronda 26.

Vedas medidas: **ternario con la asignación dentro de la condición** —la forma
que sugiere el mapa de líneas, donde la línea 455 lleva a la vez la asignación de
`p_dot` y la comparación— da **93,91983 %** (GCC compila `?:` de flotantes
distinto del `if/else`); **`for` en vez de `do/while` sin el `asm`**, 97,71097 %.
El árbol de inlines es idéntico salvo que **`numerator` no tiene registro en el
original y sí en el nuestro (f1)**.

### `sfir::calcFIRCoeffs` — 936 B, 89,25214 %, **+4 B**

La instrucción que sobra está localizada: **PRE nos iza `slwi r28,r11,2` y
`addi r29,r30,0x20` —la dirección de `pfir->coef[halfLen]`, común a los tres
`case`— a un bloque nuevo en el camino por defecto del `switch`**, y el objetivo
no los iza. Es el mismo fenómeno del punto de inserción de PRE que bloquea
`_bOutput`.

**Corrección a `regmap`**: dice que `halfTmpFloat` es una local sólo nuestra, y
es cierto en el DWARF, pero **el objetivo calcula igualmente `tmpFloat * 0.5`
una vez y lo reutiliza en f30** — es un temporal del compilador, y los
temporales no salen en DWARF. Disolverla da **87,05983 %** (veda). Mover
`halfLen = 4;` detrás del clamp, **86,96154 %** (veda).

### `dvd_device::StartNonAlignedAyncRead` — 440 B, 93,59 %, **−24 B**

Mismas locales y mismo árbol de bloques, tres registros movidos, pero **seis
instrucciones de menos**. Falta código. Nadie la ha mirado.

### `spchsamp::iSPCH_GetSampleSizeData` — 136 B, mismo tamaño, 5 registros

Permutación pura del asignador, virgen.

## 5. Herramientas

- **`regmap.py` no acepta rutas para el middleware: la llave es el nombre pelado**
  (`srender`, `inittmr`, `sfir`, `vmbase`), y con una ruta dice «unidad
  desconocida» en vez de resolverla. Con `--list` **sin patrón** devuelve
  «0 funciones» — otra vez menos datos, no un error.
- **`regmap` no encuentra** `vm::__VMAllocVirtualToARAMLUT`, `filesys::AddToQueue`,
  `vmbase::__VMBASESetupExceptionHandlers` ni `criticalpath::VP6_PredictFilteredBlock`
  en el volcado del original, aunque `grep` sí los ve. Sin diagnosticar.
- **El permutador se colgó**: 15,8 minutos vivo con **2 segundos de CPU** y cero
  salida sobre `Geometry::SphereVsBox`. Matado; dejó el fichero intacto.
- **`-fsched-verbose=3` NO EXISTE** (lo midió el agente de zEcstasy): da «Invalid
  option» y sigue sin verbose, o sea salida vacía que se lee como «no hay nada».
  Es **`-fsched-verbose-5`, con guion**. Yo había puesto la forma mala en dos
  encargos de esta ronda y no pude avisarles.

## 6. Pendiente para la ventana

**`zFEng` está al 100 % y en `configure.py` sigue como `NonMatching`.** No lo he
promocionado ni he pasado `trypromo.py`: con siete agentes editando, un DOL
construido ahora no mide nada, y `configure.py` es del agente de `linked`.

---

# Segunda parte de la ventana (con `linked` y zBWare todavía vivos)

Cuatro funciones libres más, con el mismo método. **Cero bytes**, pero cuatro
diagnósticos cerrados y una veda que corrige un comentario del árbol.

## `zPlatform::ActualReadJoystickData` — 1.588 B, 97,35516 %, **−8 B**

`regmap` da el patrón de zFEng otra vez: **`v` es una local sólo nuestra y ocupa
r0, que es donde el original tiene `data`**. Pero **disolverla NO paga**:
sustituir los cuatro bloques `v = E; if (v & 0x8000) v = 0; data = v;` por la
forma directa sobre `data` da **96,49874 %** (veda).

Los 8 B que faltan son **exactamente dos instrucciones**, y ya está visto por qué
el objetivo las tiene y nosotros no: `mr r9,r0` + **`andi. r11, r9, 0x8000`** del
`if` que precede a `joy_data->stickX`. No es código muerto: **cuando la rama no
se toma, r11 vale 0, y el objetivo REUTILIZA ese registro como la constante cero**
en las tres copias siguientes (`mr r9,r11`, `mr r10,r11`, `mr r8,r11`); nosotros
las hacemos al revés (`mr r11,r9`…) y materializamos el cero aparte.

Y hay un segundo eje: el objetivo da **un registro distinto a cada bloque**
(r8 para `substickX`, r10 para `substickY`, r11 para `stickX`) y nosotros usamos
**r5 en los tres**.

## `zFe::SubTitler::GetElapsedTime` — 116 B, 95,51724 %

El comentario que había en el fichero («regalloc - fmadds targets f1 directly»)
se queda corto. El DWARF dice que **`thetime_ms` no tiene registro en el
original**: es una copia y se disuelve. Eso fija la forma —`timeElapsed = <expr>`
y detrás `thetime_ms = timeElapsed;` **en las dos ramas**— y con ella **aparece
el `fmr` que falta y el tamaño casa (112 → 116 B)**.

Lo que queda es **la dirección de la copia**: el objetivo hace `fmadds f0` +
`fmr f1,f0` + `stfs f0` (calcula en el temporal y copia al registro de retorno),
nosotros `fmadds f1` + `fmr f0,f1` + `stfs f1`, con el f0 muerto. Las tres formas,
medidas: base 112 B **95,51724 %**; copia tras la unión 112 B 95,34483 %; copia
en las dos ramas 120 B 94,82758 %. **Comentario del fichero corregido con esto**;
código sin tocar, la base sigue siendo la mejor.

## `dvd_device::StartNonAlignedAyncRead` — 440 B, 93,59091 %, **−24 B**

Los 24 B son **seis instrucciones y se sabe cuáles**: el objetivo saca `FileInfo`
a **r28** y `MemPointer` a **r7** en el prólogo (2) y hace `mr r3, r28` antes de
cada una de las **cuatro** llamadas a `DVDReadAsyncPrio` (4). Nosotros dejamos
`FileInfo` en r3 y no necesitamos ninguna. La causa está en la otra punta:
**el objetivo mete `PreBytes` en r3** (`subf r3, r30, r12`) y nosotros en r7, así
que a él se le ocupa el registro del primer argumento y a nosotros no.

**Trampa anotada: el DWARF dice `int FileBase, int Size` y el nombre decorado del
objetivo es `…Pvll`.** DWARF codifica `int` y `long` con el mismo tipo base de 4
bytes, así que ahí **no es evidencia** y no hay que "corregir" la firma.

## `zTrack::TrackStreamer::GetLoadingPriority` — 708 B, 97,181 %

Toda la pérdida de zTrack. **El árbol de inlines casa entero** (mismas locales,
misma estructura). Lo que cambia es reparto, y con una pista de marco: **todas
nuestras ranuras de pila están 0x10 por debajo de las del original** —nuestro
marco es 16 B más pequeño—, porque el original **materializa `angle_factor` (f0)
y `priority` (r0)**, que nosotros plegamos. Reparto duro, sin eje de fuente.

## Herramientas, segunda tanda

- **Finales de línea mezclados otra vez**: `SubTitle.cpp` tiene CRLF en la cabecera
  y LF en el cuerpo, así que decidir el terminador mirando los primeros 3.000
  bytes falla. Hay que anclar **línea a línea con el terminador real de cada una**
  (`splitlines(True)`).
- **`regmap.py` con dos argumentos sueltos en un bucle `for` de shell no separa**:
  hay que pasarlos por `while IFS='|' read`. Cuatro unidades salieron en blanco
  antes de verlo.
