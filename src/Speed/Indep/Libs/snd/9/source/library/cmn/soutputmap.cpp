// soutputmap.cpp -- mapChannelToOutput(), el reparto de canal a salida fisica.
//
// Igual que sformat.c: la unidad no aporta un byte al binario.
//
//   * `splits.txt`: .text 0x8036D7B4..0x8036D7B4 (0 B) y ninguna otra seccion.
//   * La CU de DWARF1 trae `mapChannelToOutput` con AT_low_pc = 0xFFFFFFFF:
//     estripada por el enlazador original, nadie la llama.
