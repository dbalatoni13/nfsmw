# Ronda 12 — instrucciones comunes

*(La ronda 12 se interrumpió con los ocho agentes en vuelo; se rescataron
+6.432 B de lo que habían dejado escrito y el DOL sigue OK. Se relanza desde
ahí, con líneas base nuevas `base_r12b_*.json`.)*

Estado: **92,21% matched, `main.dol: OK`**, 17.888 funciones. **410 unidades
cerradas al 100%, 110 abiertas.** Faltan **208.076 B** en 24 SourceLists y
**105.788 B** en 86 unidades de fuera.

## El reparto de trabajo

**Tú analizas y escribes; yo mido, verifico y commiteo.** No hagas `git commit`
ni `git add`. Deja el árbol compilando y con todo medido.

## El playbook está AUDITADO — y lo que encontró te ahorra rondas

`docs/PLAYBOOK.md` (835 patrones) pasó una auditoría completa. Salieron **5 reglas
falsas**, **12 vedas rotas** (17 funciones que ya casaban al 100%, 38.128 B) y
**8 contradicciones**. Todo está corregido **dentro de la propia entrada**.

**La lección de las vedas rotas, que es la que más vale:** casi todas fallaban
porque **se habían barrido N formas de LA SENTENCIA EQUIVOCADA**. No es que la
función no cediera; es que se atacó el sitio que no era.

Por eso, **cuando escribas una veda, di qué sentencia barriste, no sólo cuántas
formas**. Y si una entrada del playbook te falla, **mídelo y dímelo**: en la ronda
11 los agentes desmintieron seis, y eso vale más que los bytes.

## Lo primero, antes de tocar nada

1. **`python scripts/measure.py -o antes.json <tus unidades>`**. Al terminar,
   `--cmp antes.json despues.json`. **Ese delta es tu informe.**
2. **`censo.py`**: te dice si falta código o si son todo near-miss.
3. **`dwbody.py`** (nuevo): compara el **árbol de expansiones inline del DWARF**,
   que es lo que `regmap` no mira. En una pasada sobre 32 near-miss destapó
   estructura en **13**.

## Las herramientas, y lo que ya sabemos que mienten

| script | aviso medido |
|---|---|
| `regmap` | **analiza la sobrecarga equivocada** (coge `same[0]`): comprueba cuántas hay antes de creerte su tabla. Y «SOLO NUESTRA» es **1 mejora contra 8 empeoramientos** |
| `litpos` | tiene caché (`r4_c_*.json`); **no ve el ORDEN del pool** y canta cientos de falsos en cascada si una constante cambia de sitio |
| `strseq` | la dirección «nosotros la tenemos y el objetivo no» es **ruido** |
| `fndiff` | **imprime el tamaño del objetivo en los dos lados**: el juez es el ELF |
| `fuse.py` | el oráculo «qué inline se usó» da **falso positivo con inlines anidados** |
| `permuter` | pásale **`--file` y `--symbol`** (coge la función equivocada si el nombre está duplicado); `--sweep --timeout` **no sirve**; **no valida semántica** en `move_stmt`; y **remide su ganador con objdiff** |
| `bench.py` | se degrada a 10+ min si cambia el tamaño de la función |
| `report.json` | **no trae `functions` en 70 de 590 unidades**: un barrido por función ahí devuelve **vacío, no error** |

**No solapes `build_direct.py` con el permutador**: reporta `FAILED` por
contención en unidades que compilan.

## Los patrones que más han pagado últimamente

- **La hermana que ya casa al 100% es el manual de estilo del original.** Antes de
  pelear con el asignador, mira si hay una en el mismo fichero.
- **`fuse.py` delata las asignaciones del cuerpo que en el original son la LISTA
  DE INICIALIZACIÓN**: si los `stw` salen en la línea de la declaración del ctor,
  van a la mem-init. Disolvió una permutación de 4 registros a la primera.
- **`regmap` dice «rotación limpia de N posiciones» + una local con `-` en el
  original ⇒ el original acumulaba sobre la variable existente**, no creaba el
  intermedio.
- **Un `bgt` detrás de un `cmpwi` en una cadena de igualdades = era un `switch`.**
- **`__asm__("")` es barrera de cross-jumping *y* de planificación** (va **detrás**
  de la llamada); **`__asm__("" : "+f"(x))` además es barrera de CSE** — sirve
  para **diagnosticar** qué CSE es el obstáculo.
- **La frontera de `floor_log2(n_refs)` (8, 16, 32) es un escalón real** en el
  reparto: una reasignación muerta cuesta 2 refs y puede tirar una local de 16 a
  14 y hacerle perder el registro.

## Reglas que ya han costado dinero

- **`matched_code` es todo-o-nada por función.** Una al 99,99% aporta **cero**.
- **El DWARF dice lo que el original tenía, no lo que puedes borrar.** Ni las
  locales que no lista, ni los inlines que no expande, autorizan a quitar nada
  (medido tres veces: 99,27→92,47%, 98,63→96,29%, 98,47→90,31%).
- **Quitar un local sobrante puede DESTAPAR un cross-jump**: mira el **tamaño**.
- **Trabajo a medias REGRESA.** Si no cierra, revierte y anota la veda con cifra.
- **No toques** `configure.py`, `config/GOWE69/*` ni `splits.txt`.
- **No rompas los `#if defined(__ANDROID__)`** de `src/types.h`, `bMath.hpp` y
  `UVectorMath.hpp` (**este último debe conservar 2**).
- **Cabecera compartida = mídela en TODAS las unidades que la incluyen**, y si
  puedes, con **cabecera sombra en el scratchpad + `-I` delante**, sin tocar el
  árbol.
- **El scratchpad es compartido**: pon nombre único a tus helpers.
