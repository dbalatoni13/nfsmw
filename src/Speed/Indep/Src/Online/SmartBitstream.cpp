#include "SmartBitstream.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"

IntQuantizer::IntQuantizer()
    : m_numBits(0), //
      m_minValue(0), //
      m_maxValue(0), //
      m_minValuePacked(0), //
      m_maxValuePacked(0), //
      m_isInitialized(false) {
    bStrCpy(m_name, "");
}

IntQuantizer::~IntQuantizer() {}

void IntQuantizer::Init(const char *name, int min_value, int max_value) {
    uint32 num_values;
    uint32 max_units;

    bStrCpy(m_name, name);
    m_minValue = min_value;
    m_maxValue = max_value;
    num_values = max_value - min_value + 1;
    max_units = 0;
    m_numBits = 0;
    while (max_units < num_values) {
        m_numBits++;
        max_units = 1 << m_numBits;
    }
    m_isInitialized = true;
    m_minValuePacked = max_value + 1;
    m_maxValuePacked = min_value - 1;
}

uint32 IntQuantizer::Pack(int value) {
    if (value < m_minValue) {
        value = m_minValue;
    }
    if (value > m_maxValue) {
        value = m_maxValue;
    }
    return value - m_minValue;
}

int IntQuantizer::UnPack(uint32 packed_value) { return m_minValue + packed_value; }

BitStream::BitStream() { Clear(); }

void BitStream::AddBits(uint32 value, int num_bits) {
    int byte_num;
    int bit_offset;

    byte_num = m_bitWritePosition >> 3;
    bit_offset = m_bitWritePosition & 7;
    m_bitWritePosition += num_bits;
    value <<= 32 - num_bits;

    if (bit_offset) {
        int bit_count;
        uint32 bit_value;

        bit_value = value >> (24 + bit_offset);
        m_buffer[byte_num] |= bit_value;
        bit_count = 8 - bit_offset;
        if (bit_count > num_bits) {
            bit_count = num_bits;
        }
        byte_num++;
        num_bits -= bit_count;
        value <<= bit_count;
    }

    if (num_bits > 8) {
        for (int a = 0; a < num_bits; a += 8) {
            m_buffer[byte_num++] = value >> 24;
            value <<= 8;
        }
    } else {
        m_buffer[byte_num] = value >> 24;
    }
}

void BitStream::GetBits(uint32 &value, int num_bits) {
    int read_len;
    int byte_num;
    int bit_offset;
    int bit_count;
    int shift;

    read_len = num_bits;
    byte_num = m_bitReadPosition / 8;
    bit_offset = m_bitReadPosition % 8;
    m_bitReadPosition += num_bits;
    bit_count = 8 - bit_offset;
    if (bit_count > num_bits) {
        bit_count = num_bits;
    }
    value = m_buffer[byte_num] << (24 + bit_offset);
    num_bits -= bit_count;
    shift = 24 - bit_count;
    byte_num++;
    while (shift > 0) {
        num_bits -= 8;
        value |= m_buffer[byte_num] << shift;
        byte_num++;
        shift -= 8;
    }
    if (num_bits > 0) {
        value |= m_buffer[byte_num] >> -shift;
    }
    value >>= 32 - read_len;
}

void BitStream::PokeBytes(const char *buffer, int num_bytes) {
    bMemCpy(m_buffer, buffer, num_bytes);
    m_isWritePositionPrecise = false;
    m_bitWritePosition = num_bytes << 3;
}

void BitStream::Clear() {
    m_bitWritePosition = 0;
    m_bitReadPosition = 0;
    m_isWritePositionPrecise = true;
    m_buffer[0] = 0;
}

void SmartBitStream::AddRawData(const char *data, int num_bytes) {
    AddShort(num_bytes);
    for (int i = 0; i < num_bytes; i++) {
        AddByte(data[i]);
    }
}

int SmartBitStream::GetRawData(char *buffer, int bufsize) {
    int16 length;
    length = GetShort();
    for (int i = 0; i < length; i++) {
        uint32 temp = 0;
        GetBits(temp, 8);
        buffer[i] = *reinterpret_cast<volatile uint32 *>(&temp);
    }
    return length;
}

void SmartBitStream::AddTerminatedString(const char *text) { AddRawData(text, bStrLen(text)); }

void SmartBitStream::GetTerminatedString(char *buffer, int bufsize) {
    buffer[GetRawData(buffer, bufsize)] = '\0';
}

void SmartBitStream::AddRawDataWithoutSize(const char *data, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        AddByte(data[i]);
    }
}

int SmartBitStream::GetRawDataWithoutSize(char *buffer, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
        buffer[i] = GetByte();
    }
    return num_bytes;
}

void SmartBitStream::AddFloat(float value) {
    int v = 0;
    bMemCpy(&v, &value, sizeof(v));
    AddInt(v);
}

void SmartBitStream::GetFloat(float &value) {
    int v = GetInt();
    bMemCpy(&value, &v, sizeof(v));
}

float SmartBitStream::GetFloat() {
    float value = 0.0f;
    GetFloat(value);
    return value;
}

FloatQuantizer::FloatQuantizer()
    : m_numBits(0), //
      m_minValue(0.0f), //
      m_maxValue(0.0f), //
      m_maxError(0.0f), //
      m_increment(0.0f), //
      m_minValuePacked(0.0f), //
      m_maxValuePacked(0.0f), //
      m_isInitialized(false) {
    bStrCpy(m_name, "");
}

FloatQuantizer::~FloatQuantizer() {}

void FloatQuantizer::Init(const char *name, float min_value, float max_value, float max_acceptable_error) {
    float range;
    uint32 num_values;
    uint32 max_units;

    bStrCpy(m_name, name);
    m_minValue = min_value;
    m_maxValue = max_value;
    m_maxError = max_acceptable_error;
    m_increment = max_acceptable_error * 2.0f;
    range = max_value - min_value;
    num_values = static_cast<uint32>(range / m_increment + 0.5) + 1;
    max_units = 0;
    m_numBits = 0;
    while (max_units < num_values) {
        m_numBits++;
        max_units = 1 << m_numBits;
    }
    m_increment = range / (max_units - 1);
    m_maxError = m_increment * 0.5f;
    m_minValuePacked = FLT_MAX;
    m_maxValuePacked = -FLT_MAX;
    m_isInitialized = true;
}

void FloatQuantizer::Init(const char *name, float min_value, float max_value, int num_bits) {
    float range;
    uint32 max_units;

    bStrCpy(m_name, name);
    m_minValue = min_value;
    m_maxValue = max_value;
    m_numBits = num_bits;
    range = max_value - min_value;
    max_units = (1 << num_bits) - 1;
    m_increment = range / max_units;
    m_maxError = m_increment * 0.5f;
    m_minValuePacked = FLT_MAX;
    m_maxValuePacked = -FLT_MAX;
    m_isInitialized = true;
}

uint32 FloatQuantizer::Pack(float value) {
    if (value < m_minValue) {
        value = m_minValue;
    }
    if (value > m_maxValue) {
        value = m_maxValue;
    }
    return static_cast<uint32>((value - m_minValue) / m_increment + 0.5f);
}

float FloatQuantizer::UnPack(uint32 packed_value) {
    float value;

    value = m_minValue + packed_value * m_increment;
    if (value < m_minValue) {
        value = m_minValue;
    }
    if (value > m_maxValue) {
        value = m_maxValue;
    }
    return value;
}

void FloatQuantizer::PrintUsageReport() {}
