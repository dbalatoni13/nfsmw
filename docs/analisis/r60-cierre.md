# r60 — cierre

## Progreso oficial verificado

| medida | antes r60 | despues r60 |
|---|---:|---:|
| unidades completas | 521 / 616 | **522 / 616** |
| bytes de DOL distintos eliminados | — | **660.618 B** |

`main.dol` = `9619ba57c9919f95f7f2ac951a2166a3517f91e3`, verificado al cerrar.

**PROMOCIONA: zFeOverlay.** Siete encargos, uno promociona.

## Lo que la hizo posible

Un fallo de herramienta que llevaba **cinco rondas** cegando a la unidad:
`deadstr.py` recorria TODAS las secciones `SHT_RELA` para su conjunto de simbolos
"vivos", y en zFeOverlay la informacion de depuracion nombra los `$LC`. Los 370
salian vivos y la herramienta contestaba «0 cadenas muertas» sin dar error.
Acotado a las secciones ALLOC salen las **44 exactas** (544 B), que valian
**6.211 B de `.over`**.

## Reparto

| encargo | bytes | unidades |
|---|---:|---|
| world | 295.741 | zWorld 245.700 -> 29.551, zTrack 87.702 -> 8.110 |
| phys | 226.583 | zPhysics 262.256 -> 35.673 |
| eagl | 128.164 | zEagl4Anim, `.bss` identico simbolo a simbolo |
| feov | 6.211 | **promociona** |
| ecs | 2.451 | |
| resto | 1.468 | |
| plat | 0 | las tres unidades a IGUAL en las nueve secciones |

## Los negativos que valieron la ronda

- **El plan de la r59 para zFeOverlay era INAPLICABLE**, no solo caro: pedia
  meter las 44 cadenas con `asm()` de ambito de fichero, pero caen DENTRO del
  pool de una funcion, y GCC 2.9 emite el pool entero y DELANTE de su cuerpo. Un
  `asm()` de fichero solo alcanza los huecos ENTRE pools.
- **`if (0) { x = 0.25f; }` NO crea una constante MUERTA en el pool**, contra lo
  que decia el resumen de la palanca A de zMain. `output_constant_pool` solo
  emite lo que `mark_constant_pool` encuentra en la cadena de insns VIVA, y
  `jump.c` ya se llevo el bloque. La palanca REORDENA constantes que se usan.
- La cola de inlines de `finish_file` **no sale en orden de primera PETICION**
  sino de **PARSEO DE LA DEFINICION**.
- `#define BNEW new (__FILE__, __LINE__)`: el objetivo **no pasa `__FILE__`,
  pasa el NOMBRE DEL TIPO**.
