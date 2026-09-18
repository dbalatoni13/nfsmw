/* sn_buf.cpp de la libc de SN: solo datos.  Los buffers de E/S que _sn_sfp
 * (fopen.c) asigna a los FILE de la tabla fija; empiezan vacios. */
extern "C" {

#include "local.h"

static struct __sbuf _sn_IO_buf[10] = {{0, 0}};
struct __sbuf *_sn_IO_buf_ptr = _sn_IO_buf;
}
