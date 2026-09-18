# La cola estaba mal ordenada: el enlazador descarta código, y mucho

Salió tirando del hilo del orden de emisión del mapa de PS2, pero el hallazgo es
otro y es más grande.

## Lo que medíamos mal

`promodist.py` y `datacmp.py` comparan los **tamaños de sección de los objetos**.
Pero `LDFLAGS` es `-strip-unused-data -keep config/GOWE69/keep.lst`, y eso **se
lleva todo lo que no se referencia, código incluido**. Dos medidas:

    zLua        .text del objeto +18.908 B   ->  enlazado: +0 B
    zAttribSys  emite 3 funciones que el objetivo no tiene (156 B) -> +0 B

O sea que llevábamos rondas leyendo como «le sobran 18 kB de código» algo que al
enlazador **no le cuesta un byte**.

## La medida buena, y la cola nueva

`scripts/linkdelta.py` enlaza el proyecto entero sustituyendo **una** unidad cada
vez (2,6 s) y compara las secciones del ELF contra el enlace base. Con eso,
**20 de 28 SourceLists tienen el `.text` a delta 0**:

    unidad             .text   resto
    zGameModes            +0   rodata+104
    zMiscSmall            +0   rodata+104
    zBWare                +0   rodata-248 data-64
    zAttribSys            +0   rodata-584 data-32
    zFEng                 +0   rodata-664 data-64
    zRender               +0   rodata-792
    zEcstasy              -8   rodata-760 data+64 bss+32
    zEagl4Anim           +12   rodata-864 data-96 bss-128
    zFoundation           +0   rodata-1160 data-192
    zSpeech               -4   rodata-1424 bss+32
    zWorld2               -4   rodata-1264 data-256 bss+64
    zPhysics              -8   rodata-1568 bss-32
    zAnim                 +0   rodata-1552 data-192 over-16
    zSim                  +0   rodata-1568 data-224
    zEAXSound2            +0   rodata-1760 data-288 bss-32
    zPlatform             -8   rodata-1936 data-64
    zAI                   -4   rodata-1728 data-384
    zMisc                 +0   rodata-2240 data+32
    zCamera              +16   rodata-1904 data-224 bss+32
    zLua                  +0   rodata-2048 data-320
    zEAXSound             +0   rodata-2144 data-416 bss+64
    zTrack                +0   rodata-2120 data-416 bss-160
    zPhysicsBehaviors     +4   rodata-1504 data+416 bss-988
    zMain                 +0   rodata-2888 data-96 bss+64
    zFe                   +0   rodata-3176
    zGameplay             +0   rodata-3080 data-128
    zWorld                -4   rodata-4712 data-128
    zFe2                  +0   rodata-4824 data-160 over-16

Los deltas de `.text` que quedan son de **4 a 16 bytes** en seis unidades. Todo lo
demás es `.rodata`. **El frente del match ya no es el cuello de botella; el dato
sí.**

## Cómo se llegó: el mapa de enlazador del alpha de PS2

`orig/SLES-53558-A124/NFS.MAP` (2,78 MB) lista por objeto todos sus símbolos con
dirección y tamaño, y sus objetos son **34 SourceLists + 4 de ensamblador**: el
original ya se compilaba así también en PS2. Tiene además una `zComms.cpp` propia
que en GameCube `splits.txt` mete dentro de `zDebug`.

`scripts/ps2map.py --tres` cruza los tres órdenes (objetivo, el nuestro, PS2) y
marca, de las funciones que nosotros tenemos descolocadas, **cuáles corrobora
PS2** respecto de sus vecinas. Si dos plataformas coinciden y nosotros no, el
orden lo decide la fuente y vale la pena buscar la causa; si PS2 tampoco
coincide, es ruido de versión. Primer barrido: `zTrack` y `zBWare` con **1
descolocada y 0 testigos** (orden ya bueno), `zLua` con 52 testigos, `zWorld2`
con 17 (racimo de `WRoadNetwork`), `zPlatform` con 13.

Fue mirando el racimo de `zLua` cuando salió lo de arriba: su estructura de
bloques por fichero reproduce **exactamente** la del objetivo, pero tenemos 704
funciones frente a 537. Las 167 de más resultaron ser peso muerto.

## El caso testigo de que el estripado no deja residuo en el original

`zGameModes` es el más pequeño (124 B de `.text`) y el más ilustrativo. Nuestro
objeto emite 248 B de `.rodata` y 64 de `.data`: tres tablas de nombres de
depuración (`aEmotionalSummaryTypeStrings` y compañía) con sus 21 literales. El
objeto extraído del objetivo **no tiene ni `.rodata` ni `.data`**, y `splits.txt`
no le declara esos rangos.

No es que el original no las tuviera: el DWARF las lista **con dirección
`0xFFFFFFFF`**, que es como el enlazador de SN marca lo que ha descartado. O sea
que el original las emitía y `-strip-unused-data` se las llevó **enteras**. A
nosotros nos sobreviven 104 B de residuo de estripado (`size % 8` más relleno),
y sólo tres símbolos de veinticinco.

**Ese `0xFFFFFFFF` es un marcador reutilizable**: distingue «el original no tenía
esto» de «el original lo tenía y se estripó», que hasta ahora se confundían.
