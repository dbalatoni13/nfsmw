#ifndef DEBUGGRAPH_HPP
#define DEBUGGRAPH_HPP

// Debug graph overlay used by the debug screens. The whole implementation was
// compiled out of the shipped GOWE69 build; only the constructor's presence as
// the first global object of the zDebug TU survives (see the key of
// _GLOBAL_.I.__10DebugGraphPcfffiffb).

struct Vector2 {
    float x;
    float y;
};

struct DebugGraphDataSet;

struct DebugGraph {
    typedef int DebugGraphHandle;

    inline void EnableKeyDisplay();
    inline void DisableKeyDisplay();

    DebugGraph(char *title, float dimX, float dimY, float maxUnit, int colour, float xpos, float ypos, bool defaultDataSet);
    ~DebugGraph();

    int AddGraphDataSet(char *label, int colour);
    void AddElement(float element);
    void AddElement(int dataSetHandle, float element);
    void Update();
    void Draw();

    unsigned int mElementCount;      // 0x00
    bool mDisplayKey;                // 0x04
    char mTitle[16];                 // 0x08
    float mMaxUnit;                  // 0x18
    unsigned int mColour;            // 0x1C
    Vector2 mPosition;               // 0x20
    Vector2 mDimension;              // 0x28
    unsigned int mGraphDataSetCount; // 0x30
    DebugGraphDataSet *mGraphDataSetList[16]; // 0x34
};

struct DebugGraphDataSet {
    DebugGraphDataSet(char *name, unsigned int elements, int colour, Vector2 &dimension, float maxUnit);
    ~DebugGraphDataSet();

    void AddElement(float element);
    void Update();
    void Draw(float x, float y);
    void DrawKey(float x, float y);

    char mTitle[16];          // 0x00
    unsigned int mElementCount; // 0x10
    Vector2 mDimension;       // 0x14
    unsigned int mColour;     // 0x1C
    float mDataGraphUnitScale; // 0x20
    float *mElementList;      // 0x24
};

inline void DebugGraph::EnableKeyDisplay() {
    mDisplayKey = true;
}

inline void DebugGraph::DisableKeyDisplay() {
    mDisplayKey = false;
}

#endif
