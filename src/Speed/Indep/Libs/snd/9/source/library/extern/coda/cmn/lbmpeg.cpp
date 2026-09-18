namespace Snd {
int MPEGuse_MMX = 0;

struct CMpegBase_DecodeMethod {
    CMpegBase_DecodeMethod() {
        MPEGuse_MMX = 0;
    }
};

static CMpegBase_DecodeMethod dummy;
}
