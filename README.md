# cStop

<p align="center"><img src="cStop.png"></p>

cStop is a tape stop audio effect plugin available for Mac and Windows in VST3, CLAP, and AU (Mac only) formats.

## How to download

Go to the [releases page](https://github.com/calgoheen/cStop/releases) to download the latest release installer.

**Mac:** Use the installer with extension **.pkg**

**Windows:** Use the installer with extension **.exe**

## Tips on using

- The only parameter you need to automate is called "Mode"
- When using the tape START mode, the audio will be out of sync with real time until you go back to BYPASS

## Build instructions

Mac:
```
# Clone the repo
git clone https://github.com/calgoheen/cStop.git

# Build with CMake
cmake -Bbuild -DCMAKE_BUILD_TYPE="Release"
cmake --build build
```

Windows:
```
# Clone the repo
git clone https://github.com/calgoheen/cStop.git

# Build with CMake
cmake -Bbuild
cmake --build build --config Release
```

## License and Acknowledgements

cStop is powered by [chowdsp_utils](https://github.com/Chowdhury-DSP/chowdsp_utils). 

While cStop is licensed under the MIT license, chowdsp_utils is not, so please refer to that repository's [license](https://github.com/Chowdhury-DSP/chowdsp_utils/blob/master/LICENSE.md) for more information.
