//
//
//
#ifndef OLDSOUNDTEMPLATES_HPP
#define OLDSOUNDTEMPLATES_HPP

template <typename T> T smooth(const T curr, const T target, const T deltaUp, const T deltaDown) {
    if (target > curr) {
        const T value = curr + deltaUp;
        return value < target ? value : target;
    }
    const T value = curr - deltaDown;
    return value > target ? value : target;
}

template <typename T> T smooth(const T curr, const T target, const T delta) {
    return smooth(curr, target, delta, delta);
}

#endif
