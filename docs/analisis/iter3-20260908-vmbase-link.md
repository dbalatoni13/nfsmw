# Iter3 — enlace privado de `LibSN/vmbase`

## Resultado

El control privado enlaza 616 objetos y reproduce el DOL original:

```text
SHA-1 9619ba57c9919f95f7f2ac951a2166a3517f91e3
tamano 4.541.888 B
```

El objeto fuente actual con los nombres `extern` simples no enlaza directamente,
pero una unica sombra que hace que esos `extern` nombren los aliases con
direccion ya exportados por el auto-objeto **si reproduce el DOL original**. No
es un problema del cuerpo cerrado de 380 B ni del orden de funciones. No se
modificaron fuente, objetos, configuracion, splits, `build.ninja` ni los
artefactos `build/GOWE69/main.*`.

## Prueba de ownership

El `symtab` de `orig/GOWE69/NFSMWRELEASE.ELF` coloca inmediatamente despues del
marcador `STT_FILE VMBase.c` siete simbolos `LOCAL OBJECT` consecutivos:

| direccion | tamano | simbolo |
|---:|---:|---|
| `0x804FFED8` | 4 | `g_vmBasePageTable` |
| `0x804FFEDC` | 4 | `g_vmBaseVMReversePageTable` |
| `0x804FFEE0` | 4 | `g_vmBaseLockedPageTable` |
| `0x804FFEE4` | 4 | `cbVMSwapPageIn` |
| `0x804FFEE8` | 4 | `g_baseInitialized` |
| `0x804FFEEC` | 4 | `g_originalSR7` |
| `0x804FFEF0` | 4 | `g_originalSDR1` |

Por tanto `VMBase.c` es el propietario demostrado de `.sbss
0x804FFED8..0x804FFEF4` (28 B). El split activo declara solo sus 3180 B de
`.text`; el rango de datos queda dentro de
`auto_09_804FFED8_sbss.o`, junto con 68 B de propietarios posteriores.

## Dos gates y causa precisa

### Objeto fuente actual, sin alterar

El primer enlace sustituyo unicamente `obj/LibSN/vmbase.o` por una copia
inmutable de `src/LibSN/vmbase.o`. El enlazador devuelve `L0039` para los siete
nombres anteriores: el objeto fuente los declara `extern` con sus nombres
simples, mientras el objeto automatico exporta los aliases unicos extraidos
(`g_vmBasePageTable_804FFED8`, etc.). La memoria existe; falta una identidad de
simbolo enlazable compatible.

### Unica sombra causal de datos

Se creo un split y una fuente exclusivamente privados. La fuente define los
siete objetos como `static`, en el orden de declaracion inverso que necesita
MWCC para emitirlos en el orden del ELF. El objeto resultante coincide con el
objeto objetivo de ese split en:

- 30/30 funciones y 3180/3180 B de `.text`;
- 28/28 B de `.sbss` y sus siete offsets internos;
- tamanos de todas las funciones, sin cambios vecinos.

El enlace se completa, pero produce SHA-1
`5fcd119eff3fd0258e3b4fa3b25dca11ccf2a252`, no el original. La causa es el
orden unico de objetos aplicado a secciones que en el ELF tienen ordenes
distintos:

- el codigo `vmbase` debe ir despues de las unidades CARD/SND, en
  `.text 0x803A3398`;
- sus datos deben ir antes de varios objetos de juego, en `.sbss 0x804FFED8`.

Al poner la `.text` y la `.sbss` en un solo objeto, el enlazador coloca los
siete datos en `0x804FFF18..0x804FFF34` (`+0x40`). Al retirar sus 28 B del
objeto automatico situado en la posicion de datos, el primer simbolo siguiente
(`WRoadNetwork::fValidRaceFilter`) pasa de `0x804FFEF4` a `0x804FFED4`
(`-0x20`). El DOL difiere en 184 bytes de relocacion: 60 dentro del codigo de
`vmbase` y 124 en consumidores de los datos posteriores. Cabecera y tamano del
DOL siguen iguales.

## Consecuencia para una promocion futura

La prueba adicional mas acotada conserva literalmente los 616 objetos del
control, incluido `auto_09_804FFED8_sbss.o`, y sustituye solo
`obj/LibSN/vmbase.o`. En una copia de la fuente, los siete `extern` reciben los
nombres unicos que el auto-objeto ya exporta, por ejemplo
`g_vmBasePageTable_804FFED8`; no se define, elimina ni mueve ningun dato.

Este objeto pasa simultaneamente:

- 30/30 funciones y 3180 bytes crudos de `.text` exactos;
- las 150 relocaciones de `.text` exactamente iguales en offset, tipo, addend y
  nombre de destino;
- auditoria contra el ELF original de las 30 funciones, sin overrides;
- enlace de 616 objetos con DOL SHA-1
  `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, identico byte a byte al control.

Por tanto existe una promocion de **codigo** viable manteniendo los 28 B en el
auto-objeto actual y expresando explicitamente sus aliases estables. Si se
quiere ademas devolver fisicamente esos datos a la TU que demuestra
`STT_FILE`, sigue haciendo falta una solucion central que preserve
simultaneamente:

1. el binding local y el ownership `VMBase.c` de los 28 B;
2. la posicion de `.text` de `vmbase`;
3. la posicion anterior e independiente de su `.sbss`.

Una promocion monolitica con `static` no puede satisfacer los dos ordenes con
la lista actual. Las alternativas para esa segunda etapa son soporte de orden
por seccion o un corte de objeto que conserve correctamente la resolucion de
simbolos locales. El puente de aliases ya probado no cambia el contenido ni la
direccion de los datos, pero conserva su materializacion como auto-objeto en
lugar de reconstruir todavia el binding local original dentro de `VMBase.c`.

Gate reproducible:

```text
python scratchpad/codex_20260908_iter3_vmbase_link/audit.py
python scratchpad/codex_20260908_iter3_vmbase_link/audit_alias.py
```

Artefactos: `scratchpad/codex_20260908_iter3_vmbase_link/link_results.json` y
`scratchpad/codex_20260908_iter3_vmbase_link/proof_owned/result.json` para el
diagnostico de ownership, y
`scratchpad/codex_20260908_iter3_vmbase_link/proof_alias/result.json` para el
enlace que reproduce el DOL.
