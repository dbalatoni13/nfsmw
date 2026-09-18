typedef struct {
    unsigned int value : 7;
    unsigned int selector : 1;
} tokenorptr;

typedef union {
    char l;
    tokenorptr left;
} LEFTUNION_TYPE;

typedef union {
    char r;
    tokenorptr right;
} RIGHTUNION_TYPE;

typedef struct {
    LEFTUNION_TYPE leftunion;
    RIGHTUNION_TYPE rightunion;
    unsigned char freq;
} HUFF_NODE;

typedef struct {
    int next;
    int freq;
    tokenorptr value;
} sortnode;

static void InsertSorted(sortnode *sn, int node, int *startnode) {
    int which = *startnode;
    int prior = which;

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
        int first;
        int second;
        int sumfreq;

        first = startnode;
        second = sn[first].next;
        sumfreq = sn[first].freq + sn[second].freq;

        huffptr--;

        hn[huffptr].leftunion.left = sn[first].value;
        hn[huffptr].rightunion.right = sn[second].value;
        hn[huffptr].freq = (sn[first].freq << 8) / sumfreq;

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
    int i;
    int j;
    int bits;
    tokenorptr torp;

    for (i = 0; i <= 63; i++) {
        j = 0;
        bits = 6;
        torp.value = 0;
        torp.selector = 0;

        do {
            bits--;
            j++;

            if ((i >> bits) & 1) {
                torp = HuffTreeRoot[torp.value].rightunion.right;
            } else {
                torp = HuffTreeRoot[torp.value].leftunion.left;
            }
        } while (!torp.selector && bits);

        HuffTable[i] = (torp.value * 2 + torp.selector) | (j << 12);
    }
}

void VP6_CreateCodeArray(HUFF_NODE *hn, int node, unsigned int *codearray,
                         unsigned char *lengtharray, int codevalue, int codelength) {
    if (hn[node].leftunion.left.selector) {
        codearray[hn[node].leftunion.left.value] = codevalue << 1;
        lengtharray[hn[node].leftunion.left.value] = codelength + 1;
    } else {
        VP6_CreateCodeArray(hn, hn[node].leftunion.left.value, codearray, lengtharray,
                            codevalue << 1, codelength + 1);
    }

    if (hn[node].rightunion.right.selector) {
        codearray[hn[node].rightunion.right.value] = (codevalue << 1) + 1;
        lengtharray[hn[node].rightunion.right.value] = codelength + 1;
    } else {
        VP6_CreateCodeArray(hn, hn[node].rightunion.right.value, codearray, lengtharray,
                            (codevalue << 1) + 1, codelength + 1);
    }
}
