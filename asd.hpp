
/*
    Compile unit: src/Speed/Indep/SourceLists/zAttribSys.cpp
    Producer: GNU C++ 2.95.3 SN BUILD v1.76 for Nintendo Gamecube
    Compile directory: C:\Users\kabiskac\Documents\git_contributions\nfsmw
    Language: C++
    Code range: 0x00000000 -> 0x00000534
*/
struct Array {
    // total size: 0x8
    unsigned short mAlloc; // offset 0x0, size 0x2
    unsigned short mCount; // offset 0x2, size 0x2
    unsigned short mSize; // offset 0x4, size 0x2
    unsigned short mEncodedTypePad; // offset 0x6, size 0x2
};
inline unsigned char * Array::BasePointer(struct Array * const this) {}

inline void * Array::Data(struct Array * const this, unsigned int byteindex) {
    // Local variables
    unsigned char * base;
}

inline void * Array::GetData(struct Array * const this, unsigned int index) {}

struct Node {
    // total size: 0xC
    unsigned int mKey; // offset 0x0, size 0x4
    union {
        void * mPtr; // offset 0x0, size 0x4
        struct Array * mArray; // offset 0x0, size 0x4
        unsigned int mValue; // offset 0x0, size 0x4
        unsigned int mOffset; // offset 0x0, size 0x4
    }; // offset 0x4, size 0x4
    unsigned short mTypeIndex; // offset 0x8, size 0x2
    unsigned char mMax; // offset 0xA, size 0x1
    unsigned char mFlags; // offset 0xB, size 0x1
};
inline bool Node::GetFlag(struct Node * const this, unsigned int mask) {}

inline bool Node::RequiresRelease(struct Node * const this) {}

inline bool Node::IsArray(struct Node * const this) {}

inline bool Node::IsInherited(struct Node * const this) {}

inline bool Node::IsAccessor(struct Node * const this) {}

inline bool Node::IsLaidOut(struct Node * const this) {}

inline bool Node::IsByValue(struct Node * const this) {}

inline bool Node::IsLocatable(struct Node * const this) {}

inline bool Node::IsValid(struct Node * const this) {}

inline void * Node::GetPointer(struct Node * const this, void * layoutptr) {}

inline struct Array * Node::GetArray(struct Node * const this, void * layoutptr) {}

inline unsigned int Node::GetValue(struct Node * const this) {}

inline unsigned int * Node::GetRefValue(struct Node * const this) {}

inline unsigned int Node::GetKey(struct Node * const this) {}

inline unsigned int Node::MaxSearch(struct Node * const this) {}

inline unsigned int RotateNTo32(unsigned int v, unsigned int amount) {}

struct HashMap {
    // total size: 0x10
    struct Node * mTable; // offset 0x0, size 0x4
    unsigned int mTableSize; // offset 0x4, size 0x4
    unsigned int mNumEntries; // offset 0x8, size 0x4
    unsigned short mWorstCollision; // offset 0xC, size 0x2
    unsigned short mKeyShift; // offset 0xE, size 0x2
};
inline unsigned int HashMap::Size(const struct HashMap * const this) {}

inline bool HashMap::ValidIndex(struct HashMap * const this, unsigned int index) {}

inline struct Node * HashMap::Find(struct HashMap * const this, unsigned int key) {
    // Local variables
    unsigned int index;
}

inline unsigned int HashMapTablePolicy::KeyIndex(unsigned int k, unsigned int tableSize, unsigned int keyShift) {}

inline long abs(long __x) {}

struct ldiv_t {
    // total size: 0x8
    long quot; // offset 0x0, size 0x4
    long rem; // offset 0x4, size 0x4
};
inline struct ldiv_t div(long __x, long __y) {}

inline void * __copy_trivial(const void * __first, const void * __last, void * __result) {}

inline void * __copy_trivial_backward(const void * __first, const void * __last, void * __result) {
    // Local variables
    const int _Num;
}

inline void fill(unsigned char * __first, unsigned char * __last, const unsigned char & __value) {
    // Local variables
    unsigned char __tmp;
}

inline void fill(signed char * __first, signed char * __last, const signed char & __value) {
    // Local variables
    signed char __tmp;
}

inline void fill(char * __first, char * __last, const char & __value) {
    // Local variables
    char __tmp;
}

inline const unsigned int & min(const unsigned int & __a, const unsigned int & __b) {}

inline bool lexicographical_compare(const unsigned char * __first1, const unsigned char * __last1, const unsigned char * __first2, const unsigned char * __last2) {
    // Local variables
    const unsigned int __len1;
    const unsigned int __len2;
    const int __result;
}

inline bool lexicographical_compare(const char * __first1, const char * __last1, const char * __first2, const char * __last2) {}

inline const int & min(const int & __a, const int & __b) {}

inline int __lexicographical_compare_3way(const unsigned char * __first1, const unsigned char * __last1, const unsigned char * __first2, const unsigned char * __last2) {
    // Local variables
    const int __len1;
    const int __len2;
    const int __result;
}

inline int __lexicographical_compare_3way(const char * __first1, const char * __last1, const char * __first2, const char * __last2) {}

struct exception {
    // total size: 0x4
    const struct __vtbl_ptr_type * _vptr.exception; // offset 0x0, size 0x4
};
inline struct exception * exception::exception(struct exception * const this) {}

struct __vtbl_ptr_type {
    // total size: 0x8
    short __delta; // offset 0x0, size 0x2
    short __index; // offset 0x2, size 0x2
    union { // inferred
        void * __pfn; // offset 0x4, size 0x4
        short __delta2; // offset 0x4, size 0x2
    };
};
inline virtual void exception::exception(struct exception * const this, const int __in_chrg) {}

struct bad_exception : public exception {
    // total size: 0x4
};
inline struct bad_exception * bad_exception::bad_exception(struct bad_exception * const this) {}

inline virtual void exception::bad_exception(struct bad_exception * const this, const int __in_chrg) {}

inline void * __nw(unsigned int, void * place) {}

inline void * __vn(unsigned int, void * place) {}

struct bad_alloc : public exception {
    // total size: 0x4
};
inline virtual const char * exception::what(const struct bad_alloc * const this) {}

inline void * __stl_new(unsigned int __n) {}

inline void __stl_delete(void * __p) {}

struct _STLP_mutex_base {
    // total size: 0x1
};
inline void _STLP_mutex_base::_M_initialize(struct _STLP_mutex_base * const this) {}

inline void _STLP_mutex_base::_M_destroy(struct _STLP_mutex_base * const this) {}

inline void _STLP_mutex_base::_M_acquire_lock(struct _STLP_mutex_base * const this) {}

inline void _STLP_mutex_base::_M_release_lock(struct _STLP_mutex_base * const this) {}

