#include <cstddef>
#include <string.h>

namespace Snd {

void *(*CODANew)(unsigned int size);
void (*CODADelete)(void *pmem);

namespace Coda {

// El tercer parametro va como size_t y no como unsigned int: en GameCube son el
// mismo tipo --y el codigo sale identico-- pero en LP64 memcpy toma unsigned long
// y la inicializacion no compila.
void *(*gpMemCpy)(void *pdst, void const *psrc, size_t size) = memcpy;

} // namespace Coda

void CODASetNew(void *(*pfn)(unsigned int size)) {
    CODANew = pfn;
}

void CODASetDelete(void (*pfn)(void *pmem)) {
    CODADelete = pfn;
}

} // namespace Snd
