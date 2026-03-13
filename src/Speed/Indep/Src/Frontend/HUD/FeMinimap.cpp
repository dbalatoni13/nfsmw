#include "Speed/Indep/Src/Frontend/HUD/FeMinimap.hpp"

struct bChunk;

struct ChoppedMiniMapManager {
    void Loader(bChunk *chunk);
    void Unloader(bChunk *chunk);
};

extern ChoppedMiniMapManager *gChoppedMiniMapManager;

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

void Minimap::ConvertPos(bVector2 &out, bVector2 &in, TrackInfo *track) {
    out.x = (in.x - *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xAC)) /
            *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4);
    out.y = (*reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB0) - in.y) /
            *reinterpret_cast<float *>(reinterpret_cast<char *>(track) + 0xB4) + 1.0f;
}

void Minimap::UpdateRaceElements() {
    UpdateMiniMapItems();
}

void Minimap::InitStaticMiniMapItems() {}
