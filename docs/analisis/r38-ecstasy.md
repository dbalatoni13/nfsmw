# R38 — zEcstasy

## Resultado

No se conserva ningún cambio de fuente y no hay cierre nuevo. La SourceList vuelve
byte a byte al baseline de la tanda:

| función pendiente | tamaño | similitud final | diferencia vigente |
|---|---:|---:|---|
| `GenerateHorizonFogDisplayList` | 796 B | 98,994970 % | transposición `andi.` / `srawi` |
| `eLightMaterialPlatInterface::UpdatePlatInfo` | 2.044 B | 99,334640 % | 27 filas, ciclo FPR y un bloque de literales |
| `eProject` | 268 B | 93,970146 % | 13 filas, scheduling y dos ciclos FPR |
| `epCalculateLocalDirectionalPOS16` | 2.072 B | 93,305016 % | 164 filas y ranura huérfana de frame |

`EmitterSystem::Render` permanece protegida en **696 B / 100 %**.

## Triaje antes de ensayar

Se leyeron completos `brief-r36f.md`, `r37-cierre.md`, `r36f-ecs.md`, las notas
r36b--r36e relevantes, los comentarios actuales, DWARF, diff real e historial de
zEcstasy hasta `590dc4a0`.

- `GenerateHorizonFogDisplayList` sigue siendo exactamente la misma transposición
  de dos instrucciones. `srawi` y `andi.` comparten ciclo, pero tienen prioridad
  12 frente a 2. Las 445 formas estructurales, las barreras de ranura, cadenas y
  dependencias sobre valores reales ya demostraron que cambiarlo añade código o
  rompe el `andi.` combinado. No apareció una causa nueva y no se repitió el barrido.
- `epCalculateLocalDirectionalPOS16` conserva el frame 0x178 frente a 0x170. El
  DWARF confirma todas las ranuras útiles y deja sólo la secundaria huérfana; 447
  pruebas de la r36d agotaron pines, destinatarios y cortes de scheduling. No se tocó.
- En `eProject`, el DWARF y el mapa de líneas confirman el orden actual exacto:
  `oneOverW`, `clipX`, `clipY`, `clipZ`, `halfVP2`, `halfVP3`; `clipY` se inicializa
  en su declaración original y se consume negada después. Por eso se descartó sin
  compilar la idea de diferir su inicialización: compraría el horario contradiciendo
  la fuente primaria. La local `vp[0]` y su barrera ya se probaron en r36f.
- En `UpdatePlatInfo`, todos los tipos y el orden de locales coinciden con DWARF.
  El único bloque no puramente FPR es el de los tres literales 6,0 / 0,22 / 0,86:
  el objetivo consume el primero antes de reutilizar r11; nuestro código mantiene
  tres bases vivas y agrupa los tres `lis`.

## Única hipótesis nueva medida

Se probó una barrera **read/write** sobre el valor real de `envmap_power`
inmediatamente después de asignar 6,0 en el bloque `0x68E97F75...0xDD90E320`:

```cpp
envmap_power = 6.0f;
__asm__("" : "+f"(envmap_power));
```

Es distinta de la vieja barrera sólo de entrada retirada en `70d2f869`: fuerza una
nueva definición/uso del pseudo. La hipótesis era hacer que el `lfs 6.0` se consumiera
antes del siguiente `lis`, liberando una base para la reutilización de r11 del target.

Resultado: **99,334640 → 99,168300 %**, tamaño 2.044 B sin cambio. El horario pasa
de `lis/lis/lis/lfs` a `lis/lis/lfs`, una aproximación parcial al objetivo
`lis/lfs/lis`, pero el uso adicional rota también `diffuse_max_scale`,
`specular_min_scale`, `envmap_power` y sus consumidores. Aparecen diferencias desde
la fila 64 y el ciclo FPR principal sigue presente. Es una barrera sin respaldo DWARF
y no cierra; fue retirada.

No se intentó un pin posterior: la primera diferencia no-registro no quedó resuelta y
los pines limpios de las tres variables del ciclo ya fueron negativos en r36f.

## Restauración y auditoría

- build real aislado: `scripts/build_direct.py .../zEcstasy.o`, correcto;
- `pctsnap --umbral 0`: **0 mejoras, 0 regresiones**, 539 parejas;
- `fncmp`: las mismas cuatro funciones / 5.180 B pendientes;
- `audit.py`: las 535 funciones exactas validan bytes, ramas, referencias y literales;
- `lcfix.py --check`: limpio;
- auditor propio: `before.json` y `after.json` idénticos byte a byte; 3.004 símbolos,
  646 funciones emitidas, 38.344 posiciones de instrucciones, 6.182 operaciones
  FP/load/store, 3.363 ramas, 10.177 referencias y 23 secciones sin variación;
- los cuatro ficheros de territorio recuperaron exactamente sus SHA-256 iniciales.

Artefactos en `scratchpad/codex_r38_ecstasy/`: `before.json`, `after.json`,
`before_pct.json`, `after_pct.json`, `trial_update_output_barrier.json` y
`audit_restoration.py`.
