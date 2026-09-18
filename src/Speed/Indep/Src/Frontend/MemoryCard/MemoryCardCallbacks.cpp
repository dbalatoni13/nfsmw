#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCardHelper.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern unsigned long g_GC_Disk_GameName;

void DisplayUnicode(const wchar_t *str);
void DisplayMessage(const wchar_t *msg, unsigned int count, const wchar_t **str);

MemcardCallbacks gMemcardCallbacks;

void DisplayStatus(int status) {}

MemoryCard *MemcardCallbacks::GetMemcard() {
    return MemoryCard::GetInstance();
}

UIMemcardBase *MemcardCallbacks::GetScreen() {
    return MemoryCard::GetInstance()->GetScreen();
}

void MemcardCallbacks::ShowMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t **options) {
    UIMemcardBase *pScreen;

    if (GetMemcard()->IsMemcardScreenExiting()) {
        return;
    }

    JLog(MJ_ShowMesssage);
    JLog(msg);
    JLog(nOptions);

    for (unsigned int i = 0; i < nOptions; i++) {
        JLog(options[i]);
    }

    DisplayMessage(msg, nOptions, options);

    GetMemcard()->m_bWaitingForResponse = true;

    if (GetMemcard()->IsAutoSaving() && gMemcardSetup.GetCommand() != MCO_AutoSave) {
        if (nOptions == 0) {
            GetMemcard()->m_bWaitingForResponse = false;
            return;
        }

        GetMemcard()->m_PendingMessage = BNEW MemoryCardMessage(msg, nOptions, options);
        GetMemcard()->HandleAutoSaveError();
        return;
    }

    switch (GetMemcard()->GetOp()) {
        case MemoryCard::MO_FakeLoad:
        case MemoryCard::MO_LoadYNCF:
            if (nOptions == 0) {
                return;
            }
            break;
    }

    pScreen = GetScreen();

    if (pScreen == nullptr) {
        return;
    }

    if (pScreen->IsInButtonAnimation()) {
        if (GetMemcard()->GetPendingMessage() != nullptr) {
            GetMemcard()->ReleasePendingMessage();
        }

        GetMemcard()->m_PendingMessage = BNEW MemoryCardMessage(msg, nOptions, options);
    } else {
        GetScreen()->ShowMessage(msg, nOptions, options[0], options[1], options[2]);
    }
}

void MemcardCallbacks::ClearMessage() {
    UIMemcardBase *pScreen;

    if (GetMemcard()->IsAutoSaving()) {
        return;
    }

    JLog(MJ_ClearMessage);

    switch (GetMemcard()->GetOp()) {
        case MemoryCard::MO_FakeLoad:
        case MemoryCard::MO_LoadYNCF:
            return;
    }

    pScreen = GetScreen();

    if (pScreen == nullptr) {
        return;
    }

    if (GetMemcard()->GetOp() == MemoryCard::MO_Save && pScreen->IsInButtonAnimation()) {
    }
}

void MemcardCallbacks::BootupCheckDone(RealmcIface::CardStatus status, RealmcIface::BootupCheckResults res) {
    JLog(MJ_BootupCheckDone);
    JLog(status);
    JLog(res.mEntryFound);

    GetMemcard()->m_MemOp = 0;
    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_LastError = static_cast<unsigned short>(status);
    GetMemcard()->m_SpecialError = static_cast<unsigned short>(status);

    if ((status != RealmcIface::STATUS_OK && GetMemcard()->GetPendingMessage() != nullptr) || status == RealmcIface::STATUS_UNKNOWN) {
        GetMemcard()->ReleasePendingMessage();
        GetMemcard()->BootupCheck(GetMemcard()->IsAutoLoading() && !FEDatabase->bProfileLoaded ? GetScreen()->m_FileName : nullptr);
    } else {
        GetMemcard()->m_pImp->BootupCheckDone(status, &res);
        GetMemcard()->m_bBootFoundFile = res.mEntryFound;

        if (GetMemcard()->m_bRetryBootCheck) {
            GetScreen()->SetStringCheckingCard();
        } else {
            cFEng::Get()->QueueGameMessage(0x461A18EE, GetScreen()->GetPackageName(), 0xFF);
        }
    }
}

