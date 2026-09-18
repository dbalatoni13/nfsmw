# r43 — Roadnoise: lifetime de `GetRefCount`

Fecha: 2026-09-08. Unidad exclusiva:
`Speed/Indep/SourceLists/zEAXSound2`; función investigada:
`CARSFX_RoadNoise::Play` (392 B objetivo). Todos los ensayos se compilaron en
`scratchpad/codex_r43_sound/` con los flags reales; producción no se editó ni
se recompiló.

## Punto de partida y evidencia RTL

La receta aislada de r42 (`Wet_FX=-1`) recupera 392/392 B y 99,64286 %. Sus
siete diferencias son únicamente operandos: el objetivo colorea el cero de
vida larga en `r29` y `&this->mData` en `r27`; nuestra compilación los colorea
en `r27` y `r29`, respectivamente. Las dos vidas interfieren entre el primer y
el segundo `CreateInstance`, y el cero continúa hasta el `stw` del `refCount`
en `r1+8`.

`regmap.py --all` y `dwbody.py` confirman que los cuatro nombres del DWARF, sus
ámbitos y el árbol de inlines son idénticos: parámetros `ID/r29`, `side/r28`,
`attribID` sin registro y `refcnt` sin registro. Por tanto el intercambio no
pertenece a una local nombrada: está en dos temporales del compilador. `lmap`
sitúa el `stw r29,8(r1)` objetivo dentro del `refCount = 0` de
`FX_ROADNOISE::GetRefCount`.

## Tres formas causales medidas

No se repitieron sentinelas, órdenes de setters ni el pin `data@r27`:

| forma | resultado de `Play` | filas | testigo `InitSFX` |
|---|---:|---:|---:|
| V1: declarar `refcnt` antes del `new`, asignar después | 392 B / 99,64286 % | 7 | 100 % |
| V2: una inicialización con coma, conservando la recarga del slot | 392 B / 99,64286 % | 7 | 100 % |
| V3: usar el resultado de la asignación como receptor | 380 B / 92,05102 % | 28 | 100 % |

V1 y V2 son byte-idénticas a `Wet_FX=-1`: la declaración exterior de
`refcnt` no participa en el allocno del cero. El valor efectivo se escribe en
la ranura de pila de la local interna del inline; el resultado exterior está
muerto y se elimina antes de RTL.

V3 sí crea una dependencia real distinta, pero demuestra el otro límite: el
compilador conserva directamente el puntero devuelto por `new`, elimina el
`lwzx` que el objetivo hace para releer `m_pRoadNoiseControl[side]`, deja de
necesitar dos preservados y reduce el marco de `0x30` a `0x28`. No es una
palanca de coloración aislada; cambia el grafo de interferencia y el cuerpo emitido.

## Extensión en header sombra

Se midió inmediatamente la propuesta anterior en una copia privada de
`ENVIRO_AEMS.h`, siempre sobre `Wet_FX=-1` y sin el pin rechazado de r42:

| forma interna de `GetRefCount` | `Play` | filas | `InitSFX` |
|---|---:|---:|---:|
| V4: `int refCount; refCount = 0;` | 392 B / 99,64286 % | 7 | 100 % |
| V5: `int refCount; *(volatile int *)&refCount = 0;` | 392 B / 99,64286 % | 7 | 100 % |

V4 y V5 generan objetos completos **byte a byte idénticos entre sí** (SHA-256
`97821a77...`). La explicación es concreta: `refCount` ya escapa por dirección
a `Class::GetRefCount(&refCount)`, una llamada opaca. Por ello el store inicial
a la ranura `r1+8` ya es una escritura materializada y observable; separar la
declaración o cualificar exactamente esa escritura como `volatile` no crea una
vida nueva ni cambia el pseudo que CSE comparte con los ceros del constructor.

## Veredicto

Las formas locales probadas en el uso exterior y en la escritura de
`GetRefCount` no cambian sólo los colores `r27/r29`: las que preservan
la recarga se normalizan al mismo RTL, y la forma con dependencia directa
elimina esa recarga. Las dos formas adicionales autorizadas también son
neutras. El frente se detiene sin proponer ni medir más variantes y sin retener
ningún parcial.

## Gates finales

- Producción coincide byte a byte con el baseline: fuente Roadnoise SHA-256
  `77a0496f...`, header real `864371ea...` y objeto real `6b57df24...`.
- `fncmp`: 930 funciones comparables, las mismas 926 exactas; cuatro
  pendientes/2644 B (1112 símbolos de tipo función en el inventario bruto).
- Auditoría ELF: `MsgPlayMiscSound` 780 B, 25 ramas, 64 relocs, 0 literales;
  `MsgBarrier` 140 B, 4 ramas, 9 relocs, 4 literales. Ambas pasan.
- En los cinco objetos sombra el inventario funcional y el conjunto de 926
  exactas son iguales al baseline; sólo cambia `Play`. Las cuatro secciones de
  datos/BSS conservan tamaño, `data_diff` y `reloc_diff`.
- `lcfix.py --check`: todas las entradas al día.

Artefactos: `baseline.json`, los cinco `v*.json` y `zEAXSound2_v*.o`, copias
before, `build_shadow.py` y `gate.py` bajo `scratchpad/codex_r43_sound/`.
