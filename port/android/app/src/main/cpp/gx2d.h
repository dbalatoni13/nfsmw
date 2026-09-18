#ifndef PORT_GX2D_H
#define PORT_GX2D_H

#ifdef __cplusplus
extern "C" {
#endif

int gx2d_init(void);
void gx2d_shutdown(void);
void gx2d_resize(int fb_w, int fb_h);
void gx2d_begin(void);
void gx2d_quad(float x0, float y0, float x1, float y1, float r, float g, float b, float a);
unsigned int gx2d_make_solid_tex(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
unsigned int gx2d_upload_rgba(const unsigned char *rgba, int w, int h);
unsigned int gx2d_upload_rgba_linear(const unsigned char *rgba, int w, int h);
void gx2d_quad_tex(float x0, float y0, float x1, float y1, unsigned int tex, float r, float g, float b, float a);
void gx2d_fe_poly(float x0, float y0, float x1, float y1, float s0, float t0, float s1, float t1,
                  const unsigned int colors[4], unsigned int tex);
void gx2d_flush(void);

#ifdef __cplusplus
}
#endif

#endif
