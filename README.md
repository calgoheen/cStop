# cStop

<p align="center"><img src="cStop.png"></p>

cStop is a tape stop audio effect plugin available in AU & VST3 for Mac (Windows coming soon). 

## How to download

cStop is currently only available for MacOS. Go to the [releases page](https://github.com/calgoheen/cStop/releases/tag/v1.0.0) to download the latest release installer.

## Tips on using

- The only parameter you need to automate is called "Mode"
- When using the tape START mode, the audio will be out of sync with real time until you go back to BYPASS

## Build the code

```
# clone the repo
git clone https://github.com/calgoheen/cStop.git --recurse-submodules

# build with CMake
cmake -Bbuild -DCMAKE_BUILD_TYPE="Release"
cmake --build build
```

## License and Acknowledgements

cStop is powered by [chowdsp_utils](https://github.com/Chowdhury-DSP/chowdsp_utils). 

While cStop is licensed under the MIT license, chowdsp_utils is not, so please refer to that repository's [license](https://github.com/Chowdhury-DSP/chowdsp_utils/blob/master/LICENSE.md) for more information.
