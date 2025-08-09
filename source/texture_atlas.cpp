
#include "include/texture_atlas.hpp"

/* Basically this entire file is thanks to the user https://gist.github.com/baines with his post https://gist.github.com/baines/b0f9e4be04ba4e6f56cab82eef5008ff 
   No idea who you are but thank you, and to anyone viewing this file go check his profile out */

textureAtlas::textureAtlas(const std::string& font_path) :

pen_x { 0 },
pen_y { 0 }

{

    if (FT_Init_FreeType(&ft_library)) {

        throw std::runtime_error("FAILED TO INIT FREETYPE");

    }

    if (FT_New_Face(ft_library, font_path.c_str(), 0, &ft_face)) {

        std::cerr << "\nERROR: Failed to load font at: " << font_path << "."
        "\nThis is likely due to a misconfigured ttf file. Try installing a different one and use that."
        "\nIf this message keeps appearing, let the application use the default font."
        "\nIf the default font works(Even if there are boxes, it is working), then you need to find a compatible ttf file that works with Freetype."
        "\nIf the default font also fails, try to reinstall and rebuild the project.";
        throw std::runtime_error("FAILED TO LOAD FONT");

    }

    FT_Set_Pixel_Sizes(ft_face, 0, 48);

}

void textureAtlas::load_character(const uint32_t& codepoint) {

    if (FT_Load_Char(ft_face, codepoint, FT_LOAD_RENDER)) {

        std::cerr << "\nERROR: Failed to load character."
        "\nThis is likely due to your ttf file not containing the required character."
        "\nEnsure that the ttf file you are using supports the language's characters."
        "\nYou can use the -fontfile path/to/.ttf command when running the executable to set a custom font file if you are using the default one";
        throw std::runtime_error("FAILED TO LOAD CHARACTER");

    }

    FT_Bitmap* bmp = &ft_face->glyph->bitmap;

    if (pen_x + bmp->width >= CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_WIDTH) {

        pen_x = 0;
        pen_y += (ft_face->size->metrics.height >> 6) + 1;

    }  

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(textureName::FONT));
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    if (bmp->buffer) {

        glTexSubImage2D(GL_TEXTURE_2D, 0, pen_x, pen_y, bmp->width, bmp->rows, GL_RED, GL_UNSIGNED_BYTE, bmp->buffer);

    }

    Character new_character;

    new_character.x0 = pen_x;
    new_character.y0 = pen_y;
    new_character.x1 = pen_x + bmp->width;
    new_character.y1 = pen_y + bmp->rows;
    new_character.x_off = ft_face->glyph->bitmap_left;
    new_character.y_off = ft_face->glyph->bitmap_top;
    new_character.advance = ft_face->glyph->advance.x >> 6;

    pen_x += bmp->width + 1;

    character_data_map[codepoint] = new_character;

}

bool textureAtlas::has_character(const uint32_t& codepoint) const {

    return character_data_map.count(codepoint);

}

Character textureAtlas::get_character_data(const uint32_t& codepoint) {

    /* we load characters here because it is fast enough to not care about, although it should probably be done elsewhere */

    if (!character_data_map.count(codepoint)) {

        load_character(codepoint);

    }

    return character_data_map.at(codepoint);

}