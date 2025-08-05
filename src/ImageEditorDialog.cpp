#include "ImageEditorDialog.hpp"

ImageEditorDialog::ImageEditorDialog(QWidget *parent) noexcept : QDialog(parent)
{
    this->setLayout(m_layout);

    this->setWindowModality(Qt::WindowModality::ApplicationModal);
    m_layout->addWidget(m_splitter);

    m_splitter->addWidget(m_side_panel);
    m_splitter->addWidget(m_img_label);
    m_brightness_slider->setOrientation(Qt::Orientation::Horizontal);
    m_saturation_slider->setOrientation(Qt::Orientation::Horizontal);
    m_contrast_slider->setOrientation(Qt::Orientation::Horizontal);
    m_gamma_slider->setOrientation(Qt::Orientation::Horizontal);
    m_contrast_slider->setRange(-100, 100); // 0.1 to 3.0
    m_gamma_slider->setRange(10, 300);      // 0.1 to 3.0
    m_saturation_slider->setRange(-100, 100);
    m_brightness_slider->setRange(-100, 100);
    m_contrast_slider->setValue(0);
    m_gamma_slider->setValue(100);
    m_brightness_slider->setValue(0);
    m_saturation_slider->setValue(0);
    m_side_panel_layout->addWidget(m_brightness_label, 1, 0);
    m_side_panel_layout->addWidget(m_brightness_slider, 1, 1);
    m_side_panel_layout->addWidget(m_contrast_label, 2, 0);
    m_side_panel_layout->addWidget(m_contrast_slider, 2, 1);
    m_side_panel_layout->addWidget(m_saturation_label, 3, 0);
    m_side_panel_layout->addWidget(m_saturation_slider, 3, 1);
    m_side_panel_layout->addWidget(m_gamma_label, 4, 0);
    m_side_panel_layout->addWidget(m_gamma_slider, 4, 1);
    m_side_panel_layout->addWidget(m_gray_label, 5, 0);
    m_side_panel_layout->addWidget(m_gray_cb, 5, 1);
    m_side_panel_layout->addWidget(m_invert_label, 6, 0);
    m_side_panel_layout->addWidget(m_invert_cb, 6, 1);
    m_side_panel_layout->addWidget(m_cancel_btn, 7, 0, 1, 2);
    m_side_panel_layout->addWidget(m_apply_btn, 8, 0, 1, 2);
    m_side_panel_layout->addWidget(m_reset_btn, 9, 0, 1, 2);
    m_side_panel_layout->setRowStretch(10, 1);
    m_side_panel->setLayout(m_side_panel_layout);
    m_splitter->setStretchFactor(0, 1);
    m_img_label->setAlignment(Qt::AlignmentFlag::AlignCenter);

    connect(m_gray_cb, &QCheckBox::checkStateChanged, this,
            &ImageEditorDialog::applyImageOptions);
    connect(m_invert_cb, &QCheckBox::checkStateChanged, this,
            &ImageEditorDialog::applyImageOptions);
    connect(m_brightness_slider, &QSlider::valueChanged, this,
            &ImageEditorDialog::applyImageOptions);
    connect(m_contrast_slider, &QSlider::valueChanged, this,
            &ImageEditorDialog::applyImageOptions);
    connect(m_gamma_slider, &QSlider::valueChanged, this,
            &ImageEditorDialog::applyImageOptions);
    connect(m_saturation_slider, &QSlider::valueChanged, this,
            &ImageEditorDialog::applyImageOptions);
    connect(m_reset_btn, &QPushButton::clicked, this, [this]()
    {
        emit resetImage();
        this->close();
    });

    connect(m_apply_btn, &QPushButton::clicked, this, [this]()
    {
        int brightness = m_brightness_slider->value();
        int contrast   = m_contrast_slider->value();
        int saturation = m_saturation_slider->value();
        int gamma      = m_gamma_slider->value();
        bool grayscale = m_gray_cb->isChecked();
        bool invert    = m_invert_cb->isChecked();

        emit optionsApplied(ImageOptions{brightness, saturation, contrast,
                                         gamma, grayscale, invert});
        this->close();
    });

    connect(m_cancel_btn, &QPushButton::clicked, this,
            [this]() { this->close(); });

    this->show();
}

