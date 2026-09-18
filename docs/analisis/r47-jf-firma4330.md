# r47 — el barrido de la firma de conversión, y el eje de tamaño

## El barrido: frente vacío

La ronda cerró `FindMatchTime` descubriendo que los subproductos de una
conversión int↔float se reservan al **expandir** y sobreviven a la muerte de su
propio código, incluido el `lfd` del literal de sesgo `0x4330000080000000`. El
agente propuso barrer el árbol con esa firma: una pareja `lis 0x4330` + `lfd`
que el objetivo tenga donde nosotros no tenemos conversión.

**Barridas las 42 funciones pendientes contando `0x4330`, `lfd`, `fctiwz` y
`fsub` en los dos lados del `fndiff`: el conteo CUADRA en las 42.** No hay ni un
segundo caso. El frente se cierra sin gastarle una ronda.

Arnés: `scratchpad/r47_jf/firma4330.py` (lee `report.json`, pasa `fndiff` por
cada pendiente y cuenta los mnemónicos de conversión por lado).

## Lo que sí deja la tabla: el eje de TAMAÑO

De las 42 pendientes, **30 miden exactamente lo que el objetivo** y 12 no. Es un
eje de triaje que no teníamos tabulado, y separa dos clases de trabajo distintas:
las exactas sólo pueden ser planificación o reparto; las demás tienen una causa
estructural que hay que encontrar antes de tocar el planificador.

### Las doce con el tamaño distinto

| unidad | función | objetivo/nuestro | |
|---|---|---:|---|
| `madidct` | `IdctRow` | 516/496 | **−20** |
| `zCamera` | `__static_initialization_and_destruction_0` | 3604/3620 | +16 |
| `zPlatform` | `ActualReadJoystickData` | 1588/1580 | −8 |
| `zEagl4Anim` | `Initialize` | 2352/2356 | +4 |
| `zPhysicsBehaviors` | `UpdateLoaded` | 856/860 | +4 |
| `zWorld` | `RenderFlaresOnCar` | 2908/2912 | +4 |
| `sfir` | `calcFIRCoeffs` | 936/940 | +4 |
| `zSpeech` | `LoadSpeechBank` | 316/312 | −4 |
| `zWorld` | `CullParts` | 836/832 | −4 |
| `zWorld` | `UpdateWheelYRenderOffset` | 876/872 | −4 |
| `steering` | `Effect_PerformEnvelope` | 196/192 | −4 |
| `steering` | `SimThread_Step` | 924/920 | −4 |

Siete de las doce son **±4 B, o sea una sola instrucción**, y de esas **cinco
son de menos**: al objetivo le sobra una instrucción que nosotros no emitimos.
Ese es el caso más barato de todo el censo, y es justo el que acaba de cerrar
`FindMatchTime` (una instrucción que sobrevive a su código) y el que cerró
`EvalSQT` (un `asm` que emite la instrucción que el objetivo ya tiene).

**Aviso**: `sfir` está en +4 y su diagnóstico correcto NO es un `if` defensivo
—son 11 INSERT contra 10 DELETE, un bloque puente que PRE crea en la arista
`default`—. El signo del tamaño orienta, no diagnostica.

### Las treinta exactas

Incluyen las dos que están más cerca del árbol —`HolePunchAvoidables` (9 filas)
y `Play__16CARSFX_RoadNoise` (7 filas)— y las dos peores del censo,
`IdctColumn` y `VP6_PredictFilteredBlock`. Para todas ellas la vía es el
volcado `.greg`, no probar formas de fuente: el reparto se lee.

## Refutada: `sizeof(FloatVector)` no es la explicación del marco

El agente de `world` propuso que `FloatVector` midiera mas de 12 B, porque el
DWARF del original deja 0x10 B entre el `face[4]` exterior y el interior de
`GetLoadingPriority` y con 12 B se solaparian; y que eso explicaria de paso los
16 B que le faltan al marco (37 de 72 filas).

**El DWARF lo desmiente en una linea**: `struct FloatVector` sale con
`// total size: 0xC`, tres floats en 0x0/0x4/0x8 y sin relleno. Es exactamente
nuestro tipo.

Y el `face[4]` no es una local de `GetLoadingPriority`: sus locales son
`predict_pos` en `r1+0xB8`, `direction` en `r1+0xC0`, `v` en `r1+0xC8` y siete
escalares en registro. El `face[4]` viene de un inline mas adentro, y dos
locales de **bloques lexicos disjuntos comparten ranura de pila** por diseno en
GCC: un solape asi no es prueba de nada.

**Lo que si deja el volcado, y no se habia usado**: las tres `bVector2` con su
offset exacto de marco. Comprobar los nuestros contra `0xB8`/`0xC0`/`0xC8` es
un diagnostico directo de los 16 B, y no exige inventar ninguna local --que es
justo lo que la veda de `ICEMover` prohibe--.
