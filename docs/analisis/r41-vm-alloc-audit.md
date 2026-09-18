# R41: auditoría estricta del alias local de `VMAlloc`

Fecha: 2026-09-08

## Resultado

**PASS.** La referencia `nextARAMPageToCheck$155` producida por la fuente es
el mismo objeto local que `g_nextARAMPageToCheck$233` en el ELF original. La
certificación no elimina sufijos `$NNN` ni acepta nombres por parecido: usa una
correspondencia explícita y comprueba el destino enlazado de cada relocation.

`VMAlloc` coincide en 256/256 bytes y 64/64 instrucciones. Después de aplicar
las relocations de ambos objetos con las direcciones reales, los dos cuerpos de
256 bytes son idénticos entre sí y a `NFSMWRELEASE.ELF` en `0x8039B84C`.

## Evidencia ELF

- El objeto recompilado define `nextARAMPageToCheck$155` como `STB_LOCAL`, de
  4 bytes y en `.sbss`.
- El ELF original define `g_nextARAMPageToCheck$233` como `STB_LOCAL`, de
  4 bytes y en `.sbss`, exactamente en `0x804FFD9C`.
- El objeto target extraído referencia
  `g_nextARAMPageToCheck$233_804FFD9C` como símbolo sin definir. Esto es lo
  esperado en un objeto extraído de un ejecutable enlazado; su tipo real se
  valida contra la tabla de símbolos del ELF, no contra ese placeholder.
- `config/GOWE69/symbols.txt` sitúa explícitamente
  `g_nextARAMPageToCheck$233` en `.sbss:0x804FFD9C`, coincidiendo con el símbolo
  independiente del ELF.

El stream completo de `VMAlloc` contiene 15 relocations: tres `R_PPC_REL24` y
doce `R_PPC_EMB_SDA21`. Sus offsets, tipos y addends coinciden en ambos objetos:

| Offset | Tipo | Destino de fuente |
| ---: | --- | --- |
| `0x24` | `R_PPC_REL24` | `VMGetARAMBase` |
| `0x2C` | `R_PPC_REL24` | `VMGetARAMSize` |
| `0x30` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0x44` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0x48` | `R_PPC_REL24` | `VMGetARAMSize` |
| `0x4C` | `R_PPC_EMB_SDA21` | `g_totalAllocatedVM` |
| `0x80` | `R_PPC_EMB_SDA21` | `g_baseARAMtoVM` |
| `0x84` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0x90` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0x98` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0x9C` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0xBC` | `R_PPC_EMB_SDA21` | `nextARAMPageToCheck$155` |
| `0xC0` | `R_PPC_EMB_SDA21` | `g_baseVMtoARAM` |
| `0xCC` | `R_PPC_EMB_SDA21` | `g_totalAllocatedVM` |
| `0xD4` | `R_PPC_EMB_SDA21` | `g_totalAllocatedVM` |

Los addends explícitos son cero. El auditor comprueba además que los campos
embebidos de las instrucciones tienen addend implícito cero antes de aplicar
`R_PPC_REL24` o `R_PPC_EMB_SDA21`.

Las siete referencias al estático se resuelven a `0x804FFD9C`. Como controles
negativos se repite el enlace sustituyendo sólo ese destino por los tres
objetos `.sbss` adyacentes, `0x804FFD90`, `0x804FFD94` y `0x804FFD98`. Los tres
casos dejan exactamente siete bytes distintos frente al ELF original, en los
campos inmediatos de esas siete instrucciones. Por tanto, una normalización
ciega de `$NNN` no puede ocultar una referencia a otro objeto.

## Reproducción

```powershell
python scratchpad/codex_r41_vm_alloc_audit.py
```

Artefactos:

- `scratchpad/codex_r41_vm_alloc_audit.py`: parser ELF32 big-endian, gate de
  símbolos, relocations, bytes enlazados y controles negativos.
- `scratchpad/codex_r41_vm_alloc.json`: snapshot objdiff leído por el gate.

Hashes SHA-256 de los artefactos binarios auditados:

- target `build/GOWE69/obj/LibSN/vm.o`:
  `14F7604345DBF17002A96C82F22067D1F57C5552C9B9329D3D2868D3F381B6D2`
- source `build/GOWE69/src/LibSN/vm.o`:
  `C71D5BABBD279676382A5EC0B813B374101B6D48FEDC810FE37D42850D41529C`
- original `orig/GOWE69/NFSMWRELEASE.ELF`:
  `6EE16D55D92AD1B337A9B655989C32B2976F95E7C64E148CF83F482C613F79F7`

## Alcance y limitación

Esta subtarea fue sólo lectura respecto a código, objetos y configuración: no
se editó `src/LibSN/vm.c`, no se compiló y no se ejecutó un build global. El
gate certifica la referencia estática y el cuerpo enlazado completo de
`VMAlloc`; no promueve por sí solo toda la unidad `vm.c` ni reaudita de forma
independiente la corrección del literal de `__VMMappingErrorAlert`.
