
#include "include/utility/utility.hpp"

namespace Utility {

    std::string format_time_hhmmss(const int64_t& total_seconds) {

        int hours = static_cast<int>(total_seconds / 3600);
        int minutes = static_cast<int>((total_seconds % 3600) / 60);
        int seconds = static_cast<int>(total_seconds % 60);

        std::ostringstream stream;
    
        stream << std::setfill('0') << std::setw(2) << hours << ":"
            << std::setw(2) << minutes << ":"
            << std::setw(2) << seconds;

        return stream.str();

    }

    std::string load_file_text_into_string(const std::string path) {

        std::string data;
        std::ifstream file;
    
        try {
    
            file.open(path);
    
            std::stringstream data_stream;
    
            data_stream << file.rdbuf();
    
            file.close();
    
            data = data_stream.str();
    
        } 
        catch (std::ifstream::failure f) { throw std::runtime_error("FAILED TO LOAD DATA FROM FILE"); }
    
        return data;

    }

    std::vector<uint32_t> get_codepoints_from_string(const std::string& str) {

        std::size_t i = 0;

        std::vector<uint32_t> codepoints;
        uint32_t codepoint;
        unsigned char byte1, byte2, byte3, byte4;

        while (i < str.size()) {

            byte1 = str[i];

            if ((byte1 & 0x80) == 0) { // character is ascii

                codepoints.emplace_back(byte1);

                i++;

            }

            else if ((byte1 & 0xE0) == 0xC0) { // character is 2 bytes

                byte2 = str[i + 1];

                codepoint = get_codepoint(byte1, byte2);

                codepoints.emplace_back(codepoint);

                i+=2;

            }

            else if ((byte1 & 0xF0) == 0xE0) { // character is 3 bytes

                byte2 = str[i + 1];
                byte3 = str[i + 2];

                codepoint = get_codepoint(byte1, byte2, byte3);

                codepoints.emplace_back(codepoint);

                i+=3;

            }

            else if ((byte1 & 0xF8) == 0xF0) { // character is 4 bytes

                byte2 = str[i + 1];
                byte3 = str[i + 2];
                byte4 = str[i + 3];

                codepoint = get_codepoint(byte1, byte2, byte3, byte4);

                codepoints.emplace_back(codepoint);

                i+=4;

            }

            else {

                std::cerr << "\nFailed to grab codepoints from a text string."
                "\nThis shouldn't happen, verify that the SRT file you gave has valid UTF-8 text." << std::endl;
                throw std::runtime_error("SOMETHING WENT WRONG WHEN GETTING CODEPOINTS FROM STRING");

            }

        }

        return codepoints;

    }

    uint32_t get_codepoint(unsigned char& byte1, unsigned char& byte2) {

        return (( byte1 & 0x1F) << 6) |
               (( byte2 & 0x3F));

    }

    uint32_t get_codepoint(unsigned char& byte1, unsigned char& byte2, unsigned char& byte3) {

        return (( byte1 & 0x0F) << 12) |
               (( byte2 & 0x3F) << 6) |
               (( byte3 & 0x3F));

    }

    uint32_t get_codepoint(unsigned char& byte1, unsigned char& byte2, unsigned char& byte3, unsigned char& byte4) {

        return (( byte1 & 0x07) << 18) |
               (( byte2 & 0x3F) << 12) |
               (( byte3 & 0x3F) << 6) |
               (( byte4 & 0x3F));

    }


    const std::string GET_LANGUAGE_CHARACTER_CODE(const Language& language) {

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

    const std::string GET_COLOR_ENUM_AS_STRING(const Color& color) {

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
    
    const glm::vec3 GET_FONT_COLOR(const Color& color) {

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

    const glm::vec2 GET_SUBTITLE_LOCATION(const subtitleLocation& location, const float& screen_width, const float& screen_height) {

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

    const std::string GET_SUBTITLE_LOCATION_ENUM_AS_STRING(const subtitleLocation& location) {

        switch (location) {

        case subtitleLocation::TOP_LEFT: return "UPLEFT";
        case subtitleLocation::TOP_CENTER: return "TOPCNTR";
        case subtitleLocation::BOTTOM_CENTER: return "DOWNCNTR";
        case subtitleLocation::BOTTOM_LEFT: return "DOWNLEFT";

        }

        return "TOP_LEFT";

    }

}