# Ghidra y DWARF1: revisión y candidato C++ de lbmpeg

Fecha: 14/09/2026. Continuación de `20260914-tanda2-asm.md`.
Se han revisado los recursos indicados por el usuario y se ha hecho una prueba
privada basada en evidencia nueva. **No se ha integrado código ni cambiado la
instalación o las bases de datos de Ghidra.** El gate POST de la tanda anterior
sigue vigente.

## Recursos que realmente están disponibles

Las rutas reales del repositorio son `ghidra_11.4_DEV_20250425`,
`ghidra_project`, `ghidra_scripts` y `docs/GHIDRA_NFSMW.md`; los guiones bajos
no son separadores de directorio. Se leyó completa la guía, incluidas las
secciones posteriores sobre DWARF1 y las tandas ya realizadas.

| Recurso | Resultado comprobado |
|---|---|
| `ghidra_11.4_DEV_20250425/ghidra_11.4_DEV` | Ghidra 11.4 DEV, build 2025-Apr-25; usa JDK 21 |
| Extensión exterior `Ghidra/Extensions/ghidra-dwarf1` | Ya instalada, metadatos versión 11.4; se cargó su `lib/ghidra-dwarf1.jar` |
| Copia anidada `ghidra-dwarf1/ghidra-dwarf1` | Metadatos 11.3.1; no es el JAR que cargó la prueba |
| Procesador | `PowerPC:BE:32:Gekko_Broadway`, con instrucciones paired-single |
| `ghidra_project/NFSMW` | 11.4 rechaza `/NFSMWRELEASE.ELF`: fue guardado con una versión más reciente |
| `C:/Users/jferr/AppData/Local/Temp/ghidra_r63both/NFSMW` | Abre correctamente en 11.4 con Gekko y metadatos DWARF1 ya importados |

