
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

inline constexpr glm::vec3 GET_FONT_COLOR(const Color& color) {

    switch (color) {

        case Color::RED:        return { 1.0f, 0.0f, 0.0f };
        case Color::GREEN:      return { 0.0f, 1.0f, 0.0f };
        case Color::BLUE:       return { 0.0f, 0.0f, 1.0f };
        case Color::BLACK:      return { 0.0f, 0.0f, 0.0f };
        case Color::WHITE:      return { 1.0f, 1.0f, 1.0f };
        case Color::CYAN:       return { 0.0f, 1.0f, 1.0f };
        case Color::YELLOW:     return { 1.0f, 1.0f, 0.0f };
        case Color::PURPLE:     return { 1.0f, 0.0f, 1.0f };

    }

    return { 1.0f, 1.0f, 1.0f }; // default

}

inline constexpr glm::vec2 GET_SUBTITLE_LOCATION(const subtitleLocation& location, const float& screen_width, const float& screen_height) {

    constexpr float top_left_ndcX = -0.95f; 
    constexpr float top_left_ndcY = 0.9f;

    constexpr float top_center_ndcX = -0.4f;
    constexpr float top_center_ndcY = top_left_ndcY;

    constexpr float bottom_left_ndcX = top_left_ndcX;
    constexpr float bottom_left_ndcY = -0.8f;

    constexpr float bottom_center_ndcX = top_center_ndcX;
    constexpr float bottom_center_ndcY = bottom_left_ndcY;

    switch (location) {

        case subtitleLocation::TOP_LEFT:        return { (top_left_ndcX + 1.0f) * 0.5f * screen_width, (top_left_ndcY + 1.0f) * 0.5f * screen_height };
        case subtitleLocation::TOP_CENTER:      return { (top_center_ndcX + 1.0f) * 0.5f * screen_width, (top_center_ndcY + 1.0f) * 0.5f * screen_height };
        case subtitleLocation::BOTTOM_LEFT:     return { (bottom_left_ndcX + 1.0f) * 0.5f * screen_width, (bottom_left_ndcY + 1.0f) * 0.5f * screen_height };
        case subtitleLocation::BOTTOM_CENTER:   return { (bottom_center_ndcX + 1.0f) * 0.5f * screen_width, (bottom_center_ndcY + 1.0f) * 0.5f * screen_height };

    }

    return { (top_left_ndcX + 1.0f) * 0.5f * screen_width, (top_left_ndcY + 1.0f) * 0.5f * screen_height }; // default

}

inline const std::string GET_LANGUAGE_CHARACTER_CODE(const Language& language) {

    /* 
    
        https://libretranslate.com/languages
    
    */

    switch (language) {

        case Language::GREEK: return "el";
        case Language::ENGLISH: return "en";
        case Language::SPANISH: return "es";
        case Language::ARABIC: return "ar";
        case Language::CHINESE: return "zh-Hans";
        case Language::FRENCH: return "fr";
        case Language::GERMAN: return "de";
        case Language::ITALIAN: return "it";
        case Language::JAPANESE: return "ja";
        case Language::KOREAN: return "ko";
        case Language::MALAY: return "ms";
        case Language::POLISH: return "pl";
        case Language::PORTUGUESE: return "pt";
        case Language::RUSSIAN: return "ru";
        case Language::THAI: return "th";
        case Language::URDU: return "ur";

    }

    return "en";

}

inline const std::string GET_COLOR_ENUM_AS_STRING(const Color& color) {

    switch (color) {

        case Color::RED:        return "RED";
        case Color::GREEN:      return "GREEN";
        case Color::BLUE:       return "BLUE";
        case Color::BLACK:      return "BLACK";
        case Color::WHITE:      return "WHITE";
        case Color::CYAN:       return "CYAN";
        case Color::YELLOW:     return "YELLOW";
        case Color::PURPLE:     return "PURPLE";

    }

    return "RED";

}

inline const std::string GET_SUBTITLE_LOCATION_ENUM_AS_STRING(const subtitleLocation& location) {

    switch (location) {

        case subtitleLocation::TOP_LEFT: return "UPLEFT";
        case subtitleLocation::TOP_CENTER: return "TOPCNTR";
        case subtitleLocation::BOTTOM_CENTER: return "DOWNCNTR";
        case subtitleLocation::BOTTOM_LEFT: return "DOWNLEFT";

    }

    return "TOP_LEFT";

}

inline const std::string GET_LANGUAGE_ENUM_AS_STRING(const Language& language) {

    switch (language) {

        case Language::GREEK: return "GREEK";
        case Language::ENGLISH: return "ENGLISH";
        case Language::SPANISH: return "SPANISH";
        case Language::ARABIC: return "ARABIC";
        case Language::CHINESE: return "CHINESE";
        case Language::FRENCH: return "FRENCH";
        case Language::GERMAN: return "GERMAN";
        case Language::ITALIAN: return "ITALIAN";
        case Language::JAPANESE: return "JAPANESE";
        case Language::KOREAN: return "KOREAN";
        case Language::MALAY: return "MALAY";
        case Language::POLISH: return "POLISH";
        case Language::PORTUGUESE: return "PORTUGUESE";
        case Language::RUSSIAN: return "RUSSIAN";
        case Language::THAI: return "THAI";
        case Language::URDU: return "URDU";

    }   

    return "ENGLISH";

}

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