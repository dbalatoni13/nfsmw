#include <algorithm>

#include "Speed/Indep/Libs/Support/Utility/UCollections.h"

namespace UTL {
namespace Collections {

_KeyedNode *_KeyedNode::Search(_KeyedNode *begin_iter, _KeyedNode *end_iter, uintptr_t handle) {
    _KeyedNode node(handle, NULL);
    _KeyedNode *found = std::lower_bound(begin_iter, end_iter, node);

    if (found != end_iter && found->Handle == handle) {
        return found;
    }

    return NULL;
}

} // namespace Collections
} // namespace UTL
