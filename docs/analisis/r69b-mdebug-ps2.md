# r69b: EL ORÁCULO DE FUENTE — el .mdebug del ELF de PS2 (SLES-53558-A124)

## Qué hay

`orig/SLES-53558-A124/NFS.ELF` (91,9 MB) lleva **84,5 MB de `.mdebug`** ECOFF
con los stabs del build de PS2. El codigo MIPS no sirve (otra arquitectura),
 pero cada funcion del juego lleva en su region de simbolos:

| registro | qué da |
|---|---|
| `st=6` (stProc) | la funcion, con direccion |
| `nombre:F22` | la signatura (F22 = devuelve tipo 22) |
| `this:P7566`, `dT:P14` | **parametros con tipo y offset** |
| `distance:r14` | **locales con registro** (r14 = $f14 en MIPS) o offset de pila |
| `$LBB9318/$LBE9318` | **bloques anidados** — la estructura de ambitos de los inlines |
| `t:r36`, `o:r36` | las locales de las funciones INLINED (con su bloque) |
| `FILE` + `$LM` | **la tabla de lineas**: cada `$LM` lleva la LINEA en el campo index y el FILE previo da el fichero — la expansion inline completa en orden |

Y las rutas originales: `d:/p4_apex1666_d1001856/mw/speed/indep/src/...`
(deposito Perforce `p4_apex1666`, build D1001856, todo `indep` compartido
con GameCube).

## La herramienta

    python scripts/mdebug.py fn <nombre>        # estructura + lineas de una funcion
    python scripts/mdebug.py files [patron]     # rutas de fuente

Los offsets de las tablas estan calibrados a este fichero (la cabecera HDRR
de esta toolchain no sigue el orden IRIX; se validaron por contiguidad):
PDR `0x55f804` (los PDRs resultan ser del tramo GNU-as, no cubren el codigo
MW), SYMR `0x6f5348` x1.896.029, strings `0x1ce6f08`.

## Cobertura y límites

- **`speed/indep/` completo** (el arbol compartido): TrackCar, ICEMover,
  RenderFlaresOnCar, EvalState, DynamicLoader, CarLoader, WRoadNav...
- **NO cubre** `Speed/GameCube/Src/` (eLightE, eMathE, JoyE...): el PS2 tiene
  sus propios ficheros de plataforma.
- El build es **A124, anterior al final GC**: las sentencias anadidas tarde
  (p.ej. los dos `bTan` muertos de TrackCar, el `fov` local) NO estan. Para
  lo que no cambio entre A124 y el final, los locales/tipos/lineas del PS2
  son mas ricos que el DWARF-1 de GameCube (tipos completos via stabs
  `name:tN=...`, bloques, registros).

## Uso contra las vedas

1. `mdebug.py fn <simbolo>` da la lista de locales del ORIGINAL con tipos y
   ubicacion (pila o registro) —比对 con `regmap.py` del lado GC.
2. La secuencia `FILE/$LM` es el plan.py de PS2: el orden exacto de
   expansion inline por sentencia. Si el original llama algo que nuestro
   arbol DWARF GC no tiene, aqui sale con FILE y LINEA.
3. Los bloques `$LBB/$LBE` con sus locales dan la estructura de ambitos de
   cada inline — util para reconstruir formas de fuente.

## Pendiente

- Resolver los numeros de tipo (`:95`, `:2113`) contra los stabs de tipos
  (`name:tN=...`) para tener los tipos CON nombre.
- Decodificar el tramo stabs puro (los `.ii` de GNU) para las libs de runtime.

## Mapa de cobertura medido (r69, catastro rapido)

Presentes en A124: `Update__19TrackCarCameraMoverf` (con `distance:r14`,
`vert_comp:r14` en registro y ~50 bloques inline), `GetPercentRaceComplete`,
la familia `e*LightFlare` de PS2...

AUSENTES (anadidos despues de A124, o refactorizados): los dos `bTan` muertos
de TrackCar, el local `fov` de TrackCar, `RenderFlaresOnCar__13CarRenderInfo`
(PS2 A124 usa `eRenderLightFlare__FP5eViewP11eLightFlare...` a secas).

**La palanca para ampliar cobertura**: un dump del PS2 RETAIL final
(SLES-53558) o del prototipo XBOX A138 (`MW_A138_XBOX.iso`, mas cercano al
final) con debug info alistaria las sentencias tardias. El XEX de Xbox suele
venir stripped; el ELF de PS2 retail es el candidato natural.

## r70: plan de continuacion (lo que hay y por donde)

