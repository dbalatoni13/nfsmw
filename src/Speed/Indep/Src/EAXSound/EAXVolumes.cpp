enum FXROADNOISE_TRANSITION {
    FXROADNOISE_TRANSITION_CURBON = 0,
    FXROADNOISE_TRANSITION_CURBOFF = 1,
    FXROADNOISE_TRANSITION_SPIKESTRIP = 2,
    FXROADNOISE_TRANSITION_BLOWN = 3,
};

int GetRoadNoiseTransitionVol(FXROADNOISE_TRANSITION ID) {
    if (ID == FXROADNOISE_TRANSITION_CURBOFF) {
        goto ReturnCurb;
    }
    if (ID > FXROADNOISE_TRANSITION_CURBOFF) {
        goto CheckHighTransitions;
    }
    if (ID == FXROADNOISE_TRANSITION_CURBON) {
        goto ReturnCurb;
    }
    goto ReturnDefault;

CheckHighTransitions:
    if (ID == FXROADNOISE_TRANSITION_SPIKESTRIP) {
        goto ReturnSpikeStrip;
    }
    if (ID == FXROADNOISE_TRANSITION_BLOWN) {
        goto ReturnBlown;
    }
    goto ReturnDefault;

ReturnCurb:
    return 0x3C8C;

ReturnSpikeStrip:
    return 0x6590;

ReturnBlown:
    return 0x7FFF;

ReturnDefault:
    return 0x7FFF;
}
