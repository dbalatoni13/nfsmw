# El alpha 138 de Xbox: qué aporta y qué no

`MW_A138_XBOX.iso` (prototipo del 8-oct-2005, Xbox original) es una **alpha 138**,
o sea *posterior* a la A124 de PS2 que ya usábamos y más cerca de la release.
Su `default.xbe` (4,9 MB) es un ejecutable x86 de MSVC, así que **para el codegen
no sirve de nada**: el compilador no tiene relación con GCC 2.9 PPC. Su valor
está en otra parte, y es real.

## Lo que aporta

### 1. La estructura original del árbol, con nombres exactos

El alpha conserva los `__FILE__` de sus asserts. **113 rutas de fuente
completas**, en cinco raíces de compilación distintas:

    D:\env\egami\realgraph\6\source\shape\cmn      (RealShape)
    c:\mwbuildmachine\speed\indep\src\feng          (27 ficheros, el frontend)
    C:\MyWork\packages\snd\9\source\library\cmn     (51 ficheros)
    d:\packages\realcore\6.24.00\source\file\cmn
    d:\p4_apex1666_d1001926\mw\speed\xbox\src\ecstasy

**Contraste con nuestro árbol**: los 27 de `feng` están todos, y los 3 de
`realcore` también. Sólo faltan cosas en dos sitios, y de esas **tres tienen
objeto en GameCube y están al 0%**, o sea que son trabajo escribible con el
nombre de fichero ya confirmado:

| unidad | bytes | funciones |
|---|---:|---:|
| `sgparse` | 520 | 1 |
| `ssysserv` | 320 | 3 |
| `ssysveccsismutex` | 124 | 3 |

`shpclone.cpp` (RealShape) y los tres de `online/` **no tienen objeto en
GameCube**: no existen en esta versión y no son trabajo. Y confirma de paso que
nuestros `creates.cpp` y `model.cpp` se llamaban en origen **`shpcreate.cpp` y
`shpelement.cpp`**.

### 2. Ciento cuarenta y nueve nombres `Clase::Método` exactos

Entre ellos los **tipos internos de `Attrib`**, que es donde más se pelea:
`CollectionHashMap`, `HashMapTable`, `DatabasePrivate`, `ClassPrivate`,
`ClassLoadData`, `CollectionLoadData`, `DatabaseLoadData`, `ExportPolicyPair`,
`AssetIDs`, `DataBlocks`, `attribute_data`, `layout`. Y nombres de **miembro**:
`EAGL4::SymbolPool::mpSymbolTable`, `Average::pData`, `AverageWindow::TimeData`.

Todo en `docs/analisis/xbox-a138-nombres.txt`.

### 3. Literales con su función, que es lo que no daba ninguna otra fuente

El macro de aserción del alpha emite **fichero + función + mensaje juntos**. Eso
convierte una cadena suelta del DOL en una pista con dueño.

## Lo que NO aporta, medido

**Las rutas `__FILE__` no están en el DOL de GameCube.** La release compiló los
asserts fuera. O sea que son documentación de la estructura, **no literales que
haya que reproducir** — y quien intente meterlas en la fuente emitirá `.rodata`
que el original no tiene (véase la trampa de realmemcard).

En cambio **los literales de verdad sí están en los dos**, y ahí el cruce vale:

| cadena | en el XBE | en el DOL |
|---|---|---|
| `ParticleSlotPool`, `EmitterSlotPool` | sí | **sí** (0x803DF254) |
| `EventSequencerSystems` | sí | **sí** (0x803F2304) |
| `Pivot Rot (Z)`, `Simple Image`, `Code List`… | sí | **sí** (0x803EAD10+) |
| `EAGL%d`, `merged shape` | sí | **sí** |
| rutas `__FILE__`, `Ecstacy::RVMBuffer`, `Snd::Memory::SetHeap` | sí | **no** |

La tabla de nombres de tipo de `FETypeLib` está **contigua** en el DOL a partir
de 0x803EAD10, así que se puede reconstruir en orden.

Nota: `EventSequencerSystems` lo encontró un agente por su cuenta esta misma
sesión para cerrar `EventSequencer::Create` (784 B). Que aparezca aquí confirma
el método, no lo adelanta.

## Veredicto

No es una fuente de codegen y no sustituye a nada. Es un **diccionario**: nombres
de fichero, de clase, de miembro y literales con contexto. Se usa cuando hay que
escribir una unidad de cero o reconstruir una cabecera, junto al DWARF y al `.s`
del troceador — nunca en su lugar.
