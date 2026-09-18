# Cuándo NO creerte cada herramienta

Compañero de `HERRAMIENTAS.md`: aquélla dice qué hace cada una, ésta **dónde miente**.

**Todas las entradas están medidas y llevan el caso que las destapó.** Ninguna es teórica.
Cada una costó al menos media ronda, y varias costaron una entera.

**Treinta y una entradas.** Las nueve últimas salieron de las rondas 60 a 63, que es
la serie en la que el frente dejó de ser «escribir código» y pasó a ser
«colocar bytes»: casi todas las trampas nuevas son de MEDIDA, no de escritura.

La regla que las gobierna a todas: **exígele a tu medida un control que TENGA que cambiar**.
Si el control no puede fallar, no es un control.

---

## Las que dicen menos de lo que hay

### `vtord.py` — miente si el enlazador estripa una vtable

Compara el **objeto**, no el enlace. Si emitimos una vtable que el original no tiene, el
enlazador se la lleva — pero `vtord` ya ha corrido **todos los índices** detrás de ella.

> **Caso (r58, `zAnim`)**: cantaba «13 de 24 posiciones no casan» con las **21 vtables
> enlazadas en su dirección exacta**. Las dos de más (`CAnimMarker`, `UTL::COM::IUnknown`) las
> estripaba el enlace.
>
> **Caso (r59, `zFe`)**: 130 de 157 descolocadas, y la mayoría era **un corrimiento de dos
> posiciones**.

**Confirmación**: mira las direcciones **ENLAZADAS**, no las del objeto.

### `reorden.py` — dos falsos, en las dos direcciones

- **Falso CONTENIDO**: la máscara conserva la mitad alta de un `ADDR32`, así que un puntero a
  algo que se movió **más de 64 kB** delata su desplazamiento como si fuera contenido.
  > **Caso (r55)**: señaló once vtables «con contenido» en `zWorld`, `zCamera` y `zEagl4Anim`.
  > `vtable_audit` dio **cero ranuras distintas** en las dos que se abrieron.
- **Falso CERO**: sólo compara lo que **empareja por nombre**. `zAI` tenía 388 B de `.data`
  que no emitimos y le salía 0.

**Confirmación**: para vtables, `vtable_audit.py` (compara por SÍMBOLO). Para el falso cero,
mira los símbolos «sin par».

### `reorden.py` y `triaje.py` — no ven las unidades de biblioteca

Contestan «sin ventanas que comparar» o «0 funciones» aunque la unidad esté en el enlace.
Es la familia de `nfsmw-frente-middleware`. **Sin arreglar.**

---

## Las que miden otra cosa de la que crees

### `linkdelta.py` — compara TAMAÑOS, no cercanía

Que dé `0` no significa que la unidad esté cerca.

> **Caso (r53)**: `zSpeech` daba `rodata−8` con **83 de sus 703 funciones en otra dirección**.
> `zAI` daba `rodata−240` con **781 de 1.030** descolocadas: los 240 B eran el 0,09 % del
> problema.

**Confirmación**: `movidos.py` para la posición, `dolwhere` para los bytes.

### `dolwhere.py` — cuenta RANGOS, no bytes

Extiende un rango mientras difiera alguno de los 12 siguientes.

> **Caso (r55)**: 183.693 donde había **15.369** bytes reales.

### `movidos.py` — su resta NO mide cambio de dueño

`enlazado − delta` mezcla tres cosas y sólo una es robo: bulto que le quitamos a otro objeto,
**código nuestro sin referenciar que el enlazador tira** (`-strip-unused-data` va por SÍMBOLO,
así que emitir de más es lo NORMAL) y relleno de alineación.

> **Caso (r55)**: se dijo que `zLua` «se lleva 17.676 B de otros». **Falso**: su término de
> robo es cero. La prueba limpia es `zTrack`: cero robos y sigue imprimiendo `.text −3.536`.

**Confirmación**: el cambio de dueño se mide **por símbolo** — quién lo define entre los
objetos originales y entre los nuestros, con el ganador por `(rango, índice)` donde
`GLOBAL < WEAK < COMMON`. Para un símbolo suelto: **mira su dirección en los dos ELF
enlazados; si no cambia, no hay robo.**

### El fuzzy — manda al sitio equivocado

Puede **subir** con menos instrucciones exactas. Cuenta filas con `fndiff`.

---

### `textorder.py` — su cifra NO siempre es la que manda, y puede ser FANTASMA

Dos fallos distintos, los dos medidos en la r63.

