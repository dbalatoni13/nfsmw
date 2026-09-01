enum tmemtype {
    DMEM_GENERAL = 0,
    DMEM_TEMP = 1,
    DMEM_CDBUFF = 2,
    DMEM_FRAMEBUFF = 3
};

namespace Vp6 {

void *Alloc(int size);
void Free(void *ptr);

} // namespace Vp6

extern "C" void *duck_malloc(unsigned int blocksize, enum tmemtype foo) {
    void *temp;

    temp = Vp6::Alloc(blocksize);
    return temp;
}

extern "C" void duck_free(void *old_blk) {
    Vp6::Free(old_blk);
}
