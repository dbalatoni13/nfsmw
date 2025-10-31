#ifndef MISC_ATTRIB_ASSET_H
#define MISC_ATTRIB_ASSET_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// TODO is this right to be here?

namespace Attrib {

class IGarbageCollector {
  public:
    virtual void ReleaseData(unsigned int id, void *data, unsigned int bytes) = 0;
};

}; // namespace Attrib

#endif