- **No es la metrica**: en `zAI` las descolocadas no se movieron --359 antes y
  359 despues-- mientras la distancia real al DOL bajaba un **78 %** (244.740 ->
  53.874 B). Optimizar `textorder` no es optimizar el DOL.
- **Puede contar fantasmas**: compara el **objeto**, y ahi emitimos simbolos que
  el enlace tira. En `zEagl4Anim` sus 251 «descolocadas» incluyen **106 simbolos
  que el enlazador se lleva**.

**Confirmacion**: mide tambien con `dolwhere` o comparando DIRECCIONES
ENLAZADAS. Es el mismo fallo de familia que `vtord`.

### `aranges.py` — el DELTA4 es el tamano EN EL OBJETO, y `--dir` no ve huecos

- El inicio de cada rango es una reubicacion que resuelve el **enlazador**; el
  tamano es una resta de dos etiquetas del **mismo objeto** que colapsa el
  ensamblador. O sea que **el tamano es el del OBJETO**, y el enlace lleva
  `-strip-unused-data`: por eso **140 de 315 rangos de `.text` se solapan**.
  Para el final vale `AT_high_pc` del DIE, no «el inicio del siguiente» -- eso
  habria sido falso en los cinco huecos de Metrowerks.
- **`--dir` en `.data`/`.rodata`/`.bss` no tiene deteccion de huecos** como si la
  tiene en `.text`, asi que **en cualquier direccion de Metrowerks imputa al
  ultimo GCC anterior**. `--dir 0x804B9FFC` (la `.bss` de `steering`) contesta
  `zFeOverlay`.

### `deadstr.py` — casi todo lo que propone YA ESTA en `keep.lst`

> **Caso (r60)**: barrer las 17 SourceLists pendientes daba **927 cadenas y
> 15.096 B**, que parecia un frente nuevo entero. Restando lo que ya existe
> quedan **51 en cuatro unidades** y CERO en trece.

**Confirmacion**: `comm -23` entre lo que propone y lo que ya hay. Y mira el
**signo** de `linkdelta`: empareja por CONTENIDO, asi que no distingue «al
objetivo le falta» de «el objetivo la tiene por otra via» -- en `zMisc` empuja al
reves, porque esa unidad ya emite de mas.

### El decompilado de Ghidra NORMALIZA la semantica: no es la forma del original

> **Caso (r63)**: en `IdctColumn` Ghidra imprime siete `&&` encadenados y parecia
> EL hallazgo, porque nuestra fuente tiene un pliegue con `|`. El ELF tiene un
> **`or.` de siete terminos y UN solo salto** (`8034bb18 or. r8,r0,r7`,
> `8034bb1c bne`). **Nuestra fuente ya era la buena.**

Y peor: **comparar el C de Ghidra contra el TEXTO de nuestra fuente da falsos
positivos en masa**. De ocho «hallazgos de forma», **SIETE cayeron** al
contrastarlos contra nuestro OBJETO; el conteo de llamadas sale **28 de 28
IGUAL**.

**Confirmacion**: para comparar FORMAS ve al **ASM**, nunca al decompilado. Y
compara contra el **objeto**, no contra el texto.

Aparte: los nombres de miembro que Ghidra pone salen **DESPLAZADOS en clases
derivadas** (en `Tire::UpdateLoaded` dice `this->mSlip` para `stfs f31,0xf0` y el
DWARF dice que 0xF0 es `mRoadSpeed`). **Un nombre de miembro solo vale si el
offset del listado lo confirma.**

## Las que dependen de algo que se pudre

### `linkdelta` con el `keep.lst` rancio — miente en VARIAS unidades a la vez

En cuanto un agente mueve una cadena, los `$LC` se renumeran y las entradas de `keep.lst`
dejan de resolver. Y una entrada que no resuelve **no da error**: `-strip-unused-data` se
lleva el bloque en silencio.

> **Caso (r57)**: `zPhysics` decía `rodata−384` donde hay **`+8`**; `zGameplay` `−296` donde
> hay `−136`. **392 y 160 bytes de mentira.**
>
> **Caso (r59)**: mentía en **CINCO unidades a la vez** — `zWorld −408`, `zPhysicsBehaviors
> −256`, `zPlatform −216`, `zCamera −184`, `zWorld2 −208`, **todo fantasma**. Con `lcfix`
> aplicado, las cinco leen `+0`.

