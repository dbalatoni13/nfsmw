#ifndef PORT_FONT_H
#define PORT_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

int port_font_load(void);
void port_font_draw(float x, float y, float sx, float sy, unsigned int argb, const char *text);
void port_font_draw_raw(float x, float y, float sx, float sy, unsigned int argb, const char *text);
unsigned int port_font_atlas(void);
const char *port_font_status(void);

#ifdef __cplusplus
}
#endif

#endif
