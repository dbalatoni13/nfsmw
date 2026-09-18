# Herramientas

**Generado por `scripts/indice.py`. No editar a mano: se regenera.**

133 herramientas en `scripts/`, mas 0 sondas de un solo uso de rondas pasadas que no son herramientas (ver el final).

La descripcion es la primera linea del docstring de cada una. Si una fila dice `(sin docstring)`, esa herramienta no se puede usar sin abrirla: es un deber pendiente, no una herramienta.

## Medir el frente

| herramienta | que hace |
|---|---|
| `estado.py` | el frente entero en una pantalla, y sin creerse las metricas. |
| `censo.py` | cuanto CODIGO SIN ESCRIBIR queda, por unidad y ordenado por bytes. |
| `measure.py` | Medida independiente por unidad, abriendo los objetos originales y actuales. |
| `linkdelta.py` | la distancia REAL de cada unidad al enlace, seccion por seccion. |
| `promodist.py` | QUE cuesta promocionar cada unidad y CUANTO desbloquea. |
| `promote.py` | comprobacion previa a marcar una unidad como `Matching`. |
| `trypromo.py` | prueba una promocion SIN tocar el arbol ni esperar a la ventana. |
| `pctsnap.py` | instantanea y diff del PORCENTAJE POR FUNCION. |

## Comparar codigo

| herramienta | que hace |
|---|---|
| `fncmp.py` | que funciones de una unidad NO son identicas, con o sin SourceList. |
| `fndiff.py` | *(sin docstring)* |
| `audit.py` | PRUEBA que una funcion casa, en vez de creerse el porcentaje. |
| `mnemo.py` | triaje por MULTICONJUNTO DE MNEMONICOS: separa fuente de asignador. |
| `arboljump.py` | compara el ARBOL DE SALTOS, que es lo que `mnemo` no ve. |
| `triaje.py` | clasifica los near-miss por TIPO de diferencia, no por porcentaje. |
| `triage.py` | clasifica las near-miss por lo que de verdad les pasa. |
| `regmap.py` | enfrenta local a local el reparto de registros del ORIGINAL contra el NUESTRO. |
| `symtabdiff.py` | diff de TABLAS DE SIMBOLOS ELF entre el .o objetivo y el nuestro. |
| `tamfn.py` | compila UN fichero con sus cflags reales y da el tamano de cada |
| `stripped.py` | que dato EMITIA el original y el enlazador descarto. |
| `stripmap.py` | cuanto del deficit del DOL de cada unidad es DANO DE ESTRIPADO |

## Datos, .rodata y el pool

| herramienta | que hace |
|---|---|
| `claimdata.py` | reclama para su unidad los datos que el troceador dejo sueltos. |
| `claimlbl.py` | de que direccion es el `.rodata` de cada unidad, SIN adivinar. |
| `claimrange.py` | propone los rangos de `splits.txt` que cada unidad de biblioteca debe RECLAMAR. |
| `lcfix.py` | vuelve a resolver por CONTENIDO los `$LC` que fuerza keep.lst. |
| `lcmap.py` | empareja el pool `$LCn` que emite cc1plus con el `lbl_XXXXXXXX` |
| `lcpool.py` | el pool `$LC` que emitimos y el objeto del original NO tiene. |
| `deadstr.py` | cadenas que EMITIMOS, el objetivo TIENE, y el enlazador se lleva. |
| `deadlink.py` | `deadstr.py`, pero preguntandole al ENLAZADOR quien esta muerto. |
| `dupdata.py` | dato ESCRITO A MANO en un asm() que el compilador ya emite solo. |
| `dupstr.py` | cadenas que emitimos MAS VECES que el objetivo, por CONTENIDO. |
| `rodata.py` | *(sin docstring)* |
| `rodatagaps.py` | el mapa de HUECOS de dato muerto de una unidad, con su sitio. |
| `strseq.py` | las CADENAS que objdiff no compara, por funcion. |
| `pool2lit.py` | Cambia las referencias `extern const float lbl_XXXX` por el literal que valen. |
| `datadiff.py` | diff BYTE a BYTE, con resincronizacion, de una seccion de datos. |
| `datacmp.py` | cuanto DATO tenemos ya bien, comparando BYTES y no nombres. |
| `seccdiff.py` | el tamano de CADA seccion nuestra contra el objeto EXTRAIDO. |
| `dolrod.py` | diff de una seccion ENLAZADA en la ventana de una unidad. |

## El enlace y el DOL

| herramienta | que hace |
|---|---|
| `dolwhere.py` | enlaza con una unidad promocionada y dice DONDE difiere el DOL. |
| `refs.py` | Que simbolos de mas estan REFERENCIADOS, y desde donde. |
| `extrasym.py` | QUE simbolos emitimos que el objeto EXTRAIDO no tiene, y de que son. |
| `ghostref.py` | simbolos que referenciamos y que NUNCA existieron en el original. |
| `missingcalls.py` | Detector mecanico de CODIGO QUE FALTA, ordenado por bytes. |
| `phantom.py` | RESUELTO. Este script buscaba funciones que objdiff daba por debajo del 100% |
| `relocfantasma.py` | funciones que solo fallan porque el TROCEADOR simbolizo mal. |
| `residuo.py` | cuanto de la `.rodata` del objetivo NO es dato, sino restos. |
| `checksplits.py` | valida `splits.txt` ANTES de re-extraer. |
| `rangechk.py` | valida los limites de `splits.txt` contra los STT_FILE del ELF. |
| `gapchk.py` | huecos escritos a mano que `-strip-unused-data` se lleva. |