struct _STLP_mutex_indirect {
    // total size: 0x4
    void * _M_lock; // offset 0x0, size 0x4
};
inline void _STLP_mutex_indirect::_M_initialize(struct _STLP_mutex_indirect * const this) {}

inline void _STLP_mutex_indirect::_M_destroy(struct _STLP_mutex_indirect * const this) {}

inline void _STLP_mutex_indirect::_M_acquire_lock(struct _STLP_mutex_indirect * const this) {}

inline void _STLP_mutex_indirect::_M_release_lock(struct _STLP_mutex_indirect * const this) {}

struct _STLP_mutex : public _STLP_mutex_indirect {
    // total size: 0x4
};
inline struct _STLP_mutex * _STLP_mutex::_STLP_mutex(struct _STLP_mutex * const this) {}

inline void _STLP_mutex::_STLP_mutex(struct _STLP_mutex * const this, const int __in_chrg) {}

struct _Refcount_Base {
    // total size: 0x8
    volatile unsigned int _M_ref_count; // offset 0x0, size 0x4
    struct _STLP_mutex _M_mutex; // offset 0x4, size 0x4
};
inline struct _Refcount_Base * _Refcount_Base::_Refcount_Base(struct _Refcount_Base * const this, unsigned int __n) {}

inline void _Refcount_Base::_M_incr(struct _Refcount_Base * const this) {}

inline void _Refcount_Base::_M_decr(struct _Refcount_Base * const this) {}

static inline unsigned int _Atomic_swap(volatile unsigned int * __p, unsigned int __q) {
    // Local variables
    unsigned int __result;
}

struct _STLP_auto_lock {
    // total size: 0x4
    struct _STLP_mutex_base & _M_lock; // offset 0x0, size 0x4
};
inline struct _STLP_auto_lock * _STLP_auto_lock::_STLP_auto_lock(struct _STLP_auto_lock * const this, struct _STLP_mutex_base & __lock) {}

inline void _STLP_auto_lock::_STLP_auto_lock(struct _STLP_auto_lock * const this, const int __in_chrg) {}

inline void _Destroy() {}

struct __new_alloc {
    // total size: 0x1
};
inline void * __new_alloc::allocate(unsigned int __n) {}

inline void __new_alloc::deallocate(void * __p, unsigned int) {}

struct _List_iterator_base {
    // total size: 0x4
    struct _List_node_base * _M_node; // offset 0x0, size 0x4
};
inline struct _List_iterator_base * _List_iterator_base::_List_iterator_base(struct _List_iterator_base * const this, struct _List_node_base * __x) {}

struct bidirectional_iterator_tag : public forward_iterator_tag {
    // total size: 0x1
};
struct _List_node_base {
    // total size: 0x8
    struct _List_node_base * _M_next; // offset 0x0, size 0x4
    struct _List_node_base * _M_prev; // offset 0x4, size 0x4
};
inline struct _List_iterator_base * _List_iterator_base::_List_iterator_base(struct _List_iterator_base * const this) {}

inline void _List_iterator_base::_M_incr(struct _List_iterator_base * const this) {}

inline void _List_iterator_base::_M_decr(struct _List_iterator_base * const this) {}

inline bool _List_iterator_base::__eq(const struct _List_iterator_base * const this, const struct _List_iterator_base & __y) {}

inline bool _List_iterator_base::__ne(const struct _List_iterator_base * const this, const struct _List_iterator_base & __y) {}

struct _Rb_tree_node_base {
    // total size: 0x10
    bool _M_color; // offset 0x0, size 0x1
    struct _Rb_tree_node_base * _M_parent; // offset 0x4, size 0x4
    struct _Rb_tree_node_base * _M_left; // offset 0x8, size 0x4
    struct _Rb_tree_node_base * _M_right; // offset 0xC, size 0x4
};
inline struct _Rb_tree_node_base * _Rb_tree_node_base::_S_minimum(struct _Rb_tree_node_base * __x) {}

inline struct _Rb_tree_node_base * _Rb_tree_node_base::_S_maximum(struct _Rb_tree_node_base * __x) {}

struct _Rb_tree_base_iterator {
    // total size: 0x4
    struct _Rb_tree_node_base * _M_node; // offset 0x0, size 0x4
};
inline bool _Rb_tree_base_iterator::__eq(const struct _Rb_tree_base_iterator * const this, const struct _Rb_tree_base_iterator & __y) {}

inline bool _Rb_tree_base_iterator::__ne(const struct _Rb_tree_base_iterator * const this, const struct _Rb_tree_base_iterator & __y) {}

inline int __black_count(struct _Rb_tree_node_base * __node, struct _Rb_tree_node_base * __root) {
    // Blocks
    /* anonymous block */ {
        int __bc;
    }
}

inline char * uninitialized_copy(const char * __first, const char * __last, char * __result) {}

struct _Bit_reference {
    // total size: 0x8
    unsigned int * _M_p; // offset 0x0, size 0x4
    unsigned int _M_mask; // offset 0x4, size 0x4
};
inline struct _Bit_reference * _Bit_reference::_Bit_reference(struct _Bit_reference * const this, unsigned int * __x, unsigned int __y) {}

inline struct _Bit_reference * _Bit_reference::_Bit_reference(struct _Bit_reference * const this) {}

inline bool _Bit_reference::__opb(const struct _Bit_reference * const this) {}

inline struct _Bit_reference & _Bit_reference::__as(struct _Bit_reference * const this, bool __x) {}

inline struct _Bit_reference & _Bit_reference::__as(struct _Bit_reference * const this, const struct _Bit_reference & __x) {}

inline bool _Bit_reference::__eq(const struct _Bit_reference * const this, const struct _Bit_reference & __x) {}

inline bool _Bit_reference::__lt(const struct _Bit_reference * const this, const struct _Bit_reference & __x) {}

inline void _Bit_reference::flip(struct _Bit_reference * const this) {}

inline void swap(struct _Bit_reference __x, struct _Bit_reference & __y) {
    // Local variables
    bool __tmp;
}

struct _Bit_iterator_base {
    // total size: 0x8
    unsigned int * _M_p; // offset 0x0, size 0x4
    unsigned int _M_offset; // offset 0x4, size 0x4
};
inline void _Bit_iterator_base::_M_bump_up(struct _Bit_iterator_base * const this) {}

inline void _Bit_iterator_base::_M_bump_down(struct _Bit_iterator_base * const this) {}

inline struct _Bit_iterator_base * _Bit_iterator_base::_Bit_iterator_base(struct _Bit_iterator_base * const this) {}

inline struct _Bit_iterator_base * _Bit_iterator_base::_Bit_iterator_base(struct _Bit_iterator_base * const this, unsigned int * __x, unsigned int __y) {}

