#ifndef ONLINE_BITSTREAM_HPP
#define ONLINE_BITSTREAM_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class BitStream {
  public:
    BitStream();

    void AddBits(uint32 value, int num_bits);
    void GetBits(uint32 &value, int num_bits);
    uint32 GetBits(int num_bits) {
        uint32 temp;
        GetBits(temp, num_bits);
        return temp;
    }
    void PokeBytes(const char *buffer, int num_bytes);
    int GetBitLength() const { return m_bitWritePosition; }
    int GetByteLength() const { return (m_bitWritePosition + 7) >> 3; }
    int GetByteLengthRemaining() const { return (GetBitLengthRemaining() + 7) >> 3; }
    int GetBitLengthRemaining() const { return m_bitWritePosition - m_bitReadPosition; }
    char *GetBuffer() { return reinterpret_cast<char *>(m_buffer); }
    void Clear();
    void Rewind() { m_bitReadPosition = 0; }

  private:
    static int MAX_BITS;
    static int BIT_DEPTH;

    /* 0x000 */ unsigned char m_buffer[1024];
    /* 0x400 */ int m_bitWritePosition;
    /* 0x404 */ int m_bitReadPosition;
    /* 0x408 */ bool m_isWritePositionPrecise;
};

#endif
