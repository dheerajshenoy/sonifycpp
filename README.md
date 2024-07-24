# SonifyCPP

Convert images to audio signals.

## Introduction

Image sonification refers to the process of converting images to audio signals. This is done by mapping the pixels of the image to frequencies of sound. The way in which the image is traversed changes the resultant audio, and the way in which the pixels are mapped to frequencies also changes the audio. So, there can be an infinite possibilites of mapping things.

### SonifyCPP is currently able to traverse images in the following manner:
1. Left to Right (Linear)
2. Right to Left (Linear)
3. Top to Bottom (Linear)
4. Bottom to Top (Linear)
5. Circular Outwards
6. Circular Inwards (In progress)

### Pixel mapping
Currently, I am taking the **grayscale** of the input image, mapping the pixel position (x, y) to (t, frequency) and pixel intensity to amplitude. In future, I'll add many more mapping schemes.

![paper0](https://github.com/user-attachments/assets/4c64fdbf-7d45-439c-b873-848dd09dd490)

Credit: [Link](https://www.seeingwithsound.com/im2sound.htm)

*NOTE*: This is a performant version of [Sonify](https://github.com/dheerajshenoy/sonify/) which is written in Python.

## TODO

1. [ ] Nice GUI
2. [x] Save resulting audio
3. [ ] Option for mapping selection
4. [ ] Support for custom mapping user functions (maybe add lua scripting support ?)
5. [ ] Waveform visualizer
6. [ ] Export audio to various formats like MP4, AAC etc.
7. [ ] Pixel audio visualiser (inpsecting each column or row of pixel from the image and producing the audio instantly).
8. [ ] Video record the sonification image

# Demo

*WARNING*: Lower your volume when listening to the video.

https://github.com/user-attachments/assets/8486dbb2-789e-456c-ac6e-8df99d13e622

https://github.com/user-attachments/assets/5749613d-6004-4d84-90ae-adaa8904268f



