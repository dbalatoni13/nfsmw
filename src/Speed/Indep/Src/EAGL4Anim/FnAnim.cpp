#include "FnAnim.h"
#include "FnAnimFactory.h"

namespace EAGL4Anim {

const AttributeBlock *FnAnim::GetAttributes() const {
    return nullptr;
}

bool FnAnim::gReverseDeltaSumEnabled = true;

FnAnimFactory *FnAnimFactory::mpFactory = nullptr;

}; // namespace EAGL4Anim
