#ifndef SUPPORT_MISC_SAP_H
#define SUPPORT_MISC_SAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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
            Node(Axis &axis, struct Node *&root, float position) {
                // TODO
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

          private:
            Node *mHead;     // offset 0x0, size 0x4
            Node *mTail;     // offset 0x4, size 0x4
            float mPosition; // offset 0x8, size 0x4
            float mSort;     // offset 0xC, size 0x4
            Axis &mAxis;     // offset 0x10, size 0x4
            Node *&mRoot;    // offset 0x14, size 0x4
        };

        Axis(Grid<T> &grid, Node *&root, float position, float radius) {
            // TODO
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

    ~Grid();

    Grid(T &owner, const struct Vector3 &position, float radius) {
        // TODO
    }

    Axis &GetX() {
        return mX;
    }

    Axis &GetZ() {
        return mZ;
    }

    void SetPosition(const UMath::Vector3 &position, float radius) {
        mX.SetPosition(position.x, radius);
        mZ.SetPosition(position.z, radius);
    }

    static Node *mRootX;
    static Node *mRootZ;

  private:
    Axis mX;   // offset 0x0, size 0x34
    Axis mZ;   // offset 0x34, size 0x34
    T &mOwner; // offset 0x68, size 0x4
};

}; // namespace SAP

#endif
