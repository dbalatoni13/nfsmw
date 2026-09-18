#include <string.h>

#include "../../../include/common/realmemcard/memcard_interface.h"
#include "../../../include/common/realmemcard/impl/memcard_interface_impl.h"

namespace RealmcIface {

static MemcardInterface *gInstance = nullptr;

MemcardInterface *MemcardInterface::CreateInstance(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo) {
    Realmc::SetMemAllocator(iSystem->mAllocator);
    if (gInstance == nullptr) {
        gInstance = new MemcardInterface(iSystem, iGame, gameInfo);
    }
    return gInstance;
}

MemcardInterface::MemcardInterface(Realmc::SystemInterface *iSystem, IGameInterface *iGame, GameInfo *gameInfo) {
    this->mImpl = new MemcardInterfaceImpl(iSystem, iGame, gameInfo);
}

void MemcardInterface::BootupCheck(const BootupCheckParams *params, unsigned int nEntries, const char **entryNames, wchar_t *content) {
    this->mImpl->TaskManagerBootupCheck(params, nEntries, entryNames, content);
}

void MemcardInterface::Save(const char *entryName, const char *header, const char *body, const SaveInfo *saveInfo, const TitleInfo *titleInfo) {
    if (titleInfo != nullptr) {
        this->mImpl->TaskManagerSave(entryName, header, body, saveInfo, titleInfo);
    } else {
        this->mImpl->TaskManagerSave(entryName, header, body, saveInfo, nullptr);
    }
}

void MemcardInterface::Load(const char *entryName, char *header, char *body, const wchar_t *contentName, const TitleInfo *titleInfo, const wchar_t *typeName) {
    if (titleInfo != nullptr && (titleInfo->mTitleType == TITLE_ALTERNATE || titleInfo->mDataFormat == FORMAT_RAW)) {
        this->mImpl->TaskManagerLoad(entryName, header, body, contentName, typeName, titleInfo);
    } else {
        this->mImpl->TaskManagerLoad(entryName, header, body, contentName, typeName, nullptr);
    }
}

void MemcardInterface::Delete(const char *entryName, const wchar_t *contentName) {
    this->mImpl->TaskManagerDelete(entryName, contentName);
}

void MemcardInterface::FindEntries(const char *entryNamePattern, const TitleInfo *titleInfo) {
    if (titleInfo != nullptr && (titleInfo->mDataFormat == FORMAT_RAW || titleInfo->mTitleType == TITLE_ALTERNATE)) {
        this->mImpl->TaskManagerFindEntries(entryNamePattern, titleInfo);
    } else {
        this->mImpl->TaskManagerFindEntries(entryNamePattern, nullptr);
    }
}

void MemcardInterface::MessageDone(MessageChoices choice) {
    this->mImpl->MessageDone(choice);
}

void MemcardInterface::CheckCard(CardId cardId) {
    this->mImpl->TaskManagerCheckCard(cardId);
}

void MemcardInterface::SetAutosave(AutosaveState state, unsigned int nSaveReqs, SaveReq **saveReqs, const char *entryName, CardId cardId) {
    this->mImpl->TaskManagerSetAutosave(state, nSaveReqs, saveReqs, entryName, cardId);
}

void MemcardInterface::SetMonitor(MonitorState state) {
    this->mImpl->TaskManagerSetMonitor(state);
}

void MemcardInterface::SetMessage(MessageState state, unsigned int message) {
    this->mImpl->SetMessage(state, message);
}

MemcardTask MemcardInterface::Update(unsigned int elapsedTime) {
    return this->mImpl->Update(elapsedTime);
}

bool MemcardInterface::IsResettable() {
    return this->mImpl->IsResettable();
}

void BootupCheckParams::Clear() {
    this->mEntryNamePattern = 0;
    this->mNumSaveTypes = 0;
    this->mSaveReqs = 0;
    this->mValidCardIds = -1;
}

void BootupCheckResults::Clear() {
    this->mFirstGoodCard = CARD_UNKNOWN;
    this->mEntryFound = false;
    this->mNumBlocksNeeded = 0;
}

CardInfo::CardInfo() {
    this->Clear();
}

void CardInfo::Clear() {
    this->mCardId = CARD_UNKNOWN;
    this->mStatus = STATUS_UNKNOWN;
    this->mFreeSpace = 0;
    this->mFreeFiles = 0;
    this->mTotalSpace = 0;
    this->mFreeSpaceOverLimit = false;
    this->mTotalSpaceOverLimit = false;
}

EntryInfo::EntryInfo() {
    this->Clear();
}

void EntryInfo::Clear() {
    this->mTime.Clear();
    this->mName = 0;
    this->mStatus = STATUS_ENTRY_CORRUPTED;
    this->mEntryBlocks = 0;
    this->mUserDataSize = 0;
    this->mCompanyCode[1] = 0;
    this->mCompanyCode[0] = 0;
    this->mGameCode[3] = 0;
    this->mGameCode[2] = 0;
    this->mGameCode[1] = 0;
    this->mGameCode[0] = 0;
}

GameInfo::GameInfo(const wchar_t *gameTitle, const unsigned int titleId, bool multipleSaveTypesUsed, bool multitapSupported) {
    memset(this->mGameTitle, 0, 0x42);
    memcpy(this->mGameTitle, gameTitle, Realmc::Locale::GetWstrLength(gameTitle) * 2);
    this->mTitleId = titleId;
    this->mMultipleSaveTypesUsed = multipleSaveTypesUsed;
    this->mMultitapSupported = multitapSupported;
}

GcSaveInfo::GcSaveInfo() {
    this->Clear();
}

void GcSaveInfo::Clear() {
    this->mComment1 = 0;
    this->mComment1Size = 0;
    this->mComment2 = 0;
    this->mComment2Size = 0;
    this->mIconDataInfo = 0;
    this->mBannerDataInfo = 0;
}

Ps2SaveInfo::Ps2SaveInfo() {
    this->Clear();
}

void Ps2SaveInfo::Clear() {
    this->mIconSysData = 0;
    this->mIconSysDataSize = 0;
    this->mStaticIconData = 0;
    this->mStaticIconDataSize = 0;
    this->mStaticIconFilename = 0;
    this->mCopyIconData = 0;
    this->mCopyIconDataSize = 0;
    this->mCopyIconFilename = 0;
    this->mDeleteIconData = 0;
    this->mDeleteIconDataSize = 0;
    this->mDeleteIconFilename = 0;
}

SaveInfo::SaveInfo() {
    this->Clear();
}

void SaveInfo::Clear() {
    this->mPs2Info.Clear();
    this->mXboxInfo.Clear();
    this->mGcInfo.Clear();
    this->mHeaderSize = 0;
    this->mBodySize = 0;
    this->mTypeName = 0;
    this->mContentName = 0;
}

SaveReq::SaveReq() {
    this->Clear();
}

void SaveReq::Clear() {
    this->mNumSaves = 0;
    this->mSaveInfo = 0;
}

TimeInfo::TimeInfo() {
    this->Clear();
}

void TimeInfo::Clear() {
    this->mCreated = 0;
    this->mLastModified = 0;
    this->mLastAccessed = 0;
}

void TitleInfo::Clear() {
    this->mTitleType = static_cast<TitleType>(0);
    this->mTitleId = 0;
    this->mNameType = static_cast<NameType>(0);
    this->mDataFormat = static_cast<DataFormat>(0);
}

void TitleInfo::Init(TitleType titleType, const unsigned int titleId, NameType nameType, DataFormat dataFormat) {
    this->mTitleType = titleType;
    this->mTitleId = titleId;
    this->mNameType = nameType;
    this->mDataFormat = dataFormat;
}

XboxSaveInfo::XboxSaveInfo() {
    this->Clear();
}

void XboxSaveInfo::Clear() {
    this->mImageData = 0;
    this->mImageDataSize = 0;
}

} // namespace RealmcIface
