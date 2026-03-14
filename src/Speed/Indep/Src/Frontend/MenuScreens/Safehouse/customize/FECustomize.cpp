// OWNED BY zFeOverlay AGENT - emptied to avoid DWARF crash from CarCustomize.hpp
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"

static bool gInBackRoom;
static bool gInPerformance;
static bool gInParts;

bool CustomizeIsInBackRoom() { return gInBackRoom; }
void CustomizeSetInBackRoom(bool b) { gInBackRoom = b; }
bool CustomizeIsInPerformance() { return gInPerformance; }
void CustomizeSetInPerformance(bool b) { gInPerformance = b; }
bool CustomizeIsInParts() { return gInParts; }
void CustomizeSetInParts(bool b) { gInParts = b; }
