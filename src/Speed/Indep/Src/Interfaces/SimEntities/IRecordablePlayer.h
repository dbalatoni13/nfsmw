#ifndef IRECORDABLEPLAYER_H
#define IRECORDABLEPLAYER_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Src/Sim/SimTypes.h"

class IRecordablePlayer : public UTL::COM::IUnknown, public UTL::Collections::Listable<IRecordablePlayer, 8> {
  public:
    DECL_INTERFACE(IRecordablePlayer);
};

#endif
