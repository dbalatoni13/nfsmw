extern "C" void *Alloc__3Vp6i(int size);
extern "C" void Free__3Vp6Pv(void *mem);

extern "C" void *duck_malloc(unsigned long size, void *user) {
    return Alloc__3Vp6i((int)size);
}

extern "C" void duck_free(void *mem, void *user) {
    Free__3Vp6Pv(mem);
}
