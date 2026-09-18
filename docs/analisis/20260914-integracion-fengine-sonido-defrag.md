# Integración FEngine, sonido y DefragmentPool — 14/09/2026

## Estado final verificado

Se ha seguido la tanda acordada: FEngine, frontera lbmpeg/ssysinit/sdspmix y
limpieza del literal de DefragmentPool. Las pruebas privadas de los agentes se
han contrastado con nuevas compilaciones desde las rutas productivas y con el
grafo real de 619 objetos. No se ha promovido ninguna unidad en esta tanda.

| Medida | Antes | Después |
|---|---:|---:|
| Código matched | 3.915.284 / 3.946.048 B | igual: 99,22038 % |
| Funciones exactas del reporte | 18.406 / 18.432 | igual |
| Código linked | 1.264.380 B | igual: 32,04168 % |
| Unidades linked | 525 / 619 | igual |
| Datos matched | 564.981 B | 565.009 B: +28 B |
| Datos linked | 550.429 B | igual |

**DOL productivo: 4.541.888 B, idéntico byte a byte al original.**
SHA-1: `9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

No se adjudican puntos nuevos de código a la retirada de ASM: esos cuerpos ya
se contabilizaban antes. La mejora es reconstrucción de fuente genuina y
eliminación de compensaciones, no una subida artificial de matched/linked.

## 1. FEngine integrado

En `src/Speed/Indep/Src/FEng/FEngine.cpp` se incluye `Profiler.hpp` y se restauran
los dos `ProfileNode` y las llamadas de perfilado de `Render`. Los nombres de
ámbito proceden de esos constructos C++, no de palabras hexadecimales en ASM.
Se elimina el bloque de 84 B de `.rodata`.

Se añaden cuatro retenciones anotadas por contenido en keep: `$LC130`, `$LC132`,
`$LC133` y `$LC134` de zFEng. `Setup` (`$LC131`) ya sobrevivía y no se retiene
adicionalmente. Ningún cambio en la cabecera Profiler ni flags.

Compilación productiva: mismas secciones ALLOC, alineaciones y 3.679 destinos
runtime normalizados; mismos proveedores públicos. **343/343 funciones exactas**.
El nuevo enlace productivo reproduce el DOL original. zFEng ya estaba linked.

Evidencia: `scratchpad/codex_20260914_fengine_integration/` y la sonda original
`scratchpad/codex_20260914_upstream_audit/profile/`.

## 2. Sonido integrado, con la inferencia autorizada explícita

- `lbmpeg.cpp`: entero `MPEGuse_MMX`, constructor de
  `CMpegBase_DecodeMethod` y objeto estático `dummy`. Sustituyen el bloque ASM
  completo, incluidos sus **84 B de instrucciones** y datos de inicialización.
- `ssysinit.c`: se elimina `FoldDownTargetCell { value; tail; }` y su alias ASM;
  vuelve la definición real `FoldDownTarget gFoldDownTarget`, de 4 B.
- `sdspmix.c`: la tabla Dolby existente conserva tipo `[256][4]`, valores y
  tamaño; recibe `__attribute__((aligned(8)))`.
- Keep: `ssysinit.o:gap_07_804BED84_bss` se sustituye por
  `lbmpeg.o:_3Snd.dummy`. Se preservan las cuatro retenciones nuevas de FEngine.

**El usuario autorizó expresamente `aligned(8)` en este turno.** Reconstruye la
alineación efectiva deducida del binario; **no está demostrado que el fuente
original contuviese ese atributo**. La cualificación `Snd` de clase/dummy también
es una inferencia delimitada: sus tipos y direcciones sí aparecen en DWARF;
el namespace del entero está probado. No se inventa un puntero muerto ni un
objeto de relleno para ocupar los cuatro ceros anteriores a la tabla.

Las tres compilaciones productivas reproducen los candidatos privados. Sus
**16/16 funciones siguen exactas**; instrucciones y destinos de relocaciones
no cambian. El nuevo enlace, combinado con FEngine, conserva el DOL original.

| Objeto | Dirección final | Tamaño real |
|---|---|---:|
| gFoldDownTarget | 804BED80 | 4 B |
| dummy | 804BED84 | 1 B |
| MPEGuse_MMX | 80450550 | 4 B |
| SNDDRV_dolbypl2balances | 80450558 | 2.048 B |
| __MIXChannel | 804BED88 | 5.632 B |

La auditoría privada comprobó 28 relocaciones del enum en nueve objetos y
cuatro de la tabla Dolby en dos objetos, todas con addend cero. No se tocaron
cabeceras compartidas ni consumidores.

### Corrección validada de referencias BSS

La reextracción DTK se probó primero en dos directorios privados, con
`--no-update`: baseline y candidato generan 619 objetos y **617 son idénticos**.
Los dos targets productivos anteriores eran idénticos al baseline fresco.

Después se integraron únicamente:

- `splits.txt`: ssysinit BSS termina en `804BED84`; lbmpeg recibe
  `[804BED84,804BED88)`.
- Los dos objetos de referencia de ssysinit/lbmpeg y sus dos listados generados.
- `build/GOWE69/config.json`: sólo `data_size` 60→56 y 12→16 en esas unidades.
  Se conservan todas las rutas productivas; no se copia el JSON privado entero.

La identidad y orden de las 619 unidades, el grafo real, los objetos fuente
seleccionados, symbols.txt, configure.py y ldscript no cambian. El target DTK
llama al intervalo `pad_07_804BED84_bss`; no se inventa un símbolo original
`dummy` en symbols.txt. Los targets actualizados conservan código y relocaciones.

El primer reporte fuente/keep daba +32 B de datos matched en ssysinit. Al
asignar correctamente los 4 B BSS a lbmpeg, la mejora final es **+28 B**.
No es una pérdida de bytes del ejecutable: el DOL sigue siendo idéntico.

Evidencia: `codex_20260914_lbmpeg_boundary`, `codex_20260914_sound_integration`,
`codex_20260914_lbmpeg_resplit` y `codex_20260914_sound_metadata`, bajo scratchpad.

## 3. DefragmentPool: literal integrado, función aún pendiente

En `CarLoader.cpp` se recupera como argumento real del primer `bMalloc(128,...)`
la cadena `CarLoaderDefrag but with a really long debug name!!`. Se elimina su
bloque `.asciz` de **52 B**. No se modifica otra lógica de CarLoader.

Se recompiló zWorld desde la ruta real: secciones ALLOC, símbolos públicos y
**11.581 relocaciones runtime** iguales al baseline. El reporte conserva
**578/582 funciones exactas**. DefragmentPool sigue en **99,269005 %**, 684 B.
Las otras pendientes son UpdateWheelYRenderOffset, RenderFlaresOnCar y
SetMemoryPoolSize. No se presenta la limpieza del literal como cierre de función.

**zWorld continúa NonMatching; el enlace productivo usa el objeto extraído.**
No se cambió su keep, sus splits ni su selección. Un enlace nuevo con el grafo
real posterior a FEngine/sonido reproduce el original.

Para un futuro ensayo desde fuente hay una deuda explícita: seis reglas `$LC`
de keep deben remapearse por contenido y debe retenerse el nuevo `$LC1106`.
`lcfix.py zWorld --check` sobre el objeto fuente detectará esos seis desfases;
no equivale a un fallo del DOL productivo. **No ejecutar autocorrección global
ni promover zWorld sin resolver las cuatro funciones y validar el enlace.**

La receta precisa está en `scratchpad/codex_20260914_defrag_integration/report.md`.
La prueba privada desde fuente con keep condicionado conserva su baseline DOL
`2494248b4abaf6e53f3944bae9cd055382342b4d`, que **no es el original**. Sirve como
prueba de no-regresión, no como autorización de promoción.

## Auditoría final y continuación

El censo léxico de 3.679 fuentes/cabeceras confirma:

- Dos bloques de datos ASM menos: 84 + 52 = **136 B de literales**.
- Un bloque de instrucciones ASM menos: **84 B**, lbmpeg.
- Un nombre ASM menos: el alias de la celda artificial de ssysinit.
- Ninguna barrera, pin o bloque ASM nuevo.

Quedan 458 bloques clasificados `data_or_gap` en el censo del proyecto/terceros.
El censo también incluye SDK y plataformas ajenas: no equivale a una lista de
ASM injustificado que se pueda borrar indiscriminadamente.

Gate final del estado combinado, con **4.644 archivos protegidos**:

```text
python scratchpad/codex_20260914_integrated_cleanup/final_gate.py
```

Guarda `post.json`, `result.json`, censos antes/después, `integrated.patch` y
`pending.json`. Los gates de fases anteriores son históricos y no deben exigir
sus snapshots PRE después de esta integración. HEAD e índice permanecieron
intactos; se preservaron cambios locales ajenos. `git diff --check` de los siete
archivos fuente/config de esta tanda pasó; sólo emitió avisos de autocrlf.

Quedan **25 cuerpos realmente pendientes, 30.744 B**, más una entrada de padding
de 20 B en el reporte. El siguiente frente recomendado es **eProject (268 B)**,
con DWARF original y comparación causal de expresión/temporales; después,
SetMemoryPoolSize/DefragmentPool en zWorld e IdctRow/IdctColumn. No repetir
barridos de pines/barreras ni interpretar pseudocódigo Ghidra como prueba de match.
La exportación completa y los sidecars DWARF de la ronda anterior siguen intactos.
