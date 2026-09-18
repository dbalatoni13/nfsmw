//
//
//
#ifndef OLDSOUNDTEMPLATES_HPP
#define OLDSOUNDTEMPLATES_HPP

template <typename T> T smooth(const T curr, const T target, const T deltaUp, const T deltaDown) {
    if (target > curr + deltaUp) {
        return curr + deltaUp;
    }
    if (target < curr - deltaDown) {
        return curr - deltaDown;
    }
    return target;
}

template <typename T> T smooth(const T curr, const T target, const T delta) {
    if (target > curr + delta) {
        return curr + delta;
    }
    if (target < curr - delta) {
        return curr - delta;
    }
    return target;
}

#endif
