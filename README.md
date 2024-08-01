# SonifyCPP
Convert images to audio signals.

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
5. [Things to do](#todo)
6. [Demo](#demo)
7. [Installation](#installation)
8. [Changelogs and Bug fixes](#changelogs_and_bug_fixes)
9. [Inspirations](#inspirations)

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

* Lua scripting

    Able to set options of the program in a script file. For now, only defaults can be set. In the future, I am planning to add the ability to create mapping functions from within lua to call in the C++ code for sonifying. Script file should be placed at `/home/<USER>/.config/sonifycpp/` on linux and `C:/Users/<USER>/AppData/Local/sonifycpp/`, The file can look something like this.

    ```lua
    -- default settings
    Defaults = {
        object_color = "#FFFFFF", -- set the curve color
        samples = 1024, -- default samples

        --
        -- The traverse mode can be one of the following strings:
        -- "LeftToRight", "RightToLeft", "TopToBottom", "BottomToTop", "Clockwise", "Anticlockwise", "CircleOutwards", "CircleInwards", "Path"
        --

        traverse_mode = "Anticlockwise",
        side_panel = "top", -- "left", "right", "top", "bottom"
        resolution = { height = 480, width = 720, keep_aspect = false; ask_for_resize = false; }, -- default image loading resolution
        icons = true, -- show menu and button icons or not ("true", "false")
        menubar = true, -- show menubar
        panel = true, -- show panel
        statusbar = true, -- show statusbar
    }

    local mapping1 = function ()
        print("HELLO WORLD FROM LUA")
    end

    -- Custom mapping
    Maps = {
        { name = "Mapping1", func = mapping1 },
    }

    ```

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
 
### Pixel mapping
Currently, I am taking the **grayscale** of the input image, mapping the pixel position (x, y) to (t, frequency) and pixel intensity to amplitude. In future, I'll add many more mapping schemes.

![paper0](https://github.com/user-attachments/assets/4c64fdbf-7d45-439c-b873-848dd09dd490)

Credit: [Link](https://www.seeingwithsound.com/im2sound.htm)

<a name="todo"/>

## TODO

1. [x] Nice GUI
2. [x] Save resulting audio
3. [ ] Option for mapping selection
4. [ ] Support for custom mapping user functions (maybe add lua scripting support ?)
5. [x] Waveform visualizer
6. [ ] Export audio to various formats like MP4, AAC etc.
7. [ ] Pixel audio visualiser (inpsecting each column or row of pixel from the image and producing the audio instantly).
8. [ ] Video Export: Create a video file that combines the image and the sonification, showing the progression over time.
9. [ ] **Reverse audio to produce image**
10. [x] Path-based Sonification: Draw a path on the image and sonify along that path.
11. [ ] Sonify specific regions or objects within the image.
12. [x] Color Mapping: Map different colors to different sounds or musical notes.
13. [ ] Map textures or patterns within the image to different sound effects.
14. [ ] Effect Processing: Add reverb, echo, or other audio effects to enhance the sonification experience.
15. [ ] Dynamic Range Compression: Ensure that the sound levels are balanced and not too harsh or too quiet.
16. [ ] Adjustable Speed: Allow users to slow down or speed up the sonification process.
17. [ ] MIDI Support: Allow users to export sonification data as MIDI files for further musical manipulation.
18. [ ] Spectrogram: Display a spectrogram that shows the frequency content of the sonified sound over time.
19. [ ] 3D Visualization: Provide a 3D view of the image and its corresponding soundscape.
20. [ ] **Handle Large Images**
21. [x] Frequency Analyzer
22. [x] Implement multi-threading

<a name="demo"/>

# Demo

*WARNING*: Lower your volume when listening to the video.
These are demo from when the software was in version 0.1. Have to update the videos.

https://github.com/user-attachments/assets/8486dbb2-789e-456c-ac6e-8df99d13e622

https://github.com/user-attachments/assets/5749613d-6004-4d84-90ae-adaa8904268f

**The new GUI looks like this**

![image](https://github.com/user-attachments/assets/95d032fa-a642-4063-bbde-a555baa6b47d)

<a name="installation" />

# Installation

**NOTE: I haven't tested this software on Windows, but it works on Linux.**

1. Make sure you have the following dependencies
    
    - `Qt6` (GUI)
    - `libsndfile` (reading and writing audio files)
    - `SDL2` (audio playback)
    - `lua` (scripting)
    - `fftw3` (fast fourier transforms and inverse transforms)

Since I use Arch Linux, the commands to install these packages is `sudo pacman -S qt6 libsndfile sdl2 fftw`

2. Clone this repo and go to the directory
    
    `git clone https://github.com/dheerajshenoy/sonifycpp && cd sonifycpp`

3. Run qmake to produce the makefile and make to build

    `qmake6 . && make`

4. The binary will be in the `bin` folder. You can move this file to appropriate location like /usr/bin/ if you want system wide access.

<a name="changelogs_and_bug_fixes" />

# Chanegelogs and Bug fixes

- 1 Aug 2024

    - Visual overhaul

        - *Added icon support for menu items*

        - *Support for four different panel positions* - Left, Right, Top or Bottom
        
    - Shortcuts

    - New options exposed to the lua script to toggle icons, default keybindings, menubar, statusbar, panel position.

- 31 July 2024

    - Bug fix

        The multi-threading support has been added for the rest of the traversals which were previously mentioned as not working. (*NOTE TO THE PROGRAMMER*: Never pass anything by reference to thread calls)

    - New Feature

        1. *Multi-threading support*. The image traversal algorithm performance has been boosted through multithreading support. *NOTE* : Not all traversal support multi-threading for now. Only the following supports multi-threading
            - Left to Right
            - Right to Left
            - Top to Bottom
            - Bottom to Top
            
            The rest will still sadly be single-threaded until it's updated.

        2. *Effecient pixel reading*. This is bit technical. Previously, pixels from image were read one by one and sine waves were generated. This is very memory and CPU intensive when the number of pixels are more. So, to increase the efficiency, pixels are read in groups, and the waves are produced for the group.

        3. Tone generator
            - *Wave visualizer for tone generator*. See realtime changes in frequency, amplitude and duration through plot of the tone being generated.
            - *Progress of tone being played*.

        4. Audio Effects in the works
            - *Reverb*. This works perfectly. More effects are to be added like phaser, wah-wah, compressor etc.

- 29 July 2024

    - New feature

        - *View frequency Spectrum*. Plot frequency vs amplitude of the produced sonified sound

        - *Added lua scripting support*. For now, the defaults for sample rate, asking for resizing everytime you open an image, resolution when loading an image, object color can be set through lua which will be accessed at startup if the lua file exists.

    - Bug fix

        - Handle waveform, spectrum analyser button when no sonified audio was found.

- 27 July 2024

    - New feature

        - Drag and Drop to open Image

        - Waveform visualization

      ![Waveform](https://github.com/user-attachments/assets/77632937-f965-4782-a547-1770e57b17fc)

        - Ask for resizing image when opening

    - Bug fix

        - Back to back change in different traversal methods 
        - Buffer size effect on the animation

- 26 July 2024

    - Bug fix

        - Fix delay between animation and audio playback

<a name="inspirations"/>

# Inspirations

* NASA has been a big inspiration for making me create this software. If you haven't checked out yet, they have good sonification videos on youtube.

* Main reason why I started this is because of an earlier basic Python version I started on 2023 when I participated in the 2023 NASA Hackathon. It was slow and...it was python, and I love C++, so I thought I'll create this in C++.


