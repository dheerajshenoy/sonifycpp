#ifndef __DRAWER_CPP
#define __DRAWER_CPP

#include "sonify.hpp"
#include "drawer.hpp"

Drawer::Drawer(sonify *s, QWidget *parent)
    : QWidget(parent)
{
    _DrawerInit();
    this->setLayout(drawerLayout);
    this->setContentsMargins(0, 0, 0, 0);
    _sonify = s;
}

void Drawer::_DrawerInit()
{
    QLabel *label_traverse = new QLabel("Traversing");
    QLabel *label_mapping = new QLabel("Mapping");
    QLabel *label_skip = new QLabel("Skip");
    QLabel *label_sample_rate = new QLabel("Sample Rate");
    QLabel *label_note_duration = new QLabel("Note Duration");
    QLabel *label_scale = new QLabel("Scale");
    QLabel *label_key = new QLabel("Key");
    QLabel *label_octave = new QLabel("Octave");
    QLabel *label_harmony = new QLabel("Harmony");

    // Traverse Box

    combo_traverse->addItem("Left to Right");
    combo_traverse->setItemIcon(0, QIcon(":/icons/left_to_right.png"));

    combo_traverse->addItem("Right to Left");
    combo_traverse->setItemIcon(1, QIcon(":/icons/right_to_left.png"));

    combo_traverse->addItem("Top to Botton");
    combo_traverse->setItemIcon(2, QIcon(":/icons/top_to_bottom.png"));

    combo_traverse->addItem("Bottom to Top");
    combo_traverse->setItemIcon(3, QIcon(":/icons/bottom_to_top.png"));

    combo_traverse->addItem("Radial");
    combo_traverse->setItemIcon(4, QIcon(":/icons/radial.png"));

    combo_traverse->addItem("Circular");
    combo_traverse->setItemIcon(5, QIcon(":/icons/circular.png"));

    // Mapping Box
    combo_mapping->addItem("Color to Frequency");

    // Skip Layout
    
    QHBoxLayout *skip_layout = new QHBoxLayout();
    QLabel *label_skipw = new QLabel("W");
    QLabel *label_skiph = new QLabel("H");
    box_skiph->setPlaceholderText("5");
    box_skipw->setPlaceholderText("5");


    skip_layout->addWidget(label_skipw);
    skip_layout->addWidget(box_skipw);
    skip_layout->addWidget(label_skiph);
    skip_layout->addWidget(box_skiph);

    // Sample Rate Box
    combo_sample_rate->addItems({ "22050 Hz", "44100 Hz", "48000 Hz" });

    // Note Duration Line Edit
    
    box_note_duration->setPlaceholderText("0.1");

    // Scale Box
    
    combo_scale->addItems({
            "AEOLIAN", "BLUES", "PHYRIGIAN", "CHROMATIC",
            "DORIAN", "HARMONIC_MINOR", "LYDIAN",
            "MAJOR", "MELODIC_MINOR", "MINOR", "MIXOLYDIAN",
            "NATURAL_MINOR", "PENTATONIC"});

    // Key Box
    combo_key->addItems({ "A","a","B","C","c","D","d","E","F","f","G","g" });

    // Octave Spinner   
    spinbox_octave->setRange(0, 3);

    // Harmony Box
    combo_harmony->addItems({
            "U0", "ST", "M2", "m3", "M3", "P4", "DT",
            "P5", "m6", "M6", "m7", "M7", "O8" });

    // Buttons
    
    QHBoxLayout *btn_layout = new QHBoxLayout();
    
    btn_play->setEnabled(false);
    btn_sonify->setEnabled(false);
    connect(btn_play, &QPushButton::clicked, this, [=]() { _sonify->Play(); });

    btn_layout->addWidget(btn_sonify);
    btn_layout->addWidget(btn_play);

    connect(btn_sonify, &QPushButton::clicked, this, [=]() { StartSonify(); });

    drawerLayout->addWidget(label_traverse, 0, 0);
    drawerLayout->addWidget(combo_traverse, 0, 1);
    drawerLayout->addWidget(label_mapping, 1, 0);
    drawerLayout->addWidget(combo_mapping, 1, 1);
    drawerLayout->addWidget(label_skip, 2, 0);
    drawerLayout->addLayout(skip_layout, 2, 1);
    drawerLayout->addWidget(label_sample_rate, 3, 0);
    drawerLayout->addWidget(combo_sample_rate, 3, 1);
    drawerLayout->addWidget(label_note_duration, 4, 0);
    drawerLayout->addWidget(box_note_duration, 4, 1);
    drawerLayout->addWidget(label_scale, 5, 0);
    drawerLayout->addWidget(combo_scale, 5, 1);
    drawerLayout->addWidget(label_key, 6, 0);
    drawerLayout->addWidget(combo_key, 6, 1);
    drawerLayout->addWidget(label_octave, 7, 0);
    drawerLayout->addWidget(spinbox_octave, 7, 1);
    drawerLayout->addWidget(label_harmony, 8, 0);
    drawerLayout->addWidget(combo_harmony, 8, 1);

    drawerLayout->addLayout(btn_layout, 9, 0, 1, 2);
    drawerLayout->setRowStretch(10, 10);

    this->setMaximumWidth(250);
}

void Drawer::StartSonify()
{
    _sonify->canvas->Traverse_TB_LR();
    _sonify->canvas->Img2Music();
    btn_play->setEnabled(true);
}

void Drawer::StopSonify()
{

}

QString Drawer::TraverseMethod()
{
    return this->combo_traverse->currentText();
}

QString Drawer::Mapping()
{
    return this->combo_mapping->currentText();
}

QString Drawer::Scale()
{
    return this->combo_scale->currentText();
}

QString Drawer::Key()
{
    return this->combo_key->currentText();
}

QString Drawer::SampleRate()
{
    return this->combo_sample_rate->currentText();
}

QString Drawer::NoteDuration()
{
    return this->box_note_duration->text();
}

QString Drawer::SkipW()
{
    return this->box_skiph->text();
}

QString Drawer::SkipH()
{
    return this->box_skipw->text();
}

QString Drawer::Octave()
{
    return this->spinbox_octave->text();
}

Drawer::~Drawer() {}

#endif
