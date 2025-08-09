
#include "include/video_player.hpp"

bool videoPlayer::SETTINGS_OPEN = false;
bool videoPlayer::LANGUAGE_ALTER_MENU_OPEN = false;
bool videoPlayer::SUBTITLE_DELAY_ALTERED = false;
bool videoPlayer::LANGUAGE_CHOICES_ALTERED = false;
bool videoPlayer::SETTINGS_MENU_ALTERED = false;
bool videoPlayer::SUBTITLE_LOCATION_ALTERED = false;
bool videoPlayer::FULLSCREEN_ENABLED = false;
bool videoPlayer::fullscreen_clicked = false;

unsigned int videoPlayer::LANGUAGE_ALTERED = 0;
int videoPlayer::last_windowed_mode_x = 0;
int videoPlayer::last_windowed_mode_y = 0;
int videoPlayer::last_windowed_mode_width = 0;
int videoPlayer::last_windowed_mode_height = 0;

videoPlayer::~videoPlayer() {

    std::cout << "\nDestroying subtitle decoder thread.";
    delete subtitle_decoder;
    delete renderer;
    std::cout << "\nDestroying audio player thread.";
    delete audio_player;
    std::cout << "\nDestroying video decoder thread.";
    delete video_decoder;
    std::cout << "\nDestroying GL resources.";
    GLutil::destroy_gl_context();
    std::cout << "\nComplete.";

}

videoPlayer::videoPlayer(const std::string& video_file_path, const std::string& subtitle_srt_path, const std::string& font_file_path, const bool& high_performance) :

subtitle_decoder { nullptr },
renderer { nullptr },
audio_player { nullptr },
video_decoder { nullptr },

video_duration { 0 },
elapsed { 0 },

render_overlay { false },
render_timestamp { false },
render_language_select_screen { false },

space_was_clicked { false }

{

    renderer = new Renderer(font_file_path);
    renderer->init_shaders();

    video_decoder = new videoDecoder(video_file_path, high_performance);
    video_duration = video_decoder->get_video_length();

    subtitle_decoder = new subtitleDecoder(subtitle_srt_path);
    audio_player = new audioPlayer;

    subtitle_decoder->start_decoder_thread();

    while (!subtitle_decoder->has_subtitle()) { }
    current_data.subtitle = subtitle_decoder->get_next_subtitle();

    video_decoder->start_decoder_threads();

    while (!video_decoder->has_video_data()) { }
    current_data.video_data = video_decoder->get_next_video_data();

    audio_player->startup_thread();

}

void videoPlayer::if_paused() {

    /* 
    
        I don't know why or how but this code is holding together the entire pausing system
    
    */

    process_input(GLutil::window);

    glfwSwapBuffers(GLutil::window);

}

buttonLocation get_menu_text_location(const buttonName& start, const buttonName& end) {

    buttonLocation text_location_start = Buttons::get_button_location(start);
    buttonLocation text_location_end = Buttons::get_button_location(end);

    buttonLocation box;
    box.topleftX = text_location_start.topleftX;
    box.bottomrightX = text_location_end.topleftX;
    box.topleftY = text_location_start.topleftY;
    box.bottomrightY = text_location_end.bottomrightY;

    return box;

}

void videoPlayer::set_new_language_display_text() {

    std::string choice_one = subtitleUtil::get_language_choice_as_string(1);
    std::string choice_two = subtitleUtil::get_language_choice_as_string(2);
    std::string choice_three = subtitleUtil::get_language_choice_as_string(3);
    std::string choice_four = subtitleUtil::get_language_choice_as_string(4);

    language_choice_display_text.choice_one = Utility::get_codepoints_from_string(choice_one);
    language_choice_display_text.choice_two = Utility::get_codepoints_from_string(choice_two);
    language_choice_display_text.choice_three = Utility::get_codepoints_from_string(choice_three);
    language_choice_display_text.choice_four = Utility::get_codepoints_from_string(choice_four);

    language_choice_display_text.choice_one_text_location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_ONE);
    language_choice_display_text.choice_two_text_location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_TWO);
    language_choice_display_text.choice_three_text_location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_THREE);
    language_choice_display_text.choice_four_text_location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_FOUR);

}

