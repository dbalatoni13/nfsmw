int STREAM_overhead(int requests, int filters, int taps) {
    return filters * 0xC + 0x18C + requests * 0x124 + taps * 16 + 0x20;
}
