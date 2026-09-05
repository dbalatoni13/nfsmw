#include "rcmp/rcmp.h"

extern "C" const float lbl_80410154;

namespace RCMP {

RCMP_SYSTEM rcmp_sys;

RCMP_SYSTEM::RCMP_SYSTEM()
    : AllocMemFunc(0)
    , FreeMemFunc(0) {
}

CODEC_IDATA::CODEC_IDATA()
    : m_Streamer(0)
    , m_GetDataFunc(0)
    , m_ReleaseDataFunc(0)
    , m_MaxFramesOutstanding(2) {
}

CODEC_IDATA::CODEC_IDATA(
    STREAMER *Streamer,
    GETDATACALLBACK GetDataFunc,
    RELEASEDATACALLBACK ReleaseDataFunc,
    unsigned int NumberOfFramesToBuffer
)
{
    this->m_MaxFramesOutstanding = NumberOfFramesToBuffer;
    this->m_Streamer = Streamer;
    this->m_GetDataFunc = GetDataFunc;
    this->m_ReleaseDataFunc = ReleaseDataFunc;
}

DECODER::DECODER(const CODEC_IDATA *IData)
    : m_IData() {
    this->m_IData = *IData;
    this->m_codec = 0;
    this->m_FirstChunk = 0;
}

DECODER::~DECODER() {
    this->FreeChosenCodec();
}

DETECTED_USABILITY_ENUM DECODER::ChooseCodec(CODEC *codec, CHUNK *FirstChunk) {
    DETECTED_USABILITY_ENUM ret;

    this->m_FirstChunk = FirstChunk;
    this->m_codec = codec;
    ret = codec->Init(this, FirstChunk);
    return ret;
}

void DECODER::FreeChosenCodec() {
    if (this->m_codec != 0) {
        delete this->m_codec;
        this->m_codec = 0;
    }
}

unsigned int DECODER::GetCurrentFrameNumber() {
    if (this->m_codec == 0) {
        return 0;
    }
    return this->m_codec->GetCurrentFrameNumber();
}

float DECODER::GetFrameRate() {
    if (this->m_codec == 0) {
        return lbl_80410154;
    }
    return this->m_codec->GetFrameRate();
}

FRAME *DECODER::GetFrame(unsigned int GoalFrame) {
    if (this->m_codec == 0) {
        if (this->m_FirstChunk == 0) {
            this->GetCodecIData()->m_GetDataFunc(this, this->GetCodecIData()->m_Streamer, &this->m_FirstChunk);
        }
    }
    if (this->m_codec == 0) {
        return 0;
    } else {
        return this->m_codec->GetFrame(GoalFrame);
    }
}

void DECODER::ReleaseFrame(FRAME *Frame) {
    this->m_codec->ReleaseFrame(Frame);
}

CHUNK *DECODER::GetChunk() {
    CHUNK *NextChunk;

    if (this->m_FirstChunk == 0) {
        this->GetCodecIData()->m_GetDataFunc(this, this->GetCodecIData()->m_Streamer, &NextChunk);
    } else {
        NextChunk = this->m_FirstChunk;
        this->m_FirstChunk = 0;
    }
    return NextChunk;
}

void DECODER::ReleaseChunk(CHUNK *Data) {
    this->m_IData.m_ReleaseDataFunc(this, this->m_IData.m_Streamer, Data);
}

CHUNK::CHUNK() {
    this->m_DataToDecode = 0;
    this->m_DataToDecodeSize = 0;
    this->m_UserChunkData = 0;
}

} // namespace RCMP
