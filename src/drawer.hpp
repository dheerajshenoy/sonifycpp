#ifndef __DRAWER_HPP
#define __DRAWER_HPP

class Drawer : public QWidget
{
    public:
        Drawer(sonify *s, QWidget *parent = nullptr);
        ~Drawer();
        void StartSonify();
        void StopSonify();

        QString TraverseMethod();
        QString SkipW();
        QString SkipH();
        QString Mapping();
        QString Scale();
        QString Key();
        QString Octave();
        QString SampleRate();
        QString NoteDuration();
        QString Harmony();


    private:
        QGridLayout *drawerLayout = new QGridLayout();
        QComboBox *combo_traverse = new QComboBox();
        QComboBox *combo_mapping = new QComboBox();
        QLineEdit *box_skiph = new QLineEdit();
        QLineEdit *box_skipw = new QLineEdit();
        QComboBox *combo_sample_rate = new QComboBox();
        QLineEdit *box_note_duration = new QLineEdit();
        QComboBox *combo_scale = new QComboBox();
        QComboBox *combo_key = new QComboBox();
        QSpinBox *spinbox_octave = new QSpinBox();
        QComboBox *combo_harmony = new QComboBox();
        QPushButton *btn_sonify = new QPushButton(QIcon(":/icons/sonify.png"), " Sonify");
        QPushButton *btn_play = new QPushButton(QIcon(":/icons/play.png"), " Play");
        void _DrawerInit();
        sonify *_sonify;
};

#endif
