# El porcentaje `data` de objdiff: qué mide y por qué no sirve para guiar

## Lo que dice y lo que pasa

`objdiff` da **31,89 % de datos casados** (409.976 de 1.285.748 B) y pone
**0,0 %** a casi todas las SourceLists: zPlatform 0,0 % de 141.188 B, zMain 0,0 %
de 71.864, zTrack 0,0 % de 55.648.

Comparando los mismos objetos **byte a byte** en vez de por nombre:

| unidad | contenido ya igual | objdiff dice |
|---|---|---|
| `zTrack` | **92,9 %** | 0,0 % |
| `zMain` | **80,0 %** | 0,0 % |
| `zPlatform` | **64,3 %** | 0,0 % |

**La causa: `objdiff` empareja los símbolos de datos por NOMBRE.** Los del
objetivo son `lbl_<dirección>` puestos por `dtk` al extraer; los nuestros son
`$LC*` del compilador y estáticos manglados. No casa prácticamente ninguno, así
que el porcentaje mide **coincidencia de nombres**, no de bytes.

Es la misma familia que el «0 % que miente» del `.text`, pero al revés y mucho
mayor: allí un nombre distinto escondía trabajo hecho en una unidad; aquí lo
esconde en **treinta**.

**Corolario: subir el porcentaje `data` de objdiff no es un objetivo.** Se subiría
renombrando símbolos, que no cambia un byte del DOL.

## La medida que sí sirve: `scripts/datacmp.py`

Compara el contenido de cada sección con datos contra el objeto extraído, byte a
byte y en orden. `.bss`/`.sbss` no tienen contenido (NOBITS): de esas sólo el
tamaño.

    33 SourceLists: 307.905 B distintos de 851.388  ->  63,8 % YA IGUAL

y el orden de trabajo real, que no se parece al de objdiff:

| distintos | total | ya bien | unidad |
|---|---|---|---|
| 50.381 | 141.232 | 64,3 % | `zPlatform` |
| 45.093 | 51.748 | **12,9 %** | `zFeOverlay` |
| 22.731 | 34.132 | 33,4 % | `zFe2` |
| 20.270 | 43.108 | 53,0 % | `zWorld` |
| 15.685 | 99.760 | 84,3 % | `zEcstasy` |
| 14.481 | 72.456 | 80,0 % | `zMain` |
| 12.988 | 17.376 | **25,3 %** | `zMisc` |
| 8.552 | 9.524 | **10,2 %** | `zLua` |
| 3.949 | 55.644 | **92,9 %** | `zTrack` |

## Y el hallazgo accionable: el prefijo de 92 B, contado

Con el detalle por sección, el dato que más manda es **dónde cae la primera
diferencia**:

**24 de 30 SourceLists tienen su `.rodata` distinta DESDE EL BYTE 0.**

    zPlatform, zFeOverlay, zFe2, zWorld, zMisc, zPhysicsBehaviors, zEAXSound2,
    zFe, zAI, zSpeech, zEAXSound, zCamera, zPhysics, zGameplay, zBWare, zSim,
    zTrack, zWorld2, zEagl4Anim, zRender, ...

Y las seis que empiezan bien son **exactamente** las que la r32 arregló, con la
primera diferencia justo donde acaba el prefijo que se les escribió:

    zMain +0x5C · zLua +0x5C · zAnim +0x5C · zAttribSys +0x5C   (92 = 0x5C)
    zFEng +0x98 (152)                                zEcstasy +0x4

O sea que **el trabajo de la r32 se ve en la medida al byte**, y queda
identificado el siguiente lote: **escribir el prefijo de bWare/STL en las otras
24**. La receta está en `docs/analisis/r32-pre.md` y el `asm()` plantilla en
`zDebug.cpp`; **la dirección del símbolo es distinta en cada unidad** (sale del
`start` de su rango `.rodata` en `splits.txt`).

Ojo con la lectura: `datacmp.py` cuenta «bytes iguales **en el mismo
desplazamiento**», así que mezcla contenido y colocación. Un 92,9 % como el de
`zTrack` quiere decir que su dato **y su disposición** ya están casi bien; un
12,9 % como el de `zFeOverlay` puede ser contenido malo o un desfase temprano —
el desplazamiento de la primera diferencia lo distingue.