inline void _Bit_iterator_base::_M_advance(struct _Bit_iterator_base * const this, int __i) {
    // Local variables
    int __n;
}

inline int _Bit_iterator_base::_M_subtract(const struct _Bit_iterator_base * const this, const struct _Bit_iterator_base & __x) {}

inline bool __eq(const struct _Bit_iterator_base & __x, const struct _Bit_iterator_base & __y) {}

inline bool __ne(const struct _Bit_iterator_base & __x, const struct _Bit_iterator_base & __y) {}

inline bool __lt(const struct _Bit_iterator_base & __x, const struct _Bit_iterator_base & __y) {}

inline bool __gt(const struct _Bit_iterator_base & __x, const struct _Bit_iterator_base & __y) {}

inline bool __le(const struct _Bit_iterator_base & __x, const struct _Bit_iterator_base & __y) {}

inline bool __ge(const struct _Bit_iterator_base & __x, const struct _Bit_iterator_base & __y) {}

inline bool MUTEX_create(struct MUTEX * m) {}

struct MUTEX {
    // total size: 0x1C
    int reserved[7]; // offset 0x0, size 0x1C
};
inline void MUTEX_destroy(struct MUTEX * m) {}

inline void MUTEX_lock(struct MUTEX * m) {}

inline void MUTEX_unlock(struct MUTEX * m) {}

inline int CPU_getcycle() {}

struct bNode {
    // total size: 0x8
    struct bNode * Next; // offset 0x0, size 0x4
    struct bNode * Prev; // offset 0x4, size 0x4
};
inline struct bNode * bNode::bNode(struct bNode * const this) {}

inline void bNode::bNode(struct bNode * const this, const int __in_chrg) {}

inline struct bNode * bNode::GetNext(struct bNode * const this) {}

inline struct bNode * bNode::GetPrev(struct bNode * const this) {}

inline struct bNode * bNode::AddBefore(struct bNode * const this, struct bNode * insert_point) {
    // Local variables
    struct bNode * new_prev;
    struct bNode * new_next;
}

inline struct bNode * bNode::AddAfter(struct bNode * const this, struct bNode * insert_point) {
    // Local variables
    struct bNode * new_next;
    struct bNode * new_prev;
}

inline struct bNode * bNode::Remove(struct bNode * const this) {
    // Local variables
    struct bNode * next_node;
    struct bNode * prev_node;
}

struct bList {
    // total size: 0x8
    struct bNode HeadNode; // offset 0x0, size 0x8
};
inline struct bList * bList::bList(struct bList * const this) {}

inline void bList::bList(struct bList * const this, const int __in_chrg) {}

inline void bList::InitList(struct bList * const this) {}

inline int bList::IsEmpty(struct bList * const this) {}

inline struct bNode * bList::EndOfList(struct bList * const this) {}

inline struct bNode * bList::GetHead(struct bList * const this) {}

inline struct bNode * bList::GetTail(struct bList * const this) {}

inline struct bNode * bList::AddHead(struct bList * const this, struct bNode * node) {}

inline struct bNode * bList::AddTail(struct bList * const this, struct bNode * node) {}

inline struct bNode * bList::Remove(struct bList * const this, struct bNode * node) {}

inline struct bNode * bList::RemoveHead(struct bList * const this) {}

inline int bList::IsInList(struct bList * const this, struct bNode * node) {}

struct bTNode<bPNode> : public bNode {
    // total size: 0x8
};
inline struct bTNode<bPNode> * bTNode<bPNode>::bTNode(struct bTNode<bPNode> * const this) {}

struct bPNode : public bTNode<bPNode> {
    // total size: 0xC
    void * Object; // offset 0x8, size 0x4
};
inline void bTNode<bPNode>::bTNode(struct bTNode<bPNode> * const this, const int __in_chrg) {}

inline struct bPNode * bPNode::bPNode(struct bPNode * const this, void * object) {}

inline void * bPNode::__nw(unsigned int size) {}

inline void bPNode::__dl(void * ptr) {}

struct AllocationHeader : public bTNode<AllocationHeader> {
    // total size: 0x14
    unsigned char PoolNum; // offset 0x8, size 0x1
    unsigned char MagicNumber; // offset 0x9, size 0x1
    unsigned short FrontPadding; // offset 0xA, size 0x2
    int Size; // offset 0xC, size 0x4
    int RequestedSize; // offset 0x10, size 0x4
};
inline void * AllocationHeader::GetBottomAddress(struct AllocationHeader * const this) {}

inline void * AllocationHeader::GetAllocAddress(struct AllocationHeader * const this) {}

inline const char * AllocationHeader::GetDebugText(struct AllocationHeader * const this) {}

struct MemoryPool {
    // total size: 0x60
    const char * pDebugName; // offset 0x0, size 0x4
    struct bTList<FreeBlock> FreeBlockList; // offset 0x4, size 0x8
    struct bTList<AllocationHeader> AllocationHeaderList; // offset 0xC, size 0x8
    int InitialAddress; // offset 0x14, size 0x4
    int InitialSize; // offset 0x18, size 0x4
    int NumAllocations; // offset 0x1C, size 0x4
    int TotalNumAllocations; // offset 0x20, size 0x4
    int PoolSize; // offset 0x24, size 0x4
    int AmountAllocated; // offset 0x28, size 0x4
    int MostAmountAllocated; // offset 0x2C, size 0x4
    int AmountFree; // offset 0x30, size 0x4
    int LeastAmountFree; // offset 0x34, size 0x4
    bool DebugFillEnabled; // offset 0x38, size 0x1
    bool DebugTracingEnabled; // offset 0x3C, size 0x1
    struct bMutex Mutex; // offset 0x40, size 0x20
};
inline const char * MemoryPool::GetName(struct MemoryPool * const this) {}

struct bTList<FreeBlock> : public bList {
    // total size: 0x8
};
struct bTList<AllocationHeader> : public bList {
    // total size: 0x8
};
struct bMutex {
    // total size: 0x20
    unsigned int MutexMemory[8]; // offset 0x0, size 0x20
};
struct FreeBlock : public bTNode<FreeBlock> {
    // total size: 0x10
    int Size; // offset 0x8, size 0x4
    int MagicNumber; // offset 0xC, size 0x4
};
inline bool MemoryPool::SetDebugFill(struct MemoryPool * const this, bool on_off) {
    // Local variables
    bool previous;
}

inline bool MemoryPool::SetDebugTracing(struct MemoryPool * const this, bool on_off) {
    // Local variables
    bool previous;
}

inline bool MemoryPool::IsInPool(struct MemoryPool * const this, int address) {}

inline struct AllocationHeader * bTList<AllocationHeader>::Remove(struct bTList<AllocationHeader> * const this, struct bNode * node) {}

