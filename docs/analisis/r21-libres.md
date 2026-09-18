# Las unidades sin agente — lo medido en la ventana de la ronda 21

`zMain`, `zGameplay` y `zBWare` no tuvieron agente. Entre las tres pierden
**13.760 B en seis funciones**, y `triage.py` las da todas como MURO. Esto es lo
que he medido de cada una, para que la ronda 22 no empiece de cero.

## `_._14ESpawnFragment` — zMain, 2.164 B al 98,869 %, **23 filas**

La más gorda y **nadie la había diffeado**. No es sólo permutación: hay **dos
cosas superpuestas**.

1. **Rotación de tres FPRs.** El objetivo carga `lbl_803F0B48`→f2,
   `lbl_803F0B4C`→f1, `lbl_803F0B50`→f3; nosotros →f1, →f3, →f2. De ahí salen
   nueve filas (`fmuls f11,f11,f2` contra `f1`, y las `fsubs f10/f9, f1` contra
   `f3`).
2. **El reparto de ranuras de pila NO coincide**, y eso es estructural:

   ```
   objetivo:  stfs f3, 0x90(r1)   ...   stfs f3, 0x94(r1)
   nuestro:   stfs f3, 0x9c(r1)   ...   stfs f2, 0x90(r1)
   ```

   Un desplazamiento distinto quiere decir **un temporal de más o de menos**, o
   uno de otro tamaño. Es lo que hay que atacar primero: mientras las ranuras no
   casen, la rotación de FPRs no se va a mover.

Herramienta indicada: `dwbody.py` filtrado por `low_pc` (locales del original con
su registro y el árbol de inlines). **No probado**: ningún ensayo de fuente.

## `__12EPlayRaceNIS…` — zMain, 2.176 B al 99,485 %, **31 filas**

Permutación consistente de registros, corrida en uno (r25↔r26, r28↔r30,
r29↔r30), más un intercambio de dos instrucciones independientes
(`addi r24,r1,0x18` contra `li r28,0x0`).

**Racimo de FPRs, con los valores identificados** en
`(dot * 0.75f + minTresh) * distTresh`:

| constante | objetivo | nuestro |
|---|---|---|
| `1.0f` | f27 | f26 |
| `0.5f` | f29 | f27 |
| `0.75f` | f28 | f28 |
| `0.25f` | f26 | f29 |

**Seis formas barridas, TODAS idénticas o peores**: `0.25f` en línea (idéntica),
`minTresh` antes de `dot` (idéntica), `minTresh` no const (99,4761 %),
`minTresh` primero en la suma (idéntica), `dot` partido en dos (idéntica), y sin
el temporal `dot` (98,4577 %). **La forma de esa expresión no decide el
reparto**: viene de dónde se izan las invariantes del bucle.

## `GenerateIndex__15GRaceParameters` — zGameplay, 1.680 B al 99,036 %, 5 filas

Cerrada como muro medido, con las cuatro alternativas anotadas en el fuente
(`GRaceDatabase.cpp`). La forma actual, con el `pflags` deliberado, es la mejor
de las cinco.

## `__8GTriggerRCUi` — zGameplay, 2.544 B al 99,583 %, 4 filas

**Una sola instrucción**: el objetivo pone `li r22,1` DESPUÉS del
`bl MATRIX4_multyrot` y nosotros antes. Nueve formas barridas (anotadas en
`GTrigger.cpp`). Y de paso se retiró de ahí **un pin de registro que no hacía
nada** — ver `pines.py`.

## `_bOutput` — zBWare, 5.180 B al 99,643 %

Es la única función que le falta a zBWare (238 de 239). La ronda 20 cerró un
racimo de cinco con `lreg.py`; el resto está en `r20-muros.md`.

## `Compare__…IconSort` — zGameplay, 16 B: **NO es trabajo**

Sale al 0 % y `censo.py` la daba como «sin escribir». **Está escrita y es byte a
byte idéntica** (`80030004 80640004 7c630050 4e800020`): lo único que difiere es
el contador que GCC pone al final del nombre — `.25589` el nuestro,
`.35326` el objetivo. Se arregla con una entrada en `OBJDIFF_SYMBOL_MAPPINGS` de
`configure.py`, como la que ya existe para `gc_interface`.

**Barrido el árbol entero buscando más casos así: es el único.**
