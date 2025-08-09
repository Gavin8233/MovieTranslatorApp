
#ifndef RENDERER_H
#define RENDERER_H

#include "include/texture_atlas.hpp"
#include "include/utility.hpp"
#include "include/shader.hpp"
#include "include/GLutil.hpp"

class Renderer {

    public:

        Renderer(const std::string& font_path);
        ~Renderer();

        void update_video_progress(const int64_t& video_duration, const int64_t& elapsed);
        void set_font_projection(const float& screen_width, const float& screen_height);

        void init_shaders();

        void draw_movie();
        void draw_loading_bar();
        void draw_settings_wheel();
        void draw_settings_menu();
        void draw_language_select_menu();
        
        // Draw text with a known location, only used in this code for the subtitle location
        void draw_text(const std::vector<uint32_t>& codepoints, float x, float y, float scale);

        /* Draw text with an unknown location, and needs to be placed within a box, ie. startx endx starty endy. Screen height and width are passed in because ndc coordinates need to be 
           transformed into actual screen coordinates like 1060x and 400y in order to properly check if text is within box, since text is drawn with screen coordinates. */
        void draw_text(const std::vector<uint32_t>& codepoints, const float& startX, const float& endX, const float& startY, const float& endY, const float& screen_width, const float& screen_height); 

    private:

        textureAtlas* texture_atlas;

        Shader video_shader;
        Shader font_shader;

        Shader loading_bar_shader;
        Shader loading_bar_fill_shader;
        Shader settings_wheel_shader;
        Shader settings_menu_shader;
        Shader language_select_menu_shader;

        void init_video_shader();
        void init_font_shader();

        void init_loading_bar_shader(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);
        void init_loading_bar_fill_shader(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);
        void init_settings_wheel(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);
        void init_settings_menu(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);
        void init_language_select_menu(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);

        /* these are going to be changed a lot so defined here */

        unsigned int text_quad_VAO, text_quad_VBO;

};



#endif