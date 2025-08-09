
#include "include/preferences.hpp"

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

    void alter_font_color(const bool add) {

        if (add) {

            current_color++;

            if (current_color >= colors.size()) current_color = 0;

            user_video_preferences.font_color = colors.at(current_color);

        }

        else {

            current_color--;

            if (current_color < 0) current_color = static_cast<int>(colors.size() - 1);

            user_video_preferences.font_color = colors.at(current_color);

        }

    }

    void alter_subtitle_location(const bool add) {

        if (add) {

            current_sub_location++;

            if (current_sub_location >= locations.size()) current_sub_location = 0;

            user_video_preferences.subtitle_location = locations.at(current_sub_location);

        }

        else {

            current_sub_location--;

            if (current_sub_location < 0) current_sub_location = static_cast<int>(locations.size() - 1);

            user_video_preferences.subtitle_location = locations.at(current_sub_location);

        }

    }

    void alter_subtitle_scale(const bool add) {

        if (add) {

            user_video_preferences.subtitle_scale += 0.05f;

            if (user_video_preferences.subtitle_scale >= 1.0f) user_video_preferences.subtitle_scale = 1.0f;

        }

        else {

            user_video_preferences.subtitle_scale -= 0.05f;

            if (user_video_preferences.subtitle_scale <= 0.05f) user_video_preferences.subtitle_scale = 0.05f;

        }

    }

    void alter_subtitle_delay(const bool add) {

        if (add) {

            user_video_preferences.subtitle_delay_offset_ms += 100;

        }

        else {

            user_video_preferences.subtitle_delay_offset_ms -= 100;

            if (user_video_preferences.subtitle_delay_offset_ms <= -5000) user_video_preferences.subtitle_delay_offset_ms = -5000;

        }

    }

    void alter_translate_to(const std::string translate_to) {

        user_video_preferences.translate_to = translate_to;

    }

}