inline void MemoryPool::RemoveAllocationHeader(struct MemoryPool * const this, struct AllocationHeader * allocation_header) {}

struct IAllocator {
    // total size: 0x4
    const struct __vtbl_ptr_type * _vptr.IAllocator; // offset 0x0, size 0x4
};
inline virtual void IAllocator::IAllocator(struct IAllocator * const this, const int __in_chrg) {}

struct bMemoryAllocator : public IAllocator {
    // total size: 0xC
    int mRefcount; // offset 0x4, size 0x4
    int PoolNumber; // offset 0x8, size 0x4
};
inline void bMemoryAllocator::SetMemoryPool(struct bMemoryAllocator * const this, int pool_number) {}

inline int bMemoryGetPoolNum(int allocation_params) {}

inline int bMemoryGetAlignment(int allocation_params) {
    // Local variables
    int alignment;
}

struct _type_vector {
    // total size: 0x1
};
inline const char * _type_vector::name(struct _type_vector * const this) {}

struct _type_list {
    // total size: 0x1
};
inline const char * _type_list::name(struct _type_list * const this) {}

struct _type_map {
    // total size: 0x1
};
inline const char * _type_map::name(struct _type_map * const this) {}

struct _type_set {
    // total size: 0x1
};
inline const char * _type_set::name(struct _type_set * const this) {}

struct _type_UComObject {
    // total size: 0x1
};
inline const char * _type_UComObject::name(struct _type_UComObject * const this) {}

struct Object {
    // total size: 0x10
    struct _IList _mInterfaces; // offset 0x0, size 0x10
};
inline struct _IPair * _IPair::_IPair(struct _IPair * const this, void * h, struct IUnknown * r) {}

struct vector<UTL::COM::Object::_IPair,_type_UComObject> : public vector<UTL::COM::Object::_IPair,UTL::Std::Allocator<UTL::COM::Object::_IPair,_type_UComObject> > {
    // total size: 0x10
};
struct _STLP_alloc_proxy<UTL::COM::Object::_IPair *,UTL::COM::Object::_IPair,UTL::Std::Allocator<UTL::COM::Object::_IPair,_type_UComObject> > : public Allocator<UTL::COM::Object::_IPair,_type_UComObject> {
    // total size: 0x8
    struct _IPair * _M_data; // offset 0x4, size 0x4
};
struct Allocator<UTL::COM::Object::_IPair,_type_UComObject> {
    // total size: 0x1
};
struct random_access_iterator_tag : public bidirectional_iterator_tag {
    // total size: 0x1
};
struct __false_type {
    // total size: 0x1
};
struct __true_type {
    // total size: 0x1
};
inline void * Object::__nw(unsigned int size) {}

inline struct Object * Object::Object(struct Object * const this, unsigned int icount) {}

inline void Object::Object(struct Object * const this, const int __in_chrg) {}

struct IUnknown {
    // total size: 0x8
    struct Object * _mCOMObject; // offset 0x0, size 0x4
    const struct __vtbl_ptr_type * _vptr.IUnknown; // offset 0x4, size 0x4
};
inline struct IUnknown * IUnknown::IUnknown(struct IUnknown * const this, struct Object * owner, void * handle) {}

inline virtual void IUnknown::IUnknown(struct IUnknown * const this, const int __in_chrg) {}

inline bool ComparePtr(const struct IUnknown * pUnk1, const struct IUnknown * pUnk2) {}

struct AttribAlloc {
    // total size: 0x1
};
inline void AttribAlloc::Free(void * ptr, unsigned int bytes, const char * name) {}

struct IAttribAllocator {
    // total size: 0x4
    const struct __vtbl_ptr_type * _vptr.IAttribAllocator; // offset 0x0, size 0x4
};
inline void Free(void * ptr, unsigned int bytes, const char * name) {}

struct Collection {
    // total size: 0x2C
    struct HashMap mTable; // offset 0x0, size 0x10
    const struct Collection * mParent; // offset 0x10, size 0x4
    struct Class * mClass; // offset 0x14, size 0x4
    void * mLayout; // offset 0x18, size 0x4
    unsigned int mRefCount; // offset 0x1C, size 0x4
    unsigned int mKey; // offset 0x20, size 0x4
    struct Vault * mSource; // offset 0x24, size 0x4
    const char * mNamePtr; // offset 0x28, size 0x4
};
inline void Collection::AddRef(const struct Collection * const this) {}

struct RefSpec {
    // total size: 0xC
    unsigned int mClassKey; // offset 0x0, size 0x4
    unsigned int mCollectionKey; // offset 0x4, size 0x4
    const struct Collection * mCollectionPtr; // offset 0x8, size 0x4
};
inline void RefSpec::__dl(void * ptr, unsigned int bytes) {}

inline struct RefSpec * RefSpec::RefSpec(struct RefSpec * const this) {}

inline void RefSpec::RefSpec(struct RefSpec * const this, const int __in_chrg) {}

inline unsigned int RefSpec::GetClassKey(const struct RefSpec * const this) {}

inline unsigned int RefSpec::GetCollectionKey(const struct RefSpec * const this) {}

struct Attribute {
    // total size: 0x10
    const struct Instance * mInstance; // offset 0x0, size 0x4
    const struct Collection * mCollection; // offset 0x4, size 0x4
    struct Node * mInternal; // offset 0x8, size 0x4
    void * mDataPointer; // offset 0xC, size 0x4
};
inline void * Attribute::GetElementPointer(const struct Attribute * const this, unsigned int index) {}

inline unsigned int Attribute::GetCollectionKey(const struct Attribute * const this) {}

inline const void * Attribute::GetDataAddress(const struct Attribute * const this) {}

inline struct Node * Attribute::GetInternal(const struct Attribute * const this) {}

inline bool Attribute::Get(struct Attribute * const this, unsigned int index, struct RefSpec & result) {
    // Local variables
    const struct RefSpec * resultptr;
}

struct Instance {
    // total size: 0x14
    struct IUnknown * mOwner; // offset 0x0, size 0x4
    const struct Collection * mCollection; // offset 0x4, size 0x4
    void * mLayoutPtr; // offset 0x8, size 0x4
    unsigned int mMsgPort; // offset 0xC, size 0x4
    unsigned short mFlags; // offset 0x10, size 0x2
    unsigned short mLocks; // offset 0x12, size 0x2
};
inline void * Instance::GetLayoutPointer(const struct Instance * const this) {}

inline void * Instance::GetLayoutPointer(struct Instance * const this) {}

inline const struct Collection * Instance::GetConstCollection(const struct Instance * const this) {}

inline void Instance::SetDefaultLayout(struct Instance * const this, unsigned int bytes) {}

