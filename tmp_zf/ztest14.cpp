typedef unsigned int u32;
#define AT_ADDRESS(xyz) __attribute__((address((xyz))))

extern "C" u32 t_with_addr AT_ADDRESS(0x80000100) = 1;
extern "C" u32 t_no_addr = 2;
extern "C" u32 t_addr_noinit AT_ADDRESS(0x80000104);
