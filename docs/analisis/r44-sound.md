# r44 — Roadnoise: pin `data@r27` con asm sólo de entrada

Fecha: 2026-09-08. Ensayo único y exclusivamente sombra sobre
`Speed/Indep/SourceLists/zEAXSound2`. Producción no fue editada ni compilada.

## Hipótesis y forma

Sobre la receta `Wet_FX=-1` de 392 B se repitió el puntero real de r42, pero
sin restricción de salida:

```cpp
register FX_ROADNOISEStruct *data asm("r27") = &this->mData;
asm("" : : "r"(data));
```

`data` se usa en las dos llamadas a `Class::CreateInstance`. La hipótesis era
que una entrada solamente conservaría la relación `data = this + 4` sin
extender artificialmente su vida como hizo `+r`.

## Resultado negativo

| función | baseline | sombra R44 |
|---|---:|---:|
| `CARSFX_RoadNoise::Play` | 388/392 B, 94,938774 % | 392/392 B, **83,061226 %**, 44 filas |
| `CARSFX_RoadNoise::InitSFX` | 444 B, 100 % | 448/444 B, **90,4955 %**, 27 filas |
| `SFX_Common::MsgPlayMiscSound` | 780 B, 100 % | 780 B, 100 % |
| `CARSFX_PreColWoosh::MsgBarrier` | 140 B, 100 % | 140 B, 100 % |

El asm de entrada no actúa como una preferencia suave de color. Obliga a
materializar `&mData` en `r27` en la frontera del asm, corta la región de stores
del constructor y cambia su planificación. En `Play` aparece `mr r27,r31` y el
store de `id` se fusiona como `stwu r8,4(r27)`; la carga del handle se desplaza
después del grupo de stores. El cero de `GetRefCount` vuelve a materializarse
separadamente cerca de la llamada, de modo que no queda el intercambio aislado
que tenía `Wet=-1`.

El testigo `InitSFX` confirma que no es una solución local: renombra los
preservados, cambia el store indexado final y añade 4 B. La variante se rechaza
inmediatamente y no se hicieron controles ni permutaciones adicionales.

## Gates

- Inventario bruto idéntico: 1112 símbolos de función y 930 comparables.
- Sólo cambian `Play` e `InitSFX`; el conjunto exacto cae de 926 a 925 por la
  regresión del testigo.
- Las cuatro secciones de datos/BSS mantienen tamaño, `data_diff` y
  `reloc_diff`.
- Producción conserva los SHA-256 de r43: Roadnoise `77a0496f...`, header
  `864371ea...`, objeto `6b57df24...`.
- Los cierres protegidos `MsgPlayMiscSound` y `MsgBarrier` siguen al 100 %.

Artefactos reproducibles en `scratchpad/codex_r44_sound/`: copias sombra,
`build_shadow.py`, objeto `zEAXSound2_input_pin.o`, `baseline.json`,
`input_pin.json` y `gate.py`.
