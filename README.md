### **INSTALLATION PREREQUISITES** 
  - Any C++ Compiler
  - Any system package manager (**If you are on Windows, you can NOT use a package manager**)
  - CMake
  - Git (**Optional**)

  To install, proceed to the **Building MovieTranslatorApp** header.

# **MOVIE PLAYER**

Media player built from scratch using OpenGL to draw video and FFmpeg to decode the video, with a custom SRT parser and subtitle translator which is reliant on the use of LibreTranslate. LibreTranslate is a translator API that is available online for self hosting. The app includes the following features:
  - Ability to play most FFmpeg compatible video files.
  - Modifiable subtitle delays, font colors, locations, and scale.
  - Real time language translation switching. 
  - Interactive overlay all done with the mouse.
  - Seeking video and video pausing.

<br>

And the following technical features:

  - FFmpeg to decode the video and audio
  - OpenAL for playback of the decoded audio
  - HTTPLib and nlohmann::json for communication with the LibreTranslate API
  - OpenGL + GLAD + GLFW + Freetype for the display of the decoded video and subtitles
  - All major components are multithreaded for maximum performance

This project was designed to help other language learners enjoy movies in their target language while boosting their comprehension skills. It allows the learner to view both what the native speaker is saying(and native speakers tend to speak *fast*), as well as the translated text to the user's native language, providing maximum learning efficiency. 

<br>

## **Building MovieTranslatorApp**

### **LibreTranslate**

LibreTranslate is a language translation API that allows self hosting, in order for MovieTranslatorApp to run, LibreTranslate is required. Visit their repository at https://github.com/LibreTranslate/LibreTranslate and follow the quickstart guide in the README to self host LibreTranslate. 

### **IMPORTANT:**
  - **The first part of the build process is separated by OS**, ensure you are following the steps for your specific OS.
  - If you run into any issues during the build process, scroll to the **Troubleshooting** header.
  - Ensure you read the **Additional Notes** header or you might run into strange issues when running the program. 
  - Read the **Additional Arguments** header for important details on the features MovieTranslatorApp provides.

The instructions will start with Linux, followed by Windows, and then MacOS. The build process will conclude with general instructions for each system, found under the **General Instructions** header. 

### Linux

**Open a new terminal, and then run this command:**
  - ```<your package manager> install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev```

**If this fails or says packages not found, then run:**
  - ```<your package manager> install ffmpeg```

This is required because some Linux distributions require directly installing FFmpeg libraries (libavcodec, libavutil) and others install these libraries with the general FFmpeg install. 

Now, **you need to ensure** that you have a compatible audio device which OpenAL can find. (PulseAudio, ALSA, or PipeWire) Others may work, but these are the ones I am aware of and tested. **If you do not have a compatible audio device, OpenAL will fail to find your default audio device and a full project redownload will be required.**

**Run the following command:**
  - ```<your package manager> install libopenal-dev```

**If this fails or says packages not found, OpenAL Soft is automatically downloaded later in the build process.** This command is required to try because on some distributions OpenAL Soft alone will not work. (I am not quite sure why.)

**Ensure that all steps are completed**, then proceed to the **General Instructions** header to complete the build process.

<br>

### Windows

Since FFmpeg is not automatically configured with CMake, it must be manually installed through one of FFmpeg's official releases and placed in the location MovieTranslatorApp expects. 

  - Go to https://github.com/BtbN/FFmpeg-Builds/releases and install '**ffmpeg-master-latest-win64-gpl-shared.zip**'
  - Make a folder in your C drive titled '**ffmpeg**'
  - Extract the contents of the newly downloaded folder into the ffmpeg folder. 

**You should have C:/ffmpeg/include, ensure it is not C:/ffmpeg/\<downloaded_folder\>/include** or FFmpeg will fail to be found when running MovieTranslatorApp.

After this is complete, you can proceed to the **General Instructions** header.

<br>

### MacOS

Simply open a new terminal and run ```<your package manager> install ffmpeg```. After this is done, proceed to the **General Instructions** header. 

<br>

## General Instructions

**Ensure you completed the OS specific steps before running these commands.** 

Change directory to the location you want to install the MovieTranslatorApp folder to, and run:

  - ```git clone https://github.com/Gavin8233/MovieTranslatorApp```

Or alternatively, you can install the zip file from GitHub and extract the folder to your desired location.

**Now, change directory into the installed MovieTranslatorApp folder and run the following commands:**

  - ```mkdir build```
  - ```cd build```
  - ```cmake .. -DCMAKE_BUILD_TYPE=Release```
  - ```cmake --build . --config Release```