void MemcardCallbacks::SaveCheckDone(RealmcIface::TaskResult result, RealmcIface::CardStatus status) {
    JLog(MJ_SaveCheckDone);
}

void MemcardCallbacks::SaveDone(const char *filename) {
    JLog(MJ_SaveDone);
    JLog(filename);

    if (GetMemcard()->IsTypeProfile()) {
        bFree(GetMemcard()->m_pBuffer);
    }

    GetMemcard()->m_pImp->DestructSaveInfo();
    GetMemcard()->m_pBuffer = nullptr;
    GetMemcard()->m_MemOp = 0;

    FEDatabase->bProfileLoaded = true;
    FEDatabase->SetOptionsDirty(false);

    GetMemcard()->m_bCardRemoved = false;

    if (GetMemcard()->IsManualSave() && gMemcardSetup.GetCommand() != MCO_AutoSave) {
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            GetMemcard()->SetRetryAutoSave(false);
            GetMemcard()->SetAutoSaveEnabled(true);
        } else {
            cFEng::Get()->QueueGameMessage(0x461A18EE, nullptr, 0xFF);
        }
    } else if (GetMemcard()->IsAutoSaving() || gMemcardSetup.GetCommand() == MCO_AutoSave) {
        GetMemcard()->m_bAutoSaveCardPulled = false;

#ifndef EA_BUILD_A124
        if (GetMemcard()->m_bFoundAutoSaveFile) {
            FEDatabase->bAutoSaveOverwriteConfirmed = true;
        }
#endif

        if (GetMemcard()->IsRetryingAutoSave()) {
            GetMemcard()->ShowMessages(false);
            GetMemcard()->SetRetryAutoSave(false);
            GetMemcard()->SetAutoSaveEnabled(true);
        }

        GetMemcard()->EndAutoSave();

        if (gMemcardSetup.GetCommand() == MCO_AutoSave) {
            cFEng::Get()->QueueGameMessage(0x461A18EE, nullptr, 0xFF);
        }
    }
}

RealmcIface::DataStatus MemcardCallbacks::CheckLoadedData(const char *data) {
    JLog(MJ_CheckLoadedData);

    return RealmcIface::DATA_OK;
}

void MemcardCallbacks::LoadDone(const char *filename) {
    unsigned int *pHeader;
    unsigned int iStoredVersion;
    unsigned int iStoredSize;
    bool isProfileValid;

    JLog(MJ_LoadDone);
    JLog(filename);
    JLog(GetMemcard()->GetHeader(), 8);
    JLog(GetMemcard()->GetData(), GetMemcard()->GetSize());

    pHeader = reinterpret_cast<unsigned int *>(GetMemcard()->GetHeader());
    iStoredVersion = pHeader[0];
    iStoredSize = pHeader[1];

    MemoryCard::GetInstance()->m_MemOp = 0;

    if (iStoredVersion == 0x10D && iStoredSize == GetMemcard()->GetSize() && GetMemcard()->IsTypeProfile()) {
        if (FEDatabase->LoadUserProfileFromBuffer(GetMemcard()->GetData(), GetMemcard()->GetSize(), GetMemcard()->GetPlayerNum())) {
            FEDatabase->DeallocBackupDB();

            if (GetMemcard()->GetPlayerNum() != 0) {
                if (GetMemcard()->m_pBuffer != nullptr) {
                    bFree(GetMemcard()->m_pBuffer);
                    GetMemcard()->m_pBuffer = nullptr;
                }

                cFEng::Get()->QueueGameMessage(0x461A18EE, nullptr, 0xFF);
                return;
            }

            FEDatabase->bProfileLoaded = true;
            GetMemcard()->m_bCardRemoved = false;

            if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
                if (GetMemcard()->m_pBuffer != nullptr) {
                    bFree(GetMemcard()->m_pBuffer);
                    GetMemcard()->m_pBuffer = nullptr;
                }

                GetMemcard()->SetAutoSaveEnabled(true);
            } else {
                cFEng::Get()->QueueGameMessage(gMemcardSetup.GetCommand() == MCO_BootList ? 0xA4BB7AE1 : 0x461A18EE, nullptr, 0xFF);
            }
        } else {
            GetMemcard()->ShowMessages(false);
            FEDatabase->RestoreFromBackupDB();
            cFEng::Get()->QueueGameMessage(0xF35D144E, nullptr, 0xFF);
        }
    } else {
        FEDatabase->RestoreFromBackupDB();
        cFEng::Get()->QueueGameMessage(0xF35D144E, nullptr, 0xFF);
    }

    if (GetMemcard()->m_pBuffer != nullptr) {
        bFree(GetMemcard()->m_pBuffer);
        GetMemcard()->m_pBuffer = nullptr;
    }

    FEDatabase->DeallocBackupDB();
}

