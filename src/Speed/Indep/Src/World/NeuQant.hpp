#ifndef __NEUQUANT_H
#define __NEUQUANT_H

void initnet(unsigned char *thepic, int len, int num_colours, int sample);
void unbiasnet();
void inxbuild();
int inxsearch(int b, int g, int r, int aa);
void learn();
void nqGetPaletteEntry(int i, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a);

#endif
