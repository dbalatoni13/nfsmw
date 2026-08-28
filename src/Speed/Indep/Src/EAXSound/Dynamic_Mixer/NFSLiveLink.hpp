//
//
#ifndef NFSLIVELINK_HPP
#define NFSLIVELINK_HPP // Decl: 4

// Decl: 8
#define BUFFER_WRITE(Pointer, Value)                                                                                                                 \
    *Pointer = Value;                                                                                                                                \
    Pointer++;

// total size: 0x98
// Decl: 38
class NFSLiveLink {
  public:
    NFSLiveLink();  // Decl: 40
    ~NFSLiveLink(); // Decl: 41

    void ProcessLiveLink(float dt); // Decl: 48

    void CheckInputBuffer(); // Decl: 53

    void ReceiveData();       // Decl: 60
    void ClearInputBuffer();  // Decl: 61
    void ClearOutputBuffer(); // Decl: 62

    void ReadVolPresets();    // Decl: 64
    void ReadMasterVolumes(); // Decl: 65

    void SetupChannelMonitoring();  // Decl: 71
    void UpdateChannelMonitoring(); // Decl: 72

    int *CurOutputPtr;    // offset 0x0, size 0x4, Decl: 69
    bool bMonitorChannel; // offset 0x4, size 0x1, Decl: 54
    int ChunkSize;        // offset 0x8, size 0x4, Decl: 75
    bool bFirstRun;       // offset 0xC, size 0x1, Decl: 76
    int InstID;           // offset 0x10, size 0x4, Decl: 93
    int ChannelID;        // offset 0x14, size 0x4, Decl: 94
    int InputID[32];      // offset 0x18, size 0x80, Decl: 95
};

#endif
