#ifndef REALCORE_COMMON_IMPL_STD_LIST_INL
#define REALCORE_COMMON_IMPL_STD_LIST_INL

// Relleno: Push cae en 68..78, PushTail en 81..92, Pop en 95..114 y Remove en
// 139..174 segun el mapa de lineas del volcado.
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
template <class T> inline void ListSingle<T>::Push(T *node) {
    node->next = head;
    head = node;
    nNodes++;

    if (node->next == 0) {
        tail = node;
    }
}

template <class T> inline void ListSingle<T>::PushTail(T *node) {
    ListNodeS *ls = tail;

    node->next = 0;
    tail = node;
    nNodes++;

    if (ls == 0) {
        head = node;
    } else {
        ls->next = node;
    }
}

template <class T> inline T *ListSingle<T>::Pop() {
    T *ls = head;

    if (ls != 0) {

        if (ls == tail) {

            tail = 0;
            head = 0;
        } else {

            head = (T *)ls->next;
        }

        nNodes--;
        ls->next = 0;
    }

    return ls;
}

// Relleno: el mapa de lineas situa InsertAfter en 119..134 y su rama else en
// 128..134, con el ajuste de tail ANTES de reencadenar.
//
//
//
//
template <class T> inline void ListSingle<T>::InsertAfter(T *node, T *newnode) {
    if (node == 0) {
        Push(newnode);

    } else {

        //
        //

        if (node->next == 0) {
            tail = newnode;
        }
        newnode->next = node->next;
        node->next = newnode;

        nNodes++;
    }
}

template <class T> inline bool ListSingle<T>::Remove(T *node, T *prev) {
    T *p = 0;
    bool foundit = false;

    //
    //
    if (node == head) {

        foundit = true;
        nNodes--;
        if (node == tail) {
            head = 0;
            tail = p;
        } else {
            head = (T *)node->next;
        }
    } else if (head != 0) {

        p = head;
        while (p->next != 0 && p->next != node) {
            p = (T *)p->next;
        }
        if (p->next != 0 && p->next == node) {

            foundit = true;
            nNodes--;
            p->next = node->next;
            if (node == tail) {
                tail = p;
            }
        }
    }

    if (foundit) {

        node->next = 0;
    }

    return foundit;
}

#endif
