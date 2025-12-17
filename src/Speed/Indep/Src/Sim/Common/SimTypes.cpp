#include "../SimAttachable.h"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"

#include <algorithm>

namespace Sim {

Attachments::~Attachments() {
    mList.clear();
}

bool Attachments::Attach(UTL::COM::IUnknown *object) {
    UTL::COM::ValidatePtr(object);
    UTL::COM::ValidatePtr(mOwner);
    if (UTL::COM::ComparePtr(object, mOwner)) {
        return false;
    }

    IAttachable *iother;
    if (!object->QueryInterface(&iother)) {
        return false;
    }

    IAttachable::List::iterator iter = std::find(mList.begin(), mList.end(), iother);
    // Make sure it's not in the list yet
    if (iter != mList.end()) {
        return false;
    }

    // Why don't we just push in the if check so we don't have to erase if it fails?
    mList.push_back(iother);

    if (iother->IsAttached(mOwner) || iother->Attach(mOwner)) {
        mOwner->OnAttached(iother);
        return true;
    } else {
        mList.erase(std::find(mList.begin(), mList.end(), iother));
        return false;
    }
}

bool Attachments::Detach(UTL::COM::IUnknown *object) {
    UTL::COM::ValidatePtr(object);
    IAttachable *iother;
    if (!object->QueryInterface(&iother)) {
        return false;
    }

    IAttachable::List::iterator iter = std::find(mList.begin(), mList.end(), iother);
    if (iter == mList.end()) {
        return false;
    }

    // Why don't we just erase in the if check so we don't have to push it back if it fails?
    mList.erase(iter);

    if (!iother->IsAttached(mOwner) || iother->Detach(mOwner)) {
        mOwner->OnDetached(iother);
        return true;
    } else {
        mList.push_back(iother);
        return false;
    }
}

bool Attachments::IsAttached(const UTL::COM::IUnknown *object) const {
    IAttachable::List::const_iterator iter = mList.begin();
    for (; iter != mList.end(); iter++) {
        const IAttachable *ia = *iter;
        if (UTL::COM::ComparePtr(object, ia)) {
            return true;
        }
    }
    return false;
}

void Attachments::DetachAll() {
    while (!mList.empty()) {
        IAttachable::List::iterator iter = mList.begin();
        bool result = Detach(*iter);
    }
}

}; // namespace Sim
