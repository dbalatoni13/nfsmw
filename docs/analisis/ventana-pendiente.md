# Ventana pendiente: cuatro cosas que son mías

Todo medido y validado; nada aplicado. Las cuatro necesitan que el árbol esté quieto —dos
de ellas re-extracción, que reescribe los `build/GOWE69/obj/*.o` que los agentes leen—.

---

## 1. `steering`: tres rangos de `splits.txt` (re-extracción)

Probado con los `STT_FILE` del ELF original en `r52-jf-steering.md`. El fichero
`allsrc.c` **es** `steering.c` —27 de sus 47 locales caen en el `.text` que
`splits.txt` le da—, y con eso los tres huecos se atribuyen solos:

```
LibSN/steering.c:
	.text       start:0x8031F5D8 end:0x80321810
	.data       start:0x80439F18 end:0x80439F40
+	.bss        start:0x804B9FFC end:0x804BA024      1 local: ia$1518
-	.sbss       start:0x804FF8C4 end:0x804FF8D4
+	.sbss       start:0x804FF8C0 end:0x804FF8D4      5 locales, la 1ª g_lgInitialized
+	.sdata2     start:0x80500800 end:0x80500828      9 locales, @956..@1546
```

Las tres cuadran **al byte** con lo que emitimos (`.bss` 40, `.sbss` 20, `.sdata2` 40).
`checksplits.py` pasa limpio.

**Riesgo**: `nfsmw-rango-no-basta` — los bytes salen de comodines y pasan al objeto de
`steering`, que en el orden de enlace está entre `auto_06_804394D0_data.o` y `OS.o`, así
que **se mueven**. Sólo se comprueba re-extrayendo y mirando el `sha1` del DOL.

---

## 2. Las dos fronteras que `prefijochk.py` encuentra (re-extracción)

El prefijo de bWare marca el principio de la `.rodata` de cada TU. Hay 31 en el ELF, y
**dos rangos contienen dos**:

```
zGameplay   0x803EBB48..0x803ED338   prefijos en 803EBB48 y 803EBE98
zEAXSound2  0x803D9050..0x803DD6D8   prefijos en 803D9050 y 803DD658
```

O sea:

- `0x803EBB48..0x803EBE98` (848 B) son de **`zGameModes`**, que hoy no tiene rango de
  `.rodata` ninguno. Sin esto **no puede promocionar nunca**: su objetivo tendría 0 B de
  `.rodata` y su `1.0f` está vivo, así que el `+104` es un suelo.
- `0x803DD658..0x803DD6D8` (128 B) son de **`zEcstasy`**.

**Paquete atómico** (`r53-cerca3.md`): frontera + bloque en `zGameModes.cpp` + devolver el
prefijo desde `zGameplay.cpp`, que hoy lo tiene clavado en `lbl_803EBB48` junto con 29
cadenas (409 B) ajenas. Si se mueve la frontera sin lo demás, `lbl_803EBB48` queda
duplicado.

---

## 3. Las 29 entradas huérfanas de `keep.lst`

**CORREGIDO tras medirlo**: no cuestan un byte hoy, y mi nota anterior estaba mal. El
enlazador ignora el nombre que no encuentra, y el DOL sale correcto, así que nada se pierde.
Extendí a las 29 lo que `world` midió en **una**, y no generaliza: la suya apuntaba a un
bloque que nuestra **fuente** escribe con otro nombre (`pad_05_80409D58_rodata` contra
`lbl_80409D58`); estas 29 apuntan a huecos del troceador que ya no existen. Son **fósiles**
de extracciones anteriores.

Lo que sí son es una **trampa latente**, y con destinatario concreto:

| dónde | cuántas | |
|---|---:|---|
| unidades ya promocionadas | 13 | inertes; sólo molestan si se rehace la unidad |
| unidades sin promocionar | 16 | **14 de ellas en `zEAXSound2`** |

`zEAXSound2` está en `rodata−176` y es candidata a promocionar pronto. El día que lo haga
con nuestro objeto, esas 14 entradas no preservarán lo que debían. **Hay que limpiarlas
antes de intentar su promoción, no después**, o el fallo aparecerá como un DOL roto sin
causa aparente.

No requiere re-extracción.

### CERRADO en la ventana de la r54: las 14 fuera, y las 15 restantes no son reparables

