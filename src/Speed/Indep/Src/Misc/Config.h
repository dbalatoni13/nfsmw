#ifndef MISC_CONFIG_H
#define MISC_CONFIG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

extern int IsSoundEnabled;
extern bool IsAudioStreamingEnabled;
extern bool IsSpeechEnabled;
extern bool IsNISAudioEnabled;
extern bool ShutJosieUp;
extern bool IsMemcardEnabled;
extern bool IsAutoSaveEnabled;

void InitConfig();

#endif
