
#ifndef DISPLAY_H
#define DISPLAY_H

// holds all language choice options when the language select screen is open
struct languageChoiceDisplayText {

    std::vector<uint32_t> choice_one;
    buttonLocation choice_one_text_location;

    std::vector<uint32_t> choice_two;
    buttonLocation choice_two_text_location;

    std::vector<uint32_t> choice_three;
    buttonLocation choice_three_text_location;

    std::vector<uint32_t> choice_four;
    buttonLocation choice_four_text_location;

};

// holds all display text that is needed to be drawn when the settings menu is open
struct settingsMenuDisplayText {

    std::vector<uint32_t> color;
    buttonLocation color_text_location;

    std::vector<uint32_t> subtitle_location;
    buttonLocation subtitle_location_text_location;

    std::vector<uint32_t> subtitle_delay;
    buttonLocation subtitle_delay_text_location;

    std::vector<uint32_t> subtitle_scale;
    buttonLocation subtitle_scale_text_location;

    std::vector<uint32_t> translate_to;
    buttonLocation translate_to_text_location;

};

// holds all current data that is important for rendering and proper display of video
struct currentData {

    std::vector<uint32_t> display_time_codepoints;

    int64_t last_progress_bar_update_time_ms;
    int64_t last_mouse_move_time_ms;

    float screen_width;
    float screen_height;

    double mouse_xpos;
    double mouse_ypos;

    videoData video_data;
    std::vector<int16_t> audio_data;

    Subtitle subtitle;
    glm::vec2 subtitle_location;

};

struct Display {

    currentData current_data;
    languageChoiceDisplayText language_choice_display_text;
    settingsMenuDisplayText settings_menu_display_text;

};
#endif