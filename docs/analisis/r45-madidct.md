# R45 — fronteras de retorno de MULT y vida del producto de 64 bits

Resultado: tres sombras negativas; **0 B / 0 funciones**, sin cambios en la
fuente ni el objeto de producción. Se partió del comentario corregido de r44,
no de la premisa falsa de que faltaban cinco copias high-word.

## Hipótesis y pruebas

El original no enumera inlines MULT en su DWARF y nuestro objeto sí. Esto no
demuestra un macro original, pero permite contrastar qué ocurre al sacar la
conversión final a int del helper, manteniendo la expresión matemática.
Los barridos r19 conservaban un retorno int o sustituían el helper completo por
macro; estas dos fronteras intermedias no estaban en aquellos scripts.

| forma | Column, B / % | Row, B / % |
|---|---:|---:|
| base: helper devuelve int tras redondeo y shift | 632 / 50,449368 | 496 / 46,372093 |
| helper devuelve producto long long; redondeo y shift en caller | 612 / 49,208862 | 472 / 27,232557 |
| helper devuelve producto redondeado long long; shift en caller | 612 / 49,208862 | 472 / 27,232557 |
| sólo MULT(t2,35468) de Row: producto real con barrera +r | 632 / 50,449368 | 472 / 32,64341 |

Las dos primeras sombras tienen idénticas secciones de código/datos y
relocaciones entre sí; coinciden en tamaño y porcentaje con la forma macro
medida en r19. Ninguna conserva la frontera de vida de la base.

La tercera usa un helper int privado y un valor inicializado, no una guarda:

```cpp
long long product = (long long)a * b;
asm("" : "+r"(product));
return (int)((product + 32768) >> 16);
```

Sólo se aplica al producto t2*35468 de Row. El objetivo copia ese resultado
como par antes del redondeo; el ensayo comprobaba si una dependencia sobre el
producto real conservaba esa identidad. No lo hace: elimina el mismo derrame
de LR que las otras dos formas y empeora el orden/reparto. No se retuvo el asm.

## Qué instrucciones desaparecen realmente

En los tres ensayos, el delta de mnemónicos de Row frente a la base es:

```text
lwz -1, stw -1, mflr -2, mtlr -2 = -6 instrucciones = -24 B
```

No desaparecen las cinco multiplicaciones ni sus ocho copias mr. Desaparece
el uso interno de LR para guardar un valor y, al no haber llamadas ni otro uso
de LR, también su guardado/restauración de prólogo/epílogo. El resultado de
472 B es contrario al objetivo de 516 B, que necesita CTR y LR simultáneos.
No se probaron nuevos pines de CTR ni cantidades indefinidas.

## Gates

`scratchpad/codex_r45_madidct/gate.py` comprueba:

- fuente byte-idéntica a baseline.cpp y objeto a production_before.o;
- tres funciones emitidas en todos los casos, sin helpers fuera de línea;
- datos de ejecución idénticos a producción;
- Column idéntica al byte en la prueba exclusiva de Row;
- idctcompute de 356 B pasa el auditor contra ELF original: 63 relocaciones;
- tamaños, porcentajes y conteos de instrucciones de las tres sombras.

El primer gate detectó una diferencia cruda en idctcompute al acortar Column:
se mueve el destino local de las llamadas a Row dentro de .text. Se verificó
con el auditor de destinos/relocaciones contra el ELF original; no se confundió
un desplazamiento de objeto con una regresión ni se aceptó sólo por porcentaje.

Sin cambios de fuentes, configuración, cabeceras, splits, flags, staging o
commits. Artefactos en `scratchpad/codex_r45_madidct/`; receta build_shadow.py
usa exactamente los flags actuales de la unidad y sólo genera objetos sombra.
