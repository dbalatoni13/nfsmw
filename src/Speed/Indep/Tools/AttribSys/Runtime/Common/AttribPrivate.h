#ifndef ATTRIBSYS_ATTRIB_PRIVATE_H
#define ATTRIBSYS_ATTRIB_PRIVATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Credit: Brawltendo
namespace Attrib {

class Private {
  public:
    int GetLength() const;

  private:
    unsigned char mData[8];
};

} // namespace Attrib

#endif
