
#include "include/utility.hpp"

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

}