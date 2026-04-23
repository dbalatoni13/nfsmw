#ifndef WORLD_CARPART_H
#define WORLD_CARPART_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xE
struct CarPart {
    unsigned short PartNameHashBot;          // offset 0x0, size 0x2
    unsigned short PartNameHashTop;          // offset 0x2, size 0x2
    char PartID;                             // offset 0x4, size 0x1
    unsigned char GroupNumber_UpgradeLevel;  // offset 0x5, size 0x1
    char BaseModelNameHashSelector;          // offset 0x6, size 0x1
    unsigned char CarTypeNameHashIndex;      // offset 0x7, size 0x1
    unsigned short NameOffset;               // offset 0x8, size 0x2
    unsigned short AttributeTableOffset;     // offset 0xA, size 0x2
    unsigned short ModelNameHashTableOffset; // offset 0xC, size 0x2

    char GetUpgradeLevel() { return GroupNumber_UpgradeLevel >> 5; }
    char GetGroupNumber() { return GroupNumber_UpgradeLevel & 0x1f; }

    const char *GetName();
    unsigned int GetCarTypeNameHash();
    unsigned int GetPartNameHash();
    char GetPartID();
    int HasAppliedAttribute(unsigned int namehash);
    const char *GetAppliedAttributeString(unsigned int namehash, const char *default_string);
    float GetAppliedAttributeFParam(unsigned int namehash, float default_value);
    int GetAppliedAttributeIParam(unsigned int namehash, int default_value);
    unsigned int GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value);
};

#endif
