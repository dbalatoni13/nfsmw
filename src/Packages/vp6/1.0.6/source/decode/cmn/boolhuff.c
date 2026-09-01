typedef struct {
    unsigned int lowvalue;
    unsigned int range;
    unsigned int value;
    int count;
    unsigned int pos;
    unsigned char *buffer;
    unsigned int MeasureCost;
    unsigned int BitCounter;
} BOOL_CODER;

void VP6_StartDecode(BOOL_CODER *br, unsigned char *source) {
    br->lowvalue = 0;
    br->range = 255;
    br->count = 8;
    br->buffer = source;
    br->pos = 0;
    br->value = (br->buffer[0] << 24) + (br->buffer[1] << 16) + (br->buffer[2] << 8) + br->buffer[3];
    br->pos += 4;
}
