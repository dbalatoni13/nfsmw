#ifndef PORT_FENG_H
#define PORT_FENG_H

#ifdef __cplusplus
extern "C" {
#endif

int port_feng_load(void);
int port_feng_cycle(void);
const char *port_feng_name(void);
void port_feng_bind_logo(unsigned int tex);
void port_feng_upload_placeholders(void);
void port_feng_draw(void);
const char *port_feng_status(void);

#ifdef __cplusplus
}
#endif

#endif