void MemcardCallbacks::DeleteDone(const char *filename) {
    int idx;

    JLog(MJ_DeleteDone);
    JLog(filename);

    idx = GetMemcard()->GetPrefixLength();

    if (bStrCmp(&filename[idx], FEDatabase->GetUserProfile(0)->GetProfileName()) == 0) {
        FEDatabase->DefaultProfile();
        FEDatabase->bProfileLoaded = false;
    }

    GetMemcard()->m_MemOp = 0;
    cFEng::Get()->QueueGameMessage(0x461A18EE, GetScreen()->GetPackageName(), 0xFF);
}

void MemcardCallbacks::ClearEntries() {
    JLog(MJ_ClearEntries);
}

void MemcardCallbacks::FoundEntry(const RealmcIface::EntryInfo *info) {
    unsigned int iSize;
    int iGuessSize;
    unsigned int fDefault;

    JLog(MJ_FoundEntry);
    JLog(info);

    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->m_bOldSaveFileExists = true;
        return;
    }

    if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->m_bFoundAutoSaveFile = true;
        return;
    }

    if (bStrNCmp(reinterpret_cast<const char *>(&g_GC_Disk_GameName), info->mGameCode, 4) != 0) {
        return;
    }

    iSize = GetMemcard()->GetSize();
    iGuessSize = info->mUserDataSize;
    fDefault = info->mStatus != RealmcIface::STATUS_OK ? MCFF_Corrupted : MCFF_OK;

    if (GetMemcard()->IsTypeProfile()) {
        unsigned int sec = GetMemcard()->m_TimeOffsetSec;

        // r76 (despineo-fe): LAS TRES BARRERAS RETIRADAS, y con ellas las dos
        // locales que existian solo para tener algo que atar. Eran:
        //     UIMemcardBase *scr = GetScreen();
        //     const char *nm = info->mName;
        //     __asm__("" : "+r"(nm)); __asm__("" : "+r"(iGuessSize));
        //     __asm__("" : "+r"(fDefault));
        //     scr->AddItem(nm, "", iGuessSize, fDefault);
        // Precio medido: 100 % -> 97,969 %, 512/512 B y TRES filas.
        //
        // EL DWARF DA LA RAZON A LA FORMA LIMPIA (`dwarf1.py fn 0x80139CFC`,
        // 512 B): this r29, info r31, y de locales solo g_GC_Disk_GameName,
        // iSize (optimizada fuera), iGuessSize r30 y fDefault r28. Ni `scr` ni
        // `nm` existen, y iGuessSize/fDefault caen en r30/r28 --los registros
        // del original-- sin necesidad de barrera ninguna.
        //
        // LO QUE FALTA son 2 instrucciones de 128: el par
        // `lis r5,$LC@ha` / `addi r5,r5,$LC@l` del literal "" nos sale DELANTE
        // del `lwz r4,0(r31)` de mName, y el objetivo emite
        //     lwz -> lis -> mr r6 -> mr r7 -> addi
        // Es ORDEN DE EMISION, no reparto, y esta cerrado como diagnostico:
        //   * prioridades de sched2 empatadas (lwz->call = 2, lis->addi->call = 2)
        //     y el desempate es INSN_LUID, o sea el orden en que expand_call
        //     materializo los argumentos;
        //   * el `high` nace en precompute_register_parameters (calls.c:652):
        //     con -fforce-addr la direccion del literal se fuerza a registro
        //     (expr.c:8110) ANTES de que load_register_parameters cree el `lwz`
        //     de mName. Un literal SIEMPRE pasa por precompute, asi que su LUID
        //     es menor haga lo que haga la fuente;
        //   * medido con los volcados de la forma limpia suelta:
        //       creacion (LUID): 407 high($LC) < 410 this < 412 lwz r4
        //                        < 414 lo_sum r5 < 416 mr r6 < 418 mr r7
        //       tras sched1 (.lreg):  412 407 414 416 418 410  <- lwz delante,
        //                             como el objetivo
        //       tras sched2 (final):  407 412 414 416 418      <- lis delante
        //   * el objetivo ademas deja el `addi` DETRAS de los dos `mr`, lo que
        //     pide un LUID del lo_sum mayor que el de 416/418: la direccion se
        //     materializo despues de los argumentos enteros.
        // Quien vuelva tiene que atacar expand_call, no el reparto. Un simbolo
        // con nombre (static const char[]) en vez del literal cambiaria la
        // reubicacion, asi que tampoco vale.
        //
        // FORMAS MEDIDAS (historico, NO repetir):
        //   * forma limpia sin locales ni barreras: 512/512 B, 3 filas, 97,969 %.
        //     Es la que MENOS deuda tiene, y es la del DWARF. Es esta.
        //   * solo `nm` + su barrera (sin `scr`): PEOR -- 516 B, un preservado
        //     mas (r27), marco de 0x20 y 8 filas.
        //   * las tres barreras en UN solo `asm` de tres operandos: 98,4375 %
        //     (peor que con las tres sueltas; era el andamio, ya retirado).
        // OJO AL EDITAR ESTE FICHERO: hay un `BNEW` mas abajo, y BNEW es
        // `new (__FILE__, __LINE__)`. Cualquier linea que se anada o se quite
        // POR ENCIMA de el cambia su `li r5` y mueve el .text. Esta nota se
        // dimensiono para dejar el fichero con el MISMO numero de lineas que
        // tenia con el andamio dentro; si la tocas, compensa la diferencia.
        //
        // Portabilidad (el motivo del lote r76): eran `__asm__` de GCC y hacian
        // que este fichero no compilase fuera del toolchain de GameCube; sin
        // ellas la sonda X360 lo da por bueno.
        GetScreen()->AddItem(info->mName, "", iGuessSize, fDefault);
    } else {
        if (info->mStatus != RealmcIface::STATUS_OK) {
            return;
        }

        unsigned int iOffset = GetMemcard()->m_EntryCount * 16;
        char *pNameBuf = GetMemcard()->m_pBuffer;

        bStrNCpy(&pNameBuf[iOffset], info->mName, 16);
    }
    GetMemcard()->m_EntryCount++;
}

