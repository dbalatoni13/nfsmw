void MEM_fill(void *vdst, unsigned int fill, int size);

void MEM_clear(void *ptr, int size) {
    MEM_fill(ptr, 0, size);
}
