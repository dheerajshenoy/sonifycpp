#ifndef __CANVAS_CPP
#define __CANVAS_CPP


#include "canvas.hpp"
#include "utils.hpp"

Canvas::Canvas(sonify *s, QWidget *parent)
    : QWidget(parent)
{
    _sonify = s;
    layout->setAlignment(Qt::AlignmentFlag::AlignCenter);
    layout->addWidget(imglabel);
    this->setLayout(layout);
}

void Canvas::SetImage(QString path)
{
    using namespace cv;
    _mat_img = imread(path.toStdString());
    cvtColor(_mat_img, _mat_img_hsv, cv::COLOR_BGR2HSV);

    _img = QImage(Utils::cvMatToQImage(_mat_img));
    _imghsv = QImage(Utils::cvMatToQImage(_mat_img_hsv));

    // imglabel->setPixmap(QPixmap::fromImage(_img));
    imglabel->setPixmap(QPixmap::fromImage(_imghsv));
    
    _img_width = _mat_img_hsv.cols;
    _img_height = _mat_img_hsv.rows;
    _img_step = _mat_img_hsv.step;
    _img_data = _mat_img_hsv.data;

}

// Function for splitting the channels of HSV to H, S and V
cvMatVect Canvas::ImgHSVData()
{
    cvMatVect channels;
    cv::split(_mat_img_hsv, channels);
    return channels;
}

// Traverse Top to Bottom ; Left to Right
void Canvas::Traverse_TB_LR()
{
    // hues.empty();
    hues.resize(boost::extents[_img_height][_img_width]);
    for(uint i = 0; i < _img_height; i++)
    {
        for(uint j = 0; j < _img_width; j++)
        {
            // Get only the hue values
            hues[i][j] = _mat_img_hsv.at<cv::Vec3b>(i, j)[0];
        }
    }
}

void Canvas::Img2Music()
{
    QString traverseMethod = _sonify->drawer->TraverseMethod();

    if(traverseMethod == "Left to Right")
        Traverse_TB_LR();
        

        // Handle other traversal method
}

QSize Canvas::ImageShape()
{
    return QSize(_img_width, _img_height);
}

void Canvas::UpdateLine()
{
}

Canvas::~Canvas() {}

#endif
