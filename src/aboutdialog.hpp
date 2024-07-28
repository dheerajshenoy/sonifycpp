#ifndef ABOUTDIALOG_HPP
#define ABOUTDIALOG_HPP

#include <qt6/QtWidgets/QDialog>
#include <qt6/QtWidgets/QVBoxLayout>
#include <qt6/QtWidgets/QLabel>
#include <qt6/QtWidgets/QPushButton>
#include <qt6/QtGui/QFont>

class AboutDialog : public QDialog
{

public:
    AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

private:
    QVBoxLayout *m_layout = new QVBoxLayout();
    QLabel *m_name_label = new QLabel("SonifyCPP");
    QPushButton *m_done_btn = new QPushButton("Done");
    QLabel *m_info_label = new QLabel("Convert images to audio");
    QPushButton *m_git_btn = new QPushButton("Git");
};

#endif
