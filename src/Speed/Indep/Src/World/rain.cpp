#include "./Rain.hpp"

float Rain::GetCloudIntensity() {
    return this->CloudIntensity;
}

float Rain::GetRainIntensity() {
    return this->intensity;
}

float Rain::GetRoadDampness() {
    return RoadDampness;
}

void Rain::GetPrecipFogColour(unsigned int *r, unsigned int *g, unsigned int *b) {
    *r = this->fogR;
    *g = this->fogG;
    *b = this->fogB;
}
