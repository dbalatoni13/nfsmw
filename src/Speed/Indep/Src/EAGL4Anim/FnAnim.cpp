#include "FnAnim.h"
#include "FnAnimFactory.h"

namespace EAGL4Anim {

bool FnAnim::gReverseDeltaSumEnabled = true;

FnAnimFactory *FnAnimFactory::mpFactory = nullptr;

const AttributeBlock *FnAnim::GetAttributes() const {
    return nullptr;
}

}; // namespace EAGL4Anim
