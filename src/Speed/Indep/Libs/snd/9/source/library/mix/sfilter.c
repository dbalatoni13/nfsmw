#include <snd/sfilter.h>

extern "C" {

void SFILTER_add(SFILTERNODE **pphead, SFILTERNODE *pnew) {
    SFILTERNODE *pprev;
    SFILTERNODE *pnext;

    pprev = 0;
    pnext = *pphead;
    while (pnext != 0 && pnext->priority < pnew->priority) {
        pprev = pnext;
        pnext = pprev->pfnnext;
    }
    pnew->pfnnext = pnext;
    if (pnext != 0) {
        pnext->pfnprev1 = pnew;
    }
    if (pprev == 0) {
        *pphead = pnew;
    } else {
        pprev->pfnnext = pnew;
        pnew->pfnprev1 = pprev;
    }
}

void SFILTER_remove(SFILTERNODE **pphead, SFILTERNODE *pcur) {
    SFILTERNODE *pprev;

    pprev = *pphead;
    if (pcur == pprev) {
        *pphead = pprev->pfnnext;
        return;
    }
    while (pprev->pfnnext != 0 && pprev->pfnnext != pcur) {
        pprev = pprev->pfnnext;
    }
    if (pprev->pfnnext != 0 && pprev->pfnnext == pcur) {
        if (pcur->pfnnext != 0) {
            pcur->pfnnext->pfnprev1 = pprev;
        }
        pprev->pfnnext = pprev->pfnnext->pfnnext;
    }
}

int SFILTER_connect(SFILTERNODE *pnode1, SFILTERNODE *pnode2, int node1output, int node2input) {
    if (node1output == 1 && node2input == 1 && pnode2->pfnnext == 0) {
        pnode1->pfnprev1 = pnode2;
        pnode2->pfnnext = pnode1;
        pnode2->requester = node2input;
        return 0;
    }
    if (node1output == 1 && node2input == 2 && pnode2->pfnnext2 == 0) {
        pnode1->pfnprev1 = pnode2;
        pnode2->pfnnext2 = pnode1;
        pnode2->requester = node1output;
        return 0;
    }
    if (node1output == 2 && node2input == 1 && pnode2->pfnnext == 0) {
        pnode1->pfnprev2 = pnode2;
        pnode2->pfnnext = pnode1;
        pnode2->requester = node1output;
        return 0;
    }
    if (node1output == 2 && node2input == 2 && pnode2->pfnnext2 == 0) {
        pnode1->pfnprev2 = pnode2;
        pnode2->pfnnext2 = pnode1;
        pnode2->requester = node2input;
        return 0;
    }
    return -1;
}

}
