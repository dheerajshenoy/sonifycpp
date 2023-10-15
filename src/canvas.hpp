#ifndef __CANVAS_HPP
#define __CANVAS_HPP

#include "sonify.hpp"

class sonify;
typedef std::vector<cv::Mat> cvMatVect;

class Canvas : public QWidget
{
    public:
        Canvas(sonify*, QWidget *parent = nullptr);
        ~Canvas();

        void SetImage(QString);
        QSize ImageShape();
        void Traverse_LR_TB();
        void Traverse_TB_LR();
        void Img2Music();

    private:
        QLabel *imglabel = new QLabel();
        QVBoxLayout *layout = new QVBoxLayout();

        void UpdateLine();

        cvMatVect ImgHSVData();

        QImage _img, _imghsv;
        cv::Mat _mat_img_hsv, _mat_img;
        QSize _imgsize;
        uint _img_height, _img_width;
        uint _img_step;
        uchar *_img_data;
        boost::multi_array<int, 2> hues;
        sonify *_sonify;
};

#endif
