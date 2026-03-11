int STREAM_overhead(int requests, int filters, int taps) {
    return filters * 0xC + requests * 0x124 + 0x18C + taps * 16 + 0x20;
}
