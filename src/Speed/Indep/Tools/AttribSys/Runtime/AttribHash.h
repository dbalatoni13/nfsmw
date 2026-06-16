#ifndef _stringhash_h_
#define _stringhash_h_

#include "Speed/Indep/bWare/Inc/bTypes.hpp"

namespace Attrib {

unsigned long long StringHash64(const char *k);
unsigned int StringHash32(const char *k);

} // namespace Attrib

#endif
