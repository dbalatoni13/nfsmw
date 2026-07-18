#ifndef ONLINE_SMARTBITSTREAM_HPP
#define ONLINE_SMARTBITSTREAM_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "BitStream.hpp"
#include "FloatQuantizer.hpp"
#include "IntQuantizer.hpp"

class SmartBitStream : public BitStream {
  public:
    void AddBool(bool value) { AddBits(value, 1); }
    void GetBool(bool &value) {
        uint32 temp = 0;
        GetBits(temp, 1);
        value = temp;
    }
    bool GetBool() {
        bool value;
        GetBool(value);
        return value;
    }
    void AddByte(uint8 value) { AddBits(value, 8); }
    void GetByte(uint8 &value) {
        uint32 temp = 0;
        GetBits(temp, 8);
        value = temp;
    }
    uint8 GetByte() {
        uint32 temp = 0;
        GetBits(temp, 8);
        return temp;
    }
    void AddShort(short value) { AddBits(value, 16); }
    void GetShort(short &value) {
        uint32 temp = 0;
        GetBits(temp, 16);
        value = temp;
    }
    short GetShort() {
        uint32 temp = 0;
        GetBits(temp, 16);
        return *reinterpret_cast<short *>(&temp);
    }
    void AddInt(int value) { AddBits(value, 32); }
    void GetInt(int &value) {
        uint32 temp = 0;
        GetBits(temp, 32);
        value = temp;
    }
    int GetInt() {
        uint32 temp = 0;
        GetBits(temp, 32);
        return temp;
    }
    void AddRawData(const char *data, int num_bytes);
    int GetRawData(char *buffer, int bufsize);
    void AddTerminatedString(const char *text);
    void GetTerminatedString(char *buffer, int bufsize);
    void AddRawDataWithoutSize(const char *data, int num_bytes);
    int GetRawDataWithoutSize(char *buffer, int num_bytes);
    void AddFloat(float value);
    void GetFloat(float &value);
    float GetFloat();
};

#endif
