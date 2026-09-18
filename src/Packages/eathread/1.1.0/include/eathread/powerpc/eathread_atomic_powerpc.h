#ifndef EATHREAD_POWERPC_EATHREAD_ATOMIC_POWERPC_H
#define EATHREAD_POWERPC_EATHREAD_ATOMIC_POWERPC_H

template<typename T>
class AtomicInt {
public:
    typedef T ValueType;

public:
    AtomicInt() {}

    AtomicInt(ValueType n) {
        this->mValue = n;
    }

    ValueType GetValue() const {}

    ValueType GetValueRaw() const {}

    ValueType SetValue(ValueType n) {}

    bool SetValueConditional(ValueType n, ValueType condition) {}

    ValueType Increment() {}

    ValueType Decrement() {}

    ValueType Add(ValueType n) {}

    operator ValueType const() const {}

    AtomicInt<ValueType> &operator++() {}

    AtomicInt<ValueType> &operator--() {}

    AtomicInt<ValueType> &operator=(ValueType n) {}

protected:
    volatile ValueType mValue; // offset 0x0, size 0x4
};

typedef AtomicInt<int> AtomicInt32;
typedef AtomicInt<unsigned int> AtomicUint32;
typedef AtomicInt<long long> AtomicInt64;
typedef AtomicInt<unsigned long long> AtomicUint64;
typedef AtomicInt32 AtomicIntPtr;

#endif
