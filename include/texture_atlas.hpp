
#ifndef TEXTUREATLAS_H
#define TEXTUREATLAS_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include "include/utility.hpp"
#include "include/GLutil.hpp"

#include <map>
#include <iostream>

struct Character {

    int x0, y0, x1, y1;
    int x_off, y_off;
    int advance;

};

class textureAtlas {

    public:

        textureAtlas(const std::string& font_path);

        void load_character(const uint32_t& codepoint);
        Character get_character_data(const uint32_t& codepoint);
        bool has_character(const uint32_t& codepoint) const;

    private:

        std::map<uint32_t, Character> character_data_map;

        FT_Library ft_library;
        FT_Face ft_face;
        int pen_x;
        int pen_y;

};



#endif