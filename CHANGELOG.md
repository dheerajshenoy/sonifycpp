# Chanegelogs and Bug fixes

- 21 Aug 2024

    - Change build system from qmake to CMake.
    - Remove #ifndef statments and replace with #pragma once
    - Add `const` keyword to constant references to functions
    - Add `noexcept` keyword to functions

- 4 Aug 2024

    - New Feature

        - *Image Editor*. Change Brightness, Saturation, Contrast, Gamma, Invert colors, Grayscale.

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
