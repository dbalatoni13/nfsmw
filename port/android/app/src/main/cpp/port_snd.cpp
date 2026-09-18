// port_snd.cpp -- engancha el MEZCLADOR REAL de EA (src/Speed/Indep/Libs/snd/9)
// al backend de audio del port.
//
// El mezclador del juego es software: `MIX_audioslice` rellena un buffer de
// shorts POR CANAL y no toca hardware. Eso es justo lo que AAudio necesita, asi
// que aqui no se emula nada -- se llama al codigo decompilado tal cual.
//
// Secuencia de arranque, sacada de leer smixer.c:
//   SNDMEMI_init(heap, size)   el gestor de memoria de snd sobre un heap nuestro
//   MIX_create(&cfg)           monta buses, buffers y la cadena de filtros de
//                              salida (ft24_32), o sea que no hace falta el HAL
//                              SNDPLATFORM_* solo para rendir
//   MIX_audioslice(out, n)     por callback

#include "port_snd.h"

#include <android/log.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "nfsmw-snd", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "nfsmw-snd", __VA_ARGS__)

// MIXCONFIG y el resto del reparto de sndmix vienen de smixport.h, que es la
// UNICA definicion valida en 64 bits. Aqui habia una replica a mano y era la
// cuarta vista divergente del mismo objeto: exactamente el fallo que este
// commit arregla, cometido tambien por el port.
#include "smixport.h"

// La libreria los exporta SIN decorar (sndo.h va en extern "C"), asi que hay
// que declararlos igual o el enlazador no los encuentra.
// SNDMEMI_init es C++ (smemman.c no lo declara extern "C").
void SNDMEMI_init(void *pheap, int size);
extern "C" {
void MIX_create(MIXCONFIG *pmc);
void MIX_destroy();
void MIX_audioslice(void **poutput, int frames);
// Lo llama SNDMEMI_restore(); en GameCube lo daba la capa de plataforma.
int SNDMEM_gethighwater() { return 0; }
}

// --- El minimo que la libreria espera del sistema operativo ---------------
// snd protege sus estructuras con una seccion critica. El render corre en el
// hilo de AAudio y todavia nadie mas toca el mezclador, asi que de momento
// basta con que existan; cuando entre el hilo de juego habra que ponerles un
// mutex de verdad.
extern "C" void SNDSYS_entercritical() {}
extern "C" void SNDSYS_leavecritical() {}

static char *g_heap;
static short *g_chan[PORT_SND_MAXCH];
static void *g_out[PORT_SND_MAXCH];
static int g_channels;
static int g_frames_max;
static int g_ready;
static int g_voices;

int port_snd_init(int rate, int channels, int frames_max) {
    if (g_ready) return 1;
    if (channels < 1 || channels > PORT_SND_MAXCH) channels = 2;

    // Heap del gestor de snd. 1 MiB sobra para buses y buffers de mezcla; los
    // bancos de muestras, cuando entren, van aparte.
    //
    // TIENE QUE ESTAR POR DEBAJO DE LOS 4 GiB. El gestor de memoria de snd
    // guarda direcciones en campos de 32 bits (`SNDMEMREC::addr` es unsigned
    // int) porque en GameCube la RAM entera cabe ahi. Con un `malloc` normal de
    // arm64 la direccion se trunca y el primer recorrido de la lista libre lee
    // basura: medido, SIGSEGV en SNDMEMI_allocz leyendo 0xfffffffff1569ff8, que
    // es justo la direccion truncada y extendida con signo.
    //
    // Arreglarlo en src/ significaria cambiar el tipo de esos campos, y eso SI
    // moveria el codigo de GameCube. Se arregla aqui, en el port: mmap con una
    // pista baja.
    const int heapsize = 1 << 20;
    void *p = mmap((void *)0x20000000, (size_t)heapsize, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) p = 0;
    if (p && (unsigned long long)(uintptr_t)p + (unsigned long long)heapsize > 0xFFFFFFFFull) {
        munmap(p, (size_t)heapsize);   // nos lo dio alto: no sirve
        p = 0;
    }
    if (!p) {
        LOGE("no hay heap por debajo de 4 GiB: el gestor de snd no puede arrancar");
        return 0;
    }
    g_heap = (char *)p;
    memset(g_heap, 0, heapsize);
    LOGI("heap de snd en %p (%d KiB)", g_heap, heapsize >> 10);
    SNDMEMI_init(g_heap, heapsize);

    MIXCONFIG cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.outputrate = rate;
    cfg.chanovercallback = 0;
    cfg.inputchannels = 32;   // voces del mezclador
    cfg.outputchannels = (unsigned char)channels;
    cfg.outputinterleaved = 0; // MIX_audioslice escribe POR CANAL
    g_voices = cfg.inputchannels;

    MIX_create(&cfg);

    g_channels = channels;
    g_frames_max = frames_max;
    for (int c = 0; c < channels; c++) {
        g_chan[c] = (short *)calloc((size_t)frames_max, sizeof(short));
        g_out[c] = g_chan[c];
    }

    g_ready = 1;
    LOGI("mezclador EA arrancado: %d Hz, %d canales, %d voces, heap %d KiB",
         rate, channels, g_voices, heapsize >> 10);
    return 1;
}

// EL RENDER, y por que estuvo apagado.
//
// MIX_audioslice recorria `sndmix.outputfilterhead[i]->filterfn` y saltaba
// dentro de SFILTER_unpacklfinit, que es una funcion de INIT, no un filtro.
// Lo apunte a que las structs de snd llevan relleno de tamano fijo calculado
// para punteros de 4 bytes. La familia era esa, pero la causa concreta es mas
// fea: `sndmix` es un global unico que catorce unidades veian por CUATRO
// declaraciones distintas del mismo tipo. En GameCube las cuatro coinciden;
// en arm64 no, y `sinit16.c` escribia SFILTER_unpacklfinit justo donde
// smixer.c leia outputfilterhead. La traza no mentia: era ese puntero.
//
// smixport.h da ahora UNA sola definicion, y esta comprobado con static_assert
// que en 32 bits reproduce los 17 offsets medidos contra el asm y que en arm64
// coincide con la de smixer.c. Los .o de GameCube salen identicos en codigo y
// datos (solo cambia la seccion .line).
//
// Queda por confirmar EN EL APARATO: la correccion esta probada en reparto,
// no en ejecucion. Si vuelve a caer, el siguiente sitio donde mirar son las
// otras 18 structs con definiciones divergentes -- ninguna tiene ya el patron
// peligroso (relleno seguido de puntero), pero conviene remirarlo.
static int g_render_enabled = 1;   // interruptor: ponlo a 0 si el render vuelve a caer

int port_snd_ready(void) { return g_ready && g_render_enabled; }
int port_snd_started(void) { return g_ready; }
int port_snd_voices(void) { return g_voices; }

// Rinde `frames` y los entrega YA entrelazados, que es como los quiere AAudio.
int port_snd_render_interleaved(short *dst, int frames) {
    if (!g_ready || frames <= 0) return 0;
    if (frames > g_frames_max) frames = g_frames_max;

    MIX_audioslice(g_out, frames);

    for (int i = 0; i < frames; i++)
        for (int c = 0; c < g_channels; c++)
            dst[i * g_channels + c] = g_chan[c][i];
    return frames;
}

void port_snd_shutdown(void) {
    if (!g_ready) return;
    MIX_destroy();
    for (int c = 0; c < g_channels; c++) { free(g_chan[c]); g_chan[c] = 0; }
    munmap(g_heap, (size_t)(1 << 20)); g_heap = 0;
    g_ready = 0;
}
