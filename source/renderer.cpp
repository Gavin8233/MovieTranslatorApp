#include "include/renderer.hpp"

Renderer::~Renderer() {

    glDeleteProgram(video_shader.ID);
    glDeleteProgram(font_shader.ID);
    glDeleteProgram(loading_bar_shader.ID);
    glDeleteProgram(loading_bar_fill_shader.ID);
    glDeleteProgram(settings_wheel_shader.ID);
    glDeleteProgram(settings_menu_shader.ID);
    glDeleteProgram(language_select_menu_shader.ID);

    glDeleteVertexArrays(1, &text_quad_VAO);
    glDeleteBuffers(1, &text_quad_VBO);

}

Renderer::Renderer(const std::string& font_path) {

    texture_atlas = new textureAtlas(font_path);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glDisable(GL_DEPTH_TEST);

    glGenVertexArrays(1, &text_quad_VAO);
    glGenBuffers(1, &text_quad_VBO);

    glBindVertexArray(text_quad_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, text_quad_VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(2);

}


void Renderer::draw_movie() {

    textureName location = textureName::MOVIE;

    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::VIDEO_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(location));

    video_shader.use();

    glBindVertexArray(GLutil::texture_vaos.at(location));
    glDrawArrays(GL_TRIANGLES, 0, 6); // draw movie

}

