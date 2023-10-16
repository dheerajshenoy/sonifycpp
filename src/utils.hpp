#ifndef __UTILS_HPP
#define __UTILS_HPP

class Utils
{
    public:
        Utils();
        static QImage cvMatToQImage( const cv::Mat &inMat );
        static std::vector<double> linspace(double start, double end, int num, bool endpoint = true);
    private:
};

#endif
