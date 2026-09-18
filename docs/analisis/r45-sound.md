# R45 — `SetupNextLoad`: el pin r26 ya estaba medido

Fecha: 2026-09-08. Territorio: `zEAXSound`, sin tocar `zEAXSound2`.

## Resultado

No se integra ningún cambio: **+0 B / +0 funciones**. La función permanece en
**1008 B, 98,67063 %, 23 filas**. Las dos variantes nuevas se probaron sólo en
una copia privada de `EAXAemsManager.h`; ambas se descartaron y la copia quedó
restaurada byte a byte. `EAXAemsManager.h`, `EAXAemsManager.cpp` y el objeto real
de `zEAXSound` no se editaron ni recompilaron.

La premisa de `r36f-snd.md` §4.6 estaba desactualizada: recomendaba pinchar el
contador a r26, pero ese ensayo ya figura completo en `r36c-snd.md` §4.

| variante | B | similitud | filas | veredicto |
|---|---:|---:|---:|---|
| base real/sombra | 1008 | 98,67063 % | 23 | referencia |
| histórica B1: `deleteCount asm("r26")` | 1004 | 98,11508 % | 26 | ya medida, no repetida |
| V1: sucesor real `deleteCount+1` a r28 | 1004 | 98,07539 % | 25 | peor, descartada |
| V2: V1 + barrera vacía `+r` | 1008 | 99,34524 % | 28 | fuzzy engañoso, descartada |

V2 sube el porcentaje porque deja instrucciones del mismo opcode con registros
distintos, pero reduce las filas exactas de 230 a 225. No es avance estructural
y no se conserva.

## Evidencia del inline y del DWARF

`SetupNextLoad` sólo contiene trece líneas propias; las 1008 B expanden
`SndAssetQueue::DeleteRefToAsset(Attrib::StringKey)`. El line map original ubica:

- `li r26,0` en `EAXAemsManager.h`, inicialización de `deleteCount`;
- `addi r28,r26,1` y `slwi r27,r26,2` en el primer borrado;
- `mr r26,r28` al confirmar el incremento;
- `subi r7,r26,1`, el índice de la segunda pasada;
- `mr. r26,r7` al terminar esa pasada.

La sobrecarga `StringKey` sólo tiene un llamante, en `SetupNextLoad`; las otras
tres llamadas del árbol usan la sobrecarga `SndBase *`. El header entra en ocho
ficheros de `zEAXSound`, por lo que el gate correcto es la SourceList completa.

No existe un cuerpo DWARF procesable de la función: hay un error de subprograma
en su rango. El line map sí está presente y basta para atribuir el contador y
los inlines; no se inventaron locales a partir de un cuerpo inexistente.

## Por qué falla el pin directo

La base ya contiene la topología que exige el objetivo:

```text
objetivo: addi r28,r26,1 ... mr r26,r28
fuente:   addi r29,r31,1 ... mr r31,r29
```

El pin histórico del contador no sólo elige r26. GCC coalesce el valor nuevo con
el anterior, emite `addi r26,r26,1` y elimina el `mr`: pierde 4 B y se aleja. Esa
es la regla documentada en `brief-r36c.md`; por eso repetir el pin propuesto por
r36f no podía cerrar la función.

V1 intentó evitar la repetición nombrando el **valor real** sucesor:

```cpp
register int nextDeleteCount asm("r28") = deleteCount + 1;
remove(currequst);
SfxToDel[deleteCount] = currequst.pThis;
deleteCount = nextDeleteCount;
```

Sin barrera, el asignador vuelve a coalescer ambos valores: `deleteCount` acaba
en r28, salen `addi r28,r28,1` y `mr. r28,r28`, y desaparecen los dos movimientos
del objetivo. V2 añade una barrera vacía sobre el valor inicializado y sí separa
los pseudos: recupera `addi r28,r31,1` y `mr r31,r28`. Sin embargo, no altera el
orden de los demás allocnos: `this` sigue en r27, el contador en r31 y el índice
en r29/r7. Además cambia siete registros de cargas/stores del primer borrado. El
salto de cross-jumping de la instrucción 195 tampoco cambia.

La causa queda así acotada: **el registro del sucesor puede forzarse, pero no
arrastra la rotación global ni el cross-jump; forzar el contador destruye la
separación de pseudos que el objetivo necesita**.

## Gate completo

La sombra base reproduce los **773 símbolos funcionales mapeados**: 771 exactos
y sólo `SetupNextLoad` y `cStichWrapper::Play` pendientes, iguales al objeto
real. V1 y V2 mantienen las mismas 771 exactas y sólo cambian `SetupNextLoad`;
ninguna sección runtime no-texto, literal o dato cambia.

En la base, objetivo y fuente tienen 252 instrucciones reales, 38 ramas y 12
reubicaciones externas cada uno. No hay referencia a pool literal en la función.
El diff consta de 21 `ARG_MISMATCH` y una pareja insert/delete, que es la posición
de `lis gFastMem@ha`; además permanece el destino diferente del `beq` de la fila
195. `lcfix.py --check` pasa limpio.

Hashes SHA-256 finales relevantes:

```text
EAXAemsManager.h    68aac05382cf1b50d468e1d7a663bc5c4d5255ca7c310ea017698e6fa0f57aac
EAXAemsManager.cpp  9571df0ec742f3d6151964436f26efe45f7037feefaed41464ab2f3987119080
zEAXSound.o real    6617eb9c3ee64cbdad3ee829145f4a9f28fc2a7d357c80d01e821ad19998d5d7
```

## Siguiente hipótesis legítima

No repetir pines de `deleteCount`, `idx` ni del sucesor. La siguiente palanca
tendría que cambiar **el CFG de la segunda pasada** de forma semánticamente
equivalente para que el camino que borra una entrada no mantenga `idx` vivo a
través de `Free`, y a la vez reproducir el cross-jump al encabezado
(`beq` objetivo a +0x240 frente a +0x3c0). Sólo después tendría sentido volver a
medir el orden global de `this`/contador/índice. Una simple declaración o barrera
local no alcanza ambas causas.

## Artefactos

En `scratchpad/codex_r45_sound/`:

- `d_zEAXSound.json`: baseline real contra el objetivo;
- `baseline_shadow.json` y `restored_shadow.json`: control de la sombra;
- `v1_nextcount_r28.json`, `v2_nextcount_r28_keepalive.json` y el objeto V2;
- `build_shadow.py`, wrapper y copia privada restaurada;
- `audit.py`: reproduce todos los gates anteriores sin compilar producción.
