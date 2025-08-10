
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "include/renderer.hpp"
#include "include/video_decoder.hpp"
#include "include/subtitle_decoder.hpp"
#include "include/audio_player.hpp"
#include "include/button.hpp"

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

class videoPlayer {

    public:

        videoPlayer(const std::string& video_file_path, const std::string& subtitle_srt_path, const std::string& font_file_path, const int& threads);
        ~videoPlayer();

        void start();

    private:

        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void scroll_callback(GLFWwindow* window, double xoff, double yoff);
        void custom_cursor_pos_callback(const float& xpos, const float& ypos);

        static void perform_button_action(const buttonName& name, const double& mouse_x, const double& mouse_y);
        static bool is_within_box(const double& mouse_x, const double& mouse_y, const buttonLocation& location);
        static unsigned int decide_language_altered(const double& mouse_x, const double& mouse_y);
        
        void process_input(GLFWwindow* window);

        void if_paused();

        void sync_audio();
        void sync_video();

        subtitleDecoder* subtitle_decoder;
        Renderer* renderer;
        audioPlayer* audio_player;
        videoDecoder* video_decoder;

        uint64_t video_duration;
        int64_t elapsed;

        currentData current_data;

        languageChoiceDisplayText language_choice_display_text;
        void set_new_language_display_text();
        settingsMenuDisplayText settings_menu_display_text;
        void set_new_menu_display_text();

        static bool SETTINGS_OPEN;
        static bool LANGUAGE_ALTER_MENU_OPEN;
        static bool SUBTITLE_DELAY_ALTERED;
        static bool SUBTITLE_LOCATION_ALTERED;
        static bool LANGUAGE_CHOICES_ALTERED;
        static bool SETTINGS_MENU_ALTERED;
        static bool FULLSCREEN_ENABLED;

        // if language was altered this variable is set to the option the user chose(1, 2, 3 or 4)
        static unsigned int LANGUAGE_ALTERED;

        bool render_overlay;
        bool render_timestamp;
        bool render_language_select_screen;

        /* if space was clicked since the glfw get key function returns true many times this variable is set true to avoid that and set back to false when glfw does not get space bar click */

        bool space_was_clicked;
        static bool fullscreen_clicked;

        static int last_windowed_mode_x;
        static int last_windowed_mode_y;
        static int last_windowed_mode_width;
        static int last_windowed_mode_height;

};


#endif
