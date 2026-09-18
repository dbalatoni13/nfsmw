// sformat.c -- SNDMEMI_printf() y SNDMEMI_format(), el printf interno de la
// libreria de sonido.
//
// La unidad NO aporta un solo byte al binario final. Medido, no supuesto:
//
//   * `splits.txt` le da  .text 0x8036D268..0x8036D268  (0 B) y ninguna otra
//     seccion; el objeto extraido (build/GOWE69/obj/.../sformat.o) tiene el
//     `.text` a tamano 0 y su tabla de simbolos son el STT_FILE y el simbolo de
//     seccion, nada mas.
//   * La CU de DWARF1 del ELF original si trae las dos funciones, y las trae
//     con AT_low_pc = 0xFFFFFFFF: el enlazador de EA las estripo porque nadie
//     las llama. AT_low_pc == AT_high_pc == 0x8036D268 en la propia CU.
//
// O sea que el codigo existia en el fuente de EA pero no llego al DOL. Escribir
// aqui una reimplementacion de SNDMEMI_printf no acercaria el binario ni un
// byte, y con `-strip-unused-data` (que estripa dato, no codigo) meteria en
// `.text` lo que el original no tiene. La unidad de traduccion correcta es la
// que no emite nada.
