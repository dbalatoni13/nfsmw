#include "ECommitRenderAssets.hpp"

#include "Speed/Indep/Src/World/CarLoader.hpp"

ECommitRenderAssets::ECommitRenderAssets() : Event(0x10) {
}

ECommitRenderAssets::~ECommitRenderAssets() {
    TheCarLoader.BeginLoading(nullptr, 0);
}

const char *ECommitRenderAssets::GetEventName() const {
    return "ECommitRenderAssets";
}

void ECommitRenderAssets_MakeEvent_Callback(const void *staticData) {
    new ECommitRenderAssets();
}