Las 14 de `zEAXSound2` se borraron (`c2c1e188`). De las **15 que quedan**, la pregunta
abierta era si alguna apuntaba a un símbolo real con **otro nombre** —el caso de la r53,
`pad_05_80409D58_rodata` contra `lbl_80409D58`, que valió 88 B—. Medido con la sonda que
busca, por la dirección del propio nombre, qué símbolo hay hoy ahí en los dos objetos:

    ARREGLABLES: 0        SIN CANDIDATO: 15

**Ninguna es reparable.** Doce nombran `gap_`/`lbl_` de troceados que ya no existen, y tres
(`PCwriteAsyncInit`, `LC9`, `LC10`) ni siquiera llevan dirección en el nombre. El caso de la
r53 era único, no una familia. Así que la acción correcta es **borrarlas**, no arreglarlas,
y sigue valiendo cero bytes: es higiene contra la trampa latente, no trabajo de cierre.

13 de las 15 están en unidades ya promocionadas, así que sólo molestan si se rehace la
unidad. Las dos que no: `asd2` (811, 812) — y `asd2` ya no existe como unidad desde
`5389b3ad`, con lo que también son inertes.

---

## 4. `zMisc`: 150 cadenas duplicadas, 2.186 B

`r52-jf-superavit.md` lo deja dimensionado y con el método. El objetivo tiene cada cadena
**una vez y dentro de la zona del bloque escrito a mano**; sobra la copia `$LC` de cc1plus.
La palanca ya existe en el propio fichero (`_bwarePrefix + offset`, usada tres veces): hay
que aplicarla a las 150 restantes. Es transcripción, y `dupstr.py` da la lista.

Esto **no es mío por naturaleza** —es trabajo de fuente— pero está aquí porque nadie lo ha
cogido y está listo para una ronda.

---

## Orden de ejecución

1. Las 29 de `keep.lst` (barato, sin re-extraer, verificar con `linkdelta` de las tocadas).
2. `steering` + las dos fronteras, **juntas en una sola re-extracción**, y `sha1` del DOL
   después. Si rompe, bisecar: son tres cambios independientes.
3. `zMisc` como encargo de agente.

---

## 5. r56: la «regresion» de zMain era el `keep.lst` RANCIO — CORREGIDO

**Mi diagnostico estaba mal y lo dejo escrito entero, porque el error es util.**

Durante la r56, midiendo `zMain` —que me habia reservado yo— vi esto:

| | antes de la r56 | durante | tras `lcfix` |
|---|---|---|---|
| `linkdelta zMain` | **`IGUAL`** | **`rodata−488`** | **`IGUAL`** |

Descarte lo obvio bien: `zMain.cpp` limpio en git, y el `.o` con el **mismo `sha1` en tres
compilaciones** (`f26f61234563`), asi que no era la no-determinismo de `build_direct`.
Pero de ahi salte a la conclusion equivocada: como los agentes habian tocado
`Physics/Smackable.h`, `PhysicsObject.h` y `Misc/PackedDecimal.h`, y `zMain` alcanza
`Smackable.h`, lo di por una **regresion cruzada de cabecera compartida**.

**No lo era.** Al mover cadenas, los agentes renumeraron los `$LC` de trece unidades y
dejaron **492 entradas de `keep.lst` apuntando al literal equivocado**. `keep.lst` es
entrada del ENLACE, asi que eso falsea `linkdelta` de cualquier unidad, la toque quien la
toque. Corrido `lcfix`, `zMain` vuelve a `IGUAL` sin tocar una linea de fuente.

Y no era solo `zMain`: `phys` lo midio por su lado y da la magnitud —con el `keep.lst`
rancio, `linkdelta` de `zPhysics` decia `rodata−384` donde hay **`+8`**, y el de
`zGameplay` `−296` donde hay `−136`—. **392 y 160 bytes de mentira.**

**LA REGLA QUE SALE DE AQUI, y es de cierre de ronda:**

1. **`python scripts/lcfix.py` es lo PRIMERO de la ventana**, antes de medir nada y antes
   de creerse ningun `linkdelta`, `trypromo` o `dolwhere` de la tanda.
2. Y por eso mismo **los agentes no pueden correrlo**: reescribe una entrada del enlace
   mientras los demas miden. Se acumula y lo aplica el jefe al cerrar.
3. Solo DESPUES tiene sentido comparar las unidades sin agente contra el dossier.

Comprobado tras el `lcfix` de la r56: `zMain` `IGUAL` y `fncmp` 0 de 1.380. **Ninguna
regresion cruzada de cabecera: no existia.**
