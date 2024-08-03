#ifndef IMAGE_EDITOR_DIALOG_HPP
#define IMAGE_EDITOR_DIALOG_HPP

#include <qt6/QtGui/QImage>
#include <qt6/QtGui/QPixmap>
#include <qt6/QtGui/QTransform>
#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QSlider>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtWidgets/QSplitter>
#include <qt6/QtWidgets/QGridLayout>
#include <qt6/QtWidgets/QSpinBox>
#include <qt6/QtWidgets/QCheckBox>
#include <qt6/QtGui/QKeyEvent>
#include "gv.hpp"

struct ImageOptions
{
    int Brightness;
    int Saturation;
    int Contrast;
    int Gamma;
    bool Grayscale;
    bool Invert;
};

class ImageEditorDialog : public QDialog
{
    Q_OBJECT
public:
    ImageEditorDialog(QWidget *parent = nullptr);
    ~ImageEditorDialog();
    void setPixmap(QPixmap &pix);

signals:
    void closed();
    void optionsApplied(ImageOptions);
    void resetImage();

protected:
    void keyPressEvent(QKeyEvent *e) override;
    void closeEvent(QCloseEvent *e) override;

private:

    QVBoxLayout *m_layout = new QVBoxLayout();
    QGridLayout *m_side_panel_layout = new QGridLayout();
    QSplitter *m_splitter = new QSplitter();
    QWidget *m_side_panel = new QWidget();
    QLabel *m_gamma_label = new QLabel("Gamma");
    QLabel *m_contrast_label = new QLabel("Contrast");
    QLabel *m_brightness_label = new QLabel("Brightness");
    QLabel *m_saturation_label = new QLabel("Saturation");
    QLabel *m_invert_label = new QLabel("Invert colors");
    QLabel *m_gray_label = new QLabel("Grayscale");
    QLabel *m_img_label = new QLabel();

    QCheckBox *m_gray_cb = new QCheckBox();
    QCheckBox *m_invert_cb = new QCheckBox();
    QSlider *m_gamma_slider = new QSlider();
    QSlider *m_contrast_slider = new QSlider();
    QSlider *m_saturation_slider = new QSlider();
    QSlider *m_brightness_slider = new QSlider();

    QPushButton *m_cancel_btn = new QPushButton("Cancel");
    QPushButton *m_apply_btn = new QPushButton("Apply");
    QPushButton *m_reset_btn = new QPushButton("Reset");
    QImage m_img, m_edited_img;
    int m_width, m_height;

    QImage convertToGrayscale(QImage &img);
    QImage changeSaturation(QImage &img, int value);
    QImage changeBrightness(QImage &img, int value);
    QImage changeContrast(QImage &img, int value);
    QImage changeGamma(QImage &img, int value);
    QImage invertColor(QImage &img);

    void applyImageOptions();
};


#endif
