typedef unsigned int u32;
u32 t_addr_init __attribute__((address(0x80000100))) = 1;
u32 t_plain_init = 2;
extern "C" u32 t_c_addr_init __attribute__((address(0x80000104))) = 3;
