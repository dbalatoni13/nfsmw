typedef struct {
    unsigned int bitpos;
    unsigned int range;
    unsigned int bitvalue;
    unsigned int count;
    unsigned int pos;
    const unsigned char *buffer;
} BOOL_DECODER;

void VP6_StartDecode(BOOL_DECODER *br, const unsigned char *buffer) {
    br->range = 255;
    br->count = 8;
    br->bitpos = 0;
    br->pos = 0;
    br->buffer = buffer;
    br->bitvalue = (buffer[0] << 24) + (buffer[1] << 16) + (buffer[2] << 8) + buffer[3];
    br->pos = 4;
}
