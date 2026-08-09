#ifndef ISNDATTACHABLE_HPP
#define ISNDATTACHABLE_HPP

#include "Speed/Indep/Libs/Support/Utility/UListable.h"

// total size: 0x8
class ISndAttachable : public UTL::Collections::Listable<ISndAttachable, 15> {
  public:
    virtual const bVector3 *GetPosition() = 0;
    virtual int GetType() = 0;
};

#endif
