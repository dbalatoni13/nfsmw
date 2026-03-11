#ifndef EAXSOUND_OLDSOUNDTEMPLATES_H
#define EAXSOUND_OLDSOUNDTEMPLATES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// lines 1-13: header guard/includes etc

// lines 14-18: smooth with separate deltaUp/deltaDown
template <class T>
T smooth(const T curr, const T target, const T deltaUp, const T deltaDown) {
    if (target > curr + deltaUp)
        return curr + deltaUp;
    if (target < curr - deltaDown)
        return curr - deltaDown;
    return target;
}

// lines 22-26: smooth with single delta
template <class T>
T smooth(const T curr, const T target, const T delta) {
    if (target > curr + delta)
        return curr + delta;
    if (target < curr - delta)
        return curr - delta;
    return target;
}

#endif