Frente LINKED (10 unidades con codigo 100% bloqueadas por datos):

| unidad | codigo | data-gap |
|---|---:|---:|
| zPlatform | 29.380 | 96.888 |
| zPhysics | 146.124 | 36.188 |
| zFe2 | 250.732 | 34.132 |
| zAI | 272.796 | 31.312 |
| zEAXSound | 151.092 | 26.020 |
| zFe | 174.200 | 23.176 |
| zSpeech | 178.888 | 19.928 |
| zMisc | 78.008 | 10.028 |
| zLua | 94.264 | 9.464 |
| zGameplay | 141.472 | 6.216 |

Orden por rentabilidad:
1. **zMisc**: quedan 11 cadenas (115 B) de las 150 de la r52 — palanca
   `_bwarePrefix + offset` (dupstr.py da la lista). Transcripcion pura.
2. **zAI**: 9 cadenas de mas (173 B) — mismo mecanismo.
3. **claimdata**: hoy 0 reclamables sin ambiguedad; revisar tras (1) y (2).
4. Las DOS FRONTERAS de ventana-pendiente §2 (zGameplay/zGameModes 848 B en
   paquete atomico + prefijo devuelto; zEAXSound2/zEcstasy 128 B) desbloquean
   promociones de racimo.
5. steering: las tres ranges de splits + re-extraccion con sha1 del DOL.

Procedencia (cerrada hoy para ppc2D2): MWCC 2.7/3.0a5.2 llaman a su runtime,
los 4 ProDG pliegan el @l igual — ningun compilador disponible da el addi.
No repetir; la constante es ademas problema de splits (0x8040F1B8 sin dueno).

## r71b: NFS MW Black Edition (PS2, SLUS-213.51) — STRIPPED, pero sirve de calibrador

El ISO de PS2 (que el repo oficial tambien usa) contiene:
- `SLUS_213.51` / `SCE_DD.ELF` (4.918.740 B, identicos): ELF MIPS **sin debug**
  (las secciones grandes que parecian symtab son padding SCE 0x7ffff420).
  Extraido a `orig/SLUS-21351/`.
- `FRWL.ELF` (From Russia With Love, 5,6 MB): `.mdebug.eabi64` de tamano 0 —
  stub, tambien stripped. Comparte middleware (rcmp/realshape/snd) pero al ser
  MIPS no aporta codegen PPC.
- IRX/IOP y .BIN de datos: nada util para GC.

**Valor real del SLUS final**: diff de funciones contra el alpha A124. Las
funciones que CAMBIARON entre A124 y el final delimitan donde el mdebug de
A124 (nombres de locales, lineas) NO es fiable para el codigo final — y las
que no cambiaron lo confirman como evidencia valida. Es el calibrador de
uso del oraculo A124.

**Jerarquia de oraculos tras esta comprobacion** (ninguno sustituye al otro):
1. GC `NFSMWRELEASE.ELF` .debug (92 MB) — FINAL, compilador exacto, TODO.
2. PS2 A124 mdebug (84 MB) — alpha, indep/, tipos ricos.
3. PS2 SLUS-213.51 (stripped) — calibrador de cambios alpha->final.
4. XBOX A138 — diccionario de nombres.

## r71d: LA EXTENSION EER LO RESUELVE TODO (analisis completado)

Instalada ghidra-emotionengine-reloaded v2.1.28 (chaoticgd — la que recomienda
el repo oficial) en nuestro Ghidra 11.4. El analisis headless del NFS.ELF PS2:

- **23.576 funciones, 23.479 con nombre real** (99,6%)
- **18.575 tipos de datos, 8.994 estructuras/uniones** con miembros y offsets
- Overlays VU0/DVP con sus simbolos (74+61+49+... por overlay)
- Artefacto permanente: `tools/scratch/eer_tipos_ps2.txt` (61.821 lineas,
  TODAS las estructuras). Script de export: `tools/scratch/eer_export.py`
  (ojo: Jython = Python 2, sin encoding= en open()).
- Proyecto Ghidra guardado: %TEMP%/ghidra_eer/eerPS2 (reutilizable con
  -process sin re-analizar).

Muestra de calidad: `/ICEMover (len=320)` con Next/Prev/Type/ViewID... y su
vtable completa. Esto resuelve los numeros de tipo que mdebug.py dejaba sin
decodificar (:95, :2113, :7566...).

**Jerarquia de oraculos actualizada**: GC .debug (final, todo) > PS2 EER tipos
(alpha A124 pero con el sistema de tipos COMPLETO) > SLUS calibrador > A138
diccionario.
