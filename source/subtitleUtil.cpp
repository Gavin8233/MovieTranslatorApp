
#include "include/subtitleUtil.hpp"

namespace subtitleUtil {

    httplib::Client* httplib_client = nullptr;

    void init_httplib_client(const std::string& ip, const int& port) {

        httplib_client = new httplib::Client(ip.c_str(), port); 

    }

    void destroy_httplib_client() {

        httplib_client->stop();
        delete httplib_client;

    }

    std::string translate_text(const std::string& text) {

        nlohmann::json data {

            { "q", text },
            { "source", "auto" },
            { "target", userPreferences::user_video_preferences.translate_to },
            { "format", "text" },
            { "alternatives", 0 },
            { "api_key", "" }

        };

        auto res = httplib_client->Post("/translate", data.dump(), "application/json");

        if (!res || res->body.empty() || res->status != 200/*OK*/) {

            std::cerr << "\nERROR: Translation request was denied or failed, ensure that LibreTranslate is currently running and accessible on <your ip>:5000."
            "\nIf you need to verify that LibreTranslate is open, try doing curl -v http://<your ip>:5000/translate and see if the connection is open."
            "\nIf it is not, go to the LibreTranslate website and see if you can troubleshoot."
            "\nIf it is open and your requests still are not being sent, try restarting LibreTranslate and starting the MovieTranslatorApp again."
            "\nIf the error persists and you are NOT on Windows, it is possible the SRT file you provided is encoded using windows line endings like ^M. Verify that it is not."
            "\nIf it is, then run '<your package manager> install dos2unix. Then run 'dos2unix <your file name>.srt' and try to run the program again."
            "\nIf all else fails, then try a full reinstall of the program and try running again. Although, it should most likely not be a problem with the code or httplib, "
            "and more likely to be that something is blocking the requests sent to LibreTranslate." << std::endl;
            throw std::runtime_error("Failed to get Translation from http://<your ip>:5000/translate");

        }

        nlohmann::json parsed_return_data;

        try {

            parsed_return_data = nlohmann::json::parse(res->body);

        } catch (std::exception& e) {

            std::cerr << "Failed to parse translated return data from LibreTranslate" << std::endl;
            return "FAILED TRANSLATION";

        }

        std::string translated_text; // str;
        std::size_t longest_text_length = 0; // str_len;

        /*for (const auto& alternative : parsed_return_data["alternatives"]) { 

            for alternate translations just change the json data "alternatives" field to have n alternatives(see above json) and uncomment this code and str, then define 
            str_len = 0 and it will grab the largest one (in case the translations are too simplified)

            str = alternative.get<std::string>();
            str_len = str.length();

            if (str_len > longest_text_length) {

                translated_text = str;
                longest_text_length = str.length();

            }

        }*/  

        if (longest_text_length == 0) translated_text = parsed_return_data["translatedText"];

        return translated_text;

    }

    bool decode_subtitle_text(std::ifstream& file, std::string& subtitle_text, std::string& line) {

        while (std::getline(file, line)) {

            if (line.empty()) {

                remove_html_tags(subtitle_text);

                return true;

            }

            subtitle_text += " " + line;

        }

        remove_html_tags(subtitle_text);

        return false;

    }

    void remove_html_tags(std::string& text) {  

	    std::size_t pos = 0;

	    while ((pos = text.find("<i>")) != std::string::npos) {

		    text.erase(pos, 3);

	    }

	    while ((pos = text.find("</i>")) != std::string::npos) {

		    text.erase(pos, 4);

	    }

    }

    srtTimestamp get_srt_timestamp(const std::string& time_str) {

        int hours, minutes, seconds, milliseconds;
        char sep1, sep2, comma;

        std::istringstream ss(time_str);
        ss >> hours >> sep1 >> minutes >> sep2 >> seconds >> comma >> milliseconds;

        if (ss.fail() || sep1 != ':' || sep2 != ':' || comma != ',') {

	        return { -1, -1 };

        }

        int64_t start = (hours * 3600000 + minutes * 60000 + seconds * 1000 + milliseconds) + userPreferences::user_video_preferences.subtitle_delay_offset_ms;

        std::string seperation_arrow;

        ss >> seperation_arrow >> hours >> sep1 >> minutes >> sep2 >> seconds >> comma >> milliseconds;

        if (ss.fail() || sep1 != ':' || sep2 != ':' || comma != ',') {

	        return { -1, -1 };

        }

        int64_t end = (hours * 3600000 + minutes * 60000 + seconds * 1000 + milliseconds) + userPreferences::user_video_preferences.subtitle_delay_offset_ms;

        srtTimestamp timestamp;

        timestamp.start = start;
        timestamp.end = end;

        return timestamp;

    }

    Subtitle make_subtitle(const srtTimestamp& timestamp, 
        const std::string& text, const std::string& translated_text, 
        const std::vector<uint32_t>& text_codepoints, const std::vector<uint32_t>& translated_text_codepoints) {

            
        Subtitle sub;

        sub.original_timestamp = timestamp;
        sub.display_timestamp = timestamp;
        sub.text = text;
        sub.translated_text = translated_text;
        sub.text_codepoints = text_codepoints;
        sub.translated_text_codepoints = translated_text_codepoints;

        return sub;

    }

    
    std::vector<Language> languages { 
        Language::GREEK, Language::ENGLISH, Language::SPANISH, Language::ARABIC, Language::CHINESE, Language::FRENCH, Language::GERMAN, Language::ITALIAN, Language::JAPANESE, Language::KOREAN, 
        Language::MALAY, Language::POLISH, Language::PORTUGUESE, Language::RUSSIAN, Language::THAI, Language::URDU
    };
    int language_choice_one_display_idx = 0;

    int get_valid_display_index(const int index_advance) {

        int index = language_choice_one_display_idx + index_advance;

        if (index >= languages.size()) {

            index = static_cast<int>(index - languages.size());

            return index;

        }

        return index;

    }

    Language get_language_choice(const int& chosen) {

        switch(chosen) {

            case 1:

                return languages.at(get_valid_display_index(0));

            case 2: 

                return languages.at(get_valid_display_index(1));

            case 3:

                return languages.at(get_valid_display_index(2));

            case 4:

                return languages.at(get_valid_display_index(3));

            default: 
                // this should never happen
                std::cerr << "Something went wrong" << std::endl;
                return languages.at(get_valid_display_index(0));

        }    

        return Language::ENGLISH;

    }

    std::string get_language_choice_as_string(const int& language_choice) {

        switch(language_choice) {

            case 1:

                return GET_LANGUAGE_ENUM_AS_STRING(languages.at(get_valid_display_index(0)));

            case 2: 

                return GET_LANGUAGE_ENUM_AS_STRING(languages.at(get_valid_display_index(1)));

            case 3:

                return GET_LANGUAGE_ENUM_AS_STRING(languages.at(get_valid_display_index(2)));

            case 4:

                return GET_LANGUAGE_ENUM_AS_STRING(languages.at(get_valid_display_index(3)));

            default: 
                // this should never happen
                std::cerr << "Something went wrong" << std::endl;
                return GET_LANGUAGE_ENUM_AS_STRING(languages.at(get_valid_display_index(0)));

        }    

        return "LOADING ERROR";

    }

}