inline bool Instance::IsValid(const struct Instance * const this) {}

inline const struct GenericAccessor * Instance::__rf(const struct Instance * const this) {}

struct Definition {
    // total size: 0x10
    unsigned int mKey; // offset 0x0, size 0x4
    unsigned int mType; // offset 0x4, size 0x4
    unsigned short mOffset; // offset 0x8, size 0x2
    unsigned short mSize; // offset 0xA, size 0x2
    unsigned short mMaxCount; // offset 0xC, size 0x2
    unsigned char mFlags; // offset 0xE, size 0x1
    unsigned char mAlignment; // offset 0xF, size 0x1
};
inline struct Definition * Definition::Definition(struct Definition * const this, unsigned int k) {}

inline unsigned int Definition::GetKey(const struct Definition * const this) {}

inline bool Definition::__lt(const struct Definition * const this, const struct Definition & rhs) {}

struct Class {
    // total size: 0xC
    unsigned int mKey; // offset 0x0, size 0x4
    unsigned int mRefCount; // offset 0x4, size 0x4
    struct ClassPrivate & mPrivates; // offset 0x8, size 0x4
};
inline void Class::__dl(void * ptr, unsigned int bytes) {}

struct Database {
    // total size: 0x4
    struct DatabasePrivate & mPrivates; // offset 0x0, size 0x4
};
inline struct Database & Database::Get() {}

inline bool Database::IsInitialized(struct Database * const this) {}

// Range: 0x0 -> 0x4C
struct Class * Class::Class(struct Class * const this /* r30 */, unsigned int k /* r4 */, struct ClassPrivate & privates /* r5 */) {
    // Inlines
    // Range: 0x10 -> 0x10
    inline struct Database & Database::Get() {}
}

// Range: 0x4C -> 0xD0
void Class::Class(struct Class * const this /* r31 */, const int __in_chrg /* r30 */) {
    // Inlines
    // Range: 0x5C -> 0x5C
    inline struct Database & Database::Get() {}
    // Range: 0x7C -> 0xBC
    inline void Class::__dl(void * ptr, unsigned int bytes, void * ptr /* r31 */, unsigned int bytes) {
        // Inlines
        // Range: 0x7C -> 0xBC
        inline void Free(void * ptr, unsigned int bytes, const char * name, const char * name, unsigned int bytes, void * ptr /* r31 */) {
            // Inlines
            // Range: 0x7C -> 0xBC
            inline void AttribAlloc::Free(void * ptr, unsigned int bytes, const char * name, void * ptr /* r31 */, unsigned int bytes, const char * name) {}
        }
    }
}

inline const struct Definition * lower_bound(const struct Definition * __first, const struct Definition * __last, const struct Definition & __val) {}

// Range: 0xD0 -> 0x134
const struct Definition * Class::GetDefinition(const struct Class * const this /* r3 */, unsigned int key /* r4 */) {
    // Local variables
    struct Definition target; // r1+0x8
    const struct Definition * b; // r3
    const struct Definition * e; // r30
    const struct Definition * result;

    // Inlines
    // Range: 0xE0 -> 0xE0
    inline struct Definition * Definition::Definition(struct Definition * const this, unsigned int k, struct Definition * const this, unsigned int k) {}
    // Range: 0xE0 -> 0xE0
    inline const struct Definition * lower_bound(const struct Definition * __first, const struct Definition * __last, const struct Definition & __val, const struct Definition * __first /* r3 */, const struct Definition * __last /* r30 */, const struct Definition & __val) {}
}

// Range: 0x134 -> 0x140
unsigned int Class::GetNumDefinitions(const struct Class * const this /* r3 */) {}

// Range: 0x140 -> 0x164
unsigned int Class::GetFirstDefinition(const struct Class * const this /* r3 */) {
    // Inlines
    // Range: 0x150 -> 0x150
    inline unsigned int Definition::GetKey(const struct Definition * const this, const struct Definition * const this /* r9 */) {}
}

// Range: 0x164 -> 0x1F0
unsigned int Class::GetNextDefinition(const struct Class * const this /* r29 */, unsigned int prev /* r4 */) {
    // Local variables
    struct Definition prevTarget; // r1+0x8
    const struct Definition * b; // r30
    const struct Definition * e;
    const struct Definition * result;
    unsigned int index; // r3

    // Inlines
    // Range: 0x174 -> 0x174
    inline struct Definition * Definition::Definition(struct Definition * const this, unsigned int k, struct Definition * const this, unsigned int k) {}
    // Range: 0x174 -> 0x174
    inline const struct Definition * lower_bound(const struct Definition * __first, const struct Definition * __last, const struct Definition & __val, const struct Definition * __first /* r30 */, const struct Definition * __last, const struct Definition & __val) {}
    // Range: 0x1C8 -> 0x1C8
    inline unsigned int Definition::GetKey(const struct Definition * const this, const struct Definition * const this) {}
}

struct VecHashMap<unsigned int,Attrib::Collection,Attrib::Class::TablePolicy,true,40> {
    // total size: 0x10
    struct Node * mTable; // offset 0x0, size 0x4
    unsigned int mTableSize; // offset 0x4, size 0x4
    unsigned int mNumEntries; // offset 0x8, size 0x4
    unsigned int mFixedAlloc : 1; // offset 0xC, size 0x4
    unsigned int mWorstCollision : 31; // offset 0xC, size 0x4
};
inline unsigned int VecHashMap<unsigned int,Attrib::Collection,Attrib::Class::TablePolicy,true,40>::Size(const struct VecHashMap<unsigned int,Attrib::Collection,Attrib::Class::TablePolicy,true,40> * const this) {}

// Range: 0x1F0 -> 0x1FC
unsigned int Class::GetNumCollections(const struct Class * const this /* r3 */) {
    // Inlines
    // Range: 0x1F0 -> 0x1F0
    inline unsigned int VecHashMap<unsigned int,Attrib::Collection,Attrib::Class::TablePolicy,true,40>::Size(const struct VecHashMap<unsigned int,Attrib::Collection,Attrib::Class::TablePolicy,true,40> * const this, const struct VecHashMap<unsigned int,Attrib::Collection,Attrib::Class::TablePolicy,true,40> * const this) {}
}

// Range: 0x1FC -> 0x228
unsigned int Class::GetFirstCollection(const struct Class * const this /* r3 */) {}

// Range: 0x228 -> 0x230
unsigned int Class::GetTableNodeSize(const struct Class * const this /* r3 */) {}

// Range: 0x230 -> 0x264
void Class::CopyLayout(const struct Class * const this /* r3 */, void * srcLayout /* r4 */, void * dstLayout /* r3 */) {}

