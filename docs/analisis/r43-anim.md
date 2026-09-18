# R43 — `zEagl4Anim`

Territorio: `RawStateChan.cpp` y `FnRunBlender.cpp`, limitado a
`FnRawStateChan::EvalState` y `FnRunBlender::FindMatchTime`.

## Resultado

No se cerró ninguna función. Las cuatro hipótesis causales se restauraron: los
dos fuentes, el objeto recompilado, el inventario `fncmp` y el JSON completo de
objdiff quedaron idénticos a la entrada. No se conservaron mejoras parciales.

Estado final:

| objetivo | fuente | porcentaje | diferencia actual | función |
| ---: | ---: | ---: | ---: | --- |
| 456 B | 456 B | 93,50877 % | 52 instrucciones | `FnRawStateChan::EvalState` |
| 720 B | 720 B | 97,361115 % | 11 instrucciones, 14 relocs nominales | `FnRunBlender::FindMatchTime` |

La unidad conserva cuatro funciones con código distinto, 5.008 B en total, y
314 funciones exactas pasan la auditoría de bytes, ramas, relocations y
literales.

## Hipótesis 1: el desplazamiento pertenece al caller

El historial contiene el hallazgo `ac3efb4d`: los rangos DWARF de
`RawStateChan::GetKeyData` sólo cubren el cálculo de la base, mientras que
`keyIdx * GetKeySize()` pertenece a cada caller. R38 no había incorporado esta
evidencia.

Se reprodujo sin tocar `RawStateChan.h`: cada llamada usó
`GetKeyData(0) + index * GetKeySize()`. El resultado fue una mejora estructural
real:

```text
93,50877 % / 456 B / 52 instrucciones distintas
        ->
97,76316 % / 456 B / 21 instrucciones distintas
```

La carga de `mKeyIdx` baja de la fila 7 a la 18 y el prólogo hasta ese punto
queda alineado. No se retuvo porque sigue lejos del 100 %.

La primera diferencia residual ya no es la carga: el objetivo conserva el
byte `mKeySize` mediante `mr r3,r0`, mientras el compilado usa `mr r12,r0`.
Después rota los temporales de `mNumKeys`, base y tamaño. En los dos bucles el
objetivo reutiliza la base elegida con `mr r4,r11/r9`; el fuente vuelve a
materializarla con `add r4,r30,...`. Es asignación/CSE de temporales posterior
a la corrección del contrato, no el antiguo adelanto de `mKeyIdx`.

### Extensión: helper inline privado con índice real

Se midió después un helper `static inline` privado de `RawStateChan.cpp` con
firma `(RawStateChan *c, int keyIdx)`. El caller le pasa el índice real, el
helper no lo usa y devuelve sólo `c->GetKeyData(0)`; el producto permanece
fuera. Era la prueba directa de si conservar el formal no usado creaba el
pseudo de base que falta.

El binario fue **idéntico a la hipótesis 1**, no sólo igual en porcentaje:

```text
97,76316 % / 456 B / las mismas 21 instrucciones distintas
```

Por tanto, el formal no usado se elimina antes de la etapa que decide el CSE y
el reparto. El helper fue retirado y no se hizo el ajuste opcional: la regla de
la extensión era parar ante resultado idéntico o negativo.

## Hipótesis 2: nombrar el byte de tamaño

Se añadió localmente `unsigned char keySize = c->GetKeySize()` y se usó en
todos los productos y accesos de la función. La carga se izó demasiado y GCC
eliminó copias que sí existen en el objetivo:

```text
88,24561 % / 436 B
```

Esta vía queda descartada: el objetivo conserva por separado el valor QImode y
sus promociones temporales; una única local fusiona esas vidas.

## Hipótesis 3: offset de bloque nombrado

Se factorizó dentro de cada bucle
`int keyOffset = i * c->GetKeySize()` antes de sumar la base. El multiplicador
se adelantó respecto a la selección de la base y el tamaño quedó incorrecto:

```text
87,10526 % / 448 B
```

También queda descartada. El `mullw` correcto debe permanecer después de la
selección impar/par de la base.

## Hipótesis 4: inicialización de `n` en la declaración

En `FindMatchTime`, el objetivo carga el sesgo doble de la conversión
int→float antes del primer `bso`; el fuente lo carga después. Se probó una
palanca nueva de orden de expansión: sustituir `int n; ... n = FloatToInt(...)`
por `int n = FloatToInt(...)` en la lista de locales, antes de `s`.

El resultado fue byte a byte idéntico a la base:

```text
97,361115 % / 720 B / 11 instrucciones y 14 relocs nominales
```

Por tanto, el inicializador no cambia el LUID ni la disponibilidad del literal
generado. Se mantiene la veda de R38: no repetir locales/unions/barreras del
sesgo. Una nueva vuelta necesita una palanca que actúe sobre la única constante
creada por la conversión, sin introducir un segundo slot o literal.

## Siguiente acción acotada

Para `EvalState`, el siguiente experimento justificable es un helper inline
privado del `.cpp` que implemente directamente la selección impar/par de la
base mediante los accesores públicos, en lugar de envolver `GetKeyData(0)`, y
comparar su RTL con la hipótesis 1. El wrapper con formal `idx` no usado ya está
medido y es neutro. No conviene volver a nombrar `keySize` ni `keyOffset`.

Para `FindMatchTime` no queda una variante C nueva sustentada por la evidencia
actual. Hace falta inspección de RTL/scheduler de la constante generada antes de
otro ensayo.

## Verificación y artefactos

```text
python scripts/build_direct.py Speed/Indep/SourceLists/zEagl4Anim
  -> 1 ok, 0 fallidas
python scripts/fncmp.py Speed/Indep/SourceLists/zEagl4Anim
  -> inventario final idéntico al inicial; 4 de 318 distintas
python scripts/audit.py Speed/Indep/SourceLists/zEagl4Anim
  -> 314 ok, 0 FALLA
python scripts/lcfix.py --check
  -> todas las entradas @lc están al día
python scratchpad/codex_r43_anim/final_gate.py
  -> PASS
```

`before.json` y `after.json` tienen el mismo SHA-256:
`0F7D7198DA6FACBEA0D696A5F3DDCCF83716A202DC4C1CA80A04FBA678278661`.
El objeto recompilado recuperó su hash inicial
`2F539FE182B004F2812BABC21436973E7108A4AD06225C570CE4115FA084296E`.
Los fuentes recuperaron sus hashes y EOL iniciales:

- `RawStateChan.cpp`:
  `C9772865568CDB096661C6AF6707529209B18793252FFA36E02D542C4B5D3096`
- `FnRunBlender.cpp`:
  `CF06286D23FC4475398BC39FB3691CC75441AD77E89A505AE2ABBFA29BDF5788`

Artefactos reproducibles: `scratchpad/codex_r43_anim/`. No se tocaron
cabeceras, configuración, splits ni flags; no se ejecutó `ninja`, no se hizo
staging ni commit.
