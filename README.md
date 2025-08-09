### **INSTALLATION PREREQUISITES** 
  - Any C++ Compiler
  - Any system package manager (**If you are on Windows, you can NOT use a package manager**)
  - CMake
  - Git (**Optional**)

  To install, proceed to the **Getting Started** header.

## Quickstart for Linux & Mac Users
**Linux Users -** Ensure you have PulseAudio ALSA or PipeWire installed, or OpenAL will fail to find your default audio device and a full project redownload will be required. 

LibreTranslate is required and has to be running before launching the executable. 
Example: 
  - ```libretranslate --host 0.0.0.0 --port 5000```

Run the following:
  - **If Linux:**
    -  ```<your package manager> install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev```
    - ```<your package manager> install libopenal-dev```
  - **If Mac:**
    - ```<your package manager> install ffmpeg```
  
  - ```git clone https://github.com/Gavin8233/MovieTranslatorApp```
  - ```cd MovieTranslatorApp```
  - ```mkdir build```
  - ```cd build```
  - ```cmake ..```
  - ```cmake --build .```
  - ```cmake --install .```
  - ```./MovieTranslatorApp path/to/video path/to/.srt```

<br>

# **MOVIE PLAYER**

Custom media player built from scratch using OpenGL to draw video and FFmpeg to decode the video, with a custom SRT parser and subtitle translator which is reliant on the use of LibreTranslate. LibreTranslate is a translator API that is available online for self hosting. The app includes the following features:
  - Ability to play any FFmpeg compatible video 
  - Modifiable subtitle delays, font colors, locations, and scale.
  - Real time language translation switching. 
  - Interactive overlay all done with the mouse
  - Seeking video and video pausing

<br>

And the following technical features:

  - FFmpeg to manually decode the video and audio
  - OpenAL for playback of the decoded audio
  - HTTPLib and nlohmann::json for communication with the LibreTranslate API
  - OpenGL + GLAD + GLFW + Freetype for the display of the decoded video and subtitles
  - All major components are multithreaded for maximum performance

This project was designed to help other language learners enjoy movies in their target language while boosting their comprehension skills. It allows the learner to view both what the native speaker is saying(and native speakers tend to speak *fast*), as well as the translated text to the user's native language, providing maximum learning efficiency. The build instructions are below, and I hope them to be as simple as possible. 


## **GETTING STARTED**

First off, we need to actually install the project files. To do this, change directory to the location you want to install the files to and run:

  - ```git clone https://github.com/Gavin8233/MovieTranslatorApp```

Or alternatively, you can install the zip file from GitHub and extract the folder to your desired destination. 

### **LibreTranslate**

LibreTranslate is a language translation API that allows self hosting, in order for this project to run, LibreTranslate is required. Visit their repository at https://github.com/LibreTranslate/LibreTranslate and follow the quickstart guide in the README to self host LibreTranslate, and then proceed to the next step. 

### **FFmpeg**

FFmpeg is not automatically configured with CMake, so it must be manually installed. If you are on Linux or Mac, use your system's package manager and install FFmpeg. If you are on Windows,
you **MUST** install FFmpeg through one of their official releases and not a package manager, the instructions for each system are below:

  - Windows:
    - Go to https://github.com/BtbN/FFmpeg-Builds/releases
    - Install '**ffmpeg-master-latest-win64-gpl-shared.zip**'
    - Make a folder in your C drive titled '**ffmpeg**'
    - Extract the contents of the newly downloaded folder into the ffmpeg folder. 
    - **You should have C:/ffmpeg/include, ensure it is not C:/ffmpeg/\<downloaded_folder\>/include.**

  - Linux: ```sudo apt install libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev```

  - Mac: ```brew install ffmpeg```

**(any package manager can be used, these are just examples for the most common)**

### **OpenAL**

**If you are on Windows or Mac, you can skip to the Build header.**

 OpenAL is an audio playback library for c++. In order for it work on Linux, you need to ensure the following. 

  - You have a compatible audio playback driver (PulseAudio, ALSA, or PipeWire).
    - **If you do not**, run ```<your package manager> install libpulse-dev libasound2-dev```

  - You have OpenAL installed.
    - **If you do not**, run ```<your package manager> install libopenal-dev```.

**Make sure you have a compatible audio playback driver** or you will have to delete and reinstall all project files for OpenAL to properly find your audio device. 

### **Build**

After all these steps are complete, read back through and assure you completed **everything**.

Then, change directory into the **MovieTranslatorApp** folder and run the following commands:
  - ```mkdir build```
  - ```cd build```
  - ```cmake ..```
  - ```cmake --build .```. If you want this to run faster, add ```--parallel <job amount>``` to the end. 
  - ```cmake --install .```

The final command will install the necessary resources and the /bin folder to a default location depending on your system. 
You can find the files at the following locations:

  - #### Windows
    - **C:/Program Files/MovieTranslatorApp/bin/** and **C:/Program Files/MovieTranslatorApp/share/MovieTranslatorApp/**

  - #### Linux
    - **/usr/local/bin/** and **/usr/local/share/MovieTranslatorApp/**

  - #### Mac
    - **/usr/local/MovieTranslatorApp.app/**

## Run the App

To run the application you need to pass in 2 arguments. 
  - The path to your movie file
  - The path to your SRT file

**Do not add any - to your arguments,** it **should** look like this:
  - ```./MovieTranslatorApp path/to/video path/to/srtfile```

And **not** this:
  - ```./MovieTranslatorApp -path/to/video --path/to/srtfile```

You also need to assure that LibreTranslate is running on localhost:5000(**this can be overridden with an argument**) before running the executable.

### Additional Arguments

You can pass the following additional arguments into Movie Translator.

| Argument   | Description | Default  |
| :--------: | :-------------: | :--------: |
| ```-librehost <ip>```  | Overrides the IP HTTPLib Client defaults to        |    **localhost**      |
| ```-libreport <port>``` | Overrides the port HTTPLib Client defaults to           |   **5000**       |
| ```-hpTrue```    | Enables multi-threaded video decoding         |    **False**      |
| ```-printinfo``` | Prints the SRT and video file path you passed in           |   **False**       |
| ```-fontfile <path/to/.ttf>```    |  Overrides the font file Freetype uses          |  **arial.ttf**        |

### Environment Variables

You can also set the following environment variables if you place your texture location or shaders in a different spot from where it is expected. 

  - ```set MOVIE_APP_RESOURCES_PATH=<path/to/resources>```
  - ```set MOVIE_APP_SHADER_PATH=<path/to/shaders>```

For Linux and Mac, replace ```set``` with ```export```.

### **FINAL NOTES**

Depending on the language you are attempting to display, some font files will not support it. If you need to provide your own font file that works for your language read the additional arguments header and look for the ```fontfile``` argument. 

Please note that this project is not extensively tested on each machine, and error handling is quite minimal.

<br><br><br>

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