# Iter6 — `libc/libgcc2_4` (sólo sombras)

## Resultado

No hay una promoción retenible. La fuente de producción y su objeto quedaron
intactos. El experimento acota el bloqueo: no basta con nombrar los tres
`double` originales; hacerlo cambia el lowering y el planificador antes de que
las referencias puedan coincidir con el objeto extraído.

El control recompilado de `src/libc/libgcc2_4.c` da `__floatdisf` 176/176 y
44/44 instrucciones resueltas idénticas. Su único defecto para el enlace es la
`.rodata` privada de 24 bytes:

```
4330000080000000  40f0000000000000  4330000000000000
```

El objetivo no posee esa sección. Sus seis relocs de texto usan los objetos ya
existentes del pool automático:

```
006a HA lbl_8040FE28   006e HA lbl_8040FE30
0076 LO lbl_8040FE28   007e HA lbl_8040FE38
0086 LO lbl_8040FE30   0092 LO lbl_8040FE38
```

## Hipótesis medidas

### V1 — slot C compartido y tres objetos externos

`scratchpad/codex_20260908_iter6_libgcc/v1_union.c`

Reemplaza únicamente las dos conversiones entero→double por un `union` y usa
las identidades reales `lbl_8040FE28`, `lbl_8040FE30` y `lbl_8040FE38`.
Mantiene el algoritmo GCC 2.95, genera 176 bytes, seis relocs externos y cero
bytes de `.rodata`. Es negativa: 77,27273 % en la medida de instrucciones. La
primera diferencia causal es que la carga del sesgo firmado se iza antes del
`xoris`; después rotan el word temporal, el `0x4330` y los registros FP. Los
relocs quedan en 0x5a/0x62/0x6a/0x76/0x7e/0x82, no en sus offsets originales.

### V2 — slot `volatile`

`scratchpad/codex_20260908_iter6_libgcc/v2_volatile.c`

La volatilidad fuerza el orden real de los stores low/high y evita que el
compilador trate las dos representaciones como almacenamiento independiente.
También elimina por completo `.rodata`, pero queda en 172 bytes y 78,86364 %.
La razón de los cuatro bytes es verificable: el lowering nativo del control
emite un `mr r7,r8` muerto entre el primer store y las preparaciones del pool;
la reconstrucción C no lo emite. Las cargas externas vuelven a planificarse en
otro orden y sus relocs quedan en 0x62/0x6a/0x72/0x7a/0x8a/0x8e.

No se hizo una tercera variante. Copiar la cola localizada en inline assembler
podría imponer las instrucciones vivas, pero el cierre de 176 bytes exigiría
además leer `r8` sin una definición en la función para fabricar exactamente el
`mr r7,r8` muerto. Inicializar `r8` con un valor C real añade otra instrucción;
no inicializarlo sería el ghost/valor indefinido prohibido. Tampoco se usaron
ordinales `$LCn`, modificación de objeto, flags, versiones ni reabsorción de
splits.

## Gate reproducible

Ejecutar desde la raíz:

```
python scratchpad/codex_20260908_iter6_libgcc/audit_negative.py
```

El gate:

- fija SHA-256 de fuente y objeto de producción;
- resuelve relocs PowerPC HA/LO/REL14 contra las direcciones ELF originales;
- demuestra 44/44 words del control frente al objetivo;
- comprueba `.rodata` cero y seis referencias externas en ambas sombras;
- informa 27/44 words en V1 y 23/44 en V2 (comparación posicional estricta);
- compara el algoritmo GCC 2.95 con redondeo IEEE-754 entero→float exacto en
  252.269 casos deterministas, incluyendo límites de 24, 53 y 63 bits.

Salida final observada: `PASS negative gate`. Este PASS certifica el diagnóstico
y la preservación, no certifica ninguna sombra como candidata a promoción.

## Conclusión

Las dos formas C medidas en esta tanda quedan descartadas con una causa
concreta: la identidad externa de los tres operandos cambia el scheduling del
lowering, mientras que el control que preserva el esqueleto nativo conserva la
`.rodata` privada. Esto no demuestra que toda forma C posible sea inviable. La
siguiente hipótesis tendría que explicar cómo conservar ese lowering sin
copiar el artefacto muerto ni depender de ordinales de pool inestables; no se
identificó una tercera forma con esas propiedades en esta tanda.