**Protocolo**: **`lcfix.py` es lo primero de cada ventana**, y los agentes **no** pueden
correrlo — reescribe una entrada del ENLACE mientras los demás miden. Lo cuentan y lo aplica
el jefe.

**Y ojo**: una entrada de `keep.lst` es **venenosa o imprescindible según cómo esté la fuente
ESE día**.
> **Caso**: en la r58 había que **borrar** la de `zAnim` porque revivía una cadena y dejaba su
> `.rodata` +24. En la r59 la misma familia era **obligatoria** — sin ella, `DOL ROTO`. La
> cadena había pasado a ponerla un bloque anónimo.

### `build_direct.py` — puede decir «1 ok» y dejar un `.o` que no es de esa fuente

> **Caso (r56)**: el enlace base pasó de `DOL OK` a `ROTO` por 66 B. **No era ninguna
> edición**: recompilando la MISMA fuente volvió a OK. Compilando en serie y sellando, dos
> unidades más cambiaron de sello sin que la fuente cambiara.

**Protocolo**: sella el `.o` con `sha1`. **Una promoción sin tres compilaciones del mismo
sello no vale.** Y falla además de forma intermitente sin error (`0 ok, 1 fallidas`).

### `serie.py` — un control contra una referencia RANCIA no es un control

> **Caso (r59)**: decía `CONTROL OK: 518 unidades` **tres horas después** de que
> `zGameModes` promocionara a 519. No fallaba: **coincidía porque los dos estaban atrasados**.

*Arreglado*: ahora compara fechas y se niega a pronosticar si `report.json` es más viejo que
`configure.py`, `keep.lst` o `splits.txt`.

### `promote.py` — decía «se puede marcar Matching» con el código sin casar

> **Caso (r55)**: `criticalpath` daba `LIMPIA` con una función a 37 instrucciones.
> `trypromo` daba `DOL ROTO`.

*Arreglado*: contrasta con `report.json` y avisa.

### Marcar `Matching` **no es** promocionar

`objetos_del_enlace()` lee `build.ninja`, que genera `configure.py`.

> **Caso (r60)**: edité `configure.py` sin regenerar, así que tres «DOL OK con la unidad
> promocionada» estaban enlazando la lista **vieja** — verificaban el estado anterior.

**Protocolo**: tras marcar `Matching`, **regenerar el grafo y volver a enlazar**.

---

### La herramienta AVISA y no lo lees: las lineas FALLO de `lcfix`

Es la trampa mas cara de la serie, y **no es de la herramienta: es del protocolo**.

`lcfix --check` saca DOS clases de linea y no son lo mismo:

- `CORRIGE` -- una entrada de `keep.lst` apunta a un `$LC` renumerado. Lo arregla
  `lcfix` solo, y el jefe solo tiene que contar cuantas.
- **`FALLO` -- algo que `lcfix` NO PUEDE ARREGLAR** y que exige tocar la FUENTE.

Durante rondas cerre la ventana contando solo los `CORRIGE`. Y entre los `FALLO`
estaba esto, literal:

    FALLO  zSim: $LC526 vale 3e800000 y deberia valer 3f800000
           -- QuickGame.cpp apunta al literal equivocado

> **Caso (r64b)**: `QuickGame.cpp:682` declara `lbl_80404864` ALIAS del literal
> `$LC526` con un `.set`. El pool se corrio, `$LC526` paso a ser un `0.25f`, y
> `OnManageTime` cargo el flotante equivocado: `lfs f13, 0x4870(r11)` donde el
> objetivo pone `0x4864`. **UN byte de DOL.**
>
> El fichero lo habia PREDICHO en un comentario de 18 lineas (`:660-677`) y traia
> la directiva `// @lcsrc zSim $LC526 3f800000` para que `lcfix` lo vigilara. **El
> aviso funciono desde el primer dia. Nadie lo leyo.**

**Protocolo corregido**: al cerrar la ventana, tras `lcfix`, mirar las lineas
`FALLO` UNA A UNA y clasificarlas. Muchas son inertes --entradas huerfanas de
troceados que ya no existen-- pero **una sola de la familia `@lcsrc` es un DOL roto**.

### Y su gemela: un `.o` RANCIO puede dar `DOL OK` durante rondas

El enlace usa los `.o` que hay EN DISCO, no la fuente. Si un `.o` se quedo sin
reconstruir, el `DOL OK` certifica **el objeto viejo**, no el arbol.

