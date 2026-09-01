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

extern int VP6_DecodeBool128(BOOL_CODER *br);

inline unsigned int VP6_bitread(BOOL_CODER *br, int bits) {
    unsigned int z = 0;
    int bit;

    for (bit = bits - 1; bit >= 0; bit--) {
        z |= VP6_DecodeBool128(br) << bit;
    }
    return z;
}

typedef struct FRAME_HEADER {
    unsigned char *buffer;
    unsigned int value;
    int bits_available;
    unsigned int pos;
} FRAME_HEADER;

void InitHeaderBuffer(FRAME_HEADER *Header, unsigned char *Buffer) {
    Header->buffer = Buffer;
    Header->value = (Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3];
    Header->bits_available = 32;
    Header->pos = 4;
}

unsigned int ReadHeaderBits(FRAME_HEADER *Header, unsigned int BitsRequired) {
    unsigned int pos = Header->pos;
    unsigned int available = Header->bits_available;
    unsigned int value = Header->value;
    unsigned char *Buffer = &Header->buffer[pos];
    unsigned int RetVal = 0;

    if (available < BitsRequired) {
        RetVal = value >> (32 - available);
        BitsRequired -= available;
        RetVal <<= BitsRequired;

        value = (Buffer[0] << 24) + (Buffer[1] << 16) + (Buffer[2] << 8) + Buffer[3];
        pos += 4;
        available = 32;
    }

    RetVal |= value >> (32 - BitsRequired);
    Header->value = value << BitsRequired;
    Header->bits_available = available - BitsRequired;
    Header->pos = pos;

    return RetVal;
}
