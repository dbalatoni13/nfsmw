#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

char FEUpperCase(char val) {
    return static_cast<unsigned int>(val - 'a') > 25 ? val : static_cast<char>(val - 0x20);
}

unsigned long FEHash(const char* String) {
    unsigned long Hash = 0xFFFFFFFF;

    if (String) {
        while (*String) {
            Hash += Hash << 5;
            Hash += *reinterpret_cast<const unsigned char*>(String);
            String++;
        }
    }

    return Hash;
}

unsigned long FEHashUpper(const char* String) {
    unsigned long Hash = 0xFFFFFFFF;

    if (String) {
        while (*String) {
            Hash = (Hash << 5) + Hash;
            Hash += FEUpperCase(*String) & 0xFF;
            String++;
        }
    }

    return Hash;
}

int FEStricmp(const char* s1, const char* s2) {
    int c1, c2;
    do {
        c1 = FEUpperCase(*s1);
        s1++;
        c2 = FEUpperCase(*s2);
        s2++;
        if (c1 == 0) {
            break;
        }
    } while (c1 == c2);
    return c1 - c2;
}

FENode::FENode()
    : name(nullptr), //
      nameHash(0) {
}

FENode::~FENode() {
    if (name) {
        delete[] name;
    }
}

bool FENode::SetName(const char* theName) {
    bool retval = false;
    int Len;

    if (name) {
        delete[] name;
        name = nullptr;
    }
    if (theName) {
        Len = FEngStrLen(theName);

        name = FENG_NEW char[Len + 1];
        if (name) {
            retval = true;
            FEngStrCpy(name, theName);
        }
    }
    nameHash = FEHashUpper(name);
    return retval;
}

void FEMinList::AddNode(FEMinNode* insertpoint, FEMinNode* node) {
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

FEMinNode* FEMinList::RemNode(FEMinNode* node) {
    if (!node) {
        return nullptr;
    }
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
    node->next = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    node->prev = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    numElements--;
    return node;
}

FEMinNode* FEMinList::RemHead() {
    FEMinNode* node = head;
    if (node) {
        RemNode(node);
    }
    return node;
}

FEMinNode* FEMinList::FindNode(unsigned long ordinalnumber) const {
    unsigned long i = 0;
    FEMinNode* n = GetHead();
    while (n && i != ordinalnumber) {
        n = n->GetNext();
        i++;
    }
    return n;
}

FENode* FEList::FindNode(const char* pName, FENode* node) const {
    FENode* result = nullptr;
    unsigned long hash = FEHashUpper(pName);
    while (node) {
        if (node->name) {
            if (hash == node->nameHash && FEStricmp(node->name, pName) == 0) {
                result = node;
                break;
            }
        } else if (!pName) {
            result = node;
            break;
        }
        node = node->GetNext();
    }
    return result;
}

FENode* FEList::FindNode(const char* pName) const {
    return FindNode(pName, static_cast<FENode*>(head));
}
