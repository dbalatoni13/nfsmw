#ifndef SAP_H
#define SAP_H

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace SAP {

// total size: 0x6C
template <typename T> class Grid {
  public:
    // total size: 0x34
    class Axis {
      public:
        // total size: 0x18
        class Node {
          public:
            // UNSOLVED
            Node(Axis &axis, Node *&root, float position)
                : mHead(nullptr), mTail(nullptr), mPosition(position), mSort(position), mAxis(axis), mRoot(root) {
                if (this->mRoot == nullptr) {
                    this->mRoot = this;
                } else {
                    Node *node = this->mRoot;
                    Node *head = nullptr;

                    while (node != nullptr && node->mPosition < this->mPosition) {
                        head = node;
                        node = node->mTail;
                    }

                    if (head != nullptr) {
                        this->Link(head, head->GetTail());
                    } else {
                        this->Link(node->GetHead(), node);
                    }
                }
            }

            ~Node() {
                Unlink();
                mTail = nullptr;
                mHead = nullptr;
            }

            Axis &GetAxis() {
                return this->mAxis;
            }

            Grid<T> &GetGrid() {
                return this->mAxis.GetGrid();
            }

            T &GetOwner() {
                return this->mAxis.GetOwner();
            }

            bool IsMin() {
                return this == &this->mAxis.GetMin();
            }

            Node *GetTail() {
                return this->mTail;
            }

            Node *GetHead() {
                return this->mHead;
            }

            void SetPosition(float position) {
                this->mPosition = position;
            }

            float GetPosition() const {
                return this->mPosition;
            }

            void Unlink() {
                if (this->mRoot == this) {
                    this->mRoot = this->mTail;
                }

                if (this->mHead != nullptr) {
                    this->mHead->mTail = this->mTail;
                }

                if (this->mTail != nullptr) {
                    this->mTail->mHead = this->mHead;
                }
            }

            // UNSOLVED
            void Link(Node *head, Node *tail) {
                this->Unlink();
                this->mTail = nullptr;
                this->mHead = nullptr;

                if (head != nullptr) {
                    this->mHead = head;
                    head->mTail = this;
                }

                if (tail != nullptr) {
                    this->mTail = tail;
                    tail->mHead = this;

                    if (this->mTail == this->mRoot) {
                        this->mRoot = this;
                    }
                }
            }

            static unsigned int Sweep(Node *root) {
                if (root == nullptr) {
                    return 0;
                }

                Node *from = root;
                unsigned int loop = 0;

                while (from != nullptr) {
                    Node *next = from->mTail;
                    const float position = from->mPosition;

                    from->mSort = position;

                    Node *node = nullptr;
                    Node *iter;
                    for (iter = from->mHead; iter != nullptr && position < iter->mSort; iter = iter->mHead) {
                        node = iter;
                    }

                    if (node != nullptr) {
                        from->Link(node->mHead, node);
                        ++loop;
                    } else {
                        node = nullptr;
                        for (iter = from->mTail; iter != nullptr && position > iter->mSort; iter = iter->mTail) {
                            node = iter;
                        }

                        if (node != nullptr) {
                            from->Link(node, node->mTail);
                            ++loop;
                        }
                    }

                    from = next;
                }
                return loop;
            }

          private:
            Node *mHead;     // offset 0x0, size 0x4
            Node *mTail;     // offset 0x4, size 0x4
            float mPosition; // offset 0x8, size 0x4
            float mSort;     // offset 0xC, size 0x4
            Axis &mAxis;     // offset 0x10, size 0x4
            Node *&mRoot;    // offset 0x14, size 0x4
        };

        ~Axis() {}

        Axis(Grid<T> &grid, Node *&root, float position, float radius)
            : mMin(*this, root, position - radius), mMax(*this, root, position + radius), mGrid(grid) {}

        Node &GetMin() {
            return this->mMin;
        }

        Node &GetMax() {
            return this->mMax;
        }

        Grid<T> &GetGrid() {
            return this->mGrid;
        }

        T &GetOwner() {
            return this->mGrid.GetOwner();
        }

        bool Overlaps() {
            return this->mMin.GetTail() != &this->mMax;
        }

        void SetPosition(float position, float radius) {
            this->mMin.SetPosition(position - radius);
            this->mMax.SetPosition(position + radius);
        }

      private:
        Node mMin;      // offset 0x0, size 0x18
        Node mMax;      // offset 0x18, size 0x18
        Grid<T> &mGrid; // offset 0x30, size 0x4
    };

    typedef typename Axis::Node Node;

    Grid(T &owner, const UMath::Vector3 &position, float radius)
        : mX(*this, mRootX, position.x, radius), mZ(*this, mRootZ, position.z, radius), mOwner(owner) {}

    T &GetOwner() {
        return this->mOwner;
    }

    Axis &GetX() {
        return mX;
    }

    Axis &GetZ() {
        return mZ;
    }

    static Node *GetRootX() {
        return mRootX;
    }

    static Node *GetRootZ() {
        return mRootZ;
    }

    void SetPosition(const UMath::Vector3 &position, float radius) {
        mX.SetPosition(position.x, radius);
        mZ.SetPosition(position.z, radius);
    }

    static unsigned int SweepX() {
        return Node::Sweep(mRootX);
    }

    static unsigned int SweepZ() {
        return Node::Sweep(mRootZ);
    }

    static unsigned int Sweep() {
        return SweepX() + SweepZ();
    }

    static void (*Prune(bool xprimary, void (*cb)(T &, T &, float), float cbparam))(T &, T &, float) {
        Node *iter = xprimary ? GetRootX() : GetRootZ();

        for (; iter != nullptr; iter = iter->GetTail()) {
            if (!iter->IsMin()) {
                continue;
            }

            Axis &axis = iter->GetAxis();

            if (!axis.Overlaps()) {
                continue;
            }

            Axis &otheraxis = xprimary ? iter->GetGrid().GetZ() : iter->GetGrid().GetX();
            const float lower = otheraxis.GetMin().GetPosition();
            const float upper = otheraxis.GetMax().GetPosition();
            Node *node;
            Node *end = &axis.GetMax();

            for (node = iter->GetTail(); node != end; node = node->GetTail()) {
                if (!node->IsMin()) {
                    continue;
                }

                Axis &node_otheraxis = xprimary ? node->GetGrid().GetZ() : node->GetGrid().GetX();
                float fmin = node_otheraxis.GetMin().GetPosition();
                float fmax = node_otheraxis.GetMax().GetPosition();

                if (((fmin >= lower) && (fmin <= upper)) || ((fmax >= lower) && (fmax <= upper)) || ((fmin < lower) && (fmax > upper))) {
                    cb(iter->GetOwner(), node->GetOwner(), cbparam);
                }
            }
        }

        return cb;
    }

    static Node *mRootX;
    static Node *mRootZ;

  private:
    Axis mX;   // offset 0x0, size 0x34
    Axis mZ;   // offset 0x34, size 0x34
    T &mOwner; // offset 0x68, size 0x4
};

#define IMPLEMENT_SAP_GRID(_TYPE_)                                                                                                                   \
    template <> SAP::Grid<_TYPE_>::Axis::Node *SAP::Grid<_TYPE_>::mRootX = nullptr;                                                                  \
    template <> SAP::Grid<_TYPE_>::Axis::Node *SAP::Grid<_TYPE_>::mRootZ = nullptr;
}; // namespace SAP

#endif
