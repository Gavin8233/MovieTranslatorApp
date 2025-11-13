
#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include "include/GL/renderer.hpp"
#include "include/video_player/video_decoder.hpp"
#include "include/subtitles/subtitle_decoder.hpp"
#include "include/audio/audio_player.hpp"
#include "include/GL/button.hpp"
#include "include/video_player/display.hpp"

#include <functional>

enum struct Request {

    ALTER_SUBTITLE_DELAY,
    ALTER_SUBTITLE_LOCATION,
    ALTER_LANGUAGE_CHOICES,
    ALTER_SETTING,
    SET_FULLSCREEN,
    NONE

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

        void sync_audio();
        void sync_video();

        subtitleDecoder* subtitle_decoder;
        Renderer* renderer;
        audioPlayer* audio_player;
        videoDecoder* video_decoder;

        static std::vector<Request> request_queue;
        static Display display;

        uint64_t video_duration;
        uint64_t elapsed;

        static bool SETTINGS_OPEN;
        static bool LANGUAGE_ALTER_MENU_OPEN;
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
