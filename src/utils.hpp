#ifndef UTILS_HPP
#define UTILS_HPP

#include <qt6/QtCore/QVector>
#include <qt6/QtGui/QImage>
#include <qt6/QtCore/QtMath>

namespace Utils
{

template<typename T, typename... Args>
QVector<T> addVectors(const QVector<T>& first, const QVector<T>& second, const Args&... args);

QImage changeGamma(QImage &image, int _value, int h, int w);
QImage changeBrightness(QImage &image, int value, int h, int w);
QImage changeSaturation(QImage &image, int value, int h, int w);
QImage invertColor(QImage &img, int h, int w);
QImage convertToGrayscale(QImage &img, int h, int w);
QImage changeContrast(QImage &image, int _value, int h, int w);
}

#endif