void Renderer::draw_text(const std::vector<uint32_t>& codepoints, float x, float y, float scale) {

    font_shader.use(); 
    font_shader.set_vec3("textColor", GET_FONT_COLOR(userPreferences::user_video_preferences.font_color));
    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::FONT_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(textureName::FONT));

    for (const uint32_t& codepoint : codepoints) {

        Character character = texture_atlas->get_character_data(codepoint);

        float w = (character.x1 - character.x0) * scale;
        float h = (character.y1 - character.y0) * scale;

        float xpos = x + character.x_off * scale;
        float ypos = y - (h - character.y_off * scale);

        float atlas_width = static_cast<float>(CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_WIDTH);
        float atlas_height = static_cast<float>(CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_HEIGHT);

        float u0 = character.x0 / atlas_width;
        float u1 = character.x1 / atlas_width;

        /* flip these so that text is correctly displayed */
        float v1 = character.y0 / atlas_height;
        float v0 = character.y1 / atlas_height;

        float vertices[6][4] = {

            { xpos,     ypos + h,   u0, v1 },            
            { xpos,     ypos,       u0, v0 },
            { xpos + w, ypos,       u1, v0 },

            { xpos,     ypos + h,   u0, v1 },
            { xpos + w, ypos,       u1, v0 },
            { xpos + w, ypos + h,   u1, v1 }  

        };

        glBindBuffer(GL_ARRAY_BUFFER, text_quad_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        glBindVertexArray(text_quad_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += character.advance * scale;

    }

}

void Renderer::draw_text(const std::vector<uint32_t>& codepoints, const float& startX, const float& endX, const float& startY, const float& endY, const float& screen_width, const float& screen_height) {

    font_shader.use(); 
    font_shader.set_vec3("textColor", GET_FONT_COLOR(userPreferences::user_video_preferences.font_color));
    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::FONT_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(textureName::FONT));

    float tlx = (startX + 1.0f) * 0.5f * screen_width;
    float brx = (endX + 1.0f) * 0.5f * screen_width;
    float tly = (startY + 1.0f) * 0.5f * screen_height;
    float bry = (endY + 1.0f) * 0.5f * screen_height;

    float space = brx - tlx;
    float scale = 0.3f;
    float total_text_width = 0.0f;
    float max_y_off = 0.0f;

    // figure out what scale we can use where text still fits inside box
    while (true) {

        total_text_width = 0.0f;
        max_y_off = 0.0f;

        for (const uint32_t& codepoint : codepoints) {

            Character character = texture_atlas->get_character_data(codepoint);

            if (character.y_off > max_y_off) max_y_off = float(character.y_off);

            total_text_width += character.advance * scale;

        }

        if (total_text_width <= space) break;

        scale -= 0.01f;

    }

    float x = tlx + (space - total_text_width) / 2.0f;
    x += 10.0f; // push a little to the right(just looks better)

    // now that we know proper scale, we can just draw the text as normal
    for (const uint32_t& codepoint : codepoints) {

        Character character = texture_atlas->get_character_data(codepoint);

        float w = (character.x1 - character.x0) * scale;
        float h = (character.y1 - character.y0) * scale;

        float xpos = x + character.x_off * scale;
        float ypos;
        if (codepoint == '-') ypos = (bry + max_y_off * scale) + 5.0f; // this is so hacky but the only time ypos needs to be changed is when a - sign is next to a number. if this needs to be changed it will be 
        else ypos = bry + max_y_off * scale;

        float atlas_width = static_cast<float>(CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_WIDTH);
        float atlas_height = static_cast<float>(CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_HEIGHT);

        float u0 = character.x0 / atlas_width;
        float u1 = character.x1 / atlas_width;

        /* flip these so that text is correctly displayed */
        float v1 = character.y0 / atlas_height;
        float v0 = character.y1 / atlas_height;

        float vertices[6][4] = {

            { xpos,     ypos + h,   u0, v1 },            
            { xpos,     ypos,       u0, v0 },
            { xpos + w, ypos,       u1, v0 },

            { xpos,     ypos + h,   u0, v1 },
            { xpos + w, ypos,       u1, v0 },
            { xpos + w, ypos + h,   u1, v1 }  

        };

        glBindBuffer(GL_ARRAY_BUFFER, text_quad_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        
        glBindVertexArray(text_quad_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += character.advance * scale;

    }

}

void Renderer::draw_loading_bar() {

    textureName location = textureName::PROGRESS_BAR;

    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::PROGRESS_BAR_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(location));

    loading_bar_fill_shader.use();
    
    glBindVertexArray(GLutil::texture_vaos.at(location));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLutil::texture_ebos.at(location));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    loading_bar_shader.use();

    glBindVertexArray(GLutil::texture_vaos.at(location));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Renderer::draw_settings_wheel() {

    textureName location = textureName::SETTINGS_WHEEL;

    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::SETTINGS_WHEEL_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(location));

    settings_wheel_shader.use();

    glBindVertexArray(GLutil::texture_vaos.at(location));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLutil::texture_ebos.at(location));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Renderer::draw_settings_menu() {

    textureName location = textureName::SETTINGS_MENU;

    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::SETTINGS_MENU_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, GLutil::texture_ids.at(location));

    settings_menu_shader.use();

    glBindVertexArray(GLutil::texture_vaos.at(location));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLutil::texture_ebos.at(location));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Renderer::draw_language_select_menu() {

    textureName location = textureName::LANGUAGE_SELECT;

    glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::LANGUAGE_SELECT_TEXTURE);

    language_select_menu_shader.use();

    glBindVertexArray(GLutil::texture_vaos.at(location));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GLutil::texture_ebos.at(location));

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void Renderer::init_shaders() {

    init_video_shader();
    init_font_shader();

    init_loading_bar_shader(
        CONSTANTS::TRANSLATION::NO_TRANSLATION, CONSTANTS::TRANSLATION::LOADING_BAR_Y_OFFSET, 
        CONSTANTS::TRANSLATION::NO_SCALE, CONSTANTS::TRANSLATION::LOADING_BAR_Y_SCALE
    );

    init_loading_bar_fill_shader(
        CONSTANTS::TRANSLATION::NO_TRANSLATION, CONSTANTS::TRANSLATION::FILL_SHADER_Y_OFFSET, 
        CONSTANTS::TRANSLATION::NO_SCALE, CONSTANTS::TRANSLATION::FILL_SHADER_Y_SCALE
    );

    init_settings_wheel(
        CONSTANTS::TRANSLATION::SETTINGS_WHEEL_X_OFFSET, CONSTANTS::TRANSLATION::SETTINGS_WHEEL_Y_OFFSET, 
        CONSTANTS::TRANSLATION::SETTINGS_WHEEL_X_SCALE, CONSTANTS::TRANSLATION::SETTINGS_WHEEL_Y_SCALE
    );

    init_settings_menu(
        CONSTANTS::TRANSLATION::SETTINGS_MENU_X_OFFSET, CONSTANTS::TRANSLATION::SETTINGS_MENU_Y_OFFSET, 
        CONSTANTS::TRANSLATION::SETTINGS_MENU_X_SCALE_DOWN, CONSTANTS::TRANSLATION::SETTINGS_MENU_Y_SCALE_DOWN
    );

    init_language_select_menu(
        CONSTANTS::TRANSLATION::LANGUAGE_MENU_X_OFFSET, CONSTANTS::TRANSLATION::LANGUAGE_MENU_Y_OFFSET, 
        CONSTANTS::TRANSLATION::LANGUAGE_MENU_X_SCALE_DOWN, CONSTANTS::TRANSLATION::LANGUAGE_MENU_Y_SCALE_DOWN
    );

}

void Renderer::init_video_shader() {

    video_shader.compile(GET_SHADER_FOLDER_PATH() + "video_shader.vert", GET_SHADER_FOLDER_PATH() + "video_shader.frag");
    video_shader.use();
    video_shader.set_int("texture0", CONSTANTS::TEXTURES::VIDEO_TEXTURE);

}

void Renderer::init_font_shader() {

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f); // just set to this because why not, it will be changed as soon as the main loop runs and will be properly displayed

    font_shader.compile(GET_SHADER_FOLDER_PATH() + "font_shader.vert", GET_SHADER_FOLDER_PATH() + "font_shader.frag");
    font_shader.use();
    font_shader.set_int("texture1", CONSTANTS::TEXTURES::FONT_TEXTURE);
    font_shader.set_mat4("projection", projection);

}

