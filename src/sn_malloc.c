/* The original ProDG public malloc/free/realloc/calloc entries are diagnostic
 * panic traps, not the game's allocator hooks: they report the call-site
 * address. OSPanic halts, so every entry falls off the end without a return
 * value: the r3 that reaches the caller is the one OSPanic left.
 *
 * realloc y calloc no llegan al binario (nadie las llama), pero sus mensajes
 * si: la .rodata de la unidad son las cuatro cadenas, en este orden.
 *
 * LR se lee con `mflr`: __builtin_return_address(0) de este GCC lo carga de
 * la pila. El asm tiene que ser `__volatile__`: leer LR es un efecto de lado
 * y, sin el, GCC funde/adelanta el asm y emite `lis / addi / subi`; el
 * objetivo lleva `lis / subi / addi`. Ver docs/analisis/r65-log10.md.
 */
#include <stddef.h>

extern void OSPanic(const char *file, int line, const char *format, ...);

void *malloc(size_t size) {
    unsigned int returnAddress;

    __asm__ __volatile__("mflr %0" : "=r"(returnAddress));
    returnAddress -= 4;
    OSPanic(0, 0,
            "\n*** Library error ***\nAn external call has been made to 'malloc'\n"
            "Calling function address: 0x%X\nPlease See the ProDG manual\n",
            returnAddress);
}

void free(void *pointer) {
    unsigned int returnAddress;

    __asm__ __volatile__("mflr %0" : "=r"(returnAddress));
    returnAddress -= 4;
    OSPanic(0, 0,
            "\n*** Library error ***\nAn external call has been made to 'free'\n"
            "Calling function address: 0x%X\nPlease See the ProDG manual\n",
            returnAddress);
}

void *realloc(void *pointer, size_t size) {
    unsigned int returnAddress;

    __asm__ __volatile__("mflr %0" : "=r"(returnAddress));
    returnAddress -= 4;
    OSPanic(0, 0,
            "\n*** Library error ***\nAn external call has been made to 'realloc'\n"
            "Calling function address: 0x%X\nPlease See the ProDG manual\n",
            returnAddress);
}

void *calloc(size_t count, size_t size) {
    unsigned int returnAddress;

    __asm__ __volatile__("mflr %0" : "=r"(returnAddress));
    returnAddress -= 4;
    OSPanic(0, 0,
            "\n*** Library error ***\nAn external call has been made to 'calloc'\n"
            "Calling function address: 0x%X\nPlease See the ProDG manual\n",
            returnAddress);
}
