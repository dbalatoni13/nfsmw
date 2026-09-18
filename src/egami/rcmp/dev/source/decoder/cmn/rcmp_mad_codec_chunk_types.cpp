namespace {
const unsigned int ChunkTypes[3] = {
    0x4D41446D,
    0x4D414465,
    0x4D41446B,
};
} // namespace

namespace RCMP {

int MAD_CODEC_is_chunk_for_codec(unsigned int chunk_type) {
    const unsigned int *ct = ChunkTypes;
    for (int i = 3; i != 0; i--) {
        if (*ct++ == chunk_type) {
            return 1;
        }
    }
    return 0;
}

} // namespace RCMP
