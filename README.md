# SonifyCPP
Convert images to audio signals.

## Table of Contents

1. [Introduction](#introduction)
2. [Traversal Methods](#traversal)
    1. [Left to Right (Linear)](#left_to_right)
    2. [Right to Left (Linear)](#right_to_left)
    3. [Top to Bottom (Linear)](#top_to_bottom)
    4. [Bottom to Top (Linear)](#bottom_to_top)
    5. [Circular Outwards](#circular_outwards)
    6. [Circular Inwards](#circular_inwards)
    7. [Clockwise (Linear)](#clockwise)
    8. [Anti-Clockwise (Linear)](#anti_clockwise)
    9. [Draw Path](#draw_path)
3. [Pixel Mapping](#pixel_mapping)
4. [Things to do](#todo)
5. [Demo](#demo)

<a name="introduction"/>

## Introduction

Image sonification refers to the process of converting images to audio signals. This is done by mapping the pixels of the image to frequencies of sound. The way in which the image is traversed changes the resultant audio, and the way in which the pixels are mapped to frequencies also changes the audio. So, there can be an infinite possibilites of mapping things.

This project is a performant successor to [Sonify](https://github.com/dheerajshenoy/sonify/) which was written in Python.

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

![]()

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
5. [ ] Waveform visualizer
6. [ ] Export audio to various formats like MP4, AAC etc.
7. [ ] Pixel audio visualiser (inpsecting each column or row of pixel from the image and producing the audio instantly).
8. [ ] Video Export: Create a video file that combines the image and the sonification, showing the progression over time.
9. [ ] **Reverse audio to produce image**
10. [ ] Path-based Sonification: Draw a path on the image and sonify along that path.
11. [ ] Sonify specific regions or objects within the image.
12. [ ] Color Mapping: Map different colors to different sounds or musical notes.
13. [ ] Map textures or patterns within the image to different sound effects.
14. [ ] Effect Processing: Add reverb, echo, or other audio effects to enhance the sonification experience.
15. [ ] Dynamic Range Compression: Ensure that the sound levels are balanced and not too harsh or too quiet.
16. [ ] Adjustable Speed: Allow users to slow down or speed up the sonification process.
17. [ ] MIDI Support: Allow users to export sonification data as MIDI files for further musical manipulation.
18. [ ] Spectrogram: Display a spectrogram that shows the frequency content of the sonified sound over time.
19. [ ] 3D Visualization: Provide a 3D view of the image and its corresponding soundscape.

<a name="demo"/>

# Demo

*WARNING*: Lower your volume when listening to the video.

https://github.com/user-attachments/assets/8486dbb2-789e-456c-ac6e-8df99d13e622

https://github.com/user-attachments/assets/5749613d-6004-4d84-90ae-adaa8904268f

# NOTE: The new GUI looks cool and it's not the same as shown in the demo videos

![2024-07-25_11-04](https://github.com/user-attachments/assets/ddd37fa0-c448-4d0a-992a-6850777874fb)



