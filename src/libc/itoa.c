int strround(char *s, int i)
{
    int c;

    if (i > 0) {
        i--;
        c = s[i];
        if (c > '4') {
            do {
                s[i] = '0';
                i--;
            } while (i > 0 && s[i] == '9');
            if (s[i] == '9')
                return 0;
            s[i] = s[i] + 1;
        }
    }
    return 1;
}

char *strrev(char *s)
{
    int i, j;

    for (j = 0; s[j] != '\0'; j++)
        ;
    for (i = 0, j--; i < j; i++, j--) {
        char c;

        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
    return s;
}

char *itoa(int value, char *string, int radix)
{
    static char lower[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i;
    int neg;

    neg = value < 0;
    if (neg && radix == 10)
        value = -value;
    i = 0;
    do {
        string[i++] = lower[(unsigned int)value % (unsigned int)radix];
        value = (int)((unsigned int)value / (unsigned int)radix);
    } while (value != 0);
    if (neg)
        string[i++] = '-';
    string[i] = '\0';
    return strrev(string);
}

extern char *strcpy(char *, const char *);
extern double sn_floor(double);
extern double sn_fmod(double, double);

/* La libc de SN es newlib: la palabra alta y la baja del double se sacan con
 * la macro de fdlibm (math_private.h), no con un union local a la funcion.
 *
 * r67 -- RETIRADOS el pin `lo' en r11 y las dos barreras `__asm__("")' que
 * rodeaban `u.d = value; lo = u.w.lo;'.  Las tres imitaban lo que hace esta
 * macro: el `do { } while (0)' abre notas de bucle (cambia la ponderacion de
 * REG_N_REFS por profundidad) y el union vive en su propio ambito.  MEDIDO:
 * ALLOC de itoa.o identicas byte a byte (.text 1548 B, .rodata 64 B, .bss 164 B,
 * .rela.text).  `EXTRACT_WORDS(t, lo, value); mant = t & 0xfffff; t >>= 20;'
 * da el mismo objeto; `EXTRACT_WORDS(mant, lo, value)' NO (10 filas), y un
 * `do { u.d = value; lo = u.w.lo; } while (0)' con el union de fuera tampoco
 * (4 filas, solo registro).  Los negativos de la r65 (once ordenes y tipos de
 * `lo') y de la r66 (-msafe-sda, -mstrict-align) estan en
 * docs/analisis/r65-vfprintf.md y r66-libc-flags.md; ver r67-libc.md. */
typedef union {
    double value;
    struct {
        unsigned int msw;
        unsigned int lsw;
    } parts;
} ieee_double_shape_type;

#define EXTRACT_WORDS(ix0, ix1, d)       \
    do {                                 \
        ieee_double_shape_type ew_u;     \
        ew_u.value = (d);                \
        (ix0) = ew_u.parts.msw;          \
        (ix1) = ew_u.parts.lsw;          \
    } while (0)

static char str[164];

char *fftoa(double value, int ndigits, int fmt, int trim, char *sign)
{
    unsigned int hi, mant, t, neg, expo;
    unsigned int lo;
    char *buf;
    int i, e, total, prec, digit;
    double eps;

    EXTRACT_WORDS(hi, lo, value);
    mant = hi & 0xfffff;
    t = hi >> 20;
    neg = t & 0x800;
    expo = t & 0x7ff;
    if (expo == 0x7ff) {
        if (mant == 0 && lo == 0) {
            if (neg)
                strcpy(str, "-Inf");
            else
                strcpy(str, "Inf");
        } else
            strcpy(str, "NaN");
        return str;
    }

    prec = ndigits;
    i = 0;
    if (value < 0.0) {
        if (sign != 0) {
            *sign = '-';
            buf = str;
        } else {
            str[0] = '-';
            buf = str + 1;
        }
        value = -value;
    } else {
        buf = str;
    }

    if (value >= 1.0) {
        double intpart = sn_floor(value);
        int j, k;

        value -= intpart;
        do {
            if (i > 162)
                break;
            digit = (int)sn_fmod(intpart, 10.0);
            buf[i] = digit + '0';
            i++;
            intpart -= digit;
            intpart /= 10.0;
        } while (intpart >= 1.0);

        for (j = 0, k = i - 1; j < k; j++, k--) {
            char c;

            c = buf[j];
            buf[j] = buf[k];
            buf[k] = c;
        }
    }

    e = i;
    if (e == 0) {
        if (fmt != 'f') {
            if (value != 0.0) {
                value *= 10.0;
                while (value < 1.0 && e > -1021) {
                    value *= 10.0;
                    e--;
                }
                e--;
            }
            if (value >= 1.0) {
                value /= 10.0;
                prec = ndigits + 1;
            }
        } else {
            buf[e] = '0';
            i++;
        }
    }

    if (e < 0)
        total = prec;
    else if (fmt == 'f')
        total = prec + (e > 0 ? e : 1);
    else
        total = prec + 1;

    eps = 5.551115123125783e-17;
    for (;;) {
        value *= 10.0;
        digit = (int)value;
        value -= digit;
        eps *= 10.0;
        if (i < total && value >= eps && value <= 1.0 - eps) {
            buf[i] = digit + '0';
            i++;
        } else
            break;
    }

    if (value >= 0.5)
        digit++;
    buf[i] = digit + '0';
    i++;

    while (i <= total) {
        buf[i] = '0';
        i++;
    }

    if (strround(buf, total + 1) == 0) {
        buf[0] = '1';
        total = total + 1;
        e++;
    }

    if (ndigits != 0) {
        int point;

        if (fmt == 'f') {
            if (e > 0)
                point = e;
            else
                point = 1;
        } else {
            point = 1;
        }
        for (i = total; i > point; i--)
            buf[i] = buf[i - 1];
        buf[point] = '.';
    } else {
        total--;
    }

    if (trim) {
        while (total != 0 && buf[total] == '0')
            total--;
        if (buf[total] == '.')
            total--;
    }

    if (fmt != 'f') {
        total++;
        buf[total] = fmt;
        if (e >= 0) {
            total++;
            buf[total] = '+';
            if (e != 0)
                e--;
            if (e <= 9) {
                total++;
                buf[total] = '0';
            }
        } else {
            total++;
            buf[total] = '-';
            if (e > -10) {
                total++;
                buf[total] = '0';
            }
            e = -e;
        }
        total++;
        itoa(e, &buf[total], 10);
    } else {
        buf[total + 1] = '\0';
    }

    return str;
}