void MemcardCallbacks::FindEntriesDone(RealmcIface::CardStatus status) {
    JLog(MJ_FindEntriesDone);
    JLog(status);

    GetMemcard()->m_MemOp = 0;
    GetMemcard()->SetListingForCreate(false);

    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->EndListingOldSaveFiles();
    } else if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->m_bCheckingCardForOverwrite = false;

        if (GetMemcard()->m_bFoundAutoSaveFile) {
            GetMemcard()->HandleAutoSaveOverwriteMessage();
        } else {
            GetMemcard()->DoAutoSave();
        }
    } else {
        cFEng::Get()->QueueGameMessage(0x5A051729, GetScreen()->GetPackageName(), 0xFF);

        GetMemcard()->SetBootFound(GetMemcard()->m_EntryCount > 0);
    }
}

void MemcardCallbacks::Retry(RealmcIface::CardStatus status) {
    JLog(MJ_Retry);
    JLog(status);

    if (GetScreen() != nullptr) {
        GetScreen()->SetStringCheckingCard();

        if (GetMemcard()->GetOp() == MemoryCard::MO_List) {
            GetScreen()->EmptyFileList();
        }
    }
}

void MemcardCallbacks::Failed(RealmcIface::TaskResult result, RealmcIface::CardStatus status) {
    unsigned int msg;

    JLog(MJ_Failed);
    JLog(status);
    JLog(result);

    if (GetMemcard()->IsWaitingForResponse() && (GetMemcard()->GetOp() == MemoryCard::MO_Delete || GetMemcard()->GetOp() == MemoryCard::MO_Load)) {
        GetMemcard()->m_MemOp = 0;

        if (GetMemcard()->GetOp() == MemoryCard::MO_Delete) {
            GetMemcard()->Delete(nullptr);
        } else {
            GetMemcard()->Load(nullptr);
        }

        return;
    }

    msg = 0x8867412D;

    if (GetMemcard()->m_pBuffer != nullptr) {
        bFree(GetMemcard()->m_pBuffer);
        GetMemcard()->m_pBuffer = nullptr;
    }

    if (GetMemcard()->m_pImp->GetSaveInfo() != nullptr) {
        GetMemcard()->m_pImp->DestructSaveInfo();
    }

    if (GetMemcard()->IsAutoSaving() || GetMemcard()->IsCheckingCardForAutoSave()) {
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->EndAutoSave();

        if (gMemcardSetup.GetCommand() == MCO_AutoSave) {
            cFEng::Get()->QueueGameMessage(0x8867412D, nullptr, 0xFF);
        }

        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
        return;
    }

    if (GetMemcard()->m_bListingOldSaveFiles) {
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->EndListingOldSaveFiles();
        return;
    }

    if (GetMemcard()->IsRetryingAutoSave()) {
        GetMemcard()->SetRetryAutoSave(false);
        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;

        if (result == RealmcIface::RESULT_CANCELLED || status == RealmcIface::STATUS_CARD_UNFORMATTED) {
            msg = 0xFE202E3B;
        }
    }

    if (gMemcardSetup.GetCommand() == MCO_CreateNew && GetMemcard()->GetOp() == MemoryCard::MO_List) {
        GetMemcard()->SetListingForCreate(false);
        GetMemcard()->m_MemOp = 0;
        cFEng::Get()->QueueGameMessage(0x5A051729, GetScreen()->GetPackageName(), 0xFF);
        return;
    }

    switch (GetMemcard()->GetOp()) {
        case MemoryCard::MO_Save:
            switch (status) {
                case RealmcIface::STATUS_NO_CARD:
                case RealmcIface::STATUS_CARD_DAMAGED:
                case RealmcIface::STATUS_WRONG_DEVICE:
                    if (gMemcardSetup.GetCommand() == MCO_CreateNew) {
                        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                    }
                    break;
            }

            msg = 0xDC12AF2E;
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;

        case MemoryCard::MO_Load:
            if (GetMemcard()->IsTypeProfile()) {
                bFree(GetMemcard()->m_pBuffer);
            }

            GetMemcard()->m_pBuffer = nullptr;
            GetMemcard()->m_SpecialError = static_cast<unsigned short>(status);
            break;

        case MemoryCard::MO_BootUp:
            GetMemcard()->m_pImp->DestructSaveInfo();
            break;

        case MemoryCard::MO_List:
            if (GetMemcard()->InBootSequence()) {
                msg = 0x8867412D;
            }
            break;

        case MemoryCard::MO_AutoSave:
            break;
    }

    GetMemcard()->m_LastError = static_cast<unsigned short>(status);
    GetMemcard()->m_MemOp = 0;

    DisplayStatus(status);

    if (status == RealmcIface::STATUS_RETRY_BOOT_FLOW) {
        GetMemcard()->BootupCheck(nullptr);
        GetMemcard()->m_bRetryBootCheck = true;
    } else {
        cFEng::Get()->QueueGameMessage(msg, GetScreen()->GetPackageName(), 0xFF);
    }
}

