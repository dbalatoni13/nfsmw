#ifndef ECSTASY_TEXTURE_TYPES_H
#define ECSTASY_TEXTURE_TYPES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

enum TextureLockType {
    TEXLOCK_READWRITE = 2,
    TEXLOCK_WRITE = 1,
    TEXLOCK_READ = 0,
};

#endif
