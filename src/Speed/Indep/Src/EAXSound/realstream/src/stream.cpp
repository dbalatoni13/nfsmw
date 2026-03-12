int STREAM_overhead(int requests, int filters, int taps) {
    int filterBytes = filters * 0xC;
    int tapBytes = taps << 4;
    int requestBytes = requests * 0x124;
    filterBytes += 0x18C;
    requestBytes += filterBytes;
    requestBytes += tapBytes;
    requestBytes += 0x20;
    return requestBytes;
}