// Range: 0x264 -> 0x2DC
void Class::FreeLayout(const struct Class * const this /* r31 */, void * layout /* r30 */) {
    // Inlines
    // Range: 0x2A0 -> 0x2C8
    inline void Free(void * ptr, unsigned int bytes, const char * name, void * ptr /* r30 */, unsigned int bytes, const char * name) {
        // Inlines
        // Range: 0x2A0 -> 0x2C8
        inline void AttribAlloc::Free(void * ptr, unsigned int bytes, const char * name, const char * name, unsigned int bytes, void * ptr /* r30 */) {}
    }
}

// Range: 0x2DC -> 0x344
struct Attribute Collection::Get(const struct Collection * const this /* r4 */, const struct Instance & instance /* r30 */, unsigned int attributeKey /* r6 */) {
    // Local variables
    const struct Collection * c; // r1+0x8
    struct Node * node; // r3
}

// Range: 0x344 -> 0x474
void * Collection::GetData(const struct Collection * const this /* r3 */, unsigned int attributeKey /* r4 */, unsigned int index /* r30 */) {
    // Local variables
    const struct Collection * c; // r1+0x8
    struct Node * node; // r3

    // Inlines
    // Range: 0x368 -> 0x37C
    inline bool Node::IsArray(struct Node * const this, struct Node * const this /* r3 */) {
        // Inlines
        // Range: 0x368 -> 0x37C
        inline bool Node::GetFlag(struct Node * const this, unsigned int mask, unsigned int mask, struct Node * const this /* r3 */) {}
    }
    // Range: 0x384 -> 0x3B4
    inline struct Array * Node::GetArray(struct Node * const this, void * layoutptr, struct Node * const this /* r3 */, void * layoutptr /* r9 */) {
        // Inlines
        // Range: 0x384 -> 0x39C
        inline bool Node::IsLaidOut(struct Node * const this, struct Node * const this /* r3 */) {
            // Inlines
            // Range: 0x384 -> 0x39C
            inline bool Node::GetFlag(struct Node * const this, unsigned int mask, struct Node * const this /* r3 */, unsigned int mask) {}
        }
    }
    // Range: 0x3B4 -> 0x404
    inline void * Array::GetData(struct Array * const this, unsigned int index, struct Array * const this, unsigned int index /* r30 */) {
        // Inlines
        // Range: 0x3CC -> 0x3CC
        inline void * Array::Data(struct Array * const this, unsigned int byteindex, unsigned int byteindex /* r0 */, struct Array * const this) {
            // Local variables
            unsigned char * base;

            // Inlines
            // Range: 0x3CC -> 0x3CC
            inline unsigned char * Array::BasePointer(struct Array * const this, struct Array * const this) {}
        }
        // Range: 0x3E8 -> 0x3E8
        inline void * Array::Data(struct Array * const this, unsigned int byteindex, unsigned int byteindex /* r0 */, struct Array * const this) {
            // Local variables
            unsigned char * base;

            // Inlines
            // Range: 0x3E8 -> 0x3E8
            inline unsigned char * Array::BasePointer(struct Array * const this, struct Array * const this) {}
        }
    }
    // Range: 0x408 -> 0x458
    inline void * Node::GetPointer(struct Node * const this, void * layoutptr, struct Node * const this /* r3 */, void * layoutptr /* r9 */) {
        // Inlines
        // Range: 0x408 -> 0x420
        inline bool Node::IsByValue(struct Node * const this, struct Node * const this /* r3 */) {
            // Inlines
            // Range: 0x408 -> 0x420
            inline bool Node::GetFlag(struct Node * const this, unsigned int mask, struct Node * const this /* r3 */, unsigned int mask) {}
        }
        // Range: 0x430 -> 0x440
        inline bool Node::IsLaidOut(struct Node * const this, struct Node * const this /* r3 */) {
            // Inlines
            // Range: 0x430 -> 0x440
            inline bool Node::GetFlag(struct Node * const this, unsigned int mask, struct Node * const this /* r3 */, unsigned int mask) {}
        }
    }
}

// Range: 0x474 -> 0x48C
unsigned int Collection::Count(const struct Collection * const this /* r3 */) {
    // Inlines
    // Range: 0x474 -> 0x474
    inline unsigned int HashMap::Size(const struct HashMap * const this, const struct HashMap * const this /* r3 */) {}
    // Range: 0x474 -> 0x474
    inline unsigned int HashMap::Size(const struct HashMap * const this, const struct HashMap * const this) {}
}

// Range: 0x48C -> 0x4EC
unsigned int Collection::FirstKey(const struct Collection * const this /* r31 */, bool & inLayout /* r30 */) {
    // Local variables
    unsigned int k; // r3
}

inline int __distance(const struct Definition * const & __first, const struct Definition * const & __last) {}

inline int distance(const struct Definition * const & __first, const struct Definition * const & __last) {}

inline void __advance(const struct Definition * & __i, int __n) {}

inline void advance(const struct Definition * & __i, int __n) {}

struct less<Attrib::Definition> : public binary_function<Attrib::Definition,Attrib::Definition,bool> {
    // total size: 0x1
};
inline bool less<Attrib::Definition>::__cl(const struct less<Attrib::Definition> * const this, const struct Definition & __x, const struct Definition & __y) {}

// Range: 0x4EC -> 0x534
const struct Definition * __lower_bound(const struct Definition * __first /* r3 */, const struct Definition * __last, const struct Definition & __val /* r5 */, struct less<Attrib::Definition> __comp) {
    // Local variables
    int __len; // r11
    int __half; // r4
    const struct Definition * __middle;

    // Inlines
    // Range: 0x4EC -> 0x4EC
    inline int distance(const struct Definition * const & __first, const struct Definition * const & __last, const struct Definition * const & __first, const struct Definition * const & __last) {
        // Inlines
        // Range: 0x4EC -> 0x4EC
        inline int __distance(const struct Definition * const & __first, const struct Definition * const & __last, const struct Definition * const & __last, const struct Definition * const & __first) {}
    }
    // Range: 0x4FC -> 0x4FC
    inline void advance(const struct Definition * & __i, int __n, const struct Definition * & __i, int __n /* r4 */) {
        // Inlines
        // Range: 0x4FC -> 0x4FC
        inline void __advance(const struct Definition * & __i, int __n, int __n /* r4 */, const struct Definition * & __i) {}
    }
    // Range: 0x4FC -> 0x4FC
    inline bool less<Attrib::Definition>::__cl(const struct less<Attrib::Definition> * const this, const struct Definition & __x, const struct Definition & __y, const struct less<Attrib::Definition> * const this, const struct Definition & __x, const struct Definition & __y /* r5 */) {
        // Inlines
        // Range: 0x4FC -> 0x4FC
        inline bool Definition::__lt(const struct Definition * const this, const struct Definition & rhs, const struct Definition & rhs /* r5 */, const struct Definition * const this) {}
    }
}

