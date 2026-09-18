# r38 — pista de compilador para __va_arg, sin integracion

2026-09-07. Unidad real `ppc2D2`; fuente `src/LibSN/ppc2D2.c`.

## Resultado y limite

**0 B / 0 funciones nuevas** en este frente. No se cambio la fuente real,
su objeto de build, la configuracion, las cabeceras ni los splits. Los ensayos
compilan unicamente una copia de `__va_arg` en `scratchpad/`.

El objetivo mide 244 B. La reconstruccion vigente con ProDG 3.9.3 mide
232 B / 58,491802 %. Compilar el mismo algoritmo con MW 1.2.5n eleva la
similitud a 91,88525 %; una firma experimental con segundo parametro
`unsigned char` llega a 96,803276 %. Ninguna variante es exacta.

Esto es evidencia de compatibilidad con un backend MW antiguo, **no prueba
suficiente de que la configuracion de la unidad sea incorrecta**. La unidad
contiene tambien `__cvt_fp2unsigned` (72 B exactos con GCC) y siete puentes
de 4 B. Cambiar el compilador de la unidad completa no esta justificado.
El comentario inicial que todavia menciona FSasync es historico: actualmente
FSasync tiene una unidad aparte.

## Hipotesis causal

Con GCC divergian varias decisiones a la vez: copia de list a r10,
comparaciones con signo, carga del contador con actualizacion, plegado de
la alineacion a `neg` y retorno condicional. MW conserva list en r3 y
reproduce gran parte de las comparaciones y de la secuencia de alineacion
del original. Por eso se probo la identidad de compilador antes de ampliar
otro barrido de formas GCC.

La cabecera local `include/Runtime.PPCEABI.H/__va_arg.h` declara una
va_list de 12 B y `__va_arg(..., int)`; tambien documenta el builtin MW.
La firma experimental con `unsigned char` difiere de esa declaracion y
necesita una comprobacion ABI antes de cualquier propuesta de integracion.
No basta con que el objetivo enmascare el byte bajo del argumento.

## Ensayos aislados

| ensayo | bytes fuente / fuzzy | resultado |
|---|---:|---|
| M1: algoritmo actual, parametro int, MW 1.2.5n | 236 / 91,88525 % | cambia la huella estructural hacia el original |
| M2: local type unsigned char | 236 / 91,88525 % | neutro |
| M3: parametro type unsigned char, sin raw_type | 240 / 96,803276 % | mejor similitud, falta una instruccion |
| M4: offset y mascara con nombre, alineacion separada | 240 / 96,22951 % | fusiona NOR + AND en ANDC |
| M5: usar result para offset/mascara temporal | 240 / 87,95082 % | empeora vivacidad y reparto |
| M6: conmutar base/offset y la expresion de alineacion de M3 | 244 / 95,65574 % | tamano exacto, aun no codigo exacto |
| M7: calcular next_index y rounded antes de usarlos sobre M6 | 244 / 95,65574 % | identico a M6 |

Una matriz acotada de versiones, todas sobre M3 y con los mismos flags,
da MW 1.0 y 1.2.5: 240 B / 96,803276 %; MW 2.7: 240 B / 82,78689 %;
MW 3.0a5: 236 B / 82,85246 %. No se hizo un barrido de flags.

En M6/M7 quedan la asociacion de sumas al registro salvado, la secuencia
`lwz/add/subi` del overflow y el resultado en r6 frente a r5. Obtener
244 B no resuelve esas diferencias; no se contabiliza como cierre.

## Reproduccion y siguiente paso

Artefactos con prefijo `scratchpad/codex_r38_va_arg_`:

- `mw.json`, `mw_m2.json` ... `mw_m7.json`: informes de cada ensayo.
- `mw_1.0.json`, `mw_1.2.5.json`, `mw_2.7.json`, `mw_3.0a5.json` y sus objetos:
  matriz sobre M3.
- `m3.c`: fuente M3 congelada para reproducir el mejor fuzzy.
- `probe.c`: ultima fuente M7, no la fuente de la matriz historica.
- `matrix.py`: helper de matriz; usa `probe.c`, por lo que una nueva ejecucion
  mediria M7 y sobrescribiria aquellos informes. Cambiar fuente/prefijo antes
  de reutilizarlo.

Compilador de M1--M7: `build/compilers/GC/1.2.5n/mwcceppc.exe`, flags
`-nodefaults -proc gekko -align powerpc -enum int -fp hardware
-Cpp_exceptions off -O4,p -inline auto -RTTI off -fp_contract off
-str reuse -char unsigned -lang=c`. En PowerShell citar `'-O4,p'`.
Comparar el objeto aislado contra `build/GOWE69/obj/ppc2D2.o`, simbolo
`__va_arg`, con functionRelocDiffs=none y calculatePoolRelocations=false.

La siguiente prueba util debe resolver procedencia/ABI de este cuerpo y
la asociacion de sus ultimas sumas con C fiel. Si llega al 100 %, proponer
por separado el cambio minimo de configuracion/split y verificar el DOL
aislado. No cambiar todo ppc2D2 a MW, ni tocar las ocho funciones ya exactas.
