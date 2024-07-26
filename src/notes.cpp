#include "notes.hpp"


Notes::Notes()
{

    frequencies["C0"] = 16.35;
    frequencies["C#0"] = 17.32;
    frequencies["D0"] = 18.35;
    frequencies["D#0"] = 19.45;
    frequencies["E0"] = 20.60;
    frequencies["F0"] = 21.83;
    frequencies["F#0"] = 23.12;
    frequencies["G0"] = 24.50;
    frequencies["G#0"] = 25.96;
    frequencies["A0"] = 27.50;
    frequencies["A#0"] = 29.14;
    frequencies["B0"] = 30.87;

    frequencies["C1"] = 32.70;
    frequencies["C#1"] = 34.65;
    frequencies["D1"] = 36.71;
    frequencies["D#1"] = 38.89;
    frequencies["E1"] = 41.20;
    frequencies["F1"] = 43.65;
    frequencies["F#1"] = 46.25;
    frequencies["G1"] = 49.00;
    frequencies["G#1"] = 51.91;
    frequencies["A1"] = 55.00;
    frequencies["A#1"] = 58.27;
    frequencies["B1"] = 61.74;

    frequencies["C2"] = 65.41;
    frequencies["C#2"] = 69.30;
    frequencies["D2"] = 73.42;
    frequencies["D#2"] = 77.78;
    frequencies["E2"] = 82.41;
    frequencies["F2"] = 87.31;
    frequencies["F#2"] = 92.50;
    frequencies["G2"] = 98.00;
    frequencies["G#2"] = 103.83;
    frequencies["A2"] = 110.00;
    frequencies["A#2"] = 116.54;
    frequencies["B2"] = 123.47;

    frequencies["C3"] = 130.81;
    frequencies["C#3"] = 138.59;
    frequencies["D3"] = 146.83;
    frequencies["D#3"] = 155.56;
    frequencies["E3"] = 164.81;
    frequencies["F3"] = 174.61;
    frequencies["F#3"] = 185.00;
    frequencies["G3"] = 196.00;
    frequencies["G#3"] = 207.65;
    frequencies["A3"] = 220.00;
    frequencies["A#3"] = 233.08;
    frequencies["B3"] = 246.94;

    frequencies["C4"] = 261.63;
    frequencies["C#4"] = 277.18;
    frequencies["D4"] = 293.66;
    frequencies["D#4"] = 311.13;
    frequencies["E4"] = 329.63;
    frequencies["F4"] = 349.23;
    frequencies["F#4"] = 369.99;
    frequencies["G4"] = 392.00;
    frequencies["G#4"] = 415.30;
    frequencies["A4"] = 440.00;
    frequencies["A#4"] = 466.16;
    frequencies["B4"] = 493.88;

    frequencies["C5"] = 523.25;
    frequencies["C#5"] = 554.37;
    frequencies["D5"] = 587.33;
    frequencies["D#5"] = 622.25;
    frequencies["E5"] = 659.25;
    frequencies["F5"] = 698.46;
    frequencies["F#5"] = 739.99;
    frequencies["G5"] = 783.99;
    frequencies["G#5"] = 830.61;
    frequencies["A5"] = 880.00;
    frequencies["A#5"] = 932.33;
    frequencies["B5"] = 987.77;

    frequencies["C6"] = 1046.50;
    frequencies["C#6"] = 1108.73;
    frequencies["D6"] = 1174.66;
    frequencies["D#6"] = 1244.51;
    frequencies["E6"] = 1318.51;
    frequencies["F6"] = 1396.91;
    frequencies["F#6"] = 1479.98;
    frequencies["G6"] = 1567.98;
    frequencies["G#6"] = 1661.22;
    frequencies["A6"] = 1760.00;
    frequencies["A#6"] = 1864.66;
    frequencies["B6"] = 1975.53;

    frequencies["C7"] = 2093.00;
    frequencies["C#7"] = 2217.46;
    frequencies["D7"] = 2349.32;
    frequencies["D#7"] = 2489.02;
    frequencies["E7"] = 2637.02;
    frequencies["F7"] = 2793.83;
    frequencies["F#7"] = 2959.96;
    frequencies["G7"] = 3135.96;
    frequencies["G#7"] = 3322.44;
    frequencies["A7"] = 3520.00;
    frequencies["A#7"] = 3729.31;
    frequencies["B7"] = 3951.07;

    frequencies["C8"] = 4186.01;
    frequencies["C#8"] = 4434.92;
    frequencies["D8"] = 4698.63;
    frequencies["D#8"] = 4978.03;
    frequencies["E8"] = 5274.04;
    frequencies["F8"] = 5587.65;
    frequencies["F#8"] = 5919.91;
    frequencies["G8"] = 6271.93;
    frequencies["G#8"] = 6644.88;
    frequencies["A8"] = 7040.00;
    frequencies["A#8"] = 7458.62;
    frequencies["B8"] = 7902.13;

}

qreal Notes::getFrequency(QString key)
{
    return frequencies.value(key, -1);
}