void
ImageEditorDialog::applyImageOptions() noexcept
{
    double brightness = m_brightness_slider->value();
    double contrast   = m_contrast_slider->value();
    double saturation = m_saturation_slider->value();
    double gamma      = m_gamma_slider->value();
    bool grayscale    = m_gray_cb->isChecked();
    bool invert       = m_invert_cb->isChecked();

    m_edited_img = m_img;

    if (brightness != 0)
        m_edited_img = changeBrightness(m_edited_img, brightness);
    if (contrast != 0)
        m_edited_img = changeContrast(m_edited_img, contrast);
    if (saturation != 0)
        m_edited_img = changeSaturation(m_edited_img, saturation);
    if (gamma != 100)
        m_edited_img = changeGamma(m_edited_img, gamma);
    if (invert)
        m_edited_img = invertColor(m_edited_img);
    if (grayscale)
        m_edited_img = convertToGrayscale(m_edited_img);

    m_img_label->setPixmap(QPixmap::fromImage(m_edited_img));
}

void
ImageEditorDialog::closeEvent(QCloseEvent *) noexcept
{
    emit closed();
}

// gamma 0.1 to 300
QImage
ImageEditorDialog::changeGamma(QImage &image, const int &_value) noexcept
{
    double value = _value / 100.0;
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            QColor color = image.pixelColor(x, y);

            int red = qBound(
                0, static_cast<int>(qPow(color.red() / 255.0, value) * 255),
                255);
            int green = qBound(
                0, static_cast<int>(qPow(color.green() / 255.0, value) * 255),
                255);
            int blue = qBound(
                0, static_cast<int>(qPow(color.blue() / 255.0, value) * 255),
                255);

            image.setPixelColor(x, y, QColor(red, green, blue));
        }
    }
    return image;
}

QImage
ImageEditorDialog::changeBrightness(QImage &image, const int &value) noexcept
{
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            QColor color = image.pixelColor(x, y);
            int red      = color.red() + value;
            int green    = color.green() + value;
            int blue     = color.blue() + value;

            // Clamp values to the 0-255 range
            red   = qBound(0, red, 255);
            green = qBound(0, green, 255);
            blue  = qBound(0, blue, 255);

            image.setPixelColor(x, y, QColor(red, green, blue));
        }
    }
    return image;
}

QImage
ImageEditorDialog::changeSaturation(QImage &image, const int &value) noexcept
{
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            QColor color = image.pixelColor(x, y);
            int h, s, v;
            color.getHsv(&h, &s, &v);

            s = qBound(0, s + value,
                       255); // Adjust saturation and clamp to [0, 255]

            color.setHsv(h, s, v);
            image.setPixelColor(x, y, color);
        }
    }
    return image;
}

QImage
ImageEditorDialog::invertColor(QImage &img) noexcept
{
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
        {
            QColor color = img.pixelColor(i, j);
            img.setPixelColor(i, j,
                              QColor(255 - color.red(), 255 - color.green(),
                                     255 - color.blue()));
        }

    return img;
}

QImage
ImageEditorDialog::convertToGrayscale(QImage &img) noexcept
{
    for (int i = 0; i < m_width; i++)
        for (int j = 0; j < m_height; j++)
        {
            QColor color = img.pixelColor(i, j);
            int gray     = qGray(color.rgb());
            img.setPixelColor(i, j, QColor(gray, gray, gray));
        }
    return img;
}

void
ImageEditorDialog::keyPressEvent(QKeyEvent *e) noexcept
{
    if (e->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(e);
    else
    {
    }
}

void
ImageEditorDialog::setPixmap(const QPixmap &pix) noexcept
{
    m_img = pix.toImage();
    if (m_img.height() > 200 && m_img.width() > 200)
        m_img = m_img.scaled(200, 200, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
    m_height = m_img.height();
    m_width  = m_img.width();
    m_img_label->setPixmap(QPixmap::fromImage(m_img));
}

// contrast factor 0.0 to 2.0
QImage
ImageEditorDialog::changeContrast(QImage &image, const int &_value) noexcept
{
    double value = (259.0 * (_value + 255.0)) / (255.0 * (259.0 - _value));
    for (int y = 0; y < m_height; ++y)
    {
        for (int x = 0; x < m_width; ++x)
        {
            QColor color = image.pixelColor(x, y);

            int red = qBound(
                0, static_cast<int>((value * (color.red() - 128)) + 128), 255);
            int green = qBound(
                0, static_cast<int>((value * (color.green() - 128)) + 128),
                255);
            int blue = qBound(
                0, static_cast<int>((value * (color.blue() - 128)) + 128), 255);

            image.setPixelColor(x, y, QColor(red, green, blue));
        }
    }
    return image;
}