> **Caso (r64b)**: el `.o` de `zSim` llevaba rondas rancio. El DOL base daba OK con el.
> Al reconstruirlo desde la fuente actual, aparecio la regresion de arriba. **Las
> lecturas de DOL OK de varias rondas eran falsas para esa unidad.**

Y hay dos formas conocidas de que un `.o` se quede rancio, las dos medidas:
- `build_direct.py --help` **no imprime ayuda: lo interpreta como compilalo todo**
  y lanza las 33 unidades; las que fallan dejan el `.o` anterior.
- Con varios agentes en paralelo, `build_direct` falla con **Could not open output
  file** y **deja el `.o` VIEJO diciendo "0 ok, 1 fallidas"**. Quien no lee esa
  linea mide el objeto anterior y cree que ha acertado.

**Protocolo corregido**: el `DOL OK` de cierre solo vale sobre objetos reconstruidos.
Y el control barato es comparar las **secciones ALLOC** antes y despues de
recompilar: el sha1 completo da falsos positivos porque `-gdwarf+` mueve `.line`
con cualquier cambio de numero de lineas.

## Las trampas de lectura, que no son de herramienta

### La ventana de 32 bytes

`ldscript.ld` alinea a 32 el arranque de `.data`, `.bss`, `.sdata` y `.sdata2`. Un `rodata +8`
**no es una diana**: es «sobran entre 8 y 39».

> **Caso (r57)**: ese `+8` de `zPhysics` desplazaba 32 B tres secciones, y hacía que
> **273.472 B** salieran «100 % distintos» sin un solo byte de contenido malo.

### El cadáver

`-strip-unused-data` se lleva `size & ~7`. Un literal de 8 B desaparece entero; **uno de 4 B
se queda**; y en general quedan **1 a 7 bytes**. **No da más de 7**, así que una cadena de
10 B del objetivo NO puede venir de un literal muerto. Y **no se arregla con un `asm()`**.

### Una diferencia de TAMAÑO entre símbolos del mismo nombre

Puede ser contenido **o troceado distinto**.
> **Caso (r55)**: `zTrack` daba «obj 540, nue 92» y `prefijotu` decía que faltaba **una**
> cadena de 53 B.

### Un emparejamiento SISTEMÁTICO entre dos listas que deberían diferir

Es un error de indexado, no un hallazgo.
> **Caso (r55)**: una sonda daba el mismo par en dos offsets distintos. Era la `.symtab` mal
> indexada.

### Un delta NEGATIVO en una serie de progreso

El trabajo no se deshace: sólo puede ser error de medida.
> **Caso (r54)**: mezclaba dos censos con denominadores distintos.

### `GLOBAL` gana a `WEAK` aunque vaya DESPUÉS · `SHN_COMMON` no define

Y `dtk` marca `GLOBAL` lo que el compilador emitía `WEAK`, así que los `obj/*.o` sin
promocionar **enmascaran cambios de dueño** que aparecerán solos al avanzar.

### El texto PREPROCESADO da falsos «cero regresiones»

Si no has revertido **todas** las cabeceras que tocaste.
> **Caso (r58)**: dio 14 de 14 idénticas con una regresión de 120 B dentro.

**Confirmación**: compila la unidad ajena a un `.o` **privado** (nunca a `build/GOWE69/src`,
que es de su agente) y compara secciones y **el valor de cada símbolo**.

### El scratchpad está COMPARTIDO entre los agentes de una tanda

> **Caso (r59)**: a un agente le sobrescribieron una sonda a mitad de ronda.

Usa un subdirectorio propio.

---

### Mover una raya de seccion crea huecos que se estripan EN SILENCIO

Se llaman **`pad_NN_DIR_sec`**, no `gap_`, y **hay que LEER el nombre del `.o`
re-extraido**: pegar el nombre predicho no funciona.

> **Caso (r61)**: el informe predijo DOS y con nombre `gap_`. Eran **TRES** y
> `pad_`. Con dos de tres, `DOL ROTO` y `.bss -96`.

**Confirmacion**: compara **seccion a seccion contra el ELF original**, no mires
solo el hash -- el hash dice que algo esta mal, las secciones dicen QUE.

### Un fichero puede tener DOS entradas en `configure.py`

Una con ruta completa y otra con nombre suelto. **Manda la de ruta completa.**

> **Caso (r61)**: marcada la otra, `configure.py` no protesta y `build.ninja`
> sigue diciendo `linked False`. Costo una vuelta entera.

### Incluir una cabecera puede reordenar el `.bss` SIN NINGUNA SENAL

