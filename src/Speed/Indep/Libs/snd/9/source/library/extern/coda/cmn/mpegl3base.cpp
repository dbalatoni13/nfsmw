// mpegl3base.cpp -- CMpegLayer3Base, la capa 3 del decodificador MPEG de coda.
//
// No llega al binario nada de esta unidad: el `.text` mide 0 B
// (0x803662C4..0x803662C4) y no tiene datos.
//
// La CU de DWARF1 del original si trae los diecisiete metodos de la clase
// (Dequantize, Stereo, Reorder, AntiAlias, Imdct36X1, Hybrid, ...), y los trae
// TODOS con AT_low_pc = 0xFFFFFFFF: el enlazador de EA los estripo porque
// nadie los llama --el juego no reproduce MP3 por esta ruta. Reescribirlos no
// aportaria un byte al DOL. El flotante 32767.0f de 0x80412AE8 es el literal
// de `ssdfx.c`, la unica que lo usa.
