
#include "include/user/preferences.hpp"

template<typename DISPLAY>
static void alter_display(int& index, DISPLAY& display, std::vector<DISPLAY>& displays, const bool& add, const unsigned int& max) {

    if (add) {

        index++;

        if (index >= max) {
                
            index = 0;

        }

    } else {

        index--;

        if (index < 0) {

            index = max - 1;

        }

    }

    display = displays.at(index);

}

template<typename T>
static void alter_value(T& val, const T& adjust, const T& min, const T& max, const bool& add) {

    if (add) {

        val += adjust;

        if (val > max) {

            val = max;

        }

    } else {

        val -= adjust;

        if (val < min) {

            val = min;

        }

    }

}

namespace userPreferences {

    userVideoPreferences user_video_preferences;
    
    std::vector<Color> colors { Color::RED, Color::GREEN, Color::BLUE, Color::BLACK, Color::WHITE, Color::CYAN, Color::YELLOW, Color::PURPLE };
    std::vector<subtitleLocation> locations { subtitleLocation::TOP_LEFT, subtitleLocation::TOP_CENTER, subtitleLocation::BOTTOM_CENTER, subtitleLocation::BOTTOM_LEFT };

    int current_color = 0;
    int current_sub_location = 0;

    void init_default_user_preferences() {

        user_video_preferences.font_color = colors.at(current_color);
        user_video_preferences.subtitle_location = locations.at(current_sub_location);
        user_video_preferences.subtitle_delay_offset_ms = 0;
        user_video_preferences.subtitle_scale = 0.25f;
        user_video_preferences.translate_to = "en";

    }


    void alter_translate_to(const std::string translate_to) {

        user_video_preferences.translate_to = translate_to;

    }

    void alter_font_color(const bool add) {

        unsigned int max = static_cast<unsigned int>(colors.size());

        alter_display(current_color, user_video_preferences.font_color, colors, add, max);

    }

    void alter_subtitle_location(const bool add) {

        unsigned int max = static_cast<unsigned int>(locations.size());

        alter_display(current_sub_location, user_video_preferences.subtitle_location, locations, add, max);

    }

    void alter_subtitle_scale(const bool add) {

        constexpr float min = 0.05f;
        constexpr float max = 1.0f;

        constexpr float adjust = 0.05f;
        
        alter_value(user_video_preferences.subtitle_scale, adjust, min, max, add);

    }

    void alter_subtitle_delay(const bool add) {

        constexpr int min = -15'000;
        constexpr int max =  15'000;

        constexpr int adjust = 100;

        alter_value(user_video_preferences.subtitle_delay_offset_ms, adjust, min, max, add);

    }

}