void MemcardCallbacks::CardChanged(RealmcIface::TaskResult result, RealmcIface::CardStatus status) {
    if ((result == RealmcIface::RESULT_RETRY && status == RealmcIface::STATUS_CARD_CHANGED) || status == RealmcIface::STATUS_OK) {
        cFEng::Get()->QueueGameMessage(0x3A2BE557, nullptr, 0xFF);
    } else if (result == RealmcIface::RESULT_CANCELLED) {
        cFEng::Get()->QueueGameMessage(0x8867412D, nullptr, 0xFF);
    }
}

void MemcardCallbacks::CardChecked(const RealmcIface::CardInfo *info) {
    unsigned int msg;
    UIMemcardBase *pScreen;

    JLog(MJ_CardChecked);
    JLog(info);

    msg = 0x8867412D;

    if (GetMemcard()->IsCheckingCardForAutoSave()) {
        GetMemcard()->m_MemOp = 0;
        GetMemcard()->m_LastError = static_cast<unsigned short>(info->mStatus);

        switch (info->mStatus) {
            case RealmcIface::STATUS_CARD_CHANGED:
            case RealmcIface::STATUS_CARD_UNFORMATTED:
            case RealmcIface::STATUS_CARD_DAMAGED:
            case RealmcIface::STATUS_WRONG_DEVICE:
            case RealmcIface::STATUS_CARD_ERROR:
                GetMemcard()->m_bFoundAutoSaveFile = true;
                GetMemcard()->DoAutoSave();
                break;

            case RealmcIface::STATUS_OK:
#ifndef EA_BUILD_A124
                if (!FEDatabase->bAutoSaveOverwriteConfirmed) {
                    char fileName[32];
                    UserProfile *pProfile;

                    GetMemcard()->m_bCheckingCardForAutoSave = false;
                    GetMemcard()->m_bCheckingCardForOverwrite = true;
                    GetMemcard()->ShowMessages(true);

                    pProfile = FEDatabase->GetMultiplayerProfile(0);
                    bStrCat(fileName, GetMemcard()->GetPrefix(), pProfile->GetProfileName());
                    GetMemcard()->m_bFoundAutoSaveFile = false;
                    GetMemcard()->List(fileName, nullptr);
                    break;
                }
#endif
                GetMemcard()->DoAutoSave();
                break;

            case RealmcIface::STATUS_NO_CARD:
                GetMemcard()->HandleAutoSaveError();
                break;
        }
    } else {
        MemoryCard::SetMessageMode(1, true);

        if (info->mStatus == RealmcIface::STATUS_OK) {
            msg = 0x461A18EE;
        }

        GetMemcard()->m_MemOp = 0;
        GetMemcard()->m_LastError = static_cast<unsigned short>(info->mStatus);

        pScreen = GetScreen();

        if (msg != 0 && pScreen != nullptr) {
            cFEng::Get()->QueueGameMessage(msg, pScreen->GetPackageName(), 0xFF);
        }
    }
}

