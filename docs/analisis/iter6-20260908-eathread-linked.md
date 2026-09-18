# Iter6 — prueba linked de `eathread_thread` superada

## Actualización: integrada y verificada

Tras esta promoción se integró SetupNextLoad. El gate combinado vigente es
`scratchpad/codex_20260908_iter6_setupload_integration/audit_final.py`; los
gates POST eathread citados abajo conservaron el punto intermedio y ahora
son históricos respecto a esas fuentes y métricas de audio.

Coordinación aplicó la receta exacta y recompiló únicamente el objeto real
`eathread_thread.o`. `configure.py -v GOWE69` regeneró los manifiestos sin
Ninja. El enlace real vuelve a producir el DOL original byte por byte.

Resultado oficial: **+84 B de código linked, +4 B de datos linked, +1 unidad
completa**. Linked: 771180 -> 771264 B (19,545225 %), datos linked:
355633 -> 355637 B; 492/619 unidades completas. Matched sigue en
3902288/3946048 B y 18387/18432 funciones. No se cambió ningún split,
cabecera ni flag de compilación.

Gate POST vigente, revisado y ejecutado por coordinación y por un agente
independiente:

```text
python scratchpad/codex_20260908_iter6_eathread_integration/audit_final.py
```

Comprueba fuente idéntica a la receta tras normalizar EOL, los dos cambios
exactos de configure (Matching y mapping), exactamente dos reglas keep, 29
rutas protegidas, 534 especificaciones de compilación intactas y los hashes
de los 619 objetos anteriores. El grafo cambia únicamente la selección de
`obj/eathread_thread.o` a `src/eathread_thread.o`; los otros 618 inputs son
idénticos. El objeto target anterior también se conserva intacto en disco.
Configure mantiene LF; fuente y keep mantienen CRLF.

Los 84 B se reconstruyen resolviendo ambas reubicaciones fuente: REL14 en
+0x20 y REL24 en +0x40. El target extraído también conserva su REL24 sin
resolver; se resuelve antes de comparar, no se coteja fuente resuelta contra
target crudo. Los resultados son `42000000` y `4bffffc1`, iguales en el ELF
original y el enlace real. Se verifica asimismo la entrada ctor original.
Helper y wrapper siguen LOCAL; el símbolo auxiliar de la entrada ctor es
GLOBAL, sin ampliar almacenamiento ni alterar el DOL.

El reporte conserva todos los registros de funciones y las otras 618
unidades completas; censo directo y 15493 entradas de las 33 SourceLists
son idénticos. Baselines y snapshots finales están en
`scratchpad/codex_20260908_iter6_eathread_integration/`.

Las secciones siguientes documentan la prueba PRE y su receta, ya integrada.
Sus gates de inmutabilidad son históricos: no volver a ejecutar generadores
como `prepare_build.py` ni sobrescribir baselines para simular PASS tras
la integración. Usar el gate POST anterior.

## Veredicto

La promoción tiene ahora una receta privada completa y acotada. Con el objeto
sombra de 84 B + `.ctors` 4 B/BSS0 y dos entradas `keep` calificadas por objeto,
el control y el trial enlazan un DOL byte por byte idéntico al original:

`9619ba57c9919f95f7f2ac951a2166a3517f91e3`, 4.541.888 B.

La configuración, fuentes, objetos y DOL de producción siguen intactos. Root
puede integrar la receta tras revisión independiente; este trabajo no la
aplicó.

## Única hipótesis de retención

El primer ensayo retuvo solamente el wrapper:

```text
eathread_thread.o:GlobalThreadDynamicDataInit
```

Eso colocó los 84 B en sus direcciones originales, pero `ngcld` eliminó la
entrada `.ctors`, que es un símbolo de datos independiente. Quedaron exactamente
cinco bytes distintos: el tamaño de `.ctors` del header DOL (`0x40 -> 0x3c`) y
la palabra `0x803A3080 -> 0` en `0x803C8C1C`.

