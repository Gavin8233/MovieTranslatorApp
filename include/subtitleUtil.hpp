
#ifndef SUBTITLEUTIL_H
#define SUBTITLEUTIL_H

#include <string>
#include <vector>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "include/preferences.hpp"

struct srtTimestamp {

    int64_t start;
    int64_t end;

};

struct Subtitle {

    srtTimestamp original_timestamp;
    srtTimestamp display_timestamp;
    std::string text, translated_text;
    std::vector<uint32_t> text_codepoints, translated_text_codepoints;

};

namespace subtitleUtil {

    // init http lib, set ip = nullptr if use default

    void init_httplib_client(const std::string& ip, const int& port);
    void destroy_httplib_client();

    /* sends http request to libretranslate and gets response */

    std::string translate_text(const std::string& text);

    /* goes through each line of text until it reaches an empty line and stores it all in one string */

    bool decode_subtitle_text(std::ifstream& file, std::string& subtitle_text, std::string& line);

    /* removes the <i> and </i> from srt files */

    void remove_html_tags(std::string& text);

    srtTimestamp get_srt_timestamp(const std::string& time_str);

    Subtitle make_subtitle(const srtTimestamp& timestamp, 
         const std::string& text, const std::string& translated_text, 
         const std::vector<uint32_t>& text_codepoints, const std::vector<uint32_t>& translated_text_codepoints); 
         

        
    extern std::vector<Language> languages;
    extern int language_choice_one_display_idx;

    int get_valid_display_index(const int index_advance);
    Language get_language_choice(const int& chosen);
    std::string get_language_choice_as_string(const int& language_choice);

}


#endif