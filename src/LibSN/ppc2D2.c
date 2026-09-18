/* GOWE69's GCC runtime bridges, 0x80311C50..0x80311CB4.
 * The following MW __va_arg lives in Runtime.PPCEABI.H/__va_arg.c (r39).
 */

unsigned int __cvt_fp2unsigned(double value) {
    if (value >= 2147483648.0)
        return (unsigned int)(int)(value - 2147483648.0) ^ 0x80000000u;
    return (unsigned int)(int)value;
}

/* Los siete alias que SN le pone a los ayudantes de 64 bits de libgcc son
 * UNA sola instruccion cada uno: un `b` de cola, sin prologo ni epilogo. En C
 * no salen --GCC 2.9 no hace llamadas de cola: en el binario de cc1 de ProDG
 * 3.9.3 las unicas cadenas con 'sibling' o 'tail' son atributos de DWARF--,
 * pero si salen con un asm() de AMBITO DE FICHERO, que es lo que el proyecto
 * ya usa para los alias de nombres mangled. Van aqui, entre __cvt_fp2unsigned
 * y __va_arg, que es donde los tiene el objetivo (desplazamientos 72..96).
 *
 * Los destinos salen de las reubicaciones del objeto extraido, no de suponer
 * el nombre. Y el `.size` de 4 B no los expone a `-strip-unused-data`, que
 * quita `size & ~7`: para 4 B eso es cero. */
__asm__(
    "\t.align 2\n"
    "\t.globl __shr2u\n"
    "\t.type __shr2u,@function\n"
    "__shr2u:\n"
    "\tb __lshrdi3\n"
    "\t.size __shr2u,.-__shr2u\n"
    "\t.align 2\n"
    "\t.globl __div2i\n"
    "\t.type __div2i,@function\n"
    "__div2i:\n"
    "\tb __divdi3\n"
    "\t.size __div2i,.-__div2i\n"
    "\t.align 2\n"
    "\t.globl __shl2i\n"
    "\t.type __shl2i,@function\n"
    "__shl2i:\n"
    "\tb __ashldi3\n"
    "\t.size __shl2i,.-__shl2i\n"
    "\t.align 2\n"
    "\t.globl __mod2i\n"
    "\t.type __mod2i,@function\n"
    "__mod2i:\n"
    "\tb __moddi3\n"
    "\t.size __mod2i,.-__mod2i\n"
    "\t.align 2\n"
    "\t.globl __shr2i\n"
    "\t.type __shr2i,@function\n"
    "__shr2i:\n"
    "\tb __ashrdi3\n"
    "\t.size __shr2i,.-__shr2i\n"
    "\t.align 2\n"
    "\t.globl __div2u\n"
    "\t.type __div2u,@function\n"
    "__div2u:\n"
    "\tb __udivdi3\n"
    "\t.size __div2u,.-__div2u\n"
    "\t.align 2\n"
    "\t.globl __mod2u\n"
    "\t.type __mod2u,@function\n"
    "__mod2u:\n"
    "\tb __umoddi3\n"
    "\t.size __mod2u,.-__mod2u\n");
