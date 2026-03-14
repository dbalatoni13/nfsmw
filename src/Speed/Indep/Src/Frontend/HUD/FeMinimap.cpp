#include "Speed/Indep/Src/Frontend/HUD/FeMinimap.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GIcon.h"

void LoaderMiniMap(bChunk *chunk) {
    gChoppedMiniMapManager->Loader(chunk);
}

void UnloaderMiniMap(bChunk *chunk) {
    gChoppedMiniMapManager->Unloader(chunk);
}

Minimap::Minimap(const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0)
{
}

void Minimap::Update(IPlayer *player) {
}

void Minimap::ConvertPos(bVector2 &worldPos, bVector2 &minimapPos, TrackInfo *track) {
    minimapPos.x = (worldPos.x - *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xAC)) /
            *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4);
    minimapPos.y = (*reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB0) - worldPos.y) /
            *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4) + 1.0f;
}

void Minimap::UpdateRaceElements() {
    UpdateMiniMapItems();
}

void Minimap::InitStaticMiniMapItems() {}
