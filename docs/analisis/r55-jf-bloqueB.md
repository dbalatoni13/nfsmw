# El bloque B no es «a una función», y un `= {}` valía 916.004 B

Ventana de la r55, mientras los siete agentes trabajan. Medido sobre las unidades que **no
posee ningún agente** esta ronda: `zPhysicsBehaviors`, `zTrack`, `zPlatform`, `criticalpath`.

---

## 1. La premisa del bloque B está mal

El catálogo lo llama «6 unidades a UNA función, 5.728 B que desbloquean 690.484». Medido con
`reorden.py` y `linkdelta`, **ninguna de las tres que he podido medir está a una función**:

| unidad | `reorden` (contenido real) | `linkdelta` |
|---|---|---|
| `zPhysicsBehaviors` | **0 palabras** en las cuatro secciones | `.text +4`, `rodata−40`, `data+416`, `bss−988` |
| `zTrack` | 28 palabras = `GetLoadingPriority` | `.text +0`, `rodata−352`, `data−416`, `bss−160` |
| `zPlatform` | 2 palabras en `.data` + función 8 B corta | `.text −8`, `rodata−728`, `data−64` |

En las tres hay **un déficit de datos que nadie estaba contando**, y en `zPhysicsBehaviors` la
función no es el bloqueo en absoluto: `reorden` dice que **no queda nada que escribir**, y su
única función abierta (`UpdateLoaded`) quedó **vetada estructuralmente** en la r54 —`cse2`
deshace siempre la copia de PRE, `COST 0` contra `1`—.

O sea que llevábamos contando `zPhysicsBehaviors` como «856 B de trabajo» cuando su trabajo
real es de datos y su función es inalcanzable.

---

## 2. `ScratchPtr.h`: un `= {}` que valía 916.004 B

El censo de la r55 daba a `zPhysicsBehaviors` **el mayor coste por unidad de todo el censo**:
dos símbolos, 640 B, **916.004 B de desplazamiento**. La causa está en una línea:

```cpp
// src/Speed/Indep/Src/Main/ScratchPtr.h:75, dentro de IMPLEMENT_SCRATCHPTR
template <> ALIGNVEC DATATYPE *ScratchPtr<DATATYPE>::mPointer[DATATYPE::MaxInstances] = {};
```

**Medido con una mini-TU antes de tocar nada**, que es lo que evita la novena extrapolación
falsa:

| | símbolo emitido | sección |
|---|---|---|
| con `= {}` | `_t10ScratchPtr1Z1D.mPointer`, 16 B | **`.data`**, definición real |
| sin inicializador | **ninguno** | — |

O sea que quitar el `= {}` **no lo pasa a `COMMON`: hace que no lo definamos**. Y entonces el
`COMMON` del blob original (`auto_07_804FEA4C_bss.o` / `auto_07_804F4040_bss.o`) es el único
que reclama el símbolo y lo coloca donde el original lo tiene.

**El radio de la edición es exactamente una unidad**, comprobado: la macro se expande en dos
sitios —`RigidBody.cpp:41` y `SimpleRigidBody.cpp:17`— y los dos ficheros los compila **sólo**
`zPhysicsBehaviors.cpp`. La cabecera la incluyen `zPhysics` y los ficheros de AI, pero una
macro es texto hasta que se expande, así que no les cambia un byte.

### El resultado

| | antes | después |
|---|---|---|
| `fncmp` | 1 de 1.120 | **1 de 1.120** — cero regresión |
| `.data` | **+416** | **−224** |
| `.bss` | −988 | **−348** |

Los 640 B que se van son exactamente los dos arrays (384 + 256). Y la prueba **por
construcción**, que no depende de ninguna estimación:

    NUESTRO   _t10ScratchPtr1ZQ29RigidBody8Volatile.mPointer          UNDEF
              _t10ScratchPtr1ZQ215SimpleRigidBody8Volatile.mPointer   UNDEF
    ORIGINAL  (los dos)                                               UNDEF

**Ya no lo definimos, así que no podemos robarlo.** El cambio de dueño desaparece.

`trypromo` sigue dando **DOL ROTO**: quedan `.data −224`, `.bss −348`, `rodata −40` y la
función 4 B corta. No pido promoción.

---

## 3. Una inferencia mía que era falsa, para que nadie la repita

Al ver que `reorden` daba en `zTrack` el símbolo `lbl_80408FB8` con **«obj 540, nue 92»**, y
que `0x80408FB8` es **exactamente la primera dirección de la ventana `.rodata` de zTrack**
—igual que `0x803EBB48` lo es de `zGameModes`, que es donde el agente `orden` está escribiendo
un prefijo de 540 B—, escribí que «el prefijo de TU no son 92 B, son 540, y llevamos
escribiendo una sexta parte».

**Es falso.** `prefijotu.py zTrack` dice que a esa unidad le falta **una** cadena, 53 B. El
`540 contra 92` es **granularidad de nombre**: el objetivo tiene un símbolo de 540 B donde
nosotros emitimos 92 más un montón de `$LC`, que es lo mismo que delatan los «180 sin par» de
su `.rodata`.

La regla, que ya está en el catálogo y volví a saltármela: **una diferencia de TAMAÑO entre
símbolos del mismo nombre puede ser contenido o puede ser troceado distinto**, y se distingue
preguntándole a la herramienta que compara por CONTENIDO, no por nombre.

---

## 4. Un agujero de herramienta, sin arreglar a propósito

**`reorden.py` no funciona con unidades de biblioteca**: `criticalpath` contesta «sin ventanas
que comparar» tanto con el nombre pelado como con la ruta completa, aunque `resolver` sí la
encuentra y está en el enlace como `obj/`.

Es la misma familia que ya costó una ronda entera: herramientas cableadas a `SourceLists` que
no ven el middleware (`nfsmw-frente-middleware`, y el `linkdelta` que no resolvía ni una unidad
de biblioteca).

**No lo he arreglado porque siete agentes están corriendo `reorden.py` ahora mismo**, y cambiar
una herramienta bajo sus pies invalida sus medidas. Va a la ventana de después de la ronda.

---

## 5. Lo que queda de `zPhysicsBehaviors`

Con el `= {}` fuera, para promocionar le faltan:

- **`.bss −348`** y **`.data −224`** — datos que no emitimos, y ahora son el frente principal.
- **`rodata −40`**.
- **`UpdateLoaded` 4 B corta** — y esa está vetada con prueba: la cadena que produciría el
  `lis` a registro preservado necesita que `cse1` le dé al `high` un segundo uso en otro bloque
  básico, y `cse_end_of_basic_block` corta en toda `CODE_LABEL` con `LABEL_NUSES > 1`, que es
  el caso. **De 199 casos censados, el único con cero uniones es `ActualReadJoystickData`**
  (zPlatform), no éste.

Guarda 236.176 B, y ahora se sabe que el camino es de datos.
