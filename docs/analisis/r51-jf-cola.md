# La cola real de las 23, medida en el enlace

Barrido de `linkdelta.py` sobre las 23 SourceLists sin enlazar, con la
herramienta ya arreglada (antes no resolvía ninguna unidad de biblioteca y
colapsaba seis basenames repetidos). Es la distancia **al enlace**, no a los
objetos.

## La tabla

```
unidad                    .text   resto
zSim                        +0    IGUAL
zGameModes                  +0    rodata+104
zEcstasy                    +0    rodata-288  data+64   bss+32
zSpeech                     +0    rodata-624
zPhysics                    +0    rodata-784  bss-32
zEagl4Anim                  +4    rodata-864  data-96   bss-128
zWorld2                     +0    rodata-808  data-256  bss+64
zAnim                       +0    rodata-1240 data-192  over-16
zEAXSound2                  +0    rodata-1184 data-288  bss-32
zPlatform                   -8    rodata-1408 data-64
zAI                         +0    rodata-1344 data-384
zMain                       +0    rodata-1720 data-32
zCamera                    +16    rodata-1384 data-224  bss+32
zEAXSound                   +0    rodata-1544 data-384  bss+32
zLua                        +0    rodata-1656 data-320
zTrack                      +0    rodata-1784 data-416  bss-160
zPhysicsBehaviors           +4    rodata-936  data+416  bss-988
zFe                         +0    rodata-2456 data-32
zGameplay                   +0    rodata-2656 data-128
zMisc                       +0    rodata+2840 data+32
zWorld                      +0    rodata-4136 data-128
zFe2                        +0    rodata-4216 data-160  over-16
zFeOverlay                  +0    bss-4096    sbss+4    over-1136
```

## Lo que dice

**El `.text` sale a +0 en 19 de 23.** Las cuatro excepciones son minúsculas:
`zCamera +16`, `zPlatform −8`, `zEagl4Anim +4`, `zPhysicsBehaviors +4`. El
frente **no es de código**, confirmado ahora con la herramienta arreglada.

**Casi todo es `.rodata`**, y el orden de la cola no se parece al que daba el
porcentaje: `zEcstasy` está a 288 B y `zFe2` a 4.216 B, y las dos aparecían
juntas en cualquier lista ordenada por «% de datos».

**Tres tienen SUPERÁVIT**, y ahí el trabajo es al revés —quitar, no poner—:
`zMisc` +2.840, `zGameModes` +104 y `zEcstasy` +64 de `.data`.

**`zFeOverlay` no juega en la misma liga**: no tiene `.rodata`, tiene `over` y
un `bss−4096`. Es la unidad de la sección `.over` y necesita su propio análisis.

## Y una limitación de herramienta que conviene saber

`dolwhere.py` —el que da el mapa byte a byte con el símbolo al que cae cada
diferencia— **sólo funciona cuando los tamaños de sección ya coinciden**. Si no,
para en `LAS SECCIONES NO COINCIDEN` y no llega a comparar.

O sea que de las 23, **la única sobre la que hoy se puede correr es `zSim`**.
Eso explica por qué nadie tenía ese mapa: no es que no se hubiera intentado, es
que la herramienta no llega hasta que la unidad está a delta cero en tamaños.

**El orden de trabajo que se deduce**: primero cerrar el déficit de `.rodata`
—que es lo que mide esta tabla—, y sólo entonces `dolwhere` se vuelve utilizable
y dice qué símbolos concretos quedan. En `zSim`, que ya llegó ahí, la respuesta
fueron **4.688 B casi todos en entradas de vtable**.
