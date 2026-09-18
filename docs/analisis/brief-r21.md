# Ronda 21 — instrucciones comunes

Estado: **97,07 % matched**, 18.296 funciones, `linked` 7,92 %, `main.dol: OK`.

## LEE PRIMERO `docs/HERRAMIENTAS.md`

Está todo ahí: qué herramienta para cada caso, cómo se invoca, cuál está superada
por cuál, y las trampas medidas. Las **secciones 8 y 9 son nuevas** y son el
material de esta ronda: los siete mecanismos que abrieron la 20 y la herramienta
que descubre cada uno.

## Lo primero: VERIFICA TU ENCARGO

**Tres agentes seguidos de la ronda 20 encontraron su brief caducado** — con
funciones que ya estaban al 100 % al empezar. Los números de abajo los he sacado
hoy con `triage.py` sobre los objetos recién construidos, pero el árbol es
compartido.

**Antes de tocar nada**: `build_direct.py` tus unidades y pásales `triage.py`. Si
no coincide con tu encargo, **dímelo en el informe** y trabaja sobre lo que
midas tú, no sobre lo que diga este papel.

## Lo que cambia en esta ronda, y por qué

La ronda 19 dijo «asigna near-miss sólo con un mecanismo concreto». Eso sigue en
pie, pero **el frente barato se ha agotado**: `censo.py` da **1.052 B sin escribir
en todo el árbol**, y ~920 son los `ssb*` de `snd`. No queda otra cosa que
near-miss.

Lo que ha cambiado es que **ahora sabemos abrirlos**. Los cinco agentes de la 20
cerraron 8 funciones y 3.656 B y **ninguno** salió de probar variantes a ciegas:
salieron de identificar **qué pase del compilador decide**. Por eso cada encargo
lleva la firma exacta de `triage.py` de cada función, y el catálogo de mecanismos
está en la sección 8 del documento de herramientas.

**Leer la firma de `triage.py` es el trabajo.** `faltan 4, sobran 4, de ellas 4
SUST, blt-4 b+4` no son ocho diferencias: es **una** decisión de sentido de
comparación repetida cuatro veces. `sobran 1, mr-1` es un temporal que el
original no materializa. `faltan 3, mr+3` es al revés.

## Las cuatro reglas de esta ronda

1. **`build_direct.py` antes de cualquier medida.** `ninja` no construye el
   middleware y `measure.py` no compila.
2. **`audit.py`: UNA pasada al empezar**, tráeme la lista y sigue. Encuentra bugs
   reales (esta ronda: cuatro globales cruzados en `PATHI_init`) pero **cuesta
   0 bytes**. Y **confirma todo FALLA con una segunda pasada**: el `build/` es
   compartido y da fantasmas.
3. **Un cierre no está cerrado hasta que pasa `audit.py`.** Congélalo después con
   `frozen.py cong`.
4. **Si una función lleva un `register X asm("rN")` nuestro y no casa, quita el
   pin lo primero.** Segundo caso medido de que el pin es **la causa**; en
   `CarRender.cpp` la barrera hacía falta y el registro duro sobraba.

## Método

- **Antes de editar**, escribe en `docs/analisis/r21-<grupo>.md`: tamaño e
  instrucciones, dirección, censo de llamantes (`bl` directas — dilo así), los
  llamados y su contrato, **y qué NO has probado**.
- **Ensayos numerados** `c1`, `c2`… **cada uno con su cifra medida**. Los
  descartados no quedan en el árbol.
- **Si no cierra, revierte** y anota la veda diciendo **qué sentencia barriste**.
- Una corrección estructural **real** que no cierre se queda, documentada, si
  `measure.py --cmp` demuestra que **ninguna unidad baja**.

## Lo que está prohibido, y por qué

- **Escribir ensamblador.** No cierra una función, la falsifica. Se han retirado
  tres casos, uno «al 100 %» con un salto a mano que costó 144 B falsos.
- **Restricciones de registro**: último recurso, tras documentar la búsqueda de
  la forma, sólo vacías de entrada, y **tienen que pasar `audit.py`**.
- **`configure.py` sólo si tu encargo lo dice**, y **siempre con `trypromo.py`
  antes**: promocionar rompe el DOL aunque la unidad esté al 100 %.
- **No toques** `config/GOWE69/*` ni `splits.txt`.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**dos bloques**).
- **Cabecera compartida = A/B por objetos** sobre todas las unidades que la
  incluyen. Y **el md5 del `.o` no vale** para eso: lleva el debug.

## Convivencia

Ocho agentes sobre un árbol compartido. **Compila al scratchpad**, prefijo propio
en tus ficheros y tus `.json`, y **no toques ficheros fuera de tus unidades** sin
decírmelo.
