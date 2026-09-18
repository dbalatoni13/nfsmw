#include "Speed/Indep/Src/Misc/DemoDisc.hpp"

DemoDiscManager TheDemoDiscManager;

DemoDiscManager::DemoDiscManager() {}

// STRIPPED en GameCube: la demo disc compartida no existe en esta plataforma.
void DemoDiscManager::Init(int argc, char **argv) {}

void DemoDiscManager::SetEndReason(DemoDiscEndReason end_reason) {}
