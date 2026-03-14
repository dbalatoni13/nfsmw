#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

char FEUpperCase(char val) {
    if (static_cast<unsigned int>(val - 'a') > 25) {
        return val;
    }

    return val - 0x20;
}

unsigned long FEHash(const char* String) {
    unsigned long hash = 0xFFFFFFFF;

    if (String) {
        unsigned char c = *reinterpret_cast<const unsigned char*>(String);

        while (c != 0) {
            unsigned long value = hash * 33;

            hash = c + value;
            String++;
            c = *reinterpret_cast<const unsigned char*>(String);
        }
    }

    return hash;
}

unsigned long FEHashUpper(const char* String) {
    unsigned long hash = 0xFFFFFFFF;

    if (String) {
        char c = *String;

        while (c != '\0') {
            unsigned long uc = FEUpperCase(c);

            String++;
            hash = hash * 33 + (uc & 0xFF);
            c = *String;
        }
    }

    return hash;
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
    bool result = false;
    if (name) {
        delete[] name;
        name = nullptr;
    }
    if (theName) {
        int len = FEngStrLen(theName);
        name = static_cast<char*>(FEngMalloc(len + 1, 0, 0));
        if (name) {
            result = true;
            FEngStrCpy(name, theName);
        }
    }
    nameHash = FEHashUpper(name);
    return result;
}

void FEMinList::Purge() {
    FEMinNode* cmn;
    while ((cmn = RemHead()) != nullptr) {
        delete cmn;
    }
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
    FEMinNode* node = head;
    unsigned long i = 0;
    if (!node) {
        return nullptr;
    }
    if (i == ordinalnumber) {
        return node;
    }
    do {
        node = node->next;
        i++;
        if (!node) {
            return nullptr;
        }
    } while (i != ordinalnumber);
    return node;
}

FENode* FEList::FindNode(const char* pName, FENode* node) const {
    unsigned long hash = FEHashUpper(pName);
    while (node) {
        if (!node->name) {
            if (!pName) {
                return node;
            }
        } else if (hash == node->nameHash && FEStricmp(node->name, pName) == 0) {
            return node;
        }
        node = node->GetNext();
    }
    return nullptr;
}

FENode* FEList::FindNode(const char* pName) const {
    return FindNode(pName, static_cast<FENode*>(head));
}
