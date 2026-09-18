# r42 — sonido: sentinelas negativos de `CARSFX_RoadNoise::Play`

Fecha: 2026-09-08. Unidad:
`Speed/Indep/SourceLists/zEAXSound2`; única fuente ensayada:
`Speed/Indep/Src/EAXSound/CARSFX/CARSFX_Roadnoise.cpp`.

## Resultado

La hipótesis nueva explica y recupera casi toda la expansión inline de
`FX_ROADNOISE`, pero no produce un cierre exacto:

| variante del argumento cero | tamaño fuente/objetivo | similitud | diferencias |
|---|---:|---:|---:|
| baseline | 388/392 B | 94,938774 % | 29 filas |
| `Wet_FX=-1` | 392/392 B | **99,64286 %** | 7 operandos |
| `HiPass=-1` | 392/392 B | **99,64286 %** | 7 operandos |
| `speed=-1` | 392/392 B | **99,64286 %** | 7 operandos |
| `secondaryNoise=-1` | 392/392 B | **99,64286 %** | 7 operandos |
| `azimuth=-1` | 392/392 B | 99,27551 % | 14 operandos |
| `volume=-1` | 388/392 B | 94,938774 % | 29 filas |
| `HiPass=-1, Wet_FX=-1` | 392/392 B | **99,64286 %** | 7 operandos |

Conforme al encargo, no se retuvo ninguna mejora parcial: fuente y objeto de
producción quedaron idénticos al baseline de r41. `Play` sigue sin contarse
como función exacta.

## Causa observada

Los setters de los seis argumentos probados saturan un entero negativo a cero,
por lo que `-1` conserva el estado final. En cuatro posiciones no tempranas
(`Wet_FX`, `HiPass`, `speed` y `secondaryNoise`) esa expresión impide que el
front-end colapse todos los ceros y recupera:

- el tamaño exacto de 392 B;
- los dos `li 0` separados que tiene el objetivo;
- el orden y los offsets de todos los stores del `FX_ROADNOISEStruct`.

Después de esa recuperación sólo queda un intercambio coherente de registros:
el objetivo usa `r29` para el cero persistente y `r27` para `&mData`; la fuente
usa `r27` para el cero y `r29` para `&mData`. Son siete operandos repartidos en
la materialización del cero, dos stores de campos, dos argumentos de
`CreateInstance` y el store temporal del refcount. No falta ni sobra ninguna
instrucción y no cambia ninguna reubicación.

`azimuth=-1` alarga otro valor y perturba además `r26/r27`, duplicando las
diferencias. `volume=-1`, por estar en el primer setter, no separa el grupo de
ceros. La única combinación permitida, `HiPass + Wet_FX`, mantiene exactamente
el mismo intercambio de siete operandos; por ello se detuvo el frente sin
repetir permutaciones ni órdenes de setters ya agotados.

La forma más simple del avance estructural es el sentinel de `Wet_FX`:

```cpp
new Csis::FX_ROADNOISE(ID, 0, 0x1000, 0,
                       Csis::FXROADNOISETYPETYPE_LOOP,
                       0, 0, 25000, 0, 0x7FFF, -1);
```

Se conserva de forma aislada y reproducible en
`scratchpad/codex_r42_sound/shadow/`, sin modificar el header real.

## Restauración y gates

- `CARSFX_Roadnoise.cpp` vuelve a tener diff cero y CRLF puro.
- El objeto real restaurado tiene el mismo SHA-256 que el baseline:
  `6B57DF24F280B1E4863064E1F60178BD2C067121F24352979F7070A7F53658B4`.
- `fncmp` antes/después es idéntico: quedan las mismas cuatro funciones
  pendientes y 2644 B no exactos.
- `SFX_Common::MsgPlayMiscSound` permanece en 780/780 B y 100 %.
- `CARSFX_PreColWoosh::MsgBarrier` permanece en 140/140 B y 100 %.
- La auditoría contra el ELF pasa para ambos cierres protegidos:
  `MsgPlayMiscSound`, 25 ramas/64 reubicaciones/0 literales;
  `MsgBarrier`, 4 ramas/9 reubicaciones/4 literales.
- No se tocaron headers, configuración, splits ni flags; no se ejecutó ninja.

## Artefactos

Directorio `scratchpad/codex_r42_sound/`:

- `CARSFX_Roadnoise_before.cpp`, `zEAXSound2_before.o`, `before_hashes.txt`;
- `Play_before.txt`, `Play_v1_*` a `Play_v7_*`: todos los ensayos completos;
- `fncmp_before.txt`, `fncmp_v*.txt`, `fncmp_after_restore.txt`;
- `Play_after_restore.txt`, `MsgPlay_after_restore.txt`,
  `MsgBarrier_after_restore.txt`, `protected_audit.txt`;
- `shadow/`, `zEAXSound2_wet_shadow.cpp`, `build_wet_shadow.py`: reproducción
  aislada de la mejor variante estructural.

## Extensión sombra: puntero real `data` fijado a `r27`

Se probó una única variante adicional, exclusivamente en la copia sombra del
header. Sobre `Wet_FX=-1`, el constructor inline de `FX_ROADNOISE` recibió un
puntero real y definido:

```cpp
register FX_ROADNOISEStruct *data asm("r27") = &this->mData;
asm("" : "+r"(data));
```

Las dos llamadas a `Class::CreateInstance` usan ese `data`; no cambió el orden
de setters ni se introdujo un valor fantasma. El resultado fue negativo:

- `Play`: 392/392 B, pero **95,5102 %** y 27 filas distintas. El pin reserva
  `r27` durante un lifetime mayor y desplaza también `this`, `loop` y el índice;
  por tanto no corrige el único intercambio `r27/r29` de la receta sin pin.
- `InitSFX`: 444/444 B, de 100 % a **97,324326 %**, con 22 filas distintas.
  Esta regresión de un vecino exacto obliga por sí sola a rechazar la variante.
- El inventario funcional conserva los mismos 1112 símbolos comparables; sólo
  cambian `Play` e `InitSFX`. `MsgPlayMiscSound` y `MsgBarrier` continúan al
  100 % en el objeto sombra.

No se hizo el control opcional ni una segunda forma del pin: la causa y la
regresión son inequívocas. Tampoco procede auditar/promover el header real,
porque `Play` no cierra y un consumidor ya exacto empeora. El header, la fuente
de Roadnoise y el objeto de producción conservan exactamente sus SHA-256 de
antes del ensayo.

Artefactos añadidos:

- `shadow/Speed/Indep/Src/EAXSound/SND_GEN/ENVIRO_AEMS.h`: copia aislada con el
  pin;
- `d_zEAXSound2_pin_shadow.json`: comparación completa objetivo/sombra;
- `pin_shadow_gate.py`: comprueba el negativo, los dos cierres protegidos y los
  hashes de producción.

Coordinación separó los dos arneses para no perder la receta sin pin:
`python scratchpad/codex_r42_sound/build_wet_shadow.py` usa el header real
intacto y reproduce Wet=-1; el mismo comando con `--pin` usa
`zEAXSound2_pin_shadow.cpp` y el header sombra rechazado. Cada modo escribe
su propio objeto scratch, nunca el objeto de producción.
