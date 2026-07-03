#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "types.h"

char FEUpperCase(char val) {
    return static_cast<unsigned int>(val - 'a') > 25 ? val : val - 0x20;
}

// STRIPPED
char FELowerCase(char val) {}

int FEStricmp(const char *dst, const char *src) {
    char f, l;
    do {
        f = FEUpperCase(*dst++);
        l = FEUpperCase(*src++);
        if (f == 0) {
            break;
        }
    } while (f == l);
    return f - l;
}

// STRIPPED
bool FEStrInStr(const char *pString, const char *pSubString) {}

FENode::FENode() : name(nullptr), nameHash(0) {}

FENode::~FENode() {
    if (name) {
        delete[] name;
    }
}

bool FENode::SetName(const char *theName) {
    bool retval = false;
    int Len;

    if (name) {
        delete[] name;
        name = nullptr;
    }
    if (theName) {
        Len = FEngStrLen(theName);

        name = FNEW char[Len + 1];
        if (name) {
            retval = true;
            FEngStrCpy(name, theName);
        }
    }
    nameHash = FEHashUpper(name);
    return retval;
}

void FEMinList::AddNode(FEMinNode *insertpoint, FEMinNode *node) {
    if (!node) {
        return;
    }
    if (insertpoint) {
        node->next = insertpoint->next;
        if (node->next) {
            node->next->prev = node;
        }
        node->prev = insertpoint;
        insertpoint->next = node;
    } else {
        node->next = head;
        if (node->next) {
            node->next->prev = node;
        }
        node->prev = nullptr;
        head = node;
    }
    if (tail == insertpoint) {
        tail = node;
    }
    numElements++;
}

FEMinNode *FEMinList::RemNode(FEMinNode *node) {
    if (node) {
        if (node == head) {
            head = node->next;
        }
        if (node == tail) {
            tail = node->prev;
        }
        if (node->prev) {
            node->prev->next = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        }
        node->next = reinterpret_cast<FEMinNode *>(LIST_MAGIC);
        node->prev = reinterpret_cast<FEMinNode *>(LIST_MAGIC);
        numElements--;
    }
    return node;
}

FEMinNode *FEMinList::RemHead() {
    FEMinNode *n = head;
    if (n) {
        RemNode(n);
    }
    return n;
}

FEMinNode *FEMinList::FindNode(u32 ordinalnumber) const {
    u32 i = 0;
    FEMinNode *n = GetHead();
    while (n && i != ordinalnumber) {
        n = n->GetNext();
        i++;
    }
    return n;
}

FENode *FEList::FindNode(const char *pName, FENode *node) const {
    FENode *retn = nullptr;
    u32 Hash = FEHashUpper(pName);
    while (node) {
        if (node->GetName()) {
            if (Hash == node->GetNameHash() && FEStricmp(node->GetName(), pName) == 0) {
                retn = node;
                break;
            }
        } else if (!pName) {
            retn = node;
            break;
        }
        node = node->GetNext();
    }
    return retn;
}

FENode *FEList::FindNode(const char *pName) const {
    return FindNode(pName, static_cast<FENode *>(head));
}

u32 FEHash(const char *String) {
    u32 Hash = 0xFFFFFFFF;

    if (String) {
        while (*String) {
            Hash += Hash << 5;
            Hash += *reinterpret_cast<const u8 *>(String);
            String++;
        }
    }

    return Hash;
}

u32 FEHashUpper(const char *String) {
    u32 Hash = 0xFFFFFFFF;

    if (String) {
        while (*String) {
            Hash += Hash << 5;
            Hash += FEUpperCase(*String) & 0xFF;
            String++;
        }
    }

    return Hash;
}
