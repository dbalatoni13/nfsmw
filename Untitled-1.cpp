
// Range: 0x800526AC -> 0x800527E0
void *Collection::GetData(const struct Collection *const this /* r3 */, unsigned int attributeKey /* r4 */, unsigned int index /* r30 */) {
    // Local variables
    const struct Collection *c; // r1+0x8
    struct Node *node;          // r3

    // Inlines
    // Range: 0x800526D0 -> 0x800526E4
    inline bool Node::IsArray(const struct Node *const this, const struct Node *const this /* r3 */) {
        // Inlines
        // Range: 0x800526D0 -> 0x800526E4
        inline bool Node::GetFlag(const struct Node *const this, unsigned int mask, unsigned int mask, const struct Node *const this /* r3 */) {}
    }
    // Range: 0x800526EC -> 0x8005271C
    inline struct Array *Node::GetArray(const struct Node *const this, void *layoutptr, const struct Node *const this /* r3 */,
                                        void *layoutptr /* r9 */) {
        // Inlines
        // Range: 0x800526EC -> 0x80052704
        inline bool Node::IsLaidOut(const struct Node *const this, const struct Node *const this /* r3 */) {
            // Inlines
            // Range: 0x800526EC -> 0x80052704
            inline bool Node::GetFlag(const struct Node *const this, unsigned int mask, const struct Node *const this /* r3 */, unsigned int mask) {}
        }
    }
    // Range: 0x8005271C -> 0x80052770
    inline void *Array::GetData(const struct Array *const this, unsigned int index, const struct Array *const this, unsigned int index /* r30 */) {
        // Inlines
        // Range: 0x80052728 -> 0x80052728
        inline bool Array::IsReferences(const struct Array *const this, const struct Array *const this) {}
        // Range: 0x80052734 -> 0x80052734
        inline void *Array::Data(const struct Array *const this, unsigned int byteindex, unsigned int byteindex, const struct Array *const this) {
            // Local variables
            unsigned char *base;

            // Inlines
            // Range: 0x80052734 -> 0x80052734
            inline unsigned char *Array::BasePointer(const struct Array *const this, const struct Array *const this) {}
        }
        // Range: 0x80052750 -> 0x80052750
        inline void *Array::Data(const struct Array *const this, unsigned int byteindex, unsigned int byteindex, const struct Array *const this) {
            // Local variables
            unsigned char *base;

            // Inlines
            // Range: 0x80052750 -> 0x80052750
            inline unsigned char *Array::BasePointer(const struct Array *const this, const struct Array *const this) {}
        }
    }
    // Range: 0x80052774 -> 0x800527C4
    inline void *Node::GetPointer(const struct Node *const this, void *layoutptr, const struct Node *const this /* r3 */, void *layoutptr /* r10 */) {
        // Inlines
        // Range: 0x80052774 -> 0x8005278C
        inline bool Node::IsByValue(const struct Node *const this, const struct Node *const this /* r3 */) {
            // Inlines
            // Range: 0x80052774 -> 0x8005278C
            inline bool Node::GetFlag(const struct Node *const this, unsigned int mask, const struct Node *const this /* r3 */, unsigned int mask) {}
        }
        // Range: 0x8005279C -> 0x800527AC
        inline bool Node::IsLaidOut(const struct Node *const this, const struct Node *const this /* r3 */) {
            // Inlines
            // Range: 0x8005279C -> 0x800527AC
            inline bool Node::GetFlag(const struct Node *const this, unsigned int mask, const struct Node *const this /* r3 */, unsigned int mask) {}
        }
    }
}
