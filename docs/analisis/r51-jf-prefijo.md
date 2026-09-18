# El prefijo estándar de TU: confirmado, pero es 213 B, no 22.230

Verificación de la apuesta de `ESTRATEGIA.md` §4.1 antes de gastar una ronda en
ella. Sólo lectura del ELF original y de `splits.txt`; no toca el árbol.

## Qué decía la r50

> 21 de las 23 unidades del frente comparten el mismo hueco de `.rodata` —el
> prefijo que el original emite al principio de cada TU—. **Un solo mecanismo
> toca ~21 unidades a la vez.** No hay otra palanca en el proyecto con ese
> factor.

La prueba que daba era de **frecuencia**: `'Dist'` falta en 15 unidades,
`'MAudioReflection'` en 14, `'Attrib::Gen::audioscrape'` en 13. Eso no distingue
entre un **prefijo** —las mismas cadenas, en el mismo orden— y **21 subconjuntos
de un vocabulario común**, que serían 21 trabajos. Y un `asm()` de fichero emite
en el orden en que lo escribes, así que la diferencia lo es todo.

## Lo que hay, medido en el ELF original

### 1. El vocabulario compartido existe y pesa lo que decía

Recorriendo la `.rodata` completa de las 32 SourceLists en el ELF original:

| las cadenas que salen... | bytes | % |
|---|---:|---:|
| sólo en 1 unidad | 64.820 | 59,3 % |
| en 2-4 | 15.605 | 14,3 % |
| en 5-9 | 5.133 | 4,7 % |
| en 10-19 | 7.155 | 6,5 % |
| **en 20 o más** | **16.597** | **15,2 %** |

**16.597 + 7.155 = 23.752 B** de vocabulario compartido, contra los **22.230 B**
que la r50 midió como ausentes de nuestros objetos. El solape es esencialmente
total: **lo que nos falta es el vocabulario compartido, no contenido propio de
cada unidad.** Tiene sentido —una cadena propia la referencia el código de esa
unidad, que sí tenemos, así que está viva y la emitimos—.

**Esa mitad de la afirmación queda confirmada por una medida independiente.**

### 2. Pero el ORDEN compartido son sólo 14 cadenas

De las 43 cadenas que salen en 20 o más unidades, **18 unidades llevan las 43**.
Y entre esas 18 hay **14 órdenes distintos**.

La primera posición en la que difieren es la **14** en unas y la **16** en otras.
O sea que **las 14 primeras son idénticas en las 18**, y de ahí en adelante cada
unidad las coloca a su manera:

```
 0  GAMECUBE                                 9 B
 1  d:/mw/speed/indep/bware/inc/bware.hpp   38 B
 2  bad_alloc                               10 B
 3  %f,%f,%f                                 9 B
 4  %f,%f,%f,%f                             12 B
 5  STL                                      4 B     <- 82 B: el prefijo de bWare/STL
 6  Attrib::Attribute                       18 B
 7  Attrib::Instance                        17 B
 8  Attrib::Definition                      19 B
 9  Attrib::Class                           14 B
10  Attrib::Database                        17 B
11  Attrib::TypeDesc                        17 B
12  Attrib::RefSpec                         16 B
13  Attrib::Blob                            13 B     <- 131 B: el bloque de AttribSys
                                           ------
                                            213 B
```

A partir de la 14 empieza la divergencia. Ejemplos de la posición 14 de cada una:
`zAI` pone `Attrib::Gen::simsurface`, `zAnim`/`zLua`/`zMain`/`zSpeech`/`zSim`
ponen `MGeneric`, `zFe2`/`zGameplay`/`zTrack`/`zWorld` ponen
`Attrib::Gen::effects`, `zMisc`/`zPhysicsBehaviors` ponen `Attrib::TAttrib`.

Y el resto del bloque canónico —las 29 cadenas de la 14 a la 42— son otros
**506 B por unidad**: los `Attrib::Gen::*`, los números de versión
(`16.1.0`, `1.8.1`, `19.8.31`), `MGeneric`, `GRaceStatus` y los diez
`WorldBodyConn`/`Pkt_Body_*`/`Pkt_Effect_*`.

## Veredicto

**La apuesta se sostiene a medias, y la mitad que se cae es justo la que la hacía
excepcional.**

| | lo que decía la r50 | lo medido |
|---|---|---|
| hay un vocabulario compartido que nos falta | sí | **sí, 23.752 B** |
| es un PREFIJO con orden común | implícito | **sólo las 14 primeras, 213 B** |
| un solo mecanismo toca 21 unidades | sí | **no: 213 B × ~25 uds ≈ 5.300 B** |
| el resto | — | **~17.000 B, y cada unidad en su propio orden** |

O sea: **no es una palanca, son 21 palancas mecánicas.** Que no es mala noticia,
porque el orden de cada unidad **está en el ELF y se lee**: el trabajo pasa de
«averiguar» a «transcribir», que en este proyecto es el frente más productivo.
Pero hay que dimensionarlo bien y no venderlo como un movimiento único.

## Cómo queda el plan

1. **El bloque de 14 / 213 B sí es un solo mecanismo**, y va primero: es el
   mismo texto para ~25 unidades y se comprueba de una vez.
2. **Lo demás se genera por unidad**, leyendo del ELF el orden que le toca. Es
   un guion, no una investigación.
3. Sigue en pie todo lo que la r50 midió sobre el coste: `zSim`, `zMisc` y
   `zEcstasy` tienen **superávit** de `.rodata` y salvarles más dato las aleja;
   y en `zLua` una de las cadenas salvadas mantiene viva una vtable que
   **resucita 196 B de `.text`**. Hay que pasar `deadlink.py` y mirar las tres
   cifras, no sólo `.rodata`.
4. El bloqueo de coordinación no cambia: cualquier literal que se añada desplaza
   los `$LC` y deja rancio `keep.lst`. Sigue siendo trabajo de ventana.

## Nota de método

Esto es el caso número trece de [[nfsmw-validar-metrica]] y otro de
[[nfsmw-extrapolar-frentes]]: **un hallazgo en una unidad no es un frente hasta
contarlo en las demás.** La r50 contó la frecuencia y dedujo el orden. Son dos
preguntas distintas y sólo se había medido una.

Y una corrección a mi propio primer intento: mi comprobación inicial de «mismo
orden» comparaba cada unidad contra las **6** primeras cadenas, no contra las
43, así que salía cierta trivialmente y me dio un falso «las 30 comparten el
orden». Se cazó al preguntar cuántos órdenes distintos hay entre las 18 que
llevan el bloque completo: **14**.
