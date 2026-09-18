// mpeghufftables.cpp -- las tablas de Huffman del decodificador MPEG de coda.
//
// La unidad no aporta un byte al binario. A diferencia de sformat.cpp y
// soutputmap.cpp, aqui la CU de DWARF1 del ELF original **no declara ni una
// sola definicion**: sus hijos directos son tipos (`CMpegHuffTables`,
// `CXboxADPCMDec*`, ...) y declaraciones de funciones miembro traidas por las
// cabeceras, ninguna con AT_low_pc ni AT_location. Las tablas eran estaticas y
// el enlazador se las llevo enteras.
//
//   * `splits.txt`: .text 0x803662C4..0x803662C4 (0 B) y ninguna otra seccion.
