#ifndef INTERFACES_SIMABLES_IDISPOSABLE_H
#define INTERFACES_SIMABLES_IDISPOSABLE_H

#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IDisposable : public UTL::COM::IUnknown, public UTL::Collections::Listable<IDisposable, 160> {
  public:
    DECL_INTERFACE(IDisposable);

    virtual bool IsRequired() const;
};

#endif
