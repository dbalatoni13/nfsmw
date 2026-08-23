#include "../../../include/common/realcore/std.h"

void MEM_clear(void *dest, int count) {
    MEM_fill(dest, 0, count);
}
