#include <algorithm>

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

UTL::COM::Object::_IList::_IList(unsigned int icount) {
    reserve(icount);
}

UTL::COM::Object::_IList::~_IList() {}

void UTL::COM::Object::_IList::Add(HINTERFACE handle, IUnknown *ref) {
    _IPair pair(handle, ref);

    insert(std::upper_bound(begin(), end(), pair), pair);
}

UTL::COM::IUnknown *UTL::COM::Object::_IList::Find(HINTERFACE handle) {
    iterator iter = std::lower_bound(begin(), end(), _IPair(handle, NULL));

    if (iter != end() && iter->handle == handle) {
        return iter->ref;
    }

    return NULL;
}

void UTL::COM::Object::_IList::Remove(IUnknown *pUnk) {
    _IPair::_Finder finder(pUnk);
    iterator last = end();
    iterator first = std::find_if(begin(), last, finder);
    iterator result;

    if (first == last) {
        result = last;
    } else {
        iterator next = first;
        result = std::remove_copy_if(++next, last, first, finder);
    }
    erase(result);
}
