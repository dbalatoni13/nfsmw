# ppc2D2 fase 14: `CompletePCreadAsync` implementada, 0 → 93,70 %

Continuación del plan `scratchpad/phase14_ppc2d2_plan.md`, que jferr dejó
preparado y sin implementar. **No cierra**: 0 bytes de `matched_code`.

## Estado

| | |
|---|---|
| Antes | `extern void CompletePCreadAsync(void);` — el cuerpo no existía |
| Ahora | **93,7010 %**, 396 B, **faltan 0, sobran 2** |
| Objetivo | 388 B, 97 instrucciones |
| Unidad | 1148/2448 B, 46,90 % (sin cambio: es todo-o-nada) |

## Lo que se implementó

La forma en C del plan, literal, en el sitio del marcador de posición (después de
`PCrwSyncFSACK`, que es lo que restaura el enlace local del objetivo sin tocar
ninguna cabecera compartida). Los dos bucles de drenaje van **duplicados a
propósito**, y las instantáneas explícitas de contador y puntero conservan el
orden de carga/almacén: se decrementa y guarda la cuenta, se avanza y guarda el
puntero, y **se recarga la cuenta** para la comparación.

## Ensayos numerados

| | forma | resultado |
|---|---|---|
| c0 | la del plan, con `while (...) ;` para el sondeo | 88,3402 %, 416 B, faltan 0 **sobran 7** (`lis-3, andi.-1, beq-1, lwz-1, ori-1`) |
| **c1** | **el sondeo como `do { } while (...)`** | **93,7010 %, 396 B, faltan 0 sobran 2** (`lis-2`) ← se queda |
| c2 | puntero `const` a 0xCC006828 izado | idéntico a c1 |
| c3 | puntero normal a 0xCC006828 | 89,3711 %, **388 B — el tamaño exacto** pero `faltan 2, sobran 2`. Descartado: el tamaño no es la medida |
| c4 | `((volatile unsigned int *)0xCC000000)[0x6828/4]` | idéntico a c1 |
| c5 | igual, en el sondeo también | idéntico a c1 |
| c6 | puntero base local para los dos MMIO | 88,4124 %, 376 B, faltan 8 sobran 5 |

**Lo que enseñó c1**: con `while (cond) ;` GCC **rota el bucle** y emite un
`lwz`+`andi.`+`beq` de prueba antes de entrar; el objetivo no lo hace. Cinco de
las siete instrucciones sobrantes eran eso. Vale para cualquier sondeo de MMIO
del árbol.

**Lo que enseñó c3**: séptimo caso medido de «el tamaño exacto miente».

## Lo que queda, diagnosticado

Sobran **dos `lis`** y el marco lo explica: el objetivo abre `stwu r1,-0x28` con
`stmw r24,0x8(r1)` — **ocho registros salvados** — y nosotros `stwu r1,-0x30` con
`stmw r23,0xc(r1)` — **nueve**. O sea que usamos un registro más y aun así
rematerializamos dos direcciones. Las dos son el `0xCC006828` de las dos regiones
de drenaje: el objetivo mantiene una sola base viva entre ambas.

Las seis formas de fuente probadas no lo consiguen, y las que reducen el `lis`
(c3, c6) suben el número de instrucciones que faltan. **Es reparto de registros,
no forma de fuente** — la misma familia que el mecanismo del `@ha` de larga vida
que hay abierto en zWorld/zPhysics/zGameplay.

## Lo que NO se ha probado

- El ejecutor pareado objetivo/fuente que pide el plan para cubrir las 97
  instrucciones y las dos copias de los bucles: no se ha escrito.
- La selección de entrada fase 1 contra fase 2 con estados de lectura activos.
- Restricciones de registro: el brief las permite como último recurso y **no se
  han usado**, porque la búsqueda de forma de fuente no está agotada — quedan por
  probar las formas que reducen a ocho los registros salvados.
- El censo de llamantes es el del plan (`bl` directas); no cubre punteros a
  función.
