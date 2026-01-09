#ifndef GENERATED_MESSAGES_MREQBACKUP_H
#define GENERATED_MESSAGES_MREQBACKUP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/Hermes.h"

// total size: 0x14
class MReqBackup : public Hermes::Message {
  public:
    static std::size_t _GetSize() {
        return sizeof(MReqBackup);
    }

    static UCrc32 _GetKind() {
        static UCrc32 k("MReqBackup");

        return k;
    }

    MReqBackup(int _BackupType) : Hermes::Message(_GetKind(), _GetSize(), 0), fBackupType(_BackupType) {}

    int GetBackupType() const {
        return fBackupType;
    }

  private:
    int fBackupType; // offset 0x10, size 0x4
};

#endif
