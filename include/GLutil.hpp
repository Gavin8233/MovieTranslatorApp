
#ifndef GLUTIL_H
#define GLUTIL_H

#include "include/utility.hpp"
#include "include/button.hpp"

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
    void init_texture_atlas();

    unsigned int load_texture(const std::string file);

    extern std::unordered_map<textureName, unsigned int> texture_ids;
    extern std::unordered_map<textureName, unsigned int> texture_vaos;
    extern std::unordered_map<textureName, unsigned int> texture_ebos;

    void init_overlay_textures();
    void init_video_texture(const int& frame_width, const int& frame_height);

    void init_texture_geometry(float* vertices, std::size_t vertices_size, unsigned int* indices, std::size_t indices_size, const textureName& texture_name,
        int vertex_attrib1, int attrib1_size, int attrib1_stride, void* attrib1_offset, int vertex_attrib2, int attrib2_size, int attrib2_stride, void* attrib2_offset);

    glm::mat4 make_transformation(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale);

}


#endif