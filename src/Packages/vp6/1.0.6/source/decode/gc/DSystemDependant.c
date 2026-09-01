extern void (*VP6_BuildQuantIndex)(void *);
extern void VP6_BuildQuantIndex_Generic(void *);

unsigned int VP6_GetProcessorFrequency(void) {
    return 0;
}

void VP6_DMachineSpecificConfig(void) {
    VP6_BuildQuantIndex = VP6_BuildQuantIndex_Generic;
}
