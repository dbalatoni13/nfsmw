// Este fichero DEFINE los operadores globales, que GCC 2.9 nombra __builtin_new,
// __builtin_vec_new, __builtin_delete y __builtin_vec_delete. En el ELF original
// estan consecutivos y en este mismo orden (0x802085EC, 0x80208610, 0x80208634,
// 0x80208654) con estos cuerpos: bMalloc(size, 0) y bFree(ptr).
//
// bWare.hpp declara ademas un `operator new` INLINE que reenvia al de array; es
// lo que hace que cada `new` del juego emita __builtin_vec_new. En el original
// eso vive en otra unidad de traduccion, pero aqui el build es unitario y las
// dos definiciones chocarian, asi que la guarda se pone al principio de la
// SourceList (zMisc.cpp), antes del primer include.
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstddef>

// El original NO tiene un `operator new` global: en el ELF no hay ni un
// __nw__FUi, y todos los __nw__ son de clase. Lo que si tiene son los cuatro
// envoltorios del compilador __builtin_new / __builtin_vec_new /
// __builtin_delete / __builtin_vec_delete, CONSECUTIVOS y en este mismo orden
// (0x802085EC, 0x80208610, 0x80208634, 0x80208654), con estos mismos cuerpos:
// bMalloc(size, 0) y bFree(ptr). Este fichero era el correcto; solo tenia los
// nombres equivocados.

void *operator new(size_t size) {
#ifdef MILESTONE_BUILD
    // TODO
    return bWareMalloc(size, nullptr, 0, 0);
#else
    return bMalloc(size, 0);
#endif
}

void *operator new[](size_t size) {
#ifdef MILESTONE_BUILD
    // TODO
    return bWareMalloc(size, nullptr, 0, 0);
#else
    return bMalloc(size, 0);
#endif
}

#ifdef MILESTONE_BUILD
void *operator new(size_t size, const char *file, int line) {
    return bMalloc(size, file, line, 0);
}

void *operator new[](size_t size, const char *file, int line) {
    return bMalloc(size, file, line, 0);
}
#endif

void operator delete(void *ptr) {
    bFree(ptr);
}

void operator delete[](void *ptr) {
    bFree(ptr);
}