void Renderer::init_loading_bar_fill_shader(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale) {

    loading_bar_fill_shader.compile(GET_SHADER_FOLDER_PATH() + "overlay.vert", GET_SHADER_FOLDER_PATH() + "fill_overlay.frag");
    loading_bar_fill_shader.use();

    glm::mat4 fill_bar_transformation = GLutil::make_transformation(x_offset, y_offset, x_scale, y_scale);

    loading_bar_fill_shader.set_mat4("transform", fill_bar_transformation);
    loading_bar_fill_shader.set_vec3("color", GET_FONT_COLOR(Color::RED));

    // the fill shader needs to know where the begin and end of the bar is in order to not fill past or before it

    constexpr float FILL_COLOR_BEGIN_POS_X = 0.011f; // start of loading bar box
    constexpr float FILL_COLOR_END_POS_X = 0.99f; // end of loading bar box

    loading_bar_fill_shader.set_float("bar_begin_pos", FILL_COLOR_BEGIN_POS_X);
    loading_bar_fill_shader.set_float("bar_end_pos", FILL_COLOR_END_POS_X);

}

void Renderer::init_loading_bar_shader(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale) {

    loading_bar_shader.compile(GET_SHADER_FOLDER_PATH() + "overlay.vert", GET_SHADER_FOLDER_PATH() + "overlay.frag");
    loading_bar_shader.use();

    glm::mat4 loading_bar_transformation = GLutil::make_transformation(x_offset, y_offset, x_scale, y_scale);

    Buttons::make_button(loading_bar_transformation, buttonName::SEEK_VIDEO, wholeTexture::LOADING_BAR);

    loading_bar_shader.set_mat4("transform", loading_bar_transformation);
    loading_bar_shader.set_int("tex", CONSTANTS::TEXTURES::PROGRESS_BAR_TEXTURE);

}

void Renderer::init_settings_wheel(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale) {

    settings_wheel_shader.compile(GET_SHADER_FOLDER_PATH() + "overlay.vert", GET_SHADER_FOLDER_PATH() + "overlay.frag");
    settings_wheel_shader.use();

    glm::mat4 settings_wheel_transformation = GLutil::make_transformation(x_offset, y_offset, x_scale, y_scale);

    Buttons::make_button(settings_wheel_transformation, buttonName::SETTINGS, wholeTexture::SETTINGS_WHEEL);

    settings_wheel_shader.set_mat4("transform", settings_wheel_transformation);
    settings_wheel_shader.set_int("tex", CONSTANTS::TEXTURES::SETTINGS_WHEEL_TEXTURE);

}

void Renderer::init_settings_menu(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale) {

    settings_menu_shader.compile(GET_SHADER_FOLDER_PATH() + "overlay.vert", GET_SHADER_FOLDER_PATH() + "overlay.frag");
    settings_menu_shader.use();

    glm::mat4 settings_menu_transformation = GLutil::make_transformation(x_offset, y_offset, x_scale, y_scale);

    Buttons::map_json_image_data(GET_RESOURCE_FOLDER_PATH() + "textures/data/settingsmenu.json", settings_menu_transformation, wholeTexture::SETTINGS_MENU);

    settings_menu_shader.set_mat4("transform", settings_menu_transformation);
    settings_menu_shader.set_int("tex", CONSTANTS::TEXTURES::SETTINGS_MENU_TEXTURE);

}

void Renderer::init_language_select_menu(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale) {

    language_select_menu_shader.compile(GET_SHADER_FOLDER_PATH() + "overlay.vert", GET_SHADER_FOLDER_PATH() + "overlay.frag");
    language_select_menu_shader.use();

    glm::mat4 language_select_menu_transformation = GLutil::make_transformation(x_offset, y_offset, x_scale, y_scale);

    Buttons::make_button(language_select_menu_transformation, buttonName::LANGUAGE_SELECT_MENU, wholeTexture::LANGUAGE_SELECT_MENU);
    Buttons::map_json_image_data(GET_RESOURCE_FOLDER_PATH() + "textures/data/languageselect.json", language_select_menu_transformation, wholeTexture::LANGUAGE_SELECT_MENU);

    language_select_menu_shader.set_mat4("transform", language_select_menu_transformation);
    language_select_menu_shader.set_int("tex", CONSTANTS::TEXTURES::LANGUAGE_SELECT_TEXTURE);

}

void Renderer::update_video_progress(const int64_t& video_duration, const int64_t& elapsed) {

    float progress = 1.0f * elapsed / video_duration; // 1.0f * to avoid integer divion error

    loading_bar_fill_shader.use();
    loading_bar_fill_shader.set_float("progress", progress);

}

void Renderer::set_font_projection(const float& screen_width, const float& screen_height) {

    glm::mat4 projection = glm::ortho(0.0f, screen_width, 0.0f, screen_height);

    font_shader.use();
    font_shader.set_mat4("projection", projection);    

}