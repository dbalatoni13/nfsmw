#ifndef FRONTEND_LOCALIZATION_LOCALIZE_H
#define FRONTEND_LOCALIZATION_LOCALIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

const char *GetLocalizedString(unsigned int string_label);
char *GetTranslatedString(int label_hash);

#endif
