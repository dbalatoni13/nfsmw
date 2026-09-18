# Continuacion 2026-09-08: acarreo real de IdctRow

Base: HEAD `8853122e` y cambios locales R47/R48 preservados.
Ensayos exclusivamente en `scratchpad/codex_20260908_resume_madidct/`.
No se cambia la fuente, el objeto de produccion, flags, cabeceras ni splits.

## Hipotesis nueva

El LR del objetivo conserva el acarreo normalizado de la multiplicacion
`(t4-t5)*46341`, no el `t9` usado en la receta de R46. Se reconstruye ese
acarreo como `rounded < low`, a partir del producto de 64 bits inicializado.
Un helper inline exclusivo de esa multiplicacion permite nombrarlo.
No hay cantidades fantasma ni instrucciones ASM emitidas.

| sombra | Row bytes | Row fuzzy |
|---|---:|---:|
| control con fuente actual | 496 | 46,372093 % |
| acarreo explicito, sin barrera | 496 | 44,79845 % |
| acarreo con `+l` | 508 | 44,945736 % |
| acarreo `+l` y src6 `+c` | 508 | 40,0 % |

La ultima conserva simultaneamente CTR/LR y elimina la vuelta adicional por
LR de la receta R46, pero sigue sin reconstruir las vidas del objetivo.
Contando instrucciones reales, no huecos de alineacion:

| | objetivo | ultima sombra |
|---|---:|---:|
| instrucciones | 129 | 127 |
| lwz | 9 | 9 |
| mr | 10 | 8 |
| mtctr / mfctr | 1 / 2 | 1 / 1 |
| mtlr / mflr | 2 / 2 | 2 / 2 |

El objetivo recupera CTR en los indices 17 y 90; la sombra lo recupera una
sola vez en 12 y conserva el valor en un GPR. El acarreo entra/sale de LR en
84/89 en vez de 65/80. No basta con elegir el valor correcto: falta reproducir
la duracion de los rangos. Los otros mnemonicos tambien explican el neto -2;
la tabla no pretende ser el multiconjunto completo.

`IdctColumn` permanece en 632 B / 50,449368 % e `idctcompute` en 356 B / 100 %
en los cuatro objetos. Los ensayos no se integran: **cero bytes exactos nuevos**.
JSON completos y recuento reproducible en `summarize.py`; los flags se leen de
la regla real con `build_shadow.py`. El original de produccion se conserva
como `production_before.o` y la fuente como `baseline.cpp`.
