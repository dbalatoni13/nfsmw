#include "port_aaudio.h"
#include "port_snd.h"

#include <aaudio/AAudio.h>
#include <android/log.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "nfsmw", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "nfsmw", __VA_ARGS__)

static AAudioStream *g_stream;
static volatile int g_paused;
static volatile int g_samples_played;
static char g_status[80] = "audio: off";
static volatile int g_mix_logged;

static aaudio_data_callback_result_t on_audio(AAudioStream *stream, void *, void *audioData, int32_t numFrames) {
    int16_t *out = (int16_t *)audioData;
    int32_t ch = AAudioStream_getChannelCount(stream);
    int32_t rate = AAudioStream_getSampleRate(stream);
    if (rate < 1) rate = 48000;
    if (g_paused) {
        memset(out, 0, (size_t)(numFrames * ch * (int)sizeof(int16_t)));
        return AAUDIO_CALLBACK_RESULT_CONTINUE;
    }

    // Camino normal: lo rinde el MEZCLADOR REAL de EA (MIX_audioslice).
    if (port_snd_ready()) {
        memset(out, 0, (size_t)(numFrames * ch * (int)sizeof(int16_t)));
        int done = port_snd_render_interleaved(out, numFrames);
        g_samples_played += done;

        // La primera vuelta se anota en el log: es la prueba de que el
        // mezclador decompilado corre en el telefono sin caerse.
        if (!g_mix_logged) {
            int peak = 0;
            for (int i = 0; i < done * ch; i++) {
                int v = out[i] < 0 ? -out[i] : out[i];
                if (v > peak) peak = v;
            }
            LOGI("MIX_audioslice OK: %d frames, %d canales, pico %d", done, (int)ch, peak);
            g_mix_logged = 1;
        }
        return AAUDIO_CALLBACK_RESULT_CONTINUE;
    }

    // Respaldo: el pitido de la fase 1.2, por si el mezclador no arranco.
    for (int32_t i = 0; i < numFrames; i++) {
        int n = g_samples_played++;
        float t = (float)n / (float)rate;
        float env = 1.0f;
        if (t < 0.04f) env = t / 0.04f;
        else if (t > 0.35f && t < 1.2f) env = 1.0f - (t - 0.35f) / 0.85f;
        else if (t >= 1.2f) env = 0.08f;
        if (env < 0.08f) env = 0.08f;
        float freq = (t < 0.18f) ? 392.0f : 523.25f;
        float s = sinf(6.2831853f * freq * t) * env * 0.22f;
        int16_t v = (int16_t)(s * 32767.0f);
        for (int32_t c = 0; c < ch; c++) out[i * ch + c] = v;
    }
    return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

int port_audio_start(void) {
    if (g_stream) return 1;
    AAudioStreamBuilder *b = NULL;
    if (AAudio_createStreamBuilder(&b) != AAUDIO_OK || !b) {
        snprintf(g_status, sizeof(g_status), "audio: builder fail");
        return 0;
    }
    AAudioStreamBuilder_setDirection(b, AAUDIO_DIRECTION_OUTPUT);
    AAudioStreamBuilder_setSharingMode(b, AAUDIO_SHARING_MODE_SHARED);
    AAudioStreamBuilder_setFormat(b, AAUDIO_FORMAT_PCM_I16);
    AAudioStreamBuilder_setChannelCount(b, 2);
    AAudioStreamBuilder_setSampleRate(b, 48000);
    AAudioStreamBuilder_setPerformanceMode(b, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY);
    AAudioStreamBuilder_setDataCallback(b, on_audio, NULL);
    aaudio_result_t r = AAudioStreamBuilder_openStream(b, &g_stream);
    AAudioStreamBuilder_delete(b);
    if (r != AAUDIO_OK || !g_stream) {
        snprintf(g_status, sizeof(g_status), "audio: open %d", (int)r);
        g_stream = NULL;
        return 0;
    }
    g_samples_played = 0;
    g_mix_logged = 0;

    // El mezclador de EA, con el ritmo y los canales que ha concedido AAudio.
    {
        int rate = AAudioStream_getSampleRate(g_stream);
        int chn = AAudioStream_getChannelCount(g_stream);
        int cap = AAudioStream_getBufferCapacityInFrames(g_stream);
        if (rate < 1) rate = 48000;
        if (chn < 1) chn = 2;
        if (cap < 2048) cap = 2048;
        if (port_snd_init(rate, chn, cap)) {
            snprintf(g_status, sizeof(g_status), "audio: mezclador EA %d Hz %dch", rate, chn);
        } else {
            snprintf(g_status, sizeof(g_status), "audio: mezclador EA NO arranca");
            LOGE("%s", g_status);
        }
    }

    r = AAudioStream_requestStart(g_stream);
    if (r != AAUDIO_OK) {
        snprintf(g_status, sizeof(g_status), "audio: start %d", (int)r);
        AAudioStream_close(g_stream);
        g_stream = NULL;
        return 0;
    }
    LOGI("%s", g_status);
    return 1;
}

void port_audio_stop(void) {
    if (!g_stream) return;
    AAudioStream_requestStop(g_stream);
    AAudioStream_close(g_stream);
    g_stream = NULL;
    snprintf(g_status, sizeof(g_status), "audio: off");
}

void port_audio_set_paused(int paused) {
    g_paused = paused ? 1 : 0;
}

const char *port_audio_status(void) {
    return g_status;
}