## Orden de emision

| herramienta | que hace |
|---|---|
| `permorden.py` | La permutacion exacta de una unidad: que funciones emite en otro orden. |
| `vtord.py` | el ORDEN DE EMISION de las vtables de una unidad, lado a lado. |
| `vtable_audit.py` | audita el orden/conteo de metodos virtuales. |
| `globalini.py` | el simbolo `_GLOBAL_.I.<nombre>` de cada unidad, comparado. |
| `stmtorder.py` | busca el ORDEN DE FUENTE de un bloque de stores. |
| `textorder.py` | el ORDEN de las funciones de `.text`, el frente que objdiff NO VE. |
| `missinline.py` | *(sin docstring)* |

## El compilador por dentro

| herramienta | que hace |
|---|---|
| `rtldump.py` | volcados RTL (.lreg/.greg/.combine/...) de una unidad o de un .cpp suelto. |
| `schedtrace.py` | la TRAZA del planificador, ciclo a ciclo, con lista de listos. |
| `lreg.py` | la tabla de allocnos DEL PROPIO COMPILADOR, para las funciones |
| `alloc.py` | c25w2_alloc.py -- la tabla de allocnos REAL, contrastada contra la verdad |
| `regalloc.py` | vuelca la tabla de allocnos de GCC 2.9 (refs y LONGITUD DE |
| `deadreg.py` | caza accesores con CUERPO VACIO por su firma en el asm. |
| `pines.py` | censo de los `register X asm("rN")` y si estan donde importan. |
| `sprobe.py` | Compila UN .cpp de la unidad con -S usando los cflags reales de la unidad. |
| `swsweep.py` | fe_swsweep.py -- barrido global buscando los DOS patrones de switch. |

## El original como fuente

| herramienta | que hace |
|---|---|
| `dwarfmap.py` | de quien es CADA dato, leido del DWARF del ELF original. |
| `dwbody.py` | diff del CUERPO DWARF (locales + ARBOL DE INLINES + bloques) del |
| `dwblocks.py` | Compara los RANGOS de bloque/inline del DWARF NORMALIZADOS al inicio de la |
| `lmap.py` | lmap.py <unidad> <simbolo>  -- asm del OBJETIVO con la linea de fuente al lado. |
| `ps2fn.py` | que hace una funcion, leido del build de PS2 (Alpha 124). |
| `ps2map.py` | el mapa de enlazador del alpha de PS2, que trae el orden de emision. |
| `zdump.py` | Vuelca el desensamblado original de varios simbolos de zLua fundido con la linea de fuente. |
| `derotate.py` | Desrota la tirada de stores del original y propone el orden de sentencias. |
| `x360ref.py` | referencia cruzada GC↔X360 sobre el basefile descifrado. |

## Construir y mantener

| herramienta | que hace |
|---|---|
| `build_direct.py` | Compila las unidades invocando ngccc directamente, saltandose ninja. |
| `bench.py` | el ciclo de trabajo en 1,4 s en vez de 34. |
| `frozen.py` | congela el diff de una unidad y comprueba que no ha cambiado. |
| `scratchclean.py` | purga artefactos regenerables de scratchpad/. |
| `mangfix.py` | vuelve a resolver los ALIAS de nombres mangled de clase local. |
| `indice.py` | regenera `docs/HERRAMIENTAS.md` leyendo los docstrings. |

## Sin clasificar

Nadie las ha metido en una familia de `indice.py`. Si usas una y resulta util, anadela a `FAMILIAS` para que se encuentre.

| herramienta | que hace |
|---|---|
| `antesdegradar.py` | que va a pasar si degrado esta unidad a NonMatching. |
| `aranges.py` | las fronteras de unidad AUTORITATIVAS, leidas de `.debug_aranges`. |
| `audit_zeros.py` | auditoría Fase 1: símbolos ausentes y near-misses por unidad. |
| `auditaportable.py` | lo que impide que este codigo sea decompilacion portable. |
| `auditecho.py` | audita los TECHOS documentados: cuales son falsos. |
| `callcheck.py` | el detector general de "el 100% que miente". |
| `casetree.py` | Compara el ARBOL de un switch: la secuencia de constantes comparadas. |
| `censoasm.py` | el censo EXACTO de todo el asm de GCC que queda en el arbol. |
| `censohueco.py` | censohueco.py [minimo] -- donde esta el hueco recuperable de PS2, por unidad. Lee los informes que deja |
| `censorancios.py` | recompila cada unidad PROMOCIONADA a un temporal y compara las |
| `cheap.py` | *(sin docstring)* |
| `desplaza.py` | el desplazamiento REAL que una unidad le impone al resto del enlace. |
| `desplazamientos.py` | desplazamientos.py <diff.json> [...] -- fallos de LAYOUT de struct. Busca cargas y almacenamientos que |
| `diffunidad.py` | diffunidad.py <VERSION> <unidad> [<unidad>...] -- el diff completo de objdiff (JSON, instruccion a |
| `dwarf1.py` | el oraculo del .debug DWARF-1 del ELF de GameCube (r70b). |
| `frontcoste.py` | que CARGA y que PRECIO tiene cada frontera de `aranges.py --faltan`. |
| `fuse.py` | Funde el desensamblado del original con el mapa de lineas de EA. |
| `keepchk.py` | Comprueba que cada entrada `objeto:simbolo` de keep.lst existe de verdad. |
| `libdiff.py` | compila y mide una unidad de MIDDLEWARE, sin pasar por ninja. |
| `libdwarf.py` | el volcado DWARF de una unidad de MIDDLEWARE, servido en C. |
| `litcheck.py` | el multiconjunto de VALORES del pool, funcion a funcion. |
| `litpos.py` | litpos.py <unidad>  -- compara el VALOR de cada literal del pool POSICION A |
| `loss.py` | Ranking por BYTES PERDIDOS, no por instrucciones malas. |
| `mapaplaybook.py` | el mapa de `PLAYBOOK.md`, para no tener que cargarlo entero. |
| `mdebug.py` | lee el .mdebug (ECOFF/MIPS) del build de PS2 (SLES-53558-A124). |
| `mn_diff.py` | mn_diff.py <unidad> <sym> <ours.o> [lim]  -- diff del OBJETIVO de <unidad> contra un .o cualquiera. |
| `mn_forms.py` | mn_forms.py <unidad> <src.cpp rel> <sym> <formas.json> |
| `mn_repro.py` | mn_repro.py <unidad> <src.cpp relativo> [flags extra...] |
| `mn_sweepflags.py` | mn_sweepflags.py <unidad> <src.cpp> <sym>  -- un flag por compilacion, imprime el diff resumido. |
| `movidos.py` | que simbolos caen en OTRA DIRECCION al meter una unidad. |
| `orac.py` | el oraculo, en un grep: locales, registros, inlines y structs. |
| `parseord.py` | el orden de PARSEO de las clases, que es lo que decide las vtables. |
| `pct_ecs.py` | *(sin docstring)* |
| `pctall.py` | pctall.py [--solo-informe] [VERSION ...] -- el porcentaje de TODAS las versiones, no solo de GameCube. |
| `permuter.py` | Buscador automatico de la forma de fuente que casa (un *permuter*). |
| `plan.py` | Plan de reconstruccion de una funcion: asm + linea de fuente + arbol de inlines. |
| `prefijochk.py` | ningun rango de `.rodata` puede contener DOS prefijos de TU. |
| `prefijotu.py` | el vocabulario compartido que le falta a una unidad, EN ORDEN. |
| `previo.py` | que se ha PROBADO YA en una funcion, y con que resultado. |
| `pubnames.py` | atribuye CADA SIMBOLO del original a SU UNIDAD DE COMPILACION, |
| `reladdr.py` | reladdr.py <unidad> -- la DIRECCION a la que apunta cada reubicacion HA16/LO16. |
| `reorden.py` | contrafactual: si emitieramos en el ORDEN del objetivo, casaria? |
| `rodorden.py` | la PERMUTACION de la `.rodata` de una unidad, por cadenas. |
| `s_zlua.py` | Compila el PREFIJO de zLua.cpp hasta el .cpp indicado, a ensamblador (-S). |
| `serie.py` | la tasa REAL de cierre por ronda, reconstruida del historial. |
| `sonda.py` | sonda.py <VERSION> <sonda.cpp> -- pregunta al compilador el LAYOUT real de una clase: tamanos, |
| `tabla_op.py` | tabla_op.py [--prefijo P] <diff.json> [...] -- la tabla "lo que emitimos -> lo que emite el original" de |
| `undefcheck.py` | toda llamada a un simbolo que NO EXISTE en el ELF. |
| `undlist.py` | que simbolos INDEFINIDOS impiden promocionar una unidad. |
| `unitmap.py` | porcentaje por unidad + total, desde build/GOWE69/report.json. |
| `unitscore.py` | *(sin docstring)* |
| `vtdup.py` | vtables (y demas datos con nombre) que emitimos en VARIAS unidades. |
| `zcmp.py` | Compara nuestro .s (de s_zlua.py) contra el .s original, funcion a funcion. |

## Deberes: 6 sin docstring

`cheap.py`, `fndiff.py`, `missinline.py`, `pct_ecs.py`, `rodata.py`, `unitscore.py`

## Sondas de un solo uso (0)

Guiones que un agente escribio para una ronda concreta y nadie borro. No son herramientas y no se mantienen; estan aqui solo para que se sepa que no hay que leerlas.


