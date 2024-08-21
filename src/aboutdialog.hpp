#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

class AboutDialog : public QDialog
{

public:
    AboutDialog(QWidget *parent = nullptr) noexcept;

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QLabel *m_name_label = new QLabel("SonifyCPP");
    QPushButton *m_done_btn = new QPushButton("Done");
    QLabel *m_info_label = new QLabel("Convert images to audio");
    QPushButton *m_git_btn = new QPushButton("Git");
};