La extensión es [ghidra-dwarf1 de Rafal Harabien](https://github.com/rafalh/ghidra-dwarf1).
Su autor explica que cubre DWARF1, no soportado por el analizador DWARF integrado,
y advierte de sus limitaciones y de hacer copia del proyecto antes de aplicarla.
**No hace falta instalar otra copia para este análisis.** No se ha intentado
convertir, actualizar ni forzar la apertura del proyecto incompatible.

La inspección de la base r63, con `-readOnly -noanalysis` y un script propio sin
setters, devuelve **10.898 tipos, 18.442 funciones y 13.824 firmas IMPORTED**.
Son cifras del programa Ghidra, no denominadores del reporte objdiff. En las
muestras se decodifican `psq_st` y las firmas de GenerateRoadNoise,
epCalculateLocalDirectionalPOS16, IdctRow y el inicializador de lbmpeg.

Dos correcciones a la lectura de la guía histórica:

- La afirmación inicial de que faltaba Gekko se refiere a otra instalación.
  La 11.4 proporcionada sí lo incluye y lo usa el proyecto r63 comprobado.
- Los primeros seis bytes de `.debug` son `00 00 00 9c 00 11`: longitud de DIE
  **156** seguida de **TAG_compile_unit = 0x11**, no «versión 17». Se comprobó
  el binario y la constante en el `dwarf.h` del GCC original del repositorio.

## Cómo aprovechar los scripts sin falsear la evidencia

- `Dwarf1Run.java` ejecuta el importador y modifica el programa; `SetGqr.java`
  modifica el contexto GQR0. No se han ejecutado aquí.
- `ExportOpen.java` puede crear funciones en memoria. `ExportDecomp.java` no las
  crea, pero tiene una selección alternativa por punto medio: verificar siempre
  entrada, rango y cobertura antes de atribuir su salida a una función.
- `ExportWalls*.java` usa listas históricas y límites de referencias. No es el
  inventario vigente de pendientes.
- Ya existen las exportaciones r63 en `scratchpad/ghidra63/both/out/` y las
  copias publicadas `docs/analisis/r63-ghidra-27-{decomp.c,asm.txt}`. No se ha
  repetido una exportación general de los mismos cuerpos.
- Los CSV completos de firmas y estructuras siguen en
  `C:/Users/jferr/AppData/Local/Temp/opencode/`. Un nombre IMPORTED no significa
  firma importada: `_IHandle__15IGenericMessage` conserva firma **DEFAULT**, retorno
  `undefined` y cero locales en la inspección actual. Volver a decompilar esos
  12 bytes no resuelve por sí solo su orden de emisión ni las regresiones de inline.

El DWARF no recupera texto fuente exacto ni decisiones del asignador de registros.
Sí puede aportar tipos, objetos, parámetros y relaciones inline que faltan en
una reconstrucción. Para ello conviene cruzar Ghidra con los DIE originales,
no tomar el pseudocódigo ni los rangos anteriores al stripping como verdad final.

## Hallazgo nuevo: lbmpeg sí tiene una reconstrucción C++ causal

La prueba anterior `int MPEGuse_MMX(0)` no generaba código. El DWARF permite
explicar por qué: la escritura runtime no procede de inicializar directamente
el entero, sino del constructor de un **objeto estático original**.

Original: `orig/GOWE69/NFSMWRELEASE.ELF`, SHA-256
`6ee16d55d92ad1b337a9b655989c32b2976f95e7c64e148cf83f482c613f79f7`.
Offsets DIE relativos a `.debug`, cuyo offset de archivo es `0x454E18`:

| DIE | Evidencia |
|---|---|
| `04F44BCF` | Estructura vacía `CMpegBase_DecodeMethod`, tamaño 1 B |
| `04F44C39` | Constructor abstracto inline de esa clase |
| `04F44E8F` | Instancia inline dentro de static-init, `[80366288,80366294)` |
| `04F44EAD` | `this` constante `804BED84` para esa construcción |
| `04F45760` | Objeto estático `dummy`, mismo tipo, dirección `804BED84` |
| `04F45697` | Entero `MPEGuse_MMX`, dirección `80450550` |

El rango inline contiene precisamente el store de cero. El candidato privado
usa la clase y el objeto documentados; **dummy no es un auxiliar inventado**:

```cpp
namespace Snd {
int MPEGuse_MMX = 0;
struct CMpegBase_DecodeMethod {
    CMpegBase_DecodeMethod() { MPEGuse_MMX = 0; }
};
static CMpegBase_DecodeMethod dummy;
}
```

La cualificación `Snd` está probada para el entero. Para la clase y el objeto es
una inferencia coherente, no texto fuente recuperado con certeza.

Compilación aislada con los flags reales de ProDG 3.9.3, repetida por root:

- **84 B de `.text` exactos**, funciones de 40 y 44 B, nombres y metadatos
  LOCAL/STT_FUNC iguales al target.
- **Cuatro relocaciones runtime exactas**, comparando tipo, posición y destino
  efectivo (sección + valor de símbolo + addend).
- C++ sin ASM, atributos, pines, barreras, cambio de flags o relleno inventado.
- `.data` propia de 4 B; `.bss` de sección de 4 B con objeto de 1 B; `.ctors` de
  4 B. No se afirma identidad completa con el objeto extraído actual.

Artefactos y auditor reproducible:
`scratchpad/codex_20260914_ghidra_lbmpeg/` contiene `lbmpeg.cpp`, `lbmpeg.o`,
`read_original.py`, `evidence.json`, `compile_once.py`, `compile_result.json`,
`neighbours.py`, `neighbours.json` y un informe con el detalle completo.

## Por qué todavía no se integra

Se sustituyó **sólo lbmpeg.o** en el grafo real de 619 objetos, enlazando a un
directorio privado con el keep y linker script actuales. El enlace termina,
pero el DOL no es exacto: mismo tamaño, 4.541.888 B, **2.050 bytes diferentes**,
SHA-1 `bbc6adb8cb346719f4644222f8bd5c55597962fb`.

El único símbolo global compartido con el ELF original cuya dirección cambia
es `SNDDRV_dolbypl2balances`: **80450558 -> 80450554**, desplazamiento de -4 B
de una tabla de 2.048 B. La fuente sdspmix emite `.data` alineada a 4; el objeto
extraído declara alineación 8. Es una pista para estudiar el padding, no motivo
para inventar una variable que rellene el hueco.

Dos fronteras deben resolverse conjuntamente:

1. **Datos 80450554..80450558.** Son ceros sin símbolo original ni DIE vivo que
   los identifique. Aranges asocia 4 B de datos del objeto a mpegl3base, pero su
   puntero `spTwoToNegativeQuarterPower2` tiene `OP_ADDR FFFFFFFF` en los DIE
   `04F51C07` y `04F560B9`: fue eliminado. No atribuirle esos ceros ni declararlo
   a cero basándose sólo en aranges. La propuesta r61c de frontera no demuestra
   que sobreviva ese objeto concreto.
2. **BSS 804BED84..804BED88.** El `dummy` de lbmpeg sí tiene dirección y tipo
   originales. Hoy ssysinit cubre ese intervalo ampliando artificialmente
   `gFoldDownTarget` de 4 a 8 B mediante `FoldDownTargetCell` y nombre ASM. El
   keep `ssysinit.o:gap_07_804BED84_bss` se refiere a la etiqueta extraída,
   inexistente en esa fuente. Restaurar el enum real y la propiedad del objeto
   requerirá comprobar retención, alineación y consumidores; no sumar otro
   objeto encima de la celda actual.

El próximo cierre útil es este grupo pequeño: lbmpeg, frontera de datos con
sdspmix y BSS con ssysinit. Primero justificar el almacenamiento y eliminar la
celda artificial; después probar todos los cuerpos afectados y el DOL. No
repetir variantes del entero inicializado a cero ni barridos de registros.

## Estado conservado y trazabilidad

| Métrica de producción | Sin cambios en esta revisión |
|---|---:|
| Código matched | 3.915.284 / 3.946.048 B = **99,22038 %** |
| Funciones exactas | 18.406 / 18.432 |
| Código linked | 1.264.380 B = **32,04168 %** |
| Unidades completas | 525 / 619 |

Los 84 B de lbmpeg ya contaban mediante ASM. El candidato no significa 84 B
nuevos de match ni linked y **todavía no retira ASM de producción**. Permanecen
25 cuerpos pendientes y los 20 B de padding contabilizados aparte.

El DOL de producción sigue con SHA-1
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`. No se han cambiado fuentes,
cabeceras, objetos live/target, configuración, splits, keep, grafo o reporte.
Tampoco se ha hecho git add ni commit. Los dos avisos de símbolos duplicados
del enlace privado son los históricos de `g_pEAXSound` y `PPCMtdec`.

En `scratchpad/codex_20260914_ghidra_review/`:

- `InspectNFSMW.java`, `repo_headless.log`, `dual_headless.log` y
  `dual_project.txt`: incompatibilidad del primer proyecto e inspección válida
  del segundo. El proceso rechazado devolvió exit code 0; hay que mirar el log,
  no aceptar ese código como prueba de apertura.
- `protected.json` / `protected_dual.json`: hashes de 4.594 / 4.604 archivos.
  Los controles posteriores a ambas aperturas y al enlace pasan sin cambios.
- `link_probe.py`, `link_probe/{result.json,link.rsp,link.log}`: prueba aislada
  de código, relocaciones y DOL. El directorio de resultados no se sobrescribe.
- `link_layout.py`, `link_probe/layout.json`: desplazamiento global observado.

Comprobaciones de lectura o recompilación exclusivamente privada:

```text
python scratchpad/codex_20260914_ghidra_review/protect.py check dual
python scratchpad/codex_20260914_ghidra_lbmpeg/compile_once.py
python scratchpad/codex_20260914_ghidra_review/link_layout.py
```

Los snapshots son inmutables: tras una integración futura habrá que crear una
fase nueva, no renovar estos hashes para que dejen pasar cambios posteriores.
