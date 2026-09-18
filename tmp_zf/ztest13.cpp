typedef unsigned int u32;
#define AT_ADDRESS(xyz) __attribute__((address((xyz))))

extern "C" {
u32 t_var_u32 AT_ADDRESS(0x80000100);
void *t_var_ptr AT_ADDRESS(0x80000104);
u32 t_var_init AT_ADDRESS(0x80000108) = 5;
volatile u32 t_var_vol AT_ADDRESS(0x8000010C);
struct _EP { int x; };
_EP *t_var_ep AT_ADDRESS(0x80000110);
}

float UsedFn(float x) { return x; }
