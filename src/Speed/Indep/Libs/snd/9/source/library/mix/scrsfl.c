extern "C" void rsflc(int frames, float *psrc, float *pdst, unsigned int *paccwhole,
                      unsigned int *paccfrac, unsigned int incwhole, unsigned int incfrac) {
    int i;
    float frac;
    unsigned int incrementor;

    incrementor = (incwhole << 16) | (incfrac >> 16);
    *paccfrac = *paccfrac >> 16;
    for (i = 0; i < frames; i++) {
        frac = *paccfrac * 1.5257999628386693e-05f;
        pdst[i] = psrc[*paccwhole] + frac * (psrc[*paccwhole + 1] - psrc[*paccwhole]);
        *paccfrac = *paccfrac + incrementor;
        *paccwhole = *paccwhole + (*paccfrac >> 16);
        *paccfrac = *paccfrac & 0xFFFF;
    }
    *paccfrac = *paccfrac << 16;
}
