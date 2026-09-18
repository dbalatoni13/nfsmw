namespace Snd {
namespace Coda {

extern void *(*gpMemCpy)(void *pdst, void const *psrc, unsigned int size);

// El DWARF del original da la firma exacta: `static enum Result SetMemCpy(void
// *(*)(void *, const void *, unsigned int))`, con un Result de este ambito
// reducido al unico valor que se usa. Nosotros la declarabamos devolviendo
// `int` aqui y `void` en smixer.c: la MISMA clase, dos firmas que no casaban.
enum Result {
    RESULT_OK = 0,
};

struct System {
    static Result SetMemCpy(void *(*pMemCpy)(void *, void const *, unsigned int));
};

Result System::SetMemCpy(void *(*pMemCpy)(void *, void const *, unsigned int)) {
    gpMemCpy = pMemCpy;
    return RESULT_OK;
}

} // namespace Coda
} // namespace Snd
