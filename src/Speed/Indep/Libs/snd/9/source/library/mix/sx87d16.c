#ifdef __cplusplus
extern "C" {
#endif

void decode16x87(int n, short *psrc, float *pdst) {
    int i;

    for (i = 0; i < n; i++) {
        pdst[i] = (float)psrc[i];
    }
}

#ifdef __cplusplus
}
#endif
