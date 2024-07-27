#ifndef UTILS_HPP
#define UTILS_HPP

#include <qt6/QtCore/QVector>
#include <sndfile.h>

namespace utils
{

template<typename T, typename... Args>
QVector<T> addVectors(const QVector<T>& first, const QVector<T>& second, const Args&... args) {
    size_t size = first.size();
    QVector<T> result(size, T{});

    for (size_t i = 0; i < size; ++i) {
        result[i] = first[i] + second[i];
    }

    if constexpr (sizeof...(args) > 0) {
        result = addVectors(result, args...);
    }

    return result;
}
}


#endif