void videoPlayer::set_new_menu_display_text() {

    std::string current_color = GET_COLOR_ENUM_AS_STRING(userPreferences::user_video_preferences.font_color);
    std::string current_subtitle_location = GET_SUBTITLE_LOCATION_ENUM_AS_STRING(userPreferences::user_video_preferences.subtitle_location);
    std::string current_sub_delay = std::to_string(userPreferences::user_video_preferences.subtitle_delay_offset_ms);
    std::string current_sub_scale = std::to_string(userPreferences::user_video_preferences.subtitle_scale);
    
    settings_menu_display_text.color = Utility::get_codepoints_from_string(current_color);
    settings_menu_display_text.subtitle_location = Utility::get_codepoints_from_string(current_subtitle_location);
    settings_menu_display_text.subtitle_delay = Utility::get_codepoints_from_string(current_sub_delay);
    settings_menu_display_text.subtitle_scale = Utility::get_codepoints_from_string(current_sub_scale);
    settings_menu_display_text.translate_to = Utility::get_codepoints_from_string(userPreferences::user_video_preferences.translate_to);

    settings_menu_display_text.color_text_location = get_menu_text_location(buttonName::FONT_COLOR_BACK, buttonName::FONT_COLOR_FORWARD);
    settings_menu_display_text.subtitle_location_text_location = get_menu_text_location(buttonName::SUBTITLE_LOCATION_BACK, buttonName::SUBTITLE_LOCATION_FORWARD);
    settings_menu_display_text.subtitle_delay_text_location = get_menu_text_location(buttonName::SUBTITLE_DELAY_MINUS, buttonName::SUBTITLE_DELAY_PLUS);
    settings_menu_display_text.subtitle_scale_text_location = get_menu_text_location(buttonName::SUBTITLE_SCALE_MINUS, buttonName::SUBTITLE_SCALE_PLUS);
    settings_menu_display_text.translate_to_text_location = Buttons::get_button_location(buttonName::LANGUAGE_SELECT_BOX);

}