Ese resultado justificó, sin barrido, retener el par indivisible wrapper +
entrada:

```text
eathread_thread.o:GlobalThreadDynamicDataInit
eathread_thread.o:gThreadDynamicDataConstructor
```

`scratchpad/codex_20260908_iter6_eathread/keep_link_proof_v2/result.json`
demuestra con los mismos 619 objetos:

- control: enlace rc 0, DOL original;
- trial: enlace rc 0, DOL original;
- helper local: `0x803A3058`, 40 B;
- wrapper local: `0x803A3080`, 44 B;
- entrada `.ctors`: `0x803C8C1C`, 4 B;
- todos los artefactos de producción y los 619 objetos conservan sus SHA-256.

La identidad nominal distinta del wrapper no altera el runtime: binding,
dirección, tamaño, código, relocaciones resueltas y entrada ctor coinciden, y el
DOL entero es el juez final. El `.set` de la sombra se canoniza y no aporta un
alias; se conserva en la receta exacta probada para no cambiar el objeto antes
de integración.

## Mapping de medición

Sin un mapping, objdiff no asociaría el wrapper objetivo con el nombre local de
la fuente y se perderían 44 B de métrica aunque el DOL sea exacto. El proyecto
privado `mapping_project/objdiff.json` probó esta dirección exacta:

```python
"main/Packages/eathread/1.1.0/source/eathread_thread": {
    "_GLOBAL_.I.AllocateThreadDynamicData__Q22EA6Threadv":
        "GlobalThreadDynamicDataInit",
},
```

Objdiff 3.8, usando el proyecto y las opciones oficiales, informa:

- `__static_initialization_and_destruction_0`: 40 B, 100%;
- `_GLOBAL_.I.AllocateThreadDynamicData__Q22EA6Threadv` ->
  `GlobalThreadDynamicDataInit`: 44 B, 100%.

Así se preservan los 84 B / 2 funciones ya exactos en matched al cambiar el
objeto enlazado. La promoción aporta 84 B de código linked; el delta oficial de
datos debe medirse después de integrar, sin precontar aquí la entrada `.ctors`.

## Receta exacta propuesta, no aplicada

1. Reemplazar únicamente el contenido GameCube de
   `src/Packages/eathread/1.1.0/source/gc/eathread_thread_gc.cpp` por el fragmento
   ya compilado en
   `scratchpad/codex_20260908_iter6_eathread/source/gc/eathread_thread_gc.cpp`.
   El wrapper y el soporte PS2 permanecen intactos.
2. Cambiar sólo el `Object` de
   `Packages/eathread/1.1.0/source/eathread_thread.cpp` de `NonMatching` a
   `Matching`.
3. Añadir exactamente las dos líneas `keep` anteriores.
4. Añadir el mapping target->source anterior a
   `OBJDIFF_SYMBOL_MAPPINGS` en `configure.py`.
5. No cambiar splits, flags, headers, toolchain ni nombres de unidad.

El fundamento semántico del cuarto cambio de fuente se conserva en
`iter6-20260908-eathread.md`: las tres definiciones ausentes no tienen DEF,
UNDEF ni referencias en los 619 objetos; el objeto local eliminado no posee
identidad observable, su destructor es trivial y su único efecto —el bucle de
32 vueltas del constructor— permanece en los 84 B exactos. La sombra no emite
BSS ni `.dtors`.

## Gates históricos de la prueba privada

Desde la raíz:

```powershell
# Ejecutados antes de integrar; ya no son gates del estado de producción.
# graph_audit.py / audit_shadow.py / audit_keep_proof.py
```

El gate final valida el delta exacto de `keep.lst`, DOL control/trial/original,
direcciones enlazadas, mapping 84 B/2 funciones, hashes de producción y los 619
objetos completos. Resultado estructurado:
`scratchpad/codex_20260908_iter6_eathread/audit_keep_result.json`.
