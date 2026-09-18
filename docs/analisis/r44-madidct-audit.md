# r44 — auditoría histórica de `madidct`

Fecha: 2026-09-08. Auditoría sólo lectura de
`egami/rcmp/dev/source/decoder/cmn/madidct` por el agente revisor. Sus conteos
se realizaron sin editar ni compilar la unidad. Después, coordinación corrigió
sólo el comentario UNSOLVED del cpp: mismas líneas y CRLF, objeto reconstruido
byte-idéntico (SHA-256 `40c640b0d720575017ef168acee2edea77ee9d02e7c7aa1bb51b5ab526368608`).
Fuente de conteo: JSON completo
`scratchpad/codex_r44_madidct/d_egami_rcmp_dev_source_decoder_cmn_madidct.json`,
contrastado directamente con `objdump` y la tabla de símbolos del objeto
original.

## Veredicto de la contradicción

Las cifras de r19/r20 son correctas. Para `IdctRow` hay que ignorar las entradas
vacías que objdiff inserta para alinear las dos secuencias:

| | objetivo | fuente | delta objetivo−fuente |
|---|---:|---:|---:|
| bytes / instrucciones reales | 516 / 129 | 496 / 124 | +20 B / +5 |
| `mtctr` | 1 | 0 | +1 |
| `mfctr` | 2 | 0 | +2 |
| `mr` | 10 | 8 | +2 |

No faltan cinco `mr`. La afirmación r36f confundió dos hechos diferentes: el
objetivo sí contiene cinco copias de palabra alta asociadas a los cinco
`mulhw`, pero la fuente contiene también cinco. Son, respectivamente:

```text
objetivo: mr r27,r8; mr r25,r5; mr r9,r7; mr r23,r19; mr r21,r17
fuente:   mr r9,r6;  mr r29,r21; mr r23,r19; mr r25,r7; mr r27,r17
```

Por tanto el delta completo que explica las cinco instrucciones adicionales es
exactamente `mtctr +1`, `mfctr +2`, `mr +2`. El comentario UNSOLVED r36f del
`.cpp` queda supersedido por esta auditoría.

## Derrames CTR/LR y primer hecho causal

`IdctRow` objetivo empieza en `0x8034BD3C`. El valor `src[6]` se carga y cruza
casi toda la transformación mediante CTR:

```text
8034BD4C  lwz   r0,0x18(r3)
8034BD5C  mtctr r0
8034BD80  mfctr r5       ; src[2] - src[6]
8034BEA4  mfctr r0       ; src[2] + src[6]
```

Además, el objetivo usa LR para otra cantidad: el acarreo normalizado por
`subfc/subfe/neg` que termina en `r0`, guardado con `mtlr` en `0x8034BE40` y
recuperado con `mflr` en `0x8034BE7C`. La fuente no tiene CTR en `IdctRow`:
guarda `src[6]` en LR al principio y lo recupera una sola vez; mantiene el
acarreo en un GPR.

Éste es el primer hecho causal verificable y es más preciso que «puro reparto
de registros»: el objetivo conserva simultáneamente dos vidas que acaban en
registros especiales distintos (`src[6]` en CTR y un acarreo en LR), mientras
la fuente sólo externaliza `src[6]` a LR. Esa presión y esos rangos explican el
par adicional de copias `mr`; no hay una copia de 64 bits ausente por cada
`mulhw`.

Los controles sombra de root refuerzan el límite sin cerrarlo: una entrada
`"c"(src6)` produjo 504 B y una vida `+c` produjo 512 B. La segunda ya emite
`mtctr`, pero sólo un `mfctr` y añade un par LR distinto; no reproduce las dos
recuperaciones ni el acarreo LR del objetivo. No se hicieron más variantes en
esta auditoría.

## `IdctColumn`

Ambos lados tienen 632 B y 158 instrucciones reales. El único delta de
multiconjunto es:

```text
objetivo−fuente: mr +2, lwz -2
```

El objetivo almacena `src[4]` en `r1+8` y conserva el valor vivo en `r5` para el
OR inmediato; la fuente reutiliza el registro y recarga la ranura. Más tarde la
fuente totaliza dos `lwz` adicionales donde el objetivo resuelve las vidas con
dos copias. Es un problema de reload/lifetime de tamaño neutro, separado del
derrame CTR adicional de `IdctRow`.

## Qué demuestra —y qué no— el DWARF

El DWARF original confirma exactamente los parámetros y las nueve locales
`t1..t9` de ambas funciones; `regmap` muestra el reparto distinto. Esto valida
la lista de variables de r19, pero no demuestra que la fuente sea textual o
estructuralmente completa.

Hay una diferencia que las notas antiguas omitieron: el cuerpo DWARF original
no enumera expansiones inline de `MULT`, mientras nuestro objeto enumera cinco
en cada función. No prueba por sí solo que el original usara un macro —la forma
macro ya medida genera otro tamaño—, pero sí impide concluir únicamente a
partir de las locales que «sólo queda reparto». La forma/rango del cálculo de
64 bits y, sobre todo, la vida de `src[6]` siguen siendo la causa estructural a
reproducir.

Evidencia reproducible:
`scratchpad/codex_r44_madidct/audit_counts.py`.
