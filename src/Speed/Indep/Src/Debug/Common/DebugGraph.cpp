#include "Speed/Indep/Src/Debug/Common/DebugGraph.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

// Constructors ---------------------------------------------------------

DebugGraph::DebugGraph(char *title, float dimX, float dimY, float maxUnit, int colour, float xpos, float ypos, bool defaultDataSet) {
    mElementCount = 0;
    mDisplayKey = false;
    mGraphDataSetCount = 0;
    for (int ix = 0; ix < 16; ix++) {
        mGraphDataSetList[ix] = 0;
    }
    mMaxUnit = maxUnit;
    mColour = colour;
    mPosition.x = xpos;
    mPosition.y = ypos;
    mDimension.x = dimX;
    mDimension.y = dimY;
    if (title != 0) {
        for (int ix = 0; ix < 16; ix++) {
            mTitle[ix] = title[ix];
            if (title[ix] == '\0') {
                break;
            }
        }
    }
    if (defaultDataSet) {
        AddGraphDataSet("default", colour);
    }
}

DebugGraph::~DebugGraph() {
    for (unsigned int ix = 0; ix < mGraphDataSetCount; ix++) {
        DebugGraphDataSet *graphDataSet = mGraphDataSetList[ix];
        if (graphDataSet != 0) {
            delete graphDataSet;
        }
    }
}

int DebugGraph::AddGraphDataSet(char *label, int colour) {
    int handle = mGraphDataSetCount;
    if (mGraphDataSetCount < 16) {
        mGraphDataSetList[mGraphDataSetCount] = new DebugGraphDataSet(label, 0, colour, mDimension, mMaxUnit);
        mGraphDataSetCount++;
    }
    return handle;
}

void DebugGraph::AddElement(float element) {
    AddElement(0, element);
}

void DebugGraph::AddElement(int dataSetHandle, float element) {
    DebugGraphDataSet *graphDataSet = mGraphDataSetList[dataSetHandle];
    if (graphDataSet != 0) {
        graphDataSet->AddElement(element);
    }
}

void DebugGraph::Update() {
    for (unsigned int ix = 0; ix < mGraphDataSetCount; ix++) {
        DebugGraphDataSet *graphDataSet = mGraphDataSetList[ix];
        if (graphDataSet != 0) {
            graphDataSet->Update();
        }
    }
}

void DebugGraph::Draw() {
    for (unsigned int ix = 0; ix < mGraphDataSetCount; ix++) {
        DebugGraphDataSet *graphDataSet = mGraphDataSetList[ix];
        if (graphDataSet != 0) {
            graphDataSet->Draw(mPosition.x, mPosition.y);
        }
    }
}
