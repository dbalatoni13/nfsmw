# Ronda 29 — ventana

## `sfir::calcFIRCoeffs` (936 B, unidad entera): la barrera selectiva NO sirve aquí

Es la primera aplicación de la herramienta nueva fuera de donde nació, y sale
negativa. Dos ensayos, los dos delante del `switch`:

| ensayo | cifra |
|---|---|
| `__asm__("" : "+r"(halfLen));` | **68,46581 %** (base 89,25214) |
| `__asm__("" : "+r"(pfir));` | **89,25214 %** — sin efecto, objeto idéntico |

**Y la lectura es que el diagnóstico heredado apuntaba al síntoma.** Lo que la
r25 llamó «PRE nos iza `slwi`+`addi` al camino por defecto del `switch`» son 4 B
de un problema mayor: **toda la función tiene la cascada de registros corrida**
—`filtType` r28→r27, `pfir` r29→r30, **`halfLen` r26→r11**— y el último es el
que manda:

> **El objetivo guarda `halfLen` en r26, un registro SALVADO. Nosotros lo
> rematerializamos en r11, volátil, porque GCC sabe que vale 4.**

`li r26, 0x4` contra `li r11, 0x4`: el objetivo **no rematerializa** una
constante que cruza dos llamadas (`SNDI_sin`/`SNDI_cos`); nosotros sí. La
barrera sobre `halfLen` mata la constante pero **rompe otras cosas** (−20,8 pp),
así que el eje es correcto y la herramienta no.

**Lo que queda por probar, y no es de ventana:** una forma de fuente que dé a
`halfLen` **más de un `set`** —lo que en la r28 se demostró para los `const
float`: un inicializador constante **se propaga y nace en su uso**— sin
inventar. Y comprobar en el volcado si el `halfLen` del original tiene ahí un
valor calculado en vez del literal 4.

## Aviso de método que sale de aquí

**Una herramienta nueva no convierte un diagnóstico viejo en correcto.** El de
`sfir` llevaba desde la r25 apuntando a los 4 B de PRE, que son el síntoma más
visible pero no el eje. Antes de gastar ensayos con una palanca nueva, conviene
volver a mirar el diff entero: los 75 de 235 filas de esta función no salían en
ningún informe.
