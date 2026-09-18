// author.cpp -- el sello de quien construyo la libreria de sonido.
//
// La unidad emite 64 B de `.data` en 0x804513A4..0x804513E4 y nada mas
// (.text 0x8036C90C..0x8036C90C, 0 B). Los 64 B son la cadena de 61 B mas tres
// de relleno hasta el multiplo de 4.
//
// La CU de DWARF1 del ELF original tiene UN solo hijo con dato:
//   global_variable `sndlibauthor`, tipo array [0..60] de FT_char (61 B),
//   AT_location = 0x804513A4. Ni una funcion.
//
// `ssysinit.c` lo declara `extern char sndlibauthor;` y le escribe la primera
// letra en tiempo de ejecucion, asi que el simbolo esta referenciado y el
// enlazador no se lo lleva.
char sndlibauthor[] = "SNDAUTHOR:  Adamchan, Tuesday 02:10PM Feb 08, 2005, V9.06.00";
