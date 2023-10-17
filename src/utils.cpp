#ifndef __UTILS_CPP
#define __UTILS_CPP

#include "utils.hpp"

QImage Utils::cvMatToQImage( const cv::Mat &inMat )
{
    switch ( inMat.type() )
    {
        // 8-bit, 4 channel
        case CV_8UC4:
            {
                QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_ARGB32 );

                return image;
            }

            // 8-bit, 3 channel
        case CV_8UC3:
            {
                QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_RGB888 );

                return image.rgbSwapped();
            }

            // 8-bit, 1 channel
        case CV_8UC1:
            {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
                QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_Grayscale8 );
#else
                static QVector<QRgb>  sColorTable;

                // only create our color table the first time
                if ( sColorTable.isEmpty() )
                {
                    sColorTable.resize( 256 );

                    for ( int i = 0; i < 256; ++i )
                    {
                        sColorTable[i] = qRgb( i, i, i );
                    }
                }

                QImage image( inMat.data,
                        inMat.cols, inMat.rows,
                        static_cast<int>(inMat.step),
                        QImage::Format_Indexed8 );

                image.setColorTable( sColorTable );
#endif

                return image;
            }

        default:
            qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
            break;
    }

    return QImage();
}

std::vector<double> Utils::linspace(double start, double end, int num, bool endpoint)
{
    std::vector<double> result;
    if (num < 2) {
        if (num == 1) {
            result.push_back(start);
            if (endpoint) {
                result.push_back(end);
            }
        }
        return result;
    }
    double step = (end - start) / static_cast<double>(num - (endpoint ? 1 : 0));
    for (int i = 0; i < num; ++i) {
        result.push_back(start + step * static_cast<double>(i));
    }
    return result;

}

template <typename T>
T Utils::randchoice(std::vector<T> arr)
{
    // Create a random number generator engine
    std::random_device rd;
    std::mt19937 gen(rd());

    // Create a uniform distribution for floating-point choices
    std::uniform_real_distribution<T> dist(0.0f, 1.0f);

    // Generate a random value within [0.0, 1.0) and scale it to the range of choices
    float randomValue = dist(gen);
    int randomIndex = static_cast<T>(randomValue * arr.size());
    return arr[randomIndex];
}

#endif
