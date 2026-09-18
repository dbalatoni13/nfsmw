// port_snd_hal.cpp -- la capa de plataforma que la libreria snd de EA espera.
//
// En GameCube esto lo daba `library/gc/snddrv.c` (el driver del DSP) y no se
// porta: el mezclador del juego ya hace la mezcla en CPU, asi que el hardware
// solo servia para reproducir voces. Aqui esas funciones son no-ops sensatas y
// el sonido sale por `MIX_audioslice` -> AAudio.
//
// Ademas hay DOS HUECOS DEL PROPIO DECOMP que no son culpa del port y que aqui
// se rellenan para poder enlazar:
//   Snd::CEAXABLKDecf   el decodificador de bloques XA. La clase esta declarada
//                       en supxaf/supxalf/supxapf pero su unidad (eaxadecf.cpp)
//                       no existe todavia en src/.
//   Snd::Util::SetDefaultAzimuths   el propio arbol lo dice en spktplay.c:
//                       "vive en cmn/sdownmix.c y no esta".
// Mientras no se decompilen, una muestra comprimida en XA saldra en silencio;
// el PCM y la mezcla funcionan igual.

#include <stddef.h>
#include <string.h>

enum StartMode { COLD = 0, WARM = 1 };
struct SNDSYSOPTS;
struct CHANPUB;
struct SNDSAMPLEFORMAT;
struct SNDSAMPLEATTR;

namespace Snd {
enum OutputMode { OM_DUMMY = 0 };

struct XAFSTATE {
    float sample1;
    float sample2;
};

struct CEAXABLKDecf {
    static void *operator new(size_t size);
    static void operator delete(void *ptr);
    CEAXABLKDecf();
    ~CEAXABLKDecf() {}
    int Feed(void *pSampleData, int sampleDataSize, int numSamples);
    int Decode(float **pDstBuf, int numSamples);
    XAFSTATE GetState();
    void SetState(XAFSTATE *pstate);
    char pad0[0xA8];
};
} // namespace Snd

// --- Driver de voces: en GameCube era el DSP -------------------------------
int SNDPLATFORM_init(StartMode) { return 0; }
int SNDPLATFORM_outputcaps() { return 2; }  // estereo
int SNDPLATFORM_outputset() { return 0; }
int SNDPLATFORM_restore(StartMode) { return 0; }
int SNDPLATFORM_stop(int) { return 0; }
int SNDPLATFORM_packetoverhead() { return 0; }
int SNDPLATFORM_packetplaycreate(int, void *) { return 0; }
int SNDPLATFORM_ModeSetUp(SNDSYSOPTS *, Snd::OutputMode) { return 0; }

// El mezclador software usa el rango entero de voces que le pidamos.
void SNDPLATFORM_getvoicerange(int, int *minvoicerange, int *maxvoicerange) {
    if (minvoicerange) *minvoicerange = 0;
    if (maxvoicerange) *maxvoicerange = 63;
}

int SNDPLATFORM_packetplay(int, int, int, int, int, SNDSAMPLEFORMAT *, SNDSAMPLEATTR *) { return 0; }
int SNDPLATFORM_packetplaydestroy(int) { return 0; }

// snd protege sus estructuras con una seccion critica propia. El render corre
// hoy en un solo hilo (el de AAudio); cuando entre el hilo de juego habra que
// darles un mutex de verdad.
void SNDI_mutexalloc() {}
void SNDI_mutexfree() {}

int SNDDRV_getmastervoice(int voice) { return voice; }
int SNDDRV_getsamplechan(int) { return 0; }

// La libreria lo usa como marca de version; en el original es un char en .data.
char sndlibauthor = 0;

// --- Huecos del decomp ------------------------------------------------------
namespace Snd {
void *CEAXABLKDecf::operator new(size_t size) { return ::operator new(size); }
void CEAXABLKDecf::operator delete(void *ptr) { ::operator delete(ptr); }
CEAXABLKDecf::CEAXABLKDecf() { memset(pad0, 0, sizeof(pad0)); }
int CEAXABLKDecf::Feed(void *, int, int) { return 0; }
int CEAXABLKDecf::Decode(float **, int) { return 0; }
XAFSTATE CEAXABLKDecf::GetState() { XAFSTATE s; s.sample1 = 0.0f; s.sample2 = 0.0f; return s; }
void CEAXABLKDecf::SetState(XAFSTATE *) {}
} // namespace Snd

// Los dos que el arbol referencia por su nombre mangled de GCC 2.9 con
// __asm__(): hay que definirlos con ESE nombre, no con el de Itanium.
extern "C" {
void SetDefaultAzimuths__Q23Snd4UtilP7CHANPUB(CHANPUB *) {}
void *MemCpy__Q23Snd4UtilPvPCvUi(void *pdst, const void *psrc, unsigned int n) {
    return memcpy(pdst, psrc, n);
}
}
