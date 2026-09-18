namespace RCMP {

int VP6_CODEC_is_head_chunk_for_codec(unsigned int chunk_type) {
    return (chunk_type ^ 0x4D566864) == 0;
}

int VP6_CODEC_is_chunk_for_codec(unsigned int chunk_type) {
    if (chunk_type == 0x4D563046 || chunk_type == 0x4D56304B) {
        return 1;
    }
    return 0;
}

} // namespace RCMP
