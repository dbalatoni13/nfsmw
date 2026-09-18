# Iter6 — revisión independiente de `eathread_thread_gc.cpp`

## Veredicto

La transformación está justificada semánticamente y a nivel de objeto. El
enlace privado v3 descrito abajo fue negativo, pero ha quedado superado por
`keep_link_proof_v2`: dos reglas `-keep` calificadas por objeto conservan la
entrada `.ctors` y los 84 bytes de código. La revisión independiente de ese
proof da **PASS** para integrar con el mapping de nombre documentado.

## Las tres definiciones omitidas

El inventario parsea los 619 objetos únicos del enlace. Para los nombres

- `AllocateThreadDynamicData__Q22EA6Threadv`;
- `_Q22EA6Thread.gThreadDynamicData`;
- `_Q22EA6Thread.gThreadDynamicDataAllocated`;

encuentra cero definiciones, cero UNDEF y cero relocs en el grafo efectivo. La
búsqueda por stems tampoco encuentra aliases alternativos fuera de la unidad.
El objeto extraído y el ELF original no contienen esos tres símbolos.

La fuente actual aporta una función vacía de 4 bytes y dos arrays BSS de
26.624 + 128 bytes que el objetivo no posee. Omitirlos no cambia ninguna
conducta observable del ejecutable reconstruido; conservar la declaración de
`AllocateThreadDynamicData` mantiene la interfaz para el código fuente sin
inventar una definición ausente del binario.

## El cuarto cambio: disparador del constructor

Tras las tres omisiones quedaban cuatro bytes de BSS: el objeto estático
sintético de un byte y padding. Cambiar ese objeto por un local automático en
el helper conserva el único efecto de su constructor: un bucle de 32 vueltas
sin cuerpo. `AtomicInt<int>::AtomicInt()` está vacío, el destructor implícito es
trivial y el objetivo no tiene `.dtors` ni accesos a almacenamiento.

La sombra resultante reproduce dentro del objeto:

- `.text` 84 bytes, helper 40 + wrapper 44;
- las dos funciones con instrucciones equivalentes;
- `.ctors` 4 bytes con un único `R_PPC_ADDR32` al wrapper;
- `.bss`, `.data` y `.rodata` de tamaño cero.

Si la entrada sobreviviera al strip, su posición también sería la correcta: la
unidad ocupa el slot 609 del grafo y precede al objeto automático de ctors del
slot 610.

## Obstáculo ELF confirmado

En v3 el wrapper se llama `GlobalThreadDynamicDataInit`. El ensamblador
canoniza el `.set` declarativo hacia ese símbolo; no queda en la tabla ELF un
alias local llamado
`_GLOBAL_.I.AllocateThreadDynamicData__Q22EA6Threadv`, y el reloc de `.ctors`
continúa apuntando a `GlobalThreadDynamicDataInit`.

Con `-strip-unused-data`, el resultado privado demuestra el fallo:

- control: `.text` 0x3a0d18, `.ctors` 0xe0, DOL 4.541.888 bytes;
- trial v3: `.text` 0x3a0cc4, `.ctors` 0xdc, DOL 4.541.824 bytes.

Desaparecen la entrada de cuatro bytes y los 84 bytes de código; el movimiento
posterior explica el delta DOL de 64 bytes tras alineaciones.

Nombrar directamente la función `static` con el asm-name original no resultó
viable: ProDG genera etiquetas DWARF derivadas de ese nombre y `ngcas` las
rechaza. La alternativa `.local/.type/.set/.size` tampoco es válida porque este
ensamblador no reconoce `.local`. Éste es el bloqueo histórico de v3, no el
resultado final de la prueba `keep_link_proof_v2`.

## Actualización: `keep_link_proof_v2`

La solución privada añade únicamente estas dos entradas al `keep.lst` de
control:

```text
eathread_thread.o:GlobalThreadDynamicDataInit
eathread_thread.o:gThreadDynamicDataConstructor
```

El gate independiente
`scratchpad/codex_20260908_iter6_linked_triage/review_eathread_keep2.py`
comprueba directamente:

- DOL de control, trial y original idénticos byte por byte, SHA-1
  `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 4.541.888 bytes;
- helper LOCAL de 40 B en `0x803A3058` y wrapper LOCAL de 44 B en
  `0x803A3080`, con los 84 B runtime idénticos al control;
- entrada `.ctors` en `0x803C8C1C` cuyo valor es exactamente `0x803A3080`;
- layout allocable del objeto limitado a `.text` 84 B + `.ctors` 4 B y cero
  BSS/data;
- 619 inputs únicos, un único reemplazo en el índice 608 y los otros 618 en la
  misma posición, además de los hashes capturados de los 619 objetos;
- delta de keep exacto y hashes de fuente/configuración/objetos de producción
  intactos.

Matiz ELF no bloqueante: helper y wrapper son LOCAL como en el objetivo. El
símbolo auxiliar que nombra los cuatro bytes de ctor es GLOBAL; el DOL no
contiene tabla de símbolos y su dirección/contenido runtime son exactos. En el
objeto raw sólo difiere la representación del `bdnz` local a `+0x20`: target
`0x42000000`, trial `0x42000020` con un `R_PPC_REL14` de sección, tipo 11 y
addend 32. El enlazador resuelve el trial a `0x42000000`; la palabra y los 84 B
completos coinciden con el ELF original.

## Gate final actualizado

- **PASS conceptual:** omisión de las tres definiciones, ausencia de usos,
  storage cero, equivalencia ctor/dtor y orden potencial de `.ctors`.
- **PASS de promoción privada:** mapping de nombre + dos reglas keep producen
  el DOL original con el wrapper LOCAL y la entrada ctor exacta.
- Producción, configuración, splits y objetos del grafo no fueron modificados
  durante esta revisión.
