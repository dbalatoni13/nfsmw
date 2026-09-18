#ifdef __cplusplus
extern "C" {
#endif

void mixc(int count, float *psrc, float *pdst, float vol) {
    int i;

    for (i = 0; i < count; i++) {
        pdst[i] = psrc[i] * vol + pdst[i];
    }
}

#ifdef __cplusplus
}
#endif
