/* Con la pareja `#pragma interface`/`#pragma implementation` GCC 2.95 saca en
   esta unidad las CUATRO vtables de la cabecera (Interface, Event, Device y
   Effect, 256 B de `.rodata` contra los 80 del objetivo) y ademas los siete
   cuerpos de las virtuales en clase de `Device`, que empujan el destructor de
   `Interface` de .text+152 a +208.  El original solo trae aqui la vtable de
   `Interface`: las de Device/Effect/Event viven en el hueco
   `auto_05_80414CE0_rodata` y los metodos de `Device` en `device.cpp`.
   Apagando el `#pragma interface` SOLO para esta unidad (la guarda esta en la
   cabecera y no cambia nada para quien no la define) manda la regla normal del
   metodo clave: `Interface::AddRef` es la primera virtual no en clase y se
   define aqui, asi que su vtable sale aqui y las otras tres no.  */
#define REALINPUT_INPUT_IMPLEMENTATION
#include "../../../include/common/realcore/input.h"
#include "../gc/gc_interface.h"

namespace RealInput {

extern Interface *gInterface;

Interface *Interface::CreateInstance(const ConfigOptions &options) {
    if (gInterface == nullptr) {
        SetAllocator(options.mAllocator);
        gInterface = new GcInterface(options);
    }
    return gInterface;
}

int Interface::AddRef() {
    return 0;
}

int Interface::Release() {
    return 0;
}

void Interface::Update() {}

Device *Interface::GetPad() {
    return nullptr;
}

Device *Interface::GetMouse() {
    return nullptr;
}

Device *Interface::GetKeyboard() {
    return nullptr;
}

Event *Interface::GetEvent() {
    return nullptr;
}

Interface *gInterface = nullptr;

} // namespace RealInput
