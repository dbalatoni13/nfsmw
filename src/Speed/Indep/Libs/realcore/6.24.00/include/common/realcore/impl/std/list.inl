#ifndef REALCORE_COMMON_IMPL_STD_LIST_INL
#define REALCORE_COMMON_IMPL_STD_LIST_INL

template <class T> inline ListSingle<T>::ListSingle() : head(nullptr), tail(nullptr), nNodes(0) {}

template <class T> inline ListSingle<T>::~ListSingle() {
    ListNodeS *node = this->head;
    ListNodeS *nextnode = node;
    while (node != nullptr && node->next != nullptr) {
        nextnode = node->next;
        node->next = nullptr;
        node = nextnode;
    }
    this->tail = nullptr;
    this->head = nullptr;
    this->nNodes = 0;
}

template <class T> inline void ListSingle<T>::Push(T *node) {
    node->next = this->head;
    this->head = node;
    this->nNodes++;
    if (node->next == nullptr) {
        this->tail = node;
    }
}

template <class T> inline void ListSingle<T>::PushTail(T *node) {
    ListNodeS *ls = this->tail;
    node->next = nullptr;
    this->tail = node;
    this->nNodes++;
    if (ls == nullptr) {
        this->head = node;
    } else {
        ls->next = node;
    }
}

template <class T> inline T *ListSingle<T>::Pop() {
    T *ls = this->head;
    if (ls != nullptr) {
        if (ls == this->tail) {
            this->head = nullptr;
            this->tail = nullptr;
        } else {
            this->head = static_cast<T *>(ls->next);
        }
        this->nNodes--;
        ls->next = nullptr;
    }
    return ls;
}

template <class T> inline T *ListSingle<T>::Head() {
    return this->head;
}

template <class T> inline unsigned int ListSingle<T>::GetCount() {
    return this->nNodes;
}

template <class T> inline void ListSingle<T>::InsertAfter(T *node, T *newnode) {
    if (node == nullptr) {
        this->Push(newnode);
    } else {
        if (node->next == nullptr) {
            this->tail = newnode;
        }
        newnode->next = node->next;
        node->next = newnode;
        this->nNodes++;
    }
}

template <class T> inline bool ListSingle<T>::Remove(T *node, T *prev) {
    bool foundit = false;
    if (node == this->head) {
        this->nNodes--;
        foundit = true;
        if (node == this->tail) {
            this->tail = nullptr;
            this->head = nullptr;
        } else {
            this->head = static_cast<T *>(node->next);
        }
    } else if (this->head != nullptr) {
        prev = this->head;
        while (prev->next != nullptr && prev->next != node) {
            prev = static_cast<T *>(prev->next);
        }
        if (prev->next != nullptr && prev->next == node) {
            this->nNodes--;
            foundit = true;
            prev->next = node->next;
            if (node == this->tail) {
                this->tail = prev;
            }
        }
    }
    if (foundit) {
        node->next = nullptr;
    }
    return foundit;
}

template <class T> inline ListSingleIterator<T> ListSingle<T>::Begin() {
    return ListSingleIterator<T>(this->head);
}

template <class T> inline void ListSingle<T>::VerifyList() {}

#endif
