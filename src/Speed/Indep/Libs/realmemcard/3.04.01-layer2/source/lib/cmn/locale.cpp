#include "../../../include/common/realmemcard/memcard_interface.h"

namespace Realmc {

namespace Locale {

static const char *(*gLocaleCallback)(int) = nullptr;

void SetLocaleGetStrCallback(const char *(*cb)(int)) {
    gLocaleCallback = cb;
}

int GetWstrLength(const wchar_t *str) {
    int len = 0;

    if (str != nullptr) {
        while (*str++ != 0) {
            len++;
        }
    }

    return len;
}

} // namespace Locale

} // namespace Realmc
