
#ifndef UTILITY_H
#define UTILITY_H

#include <string>
inline std::string GET_RESOURCE_FOLDER_PATH() {
// environment var override
const char* resource_env = std::getenv("MOVIE_APP_RESOURCES_PATH");
if (resource_env) {
    
    return std::string(resource_env);

}
// default to 
#ifdef __APPLE__
    return "/usr/local/MovieTranslatorApp.app/Contents/resources/"
#elif _WIN32
    return "C:/Program Files/MovieTranslatorApp/share/MovieTranslatorApp/resources/";
#else 
    return "/usr/local/share/MovieTranslatorApp/resources/";
#endif
}

inline std::string GET_SHADER_FOLDER_PATH() {
// environment var override
const char* shader_env = std::getenv("MOVIE_APP_SHADER_PATH");
if (shader_env) {

    return std::string(shader_env);

}
// default to
#ifdef __APPLE__
    return "/usr/local/MovieTranslatorApp.app/Contents/resources/"
#elif _WIN32
    return "C:/Program Files/MovieTranslatorApp/share/MovieTranslatorApp/shaders/";
#else 
    return "/usr/local/share/MovieTranslatorApp/shaders/";
#endif
}

#include <sstream>
#include <fstream>
#include <iomanip>

#include "include/variables.hpp"
#include "include/subtitleUtil.hpp"
#include "include/thread.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Utility {

    std::string format_time_hhmmss(const int64_t& total_seconds);
    std::string load_file_text_into_string(const std::string path);

    std::vector<uint32_t> get_codepoints_from_string(const std::string& str);
    uint32_t get_codepoint(unsigned char& byte1, unsigned char& byte2);
    uint32_t get_codepoint(unsigned char& byte1, unsigned char& byte2, unsigned char& byte3);
    uint32_t get_codepoint(unsigned char& byte1, unsigned char& byte2, unsigned char& byte3, unsigned char& byte4);

}


#endif