> **Caso (r63, `spchdata.c`)**: incluir `spchi.h` --lo obvio-- trae dos `extern`
> que ADELANTAN `gEventDats` al desplazamiento 0 y empujan `gCallbacks` a 64 (el
> original es gCallbacks 0, gEventDats 20). **Y la `.data` sale PERFECTA en ese
> mismo intento**, asi que no hay ninguna señal si solo se mira `.data`.

### El CENSO se mueve: los deltas entre rondas no son comparables

616 -> 617 al anadir `spchdata`, 617 -> 619 al partirse dos comodines. La cifra
honesta es el **CONTEO** de `linked` contra el **TECHO** (545), no la fraccion
contra el censo.

### Los ANDAMIOS no compensan una forma equivocada: SOSTIENEN el reparto

Es la hipotesis mas atractiva del proyecto y esta **refutada tres veces**:
- la familia D midio que quitarlos **empeora** (`eProject` 14 -> 23 filas,
  `UpdatePlatInfo` 28 -> 60) y devuelve **cero bytes**;
- de los 727 del arbol, **696 viven en codigo que YA CASA al 100 %**, y la
  interseccion «andamio Y forma que difiere de verdad» es **CERO**;
- **`criticalpath` cerro al 100 % con ONCE andamios dentro**.

Quitarlos es limpieza **posterior** al 100 %, con el DOL como red, no un camino
hacia el.

### El censo de andamios no veía la palabra clave CORTA `asm("")` (r67)

GCC 2.95 acepta `asm` igual que `__asm__`, y 70 barreras estan escritas asi. El patron
`__asm__\s*\(\s*""` las dejaba fuera: en HEAD eran **238, no 166**. Patron correcto:
`(?<![\w])(?:__asm__|asm)\s*\(\s*""`. Y los literales de un asm se CONCATENAN
(`"a\n" "b\n"`): un regex que coge solo el primero clasifica mal el bloque. Al cerrar una
ronda de limpieza se cuenta por fichero en HEAD y en el arbol: un pin convertido en
`asm("" : "=r"(x))` no es un andamio retirado.

### Una cadena C muerta no sobrevive al enlace sin su `@lc`, y un `.asciz` pegado puede estar COMPENSANDO (r67)

Sustituir un bloque `asm(".section .rodata" ".asciz ...")` por los literales C que el
original asignaba en codigo muerto deja el OBJETO mas cerca del original, pero en el enlace
`-strip-unused-data` se lleva las cadenas (nadie las referencia): `.rodata` 160 B corta.
Con sus `# @lc` en `keep.lst` sobreviven, y aun asi la seccion salio **32 B larga**: el
`.asciz` sin alinear compensaba otro bloque demasiado largo. **Medir el enlace
(`dolwhere`), no el objeto**, antes de aceptar un parche de datos «mas fiel».

### Una nota en un fichero con `BNEW` mueve el `.text` (r67)

`BNEW` es `new (__FILE__, __LINE__)`: una linea de comentario de mas antes de la llamada
cambia el `li r5`. Antes de comentar un fichero, busca `__LINE__`/`BNEW`, y reescribe
lineas de comentario existentes en vez de anadir.

### Anclar una nota en la línea del andamio puede BORRAR el andamio (r67)

Si el `new_string` de la edicion no vuelve a escribir la linea de anclaje, el andamio
desaparece sin que el diff de lineas `+` lo delate. Revisa las lineas `-` del diff: una
que no sea comentario es una regresion.

### `git archive` no saca los ficheros IGNORADOS que el build necesita (r67)

Una extraccion de HEAD para comparar objetos no compila hasta copiar los 34
`Generated/Hashes/*.h` de `.gitignore`
(`git ls-files --others --ignored --exclude-standard src`). El control negativo (una unidad
intacta tiene que salir IDENTICA) lo caza.

## Y la más cara de todas: no mirar si ya está probado

**El trabajo previo de este proyecto vive en los COMENTARIOS DEL FUENTE**, junto a la función.
Son **264 bloques** y no estaban indexados hasta la r55.

> **Caso**: en una sola ventana estuve a punto de re-atacar **tres** funciones que ya tenían
> cuatro, once y doce variantes medidas y negativas. En una llegué a escribir en el dossier
> «nadie la ha mirado» cuando tres rondas la habían trabajado y mis «tres ciclos de registro
> nuevos» eran literalmente los que ya estaban en el comentario.

    python scripts/previo.py <funcion>

**Cuesta un segundo. Córrelo antes de atacar nada.**
