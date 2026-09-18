#ifndef UTL_VECTOR_COLLECTIONS_H
#define UTL_VECTOR_COLLECTIONS_H

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include <cstddef>

static const int DEFAULT_VECTOR_ALIGNMENT = 16;

namespace UTL {

template <typename T> class RepeatSequencer {
  public:
    explicit RepeatSequencer(const T &t)
        : mValue(&t) {}

    const T &operator[](unsigned int) const {
        return *mValue;
    }

  private:
    const T *mValue;
};

// total size: 0x10
template <typename T, int Alignment = DEFAULT_VECTOR_ALIGNMENT> class Vector {
  public:
    typedef T value_type;
    typedef value_type *pointer;
    typedef value_type &reference;
    typedef value_type *iterator;
    typedef value_type *reverse_iterator;
    typedef const value_type *const_pointer;
    typedef const value_type &const_reference;
    typedef const value_type *const_iterator;
    typedef const value_type *const_reverse_iterator;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

  public:
    void Init() {
        mBegin = nullptr;
        mCapacity = 0;
        mSize = 0;
    }

    Vector() {
        Init();
    }

    virtual ~Vector() {}

    size_type size() const {
        return mSize;
    }

    size_type capacity() const {
        return mCapacity;
    }

    const_iterator begin() const {
        return mBegin;
    }

    iterator begin() {
        return mBegin;
    }

    const_iterator end() const {
        return mBegin + mSize;
    }

    iterator end() {
        return mBegin + mSize;
    }

    reference operator[](size_type idx) {
        return mBegin[idx];
    }

    const_reference operator[](size_type idx) const {
        return mBegin[idx];
    }

    void push_back(value_type const &val) {
        if (size() >= capacity()) {
            reserve(GetGrowSize(size() + 1));
        }
        new (&mBegin[size()]) T(val);
        mSize++;
    }

    void push_back() {
        if (size() >= capacity()) {
            reserve(GetGrowSize(size() + 1));
        }
        new (&mBegin[size()]) T();
        mSize++;
    }

    void resize(size_type num) {
        if (num > size()) {
            reserve(num);
            while (size() < num) {
                push_back();
            }
        } else {
            while (size() > num) {
                pop_back();
            }
        }
    }

    void assign(const Vector &src) {
        assign(src.begin(), src.end());
    }

    void assign(const_iterator srcBeg, const_iterator srcEnd) {
        size_type minSize = srcEnd - srcBeg;
        const_iterator srcIt = srcBeg;
        if (srcIt == 0) {
            // El objetivo emite aqui `reserve` + un bucle de CRECIMIENTO con
            // push_back() -- leido del desensamblado de
            // `_Storage<CameraAI::Director*,2>::_Storage(const&)` en zCamera
            // (0x80081938..0x80081A44): sube el 59,56% -> 96,16%. El
            // `push_back()` sin argumento instancia `new (p) T()`, asi que
            // `GarbageNode<T,N>::Collector::_Node` necesita su constructor por
            // defecto (anadido en UCollections.h). Compilado zSim, zPhysics,
            // zMain y zFe tras el cambio.
            // r51-cam: la condicion del bucle NO puede llamar a `size()`.
            // El mapa de lineas del objetivo tiene `UTLVector.h:249` DOS veces
            // --delante del bucle y al fondo, con un `mr r4,r9` de mas--: es
            // `jump.c::duplicate_loop_exit_test`, y solo dispara si
            // `stmt.c::expand_end_loop` roto antes el bucle. A -O1 eso no ocurre
            // si la condicion lleva una llamada inline, porque emite
            // NOTE_INSN_BLOCK_BEG y el barrido se corta en
            // `if (optimize < 2 && ...) break;`. Con `mSize` a pelo si rota, y
            // los dos racimos estructurales de `_Storage` se cierran.
            // Radio medido: once SourceLists con y sin, +0 B en las once, y los
            // once objetos distintos por md5 (el cambio llega y no cuesta).
            if (minSize > size()) {
                reserve(minSize);
                while (minSize > mSize) {
                    push_back();
                }
            }
            return;
        }
        reserve(minSize);
        iterator destIt = begin();
        while (destIt != end() && srcIt != srcEnd) {
            reference dest = *destIt++;
            const_reference src = *srcIt++;
            dest = src;
        }
        while (end() != destIt) {
            pop_back();
        }
        while (srcIt != srcEnd) {
            push_back(*srcIt++);
        }
    }

    iterator insert(iterator position, const_reference val) {
        size_type posIndex = position - mBegin;
        RepeatSequencer<const value_type> sequencer(val);
        insert_sequence(position, 1, sequencer);
        return mBegin + posIndex;
    }

    void pop_back() {
        mSize = size() - 1;
    }

    void reserve(size_type num) {
        if (num > capacity()) {
            OnGrowRequest(num);
            pointer oldBuffer = mBegin;
            int oldSize = size();
            int oldCapacity = capacity();

            mBegin = AllocVectorSpace(num, Alignment);
            mCapacity = num;
            if (oldBuffer != mBegin) {
                mSize = 0;
                for (int ii = 0; ii < oldSize; ++ii) {
                    push_back(oldBuffer[ii]);
                }
                if (oldBuffer) {
                    FreeVectorSpace(oldBuffer, oldCapacity);
                }
            }
        }
    }

    void make_empty() {
        int num = size();
        for (int ii = 0; ii < num; ii++) {
            pop_back();
        }

        if (mBegin) {
            FreeVectorSpace(mBegin, mCapacity);
            mBegin = nullptr;
            mCapacity = 0;
            mSize = 0;
        }
    }

    void clear() {
        make_empty();
    }

    size_type indexof(pointer pos) {
        size_type index = pos - mBegin;
        return index;
    }

    iterator erase(iterator begIt, iterator endIt) {
        if (begIt == endIt) {
            return end();
        }

        size_type iPos = indexof(begIt);
        size_type num = endIt - begIt;
        for (iterator it = begIt; it != endIt; ++it) {
            value_type &obj = *it;
            obj.~T();
        }

        for (size_type ii = 0; ii < size() - (iPos + num); ++ii) {
            size_type src = iPos + num + ii;
            size_type dest = iPos + ii;

            new (&mBegin[dest]) T(mBegin[src]);
        }
        mSize = size() - num;
        return end();
    }

    iterator erase(iterator pos) {
        if (pos == pos + 1) {
            return nullptr;
        }
        return erase(pos, pos + 1);
    }

  protected:
    // OJO: la forma exacta de este cuerpo es la que casa (Speech::SpeechHashIDMap::Add y
    // Speech::EventHistory::Init al 100%). Tres detalles NO son cosmeticos:
    //   - el primer guardian es `oldBuffer != mBegin`, sin el `oldBuffer &&`;
    //   - el bucle de desplazamiento NO lleva delante un `if (oldSize != iPos)`;
    //   - las direcciones se calculan como `offset + (unsigned int)base` y no como
    //     `base + indice`: el front-end normaliza `ptr + int` a plus(ptr,int) y GCC 2.9
    //     emite entonces `add base,offset`, mientras que el original emite `add offset,base`.
    // Y `src[-1]` en vez de `--src` evita que GCC funda el decremento en un `lwzu`.
    void insert_sequence(iterator pos, unsigned int num, RepeatSequencer<const value_type> &sequencer) {
        pointer oldBuffer = mBegin;
        size_type oldSize = size();
        size_type oldCapacity = capacity();
        size_type iPos = pos - oldBuffer;
        size_type newSize = oldSize + num;

        if (newSize > oldCapacity) {
            OnGrowRequest(newSize);

            oldSize = size();
            oldCapacity = capacity();
            iPos = pos - mBegin;
            newSize = oldSize + num;

            if (newSize > oldCapacity) {
                mBegin = AllocVectorSpace(newSize, Alignment);
                mCapacity = newSize;
            }
        }

        mSize = newSize;

        if (oldBuffer != mBegin) {
            for (size_type ii = 0; ii < iPos; ++ii) {
                pointer dst = reinterpret_cast<pointer>(ii * sizeof(value_type) + reinterpret_cast<unsigned int>(mBegin));
                if (dst) {
                    new (dst) T(oldBuffer[ii]);
                }
            }
        }

        for (size_type ii = 0; ii < oldSize - iPos; ++ii) {
            pointer dst = reinterpret_cast<pointer>((newSize - ii) * sizeof(value_type) + reinterpret_cast<unsigned int>(mBegin));
            --dst;
            if (dst) {
                pointer src = reinterpret_cast<pointer>((oldSize - ii) * sizeof(value_type) + reinterpret_cast<unsigned int>(oldBuffer));
                new (dst) T(src[-1]);
            }
        }

        for (size_type ii = 0; ii < num; ++ii) {
            pointer dst = reinterpret_cast<pointer>((iPos + ii) * sizeof(value_type) + reinterpret_cast<unsigned int>(mBegin));
            if (dst) {
                new (dst) T(sequencer[ii]);
            }
        }

        if (oldBuffer && oldBuffer != mBegin) {
            FreeVectorSpace(oldBuffer, oldCapacity);
        }
    }

    // Unfinished
    virtual pointer AllocVectorSpace(size_type num, unsigned int alignment) = 0;

    virtual void FreeVectorSpace(pointer buffer, size_type num) = 0;

    virtual size_type GetGrowSize(size_type minSize) const {
        size_type growSize = mCapacity + ((mCapacity + 1) >> 1);
        return minSize > growSize ? minSize : growSize;
    }

    virtual size_type GetMaxCapacity() const {
        return 0x7FFFFFFF;
    }

    virtual void OnGrowRequest(size_type newSize) {}

  protected:
    pointer mBegin;      // offset 0x0, size 0x4
    size_type mCapacity; // offset 0x4, size 0x4
    size_type mSize;     // offset 0x8, size 0x4
};

template <typename T, int Size, int Alignment = 16> class FixedVector : public Vector<T, Alignment> {
  public:
    FixedVector() {}

    FixedVector(const FixedVector &src) {
        Vector<T, Alignment>::Init();
        *this = src;
    }

    FixedVector &operator=(const FixedVector &rhs) {
        Vector<T, Alignment>::assign(rhs);
        return *this;
    }

    ~FixedVector() override {
        // clang is being annoying
        Vector<T, Alignment>::clear();
    }

    // TODO also put the typedefs here according to the dwarf?

  protected:
    virtual std::size_t GetGrowSize(std::size_t minSize) const override {
        return Size;
    }

    virtual typename Vector<T, Alignment>::pointer AllocVectorSpace(std::size_t num, unsigned int alignment) override {
        return reinterpret_cast<typename Vector<T, Alignment>::pointer>(mVectorSpace);
    }

    virtual void FreeVectorSpace(typename Vector<T, Alignment>::pointer buffer, std::size_t) override {}

    virtual std::size_t GetMaxCapacity() const override {
        return Size;
    }

  private:
    // TODO speed considerations for 64 bit
    int mVectorSpace[(sizeof(typename Vector<T, Alignment>::value_type) * Size) / sizeof(int)];
};

template <typename T, int Alignment = 16> class FastVector : public Vector<T, Alignment> {
  public:
    FastVector() {}

    ~FastVector() override {
        Vector<T, Alignment>::clear();
    }

  protected:
    typename Vector<T, Alignment>::pointer AllocVectorSpace(std::size_t num, unsigned int alignment) override {
        return static_cast<typename Vector<T, Alignment>::pointer>(gFastMem.Alloc(num * sizeof(T), "FastVector"));
    }

    void FreeVectorSpace(typename Vector<T, Alignment>::pointer buffer, std::size_t num) override {
        gFastMem.Free(buffer, num * sizeof(T), "FastVector");
    }
};
}; // namespace UTL

#endif
