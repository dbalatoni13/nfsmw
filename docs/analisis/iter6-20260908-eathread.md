# Iter6 — `eathread_thread.cpp` / 84 bytes linked

> **Actualización:** el negativo de identidad descrito aquí quedó resuelto en
> una extensión privada mediante dos entradas `objeto:símbolo` en `keep.lst`.
> La prueba DOL y la receta vigente están en
> `docs/analisis/iter6-20260908-eathread-linked.md`. Este documento conserva los
> ensayos negativos que justifican esa solución.

## Resultado

Diagnóstico cerrado, **sin cambio de producción** y sin promoción. La omisión
GameCube de las tres definiciones ausentes del objetivo está demostrada, y una
sombra llega a `.text` 84 B + `.ctors` 4 B exactos con BSS cero. Sin embargo,
el enlace privado no conserva el nombre/binding local especial del wrapper
`_GLOBAL_.I.AllocateThreadDynamicData__Q22EA6Threadv`; `ngcld` elimina entonces
la entrada de ctor y sus 84 B. No se debe integrar el fragmento mientras ese
gate no reproduzca el DOL original.

La revisión independiente llegó al mismo veredicto en
`docs/analisis/iter6-20260908-eathread-review.md`.

## Evidencia de propiedad y ausencia de usos

`scratchpad/codex_20260908_iter6_eathread/graph_audit.py` recorrió los **619
objetos únicos** del edge real de enlace. Para estos tres nombres exactos:

- `AllocateThreadDynamicData__Q22EA6Threadv`;
- `_Q22EA6Thread.gThreadDynamicData`;
- `_Q22EA6Thread.gThreadDynamicDataAllocated`;

el resultado es 0 definiciones, 0 UNDEF y 0 relocaciones en todo el grafo.
Tampoco existen en el objeto objetivo ni en el ELF original. La búsqueda de
fuente sólo encuentra sus definiciones en
`source/gc/eathread_thread_gc.cpp`; no hay consumidores ni aliases. El wrapper
`eathread_thread.cpp` conserva intacta su rama PS2, de modo que la propuesta se
limita al include privado de GameCube.

El objeto fuente actual introduce exclusivamente por esas construcciones:

- función vacía `AllocateThreadDynamicData`: 4 B;
- `gThreadDynamicDataAllocated`: 128 B BSS;
- `gThreadDynamicData`: 26.624 B BSS;
- objeto local `gThreadDynamicDataInit`: 1 B, con sección alineada a 4 B.

El objetivo sólo posee dos funciones locales, 40+44 B, una entrada `.ctors` de
4 B y ninguna sección BSS asignada a la unidad.

## Propuesta fuente exacta probada

La sombra completa está en
`scratchpad/codex_20260908_iter6_eathread/source/gc/eathread_thread_gc.cpp`.
Los cambios respecto a producción son:

1. convertir el cuerpo vacío de `AllocateThreadDynamicData` en mera
   declaración;
2. omitir únicamente las dos matrices GameCube;
3. sustituir el objeto estático sintético por un helper local que construye un
   `ThreadDynamicDataInit` automático y una entrada explícita `.ctors`.

El cuarto cambio es necesario: omitir sólo las tres definiciones deja 4 B BSS
y desplaza todas las direcciones BSS posteriores. La identidad del antiguo
objeto es inobservable (no tiene referencias en fuente, grafo ni ELF); su único
efecto es ejecutar el constructor. El helper conserva exactamente ese efecto:
comprueba prioridad 65535 e inicialización distinta de cero, construye el local
y ejecuta el bucle de 32 iteraciones. El destructor implícito es trivial y ni
objetivo ni sombra emiten `.dtors`.

La forma compilable usa solamente C++ y metadatos de sección; el único asm
declarativo ensayado es un `.set` de identidad estable, sin instrucciones ni
valores fantasma. El objeto sombra resultante contiene:

- `.text`: 84 B, mismas instrucciones tras resolver el `R_PPC_REL14` local;
- funciones locales en offsets 0/40 y tamaños 40/44;
- `.ctors`: 4 B, flags `ALLOC`, `R_PPC_ADDR32` al offset 40;
- `.bss`, `.data`, `.rodata`: 0 B;
- ninguna `.dtors`.

## Pruebas de enlace y causa restante

La omisión simple (tres definiciones, objeto estático conservado) enlaza, pero
su BSS de 4 B produce SHA-1
`977db325db27cfe34cd131c7378c2ee26fc4bc4d`; cambian el tamaño BSS del header y
siete referencias posteriores. El control conserva el SHA-1 original
`9619ba57c9919f95f7f2ac951a2166a3517f91e3`.

La variante sin storage conserva en el objeto el código y `.ctors`, pero
`ngcas` canoniza el `.set` hacia `GlobalThreadDynamicDataInit`; el alias local
objetivo no aparece en la tabla de símbolos y la relocación también usa ese
nombre. En el enlace privado:

- control: DOL original, 4.541.888 B;
- trial: SHA-1 `9f18ee20ee08fefa19c9fd0177bf963df996001c`,
  4.541.824 B;
- desaparecen `.ctors` 4 B y `.text` 84 B; el delta final es 64 B por
  alineación.

Se probó una última solución causal de identidad: asm-name directo sobre la
función `static`, respetando el binding LOCAL del objetivo. ProDG genera con
`-gdwarf+` las etiquetas `.L_F*_GLOBAL_.I..._{s,e}` y `ngcas` las rechaza como
opcode inválido. La refinación `.local/.type/.set/.size` tampoco es válida:
este `ngcas` no reconoce `.local`. No se probaron flags, versiones ni ASM de
cuerpo.

Por tanto, una integración futura sólo es aceptable si mantiene simultáneamente
el fragmento semántico anterior, BSS cero, la entrada `.ctors` y un símbolo
LOCAL exacto `_GLOBAL_.I...` que sobreviva a `-strip-unused-data`. La sombra
actual no cumple el último requisito y no es una propuesta lista para aplicar.

## Gates reproducibles

- `python scratchpad/codex_20260908_iter6_eathread/graph_audit.py`
- `python scratchpad/codex_20260908_iter6_eathread/prepare_build.py`
- `python scratchpad/codex_20260908_iter6_eathread/audit_shadow.py`
- `python scripts/lcfix.py --check`

`audit_shadow.py` da PASS a la evidencia, a los 619 objetos, al inventario
84+4/BSS0 y a la inmutabilidad de producción; termina deliberadamente con
`NEGATIVE: ... no integration proposed`. Los resultados estructurados están en
`graph_audit.json`, `build_manifest.json`, `audit_result.json` y
`link_proof{,_v3}`.
