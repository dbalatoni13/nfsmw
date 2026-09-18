# Iter5 2026-09-08 — `EAXAemsManager::SetupNextLoad`

## Alcance corregido y resultado

La ruta del encargo era imprecisa: `SetupNextLoad__14EAXAemsManager` no está en
`Stream/SpeechManager.cpp`, sino en `EAXSound/EAXAemsManager.cpp`; sus 1008 B
incluyen el `DeleteRefToAsset(Attrib::StringKey)` inline de
`EAXSound/EAXAemsManager.h`. Se trabajó únicamente con copias privadas de la
SourceList completa. Fuente, cabecera y objeto de producción quedaron intactos.

No hay cierre y no se integra un parcial: **+0 B / +0 funciones**. Sí queda
demostrada una corrección de CFG y semántica que reduce el diff de **23 a 17
filas**, manteniendo 1008 B y el multiconjunto exacto.

## Primera causa real: repetir el mismo objeto tras borrar una referencia

La segunda pasada recorre los `SndBase *` guardados en `SfxToDel` en orden
inverso. En la fuente actual, cuando encuentra una petición cuyo `pThis`
coincide, ejecuta `remove(currequst)` y salta a `ContinueDeleting`; esa etiqueta
está antes de `deleteCount = idx`, así que decrementa inmediatamente y pasa al
objeto anterior.

El objetivo hace otra cosa. El mapa de líneas e instrucciones muestra que, al
terminar el `remove` inline, su rama vuelve al encabezado de la segunda pasada
(la actual fila 195 era la única rama con destino distinto). Sólo cuando el
recorrido llega al final sin otra coincidencia ejecuta `mr. r26,r7`, equivalente
a `deleteCount = idx`, y avanza al objeto anterior. Por tanto elimina **todas**
las referencias del mismo `SndBase` antes de decrementar.

La historia de la fuente confirma que esto no era una forma ya medida. El cuerpo
se introdujo en `151b5627` y `68292d35` corrigió ámbitos, postincrementos y CSE,
pero ambos conservaron el `goto ContinueDeleting` que decrementa. El objetivo,
en cambio, respalda el reinicio de `RemoveQueuedLoads`.

## Tres formas CFG medidas

| forma privada | tamaño | similitud | filas | resultado |
|---|---:|---:|---:|---|
| base | 1008 | 98,67063 % | 23 | referencia |
| V1: tras `remove`, `goto RemoveQueuedLoads` | 1008 | **99,66270 %** | **17** | mejora estructural, no cierre |
| V2: mover `ContinueDeleting` detrás del decremento | 1012 | 99,06746 % | 24 | negativa |
| V3: `while (true)` y salida explícita | 1008 | **99,66270 %** | **17** | mismo código funcional que V1 |

V1/V3 arreglan a la vez el destino de rama, el contador en r26, `idx` en r7,
su desplazamiento en r6, el `mr.` de fin de pasada y la posición del primer
`lis gFastMem@ha`. No son un pin reciclado: cambian el CFG y la conducta de la
segunda pasada. V2 conserva la conducta correcta, pero la forma de control añade
una instrucción y empeora.

Lo que queda en V1/V3 son 17 `ARG_MISMATCH`, un único ciclo de tres registros:

```text
              objetivo   V1/V3
this             r29       r27
offset SfxToDel  r27       r28
sucesor/FastMem  r28       r29
```

Ya no queda una diferencia de rama, tamaño, opcode, literal o reubicación. La
siguiente investigación debía estudiar afinidades/prioridades de esos tres
allocnos **sobre V1**, no volver a los pines de contador, índice o sucesor de
r36c/r45 ni seguir permutando el CFG. Esa extensión se completó a continuación.

## Afinidad y prioridad sobre V1

Los volcados privados `zEAXSound_v1.i.lreg/.greg` identifican los valores:

| pseudo | valor | clase | referencias / vida | registro V1 |
|---:|---|---|---:|---:|
| 82 | parámetro `this` | BASE | 12 / 241, cruza 5 llamadas | r27 |
| 429 | `deleteCount + 1` | GENERAL | 6 / 58, cruza 1 llamada | r29 |
| 430 | `deleteCount << 2` | GENERAL | 6 / 58, cruza 1 llamada | r28 |
| 372 | segundo `gFastMem@ha` | BASE | 7 / 108, cruza 1 llamada | r29 |

`global.c:627-649` ordena con
`floor_log2(n_refs) * n_refs / live_length`. Eso da aproximadamente 1493 para
82 y 2068 para 429/430; el `.greg` confirma el orden `... 429 430 82 ...`.
Los dos temporales toman r29/r28 y `this` queda en r27. Ésta es la primera causa
del ciclo residual, no una suposición basada en el fuzzy.

Se midieron las dos únicas formas C ordinarias y acotadas que añadían una copia
o afinidad real, siempre combinadas con V1:

| forma | tamaño | similitud | filas | resultado |
|---|---:|---:|---:|---|
| A1: `EAXAemsManager *manager = this`, usado en todo el cuerpo | 1008 | 99,66270 % | 17 | eliminado/coalescido; código idéntico a V1 |
| A2: `SndAssetQueue *waitForResolve = &this->mWaitForResolve` | 988 | 94,89683 % | 57 | rompe el CSE; negativa |

No se usaron pines ni `asm`. Una copia ordinaria de `this` no crea un allocno
distinto después de optimizar; hacer explícito el puntero de la cola sí lo crea,
pero elimina 20 B y desarma el inline. El siguiente paso distinto, si se retoma,
es buscar en RTL una dependencia real que eleve `n_refs` de 82 o reduzca la
prioridad de 429/430 sin emitir código; no quedan más variantes CFG ni aliases C
justificados en esta familia.

## Gates

`audit.py` pasa con 773 funciones mapeadas: 771 exactas y las mismas dos
pendientes (`SetupNextLoad` y `cStichWrapper::Play`). En las tres variantes sólo
cambia `SetupNextLoad`; `Play` y las 771 exactas no retroceden. V1 tiene 252
instrucciones, 38 ramas y 12 reubicaciones externas como el objetivo, con firmas
de tipo/símbolo/addend idénticas; las reubicaciones internas de salto de la fuente
se normalizan contra los mismos destinos. No hay pool literal. `.data`/`.bss` runtime no
cambian y `lcfix.py --check` pasa.

Hashes de producción preservados:

```text
EAXAemsManager.h      68aac05382cf1b50d468e1d7a663bc5c4d5255ca7c310ea017698e6fa0f57aac
EAXAemsManager.cpp    f539d4c50ddfb2d3ada09b222dd3f12c62f5638d98c303c83c50a6b97cc2d055
SpeechManager.cpp     b24fbfd74aa4d0f302ca4145a44f54a59115fe478c52b082f46f7058d07d9693
zEAXSound.o real      b48d2e619204a8c930b72d5b28ca7c22987ff3bbe446b7181b101d2b61900fad
```

Artefactos reproducibles: `scratchpad/codex_20260908_iter5_setupload/`, en
particular `build_shadow.py`, los JSON de cinco variantes, `baseline_lmap.txt`,
`v1_diff_rows.txt`, `variant_summary.txt`, los volcados `zEAXSound_v1.i.*` y
`audit.py`.
