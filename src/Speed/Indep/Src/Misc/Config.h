#ifndef MISC_CONFIG_H
#define MISC_CONFIG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

extern int IsSoundEnabled;
extern int IsAudioStreamingEnabled;
extern int IsSpeechEnabled;
extern int IsNISAudioEnabled;
extern bool ShutJosieUp;
extern int IsMemcardEnabled;
extern int IsAutoSaveEnabled;

void InitConfig();

#endif