void videoPlayer::start() {

    glfwSetFramebufferSizeCallback(GLutil::window, &videoPlayer::framebuffer_size_callback); 
    glfwSetMouseButtonCallback(GLutil::window, &videoPlayer::mouse_button_callback);
    glfwSetScrollCallback(GLutil::window, &videoPlayer::scroll_callback);

    GLutil::init_video_texture(current_data.video_data.width, current_data.video_data.height);

    constexpr int draw_overlay_time_ms = 1250;
    constexpr int update_progress_delay_ms = 5000;
    
    double xpos, ypos;
    int curr_screen_width, curr_screen_height;
    float mouse_x_normalized, mouse_y_normalized;

    set_new_language_display_text();
    set_new_menu_display_text();

    glfwGetWindowSize(GLutil::window, &curr_screen_width, &curr_screen_height);

    current_data.subtitle_location = GET_SUBTITLE_LOCATION(userPreferences::user_video_preferences.subtitle_location, float(curr_screen_width), float(curr_screen_height));

    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(GLutil::window)) {

        glfwPollEvents();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        renderer->draw_movie();

        if (GLOBAL_STATES::VIDEO_SEEK_REQUESTED) {

            // figure out where the mouse click is at to seek to that position

            buttonLocation progress_bar_position = Buttons::get_button_location(buttonName::SEEK_VIDEO);

            float hover_progress = (mouse_x_normalized - progress_bar_position.topleftX) / (progress_bar_position.bottomrightX - progress_bar_position.topleftX);
            int64_t display_time_ms = static_cast<int64_t>(hover_progress * video_duration);

            int64_t seconds = display_time_ms / 1000;

            video_decoder->seek_video(seconds);
            subtitle_decoder->seek_subtitle_location(display_time_ms);
            audio_player->clear_queue_and_restart();

            current_data.subtitle.display_timestamp.end = 0; // in case subtitle had not displayed yet set the end timestamp = 0 so a new one is displayed(the updated one)
            current_data.last_progress_bar_update_time_ms = 0; // for same basic reason

        }

        if (LANGUAGE_CHOICES_ALTERED) { // user scrolled through languages

            set_new_language_display_text();

            LANGUAGE_CHOICES_ALTERED = false;

        }

        if (SETTINGS_MENU_ALTERED) { // user changed a setting that requires text to be updated

            set_new_menu_display_text();

            SETTINGS_MENU_ALTERED = false;

        }

        if (LANGUAGE_ALTERED != 0) {

            userPreferences::alter_translate_to(GET_LANGUAGE_CHARACTER_CODE(subtitleUtil::get_language_choice(LANGUAGE_ALTERED)));

            LANGUAGE_ALTERED = 0;
            subtitle_decoder->seek_subtitle_location(elapsed); // to reset subtitles and re translate
            
            while (!subtitle_decoder->has_subtitle()) {} // we do this so that subtitles are displayed again immediately after switching and are correct to the switch
            current_data.subtitle = subtitle_decoder->get_next_subtitle();

        }

        if (SUBTITLE_DELAY_ALTERED) {

            current_data.subtitle.display_timestamp.start = current_data.subtitle.original_timestamp.start + userPreferences::user_video_preferences.subtitle_delay_offset_ms;
            current_data.subtitle.display_timestamp.end = current_data.subtitle.original_timestamp.end + userPreferences::user_video_preferences.subtitle_delay_offset_ms;

            subtitle_decoder->adjust_queued_subtitle_delays(userPreferences::user_video_preferences.subtitle_delay_offset_ms);

            SUBTITLE_DELAY_ALTERED = false;

        }

        if (SUBTITLE_LOCATION_ALTERED) {

            current_data.subtitle_location = GET_SUBTITLE_LOCATION(userPreferences::user_video_preferences.subtitle_location, current_data.screen_width, current_data.screen_height);

            SUBTITLE_LOCATION_ALTERED = false;

        }

        glfwGetWindowSize(GLutil::window, &curr_screen_width, &curr_screen_height);

        if (curr_screen_height != current_data.screen_height || curr_screen_width != current_data.screen_width) {

            current_data.screen_height = float(curr_screen_height);
            current_data.screen_width = float(curr_screen_width);

            renderer->set_font_projection(current_data.screen_width, current_data.screen_height);
            current_data.subtitle_location = GET_SUBTITLE_LOCATION(userPreferences::user_video_preferences.subtitle_location, float(current_data.screen_width), float(current_data.screen_height));

        }

        glfwGetCursorPos(GLutil::window, &xpos, &ypos);

        if (xpos != current_data.mouse_xpos || ypos != current_data.mouse_ypos) {

            current_data.mouse_xpos = xpos;
            current_data.mouse_ypos = ypos;

            mouse_y_normalized = 1.0f - 2.0f * float(ypos) / current_data.screen_height;
            mouse_x_normalized = -1.0f + 2.0f * float(xpos) / current_data.screen_width;

            custom_cursor_pos_callback(mouse_x_normalized, mouse_y_normalized);

            current_data.last_mouse_move_time_ms = elapsed;
            render_overlay = true;

        }

        elapsed = video_decoder->get_current_audio_pts_val();

        if (elapsed >= current_data.subtitle.display_timestamp.end) {

            if (subtitle_decoder->has_subtitle()) {

                current_data.subtitle = subtitle_decoder->get_next_subtitle();

            }

        }

        if (elapsed - current_data.last_progress_bar_update_time_ms >= update_progress_delay_ms) {

            current_data.last_progress_bar_update_time_ms = elapsed;

            renderer->update_video_progress(video_duration, elapsed);

        }

        if (elapsed >= current_data.subtitle.display_timestamp.start) {

            renderer->draw_text(current_data.subtitle.text_codepoints, current_data.subtitle_location.x, current_data.subtitle_location.y, userPreferences::user_video_preferences.subtitle_scale);
            renderer->draw_text(current_data.subtitle.translated_text_codepoints, current_data.subtitle_location.x, current_data.subtitle_location.y - 40.0f, userPreferences::user_video_preferences.subtitle_scale);

        }

        if (current_data.last_mouse_move_time_ms + draw_overlay_time_ms <= elapsed) {

            render_overlay = false;

        }

        if (LANGUAGE_ALTER_MENU_OPEN) {

            renderer->draw_language_select_menu();

            renderer->draw_text(
                language_choice_display_text.choice_one, 
                language_choice_display_text.choice_one_text_location.topleftX, language_choice_display_text.choice_one_text_location.bottomrightX, 
                language_choice_display_text.choice_one_text_location.topleftY, language_choice_display_text.choice_one_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                language_choice_display_text.choice_two, 
                language_choice_display_text.choice_two_text_location.topleftX, language_choice_display_text.choice_two_text_location.bottomrightX, 
                language_choice_display_text.choice_two_text_location.topleftY, language_choice_display_text.choice_two_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                language_choice_display_text.choice_three, 
                language_choice_display_text.choice_three_text_location.topleftX, language_choice_display_text.choice_three_text_location.bottomrightX, 
                language_choice_display_text.choice_three_text_location.topleftY, language_choice_display_text.choice_three_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                language_choice_display_text.choice_four, 
                language_choice_display_text.choice_four_text_location.topleftX, language_choice_display_text.choice_four_text_location.bottomrightX, 
                language_choice_display_text.choice_four_text_location.topleftY, language_choice_display_text.choice_four_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

        }

        if (SETTINGS_OPEN) {

            renderer->draw_settings_menu();

            renderer->draw_text(
                settings_menu_display_text.color, 
                settings_menu_display_text.color_text_location.topleftX, settings_menu_display_text.color_text_location.bottomrightX, 
                settings_menu_display_text.color_text_location.topleftY, settings_menu_display_text.color_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                settings_menu_display_text.subtitle_location, 
                settings_menu_display_text.subtitle_location_text_location.topleftX, settings_menu_display_text.subtitle_location_text_location.bottomrightX, 
                settings_menu_display_text.subtitle_location_text_location.topleftY, settings_menu_display_text.subtitle_location_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                settings_menu_display_text.subtitle_delay, 
                settings_menu_display_text.subtitle_delay_text_location.topleftX, settings_menu_display_text.subtitle_delay_text_location.bottomrightX, 
                settings_menu_display_text.subtitle_delay_text_location.topleftY, settings_menu_display_text.subtitle_delay_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                settings_menu_display_text.subtitle_scale, 
                settings_menu_display_text.subtitle_scale_text_location.topleftX, settings_menu_display_text.subtitle_scale_text_location.bottomrightX, 
                settings_menu_display_text.subtitle_scale_text_location.topleftY, settings_menu_display_text.subtitle_scale_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

            renderer->draw_text(
                settings_menu_display_text.translate_to,
                settings_menu_display_text.translate_to_text_location.topleftX, settings_menu_display_text.translate_to_text_location.bottomrightX, 
                settings_menu_display_text.translate_to_text_location.topleftY, settings_menu_display_text.translate_to_text_location.bottomrightY, 
                current_data.screen_width, current_data.screen_height
            );

        }

        if (render_timestamp) {

            renderer->draw_text(
                current_data.display_time_codepoints, 
                float(current_data.mouse_xpos), 
                CONSTANTS::TRANSLATION::DISPLAY_TIME_Y_TRANSLATION, 
                CONSTANTS::TRANSLATION::TINY_TEXT_SCALE_DOWN + 0.1f
            );

        }

        if (render_overlay || SETTINGS_OPEN || LANGUAGE_ALTER_MENU_OPEN || GLOBAL_STATES::VIDEO_PAUSED) {

            renderer->draw_loading_bar();
            renderer->draw_settings_wheel();

        }


        if (GLOBAL_STATES::VIDEO_PAUSED) {

            if_paused();

            continue;

        }

        sync_video();
        sync_audio(); 


        process_input(GLutil::window);

        glfwSwapBuffers(GLutil::window);

    }

}

void videoPlayer::sync_audio() {

    if (video_decoder->has_audio()) {

        if (!audio_player->is_queue_full()) {

            current_data.audio_data = video_decoder->get_next_audio_pcm_data();
            audio_player->push_to_queue(current_data.audio_data);

        }

    }

}

void videoPlayer::sync_video() {

    if (video_decoder->has_video_data() && video_decoder->get_current_video_pts_val() <= elapsed) {

        current_data.video_data = video_decoder->get_next_video_data();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(textureName::MOVIE));

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, current_data.video_data.width, current_data.video_data.height, GL_RGB, GL_UNSIGNED_BYTE, current_data.video_data.rgb_pixels.data());

    }

}

