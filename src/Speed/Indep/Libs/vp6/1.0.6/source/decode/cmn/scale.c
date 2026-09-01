void NullScale(unsigned char *dest, unsigned int destPitch, unsigned int destWidth) {}

extern void *memcpy(void *dest, const void *source, unsigned int size);

void HorizontalLine_Copy(const unsigned char *source, unsigned int sourceWidth, unsigned char *dest, unsigned int destWidth) {
    memcpy(dest, source, sourceWidth);
}
