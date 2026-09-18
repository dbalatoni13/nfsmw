# r41 - coordinacion: literal VM real y pruebas de asignacion retiradas

2026-09-07/08. Base `590dc4a0` mas cambios locales r37-r40. Territorios de
coordinacion: `LibSN/vmbase.c`, `LibSN/vm.c` y `realcore/.../gc/dvd_device.cpp`.
No se modificaron configuracion, flags compartidos, splits ni cabeceras.

## Correccion retenida: una funcion al 100 % nominal tenia un literal falso

La auditoria estandar de `LibSN/vm` detecto una discrepancia REAL en
`__VMMappingErrorAlert`, aunque sus 56 B/14 instrucciones ya estaban al 100 %
en objdiff. La fuente usaba una cadena reconstruida de 40 B:

```text
VM Error: address 0x%08x is not mapped\n
```

Las dos relocaciones HA/LO del objeto extraido apuntan a **@311_80452860**,
con addend cero. La inspeccion directa de `NFSMWRELEASE.ELF` y
`symbols.txt:35936` confirma en **0x80452860** una cadena distinta de **103 B
incluido el NUL**:

```text
Virtual address (%x) has not been allocated. Call VMAlloc on virtual address ranges before using them.
```

Se reemplazo exclusivamente ese literal en `src/LibSN/vm.c`. No lleva salto
de linea. La funcion mantiene exactamente sus instrucciones, llamadas a
`sprintf`/`PPCHalt`, relocaciones, marco y firma. El formato `%x` tambien es
el del original, no `%08x`.

### Evidencia y alcance

- `codex_r41_vm_literal.py` resuelve las dos referencias originales y lee
  hasta el NUL desde el ELF: 103/103 B iguales a nuestro simbolo `@191`.
- `scripts/audit.py LibSN/vm __VMMappingErrorAlert` pasa ahora: **56 B, cuatro
  relocaciones y dos usos de literal**. Antes fallaba el contenido.
- `codex_r41_root_audit.py`: todos los simbolos fuente, instrucciones,
  relocaciones y secciones distintos de `@191`/su descriptor `.data` son
  identicos al baseline. `.data` solo contiene esta cadena y pasa de 40 a
  103 B; `.sbss` no cambia. No hay otro dato desplazado dentro de esa seccion.
- La fuente final equivale byte por byte a la copia inicial con una unica
  sustitucion del literal; se conserva su formato LF.
- **No contabilizar 56 B como nuevos matched ni 103 B como nuevos datos
  matched.** El codigo ya era nominalmente exacto y el split de vm no posee
  ninguna seccion de datos; los datos originales siguen en su auto-unidad.
- **No se promociona vm.** La funcion de asignacion sigue pendiente y faltan
  sus datos en el split. Su `.data` fuente correcta no implica un enlace
  valido de la unidad completa.
- La excepcion de nombre del auditor general en `VMAlloc`,
  `g_nextARAMPageToCheck$233_804FFD9C` frente a `nextARAMPageToCheck$155`, queda
  certificada por una subauditoria independiente: objeto local `.sbss` de
  4 B en la direccion original 0x804FFD9C, 15 relocaciones con tipos y
  addends exactos y los 256 B completamente relocalizados iguales al ELF.
  Tres destinos vecinos fallan como controles negativos; no se normalizan
  sufijos a ciegas. Vease [r41-vm-alloc-audit.md](r41-vm-alloc-audit.md).
  Las otras 16 funciones nominalmente exactas pasan el auditor estandar.

Esta es una correccion de fidelidad demostrada, no una subida del contador.

## `vmbase::__VMBASESetupExceptionHandlers`: dos controles neutros

Tras leer r20/r26 y la veda r31 incorporada al fuente se probaron dos
combinaciones de acceso localizado, no otro barrido de declaraciones/pragmas:

1. Marcar volatile solo las dos lecturas reales de las instrucciones originales
   en 0x80000300/0x80000400.
2. Añadir a lo anterior accesos volatile localizados en los cuatro stores de
   instrucciones, sobre el baseline actual de cuatro pseudos `branch1..4`.

Ambas dejan exactamente **380 B / 99,1579 %** y las mismas doce diferencias
de registro. No cambian la cadena `instruction` de r5/r6 a r0. La familia no
transfiere el efecto observado en steering porque aqui no habia otra lectura
o escritura que ordenar entre esos accesos. No se conserva ningun cambio.

Fuente final identica byte por byte, incluidos sus finales MIXTOS: 650 CRLF
de 721 LF. JSON completo anterior/posterior identico: **53 simbolos, 30
funciones medidas, 29 exactas**. Auditor estandar sin fallos en las exactas.
No se toco ninguno de los cuerpos ASM que ya existian.

## `dvd_device::StartNonAlignedAyncRead`: dependencia de alineacion

Baseline **416/440 B, 93,59091 %**, seis copias `mr` ausentes. Se leyeron r20,
r21, r25, la veda r36e del fuente y `ai_dvd_round7_notes.md`; la familia de
pin de alineacion ya existia y se trato como calibracion, no como descubrimiento.

| forma medida | resultado |
|---|---:|
| `alignedStart` real ligado a r4 y `+r` inmediatamente antes del store | 444 B / 89,74545 % |
| cambiar esa dependencia a solo entrada `r` | 452 B / 75,1 % |

La primera reproduce las copias de entrada, pero añade un preservado y cambia
el coalescing de la suma/mascara y la base global. No reproduce los 440 B de
otra forma historica: el punto y la clase de dependencia importan. La segunda
rompe aun mas scheduling, CSE y marco. Ambas retiradas; no seguir con
permutaciones arbitrarias sobre estos resultados.

Se preserva `BytesRead +=` de la correccion semantica anterior. Fuente final
identica, incluido su formato MIXTO de 453 CRLF/454 LF. JSON completo
anterior/posterior identico: **64 simbolos, 18 funciones medidas, 17 exactas**.
Auditor estandar sin fallos en las exactas.

## `vm::__VMAllocVirtualToARAMLUT`: dos controles neutros

Store volatile localizado de `g_baseVMtoARAM`, solo y combinado con el
argumento nuevo de `OSSetArenaLo` materializado antes del store. Los dos dejan
el JSON completo identico: **164/168 B, 96,07143 %**, sigue faltando `mr r4,r3`.
La primera escritura del segundo ensayo uso una declaracion despues de una
sentencia que este MW rechaza; se movio la declaracion al bloque inicial C90
y se midio SOLO despues de compilar correctamente. El JSON del fallo no se
usa como evidencia de la variante.

Las dos variantes fueron retiradas antes de corregir la cadena independiente.
No hay cambio retenido en esta funcion ni en su hermana exacta.

## Artefactos

`scratchpad/codex_r41_{vmbase,dvd,vm}_{before,after,c1,c2}.json` y copias
iniciales de las fuentes. El snapshot `codex_r41_vm_after.json` certifica la
restauracion de los ensayos; **el resultado final con literal corregido es
`codex_r41_vm_literal_after.json`**, no confundirlos.

Auditores: `codex_r41_root_audit.py` y `codex_r41_vm_literal.py`.
