#ifndef PORT_AAUDIO_H
#define PORT_AAUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

int port_audio_start(void);
void port_audio_stop(void);
void port_audio_set_paused(int paused);
const char *port_audio_status(void);

#ifdef __cplusplus
}
#endif

#endif
