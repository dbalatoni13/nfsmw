typedef unsigned int USItype __attribute__((mode(SI)));

int __cmpdi2(long long a, long long b)
{
    union {
        long long ll;
        struct { int high; USItype low; } s;
    } aa, bb;

    aa.ll = a;
    bb.ll = b;
    if (aa.s.high < bb.s.high)
        return 0;
    if (aa.s.high > bb.s.high)
        return 2;
    if (aa.s.low < bb.s.low)
        return 0;
    if (aa.s.low > bb.s.low)
        return 2;
    return 1;
}
