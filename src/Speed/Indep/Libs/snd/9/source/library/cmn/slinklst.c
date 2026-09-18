#include "./sndcmn.h"

void SNDLINKI_init(SNDLINKLIST *plist) {
    plist->phead = 0;
    plist->ptail = 0;
    plist->items = 0;
}

void SNDLINKI_push(SNDLINKLIST *plist, SNDLINKNODE *pnode) {
    pnode->pnext = plist->phead;
    pnode->pprev = 0;
    if (plist->phead != 0) {
        plist->phead->pprev = pnode;
    } else {
        plist->ptail = pnode;
    }
    plist->phead = pnode;
    plist->items++;
}

void SNDLINKI_pushtail(SNDLINKLIST *plist, SNDLINKNODE *pnode) {
    pnode->pnext = 0;
    pnode->pprev = plist->ptail;
    if (plist->ptail != 0) {
        plist->ptail->pnext = pnode;
    } else {
        plist->phead = pnode;
    }
    plist->ptail = pnode;
    plist->items++;
}

void *SNDLINKI_pop(SNDLINKLIST *plist) {
    SNDLINKNODE *pnode = plist->phead;

    if (pnode != 0) {
        plist->phead = pnode->pnext;
        if (plist->phead == 0) {
            plist->ptail = 0;
        } else {
            plist->phead->pprev = 0;
        }
        plist->items--;
    }
    return pnode;
}

void SNDLINKI_remove(SNDLINKLIST *plist, SNDLINKNODE *pnode) {
    if (pnode == plist->phead) {
        plist->phead = pnode->pnext;
    }
    if (pnode == plist->ptail) {
        plist->ptail = pnode->pprev;
    }
    if (pnode->pprev != 0) {
        pnode->pprev->pnext = pnode->pnext;
    }
    if (pnode->pnext != 0) {
        pnode->pnext->pprev = pnode->pprev;
    }
    plist->items--;
}
