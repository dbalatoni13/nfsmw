#ifndef FEPACKAGECHUNKS_H_
#define FEPACKAGECHUNKS_H_

#include "types.h"

static const u32 FENG_PACKAGEWRITER_VERSION = 0x020000; // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:15
static const u32 Chunk_Nested = 0x80000000;             // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:16

#define FE_ID(a, b, c, d) ((a) | (b << 8) | (c << 16) | (d << 24)) // :20

#define FE_TAG(a, b) ((a) | (b << 8)) // :28

static const u16 Tag_End = 0;                                                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:31
static const u32 Chunk_FEPackage = FE_ID('F', 'E', 'n', 'g') | Chunk_Nested;    // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:34
static const u32 Chunk_FEPackageHeader = FE_ID('P', 'k', 'H', 'd');             // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:35
static const u32 Chunk_FETypeList = FE_ID('T', 'L', 's', 't') | Chunk_Nested;   // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:36
static const u32 Chunk_FETypeSizes = FE_ID('T', 'y', 'p', 'S');                 // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:37
static const u32 Chunk_FETypeNode = FE_ID('T', 'y', 'n', 'o');                  // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:38
static const u16 Tag_FETypeName = FE_TAG('T', 'n');                             // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:39
static const u16 Tag_FETypeID = FE_TAG('T', 'i');                               // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:40
static const u16 Tag_FETypeField = FE_TAG('T', 'F');                            // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:41
static const u16 Tag_FEFieldName = FE_TAG('F', 'n');                            // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:42
static const u16 Tag_FEFieldSize = FE_TAG('F', 's');                            // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:43
static const u16 Tag_FEFieldDefault = FE_TAG('F', 'd');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:44
static const u32 Chunk_FELibraryList = FE_ID('L', 'i', 'b', 'L');               // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:46
static const u32 Chunk_FELibRefList = FE_ID('L', 'i', 'b', 'R');                // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:47
static const u32 Chunk_FEResList = FE_ID('R', 'e', 's', 'L') | Chunk_Nested;    // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:49
static const u32 Chunk_FEResNames = FE_ID('R', 's', 'N', 'm');                  // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:50
static const u32 Chunk_FEResRequests = FE_ID('R', 's', 'R', 'q');               // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:51
static const u32 Chunk_FEObjectList = FE_ID('O', 'b', 'j', 'L') | Chunk_Nested; // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:53
static const u32 Chunk_FEButtonCount = FE_ID('B', 'u', 't', 'n');               // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:54
static const u32 Chunk_FEObject = FE_ID('F', 'O', 'b', 'j') | Chunk_Nested;     // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:55
static const u32 Chunk_FEObjectData = FE_ID('O', 'b', 'j', 'D');                // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:56
static const u16 Tag_FEObjType = FE_TAG('O', 't');                              // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:58
static const u16 Tag_FEObjName = FE_TAG('O', 'n');                              // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:59
static const u16 Tag_FEObjNameHash = FE_TAG('O', 'h');                          // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:60
static const u16 Tag_FEObjProperties = FE_TAG('O', 'P');                        // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:61
static const u16 Tag_FERefSubFlags = FE_TAG('R', 'f');                          // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:62
static const u16 Tag_FEObjParentGUID = FE_TAG('P', 'A');                        // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:64
static const u16 Tag_FEObjStaticAnimData = FE_TAG('S', 'A');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:66
static const u16 Tag_FEObjImageFlags = FE_TAG('I', 'f');                        // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:68
static const u16 Tag_FEObjMultiImageTex1 = FE_TAG('M', '1');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:70
static const u16 Tag_FEObjMultiImageTex2 = FE_TAG('M', '2');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:71
static const u16 Tag_FEObjMultiImageTex3 = FE_TAG('M', '3');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:72
static const u16 Tag_FEObjMultiImageFlags1 = FE_TAG('M', 'a');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:73
static const u16 Tag_FEObjMultiImageFlags2 = FE_TAG('M', 'b');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:74
static const u16 Tag_FEObjMultiImageFlags3 = FE_TAG('M', 'c');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:75
static const u16 Tag_FEObjStringBufLength = FE_TAG('S', 'b');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:77
static const u16 Tag_FEObjStringText = FE_TAG('S', 't');                        // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:78
static const u16 Tag_FEObjStringJustification = FE_TAG('S', 'j');               // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:79
static const u16 Tag_FEObjStringLeading = FE_TAG('S', 'l');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:80
static const u16 Tag_FEObjStringMaxWidth = FE_TAG('S', 'w');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:81
static const u16 Tag_FEObjStringLabelName = FE_TAG('S', 'L');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:82
static const u16 Tag_FEObjStringLabelHash = FE_TAG('S', 'H');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:83
static const u16 Tag_FEObjListDimesions = FE_TAG('L', 'd');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:85
static const u16 Tag_FEObjListAutoWrap = FE_TAG('L', 'w');                      // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:86
static const u16 Tag_FEObjListViewDimensions = FE_TAG('L', 'v');                // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:87
static const u16 Tag_FEObjListSelectSpeed = FE_TAG('L', 's');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:88
static const u16 Tag_FEObjListColumnData = FE_TAG('L', 'c');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:89
static const u16 Tag_FEObjListRowData = FE_TAG('L', 'r');                       // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:90
static const u16 Tag_FEObjListCellColor = FE_TAG('C', 'c');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:92
static const u16 Tag_FEObjListCellScale = FE_TAG('C', 's');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:93
static const u16 Tag_FEObjListCellResource = FE_TAG('C', 'r');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:94
static const u16 Tag_FEObjListCellType = FE_TAG('C', 'T');                      // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:95
static const u16 Tag_FEObjListCellStringText = FE_TAG('C', 't');                // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:96
static const u16 Tag_FEObjListCellImageRect = FE_TAG('C', 'i');                 // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:97
static const u16 Tag_FEObjCodeListDimensions = FE_TAG('L', 'D');                // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:99
static const u16 Tag_FEObjCodeListViewDimensions = FE_TAG('L', 'v');            // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:100
static const u16 Tag_FEObjCodeListStringInfo = FE_TAG('S', 'I');                // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:101
static const u16 Tag_FEObjCodeListAutoWrap = FE_TAG('L', 'w');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:102
static const u16 Tag_FEObjCodeListScrollCenter = FE_TAG('S', 'r');              // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:103
static const u16 Tag_FEObjCodeListSelectSpeed = FE_TAG('L', 's');               // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:104
static const u16 Tag_FEObjCodeListSelectionless = FE_TAG('L', 'l');             // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:105
static const u16 Tag_FEObjCodeListFlags = FE_TAG('L', 't');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:106
static const u16 Tag_FEObjCodeListJustifyFlags = FE_TAG('L', 'j');              // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:107
static const u32 Chunk_FEScript = FE_ID('S', 'c', 'r', 'p');                    // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:109
static const u16 Tag_FEScriptName = FE_TAG('S', 'n');                           // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:110
static const u16 Tag_FEScriptHeader = FE_TAG('S', 'h');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:111
static const u16 Tag_FEScriptID = FE_TAG('S', 'I');                             // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:113
static const u16 Tag_FEScriptLength = FE_TAG('S', 'l');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:114
static const u16 Tag_FEScriptFlags = FE_TAG('S', 'f');                          // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:115
static const u16 Tag_FEScriptTrackIndex = FE_TAG('T', 'i');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:116
static const u16 Tag_FEScriptTrackInterpType = FE_TAG('T', 't');                // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:117
static const u16 Tag_FEScriptTrackInterpAction = FE_TAG('T', 'a');              // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:118
static const u16 Tag_FEScriptTrackBaseKey = FE_TAG('T', 'b');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:119
static const u16 Tag_FEScriptTrackOffset = FE_TAG('T', 'o');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:120
static const u16 Tag_FEScriptChainTo = FE_TAG('S', 'c');                        // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:121
static const u16 Tag_FEScriptFieldInfo = FE_TAG('F', 'I');                      // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:123
static const u16 Tag_FEScriptFieldRefInfo = FE_TAG('F', 'R');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:124
static const u16 Tag_FEScriptFieldKeyData = FE_TAG('K', 'd');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:125
static const u16 Tag_FEScriptEvents = FE_TAG('E', 'V');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:127
static const u32 Chunk_FEMessageResponseList = FE_ID('M', 's', 'g', 'R');       // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:129
static const u32 Chunk_FEPackageResponseList = FE_ID('P', 'k', 'g', 'R');       // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:130
static const u16 Tag_FEMsgRespMsgID = FE_TAG('M', 'i');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:131
static const u16 Tag_FEMsgRespCount = FE_TAG('M', 'C');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:132
static const u16 Tag_FEResponseID = FE_TAG('R', 'i');                           // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:133
static const u16 Tag_FEResponseUINT = FE_TAG('R', 'u');                         // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:134
static const u16 Tag_FEResponseString = FE_TAG('R', 's');                       // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:135
static const u16 Tag_FEResponseTarget = FE_TAG('R', 't');                       // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:136
static const u32 Chunk_FEMessageTargetList = FE_ID('T', 'a', 'r', 'g');         // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:138
static const u16 Tag_FEMsgTargetCount = FE_TAG('T', 'c');                       // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:139
static const u16 Tag_FEMsgTarget = FE_TAG('M', 't');                            // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:140
static const u32 Chunk_FEMessageNameList = FE_ID('M', 's', 'g', 'N');           // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:142
static const u16 Tag_FEMsgName = FE_TAG('M', 'N');                              // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:143
static const u16 Tag_FEMsgClass = FE_TAG('M', 'C');                             // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:144
static const u32 Chunk_FEEditorData = FE_ID('E', 'D', 'a', 't');                // size: 0x4, Decl: speed/indep/src/feng/FEPackageChunks.h:146
static const u16 Tag_FEEditorGUID = FE_TAG('E', 'G');                           // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:147
static const u16 Tag_FEEditorFileCounter = FE_TAG('E', 'F');                    // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:148
static const u16 Tag_FEEditorObjectCounter = FE_TAG('E', 'O');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:149
static const u16 Tag_FEEditorObjectComment = FE_TAG('E', 'K');                  // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:150
static const u16 Tag_FEEditorObjectGUID = FE_TAG('E', 'P');                     // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:151
static const u16 Tag_FEEditorObjectDefine = FE_TAG('E', 'e');                   // size: 0x2, Decl: speed/indep/src/feng/FEPackageChunks.h:152

#endif