void MemcardCallbacks::CardRemoved() {
    JLog(MJ_CardRemoved);

    GetMemcard()->m_bAutoSaveCardPulled = true;

    if (GetMemcard()->GetOp() == MemoryCard::MO_Save) {
        GetMemcard()->m_bAutoSaveCardPulledDuringSave = true;
    }

    if (GetMemcard()->IsCheckingCardForOverwrite()) {
        GetMemcard()->HandleAutoSaveError();
    } else {
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            if (!MemoryCard::GetInstance()->IsAutoSaving()) {
                GetMemcard()->m_bCardRemoved = true;
            }
        }

        FEDatabase->GetGameplaySettings()->AutoSaveOn = false;

        if (FEDatabase->IsOptionsMode()) {
            cFEng::Get()->QueueGameMessage(0x7E998E5E, nullptr, 0xFF);
        }

#ifndef EA_BUILD_A124
        FEDatabase->bAutoSaveOverwriteConfirmed = false;
#endif
    }
}

void MemcardCallbacks::SetAutosaveDone(RealmcIface::TaskResult res, RealmcIface::CardStatus status, RealmcIface::AutosaveState flag) {
    unsigned int msg;

    JLog(MJ_SetAutosaveDone);
    JLog(res);
    JLog(status);
    JLog(reinterpret_cast<unsigned int &>(flag));

    GetMemcard()->m_MemOp = 0;

    GetMemcard()->m_bAutoSave = flag == RealmcIface::AUTOSAVE_ENABLE;
    GetMemcard()->m_bAutoSaveCardPulled = false;
    GetMemcard()->m_bAutoSaveCardPulledDuringSave = false;

    if (GetMemcard()->m_bDisablingAutoSaveForSave) {
        GetMemcard()->m_bDisablingAutoSaveForSave = false;
        GetMemcard()->ShowMessages(true);

        cFEng::Get()->QueueGameMessage(0xC6C6B68F, GetMemcard()->IsMemcardScreenShowing() ? gMemcardSetup.mMemScreen : nullptr, 0xFF);
        return;
    }

    msg = 0x461A18EE;

    if (status != RealmcIface::STATUS_OK && flag != RealmcIface::AUTOSAVE_ENABLE) {
        switch (status) {
            case RealmcIface::STATUS_NO_CARD:
            case RealmcIface::STATUS_CARD_REMOVED:
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                msg = 0xB57FDB17;
                break;

            case RealmcIface::STATUS_CARD_CHANGED:
            case RealmcIface::STATUS_CARD_UNFORMATTED:
            case RealmcIface::STATUS_CARD_DAMAGED:
            case RealmcIface::STATUS_WRONG_DEVICE:
            case RealmcIface::STATUS_CARD_ERROR:
            case RealmcIface::STATUS_ENTRY_CORRUPTED:
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                break;

            case RealmcIface::STATUS_ENTRY_NOT_FOUND:
            case RealmcIface::STATUS_ENTRY_DELETED:
            case RealmcIface::STATUS_INSUFFICIENT_SPACE:
            case RealmcIface::STATUS_CANNOTMOUNT:
            case RealmcIface::STATUS_RETRY_BOOT_FLOW:
            case RealmcIface::STATUS_EXIT_TO_CARD_MANAGER:
            default:
                FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
                break;
        }
    }

    if (gMemcardSetup.mPreviousCommand == MCO_BootList) {
        msg = 0xA4BB7AE1;
    }

    if (GetMemcard()->IsAutoSaving()) {
        if (flag != RealmcIface::AUTOSAVE_ENABLE && FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            FEDatabase->GetGameplaySettings()->AutoSaveOn = false;
            GetMemcard()->m_bCardRemoved = true;
        }

        GetMemcard()->EndAutoSave();
    } else {
        cFEng::Get()->QueueGameMessage(msg, nullptr, 0xFF);
    }

    if (flag == RealmcIface::AUTOSAVE_ENABLE) {
        if (gMemcardSetup.GetCommand() == MCO_EnableAutoSave && FEDatabase->IsOptionsMode()) {
#ifndef EA_BUILD_A124
            FEDatabase->bAutoSaveOverwriteConfirmed = false;
#endif
        }

        FEDatabase->GetGameplaySettings()->AutoSaveOn = true;
        GetMemcard()->m_bCardRemoved = false;
    }
}

