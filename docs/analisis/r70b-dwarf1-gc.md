# r70b: EL .debug DE 92 MB DEL ELF DE GAMECUBE — el oráculo definitivo

## Qué hay (verificado, no supuesto)

`orig/GOWE69/NFSMWRELEASE.ELF` — nuestro propio objetivo — lleva 92 MB de
`.debug` que NADIE habia abierto (lmap usa solo `.line`). Contiene los DIEs
DWARF-1 del build FINAL con:

- **Nombres de locales de las funciones** (verificados en el binario):
  `hole_punch_safety_margin` (+0x46b185f), `f_route_param_lenght` (+0xa15f9f),
  `halfVP2` (+0x15322ae), `vert_comp`/`look_offset` (+0x9e82cc/3b), `SignedMod`,
  `GetKeyData`...
- **El compilador exacto por unidad**: `XGNU C++ 2.95.3 SN BUILD v1.76 for
  Nintendo Gamecube` (mas fino que nuestro "ProDG 3.9.3").
- **Rutas de la maquina de build**: `D:/mw/Speed/...` y `D:\mw\Speed`.
- Prueba de coherencia: `interpolationTime` (nombre que INVENTAMOS en r65 para
  MsgBarrier) NO esta — y `halfVP2` (nombre original) SI. El filtro es real.

Esto SUPERA al mdebug del PS2 (A124, alpha): aqui es el build final, compilador
correcto, sentencias tardias incluidas (los bTan de TrackCar, RenderFlaresOnCar).

## Formato (DWARF-1, decodificado a mano en la zona de vert_comp)

```
... [u32][0x000c] [u32 strx] 00 38 'distance\0'
    0000 5500 0e00 2300 0501 0000 003f 0000     <- attrs
    [u32][0x000c] [u32 strx] 00 38 'vert_comp\0' ...
```

- `0x0012` repetido = **AT_mod_fund_type** (DWARF-1: tipo fundamental
  modificado — lo que es un local float/entero en memoria o registro).
- `0x000e` = TAG_variable? (14), `0x000d` = TAG_formal_parameter (13).
- TAG_inlined_subroutine = **0x1d** — el arbol de inlines con nombres vive aqui.
- Los strx apuntan dentro de .debug (no auto-referentes; base por determinar).

Secciones hermanas: `.line` (5,5 MB, ya usado), `.debug_srcinfo` (1,3 MB),
`.debug_sfnames` (243 KB, nombres de fuente), `.debug_aranges` (18 KB),
`.debug_pubnames` (501 KB).

## Pendiente (la herramienta)

`scripts/dwarf1.py`:
1. Parser de DIEs DWARF-1: [len:u4][tag:u2]{attr:u2 + valor(2|4)} — validar
   contra la zona de vert_comp (+0x9e82cc) y SignedMod (+0x5d7ffa).
2. Por funcion (via .debug_pubnames/aranges): volcar locales con nombre,
   tipo (resolver AT_mod_fund_type/AT_mod_u_d_type a float/int/ptr) y
   LOCALIZACION (stack offset o registro — el dato que cerro ICEMover).
3. Arbol de TAG_inlined_subroutine con nombres.
4. Aplicarlo a las vedas abiertas: TrackCar (R3: que ocupa f0/f13),
   RenderFlaresOnCar (480 filas), EvalState, DynamicLoader, epCalculate
   (los double con nombre), MsgBarrier.

## Relacion con las otras fuentes

| fuente | build | cobertura |
|---|---|---|
| **GC .debug (esto)** | FINAL | todo, con nombres del original |
| GC .line | final | ya usado por lmap |
| PS2 mdebug | A124 alpha | indep/ sin tardios |
| XBOX A138 XBE | alpha 138 | diccionario de nombres (sin debug binario) |
