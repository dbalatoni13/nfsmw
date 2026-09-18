# r75: la teoria del compilador ausente, refutada; y el inventario exacto del gap

## 1. REFUTACION EMPIRICA DE "FALTA v1.72/v1.83"

`docs/analisis/compiladores-que-faltan.md` proponia conseguir los compiladores
v1.72 (rcmp) y v1.83 (path). Hoy se ha medido y NO HAY NADA QUE CONSEGUIR:

| unidad | 3.9.3 (v1.76) | 3.8.1 (v1.54) | 3.7 (v1.46) |
|---|---|---|---|
| madidct (fuente r68, misma entrada) | .text 3df2d512784e | .text 3df2d512784e | .text 3df2d512784e |
| pathnode | 100,00 % | 100,00 % | 100,00 % |

El codegen de SN para GC esta CONGELADO de v1.46 a v1.76 para este codigo
(.text y .data con el MISMO SHA-1 en las tres versiones). v1.72 esta
encajonado entre versiones identicas; path ya matchea al 100% con v1.76.
SONDAS: `Temp/sonda_versiones_madidct.py`, `Temp/sonda_versiones_pathnode.py`.

Ademas el COMPDIR del DWARF identifica los builds exactos:
- v1.72/rcmp: `D:\env\x3rdparty\gc\cur\SNSystems\ngc\bin` (RAD, Bink/MAD)
- v1.83/path: `C:\packages\GameCubeSN\3.9.3-4-non-proxy\sdk\bin` (EA audio,
  un 3.9.3 parcheado interno "-4")

CONCLUSION: las vedas restantes son TODAS de forma de fuente con el compilador
que ya tenemos. La informacion esta en el DWARF-1 del ELF (mapa de registros
por local, arbol de inlines, orden de declaracion), no en un binario perdido.

## 2. INVENTARIO EXACTO DEL GAP (informe fresco, post-r74)

**99,3917 % de codigo** (3.922.044/3.946.048 B). 25 funciones abiertas,
de las cuales 23 con datos por funcion:

| funcion | unidad | B | match |
|---|---|---|---|
| __static_initialization_and_destruction_0 | zCamera | 3604 | 99,84 |
| HolePunchAvoidables | zWorld2 | 2980 | 99,97 (veda cr2/cr3 r73) |
| Initialize DynamicLoader | zEagl4Anim | 2352 | 99,74 |
| epCalculateLocalDirectionalPOS16 | zEcstasy | 2072 | 93,31 |
| UpdatePlatInfo | zEcstasy | 2044 | 99,96 |
| GenerateRoadNoise | zEAXSound2 | 1240 | 96,80 |
| Update TrackCarCameraMover | zCamera | 992 | 99,63 (veda r61/r63) |
| SimThread_Step | steering | 924 | 95,67 |
| UpdateWheelYRenderOffset | zWorld | 876 | 98,93 |
| UpdateLoaded Tire | zPhysicsBehaviors | 856 | 97,21 |
| InitAtSegment | zWorld2 | 816 | 99,66 |
| GetLoadingPriority | zTrack | 708 | 94,97 |
| DefragmentPool | zWorld | 684 | 99,27 |
| IdctColumn | madidct | 632 | 50,45 |
| HandleTriggers | steering | 588 | 98,30 |
| IdctRow | madidct | 516 | 46,37 |
| EvalState FnRawStateChan | zEagl4Anim | 456 | 98,07 |
| SimThread_Init | steering | 324 | 94,69 |
| SetMemoryPoolSize | zWorld | 304 | 97,37 |
| Effect_Init | steering | 276 | 95,00 |
| eProject | zEcstasy | 268 | 93,97 |
| CookValues | steering | 260 | 99,08 |
| Effect_PerformEnvelope | steering | 196 | 93,27 |

REPARTO de agentes en curso: ronda-4 (steering x6, zTrack, zEAXSound2,
zPhysicsBehaviors = 9), madidct (2), epCalculate+zEagl4Anim (3). Libres:
zCamera x2, zWorld x3, zWorld2 x1 (HolePunch aparcada).

## 3. Hallazgos de hoy por funcion (no agentadas)

### UpdatePlatInfo (99,96 %): la fila 98 es una conmutacion con CONTRAPARTIDA
La unica fila real es `fmuls f26, f5, f10` (objetivo) contra nuestro
`fmuls f26, f10, f5`: la linea 642 de eLightE.cpp
(`envmap_min_b = EnvmapMinB * envmap_min_scale`) tiene la escala en segundo.
Des-conmutarla (escala primero, como las filas 94/96 de min_r/min_g) BAJA la
funcion a 99,7456 %: la conmutacion sostiene ~6 filas del switch de abajo.
La fila 98 queda como veda CON mecanismo: el orden de operandos del DEF
controla a la vez el def (fila 98) y los usos del switch. Medido 2026-09-16.

### eProject (93,97 %): el orden del DWARF es neutro, el pin sostiene
El DWARF declara oneOverW ANTES que clipX/clipY/clipZ (nuestra fuente lo
tenia despues). Migrado a `float oneOverW = eRecip(-eye.z);` antes de los
clips: CON el pin +f(halfVP2) el objeto es IDENTICO (93,97 %); SIN el pin,
86,12 %. El pin sigue siendo irreducible por esta via. Comentario r65/r67
condensado en la fuente.

### TrackCarCameraMover::Update (99,63 %): sin cambios
Revisado contra los comentarios r61/r63 ya presentes en la fuente: R1 es
permutacion de sched2 (los stores a mano empeoran: 95,98 %), R3 necesita
f0+f13 ocupados y la unica palanca medida (doble fantasma T1) mete el cierre
dentro de R3 y ademas seria FABRICAR PIN NUEVO, direccion contraria a la
directiva de eliminar asm. Aparcada como estaba.

## 4. Reparacion de build
src/LibSN/ppc2D2.c tenia un bloque de comentario r70 sin el abridor `/*`
(corrompido en el commit ea7535f9; el .o fresco lo tapaba). Reparado: un
build desde cero vuelve a compilar. OJO: revisar futuros commits de comentarios
multilinea con este patron.

## 5. Herramienta nueva
`tools/scratch/diverg.py <json-objdiff> <subcadena>`: alinea y muestra los
bloques de divergencia real entre original y nuestro, normalizando saltos
(`beq 0x12bf0` == `beq Sym+0xb40`) y etiquetas ($LC vs lbl_). Usado para
separar filas cosmeticas de reales en UpdatePlatInfo/eProject/zCamera.
