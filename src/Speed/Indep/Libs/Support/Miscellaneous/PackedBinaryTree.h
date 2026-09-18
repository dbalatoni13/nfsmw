#ifndef SUPPORT_MISC_PACKEDBINARYTREE_H
#define SUPPORT_MISC_PACKEDBINARYTREE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template<typename T, typename U>
unsigned int SearchPackedBinaryTree(unsigned int count, T *table, U key) {
    unsigned int index = 0;

    while (index < count && table[index] != key) {
        unsigned int less = table[index] < key;
        unsigned int branch = less + 1;

        index = 2 * index + branch;
    }

    return index;
}

#endif
