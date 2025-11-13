
#ifndef GLUTIL_H
#define GLUTIL_H

#include "include/utility/utility.hpp"
#include "include/GL/button.hpp"

#include <unordered_map>

enum class textureName {

    MOVIE,
    FONT,
    PROGRESS_BAR,
    SETTINGS_WHEEL,
    SETTINGS_MENU,
    LANGUAGE_SELECT

};

namespace GLutil {

    extern GLFWwindow* window;
    void destroy_gl_context();
    void init_opengl_context();

    extern std::unordered_map<textureName, unsigned int> texture_ids;
    extern std::unordered_map<textureName, unsigned int> texture_vaos;
    extern std::unordered_map<textureName, unsigned int> texture_ebos;

    void init_texture_atlas();
    void init_overlay_textures();
    void init_video_texture(const int& frame_width, const int& frame_height);

    glm::mat4 make_transformation(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);

}


#endif