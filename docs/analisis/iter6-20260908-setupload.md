# Iter6 — SetupNextLoad: umbral de prioridad de `this`

## Alcance y baseline

Se trabajó sólo con copias privadas de `EAXAemsManager.cpp` y
`EAXAemsManager.h`; producción no se recompiló ni modificó. La base de esta
ronda es V1 de Iter5: segunda pasada de `DeleteRefToAsset` reiniciada sobre el
mismo `SndBase` después de quitar una referencia. Esa corrección de CFG mide
1008 B, 99,66270 %, 17 filas.

Hashes de producción conservados:

- `EAXAemsManager.cpp`: `f539d4c50ddfb2d3ada09b222dd3f12c62f5638d98c303c83c50a6b97cc2d055`.
- `EAXAemsManager.h`: `68aac05382cf1b50d468e1d7a663bc5c4d5255ca7c310ea017698e6fa0f57aac`.
- `Stream/SpeechManager.cpp`: `b24fbfd74aa4d0f302ca4145a44f54a59115fe478c52b082f46f7058d07d9693`.
- objeto real `zEAXSound.o`: `b48d2e619204a8c930b72d5b28ca7c22987ff3bbe446b7181b101d2b61900fad`.

## Hipótesis y cuatro formas

El RTL V1 explicaba el ciclo: pseudo 82 (`this`) tenía 12 referencias/241
instrucciones y prioridad aproximada 1493; los pseudos 429/430 (sucesor y
desplazamiento) tenían 6/58 y prioridad aproximada 2068. Con la fórmula de
`global_alloc`, 13–15 referencias siguen usando `floor_log2=3`; 16 referencias
saltan a `floor_log2=4` y superan a 429/430.

Se probaron cuatro escalones acumulativos. Cada entrada adicional de `this`
está asociada mediante `asm` vacío a un valor C real, inicializado y utilizado:
`nextBankIndex`, `currentLoad`, `bankIndex` y `endIndex`. No hay operando
fantasma ni instrucción PPC escrita a mano.

| forma | bytes | porcentaje | filas distintas | resultado |
|---|---:|---:|---:|---|
| V1 | 1008 | 99,66270 % | 17 | baseline CFG corregido |
| D1 | 1008 | 99,50397 % | 21 | nueva selección al formar `g_SndAssetList` |
| D2 | 1008 | 97,916664 % | 25 | además desplaza dos `li` |
| D3 | 1008 | 97,916664 % | 25 | mismo cuerpo de función que D2 |
| D4 | 1008 | 98,25397 % | 9 | cierra el ciclo principal, pero conserva deuda nueva |

El volcado D4 confirma el mecanismo, no sólo el diff:

- pseudo 82 pasa a **16 refs/245 instrucciones** y queda en `r29`, como el
  objetivo;
- pseudo 370 (`gFastMem` de la segunda pasada) queda en `r28`;
- pseudo 427 (`deleteCount+1`) queda en `r28` y pseudo 428
  (`deleteCount<<2`) en `r27`.

Con ello desaparecen las 17 diferencias originales del ciclo
`this/sucesor/desplazamiento/FastMem`. Las nueve filas restantes de D4 no son
residuo de ese ciclo: cinco intercambian `r9/r11` al formar y guardar
`g_SndAssetList + nextBankIndex`; cuatro son dos pares delete/insert porque
`li r26,0` y `li r5,0x80` se planifican tres instrucciones más tarde. D1 y D2
demuestran qué dependencias introducen respectivamente esas dos deudas.

## Verificación y decisión

`scratchpad/codex_20260908_iter6_setupload/audit.py` verifica:

- 773 funciones mapeadas; las 771 funciones exactas vecinas siguen exactas y
  `cStichWrapper::Play` no cambia;
- D4 conserva 252 instrucciones y 38 ramas frente al objetivo;
- las 12 reubicaciones externas coinciden por posición, tipo, símbolo y
  addend; no hay literales de pool;
- secciones de datos/BSS de runtime idénticas entre sombras;
- hashes de producción intactos y `lcfix.py --check` limpio.

No se integra ninguna forma: ninguna llega al 100 %. La familia sí demuestra
la causa y el umbral exacto del ciclo de registros. La continuación correcta
debe conservar las cuatro referencias reales que elevan `this`, pero reemplazar
las dependencias sobre `nextBankIndex` y `currentLoad` por productores o valores
reales que no alteren respectivamente la selección `r9/r11` ni el scheduling de
los dos `li`; no corresponde seguir añadiendo barreras.

Artefactos reproducibles: `scratchpad/codex_20260908_iter6_setupload/` (`build_shadow.py`,
`v1.json`, `d1.json`…`d4.json`, objetos privados y volcados `.lreg/.greg`).
