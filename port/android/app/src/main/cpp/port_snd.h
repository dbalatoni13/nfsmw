#ifndef PORT_SND_H
#define PORT_SND_H

#define PORT_SND_MAXCH 6

#ifdef __cplusplus
extern "C" {
#endif

int  port_snd_init(int rate, int channels, int frames_max);
int  port_snd_ready(void);
int  port_snd_started(void);
int  port_snd_voices(void);
int  port_snd_render_interleaved(short *dst, int frames);
void port_snd_shutdown(void);

#ifdef __cplusplus
}
#endif
#endif
