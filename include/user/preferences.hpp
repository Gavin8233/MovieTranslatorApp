
#ifndef ENUMDEFINITIONS_H
#define ENUMDEFINITIONS_H

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <string>
#include <vector>

enum class Color {
    
    RED,
    GREEN,
    BLUE,
    BLACK,
    WHITE,
    CYAN,
    YELLOW,
    PURPLE

};

enum class subtitleLocation {

    TOP_LEFT,
    TOP_CENTER,
    BOTTOM_LEFT,
    BOTTOM_CENTER

};

enum class Language {

    ENGLISH,
    GREEK,
    SPANISH,
    ARABIC,
    CHINESE,
    FRENCH,
    GERMAN,
    ITALIAN,
    JAPANESE,
    KOREAN,
    MALAY,
    POLISH,
    PORTUGUESE,
    RUSSIAN,
    THAI,
    URDU

};

struct userVideoPreferences {

    Color font_color;
    subtitleLocation subtitle_location;
    float subtitle_scale;
    int subtitle_delay_offset_ms;
    std::string translate_to;

};

namespace userPreferences {

    extern userVideoPreferences user_video_preferences;

    void init_default_user_preferences();
    void alter_font_color(const bool add);
    void alter_subtitle_scale(const bool add);
    void alter_subtitle_delay(const bool add);
    void alter_translate_to(const std::string translate_to);
    void alter_subtitle_location(const bool add);

}

#endif