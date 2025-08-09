
#include "include/button.hpp"

namespace Buttons {

    void map_json_image_data(const std::string file_path, const glm::mat4& transformation, const wholeTexture& texture) {

        std::string text = Utility::load_file_text_into_string(file_path);
        nlohmann::json image_data = nlohmann::json::parse(text);

        const int width = image_data["width"].get<int>();
        const int height = image_data["height"].get<int>();

        nlohmann::json button_data = image_data["buttons"];

        for (const nlohmann::json& data : button_data) {

            buttonName name = GET_BUTTON_NAME_ENUM_FROM_STRING(data["name"].get<std::string>());
            float top_leftX = data["tlx"].get<float>();
            float top_leftY = data["tly"].get<float>();
            float bottom_rightX = data["brx"].get<float>();
            float bottom_rightY = data["bry"].get<float>();

            make_button(name, texture, transformation, top_leftX, top_leftY, bottom_rightX, bottom_rightY, width, height);

        }

    }

    std::vector<Button> buttons;

    std::unordered_map<wholeTexture, glm::vec2> top_left_corners;
    std::unordered_map<wholeTexture, glm::vec2> bottom_right_corners;

    buttonLocation get_button_location(const buttonName& name) {

        for (const Button& button : buttons) {

            if (button.name == name) return button.location;

        }

        return buttons.at(0).location;

    }

    void set_corner_values(const wholeTexture& name, const float& top_leftX, const float& top_leftY, const float& bottom_rightX, const float& bottom_rightY) {

        glm::vec2 top_left(top_leftX, top_leftY);
        glm::vec2 bottom_right(bottom_rightX, bottom_rightY);

        top_left_corners.insert({ name, top_left });
        bottom_right_corners.insert({ name, bottom_right });

    }

    void make_button(const glm::mat4& transformation, const buttonName& name, const wholeTexture& texture_name) {

        glm::vec4 top_left_ndc = transformation * glm::vec4(top_left_corners.at(texture_name), 0.0f, 1.0f);
        glm::vec4 bottom_right_ndc = transformation * glm::vec4(bottom_right_corners.at(texture_name), 0.0f, 1.0f);

        buttonLocation location;

        location.topleftX = top_left_ndc.x;
        location.topleftY = top_left_ndc.y;
        location.bottomrightX = bottom_right_ndc.x;
        location.bottomrightY = bottom_right_ndc.y;

        Button button;

        button.location = location;
        button.name = name;

        buttons.emplace_back(button);

    }

    void make_button(const buttonName& button_name, const wholeTexture& texture_name, const glm::mat4& transformation, 
        float& top_leftX, float& top_leftY, float& bottom_rightX, float& bottom_rightY, const int& width, const int& height) {

        // to normalized coordinates

        top_leftX /= width;
        bottom_rightX /= width;
        top_leftY /= height;
        bottom_rightY /= height;

        glm::vec4 transformed_ndc_top_left = transformation * glm::vec4(top_left_corners.at(texture_name), 0.0f, 1.0f);
        glm::vec4 transformed_ndc_bottom_right = transformation * glm::vec4(bottom_right_corners.at(texture_name), 0.0f, 1.0f);

        glm::vec2 menu_ndc_top_left = glm::vec2(transformed_ndc_top_left.x, transformed_ndc_top_left.y);
        glm::vec2 menu_ndc_bottom_right = glm::vec2(transformed_ndc_bottom_right.x, transformed_ndc_bottom_right.y);

        glm::vec2 button_top_left_ndc = glm::mix(menu_ndc_top_left, menu_ndc_bottom_right, glm::vec2(top_leftX, top_leftY));
        glm::vec2 button_bottom_right_ndc = glm::mix(menu_ndc_top_left, menu_ndc_bottom_right, glm::vec2(bottom_rightX, bottom_rightY));

        buttonLocation location;

        location.topleftX = button_top_left_ndc.x;
        location.topleftY = button_top_left_ndc.y;
        location.bottomrightX = button_bottom_right_ndc.x;
        location.bottomrightY = button_bottom_right_ndc.y;

        Button button;

        button.location = location;
        button.name = button_name;

        buttons.emplace_back(button);

    }

}