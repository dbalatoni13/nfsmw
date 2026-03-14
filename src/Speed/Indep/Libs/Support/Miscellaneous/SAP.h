#ifndef SUPPORT_MISC_SAP_H
#define SUPPORT_MISC_SAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"

#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace SAP {

template <typename T> class Grid {
    // total size: 0x6C
  public:
    class Axis {
        // total size: 0x34
      public:
        class Node {
            // total size: 0x18
          public:
            Node(Axis &axis, Node *&root, float position)
                : mHead(nullptr) //
                , mTail(nullptr) //
                , mPosition(position) //
                , mSort(position) //
                , mAxis(axis) //
                , mRoot(root) {
                if (!root) {
                    root = this;
                    return;
                }

                Node *node = root;
                Node *head = nullptr;

                if (node->mPosition < mPosition) {
                    do {
                        head = node;
                        node = node->mTail;
                    } while (node && node->mPosition < mPosition);
                }

                if (head) {
                    Node *tail = head->GetTail();
                    Link(head, tail);
                } else {
                    Node *h = root->GetHead();
                    Link(h, root);
                }
            }

            ~Node() {
                Unlink();
            }

            Node *GetHead() {
                return mHead;
            }

            Node *GetTail() {
                return mTail;
            }

            void SetPosition(float position) {
                mPosition = position;
            }

            float GetPosition() const {
                return mPosition;
            }

            bool IsMin() {
                return this == &mAxis.GetMin();
            }

            Axis &GetAxis() {
                return mAxis;
            }

            Grid<T> &GetGrid() {
                return mAxis.GetGrid();
            }

            T &GetOwner() {
                return mAxis.GetOwner();
            }

            void Unlink() {
                if (mRoot == this) {
                    mRoot = mTail;
                }
                if (mHead) {
                    mHead->mTail = mTail;
                }
                if (mTail) {
                    mTail->mHead = mHead;
                }
                mTail = nullptr;
                mHead = nullptr;
            }

            void Link(Node *head, Node *tail) {
                Unlink();

                mHead = nullptr;
                if (head) {
                    mHead = head;
                    head->mTail = this;
                }

                if (!tail) return;

                mTail = tail;
                tail->mHead = this;

                if (mTail == mRoot) {
                    mRoot = this;
                }
            }

            static unsigned int Sweep(Node *root) {
                unsigned int loop = 0;
                Node *from = root;
                Node *next = root ? root->mTail : nullptr;

                while (from) {
                    float position = from->mPosition;
                    Node *head = from->mHead;
                    from->mSort = position;
                    Node *node = nullptr;

                    if (head) {
                        float sort = head->mSort;
                        Node *iter = head;
                        while (position < sort) {
                            node = iter;
                            iter = iter->mHead;
                            if (!iter) break;
                            sort = iter->mSort;
                        }
                    }

                    if (!node) {
                        node = nullptr;
                        if (next) {
                            float sort = next->mSort;
                            Node *iter = next;
                            while (sort < position) {
                                node = iter;
                                iter = iter->mTail;
                                if (!iter) break;
                                sort = iter->mSort;
                            }
                            if (node) {
                                Node *tail = node->GetTail();
                                from->Link(node, tail);
                            }
                        }
                    } else {
                        Node *h = node->GetHead();
                        from->Link(h, node);
                    }

                    if (!next) break;
                    from = next;
                    next = next->mTail;
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

        Axis(Grid<T> &grid, Node *&root, float position, float radius)
            : mMin(*this, root, position - radius) //
            , mMax(*this, root, position + radius) //
            , mGrid(grid) {
        }

        ~Axis() {
        }

        Node &GetMin() {
            return mMin;
        }

        Node &GetMax() {
            return mMax;
        }

        Grid<T> &GetGrid() {
            return mGrid;
        }

        T &GetOwner() {
            return mGrid.GetOwner();
        }

        bool Overlaps() {
            return mMin.GetTail() != &mMax;
        }

        void SetPosition(float position, float radius) {
            mMin.SetPosition(position - radius);
            mMax.SetPosition(position + radius);
        }

      private:
        Node mMin;      // offset 0x0, size 0x18
        Node mMax;      // offset 0x18, size 0x18
        Grid<T> &mGrid; // offset 0x30, size 0x4
    };

    typedef typename Axis::Node Node;

#ifdef EA_PLATFORM_GAMECUBE
    void *operator new(std::size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    void operator delete(void *mem, std::size_t size) {
        if (mem) {
            gFastMem.Free(mem, size, nullptr);
        }
    }
#endif

    ~Grid();

    Grid(T &owner, const UMath::Vector3 &position, float radius)
        : mX(*this, mRootX, position.x, radius) //
        , mZ(*this, mRootZ, position.z, radius) //
        , mOwner(owner) {
    }

    Axis &GetX() {
        return mX;
    }

    Axis &GetZ() {
        return mZ;
    }

    T &GetOwner() {
        return mOwner;
    }

    void SetPosition(const UMath::Vector3 &position, float radius) {
        mX.SetPosition(position.x, radius);
        mZ.SetPosition(position.z, radius);
    }

    static Node *GetRootX() {
        return mRootX;
    }

    static Node *GetRootZ() {
        return mRootZ;
    }

    static unsigned int SweepX() {
        return Node::Sweep(mRootX);
    }

    static unsigned int SweepZ() {
        return Node::Sweep(mRootZ);
    }

    static unsigned int Sweep() {
        SweepX();
        return SweepZ();
    }

    static void Prune(bool xprimary, void (*cb)(T &, T &, float), float cbparam) {
        Node *iter = xprimary ? GetRootZ() : GetRootX();

        if (!iter) return;

        Node *end = iter;
        while (iter) {
            if (iter->IsMin()) {
                Axis &axis = iter->GetAxis();
                if (!axis.Overlaps()) {
                    iter = iter->GetTail();
                    continue;
                }

                Axis &otheraxis = iter->GetAxis();
                Grid<T> &grid = iter->GetGrid();
                Axis &secondary = xprimary ? grid.GetX() : grid.GetZ();

                float lower = secondary.GetMin().GetPosition();
                float upper = secondary.GetMax().GetPosition();

                Node *node = iter->GetTail();
                while (node && node != &axis.GetMax()) {
                    if (node->IsMin()) {
                        Grid<T> &nodeGrid = node->GetGrid();
                        Axis &nodeSecondary = xprimary ? nodeGrid.GetX() : nodeGrid.GetZ();

                        float fmin = nodeSecondary.GetMin().GetPosition();
                        float fmax = nodeSecondary.GetMax().GetPosition();

                        if (fmin >= lower && fmin <= upper) {
                            cb(iter->GetGrid().GetOwner(), node->GetGrid().GetOwner(), cbparam);
                        } else if (fmax >= lower && fmax <= upper) {
                            cb(iter->GetGrid().GetOwner(), node->GetGrid().GetOwner(), cbparam);
                        } else if (fmin < lower && fmax > upper) {
                            cb(iter->GetGrid().GetOwner(), node->GetGrid().GetOwner(), cbparam);
                        }
                    }
                    node = node->GetTail();
                }
            }
            iter = iter->GetTail();
        }
    }

    static Node *mRootX;
    static Node *mRootZ;

  private:
    Axis mX;   // offset 0x0, size 0x34
    Axis mZ;   // offset 0x34, size 0x34
    T &mOwner; // offset 0x68, size 0x4
};

template <class T>
SAP::Grid<T>::~Grid() {
}

template <class T>
typename SAP::Grid<T>::Node *SAP::Grid<T>::mRootX;

template <class T>
typename SAP::Grid<T>::Node *SAP::Grid<T>::mRootZ;

}; // namespace SAP

#endif
