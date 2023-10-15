#ifndef __STATUSBAR_CPP
#define __STATUSBAR_CPP

#include "statusbar.hpp"

// Function to format file size into human-readable form
QString HumanReadableFileSize(qint64 size) {
    if (size < 1024) {
        return QString::number(size) + " bytes";
    } else if (size < 1024 * 1024) {
        return QString::number(size / 1024.0, 'f', 2) + " KB";
    } else if (size < 1024 * 1024 * 1024) {
        return QString::number(size / (1024.0 * 1024.0), 'f', 2) + " MB";
    } else {
        return QString::number(size / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
    }
}

StatusBar::StatusBar(sonify *s, QWidget *parent)
    : QWidget(parent)
{
    _sonify = s;
    layout->addWidget(msgLabel, 0, 0);
    layout->addWidget(progressbar, 0, 1);
    
    layout->setColumnStretch(5, 10);

    layout->addWidget(fileName, 0, 6);
    layout->addWidget(fileSize, 0, 7);
    layout->addWidget(fileDim, 0, 8);
    this->setLayout(layout);
    progressbar->setHidden(true);
    
    QFont fileNameFont;
    fileNameFont.setItalic(true);
    fileNameFont.setBold(true);
    fileName->setFont(fileNameFont);
}

void StatusBar::Msg(QString msg, uint32_t time_s)
{
    this->msgLabel->setText(msg);
    QTimer().singleShot(time_s * 1000, [=]() { this->msgLabel->clear(); });
}

void StatusBar::SetFile(QString file)
{
    _file = file;
    QFileInfo f(_file);
    fileName->setText(f.absoluteFilePath());
    fileSize->setText(HumanReadableFileSize(f.size()));
    QSize imgdim = _sonify->canvas->ImageShape();
    fileDim->setText(QString("(%1, %2)").arg(imgdim.height()).arg(imgdim.width()));
}

StatusBar::~StatusBar()
{}

#endif
