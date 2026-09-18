# r43 - sfir: dos extracciones inline medidas y retiradas

2026-09-08. Territorio exclusivo de coordinacion:
`Speed/Indep/Libs/snd/9/source/library/mix/sfir.c`.
Se leyeron el fuente con su veda r36e y los informes r19, r20, r25, r26 y r29.
La extraccion de un case a helper inline seguia propuesta sin medida en r19/r20.

## Baseline y alcance

`calcFIRCoeffs`: 936 B objetivo, 940 B fuente, 89,25214 %; unica funcion emitida
de esta unidad. El parentesco de nombres no autoriza compilar sfir8: se uso
exclusivamente la ruta exacta del objeto sfir.o con build_direct.py.

No se cambiaron firmas externas, headers compartidos, flags, configuracion,
splits ni ASM. Se preservo el calculo completo, incluido el orden de las
lecturas de cutLow/cutHigh dentro de cada iteracion y la normalizacion final.
En particular no se cacheo un cutoff a traves de SNDI_sin/cos ni se cambio
la rama de valor absoluto por una formula semanticamente diferente.

## Ensayos acotados

| ensayo | fuente | similitud | resultado |
|---|---:|---:|---|
| base | 940 B | 89,25214 % | referencia |
| c1: extraer solo case 2 | 940 B | 88,11966 % | peor |
| c2: extraer solo case 4, desde base | 940 B | 87,73505 % | peor |

Cada helper fue `static inline`, recibio el puntero real, halfLen y
`angleStep = 3.1415927f`; el contador y temporales FP pertenecian al helper.
Se mantuvo el resto de la funcion original sin cambios.

c1 uso el siguiente cuerpo (el nombre experimental calcHighPassCoeffs no
pretende reclasificar el filtro del case 2):

```cpp
static inline void calcHighPassCoeffs(SNDFIRSTATE *pfir, int halfLen, float angleStep) {
    pfir->coef[halfLen] = pfir->cutHigh;
    for (int cnt = 1; cnt <= halfLen; cnt++) {
        float tmpFloat = cnt * angleStep;
        pfir->coef[halfLen - cnt] = SNDI_sin(pfir->cutHigh * tmpFloat) / tmpFloat;
    }
}
```

c2 traslado el cuerpo del case 4 con sus dos llamadas, SNDI_sin y SNDI_cos,
a calcBandPassCoeffs; la copia exacta esta guardada en `c2.c`.

En los dos casos se confirma UNA sola funcion emitida: los helpers realmente
se expanden inline y no dejan funciones auxiliares en el objeto. Ninguno
elimina los 4 B excedentes ni mejora la candidata. El descriptor completo de
.rodata permanece igual en c2; cambia en c1, otro motivo para no retenerlo.
No se hizo un barrido de nombres, parametros o declaraciones alrededor de
estos negativos ni se probaron otra vez las barreras/pragmas ya vedadas.

## Restauracion demostrada

El parche inverso restauro el contenido; despues se restauraron mecanicamente
los CRLF de las lineas editadas, comprobando antes que solo diferian finales
de linea. El archivo recupera exactamente sus 4.598 B y sus 109 CRLF.
Se recompilo sfir.o y se genero un nuevo JSON completo.

`scratchpad/codex_r43_sfir/audit.py` exige y confirma:

- fuente final identica byte a byte a before.c, no solo diff textual vacio;
- objeto recompilado completo identico a before.o;
- ambos lados del JSON final identicos al JSON inicial;
- una unica funcion de 940 B / 89,25214 % como antes;
- cifras de ambas variantes conservadas sin confundirlas con el resultado final.

Resultado retenido: **0 B / 0 funciones, ninguna regresion y ninguna fuente
modificada**. La opcion de extraer individualmente los cases 2 o 4 mediante
estos parametros ya queda medida, no debe volver a presentarse como pendiente.

Artefactos en `scratchpad/codex_r43_sfir/`: before.c, before.o, before.json,
c1.json, c2.c, c2.json, after.json y audit.py.
