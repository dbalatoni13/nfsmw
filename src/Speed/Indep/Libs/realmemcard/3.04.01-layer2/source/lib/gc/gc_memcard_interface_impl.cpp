#include <string.h>

#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"
#include "../../../include/common/realmemcard/memcard_utilities.h"

namespace RealmcIface {

Realmc::CardID _ChangeToRealmcCardId(CardId cardId) {
    Realmc::CardID memcard;

    memcard.slot = 1;
    if (cardId == PORT1_DEFAULT) {
        memcard.slot = 0;
    }
    return memcard;
}

void _SplitPath(const char *path, char *filename) {
    char tempPath[63];
    char *separator;
    unsigned int length;

    strcpy(tempPath, path);
    separator = nullptr;
    length = strlen(path);
    if (length != 0) {
        unsigned int pos;

        pos = length;
        for (; pos > 0; pos--) {
            if (tempPath[pos] == '\\' || tempPath[pos] == '/') {
                tempPath[pos] = 0;
                separator = tempPath + pos;
            }
            if (separator != nullptr) {
                break;
            }
        }
    }
    strncpy(filename, separator + 1, 0x1f);
}

unsigned int MemcardInterfaceImpl::_CalcSignature(const void *data, unsigned int size) {
    return RealmcUtils::Crc32(data, size);
}

void FileHeader::Clear() {
    this->mFileHeaderVersion = 0;
    this->mFileSize = 0;
    this->mUserHeaderSize = 0;
    this->mUserBodySize = 0;
    memset(&this->mUserHeaderSignature, 0, sizeof(this->mUserHeaderSignature));
    memset(&this->mUserBodySignature, 0, sizeof(this->mUserBodySignature));
    memset(&this->mFileHeaderSignature, 0, sizeof(this->mFileHeaderSignature));
}

} // namespace RealmcIface

namespace Realmc {

GCMessage::~GCMessage() {}

void GCMessage::Init() {
    this->Clear();
}

void GCMessage::_SetMsgOptions(int options) {
    int iOption;
    int curOption;

    this->info.trc.mNumOptions = 0;
    if (options != 0) {
        while (options != 0) {
            iOption = this->info.trc.mNumOptions;
            curOption = options & 0xff;
            this->info.trc.mNumOptions++;
            this->info.trc.mOptions[iOption].mMsgId = curOption;
            this->info.trc.mOptions[iOption].mMsg = Locale::GetString(curOption, nullptr);
            options >>= 8;
        }
    }
}

short *GCMessage::_LcGetSlotString(int slotnum) {
    static short slotA[2] = {'A', 0};
    static short slotB[2] = {'B', 0};

    if (slotnum == 1) {
        return slotB;
    }
    return slotA;
}

void GCMessage::Clear() {
    memset(this, 0, 0x78);
}

} // namespace Realmc
