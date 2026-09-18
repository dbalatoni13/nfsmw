// ColourConversion.cpp - cross-platform colour format helpers.
//
// Xbox colours are packed ARGB (0xAARRGGBB) while the target consoles want
// the red and blue channels swapped, so the conversion is a straight channel
// swap that leaves alpha and green in place.

#include "Speed/Indep/Src/Render/Common/ColourConversion.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"

int ColourConvertXBoxToPS2(int colour) {
    return (colour & 0xFF00FF00) | ((colour >> 16) & 0x000000FF) | ((colour & 0x000000FF) << 16);
}
