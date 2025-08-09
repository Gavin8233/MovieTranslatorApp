
#ifndef BUTTON_H
#define BUTTON_H

#include <vector>
#include <unordered_map>

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "include/variables.hpp"
#include "include/subtitleUtil.hpp"
#include "include/utility.hpp"


enum class buttonName {

    SETTINGS,
    FONT_COLOR_BACK,
    FONT_COLOR_FORWARD,
    SUBTITLE_DELAY_MINUS,
    SUBTITLE_DELAY_PLUS,
    LANGUAGE_SELECT,
    LANGUAGE_SELECT_BOX,
    SUBTITLE_LOCATION_BACK,
    SUBTITLE_LOCATION_FORWARD,
    SUBTITLE_SCALE_MINUS,
    SUBTITLE_SCALE_PLUS,
    SEEK_VIDEO,
    LANGUAGE_SELECT_MENU,
    LANGUAGE_CHOICE_ONE,
    LANGUAGE_CHOICE_TWO,
    LANGUAGE_CHOICE_THREE,
    LANGUAGE_CHOICE_FOUR

};

enum class wholeTexture {

    SETTINGS_MENU,
    SETTINGS_WHEEL,
    LANGUAGE_SELECT_MENU,
    LOADING_BAR

};

// i know this is ugly, this function is only for the json parsing in the init known buttons 
inline const buttonName GET_BUTTON_NAME_ENUM_FROM_STRING(const std::string& str) noexcept {

    if (str == "FONT_COLOR_BACK") return buttonName::FONT_COLOR_BACK;
    if (str == "FONT_COLOR_FORWARD") return buttonName::FONT_COLOR_FORWARD;
    if (str == "SUBTITLE_DELAY_MINUS") return buttonName::SUBTITLE_DELAY_MINUS;
    if (str == "SUBTITLE_DELAY_PLUS") return buttonName::SUBTITLE_DELAY_PLUS;
    if (str == "LANGUAGE_SELECT") return buttonName::LANGUAGE_SELECT;
    if (str == "LANGUAGE_SELECT_BOX") return buttonName::LANGUAGE_SELECT_BOX;
    if (str == "SUBTITLE_LOCATION_BACK") return buttonName::SUBTITLE_LOCATION_BACK;
    if (str == "SUBTITLE_LOCATION_FORWARD") return buttonName::SUBTITLE_LOCATION_FORWARD;
    if (str == "SUBTITLE_SCALE_MINUS") return buttonName::SUBTITLE_SCALE_MINUS;
    if (str == "SUBTITLE_SCALE_PLUS") return buttonName::SUBTITLE_SCALE_PLUS;
    if (str == "LANGUAGE_CHOICE_ONE") return buttonName::LANGUAGE_CHOICE_ONE;
    if (str == "LANGUAGE_CHOICE_TWO") return buttonName::LANGUAGE_CHOICE_TWO;
    if (str == "LANGUAGE_CHOICE_THREE") return buttonName::LANGUAGE_CHOICE_THREE;
    if (str == "LANGUAGE_CHOICE_FOUR") return buttonName::LANGUAGE_CHOICE_FOUR;

    return buttonName::FONT_COLOR_FORWARD;

}

struct buttonLocation {

    float topleftX, topleftY;
    float bottomrightX, bottomrightY;

};

struct Button {

    buttonLocation location;
    buttonName name;

};

namespace Buttons {

    extern std::vector<Button> buttons;
    
    buttonLocation get_button_location(const buttonName& name);

    /* To set values for the entire texture that the buttons will then be derived from. For example, the settings menu texture top left is xy and bottom right is x1y1, to get the actual
       top left value when making a button the transformation that is applied to the settings menu is sent into the function from the renderer and applied to the xy values to get the actual 
       correct location. After this location is found, the button locations can be derived from the resources/textures/data/info.json and placed in the correct location according to the settings
       menu top left and bottom right values. */
    void set_corner_values(const wholeTexture& name, const float& top_leftX, const float& top_leftY, const float& bottom_rightX, const float& bottom_rightY);

    /* In resources/textures/data there is json data for the button locations within the texture, in order to load these buttons in this function is used and called from the renderer class */
    void map_json_image_data(const std::string file_path, const glm::mat4& transformation, const wholeTexture& texture);

    // This make button is for the whole texture where the bottom right and top left values are already known from the set corner values function
    void make_button(const glm::mat4& transformation, const buttonName& name, const wholeTexture& texture_name);

    // This make button is for the json data where the button needs to be placed dependent on where the whole texture is set
    void make_button(const buttonName& name, const wholeTexture& texture_name, const glm::mat4& transformation, 
        float& top_leftX, float& top_leftY, float& bottom_rightX, float& bottom_rightY, const int& width, const int& height);

}

#endif