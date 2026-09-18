# Ronda 22 — instrucciones comunes

Estado: **97,26 % matched**, 18.308 funciones, **`linked` 8,16 %** (352/589),
`main.dol: OK`.

## LEE PRIMERO `docs/HERRAMIENTAS.md`

Qué herramienta para cada caso, cuál está superada por cuál, y las trampas
medidas. Las **secciones 8 y 9** son el catálogo de mecanismos.

## Lo primero: VERIFICA TU ENCARGO

Los números de abajo los saqué hoy con `triage.py` sobre objetos recién
construidos. Aun así: **`build_direct.py` tus unidades, pásales `triage.py`, y si
no coincide, dímelo en el informe y trabaja sobre lo que midas tú.** En la ronda
20 tres agentes seguidos encontraron su encargo caducado; en la 21, con esta
regla puesta, sólo hubo una discrepancia (una lista de opcodes truncada).

## Tres cosas que la ronda 21 corrigió, y una es mía

1. **«Quita el pin lo primero» ES UNA SONDA, NO UNA LEY.** Dos contraejemplos
   medidos: en `VP6_PredictFilteredBlock` quitar los cinco pines baja 1,2 pp, y
   en `DynamicLoader::Initialize` **10,6 pp**. Úsalo como hipótesis y mídela.
   **`python scripts/pines.py`** dice exactamente qué pines están en funciones
   que no casan — de 193 en el árbol, sólo dos lo estaban.
2. **El permutador puede dejar el fuente PEOR.** En `UpdatePlatInfo` un commit
   suyo subió **+0,05 pp permutando la fuente**; restaurar el orden real hizo que
   todas las `lfs`/`fmuls` casaran fila a fila. Si tu función tiene un commit del
   permutador con ganancia por debajo de ~0,2 pp, **contrasta el orden actual con
   `lmap.py`/`dwbody.py` antes de gastar ensayos**.
3. **«Al objetivo le sobra una carga» NO es `true_dependence`.** Antes de
   invocar ese mecanismo hay que comprobar (a) que sea un global escalar en
   dirección fija y (b) que haya una escritura entre medias. Medido con volcado
   RTL en `spch`: no lo era, y el frente entero de `path` está cerrado.

## Los mecanismos nuevos que dejó la 21 — están en la sección 8, pero éstos son de esta ronda

- **`(x + 7) / 8` no es `(x + 7) >> 3`.** Mismo `srawi`, pero `expand_divmod`
  construye otra RTL y cambia el reparto **de la función entera**. Cerró 4
  funciones. No vale para cualquier `>>`.
- **La local que sobra puede necesitar DOS ramas.** Quitarla sola empeora (GCC
  rematerializa); con un `if/else` de dos ramas la carga se queda izada. 2.752 B
  en zEagl4Anim.
- **`if (X){A}else{B}` y `if (!X){B}else{A}` NO son iguales**, y cuál gana depende
  de si las dos ramas asignan el mismo valor.
- **Si al objetivo le SOBRA una guarda, la inicialización está en OTRO bloque
  básico extendido.** No es `volatile` ni una bandera: es dónde está la sentencia.
- **Un argumento decide el registro**: `result` caía en r6 porque era el 4.º
  argumento de una llamada. Sacarlo a un local propio lo libera.

## Las cuatro reglas

1. **`build_direct.py` antes de cualquier medida.** `ninja` no construye el
   middleware y `measure.py` no compila.
2. **`audit.py`: UNA pasada al empezar**, tráeme la lista y sigue. **Confirma
   todo FALLA con una segunda pasada.**
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Congélalo con
   `frozen.py cong`.
4. **Cuando una variante dé el tamaño exacto del objetivo, desconfía**: van
   **diez** casos medidos en que empeora. Y comprueba el efecto en las funciones
   vecinas, no sólo en la tuya.

## Método

- **Antes de editar**, escribe en `docs/analisis/r22-<grupo>.md`: tamaño e
  instrucciones, dirección, censo de llamantes (`bl` directas — dilo así), los
  llamados y su contrato, **y qué NO has probado**.
- **Ensayos numerados** `c1`, `c2`… **cada uno con su cifra medida**. Los
  descartados no quedan en el árbol.
- **Si no cierra, revierte** y anota la veda con **la sentencia que barriste**.
- Una corrección estructural **real** que no cierre se queda, documentada, si
  `measure.py --cmp` demuestra que ninguna unidad baja — **y dímelo si además
  baja el porcentaje**, que eso es decisión mía.

## Prohibido

- **Escribir ensamblador.** Van **cuatro** casos retirados; el último, dos
  inlines falsificados en `spchevnt.c` que daban 86,8 % y **cero bytes**.
  Renombrar un símbolo con `__asm__("nombre_real")` sí es legítimo.
- **Restricciones de registro**: último recurso, sólo vacías de entrada, y tienen
  que pasar `audit.py`.
- **`configure.py` sólo si tu encargo lo dice**, y siempre con `trypromo.py`
  antes. **No toques** `config/GOWE69/*` ni `splits.txt`.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Cabecera compartida = A/B por objetos** sobre todas las unidades que la
  incluyen. El md5 del `.o` no vale: lleva el debug.

## Convivencia

Ocho agentes sobre un árbol compartido. Compila al scratchpad, prefija tus
ficheros, y **no toques ficheros fuera de tus unidades** sin decírmelo.
