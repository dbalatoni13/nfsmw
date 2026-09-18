// Variables globales del sistema (solo datos).

unsigned int vbltmrsub[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int tmrsub[8] = {0, 0, 0, 0, 0, 0, 0, 0};

unsigned int vblticks = 0;
unsigned int TIMERhz = 0;
unsigned int ticks = 0;
unsigned int libticks = 0;


namespace RealFontOld {
void *gFontDriver = 0;
}

namespace RealShape {
struct MemObject {
    static void *sAllocator;
};
void *MemObject::sAllocator = 0;
}