void videoPlayer::perform_button_action(const buttonName& name, const double& mouse_x, const double& mouse_y) {

    switch (name) {

        case buttonName::SETTINGS: 
       
            SETTINGS_OPEN = !SETTINGS_OPEN;

            if (!SETTINGS_OPEN) {

                LANGUAGE_ALTER_MENU_OPEN = false;

            }

            break;

        case buttonName::FONT_COLOR_BACK:

            if (!SETTINGS_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            userPreferences::alter_font_color(false);

            break;
        
        case buttonName::FONT_COLOR_FORWARD:

            if (!SETTINGS_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            userPreferences::alter_font_color(true);

            break;

        case buttonName::SUBTITLE_DELAY_MINUS:

            if (!SETTINGS_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            SUBTITLE_DELAY_ALTERED = true;
            userPreferences::alter_subtitle_delay(false);

            break;

        case buttonName::SUBTITLE_DELAY_PLUS:

            if (!SETTINGS_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            SUBTITLE_DELAY_ALTERED = true;
            userPreferences::alter_subtitle_delay(true);

            break;

        case buttonName::LANGUAGE_SELECT:

            if (!SETTINGS_OPEN) break;

            LANGUAGE_ALTER_MENU_OPEN = !LANGUAGE_ALTER_MENU_OPEN;

            break;

        case buttonName::SUBTITLE_LOCATION_BACK:

            if (!SETTINGS_OPEN) break;

            SUBTITLE_LOCATION_ALTERED = true;
            SETTINGS_MENU_ALTERED = true;
            userPreferences::alter_subtitle_location(false);

            break;

        case buttonName::SUBTITLE_LOCATION_FORWARD:

            if (!SETTINGS_OPEN) break;

            SUBTITLE_LOCATION_ALTERED = true;
            SETTINGS_MENU_ALTERED = true;
            userPreferences::alter_subtitle_location(true);

            break;

        case buttonName::SUBTITLE_SCALE_MINUS:

            if (!SETTINGS_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            userPreferences::alter_subtitle_scale(false);

            break;

        case buttonName::SUBTITLE_SCALE_PLUS:

            if (!SETTINGS_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            userPreferences::alter_subtitle_scale(true);

            break;

        case buttonName::SEEK_VIDEO: 

            GLOBAL_STATES::VIDEO_SEEK_REQUESTED = true;

            break;

        case buttonName::LANGUAGE_SELECT_MENU:

            if (!LANGUAGE_ALTER_MENU_OPEN) break;

            SETTINGS_MENU_ALTERED = true;
            LANGUAGE_ALTERED = decide_language_altered(mouse_x, mouse_y);

            break;
            
        default:

           SETTINGS_OPEN = false; // something went wrong

    }

}

void videoPlayer::process_input(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {

        if (!space_was_clicked) {

            space_was_clicked = true;

            GLOBAL_STATES::VIDEO_PAUSED = !GLOBAL_STATES::VIDEO_PAUSED;

            if (!GLOBAL_STATES::VIDEO_PAUSED) {

                GLOBAL_STATES::VIDEO_PAUSED_CONDITION.notify_one(); // wake up all threads

                return;

            }

        }

    } else {

        space_was_clicked = false;

    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {

        if (!fullscreen_clicked) {

            std::cout << "\nTRUE!";

            fullscreen_clicked = true;
            FULLSCREEN_ENABLED = !FULLSCREEN_ENABLED;

            if (FULLSCREEN_ENABLED) {

                glfwGetWindowPos(GLutil::window, &last_windowed_mode_x, &last_windowed_mode_y);
                glfwGetWindowSize(GLutil::window, &last_windowed_mode_width, &last_windowed_mode_height);

                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);

                glfwSetWindowMonitor(GLutil::window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

            } else {

                glfwSetWindowMonitor(window, nullptr, last_windowed_mode_x, last_windowed_mode_y, last_windowed_mode_width, last_windowed_mode_height, 0);

                glfwSetWindowAttrib(window, GLFW_DECORATED, GLFW_TRUE);

            }

        }

    } else {

        fullscreen_clicked = false;

    }

}

void videoPlayer::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {

        double xpos, ypos;

        glfwGetCursorPos(window, &xpos, &ypos);

        int width, height;
        glfwGetWindowSize(GLutil::window, &width, &height);
        
        const float mouse_y_normalized = 1.0f - 2.0f * float(ypos) / height;
        const float mouse_x_normalized = -1.0f + 2.0f * float(xpos) / width;

        for (const Button& b : Buttons::buttons) {

            const buttonLocation location = b.location;

            if (is_within_box(mouse_x_normalized, mouse_y_normalized, location)) {

                perform_button_action(b.name, mouse_x_normalized, mouse_y_normalized);

                break;

            }

        }

    }

}

void videoPlayer::custom_cursor_pos_callback(const float& xpos, const float& ypos) {

    const buttonLocation progress_bar_position = Buttons::get_button_location(buttonName::SEEK_VIDEO);

    if (is_within_box(xpos, ypos, progress_bar_position)) {

        render_timestamp = true;

        float hover_progress = (xpos - progress_bar_position.topleftX) / (progress_bar_position.bottomrightX - progress_bar_position.topleftX);
        double display_time_ms = hover_progress * video_duration;

        int64_t seconds = static_cast<int64_t>(display_time_ms / 1000);

        std::string display_time = Utility::format_time_hhmmss(seconds);
        current_data.display_time_codepoints = Utility::get_codepoints_from_string(display_time);

    } else {

        render_timestamp = false;

    }

}

void videoPlayer::scroll_callback(GLFWwindow* window, double xoff, double yoff) {

    if (LANGUAGE_ALTER_MENU_OPEN != true) return; // to avoid altering language order while not opened

    double xpos, ypos;

    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(GLutil::window, &width, &height);
        
    const float mouse_y_normalized = 1.0f - 2.0f * float(ypos) / height;
    const float mouse_x_normalized = -1.0f + 2.0f * float(xpos) / width;

    const buttonLocation position = Buttons::get_button_location(buttonName::LANGUAGE_SELECT_MENU);

    if (is_within_box(mouse_x_normalized, mouse_y_normalized, position)) {

        if (yoff > 0) { // scrolled up 

            if (subtitleUtil::language_choice_one_display_idx != 0) {

                subtitleUtil::language_choice_one_display_idx--;

                LANGUAGE_CHOICES_ALTERED = true;

            }

        }

        else { // scrolled down

            if (!(subtitleUtil::language_choice_one_display_idx >= subtitleUtil::languages.size() - 4)) { // -4 since that is amount of languages displayed at a time

                subtitleUtil::language_choice_one_display_idx++;

                LANGUAGE_CHOICES_ALTERED = true;

            }

        }

    }

}

unsigned int videoPlayer::decide_language_altered(const double& mouse_x, const double& mouse_y) {

    buttonLocation location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_ONE);
    if (is_within_box(mouse_x, mouse_y, location)) return 1;

    location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_TWO);
    if (is_within_box(mouse_x, mouse_y, location)) return 2;

    location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_THREE);
    if (is_within_box(mouse_x, mouse_y, location)) return 3;

    location = Buttons::get_button_location(buttonName::LANGUAGE_CHOICE_FOUR);
    if (is_within_box(mouse_x, mouse_y, location)) return 4;

    return 1;

}

bool videoPlayer::is_within_box(const double& mouse_x, const double& mouse_y, const buttonLocation& location) {

    return (location.topleftY >= mouse_y && location.bottomrightY <= mouse_y) &&
           (location.topleftX <= mouse_x && location.bottomrightX >= mouse_x);

}

void videoPlayer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {

    glViewport(0, 0, width, height);

}