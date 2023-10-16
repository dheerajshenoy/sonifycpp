#ifndef __CANVAS_CPP
#define __CANVAS_CPP

#include "canvas.hpp"

auto PI = boost::math::constants::pi<double>();

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
    _hues.resize(boost::extents[_img_height][_img_width]);
    for(uint i = 0; i < _img_height; i++)
    {
        for(uint j = 0; j < _img_width; j++)
        {
            // Get only the hue values
            _hues[i][j] = _mat_img_hsv.at<cv::Vec3b>(i, j)[0];
        }
    }
}

float Canvas::Hue2Freq(float h, float *scale_freqs)
{
    int thresholds[] = { 26 , 52 , 78 , 104,  128 , 154 , 180 };
    float note = scale_freqs[0];
    if (h <= thresholds[0])
         note = scale_freqs[0];
    else if ((h > thresholds[0]) & (h <= thresholds[1]))
        note = scale_freqs[1];
    else if ((h > thresholds[1]) & (h <= thresholds[2]))
        note = scale_freqs[2];
    else if ((h > thresholds[2]) & (h <= thresholds[3]))
        note = scale_freqs[3];
    else if ((h > thresholds[3]) & (h <= thresholds[4]))
        note = scale_freqs[4];
    else if ((h > thresholds[4]) & (h <= thresholds[5]))
        note = scale_freqs[5];
    else if ((h > thresholds[5]) & (h <= thresholds[6]))
        note = scale_freqs[6];
    else
        note = scale_freqs[0];
    return note;
}

void Canvas::Img2Music()
{
    QString traverseMethod = _sonify->drawer->TraverseMethod();

    if(traverseMethod == "Left to Right")
        Traverse_TB_LR();
    // Handle other traversal method
        
    float scale_freqs[] = { 220.00, 246.94 ,261.63, 293.66, 329.63, 349.23, 415.30 };
    
    _notes.resize(boost::extents[_img_height][_img_width]);

    for(size_t i=0; i < _img_height; i++)
        for(size_t j=0; j < _img_width; j++)
        {
            _notes[i][j] = Hue2Freq(_hues[i][j], scale_freqs);
            // _notes.push_back(Hue2Freq(_hues[i][j], scale_freqs));
        }

    uint SR = _sonify->drawer->SampleRate().split("Hz")[0].toInt();
    QString d = _sonify->drawer->NoteDuration();
    if (d.isEmpty())
        _duration = 0.1;
    else
        _duration = d.toFloat();

    auto samples = int(_duration * SR);
    auto _t = Utils::linspace(0, _duration, samples, false);

    int amp = 100;
    float val, n;
    std::vector<float> note;
    note.resize(_t.size());
    // for(size_t i = 0; i < _img_height; i++)
    //     for(size_t j = 0; j < _img_width; j++)
    for(size_t i = 0; i < 10; i++)
        for(size_t j = 0; j < 10; j++)
        {
            val = _notes[i][j];
            for(size_t x = 0; x < _t.size(); x++)
            {
                n = amp * sin(2 * PI * val * _t.at(x));
                note[x] = n;
            }
            _song.insert(_song.end(), note.begin(), note.end());
        }

    std::vector<sf::Int16> Song;
    for (double sample : _song) {
        // Ensure the value is within the valid range for sf::Int16
        if (sample > 1.0) sample = 1.0;
        if (sample < -1.0) sample = -1.0;
        Song.push_back(static_cast<sf::Int16>(sample * 32767)); // Scale to the full range
    }

    sf::SoundBuffer soundBuffer;
    soundBuffer.loadFromSamples(&Song[0], Song.size(), 1, 44100);
    sf::Sound sound;
    sound.setBuffer(soundBuffer);

    // Play the sound
    sound.play();
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
