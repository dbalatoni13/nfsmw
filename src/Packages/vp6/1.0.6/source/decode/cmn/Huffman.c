#define HUFF_LUT_LEVELS 6

typedef struct _tokenorptr {
    unsigned int value : 7;
    unsigned int selector : 1;
} tokenorptr;

typedef struct _huffnode {
    union {
        char l;
        tokenorptr left;
    } leftunion;
    union {
        char r;
        tokenorptr right;
    } rightunion;
    unsigned char freq;
} HUFF_NODE;

typedef struct HUFF_TABLE_NODE {
    unsigned short length : 4;
    unsigned short unused : 6;
    unsigned short value : 5;
    unsigned short flag : 1;
} HUFF_TABLE_NODE;

typedef struct _SORT_NODE {
    int next;
    int freq;
    unsigned char value;
} SORT_NODE;

typedef struct _sortnode {
    int next;
    int freq;
    tokenorptr value;
} sortnode;

static void InsertSorted(sortnode *sn, int node, int *startnode) {
    int which = *startnode;
    int prior = *startnode;

    while (which != -1 && sn[node].freq > sn[which].freq) {
        prior = which;
        which = sn[which].next;
    }

    if (which == *startnode) {
        *startnode = node;
        sn[node].next = which;
    } else {
        sn[prior].next = node;
        sn[node].next = which;
    }
}

void VP6_BuildHuffTree(HUFF_NODE *hn, unsigned int *counts, int values) {
    int i;
    sortnode sn[64];
    int sncount = 0;
    int startnode = 0;
    int huffptr = values - 1;

    for (i = 0; i < values; i++) {
        sn[i].value.selector = 1;
        sn[i].value.value = i;
        if (counts[i] == 0) {
            counts[i] = 1;
        }
        sn[i].freq = counts[i];
        sn[i].next = -1;
    }

    sncount = values;

    for (i = 1; i < values; i++) {
        InsertSorted(sn, i, &startnode);
    }

    while (sn[startnode].next != -1) {
        int first = startnode;
        int second = sn[startnode].next;
        int sumfreq = sn[first].freq + sn[second].freq;

        --huffptr;
        hn[huffptr].leftunion.left = sn[first].value;
        hn[huffptr].rightunion.right = sn[second].value;
        hn[huffptr].freq = 256 * sn[first].freq / sumfreq;

        sn[sncount].value.selector = 0;
        sn[sncount].value.value = huffptr;
        sn[sncount].freq = sumfreq;
        sn[sncount].next = -1;

        startnode = sn[second].next;
        InsertSorted(sn, sncount, &startnode);
        sncount++;
    }
}

void VP6_BuildHuffLookupTable(HUFF_NODE *HuffTreeRoot, unsigned short *HuffTable) {
    int i, j;
    int bits;
    tokenorptr torp;

    for (i = 0; i < (1 << HUFF_LUT_LEVELS); i++) {
        bits = i;
        j = 0;

        torp.value = 0;
        torp.selector = 0;

        do {
            j++;
            if ((bits >> (HUFF_LUT_LEVELS - j)) & 1) {
                torp = HuffTreeRoot[torp.value].rightunion.right;
            } else {
                torp = HuffTreeRoot[torp.value].leftunion.left;
            }
        } while (!(torp.selector) && (j < HUFF_LUT_LEVELS));

        HuffTable[i] = torp.value << 1 | torp.selector | (j << 12);
    }
}

void VP6_CreateCodeArray(HUFF_NODE *hn, int node, unsigned int *codearray, unsigned char *lengtharray, int codevalue, int codelength) {
    if (hn[node].leftunion.left.selector) {
        codearray[hn[node].leftunion.left.value] = (codevalue << 1) + 0;
        lengtharray[hn[node].leftunion.left.value] = codelength + 1;
    } else {
        VP6_CreateCodeArray(hn, hn[node].leftunion.left.value, codearray, lengtharray, (codevalue << 1) + 0, codelength + 1);
    }

    if (hn[node].rightunion.right.selector) {
        codearray[hn[node].rightunion.right.value] = (codevalue << 1) + 1;
        lengtharray[hn[node].rightunion.right.value] = codelength + 1;
    } else {
        VP6_CreateCodeArray(hn, hn[node].rightunion.right.value, codearray, lengtharray, (codevalue << 1) + 1, codelength + 1);
    }
}
