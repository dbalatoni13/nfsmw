#ifndef SUPPORT_MISC_CARP_H
#define SUPPORT_MISC_CARP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct UGroup;

namespace CARP {
unsigned int ResolveTagReferences(const UGroup *g, unsigned int deltaAddress);
}

#endif
