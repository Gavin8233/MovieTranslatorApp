
#ifndef VARIABLES_H
#define VARIABLES_H

#include <condition_variable>

namespace CONSTANTS {

    namespace AUDIO {

        constexpr int SAMPLE_RATE = 44100; // forced to this sample rate for easier audio handling
        constexpr int BUFFER_QUEUE_SIZE = 3; // for openAL buffer queue 
        constexpr int BYTES_NEEDED_PER_AUDIO_PACKET = 200 * sizeof(int16_t); // size of pcm data before it is sent to be played

    }

    namespace TEX_ATLAS {

        // Should be a large enough texture to load all characters of any language
        constexpr int TEXTURE_ATLAS_WIDTH = 1024;
        constexpr int TEXTURE_ATLAS_HEIGHT = 1024;

    }

    namespace TEXTURES {

        /* to map the textures to the GL texture units */

        constexpr int VIDEO_TEXTURE = 0;
        constexpr int FONT_TEXTURE = 1;
        constexpr int PROGRESS_BAR_TEXTURE = 2;
        constexpr int SETTINGS_WHEEL_TEXTURE = 3;
        constexpr int SETTINGS_MENU_TEXTURE = 4;
        constexpr int LANGUAGE_SELECT_TEXTURE = 5;

    }

    namespace TRANSLATION {

        constexpr float NO_TRANSLATION = 0.0f;
        constexpr float NO_SCALE = 1.0f;

        constexpr float TINY_TEXT_SCALE_DOWN = 0.2f; 
        constexpr float LANGUAGE_CHOICE_TEXT_SCALE_DOWN = 0.3f; // Exclusively for the language choice menu

        constexpr float DISPLAY_TIME_Y_TRANSLATION = 80.0f; // When hovering over the progress bar this value is used to display the timestamp above the bar

        constexpr float LOADING_BAR_Y_OFFSET = -0.885f; // to bring the loading bar to the bottom of the screen
        constexpr float LOADING_BAR_Y_SCALE = 0.065f; // to make the rectangle skinny(scale y down)

        constexpr float FILL_SHADER_Y_OFFSET = LOADING_BAR_Y_OFFSET + 0.01f; // to align the fill shader box with the loading bar box(+0.01f so it is perfect)
        constexpr float FILL_SHADER_Y_SCALE = 0.015f; // to make the fill bar skinny(has to be skinnier than the loading bar or it would go outside the box a little)

        constexpr float SETTINGS_WHEEL_X_OFFSET = 0.945f; // to push settings wheel to almost edge of screen
        constexpr float SETTINGS_WHEEL_Y_OFFSET = FILL_SHADER_Y_OFFSET; // to align settings wheel with filled bar(is more centered than aligning with loading bar)
        constexpr float SETTINGS_WHEEL_X_SCALE = 0.1f; // to make settings wheel small
        constexpr float SETTINGS_WHEEL_Y_SCALE = 0.1f; // same as above

        constexpr float LANGUAGE_MENU_X_OFFSET = 0.75f; // bring to the right side of screen
        constexpr float LANGUAGE_MENU_Y_OFFSET = 0.1f; // bring to just above the settings menu
        constexpr float LANGUAGE_MENU_X_SCALE_DOWN = 0.5f; 
        constexpr float LANGUAGE_MENU_Y_SCALE_DOWN = LANGUAGE_MENU_X_SCALE_DOWN;

        constexpr float SETTINGS_MENU_X_OFFSET = 0.75f; // bring to the right side of screen
        constexpr float SETTINGS_MENU_Y_OFFSET = -0.42f; // bring to almost bottom 
        constexpr float SETTINGS_MENU_X_SCALE_DOWN = 0.8f;
        constexpr float SETTINGS_MENU_Y_SCALE_DOWN = 0.6f;

    }

}

namespace GLOBAL_STATES {

    extern bool VIDEO_PAUSED;
    extern bool VIDEO_SEEK_REQUESTED;

    extern std::condition_variable VIDEO_PAUSED_CONDITION;
    extern std::mutex VIDEO_PAUSED_MUTEX;

}

#endif