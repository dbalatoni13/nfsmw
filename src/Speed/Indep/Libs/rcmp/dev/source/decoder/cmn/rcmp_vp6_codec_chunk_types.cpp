namespace RCMP {

bool VP6_CODEC_is_head_chunk_for_codec(unsigned int chunktype) {
    return chunktype == 0x4D566864;
}

bool VP6_CODEC_is_chunk_for_codec(unsigned int chunktype) {
    if (chunktype == 0x4D563046 || chunktype == 0x4D56304B) {
        return true;
    }
    return false;
}

} // namespace RCMP
