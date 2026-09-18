# r63 — La extension ghidra-dwarf1 sobre nuestro DWARF-1

**Veredicto: FUNCIONA, y cambia el frente Ghidra por completo — pero solo despues
de un parche de 12 lineas.** Sin el parche importa CERO: revienta en el primer DIE.

Medido el 2026-09-10 sobre `orig/GOWE69/NFSMWRELEASE.ELF` con Ghidra 12.1.2 y
Ghidra 11.4 DEV. Todo lo de abajo tiene su comprobacion.

---

## 1. Compila e instala en 11.4 DEV — SI, sin tocar una linea de API

`emoose/ghidra-dwarf1`, commit `5b0666c` ("11.3 fixups, forward-port
ElfSectionProvider from old ghidra"). No hay zip de release para 11.4 (el ultimo
es la tag `11.3.1`), asi que se compila:

    export GHIDRA_INSTALL_DIR=<install>
    gradle --offline buildExtension        # gradle 8.13 de ~/.gradle/wrapper/dists

`BUILD SUCCESSFUL`, cero errores, en las DOS instalaciones:

| Ghidra | zip generado |
|---|---|
| 11.4 DEV 20250425 | `ghidra_11.4_DEV_20260910_ghidra-dwarf1.zip` |
| 12.1.2 PUBLIC     | `ghidra_12.1.2_PUBLIC_20260910_ghidra-dwarf1.zip` |

No hizo falta tocar `extension.properties` (el token `@extversion@` lo rellena
`buildExtension.gradle` del propio Ghidra). El unico riesgo de version era el
import `ghidra.app.util.bin.format.dwarf.sectionprovider.ElfSectionProvider`
(paquete renombrado de `dwarf4` a `dwarf` en 11.3); el fork ya lo trae
reimplementado en su propio arbol, asi que compila igual en 11.4 y en 12.1.2.

## 2. ¿Parsea nuestro DWARF? NO de fabrica. Y la trampa NO era la que esperabamos

La sospecha del encargo era `.debug_srcinfo` / `.debug_sfnames`. **Es falsa**:
la extension lee **una sola seccion**, `.debug` — `DWARF1SectionNames.java` define
exactamente una constante, `DEBUG = "debug"` — y ni mira las otras seis. Las
extensiones de GCC a nivel de SECCION nos dan igual.

La trampa real es de ATRIBUTO, no de seccion. `gcc/dwarf.h:184-189`:

    AT_sf_names    = (0x8000|FORM_DATA4),
    AT_src_info    = (0x8010|FORM_DATA4),
    AT_mac_info    = (0x8020|FORM_DATA4),
    AT_src_coords  = (0x8030|FORM_DATA4),
    AT_body_begin  = (0x8040|FORM_ADDR),
    AT_body_end    = (0x8050|FORM_ADDR)

Esos seis viven en **0x8000-0x8050**, y el rango de usuario de la spec DWARF-1 es
`AT_lo_user=0x2000 .. AT_hi_user=0x3ff0`. `AttributeName.decode()` no los conoce y
**lanza**, dentro del constructor de `DebugInfoEntry`, donde nadie captura
(`process()` solo captura `IOException`). Resultado, literal:

    EXCEPCION AL ANALIZAR: java.lang.IllegalArgumentException: invalid attribute value 32768
        at com.github.rafalh.ghidra.dwarfone.model.AttributeName.decode(AttributeName.java:77)
        at com.github.rafalh.ghidra.dwarfone.model.DebugInfoEntry.<init>(DebugInfoEntry.java:37)
        at com.github.rafalh.ghidra.dwarfone.DWARF1ProgramAnalyzer.processDebugSection(...:80)
    process() = false  en 0 s

`32768` = `0x8000` = `AT_sf_names`, y esta en **el DIE de desplazamiento 0**: el
primer `TAG_compile_unit` del fichero. Cero DIEs importados, cero tipos, cero todo.

Censo de mi replica del parser (`scratchpad/ghidra63/dwarf1/work/replica.py`,
recorre los 91.947.256 B de `.debug`, 2.502.870 DIEs):

    atributos que hacen throw:  0x8000 x316   0x8010 x316
                                0x8040 x30267 0x8050 x30267
    tags que hacen throw:       ninguno
    formas invalidas:           ninguna

Es decir: **solo esos cuatro**, y con arreglarlos el formato es DWARF-1 de spec.
`0x8000`/`0x8010` salen 316 veces = una por unidad de compilacion; `0x8040`/`0x8050`
(`AT_body_begin`/`AT_body_end`) 30.267 veces, una por funcion con cuerpo.

### El parche (12 lineas) — `r63-dwarf1-gcc-ext.patch`

Da nombre a los seis atributos de GCC y hace tolerantes los dos `decode()`
(devolver `USER` en vez de lanzar). Nada mas. Con el:

    process() = true  en 47 s     (12.1.2)
    process() = true  en 39 s     (11.4 DEV)

## 3. Que entra de verdad

Proyecto 12.1.2 (copia del del usuario, con su auto-analisis completo):

