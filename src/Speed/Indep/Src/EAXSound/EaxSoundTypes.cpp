#include <types.h>

namespace Speech {

struct SpeechSampleData;

struct ScheduledSpeechEvent {
    char _pad[0x3A];
    unsigned char curndx; // offset 0x3A

    void *GetData(unsigned int *datasize);
    unsigned char ReserveSample();

    static void *operator new(unsigned int size, unsigned int alignment);
    static void operator delete(void *ptr);
};

void *ScheduledSpeechEvent::GetData(unsigned int *datasize) {
    void *data = reinterpret_cast<char *>(this) + 0x40;
    if (datasize != nullptr) {
        *datasize = 0x40;
    }
    return data;
}

unsigned char ScheduledSpeechEvent::ReserveSample() {
    unsigned char ndx = curndx;
    curndx = static_cast<unsigned char>(ndx + 1);
    return static_cast<unsigned char>(ndx);
}

} // namespace Speech
