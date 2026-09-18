#include "Speed/Indep/Src/AI/AIRoadBlock.h"
RoadblockSetup RoadblockCandidateList[16] = {
    {8.0f, 2,
     {RoadblockElement(kCar, -3.0f, 0.0f, 0.75f), RoadblockElement(kCar, 2.0f, -0.35f, 0.27f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {12.0f, 3,
     {RoadblockElement(kCar, -5.2f, -2.0f, 0.875f), RoadblockElement(kCar, 0.0f, -2.0f, 0.12f),
      RoadblockElement(kCar, 4.5f, -1.0f, 0.87f), RoadblockElement(kBarrier, -2.9f, 0.2f, 0.48f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {5.5f, 2,
     {RoadblockElement(kCar, -2.0f, 0.0f, 0.26f), RoadblockElement(kCar, 2.0f, 5.0f, 0.48f),
      RoadblockElement(kBarrier, 1.5f, 1.7f, 0.61f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {6.5f, 2,
     {RoadblockElement(kCar, -2.0f, 0.0f, 0.75f), RoadblockElement(kCar, 3.0f, 4.0f, 0.25f),
      RoadblockElement(kBarrier, 1.5f, 1.7f, 0.61f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {8.5f, 2,
     {RoadblockElement(kCar, -0.9f, 0.0f, 0.72f), RoadblockElement(kCar, 4.0f, -0.0f, 0.32f),
      RoadblockElement(kBarrier, -4.8f, -0.8f, 0.54f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {15.0f, 4,
     {RoadblockElement(kCar, -5.0f, 0.0f, 0.71f), RoadblockElement(kCar, 1.0f, 0.0f, 0.29f),
      RoadblockElement(kCar, 5.0f, 6.0f, 0.24f), RoadblockElement(kCar, 8.0f, 2.0f, 0.5f),
      RoadblockElement(kBarrier, 4.1f, -1.4f, 0.57f), RoadblockElement(kBarrier, 6.1f, -1.4f, 0.43f)}},

    {13.0f, 3,
     {RoadblockElement(kCar, -4.0f, 1.0f, 0.26f), RoadblockElement(kCar, -0.9f, -4.2f, 0.75f),
      RoadblockElement(kCar, 5.0f, -4.0f, 0.26f), RoadblockElement(kBarrier, -3.3f, -2.2f, 0.4f),
      RoadblockElement(kBarrier, -5.0f, -4.0f, 0.46f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {11.4f, 3,
     {RoadblockElement(kCar, -4.4f, -1.2f, 0.65f), RoadblockElement(kCar, 0.0f, 2.4f, 0.24f),
      RoadblockElement(kCar, 4.4f, -1.2f, 0.35f), RoadblockElement(kBarrier, 0.0f, -2.7f, 0.49f),
      RoadblockElement(kBarrier, 2.1f, 0.0f, 0.35f), RoadblockElement(kBarrier, -2.1f, 0.0f, 0.65f)}},

    {12.1f, 3,
     {RoadblockElement(kCar, -4.5f, 0.0f, 0.75f), RoadblockElement(kCar, 1.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 4.0f, 4.5f, 0.72f), RoadblockElement(kBarrier, 4.3f, -1.2f, 0.48f),
      RoadblockElement(kBarrier, 4.9f, 0.3f, 0.04f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {10.0f, 4,
     {RoadblockElement(kCar, -4.5f, 0.0f, 0.27f), RoadblockElement(kCar, -0.3f, -4.0f, 0.75f),
      RoadblockElement(kCar, 4.2f, -4.0f, 0.24f), RoadblockElement(kCar, 4.0f, 0.0f, 0.26f),
      RoadblockElement(kBarrier, -4.9f, -5.0f, 0.51f), RoadblockElement(kBarrier, -4.2f, -3.3f, 0.53f)}},

    {14.0f, 4,
     {RoadblockElement(kCar, -4.5f, 0.0f, 0.25f), RoadblockElement(kCar, 1.0f, 0.0f, 0.75f),
      RoadblockElement(kCar, 0.0f, 5.4f, 0.25f), RoadblockElement(kCar, 5.3f, 5.4f, 0.22f),
      RoadblockElement(kBarrier, -2.0f, 2.2f, 0.37f), RoadblockElement(kBarrier, 5.4f, -0.2f, 0.53f)}},

    {12.0f, 4,
     {RoadblockElement(kCar, -4.3f, 0.0f, 0.74f), RoadblockElement(kCar, -5.0f, 4.0f, 0.75f),
      RoadblockElement(kCar, 0.2f, 4.0f, 0.27f), RoadblockElement(kCar, 4.0f, 0.0f, 0.25f),
      RoadblockElement(kBarrier, -0.7f, -1.0f, 0.355f), RoadblockElement(kBarrier, 0.7f, -1.0f, 0.635f)}},

    {22.0f, 4,
     {RoadblockElement(kCar, -9.0f, 0.0f, 0.74f), RoadblockElement(kCar, -3.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 3.0f, 0.0f, 0.75f), RoadblockElement(kCar, 9.0f, 0.0f, 0.25f),
      RoadblockElement(kBarrier, 0.0f, -3.0f, 0.5f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {15.0f, 3,
     {RoadblockElement(kCar, -5.9f, 0.0f, 0.74f), RoadblockElement(kCar, 0.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 5.9f, 0.0f, 0.75f), RoadblockElement(kBarrier, 3.0f, -2.0f, 0.51f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {28.0f, 5,
     {RoadblockElement(kCar, -12.0f, 0.0f, 0.74f), RoadblockElement(kCar, -6.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 0.0f, 0.0f, 0.75f), RoadblockElement(kCar, 6.0f, 0.0f, 0.75f),
      RoadblockElement(kCar, 12.0f, 0.0f, 0.25f), RoadblockElement(kBarrier, -3.0f, -3.4f, 0.5f)}},

    {0.0f, 0,
     {RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},
};


RoadblockSetup SPIKES_RoadblockCandidateList[10] = {
    {8.0f, 1,
     {RoadblockElement(kCar, -3.0f, 0.0f, 0.75f), RoadblockElement(kSpikeStrip, 2.0f, -0.35f, 0.52f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {15.0f, 3,
     {RoadblockElement(kSpikeStrip, -5.0f, 0.0f, 0.51f), RoadblockElement(kCar, 1.0f, 0.0f, 0.29f),
      RoadblockElement(kCar, 5.0f, 6.0f, 0.24f), RoadblockElement(kCar, 8.0f, 2.0f, 0.5f),
      RoadblockElement(kBarrier, 4.1f, -1.4f, 0.57f), RoadblockElement(kBarrier, 6.1f, -1.4f, 0.43f)}},

    {15.0f, 2,
     {RoadblockElement(kCar, -4.0f, 1.0f, 0.26f), RoadblockElement(kCar, -0.9f, -4.2f, 0.75f),
      RoadblockElement(kSpikeStrip, 5.0f, -4.0f, 0.52f), RoadblockElement(kBarrier, -3.3f, -2.2f, 0.4f),
      RoadblockElement(kBarrier, -5.0f, -4.0f, 0.46f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {14.1f, 2,
     {RoadblockElement(kSpikeStrip, -4.8f, 0.0f, 0.5f), RoadblockElement(kCar, 1.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 4.0f, 4.5f, 0.72f), RoadblockElement(kBarrier, 4.3f, -1.2f, 0.48f),
      RoadblockElement(kBarrier, 4.9f, 0.3f, 0.04f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {12.0f, 3,
     {RoadblockElement(kCar, -4.0f, 0.0f, 0.27f), RoadblockElement(kSpikeStrip, -0.3f, -4.4f, 0.5f),
      RoadblockElement(kCar, 4.8f, -4.0f, 0.24f), RoadblockElement(kCar, 4.0f, 0.0f, 0.26f),
      RoadblockElement(kBarrier, -4.9f, -5.0f, 0.51f), RoadblockElement(kBarrier, -4.2f, -3.3f, 0.53f)}},

    {13.0f, 3,
     {RoadblockElement(kSpikeStrip, -4.0f, 0.0f, 0.51f), RoadblockElement(kCar, -5.0f, 4.0f, 0.75f),
      RoadblockElement(kCar, 0.2f, 4.0f, 0.27f), RoadblockElement(kCar, 4.0f, 0.0f, 0.25f),
      RoadblockElement(kBarrier, -0.7f, -1.0f, 0.355f), RoadblockElement(kBarrier, 0.7f, -1.0f, 0.635f)}},

    {21.0f, 2,
     {RoadblockElement(kSpikeStrip, -9.0f, 0.0f, 0.48f), RoadblockElement(kCar, -3.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 3.0f, 0.0f, 0.75f), RoadblockElement(kSpikeStrip, 9.0f, 0.0f, 0.52f),
      RoadblockElement(kBarrier, 0.0f, -3.0f, 0.5f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {15.0f, 2,
     {RoadblockElement(kSpikeStrip, -5.2f, 0.0f, 0.51f), RoadblockElement(kCar, 0.0f, 0.0f, 0.25f),
      RoadblockElement(kCar, 5.9f, 0.0f, 0.75f), RoadblockElement(kBarrier, 3.0f, -2.0f, 0.51f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {28.0f, 4,
     {RoadblockElement(kCar, -12.0f, 0.0f, 0.74f), RoadblockElement(kCar, -6.0f, 0.0f, 0.25f),
      RoadblockElement(kSpikeStrip, -1.0f, 0.0f, 0.51f), RoadblockElement(kCar, 5.0f, 0.0f, 0.75f),
      RoadblockElement(kCar, 11.0f, 0.0f, 0.25f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},

    {0.0f, 0,
     {RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f),
      RoadblockElement(kNone, 0.0f, 0.0f, 0.0f), RoadblockElement(kNone, 0.0f, 0.0f, 0.0f)}},
};
