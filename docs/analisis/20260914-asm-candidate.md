# 2026-09-14: candidato ASM -> C (`csis` / `slinklist.h`)

## Alcance y control

- Base autoritaria comunicada: `5691c2d4`, ronda r68.
- Produccion, `configure.py`, grafo y objetos de `build/GOWE69` no se escribieron.
- Se leyeron enteros `r67-bibliotecas.md` y `r67b-libs.md`, y se contrastaron
  `r65-libs.md`, cierres r67/r67b, historia del header y DWARF original.
- Todos los ensayos se hicieron con copias privadas bajo
  `scratchpad/codex_20260914_asm_candidate` y el comando reproducible
  `build_shadow.ps1`.

La unidad de produccion sigue con 57/57 funciones de codigo exactas. Ese estado
depende del andamio ya existente en `System::Unsubscribe`; este trabajo no lo
cuenta como un cierre nuevo.

## C natural y causa restante

La forma coherente con el DWARF es:

```cpp
gSystems.Remove(&pSystemDesc->linkNode);
return RESULT_OK;
```

El DWARF de `System::Unsubscribe` sólo declara `pSystemDesc` e `i`; los
`ptempnode` pertenecen a los dos bloques inline de `CListDStack::Remove`. Por
tanto, las locales `node`/`next` del andamio no son fuente original.

La forma natural conserva tamaño 232 B y el resultado histórico 99,01724 %, pero
CSE vuelve a representar el enlace anterior desde el contenedor:

| punto | objetivo / produccion exacta | C natural |
|---|---|---|
| materializar `linkNode` | `addi r10,r3,0x20` (`39430020`) | `addi r8,r3,0x20` (`39030020`) |
| leer `pprev` | `lwz r9,4(r10)` (`812a0004`) | `lwz r9,0x24(r3)` (`81230024`) |

No es una diferencia de semantica de la lista: son dos direcciones equivalentes.
La diferencia que queda es la eleccion de base/lifetime hecha por el optimizador.

## Formas C medidas

Las siguientes variantes preservan layout, API y algoritmo, y no introducen
ASM, pines, cebadores ni datos:

| variante privada | resultado ejecutable de `Unsubscribe` |
|---|---|
| llamada natural con header actual | 232 B, SHA-256 `60dfef09939f68d230d8be8a4e0dcf6ba0c150915759ef22a65a7793fee351dc` |
| `pnext`/`pprev` declarados despues de los accessors | identico byte a byte |
| los cuatro accessors definidos inline fuera de clase | identico byte a byte |
| `Remove` definido inline fuera de clase, `phead` declarado al final y parametro `register` | identico byte a byte |

El cuerpo exacto de produccion tiene SHA-256
`5f2d9bc55612ab9e02ff65f4705b15bdcc14a020c317a41b0fc933149d40e706`.
`audit_shadow.py` extrae el simbolo STT_FUNC de cada ELF, comprueba los 232 B y
falla si cualquiera de las cuatro formas C deja de ser identica a la natural.

Reproduccion:

```text
powershell -File scratchpad/codex_20260914_asm_candidate/build_shadow.ps1 v_remove_outclass
python scratchpad/codex_20260914_asm_candidate/audit_shadow.py
```

Resultado final del gate: `PASS: all four pure-C structural forms are
code-identical; production exact remains distinct`.

## Veredicto acotado

No hay una sustitucion C demostrada para esta barrera en las formas medidas. La
declaracion de miembros, separar accessors/`Remove` y la sugerencia C
`register` no afectan el cuerpo: no son palancas para el CSE de ProDG 3.9.3.
Esto **no** demuestra que toda formulacion C sea imposible; sólo descarta estas
formas concretas y evita otro barrido de cosmetica del header compartido.

La siguiente prueba causal debe partir de evidencia nueva de la fuente original
de `slinklist.h` o del RTL que explique por que el parametro inline conserva base
propia. Sin esa evidencia, tocar un header consumido por decenas de unidades
tiene mas riesgo que valor. No se promovio ninguna variante y no fue necesario
recompilar vecinos: produccion quedo intacta.

## Seguimiento acotado: `madidct.cpp` / `IdctRow`

Se midio una sola variante privada, solicitada despues del diagnostico anterior:
retirar la local fantasma fijada a `r5` y sus dos `asm("")`, sin cambiar ninguna
operacion matematica. La evidencia historica r64 se conserva, pero su conclusion
se actualiza para indicar que el andamio se retiro el 14/09 por la nueva prioridad
de C real.

| funcion | antes | sin fantasma | exactas |
|---|---:|---:|---:|
| `IdctColumn` | 632 B / 50,449368 % | igual | no |
| `IdctRow` | 516 B / 44,36434 % | 496 B / 46,372093 % | no |
| `idctcompute` | 356 B / 100 % | igual | si |

El menor tamano de `IdctRow` **no es un avance de matching ni un cierre**. El
beneficio es retirar tres construcciones sin semantica de programa. El gate
privado demuestra:

- el conjunto de funciones exactas sigue siendo `{idctcompute}`;
- sólo cambia el stream de instrucciones de `IdctRow`;
- `IdctColumn` y `idctcompute` conservan su stream normalizado;
- `idctcompute` pasa contra el ELF original con 356 B y 63 reubicaciones;
- `.rodata`, `.data`, `.sdata`, `.sdata2`, `.bss` y `.sbss` son byte-identicas.

Artefactos:

```text
python scratchpad/codex_20260914_asm_candidate/madidct_remove_ghost/build_shadow.py
python scratchpad/codex_20260914_asm_candidate/madidct_remove_ghost/audit.py
```

`audit.py` usa la copia inmutable del objeto anterior (SHA-256
`8bd966fa401320fab3998c4bf57346427654028c71828c818243b68691743972`),
por lo que sigue siendo reproducible despues de la integracion de raiz.