struct TagValuePair {
    // total size: 0xC
    unsigned int mTag; // offset 0x0, size 0x4
    union {
        int mInt; // offset 0x0, size 0x4
        unsigned int mSize; // offset 0x0, size 0x4
        float mFloat; // offset 0x0, size 0x4
        const void * mPointer; // offset 0x0, size 0x4
    } mValue; // offset 0x4, size 0x4
    const struct TagValuePair * mNext; // offset 0x8, size 0x4
};
struct Mutex {
    // total size: 0x1C
    char mBuf[28]; // offset 0x0, size 0x1C
};
typedef unsigned int Key;
struct Vault {
};
struct DatabasePrivate {
};
struct ClassPrivate : public Class {
    // total size: 0x3C
    struct HashMap mLayoutTable; // offset 0xC, size 0x10
    struct CollectionHashMap mCollections; // offset 0x1C, size 0x10
    unsigned short mLayoutSize; // offset 0x2C, size 0x2
    unsigned short mNumDefinitions; // offset 0x2E, size 0x2
    struct Definition * mDefinitions; // offset 0x30, size 0x4
    struct Vault * mSource; // offset 0x34, size 0x4
    const char * mNamePtr; // offset 0x38, size 0x4
};
struct Node {
};
struct Private {
    // total size: 0x8
    unsigned char mData[8]; // offset 0x0, size 0x8
};
struct ClassLoadData {
};
struct GenericAccessor {
};
struct output_iterator_tag {
    // total size: 0x1
};
struct output_iterator : public iterator<_STL::output_iterator_tag,void,void,void,void> {
    // total size: 0x1
};
typedef void (* __oom_handler_type)();
struct _Node_alloc_obj {
    // total size: 0x4
    struct _Node_alloc_obj * _M_free_list_link; // offset 0x0, size 0x4
};
struct __node_alloc<false,0> {
    // total size: 0x1
};
typedef struct __node_alloc<false,0> _Node_alloc;
typedef _Node_alloc __sgi_alloc;
typedef _Node_alloc __single_client_alloc;
struct __node_alloc<true,0> {
};
typedef struct __node_alloc<true,0> __multithreaded_alloc;
struct _List_global<bool> {
    // total size: 0x1
};
typedef struct _List_global<bool> _List_global_inst;
typedef bool _Rb_tree_Color_type;
struct _Rb_global<bool> {
    // total size: 0x1
};
typedef struct _Rb_global<bool> _Rb_global_inst;
struct _Bit_iter<bool,const bool *> {
};
typedef struct _Bit_iter<bool,const bool *> _Bit_const_iterator;
struct _Bit_iter<_STL::_Bit_reference,_STL::_Bit_reference *> : public _Bit_iterator_base {
    // total size: 0x8
};
typedef struct _Bit_iter<_STL::_Bit_reference,_STL::_Bit_reference *> _Bit_iterator;
struct vector<bool,_STL::allocator<bool> > {
};
typedef struct vector<bool,_STL::allocator<bool> > bit_vector;
static const int __stl_chunk_size; // size: 0x4
struct IAttribAllocator * mAllocator; // size: 0x4
struct Database * sThis; // size: 0x4
typedef const struct __vtbl_ptr_type __vtbl_ptr_type;
typedef int ptrdiff_t;
typedef unsigned int size_t;
typedef unsigned int wint_t;
typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;
typedef volatile unsigned long long vu64;
typedef volatile signed char vs8;
typedef volatile short vs16;
typedef volatile long vs32;
typedef volatile long long vs64;
typedef float f32;
typedef double f64;
typedef volatile float vf32;
typedef volatile double vf64;
typedef int BOOL;
typedef unsigned char Bool;
typedef unsigned int uintptr_t;
typedef int intptr_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;
typedef unsigned int clock_t;
typedef int time_t;
struct tm {
    // total size: 0x24
    int tm_sec; // offset 0x0, size 0x4
    int tm_min; // offset 0x4, size 0x4
    int tm_hour; // offset 0x8, size 0x4
    int tm_mday; // offset 0xC, size 0x4
    int tm_mon; // offset 0x10, size 0x4
    int tm_year; // offset 0x14, size 0x4
    int tm_wday; // offset 0x18, size 0x4
    int tm_yday; // offset 0x1C, size 0x4
    int tm_isdst; // offset 0x20, size 0x4
};
typedef unsigned int ULong;
struct _glue {
    // total size: 0xC
    struct _glue * _next; // offset 0x0, size 0x4
    int _niobs; // offset 0x4, size 0x4
    struct __sFILE * _iobs; // offset 0x8, size 0x4
};
struct __sFILE {
    // total size: 0x60
    unsigned char * _p; // offset 0x0, size 0x4
    int _r; // offset 0x4, size 0x4
    int _w; // offset 0x8, size 0x4
    short _flags; // offset 0xC, size 0x2
    short _file; // offset 0xE, size 0x2
    struct __sbuf _bf; // offset 0x10, size 0x8
    int _lbfsize; // offset 0x18, size 0x4
    void * _cookie; // offset 0x1C, size 0x4
    int (* _read)(void *, char *, int); // offset 0x20, size 0x4
    int (* _write)(void *, const char *, int); // offset 0x24, size 0x4
    int (* _seek)(void *, long, int); // offset 0x28, size 0x4
    int (* _close)(void *); // offset 0x2C, size 0x4
    struct __sbuf _ub; // offset 0x30, size 0x8
    unsigned char * _up; // offset 0x38, size 0x4
    int _ur; // offset 0x3C, size 0x4
    unsigned char _ubuf[10]; // offset 0x40, size 0xA
    unsigned char _nbuf[1]; // offset 0x4A, size 0x1
    struct __sbuf _lb; // offset 0x4C, size 0x8
    int _blksize; // offset 0x54, size 0x4
    int _offset; // offset 0x58, size 0x4
    struct _reent * _data; // offset 0x5C, size 0x4
};
struct __sbuf {
    // total size: 0x8
    unsigned char * _base; // offset 0x0, size 0x4
    int _size; // offset 0x4, size 0x4
};
struct _reent {
    // total size: 0x304
    int _errno; // offset 0x0, size 0x4
    struct __sFILE * _stdin; // offset 0x4, size 0x4
    struct __sFILE * _stdout; // offset 0x8, size 0x4
    struct __sFILE * _stderr; // offset 0xC, size 0x4
    int _inc; // offset 0x10, size 0x4
    char _emergency[25]; // offset 0x14, size 0x19
    int _current_category; // offset 0x30, size 0x4
    const char * _current_locale; // offset 0x34, size 0x4
    int __sdidinit; // offset 0x38, size 0x4
    void (* __cleanup)(struct _reent *); // offset 0x3C, size 0x4
    struct _Bigint * _result; // offset 0x40, size 0x4
    int _result_k; // offset 0x44, size 0x4
    struct _Bigint * _p5s; // offset 0x48, size 0x4
    struct _Bigint * * _freelist; // offset 0x4C, size 0x4
    int _cvtlen; // offset 0x50, size 0x4
    char * _cvtbuf; // offset 0x54, size 0x4
    union {
        struct {
            // total size: 0x4C
            unsigned int _rand_next; // offset 0x0, size 0x4
            char * _strtok_last; // offset 0x4, size 0x4
            char _asctime_buf[26]; // offset 0x8, size 0x1A
            struct tm _localtime_buf; // offset 0x24, size 0x24
            int _gamma_signgam; // offset 0x48, size 0x4
        } _reent; // offset 0x0, size 0x4C
        struct {
            // total size: 0xF0
            unsigned char * _nextf[30]; // offset 0x0, size 0x78
            unsigned int _nmalloc[30]; // offset 0x78, size 0x78
        } _unused; // offset 0x0, size 0xF0
    } _new; // offset 0x58, size 0xF0
    struct _atexit * _atexit; // offset 0x148, size 0x4
    struct _atexit _atexit0; // offset 0x14C, size 0x88
    void (* * _sig_func)(int); // offset 0x1D4, size 0x4
    struct _glue __sglue; // offset 0x1D8, size 0xC
    struct __sFILE __sf[3]; // offset 0x1E4, size 0x120
};
struct _Bigint {
    // total size: 0x18
    struct _Bigint * _next; // offset 0x0, size 0x4
    int _k; // offset 0x4, size 0x4
    int _maxwds; // offset 0x8, size 0x4
    int _sign; // offset 0xC, size 0x4
    int _wds; // offset 0x10, size 0x4
    unsigned int _x[1]; // offset 0x14, size 0x4
};
struct _atexit {
    // total size: 0x88
    struct _atexit * _next; // offset 0x0, size 0x4
    int _ind; // offset 0x4, size 0x4
    void (* _fns[32])(); // offset 0x8, size 0x80
};
typedef int _fpos_t;
struct div_t {
    // total size: 0x8
    int quot; // offset 0x0, size 0x4
    int rem; // offset 0x4, size 0x4
};
typedef struct div_t div_t;
typedef struct ldiv_t ldiv_t;
struct __va_list_tag {
    // total size: 0xC
    char gpr; // offset 0x0, size 0x1
    char fpr; // offset 0x1, size 0x1
    char * overflow_arg_area; // offset 0x4, size 0x4
    char * reg_save_area; // offset 0x8, size 0x4
};
typedef struct __va_list_tag __va_list[1];
typedef __va_list __gnuc_va_list;
typedef int fpos_t;
typedef struct __sFILE FILE;
typedef void (* terminate_handler)();
typedef void (* unexpected_handler)();
struct nothrow_t {
    // total size: 0x1
};
typedef void (* new_handler)();
typedef unsigned int __stl_atomic_t;
struct MemoryPoolOverrideInfo {
    // total size: 0x20
    const char * Name; // offset 0x0, size 0x4
    void * Pool; // offset 0x4, size 0x4
    int Address; // offset 0x8, size 0x4
    int Size; // offset 0xC, size 0x4
    void * (* Malloc)(void *, int, const char *, int, int); // offset 0x10, size 0x4
    void (* Free)(void *, void *); // offset 0x14, size 0x4
    int (* GetAmountFree)(void *); // offset 0x18, size 0x4
    int (* GetLargestFreeBlock)(void *); // offset 0x1C, size 0x4
};
struct MemoryPoolInfo {
    // total size: 0x10
    bool NumberReserved; // offset 0x0, size 0x1
    bool TopMeansLargerAddress; // offset 0x4, size 0x1
    int OverflowPoolNumber; // offset 0x8, size 0x4
    struct MemoryPoolOverrideInfo * OverrideInfo; // offset 0xC, size 0x4
};
struct bMemoryTraceNewPoolPacket {
    // total size: 0x24
    unsigned int PoolID; // offset 0x0, size 0x4
    char Name[32]; // offset 0x4, size 0x20
};
struct bMemoryTraceDeletePoolPacket {
    // total size: 0x4
    unsigned int PoolID; // offset 0x0, size 0x4
};
struct bMemoryTraceFreePacket {
    // total size: 0xC
    unsigned int PoolID; // offset 0x0, size 0x4
    unsigned int MemoryAddress; // offset 0x4, size 0x4
    int Size; // offset 0x8, size 0x4
};
struct bVirtualMemoryManager {
    // total size: 0x18
    unsigned int mVirtualBaseAddr; // offset 0x0, size 0x4
    unsigned int mARamBaseAddr; // offset 0x4, size 0x4
    unsigned int mARamSize; // offset 0x8, size 0x4
    unsigned int mMRamBaseAddr; // offset 0xC, size 0x4
    unsigned int mMRamSize; // offset 0x10, size 0x4
    unsigned int bIsValid; // offset 0x14, size 0x4
};
struct AllocDesc {
    // total size: 0x10
    unsigned int mIndex; // offset 0x0, size 0x4
    const char * mName; // offset 0x4, size 0x4
    unsigned int mCount; // offset 0x8, size 0x4
    unsigned int mHigh; // offset 0xC, size 0x4
};
struct FastMem {
    // total size: 0x32C
    struct FreeBlock * mFreeLists[64]; // offset 0x0, size 0x100
    const char * mName; // offset 0x100, size 0x4
    unsigned int mExpansionSize; // offset 0x104, size 0x4
    unsigned int mLocks; // offset 0x108, size 0x4
    bool mInited; // offset 0x10C, size 0x1
    void * mBlock; // offset 0x110, size 0x4
    unsigned int mBytes; // offset 0x114, size 0x4
    unsigned int mUsed; // offset 0x118, size 0x4
    unsigned int mAlloc[64]; // offset 0x11C, size 0x100
    unsigned int mAvail[64]; // offset 0x21C, size 0x100
    unsigned int mAllocOver; // offset 0x31C, size 0x4
    struct AllocDesc * mTrack; // offset 0x320, size 0x4
    unsigned int mTrackMax; // offset 0x324, size 0x4
    unsigned int mTrackCount; // offset 0x328, size 0x4
};
struct FastMem gFastMem; // size: 0x32C
struct MemoryPool * MemoryPools[16]; // size: 0x40
typedef void * HINTERFACE;
struct reverse_iterator<UTL::COM::Object::_IPair *> {
};
struct reverse_iterator<const UTL::COM::Object::_IPair *> {
};
struct iterator {
};
struct iterator {
};
