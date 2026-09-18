# r41 — sonido: receta exacta para `SFX_Common::MsgPlayMiscSound`

Fecha: 2026-09-08. Territorio estudiado:
`Speed/Indep/SourceLists/zEAXSound2`, con control de consumidor en
`Speed/Indep/SourceLists/zEAXSound`.

## Resultado

Se aisló una receta C++ que cierra realmente
`MsgPlayMiscSound__10SFX_CommonRC10MMiscSound`:

| estado | tamaño | similitud | diferencias |
|---|---:|---:|---:|
| base r41 | 780 B | 99,40513 % | 28 instrucciones |
| `width=-1` en casos 3/4 | 780 B | 99,86667 % | 6 instrucciones |
| `width=-1` + orden DWARF del constructor | **780 B** | **100 %** | **0** |

Con autorización explícita para el header compartido, la solución completa se
integró en producción y aporta **+780 B / +1 función**. Se retiró además el
comentario `UNSOLVED` que ya no correspondía. No se cambió configuración,
splits ni flags.

## Causa

Las 28 diferencias estaban dentro de las tres expansiones inline del
constructor `Csis::FX_UVES`; el resto de la función ya era idéntico.

La evidencia converge en dos causas independientes:

1. El DWARF original y la primera reconstrucción del header ordenan los setters
   como `SetID`, `SetVolume`, `SetWidth`, `SetPitch_Offset`, `SetIntensity`,
   `SetStop`. Antes de r41 el árbol intercambiaba los dos primeros, cambio introducido
   por `5d1ac0cf` como mejora parcial de una búsqueda anterior.
2. En los casos de mensaje 3 y 4, el line map apunta a la saturación inferior
   de `SetWidth`, compatible con un argumento negativo. Usar `-1` reproduce
   exactamente el código de esa ruta y conserva el estado final
   (`mData.width == 0`); no permite recuperar qué literal negativo concreto
   contenía la fuente original. El caso 1 permanece con `width == 0`.

El cambio integrado restaura, por tanto, `SetID` antes de `SetVolume` en
el constructor inline y usar:

```cpp
new Csis::FX_UVES(2, 0, -1, 0, 0, 0);
new Csis::FX_UVES(1, 0, -1, 0, 0, 0);
```

No hay cuerpo ASM, valor indefinido ni símbolo fantasma. Los setters escriben
campos distintos, por lo que reordenar `ID` y `volume` no cambia la semántica.

## Consumidores y alcance

La búsqueda completa encuentra sólo tres construcciones `FX_UVES`, todas en
`SFX_Common.cpp`. El header compartido entra además en:

- `zEAXSound`, mediante `EAXFrontEnd.cpp`;
- `zEAXSound2`, mediante `SFX_Common.cpp`, `MAIN_AEMS.cpp` y varios headers de
  objetos de coche.

Por eso se compilaron primero ambos SourceLists con una copia sombra del header
y, tras la autorización, se recompilaron los dos objetos reales. En
`zEAXSound` siguen pendientes únicamente `SetupNextLoad` (1008 B) y
`cStichWrapper::Play` (384 B). Las 771 funciones ya exactas pasan la auditoría
contra el ELF y el objeto real no cambia en ninguna sección runtime.

La única diferencia aparente durante el ensayo sombra fue el contador de
un estático local, por ejemplo `PlayerUpgrade.24094` frente a
`PlayerUpgrade.24796`, desplazado al incluir primero el header sombra. Tras la
comprobación uno a uno, la función señalada conservaba sus 327 instrucciones;
esto sólo sirvió como diagnóstico del arnés sombra. La auditoría final de los
objetos reales **no normaliza esos sufijos**: exige identidades crudas iguales.

## Ensayos negativos retirados

Se probaron hipótesis locales antes de identificar el argumento `width`:

| variante | resultado |
|---|---:|
| keepalive de argumentos reales | 836 B, peor |
| pin sin ASM de cuerpo | 860 B, peor |
| temporales locales / puntero local | neutro o peor |
| `volume=-1` | 772 B, peor |
| `ID=-1` con `width=-1` | no cierra |
| `width=-1` en los tres sitios | no cierra |
| `stop=-1` o `intensity=-1` | no cierra |
| alterar ID del caso 3 | no cierra |

Ninguno quedó en las fuentes reales. Tampoco se reabrieron `ProcessUpdate`,
`BindToData`, `MsgBarrier` ni las funciones de `CARSFX_RoadNoise`.

## Auditoría final de producción

- Compilación directa de `zEAXSound` y `zEAXSound2` con sus flags reales.
- `fndiff`: 195/195 instrucciones y 780/780 B idénticos.
- `scripts/audit.py`: 25 ramas, 64 reubicaciones con tipos/addends correctos y
  cero literales; comparación byte a byte contra el ELF aprobada.
- Unidad `zEAXSound2`: 925 -> 926 funciones exactas; sólo cambia y se gana
  `MsgPlayMiscSound`. `MsgBarrier` permanece en 140 B/100 %.
- Unidad `zEAXSound`: 771 exactas antes y después; `fncmp` idéntico y auditoría
  completa aprobada.
- `production_audit.py`, sin normalización de nombres: tablas de símbolos ELF
  crudas idénticas (2674 entradas en `zEAXSound`, 2807 en `zEAXSound2`), con
  nombre, tipo, sección, offset y tamaño conservados. Las cuatro secciones de
  reubicación runtime de cada objeto son byte a byte idénticas.
- En `zEAXSound2` cambia exclusivamente el intervalo `.text` de
  `MsgPlayMiscSound`; todos los bytes alloc fuera de ese intervalo, así como
  `.rodata`, `.data`, `.bss` y `.ctors`, permanecen idénticos. En `zEAXSound`
  no cambia ningún byte runtime.
- `scripts/lcfix.py --check`: limpio.

## Artefactos

Directorio `scratchpad/codex_r41_sound/`:

- `shadow/`, `zEAXSound*_shadow.cpp`, `build_shadow.py`: reproducción aislada;
- `MsgPlay_shadow_reproduced.txt` y `MsgPlay_shadow_audit.txt`: cierre y auditoría;
- `production_audit.py` / `production_audit.txt`: aislamiento estricto de los
  objetos reales, símbolos y reubicaciones;
- `shadow_audit.py` / `shadow_audit.txt`: diagnóstico previo a la autorización;
- `shadow_zEAXSound*_audit.txt`: auditoría completa de funciones exactas;
- `final_zEAXSound*.json`, `final_zEAXSound*_audit.txt`: objetos reales finales;
- `MsgPlay_final.txt`, `MsgBarrier_final.txt`, `final_function_audit.txt`:
  evidencia individual de ambos cierres preservados;
- `fncmp_zEAXSound*_*.txt`: inventarios real/sombra;
- `diagnose_shadow_diff.py`: explicación del contador local;
- `MsgPlay_v*.txt`: negativos cuantificados.
