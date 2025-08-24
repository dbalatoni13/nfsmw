#ifndef MISC_TABLE_H
#define MISC_TABLE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

class TableBase {
    // total size: 0x10
    int NumEntries;        // offset 0x0, size 0x4
    float MinArg;          // offset 0x4, size 0x4
    float MaxArg;          // offset 0x8, size 0x4
    float IndexMultiplier; // offset 0xC, size 0x4
};

class Table : public TableBase {

  public:
    float GetValue(float arg);
    float InverseLookup(float value);

    Table() {}

    Table(const float *table, int num, float min, float max) {}

    const float *GetData() const {}

    void SetData(const float *data, int num) {}

  protected:
    // total size: 0x14
    const float *pTable; // offset 0x10, size 0x4
};

template <typename T> class tTable : public TableBase {
    T *pTable;

  public:
};

#endif
