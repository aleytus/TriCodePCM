TriCodePCM
==========
Generate and read PCM, written in C++ for Windows and Linux

You can find some basic information in our [**website**](https://blog.vintage-cd.ru/?p=540). 
For discussion about this Digital Audio Processor PCM-F1 please visit our [**forums**](https://forum.fagear.ru/viewtopic.php?t=88) and our [**Discord server**](https://discord.gg/BrXa2Kj).

## Development
If you want to contribute please take a look at the [Developer Information](https://github.com/walhi/pcm) pages. You should as well contact any of the developers in the forums or in Discord in order to know more about the current situation of the emulator.

## Download
* [TriCodePCM for Windows x86_64](https://github.com/aleytus/TriCodePCM/releases)

## Clone and initialize the repository
```
git clone https://github.com/aleytus/TriCodePCM.git
cd TriCodePCM
git submodule update --init
```

## Dependencies
### Windows
* [Visual Studio 2019](https://visualstudio.microsoft.com/ru/downloads/)
* [OpenCV 4.2.0+](https://opencv.org/releases/)
```
Add OPENCV_DIR variable to OpenCV\Build\[arch]\[compiler]
Edit PATH variable to %OPENCV_DIR%\bin
```
* [Libsndfile 1.0.28+](http://www.mega-nerd.com/libsndfile/#Download)
```
Add SNDFILE_DIR variable to "C:\Program Files\Mega-Nerd\libsndfile"
Edit PATH variable to %SNDFILE_DIR%\bin
```

### Linux
* [Kdevelop](https://www.kdevelop.org/download) or make
* OpenCV 4.2.0+
* Libsndfile 1.0.28+

## License
Most files are licensed under the terms of GNU GPLv2 License, see LICENSE file for details. Some files may be licensed differently, check appropriate file headers for details.
