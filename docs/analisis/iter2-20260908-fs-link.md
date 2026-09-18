# Iter2 FSasync: orden de definiciones restaurado y enlace probado

Despues del cierre de 388 B de CompletePCreadAsync, las 13 funciones eran
exactas, pero tres definiciones seguian en otro orden dentro de `.text`.

| funcion | objetivo | fuente antes del reorden |
|---|---:|---:|
| CompletePCreadAsync | +704 | +1088 |
| PCrwAsyncFSACK | +1092 | +704 |
| PCrwSyncFSACK | +1260 | +872 |

Se desplazaron las dos definiciones FSACK detras de CompletePCreadAsync y se
anadio un prototipo static de PCrwSyncFSACK. **Ese reorden si esta integrado
en FSasync.c**. Los cuerpos de las dos funciones movidas son byte-identicos
en fuente; fuera de moverlas y anadir el prototipo no cambia nada respecto
al cierre anterior. Se conservan el comentario nuevo, los tres macros y sus
undef inmediatamente despues de CompletePCreadAsync. Fuente CRLF puro.

## Prueba de enlace

`scratchpad/codex_20260908_iter2_fs_link/link.py` reconstruye la lista real
de objetos de build.ninja y realiza dos enlaces privados completos:

- control, lista generada sin modificar;
- prueba, sustituyendo exclusivamente `obj/FSasync.o` por `ordered.o`.

Ambos generan **4.541.888 B** y SHA-1
**9619ba57c9919f95f7f2ac951a2166a3517f91e3**. Los DOL son iguales byte a byte.
Las referencias de sufijo numerico que advierte promote.py resuelven en la
lista real; no se ignoran por su grafia. Ningun objeto vecino se sustituye.
Solo aparece el aviso historico de doble PPCMtdec.

La fuente real reordenada se compilo con el target exacto. Sus secciones
runtime, incluida `.text`, y **todas las secciones crudas de relocacion** son
identicas al objeto privado usado para el DOL exacto. Las trece funciones
pasan otra vez el auditor ELF; fncmp da cero pendientes.

## Gates y baselines

- `codex_20260908_iter2_fsasync/manifest.json` y sus copias anteriores al cierre
  permanecen inmutables.
- Su `production_manifest.json` identifica el estado exacto inmediatamente
  anterior al reorden. Se comprobo `audit_production.py` en ese estado.
- `codex_20260908_iter2_fs_link/production_before_order.c/.o` preservan dicho
  estado. **No ejecutar los gates historicos de proteccion contra la fuente
  reordenada esperando que pasen**: acotaban una region distinta.
- Gate vigente: `python scratchpad/codex_20260908_iter2_fs_link/audit_final.py`.
  Comprueba hashes del estado intermedio, movimiento exclusivo de las dos
  definiciones, CRLF, identidad runtime con la prueba enlazada y hashes de
  configuracion/splits/lista/linker/keep sin cambios.
- `python scripts/audit.py FSasync`: trece OK; Complete388/10ramas/58relocs.

## Promocion pendiente de autorizacion

No se ha cambiado configure.py. La entrada **activa** es:

```python
Object(NonMatching, "FSasync.c", source="LibSN/FSasync.c", cflags=[*cflags_libc, "-G0"])
```

El arbol tambien tiene una entrada de LibSN/FSasync.c que no corresponde al
objeto activo: no modificar ambas por busqueda global. La promocion propuesta
es cambiar solo NonMatching -> Matching en la entrada activa, regenerar la
configuracion y repetir gates/censo/DOL; no requiere splits. Aporta **2.104 B
linked**, no otros 388 B matched. La fuente ya aporta esos 388 desde el cierre.
Se envio pregunta acotada al usuario; hasta recibir respuesta, linked sigue
en 757.604 B / 19,199057 %.
