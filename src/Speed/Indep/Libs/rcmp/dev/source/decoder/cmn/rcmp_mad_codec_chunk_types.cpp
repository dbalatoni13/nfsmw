static const unsigned int ChunkTypes[3] = {
    0x4D41446D,
    0x4D414465,
    0x4D41446B,
};
static const unsigned int NumberChunkTypes = sizeof(ChunkTypes) / sizeof(ChunkTypes[0]);

namespace RCMP {

bool MAD_CODEC_is_chunk_for_codec(unsigned int chunktype) {
    {
        unsigned int i;
        const unsigned int *chunkTypes = ChunkTypes;

        for (i = NumberChunkTypes; i != 0; --i) {
            if (*chunkTypes++ == chunktype) {
                return true;
            }
        }
    }
    return false;
}

} // namespace RCMP
