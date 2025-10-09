#ifndef FRONTEND_DATABASE_FEDATABASE_H
#define FRONTEND_DATABASE_FEDATABASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x20
struct GameplaySettings {
    bool AutoSaveOn;                    // offset 0x0, size 0x1
    bool RearviewOn;                    // offset 0x4, size 0x1
    bool Damage;                        // offset 0x8, size 0x1
    unsigned char SpeedoUnits;          // offset 0xC, size 0x1
    unsigned char RacingMiniMapMode;    // offset 0xD, size 0x1
    unsigned char ExploringMiniMapMode; // offset 0xE, size 0x1
    unsigned int MapItems;              // offset 0x10, size 0x4
    unsigned char LastMapZoom;          // offset 0x14, size 0x1
    unsigned char LastPursuitMapZoom;   // offset 0x15, size 0x1
    unsigned char LastMapView;          // offset 0x16, size 0x1
    bool JumpCam;                       // offset 0x18, size 0x1
    float HighlightCam;                 // offset 0x1C, size 0x4
};

// struct UserProfile {
//     // total size: 0x9CF4
//     char m_aProfileName[32];                    // offset 0x0, size 0x20
//     bool m_bNamed;                              // offset 0x20, size 0x1
//     struct OptionsSettings TheOptionsSettings;  // offset 0x24, size 0xC0
//     struct CareerSettings TheCareerSettings;    // offset 0xE4, size 0x27C
//     struct JukeboxEntry Playlist[30];           // offset 0x360, size 0xF0
//     struct FEPlayerCarDB PlayersCarStable;      // offset 0x450, size 0x8CC8
//     bool CareerModeHasBeenCompletedAtLeastOnce; // offset 0x9118, size 0x1
//     struct HighScoresDatabase HighScores;       // offset 0x911C, size 0xBD8
// };

// // total size: 0xA28
// struct cFrontendDatabase {
//     unsigned char iNumPlayers;                              // offset 0x0, size 0x1
//     bool bComingFromBoot;                                   // offset 0x4, size 0x1
//     bool bSavedProfileForMP;                                // offset 0x8, size 0x1
//     bool bProfileLoaded;                                    // offset 0xC, size 0x1
//     bool bIsOptionsDirty;                                   // offset 0x10, size 0x1
//     bool bAutoSaveOverwriteConfirmed;                       // offset 0x14, size 0x1
//     unsigned int iDefaultStableHash;                        // offset 0x18, size 0x4
//     signed char PlayerJoyports[2];                          // offset 0x1C, size 0x2
//     struct UserProfile *CurrentUserProfiles[2];             // offset 0x20, size 0x8
//     enum Type RaceMode;                                     // offset 0x28, size 0x4
//     struct RaceSettings TheQuickRaceSettings[11];           // offset 0x2C, size 0x18C
//     char *m_pCarStableBackup;                               // offset 0x1B8, size 0x4
//     char *m_pDBBackup;                                      // offset 0x1BC, size 0x4
//     unsigned int FEGameMode;                                // offset 0x1C0, size 0x4
//     enum eLoadSaveGame LoadSaveGame;                        // offset 0x1C4, size 0x4
//     struct FEKeyboardSettings mFEKeyboardSettings;          // offset 0x1C8, size 0x14C
//     int iCurPauseSubOptionType;                             // offset 0x314, size 0x4
//     int iCurPauseOptionType;                                // offset 0x318, size 0x4
//     struct FECustomizationRecord *SplitScreenCustomization; // offset 0x31C, size 0x4
//     char SplitScreenCarType[256];                           // offset 0x320, size 0x100
//     struct cFinishedRaceStats FinishedRaceStats;            // offset 0x420, size 0x604
//     enum ePostRaceOptions PostRaceOptionChosen;             // offset 0xA24, size 0x4
// };

// extern cFrontendDatabase *FEDatabase;

#endif