void MemcardCallbacks::SetMonitorDone(RealmcIface::CardStatus status, RealmcIface::MonitorState state) {
    unsigned int msg;

    JLog(MJ_SetMonitorDone);
    JLog(status);
    JLog(state);

    GetMemcard()->m_MemOp = 0;
    GetMemcard()->m_bMonitorOn = state == RealmcIface::MONITOR_ON || state == RealmcIface::MONITOR_ON_USER_CANCELLED;

    if (state == RealmcIface::MONITOR_ON) {
        if (status == RealmcIface::STATUS_OK) {
            msg = 0x54B3AC6C;
        } else {
            msg = 0x8867412D;
        }
    } else {
        if (cFEng::Get()->IsPackagePushed("MC_List.fng")) {
            msg = 0xEB29392A;
        } else if (MemoryCard::GetInstance()->IsMemcardScreenShowing()) {
            msg = 0x8867412D;
        }
    }

    cFEng::Get()->QueueGameMessage(msg, nullptr, 0xFF);
}

RealmcIface::TaskStatus MemcardCallbacks::LoadReady(const char *entryName, unsigned int headerSize, unsigned int bodySize, char *&headerData,
                                                    char *&bodyData) {
    JLog(MJ_LoadReady);
    JLog(entryName);
    JLog(headerSize);
    JLog(bodySize);

    RealmcIface::TaskStatus status = RealmcIface::TASK_CANCEL;

    if (headerSize == 8 && bodySize == GetMemcard()->GetSize()) {
        bodyData = GetMemcard()->GetData();
        headerData = GetMemcard()->GetHeader();
        status = RealmcIface::TASK_CONTINUE;
    }

    return status;
}

