# Ronda 31 / ventana — el dato duplicado, extrapolado a las 33

Encargo: extrapolar el hallazgo de `zDebug` (dato escrito a mano en un `asm()`
que el compilador emite *también*) al resto de las SourceLists. **Contado, no
supuesto** — la regla del proyecto es que un hallazgo no es un frente hasta
contarlo, y tres extrapolaciones mías salieron falsas en una ronda.

## El dato que lo ordena todo: los objetos del original NO TIENEN NI UN `$LC`

`scripts/lcpool.py`, las 33 unidades:

```
TOTAL delta -4.584 B   nuestro $LC 142.460 B   objetivo $LC 7.616 B
```

Los 7.616 B del objetivo son **de una sola unidad** (`zMisc`, más `zAttribSys`).
En las otras 31, **el objetivo tiene cero `$LC`**: todo su `.rodata` son `lbl_`
—las etiquetas con que `dtk` bautiza el pool anónimo—, nombres y relleno.

O sea: **el pool de literales existe en los dos lados; lo que cambia es el
nombre**, `lbl_<dirección>` contra `$LC169`. Eso ya lo decía el §1 del brief y
sigue siendo cierto **como descripción**. Lo que estaba mal era la consecuencia
que saqué (que bastara ceder el rango al comodín): no basta, porque el problema
no es de atribución sino de **cuántos bytes emite cada lado**.

## Y ahí las 33 se parten en dos frentes distintos

### A) SIETE unidades escriben su pool A MANO — y les sobra el del compilador

En estas, el `asm()` del fuente reproduce el pool del original y **el total
casa EXACTO**:

| unidad | `lbl_` nuestro | `lbl_` objetivo | `$LC` nuestro (SOBRA) | delta |
|---|---|---|---|---|
| `zEcstasy` | 7.508 | **7.508** | 5.146 | **+5.632** |
| `zMisc` | 291 | **291** | 11.167 (obj. 6.804) | **+4.768** |
| `zRender` | 4 | **4** | 1.102 | **+1.208** |
| `zMiscSmall` | 352 | **352** | 583 | **+648** |
| `zDynamics` | 436 | **436** | 286 | **+296** |
| `zMission` | 4 | **4** | 14 | **+24** |
| `zDebug` | 328 | 336 | 12 | +8 (**ya promocionada**) |

**El `lbl_` escrito a mano no sobra: casa byte a byte. Lo que sobra es el pool
`$LC` que `cc1plus` emite por su cuenta**, porque nuestro código escribe la
constante donde el original **referenciaba el símbolo del pool**.

**La aritmética cierra en `zDynamics`**, que es el caso limpio: objeto +296,
DOL +192; la diferencia de 104 B es el `lbl_` que `-strip-unused-data` tira por
no estar referenciado. Si el código usara las 62 etiquetas en vez de literales,
las 436 quedarían referenciadas y el `$LC` desaparecería: **436 = 436, exacto.**

Total del frente: **12.576 B** en seis unidades, y `zDynamics` (28.976 B de
código, `.text` ya al 100 %) es la más barata.

**La cura y su regla, ya medidas por `und` en esta misma ronda**: declarar
`extern const float lbl_XXXXXXXX;` y usarlo en el código — **con la definición
AL FINAL de la unidad de traducción**. Con el inicializador visible en el punto
de uso GCC pliega la carga y rompe la función (`OnManageTime` cayó de 100 % a
97,61 %). La técnica ya está en el árbol: `UMath.cpp` la usa.

### B) LAS OTRAS 26 no escriben pool, y van CORTAS

`lbl_` nuestro **0**, y el `$LC` que emitimos es **menor** que el `lbl_` del
objetivo: `zAI` 7.795 contra 8.453, `zWorld` 10.854 contra 14.394, `zFe2` 13.275
contra 17.011. Ahí no sobra: **falta pool**, y lo confirmó `und` por otra vía
(forzando `keep.lst` con todos los símbolos de nuestro objeto, `zFoundation`
sólo recupera 32 B de 1.248: los bytes no existen).

Parte de ese déficit ya tiene nombre, y es de `lk`: **la `.rodata` de 31 de 33
empieza por el mismo pool de bWare/STL de 92 B**, con la ruta
`d:/mw/speed/indep/bware/inc/bware.hpp` — el `__FILE__` de la máquina de EA.
**No se puede generar: hay que escribirlo.**

## El censo de duplicados de verdad

`scripts/dupdata.py` busca lo que pasó en `zDebug`: un símbolo escrito a mano
cuyo gemelo **lo emite el compilador**, mismos bytes y mismo tamaño.

```
6 unidades, 6.146 B: zEcstasy 3.766, zMisc 2.080, zDynamics 288,
                     zMiscSmall 4, zMission 4, zRender 4
```

**Ojo con leer esto como «6.146 B que borrar»**: en `zDynamics` los 62 «gemelos»
son 62 direcciones distintas del original que casualmente tienen el mismo valor
(muchas veces `$LC169`), y el compilador las deduplica en una. Borrar las de
mano nos dejaría **cortos**, no a cero. El duplicado real de `zDebug` era otra
cosa: allí el original **sí** tenía una sola copia.

**Sirve como filtro, no como lista de borrado.** Lo que hay que quitar es el
pool del compilador, y eso se quita usando las etiquetas, no borrándolas.

## Herramientas nuevas

- **`scripts/lcpool.py`** — el pool `$LC` que emitimos contra el `lbl_` del
  objetivo, por unidad. Es la tabla que parte las 33 en los dos frentes.
- **`scripts/dupdata.py`** — símbolos escritos a mano cuyo gemelo emite el
  compilador, y símbolos **ajenos** (definidos a mano con una dirección que no
  cae en ningún rango que `splits.txt` da a esa unidad; en `zDebug` era
  `kFloatScaleUp_8045B100`, de zDynamics). Hoy: **0 ajenos** en las 33, así que
  esa mitad del hallazgo de `zDebug` **no se extrapola**.
