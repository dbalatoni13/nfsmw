# Ronda 38 — zEagl4Anim

Territorio: `src/Speed/Indep/Src/EAGL4Anim/**`. Se revisaron
`FnRawStateChan::EvalState` y `FnRunBlender::FindMatchTime`; no se tocaron
`Initialize` (622 variantes ya agotadas) ni `FnStatelessQ::EvalSQT` (su firma
original necesita una extension de argumento que no puede expresarse sin tocar
la cabecera compartida).

**Cerrado: nada.** Las dos vias quedaron restauradas byte por byte al estado de
entrada. La instantanea completa antes/despues de zEagl4Anim es identica: 318
funciones, 0 mejoras y 0 regresiones.

## Estado comprobado

| bytes objetivo | bytes fuente | porcentaje | funcion |
|---:|---:|---:|---|
| 2.352 | 2.356 | 98,8758 % | `DynamicLoader::Initialize` |
| 1.480 | 1.488 | 99,4595 % | `FnStatelessQ::EvalSQT` |
| 720 | 720 | 97,3611 % | `FnRunBlender::FindMatchTime` |
| 456 | 456 | 93,5088 % | `FnRawStateChan::EvalState` |

Verificacion final:

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEagl4Anim
  -> 1 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> 4 de 318 funciones distintas, 5.008 B
python scripts/pctsnap.py --umbral 0 --cmp before_pct.json after_pct.json
  -> EMPEORAN: ninguna; MEJORAN: 0
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim
  -> todas las funciones que objdiff da al 100 % pasan bytes, ramas,
     reubicaciones y literales
python scripts/lcfix.py --check
  -> todas las entradas @lc estan al dia
```

Ademas, los JSON completos de objdiff de entrada y salida son iguales, no solo
los porcentajes (`FULL_JSON_IDENTICAL True`, `PCT_IDENTICAL True`). Los
artefactos estan en `scratchpad/codex_r38_anim/`.

## `FnRawStateChan::EvalState`: la carga tardia no sale de la local C

El objetivo carga `mKeyIdx` una sola vez en la fila 19 y conserva la base de
`GetKeyData`; nosotros adelantamos la carga a la fila 7 y recalculamos dos bases
con `add` donde el objetivo usa `mr`. El DWARF solo declara `c` en r30, `i` en
r31, `k` en r4 y `dest`; la local artificial `keyIdx` sigue siendo necesaria
para evitar dos cargas.

Ensayos nuevos, todos revertidos:

| forma | resultado |
|---|---|
| asignar `keyIdx = mKeyIdx` dentro del argumento de `GetKeyData` | neutro, 93,5088 %, 456 B |
| pin DWARF `k asm("r4")` | neutro, 93,5088 %, 456 B |
| pin de `keyIdx` a r11 | 93,3333 %, 460 B; introduce copia |
| copia real mediante `asm("" : "=r" ... : "0" ...)` | 92,6316 %, 460 B |
| usar `k` para la primera clave y quitar `keyIdx`, forma fiel al inventario DWARF | reproduce el negativo conocido, 89,8684 %, 460 B |
| permutador guiado, 96 transformaciones simples | ninguna mejora; 7 no compilan |

La asignacion dentro del argumento era la unica forma nueva que ligaba la
lectura a la expresion sin cambiar semantica; GCC la vuelve al mismo RTL. El pin
de `k` tampoco altera codigo, por lo que el registro del DWARF no es una palanca
de reparto en esta version. La funcion sigue bloqueada en CSE/PRE, no en el
orden textual de las declaraciones.

## `FnRunBlender::FindMatchTime`: confirmacion causal del sesgo int->float

El objetivo carga el sesgo `0x4330000080000000` antes del primer `bso`; el
fuente reconstruido lo carga dentro del segundo termino del `&&`. Las restantes
diferencias son la cascada f12/f13 y r9/r10. `regmap.py` vuelve a confirmar el
mismo inventario y reparto de locales.

Ensayos nuevos, todos revertidos:

| forma | resultado |
|---|---|
| local `searchLength` + barrera `+f` sobre la local | 97,2111 %, 720 B; solo cambia reparto FPR |
| barrera de entrada sobre `input.mSearchLength` | 93,0222 %, 732 B; fuerza recargas |
| permutador guiado, 145 transformaciones simples | ninguna mejora; 2 no compilan |
| union C explicita para int->float + sesgo real vivo | 96,1944 %, 720 B; mueve el `lfd`, pero crea otro slot y desplaza la pila |
| sesgo real como entrada de asm, conversion implicita intacta | 93,1333 %, 736 B; GCC no comparte el literal generado y lo duplica |

La union demuestra que el diagnostico es correcto: el `lfd` se puede llevar al
bloque deseado, pero una local nombrada recibe otro slot de pila. La barrera de
entrada demuestra el segundo limite: GCC trata el sesgo que introduce su
expansion de int->float como distinto del literal C y emite dos cargas. Sin
importar el literal original de la TU, sin un cuerpo ASM o sin cambiar el
compilador, no aparece una palanca C limpia. Ninguna de esas tres salidas es
admisible en esta ronda.

## Veda resultante

- No repetir pines de `k`/`keyIdx`, asignacion dentro del argumento ni la forma
  `k = GetKeyData(mKeyIdx)` en `EvalState`.
- No repetir locales/barreras del sesgo ni una union de conversion separada en
  `FindMatchTime`.
- Una vuelta futura a `EvalState` necesita un cambio que altere la
  disponibilidad PRE sin agregar una local; una vuelta a `FindMatchTime`
  necesita compartir legitimamente el slot/constante que genera la conversion,
  no solo mover una sentencia.

