typedef void (*VP6_BuildQuantIndex_FUNC)(void);

extern void VP6_BuildQuantIndex_Generic(void);
extern VP6_BuildQuantIndex_FUNC VP6_BuildQuantIndex;

int VP6_GetProcessorFrequency(void) {
    return 0;
}

void VP6_DMachineSpecificConfig(void) {
    VP6_BuildQuantIndex = VP6_BuildQuantIndex_Generic;
}
