#ifndef __BREPEAT_HPP__
#define __BREPEAT_HPP__ // Decl: 36

#include "types.h"

// total size: 0x24
// Decl: 40
struct bRepeatChannel {
    int32 RepeatSize;        // offset 0x0, size 0x4, Decl: 42
    int32 DefaultRemoveData; // offset 0x4, size 0x4, Decl: 43

    int32 AddDataSize;    // offset 0x8, size 0x4, Decl: 46
    int32 AddRepeatCount; // offset 0xC, size 0x4, Decl: 47
    int32 AddLocation;    // offset 0x10, size 0x4, Decl: 48
    int32 AddData;        // offset 0x14, size 0x4, Decl: 49

    int32 RemoveRepeatCount; // offset 0x18, size 0x4, Decl: 52
    int32 RemoveLocation;    // offset 0x1C, size 0x4, Decl: 53
    int32 RemoveData;        // offset 0x20, size 0x4, Decl: 54
};

// total size: 0x54
// Decl: 60
class bRepeat {
  public:
    void PrepareToAddData(int32 buffer_size, int32 num_channels, int32 enable_codeine_capturing); // Decl: 65

    // void InitChannel(int32 channel_number, int32 repeat_size, int32 default_data) {} // Decl: 70

    int32 AddData(int32 data, int32 data_size, int32 channel_number); // Decl: 73

    // int32 CanWeAddMoreData() {} // Decl: 76

    // int32 GetAmountAdded() {} // Decl: 79

    void PrepareToRemoveData(); // Decl: 82

    int32 RemoveData(int32 data_size, int32 channel_number); // Decl: 86

    // int32 IsMoreData() {} // Decl: 89

    // int WasRemoveDataError() {} // Decl: 94

    static int32 DoesCodeineHaveABufferForUs(); // Decl: 98

    void GetBufferFromCodeine(); // Decl: 101

    void TellCodeineWeAreFinishedRemovingData(); // Decl: 105

    // int GetNumItemsAdded() {}   // Decl: 113
    // int GetNumItemsRemoved() {} // Decl: 114

    // void SetRecordingTime(int time) {} // Decl: 117
    // int GetRecordingTime() {}          // Decl: 118

  private:
    int32 Magic;                // offset 0x0, size 0x4, Decl: 122
    int32 CodeineNeedsUpdating; // offset 0x4, size 0x4, Decl: 123
    int32 AddLocation;          // offset 0x8, size 0x4, Decl: 124
    int32 HeaderSize;           // offset 0xC, size 0x4, Decl: 125

    int32 RemoveLocation;         // offset 0x10, size 0x4, Decl: 127
    int32 AddBufferSize;          // offset 0x14, size 0x4, Decl: 128
    int32 NumChannels;            // offset 0x18, size 0x4, Decl: 129
    int32 EnableCodeineCapturing; // offset 0x1C, size 0x4, Decl: 130
    int32 NumItemsAdded;          // offset 0x20, size 0x4, Decl: 131
    int32 NumItemsRemoved;        // offset 0x24, size 0x4, Decl: 132
    int32 RemoveDataError;        // offset 0x28, size 0x4, Decl: 133

    int32 RecordingTime; // offset 0x2C, size 0x4, Decl: 135

    struct bRepeatChannel Channel[1]; // offset 0x30, size 0x24, Decl: 137

    void WriteToLocation(int32 location, int32 data, int32 size); // Decl: 139
    int32 ReadFromLocation(int32 location, int32 size);           // Decl: 140
    void ShovelDataOffToCodeine();                                // Decl: 141
};

#endif