**This final command requires admin**, if you are on Linux or Mac run it with ```sudo``` pre-fixed. If you are on Windows, open an admin command prompt and change directory to the build folder. **Run this command:**

  - ```cmake --install .```

<br>

### Start the app

To start the application, assure that LibreTranslate is running on localhost:5000 (**This can be overridden with an argument**) before running the executable.

To run the executable you need to pass in 2 arguments. 
  - The path to your movie file
  - The path to your SRT file

**Do not add any - to your arguments,** it **should** look like this:
  - ```./MovieTranslatorApp path/to/video path/to/srtfile```

And **not** this:
  - ```./MovieTranslatorApp -path/to/video --path/to/srtfile```

<br>

### Additional Notes

You can enter fullscreen mode by clicking 'F' and exit it by clicking 'F' again. 

Depending on the language you are attempting to display, some font files will not support it. If you need to provide your own font file that works for your language read the **Additional Arguments** header and look for the ```fontfile``` argument. 

If your video is high quality and the video is not decoding fast enough (It will be in slow motion), read the **Additional Arguments** header and look for the ```-hpTrue <threads>``` argument and pass this argument into MovieTranslatorApp. 

If you are on Linux or Mac and the SRT file you install uses Windows line endings, you need to install dos2unix or another similar tool and run ```dos2unix subtitles.srt```. If you are not sure what line endings are used run MovieTranslatorApp and it will throw an exception and an error message telling you that the SRT file needs to be converted. 


#### **You can find the executable at the following locations:**

|     OS       | Path                                                                                |
| :--------:   | :---------------------------------------------------------------------------------- |
| **Windows**  | **C:/Program Files/MovieTranslatorApp/bin/MovieTranslatorApp**                      |
| **Linux**    | **/usr/local/bin/MovieTranslatorApp**                                                   |
| **MacOS**    | **/usr/local/MovieTranslatorApp.app/Contents/MacOS/MovieTranslatorApp**             |

#### **And the resources and shaders folder at the following locations:**

|     OS       | Path                                                                                |
| :--------:   | :---------------------------------------------------------------------------------- |
| **Windows**  | **C:/Program Files/MovieTranslatorApp/share/MovieTranslatorApp/**                   |
| **Linux**    | **/usr/local/share/MovieTranslatorApp/**                                            |
| **MacOS**    | **/usr/local/MovieTranslatorApp.app/Contents/**                                              |

<br>

**Please note that this project is not extensively tested on each machine, and error handling is quite minimal.**

<br>

### Additional Arguments

You can pass the following additional arguments into Movie Translator.

|             Argument                  |                       Description                             |     Default      |
| :-----------------------------------: | :-----------------------------------------------------------: | :--------------: |
|       ```-librehost <ip>```           |        Overrides the IP HTTPLib Client defaults to            |   **localhost**  |
|       ```-libreport <port>```         |        Overrides the port HTTPLib Client defaults to          |   **5000**       |
|       ```-hpTrue <threads>```         |        Enables multi-threaded video decoding                  |   **False**      |
|       ```-printinfo```                |        Prints the SRT and video file path you passed in       |   **False**      |
|       ```-fontfile <path/to/.ttf>```  |        Overrides the font file Freetype uses                  |   **arial.ttf**  |

### Environment Variables

You can also set the following environment variables if you place your texture location or shaders in a different spot from where it is expected. 

  - ```set MOVIE_APP_RESOURCES_PATH=<path/to/resources>```
  - ```set MOVIE_APP_SHADER_PATH=<path/to/shaders>```

For Linux and Mac, replace ```set``` with ```export```.

<br>

# **TROUBLESHOOTING**

Problems I ran into while building on different systems are defined here with corresponding solutions. The error message received is first followed by a colon and the command that will fix the issue. Commands will use(sudo apt install, brew install) but any package manager can be used. **With the correct steps followed I was unable to produce any build problems on Windows**

### Linux:

  - #### **Failed to find wayland-scanner** : ```sudo apt install wayland-protocols libwayland-dev```
  - #### **pkg-config tool not found** : ```sudo apt install pkg-config```
  - #### **xkbcommon >= 0.5.0** : ```sudo apt install libxkbcommon-dev```
  - #### **Could not find X11** : ```sudo apt install libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev```
  - #### **Could not find OpenGL** : ```sudo apt install libgl1-mesa-dev```

<br>

### Mac:
  - #### **Could not find PkgConfig** : ```brew install pkg-config```

<br>

**I am sure that there are other issues,** but these were the only ones that I ran into during the build process on **Windows**, **Linux**, and **Mac**.

<br><br><br>