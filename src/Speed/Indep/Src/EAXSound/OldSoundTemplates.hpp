#ifndef EAXSOUND_OLDSOUNDTEMPLATES_H
#define EAXSOUND_OLDSOUNDTEMPLATES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

template <typename T> T smooth(const float curr /* f0 */, const T target /* f2 */, const T deltaUp /* f3 */, const T deltaDown /* f4 */) {}

template <typename T> T smooth(const T curr /* f0 */, const T target /* f2 */, const T delta /* f3 */) {}

#endif
