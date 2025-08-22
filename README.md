# SonifyCPP

SonifyCPP is a powerful program for transforming image data into sound. It provides a flexible framework to sonify visual information — enabling new ways to interpret, analyze, or experience images through audio.

**Current development version: 0.1.2**

> [!WARNING]
> This is still a work in progress

## Table of Contents

1. [Introduction](#introduction)
2. [Features](#features)
3. [Traversal Methods](#traversal)
    1. [Left to Right (Linear)](#left_to_right)
    2. [Right to Left (Linear)](#right_to_left)
    3. [Top to Bottom (Linear)](#top_to_bottom)
    4. [Bottom to Top (Linear)](#bottom_to_top)
    5. [Circular Outwards](#circular_outwards)
    6. [Circular Inwards](#circular_inwards)
    7. [Clockwise (Linear)](#clockwise)
    8. [Anti-Clockwise (Linear)](#anti_clockwise)
    9. [Draw Path](#draw_path)
4. [Pixel Mapping](#pixel_mapping)
5. [Reference](#reference)
6. [Things to do](#todo)
7. [Demo](#demo)
8. [Installation](#installation)
9. [Changelogs and Bug fixes](#changelogs_and_bug_fixes)
10. [Inspirations](#inspirations)

<a name="introduction"/>

## Introduction

Image sonification refers to the process of converting images to audio signals. This is done by mapping the pixels of the image to frequencies of sound. The way in which the image is traversed changes the resultant audio, and the way in which the pixels are mapped to frequencies also changes the audio. So, there can be an infinite possibilites of mapping things.

This project is a performant successor to [Sonify](https://github.com/dheerajshenoy/sonify/) which was written in Python.

<a name="features" />

## Features

Apart from sonifying the images, sonifyCPP ships with few features.

* Tone Generator

    Just generate single frequency tones. It's fun

https://github.com/user-attachments/assets/d00d3c65-ccbb-42d5-90e7-c1b127f9cfe9


* Waveform visualizer

    Visualize the sonified sound waveform

  ![image](https://github.com/user-attachments/assets/65f23858-88f9-4ceb-85d1-e3f13ff9b572)

* Frequency Spectrum Analyzer

    Visualize the frequency vs amplitude plot

  ![image](https://github.com/user-attachments/assets/1c8997f1-b2d4-45cf-b50e-67f18348cc62)

<a name="traversal"/>

## Traversal Methods

SonifyCPP is currently able to traverse images in the following manner:

<a name="left_to_right" />

### 1. Left to Right (Linear)

![Left to Right](https://github.com/user-attachments/assets/5342b735-028f-429a-9c82-d8ac401a0769)

<a name="right_to_left" />

### 2. Right to Left (Linear)

![Right to Left](https://github.com/user-attachments/assets/68a9f8da-1116-4a0c-8f82-4806da90ba18)

<a name="top_to_bottom" />

### 3. Top to Bottom (Linear)

![Top to Bottom](https://github.com/user-attachments/assets/d0d54029-6cf5-43b9-a31d-cbf22b5f6ac2)

<a name="bottom_to_top" />

### 4. Bottom to Top (Linear)

![Bottom to Top](https://github.com/user-attachments/assets/7f4b1aef-1ea1-4208-874f-487adb1eb0ea)

<a name="circular_outwards" />

### 5. Circular Outwards

![Circular Outwards](https://github.com/user-attachments/assets/33dbc508-8a17-44b2-ac1c-2db59b249d1d)

<a name="circular_inwards" />

### 6. Circular Inwards

![Circular Inwards](https://github.com/user-attachments/assets/077c4ae8-e7e9-4e3c-931e-156799afd3a7)

<a name="clockwise" />

### 7. Clockwise (Linear)

![Clockwise](https://github.com/user-attachments/assets/3fb24eb7-847a-46f3-86f5-07962d3841bc)

<a name="anti_clockwise" />

### 8. Anti-Clockwise (Linear)

![Anti-Clockwise](https://github.com/user-attachments/assets/9c813886-d9b1-466d-ab91-4b40c0fde28c)

<a name="draw_path" />

### 9. Draw Path

![Draw Path](https://github.com/user-attachments/assets/7825fb6d-7b3e-4763-9604-b6b46677bdd7)

<a name="pixel_mapping"/>

### 10. Region

> [!NOTE]
> Not implemented yet!

### Pixel mapping

By default, there are two kinds of pixel to sound mapping:

1. **intensity** of the input image, mapping the pixel position (x, y) to (t, frequency) and pixel intensity to amplitude.

2. **hsv**, map the hue to amplitude.

You can write your own pixel sound mapping through C++ code, check out [PIXEL_MAPPING_DOC.md](./pixel_mapping_doc.md)

### Reference

![paper0](https://github.com/user-attachments/assets/4c64fdbf-7d45-439c-b873-848dd09dd490)

Credit: [Link](https://www.seeingwithsound.com/im2sound.htm)

<a name="todo"/>

## TODO

* [x] Nice GUI
* [x] Save resulting audio
* [x] Waveform visualizer
* [x] Path-based Sonification: Draw a path on the image and sonify along that path.
* [x] Color Mapping: Map different colors to different sounds or musical notes.
* [x] Frequency Analyzer
* [x] Implement multi-threading
* [x] Option for mapping selection
* [x] Support for custom mapping user functions (add scripting support ?)
* [ ] Pixel audio visualiser (inpsecting each column or row of pixel from the image and producing the audio instantly).
* [ ] Video Export: Create a video file that combines the image and the sonification, showing the progression over time.
* [ ] **Reverse audio to produce image**
* [ ] Sonify specific regions or objects within the image.
* [ ] Map textures or patterns within the image to different sound effects.
* [ ] Effect Processing: Add reverb, echo, or other audio effects to enhance the sonification experience.
* [ ] Dynamic Range Compression: Ensure that the sound levels are balanced and not too harsh or too quiet.
* [ ] Adjustable Speed: Allow users to slow down or speed up the sonification process.
* [ ] MIDI Support: Allow users to export sonification data as MIDI files for further musical manipulation.
* [ ] Spectrogram: Display a spectrogram that shows the frequency content of the sonified sound over time.
* [ ] 3D Visualization: Provide a 3D view of the image and its corresponding soundscape.
* [ ] **Handle Large Images**

<a name="demo"/>

# Demo

> [!WARNING]
> Lower your volume when listening to the video.

These are demo from when the software was in version 0.1. Have to update the videos.

https://github.com/user-attachments/assets/8486dbb2-789e-456c-ac6e-8df99d13e622

https://github.com/user-attachments/assets/5749613d-6004-4d84-90ae-adaa8904268f

**The new GUI looks like this**

![image](https://github.com/user-attachments/assets/95d032fa-a642-4063-bbde-a555baa6b47d)

<a name="installation" />

# Installation

> [!NOTE]
> I haven't tested this software on Windows, but it works on Linux.

1. Make sure you have the following dependencies

    - `Qt6` (GUI)
    - `libsndfile` (reading and writing audio files)
    - `SDL2` (audio playback)
    - `ninja` (build dependency)
    - `fftw3` (fast fourier transforms and inverse transforms)

Since I use Arch Linux, the commands to install these packages is `sudo pacman -S qt6 libsndfile sdl2 fftw ninja`

2. Clone this repo and go to the directory

    `git clone https://github.com/dheerajshenoy/sonifycpp && cd sonifycpp`

3. Create a build directory and run cmake`

    `mkdir build && cmake -G Ninja -Bbuild -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release`

4. Run make inside the build directory

    `make -j$(nproc)`

<a name="changelogs_and_bug_fixes" />

Please see [CHANGELOG.md](./CHANGELOG.md)

<a name="faq" />

# FAQ

## Why ?

Main reason why I started this is because of an earlier basic Python version I started on 2023
when I participated in the 2023 NASA Hackathon. It was slow... it was python, and I love C++, so I thought I'll create this in C++.

## Why the wierd name ?

Because I already had a python project called sonify, and i wanted CPP in the name, hence sonifyCPP.

<a name="inspirations"/>

# Inspirations

* NASA has been a big inspiration for making me create this software. If you haven't checked out yet, they have good sonification videos on youtube.
