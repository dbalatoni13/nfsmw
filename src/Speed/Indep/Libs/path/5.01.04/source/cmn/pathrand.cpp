#include "pathi.h"

/* En el objetivo la semilla esta INICIALIZADA y vive en .data 0x80451EB8, en el
   comodin auto_06_80451E88_data, que splits.txt no asigna a nadie (a esta
   unidad solo le da .text).  El original es:
       static unsigned int seedPATH[6] = { 0xF22D0E56, 0x883126E9, 0xC624DD2F,
                                           0x0702C49C, 0x9E353F7D, 0x6FDF3B64 };
   Declarandola extern con el nombre del comodin, pathrand.o no emite .bss. */
extern unsigned int seedPATH[6];

unsigned int PATHI_random()
{
    unsigned int ax;
    unsigned int c = 0;

    ax = seedPATH[5] + seedPATH[4];
    if (ax < seedPATH[5] || ax < seedPATH[4])
        c = 1;
    seedPATH[4] = ax;

    ax = ax + seedPATH[3] + c;
    c = (ax < seedPATH[3]);
    seedPATH[3] = ax;

    ax = ax + seedPATH[2] + c;
    c = (ax < seedPATH[2]);
    seedPATH[2] = ax;

    ax = ax + seedPATH[1] + c;
    c = (ax < seedPATH[1]);
    seedPATH[1] = ax;

    ax = ax + seedPATH[0] + c;
    seedPATH[0] = ax;

    if (++seedPATH[5] == 0)
    {
        if (++seedPATH[4] == 0)
        {
            if (++seedPATH[3] == 0)
            {
                if (++seedPATH[2] == 0)
                {
                    if (++seedPATH[1] == 0)
                    {
                        ax = ++seedPATH[0];
                    }
                }
            }
        }
    }

    return ax;
}