| | ANTES | DESPUES |
|---|---:|---:|
| funciones con firma (`SourceType.IMPORTED`) | 86 | **13.824** |
| funciones con >=1 parametro | 83 | **12.968** |
| structs | 11 | **4.076** (3.802 con campos) |
| unions | 0 | 49 |
| enums | 2 | **2.872** |
| function-defs | 2 | 189 |
| tipos totales en el DTM | 271 | **10.896** |
| datos con tipo real | 36.193 | 39.098 |

En 11.4 DEV, sobre un import limpio: 13.738 firmas, 4.069 structs, 2.872 enums,
10.853 tipos. Mismo resultado.

### Contraste con el techo teorico del `.debug`

`scratchpad/ghidra63/dwarf1/work/replica2.py` cuenta lo que HAY:

    DIE subroutine/global_subroutine        : 591.192
      sin nombre o sin low/high_pc          : 572.349   (declaraciones repetidas por TU)
      low_pc tombstone 0xFFFFFFFF / 0       :   4.893   (dead-stripped)
      IMPORTABLES                           :  13.950
      direcciones unicas                    :  13.950
      parametros formales                   :  27.604   (27.530 con nombre)

    structure_type 77.202 / class_type 0 / union_type 1.395 / enum 24.846
      agregados unicos (tag,nombre,tamano)  :   6.674
      members + inheritance                 : 298.255

    global_variable 22.628, con AT_location  :   9.918

La extension recoge **13.824 de 13.950 funciones importables (99,1 %)**. Los 126
que faltan son los que chocan (`OverlappingFunctionException`: un DIE cuyo rango
pisa a otra funcion ya existente); se ven en su `MessageLog`.

De tipos recoge 4.125 agregados de 6.674 (62 %): **la importacion de tipos es
PEREZOSA**, solo entran los alcanzables desde una funcion o una global.
`DWARF1ProgramAnalyzer.processDebugInfoEntry` solo llama al importador de tipos
para `CLASS_TYPE` y `ENUMERATION_TYPE`, y **nuestro GCC no emite ni un
`TAG_class_type`** (0 de 2,5 M DIEs; las clases C++ salen como
`TAG_structure_type`). Meter `STRUCTURE_TYPE`/`UNION_TYPE` en ese switch es la
mejora obvia siguiente — no medida todavia.

Contraste con `symbols/mw_dwarfdump.nothpp`: el volcado dice
`mFlags // offset 0x24`, `mSpikeOffset // offset 0x28`, `mNumBlocks // offset 0x30`
para `RoadblockFlow`, y son EXACTAMENTE los nombres y desplazamientos que el
decompilado usa. No inventa.

## 4. EL CONTROL: `Setup__Q26Speech13RoadblockFlow` @802a2248

