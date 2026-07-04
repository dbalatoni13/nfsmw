#ifndef EATHREAD_EATHREAD_ATOMIC_POWERPC_H
#define EATHREAD_EATHREAD_ATOMIC_POWERPC_H

template<typename T>
class AtomicInt {
public:
    typedef T ValueType;

protected:
    ValueType mValue; // offset 0x0, size 0x4

public:
    AtomicInt() {}

    ValueType GetValue() const {}

    ValueType GetValueRaw() const {}

    ValueType SetValue(ValueType n) {}

    bool SetValueConditional(ValueType n, ValueType condition) {}

    ValueType Increment() {}

    ValueType Decrement() {}

    ValueType Add(ValueType n) {}

    AtomicInt<ValueType> &operator++() {}

    struct AtomicInt<ValueType> &operator--() {}

    AtomicInt(ValueType n) {
        this->mValue = n;
    }

    struct AtomicInt<ValueType> &operator=(ValueType n) {}

    operator ValueType const() const {}
};

typedef struct AtomicInt<int> AtomicInt32;
typedef struct AtomicInt<unsigned int> AtomicUint32;
typedef struct AtomicInt<long long int> AtomicInt64;
typedef struct AtomicInt<long long unsigned int> AtomicUint64;
typedef AtomicInt32 AtomicIntPtr;

#endif
