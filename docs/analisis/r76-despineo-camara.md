# r76 -- despineo de zCamera (ICEManager + TrackCop)

Unidad **zCamera** (NonMatching en `configure.py`: el DOL no se mueve).
Lote: 3 barreras `asm` en 2 ficheros. **Las 3 retiradas, ninguna reintroducida
en ninguna forma.** Fuera van tambien las dos variables inventadas que las
alimentaban (`guard`, `dst`) y el `do { } while (0)` de la r67b, que era la
tercera pieza del mismo andamio.

Medida: `build_direct.py zCamera` (1 ok, 0 fallidas en todas las pasadas) +
`fndiff.py`. Instantanea `pctsnap` de las 453 funciones de zCamera antes y
despues: **solo se mueven las dos funciones del lote, cero danos colaterales.**

## Tabla

| funcion | andamio retirado | que dijo el DWARF | % antes | % despues | nuestro/objetivo | formas de C++ real probadas y descartadas |
|---|---|---|---|---|---|---|
| `ICEManager::LoadCameraShakes(bChunk*)` | `asm("" : "+r"(guard) : "r"(num_tracks))` + `int guard` + `do { } while (0)` | locales reales: `id`, `warned_overflow`(r0), `chunk`, `p_handle`, `group`(r31), `num_tracks`(r28), `track`(r30), `i`(r27). **`guard` NO existe** | 100,00 % | **96,67 %** | 164 / 168 B (**-4 B**) | `while` con `i` externa; `++i`; `int i` fuera del `for`; `track` declarado antes que `num_tracks`; `warned_overflow` movida junto al `for`; `warned_overflow` como `int`; leida dentro del bucle (73,5 %); escrita dentro del bucle; condicion invertida `num_tracks > i` (**91,3 %**); bloque `{}` extra alrededor del `for` |
| `TrackCopCameraMover::Update(float)` | `asm("" : "+m"(hcomp))` y `asm("" : "+r"(dst))` + `bVector3 *dst` | locales reales: `fe`, `up`, `Eye`, `Look`, `Zoom`, `fov`, `displacement`, `distance`(f13), `vert_comp`(f30), `horiz_comp`, `hcomp`[r1+88], `look_offset`[r1+104], `m`, `focal_dist`(f31). **`dst` NO existe** | 100,00 % | **99,16 %** | 948 / 948 B (**mismo tamano**) | ceros de `look_offset` antes del `bScale` (99,14 %); `look_offset` declarada junto a `hcomp` (99,16 %, igual); ceros en orden z/y/x (99,15 %) |

Coste total del despineo: **-3,33 pp y -4 B** en una funcion, **-0,84 pp y 0 B**
en la otra. Ningun otro simbolo de la unidad se mueve.

## LoadCameraShakes: lo que el andamio tapaba (y lo que el mapa de lineas refuta)

El comentario de la r36f/r67b afirmaba que las tres piezas eran necesarias e
*irreducibles*, y que el `li r0,0` del objetivo venia de `warned_overflow`
(DWARF: r0). **Es falso, y se comprueba en un grep.** `symbols/debug_lines.txt`
da la linea de origen de cada instruccion del ELF original:

```
0x8007EFD8  ICEManager.cpp 6813   <- warned_overflow = false  (colapsada)
0x8007EFD8  ICEManager.cpp 6814   <- int num_tracks = *p_handle   (lwz r28)
0x8007EFDC  ICEManager.cpp 6816   <- li r0,0
0x8007EFE0  ICEManager.cpp 6815   <- track = p_handle+1           (addi r30)
0x8007EFE4  ICEManager.cpp 6816   <- li r27,0
                                     cmpw r0,r28 / bge
0x8007EFF4  ICEManager.cpp 6818   <- cuerpo del bucle
```

Los **dos** ceros pertenecen a la **misma linea 6816**, la del `for`.
`warned_overflow` es la 6813 y **no emite ni una instruccion** (su marcador
colapsa sobre el `lwz` de la 6814, igual que `id` y `chunk`, que el DWARF da
por optimizadas fuera). El `li r0,0` no es una variable: es la materializacion
por *reload* de un `0` que cse propago al operando izquierdo del `cmpw` de la
guarda de entrada (`compare (const_int 0) (reg num_tracks)`, sin canonicalizar
-- por eso no sale un `cmpwi`). Nuestro cse prefiere el registro de `i` y emite
`cmpw r28,r27`, 4 B menos y el reparto r27/r28 cambiado.

Eso invalida la via que el andamio perseguia (revivir `warned_overflow`): se
probo como `bool` y como `int`, movida, leida y escrita dentro del bucle. GCC
2.95 borra el store muerto en todos los casos y el codigo no cambia; leerla de
verdad emite el test y hunde la funcion al 73,5 %. Las once formas del bucle
dan 164 B salvo las dos que empeoran. **Queda a 96,67 % y se acepta.**

## TrackCop::Update: una ranura de sched2, y nada mas

Retiradas las dos barreras, el tamano **no cambia** (948/948 B) y las 236
instrucciones estan todas presentes: sobra un unico intercambio adyacente.

```
objetivo              nuestro
addi r3, r1, 0x68     addi r3, r1, 0x68
mr   r5, r3           stfs f30, 0x58(r1)
stfs f30, 0x58(r1)    mr   r5, r3
```

`0x58(r1)` es `hcomp` y `0x68(r1)` es `look_offset`, las dos confirmadas por el
DWARF. Es un empate puro de la lista de listos de sched2: el objetivo desempata
a favor del `mr` (LUID menor). Las tres reordenaciones de fuente probadas no lo
mueven -- dos lo empeoran. El orden de declaracion de nuestras locales ya
coincide exactamente con el del DWARF (`...vert_comp, horiz_comp, hcomp,
look_offset, m, focal_dist`), asi que no hay margen ahi sin contradecir al
oraculo.

Se mantiene escrito (y comprobado otra vez en esta ronda) que el original tiene
**codigo muerto que nosotros no tenemos**: dos `bTan` y `GetGeometryPosition`
x2 + `GetVelocity`, todos con rango CERO en 0x80077018, y una local `fe`
(`FEManager*`) optimizada fuera. No emiten bytes ni lineas, asi que no son
recuperables; no es eso lo que tapaban las barreras.

## Nota de portabilidad

Las tres barreras eran `asm` de GCC: compilan en GameCube y PS2, pero X360 da
`C3861: '__asm__': identifier not found`. `guard` y `dst` no existen en el
original, de modo que el codigo que queda es ademas mas fiel, no solo mas
portable. En los dos ficheros no queda ningun `PIN` ni ninguna `BARRERA`
(`censoasm.py --clase BARRERA` ya no los lista).
