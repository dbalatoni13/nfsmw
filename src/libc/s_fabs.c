typedef unsigned int __uint32_t;

double fabs(double x)
{
    __uint32_t hx;
    union { double d; __uint32_t i[2]; } gw_u, sw_u;

    sw_u.d = x;
    gw_u.d = x;
    hx = gw_u.i[0];
    sw_u.i[0] = hx & 0x7fffffff;
    x = sw_u.d;
    return x;
}