Los dos decompilados completos estan en `r63-dwarf1-decomp.txt`. El criterio del
encargo se cumple:

    SIN:  if (((*(uint *)(param_1 + 0x24) ^ 1) & 1) == 0) {
    CON:  if (((this->mFlags ^ 1) & 1) == 0) {

    SIN:  dVar6 = (double)bRandom__Ff(0x3ff0000000000000);
    CON:  fVar8 = bRandom(1.0);

    SIN:  IsCopSpeechPlaying__Q26Speech7Manager18SPCHType_1_EventID(0x4d)
    CON:  Manager__IsCopSpeechPlaying(kSPCH1_EventID_DispRBReply)

    SIN:  RBPosition__10MiscSpeechi(*(undefined4 *)(param_1 + 0x28));
    CON:  MiscSpeech__RBPosition(this->mSpikeOffset);

    SIN:  if ((*(int *)(param_1 + 0x30) < 2) || ...
    CON:  if ((this->mNumBlocks < 2) || ...

Y en `SetMemoryPoolSize__9CarLoaderi` @802dd360, mejor todavia:

    SIN:  bInitMemoryPool__FiPviPCc(...,&DAT_8040caa4);
    CON:  bInitMemoryPool(CarLoaderMemoryPoolNumber, this->MemoryPoolMem,
                          this->MemoryPoolSize, "Cars");

    SIN:  if (*(int *)(param_1 + 0x58) != param_1 + 0x58) return;
    CON:  if ((this->LoadedRideInfoList).__base.HeadNode.Next != (bNode *)&this->LoadedRideInfoList)
            return;

**La unica parte del criterio que NO se cumple: las llamadas virtuales.**
`(**(code **)(*piVar2 + 0x2e4))(...)` no se convierte en
`cop->PursuitApproaching()`; se queda en
`(**(code **)&p_Var7[0x5c].__delta2)(...)` sobre un `__vtbl_ptr_type`.
Es esperable: DWARF-1 **no describe la disposicion de la vtable**, solo marca las
funciones con `AT_virtual`. Para resolver eso hace falta el orden de vtable, que
es justo lo que el volcado de PS2 da (ver `nfsmw-ps2-como-referencia`).

## 5. Cobertura sobre las 27 funciones abiertas

`r63-dwarf1-cobertura.txt`, medido una a una: **22 de 28 tienen firma DWARF**.

Las **6 que no** son TODAS de `steering`: `SimThread_Init`, `SimThread_Step`,
`HandleTriggers`, `Effect_Init`, `CookValues`, `Effect_PerformEnvelope`. Encaja
con lo ya sabido (`nfsmw-catalogo-metrowerks`): steering no es GCC, y por tanto no
tiene DWARF-1. Su decompilado no cambia ni un caracter con la extension puesta:
lo comprobe con `SimThread_Init`, identico antes y despues.

Firmas ganadas que valen dinero, por ejemplo:

    802feab0  WRoadNav::HolePunchAvoidables(WRoadNav*, NavCookie* cookies,
                  int num_cookies, float current_offset, float delta_offset)
    802cb51c  CarRenderInfo::RenderFlaresOnCar(CarRenderInfo*, eView* view,
                  bVector3* position, bMatrix4* body_matrix, int force_light_state,
                  int reflexion, int renderFlareFlags)
    80109358  epCalculateLocalDirectionalPOS16(uint* colour_table0, uint* colour_table1,
                  int num_colour_entries, ushort* position_table_16, int* normal_table,
                  uchar* index_buffer, int vertex_description, int num_indicies,
                  eLightMaterial*, eLightContext*)
    8030183c  WRoadNav::InitAtSegment(WRoadNav*, short segInd, char laneInd, float timeStep)

## 6. NEGATIVO MEDIDO: no desbloquea los paired-singles

Puesta la extension, las 12 funciones con `psq_` SIGUEN truncadas. Medido
(`r63-dwarf1-paired-singles.txt`):

    802feab0 WRoadNav::HolePunchAvoidables
      instrucciones desensambladas en el cuerpo: 3 de 745 posibles
      -> halt_baddata();  /* Bad instruction - Truncating control flow here */
    802c9fb8 CarRenderInfo::UpdateWheelYRenderOffset : 3 de 219
    802bc0a4 TrackStreamer::GetLoadingPriority       : 3 de 177

La extension da el PROTOTIPO y el RANGO del cuerpo (`low_pc`/`high_pc`), que ya es
mas de lo que habia, pero el desensamblador para igual que antes.
**Los 22.964 B de las 12 funciones [PS] siguen necesitando el sleigh Gekko;
la extension resuelve las OTRAS 15 (9.116 B).**

## 7. Ficheros escritos fuera del repo

Instale la extension en las dos instalaciones. Los dos directorios
`Ghidra/Extensions/` **estaban VACIOS** (`ls -la` antes), asi que no sobrescribi
nada y no hizo falta copia de seguridad. Se creo, en cada uno, solo:

    <install>/Ghidra/Extensions/ghidra-dwarf1/
        extension.properties, Module.manifest, README.md, LICENSE,
        doc/dwarf_1_1_0.pdf, lib/ghidra-dwarf1.jar, lib/ghidra-dwarf1-src.zip

Instalaciones tocadas:

  - `C:\Users\jferr\Desktop\nfsdecompiled\ghidra_11.4_DEV_20250425\ghidra_11.4_DEV`
  - `C:\Users\jferr\Desktop\ghidra_12.1.2_PUBLIC_20260605\ghidra_12.1.2_PUBLIC`

Ambos jar instalados son **la version PARCHEADA**. Para quitar la extension basta
borrar esos dos directorios `ghidra-dwarf1/`.

El proyecto Ghidra del usuario (`ghidra_project/`) **no se toco**: se copio a
`C:\Users\jferr\AppData\Local\Temp\ghidra_dwarf1\` y todo el trabajo fue sobre la
copia (que ya queda con el DWARF importado y guardado). Hay ademas un proyecto
11.4 recien importado en `C:\Users\jferr\AppData\Local\Temp\ghidra_dwarf1_114\`.

## 8. Como repetirlo

    cd scratchpad/ghidra63/dwarf1/ghidra-dwarf1        # ya clonado, ya parcheado
    export JAVA_HOME="C:/Program Files/Microsoft/jdk-21.0.12.101-hotspot"
    export GHIDRA_INSTALL_DIR="<la instalacion que quieras>"
    "C:/Users/jferr/.gradle/wrapper/dists/gradle-8.13-bin/5xuhj0ry160q40clulazy9h7d/gradle-8.13/bin/gradle.bat" \
        --offline --console=plain buildExtension
    unzip -o dist/*.zip -d "$GHIDRA_INSTALL_DIR/Ghidra/Extensions/"

    # y sobre una COPIA del proyecto:
    export GHIDRA_HEADLESS_MAXMEM=24G
    <install>/support/analyzeHeadless.bat <projdir> <projname> -process -noanalysis \
        -scriptPath scratchpad/ghidra63/dwarf1/scripts -postScript Dwarf1Probe.java <salida> dwarf1

En la GUI: `Analysis > One Shot > DWARF1` (viene con
`setDefaultEnablement(false)` y `setSupportsOneTimeAnalysis()`, no se dispara solo).
