#ifndef __STATUSBAR_HPP
#define __STATUSBAR_HPP


#include "sonify.hpp"

class StatusBar : public QWidget
{
    public:
        StatusBar(sonify *s, QWidget *parent = nullptr);
        ~StatusBar();
        QProgressBar ProgressBar();
        void Msg(QString msg, uint32_t time_s = 1);
        void SetFile(QString);

    private:

        QGridLayout *layout = new QGridLayout();
        QLabel *msgLabel = new QLabel("A");
        QLabel *fileName = new QLabel("B");
        QLabel *fileSize = new QLabel("C");
        QLabel *fileDim = new QLabel("D");
        QProgressBar *progressbar = new QProgressBar();

        sonify *_sonify;
        QString _file;
};

#endif
