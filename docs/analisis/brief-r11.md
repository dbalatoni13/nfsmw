# Ronda 11 — instrucciones comunes

Estado al empezar: **91,49% matched, 7,12% linked (301/590), `main.dol: OK`**,
17.856 funciones. Faltan **223.664 B** en 24 SourceLists. La ronda 10 subió
+34.880 B y +66 funciones **sin que bajara ninguna unidad**.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## Aviso sobre el playbook

Hay un agente **auditando `docs/PLAYBOOK.md`** ahora mismo, porque sabemos que
tiene entradas falsas. **Léelo igual —sigue siendo la mejor herramienta que
tenemos—, pero si una entrada te falla, mídelo y dímelo en el informe**: es
exactamente lo que estamos buscando. **No lo edites tú.**

Correcciones ya confirmadas, por si tu copia todavía las lleva mal:
- Las locales **muertas** NO reordenan el pool (sólo las que se **usan**).
- Un límite sin canonicalizar **no** viene sólo de un `switch`: también de un
  bucle con cota **variable**.
- Para decidir si un aviso de `litpos` es real, el test es el **multiconjunto**,
  no el registro destino del `lfs`.
- `ATTRIB_NO_INLINE_CLASSKEY` **no es un frente** y empeora seis unidades.

## Lo primero, antes de tocar nada

1. **`python scripts/measure.py -o antes.json <tus unidades>`**. Al terminar,
   `--cmp antes.json despues.json`. **Ese delta es tu informe.**
2. Pasa `censo.py`: te dice si falta código o si son todo near-miss. Cambia la
   estrategia por completo.

## Las herramientas, y lo que ya sabemos que mienten

| script | para qué | aviso |
|---|---|---|
| `bench.py` | el ciclo en 1,4 s | se degrada a 10+ min si cambia el tamaño de la función |
| `litpos.py` | valores del pool | **borra su caché `r4_c_<unidad>.json`**; cambiar una constante reordena el pool y canta decenas de falsos en cascada |
| `strseq.py` | cadenas | la dirección «nosotros la tenemos y el objetivo no» es **ruido**; confirma con `grep` en `src/` |
| `regmap` | locales y árbol | «SOLO NUESTRA» es una **hipótesis**; y puede casar la **sobrecarga equivocada** |
| `fndiff.py` | diff por función | **imprime el tamaño del objetivo en los dos lados**: el juez es el ELF |
| `permuter.py` | reparto de registros | rentable sólo si `regmap` dice «reparto con UN registro»; **`--sweep --timeout` no sirve** (8-85 s × 120-320 variantes: por función, sin timeout, `-j 8`); **no valida semántica en `move_stmt`** |
| `mnemo` / `auditecho` / `arboljump` / `globalini` / `symtabdiff` | triaje | |
| `fuse.py` / `plan.py` | el DWARF y el mapa de líneas fundidos con el asm | |

## Lo que más rindió en la ronda 10 — repítelo

- **`regmap` dice «rotación limpia de N posiciones» + una local con `-` en el
  original ⇒ el original acumulaba sobre la variable existente**, no creaba el
  intermedio. 1.516 B en dos funciones, las dos a la primera compilación.
- **El mapa de líneas (`fuse.py`) identifica QUÉ inline se usó, no sólo dónde**:
  si la línea del objetivo no cita la cabecera del inline que tú usas, no estás
  usando el mismo inline.
- **`symtabdiff` + static-init por debajo del 100% son el mismo bug**: un global
  **que no está definido en ningún sitio** se come su constructor.
- **`__asm__("")` es barrera de cross-jumping *y* de planificación**, cuesta cero
  bytes, y va **detrás** de la llamada (delante no sirve).
- **Los literales y las cadenas.** En la ronda 10 salieron ocho constantes mal,
  **tres de ellas en funciones que marcaban 100%**.

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada por función.** Una al 99,99% aporta **cero**.
- **El DWARF NO lista los temporales que GCC coalesce**: «esta local no está en el
  volcado» **no** autoriza a borrarla (medido: 99,27 → 92,47%).
- **Quitar un local sobrante puede DESTAPAR un cross-jump**: mira el **tamaño**,
  no sólo el porcentaje.
- **Trabajo a medias REGRESA.** Si una vía no cierra, revierte y anota la veda
  **con el porcentaje medido**.
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`.
- **No rompas los bloques `#if defined(__ANDROID__)`** de `src/types.h`,
  `bMath.hpp` y `UVectorMath.hpp` (**este último debe conservar 2**).
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen.**
  `UTLVector.h` tumbó zSim/zPhysics/zMain **dos veces** en la ronda 9.
- **El scratchpad es compartido**: pon nombre único a tus helpers.