void IJoyHelper::EmulateMemoryCardLibrary(int aJoyOp) {
    const int iTempSize = 1024;
    char *pBuf = BNEW char[iTempSize];
    char *pBuf1 = pBuf + 1;
    wchar_t *pWStr;
    const wchar_t *pOptions[4] = {reinterpret_cast<const wchar_t *>(&pBuf[iTempSize - 200]), reinterpret_cast<const wchar_t *>(&pBuf[iTempSize - 150]),
                                  reinterpret_cast<const wchar_t *>(&pBuf[iTempSize - 100]), reinterpret_cast<const wchar_t *>(&pBuf[iTempSize - 50])};
    RealmcIface::CardInfo lCardInfo;
    RealmcIface::EntryInfo lEntryInfo;
    RealmcIface::BootupCheckResults lBootRes;

    lEntryInfo.mName = pBuf;

    switch (aJoyOp) {
        case MJ_None:
            break;

        case MJ_ShowMesssage:
            gMemcardCallbacks.ShowMessage(reinterpret_cast<const wchar_t *>(pBuf), 0, pOptions);
            break;

        case MJ_ClearMessage:
            gMemcardCallbacks.ClearMessage();
            break;

        case MJ_BootupCheckDone:
            lBootRes.Clear();
            gMemcardCallbacks.BootupCheckDone(RealmcIface::STATUS_OK, lBootRes);
            break;

        case MJ_SaveCheckDone:
            gMemcardCallbacks.SaveCheckDone(RealmcIface::RESULT_SUCCESS, RealmcIface::STATUS_OK);
            break;

        case MJ_SaveDone:
            gMemcardCallbacks.SaveDone(pBuf);
            break;

        case MJ_CheckLoadedData:
            gMemcardCallbacks.CheckLoadedData(pBuf);
            break;

        case MJ_LoadDone:
            gMemcardCallbacks.LoadDone(pBuf);
            break;

        case MJ_DeleteDone:
            gMemcardCallbacks.DeleteDone(pBuf);
            break;

        case MJ_ClearEntries:
            gMemcardCallbacks.ClearEntries();
            break;

        case MJ_FoundEntry:
            gMemcardCallbacks.FoundEntry(&lEntryInfo);
            break;

        case MJ_FindEntriesDone:
            gMemcardCallbacks.FindEntriesDone(RealmcIface::STATUS_OK);
            break;

        case MJ_Retry:
            gMemcardCallbacks.Retry(RealmcIface::STATUS_OK);
            break;

        case MJ_Failed:
            gMemcardCallbacks.Failed(RealmcIface::RESULT_SUCCESS, RealmcIface::STATUS_OK);
            break;

        case MJ_CardChecked:
            gMemcardCallbacks.CardChecked(&lCardInfo);
            break;

        case MJ_CardRemoved:
            gMemcardCallbacks.CardRemoved();
            break;

        case MJ_SetAutosaveDone:
            gMemcardCallbacks.SetAutosaveDone(RealmcIface::RESULT_SUCCESS, RealmcIface::STATUS_OK, RealmcIface::AUTOSAVE_DISABLE);
            break;

        case MJ_LoadReady:
            gMemcardCallbacks.LoadReady(pBuf, 0, 0, pBuf1, pBuf1);
            break;

        case MJ_SetMonitorDone:
            gMemcardCallbacks.SetMonitorDone(RealmcIface::STATUS_OK, RealmcIface::MONITOR_ON);
            break;
    }

    delete[] pBuf;
}

void DisplayUnicode(const wchar_t *str) {
    const short *pWChar = reinterpret_cast<const short *>(str);

    while (*pWChar != 0) {
        pWChar++;
    }
}

void DisplayMessage(const wchar_t *msg, unsigned int count, const wchar_t **str) {
    DisplayUnicode(msg);

    if (count == 0) {
        return;
    }

    for (unsigned int i = 0; i < count; i++) {
        DisplayUnicode(str[i]);
    }
}
