
#include "include/utility/GLutil.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static unsigned int load_texture(const std::string file) {

    stbi_set_flip_vertically_on_load(true);

    unsigned int texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nr_channels;
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &nr_channels, 0);

    if (data) {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

    }

    else {

        throw std::runtime_error("FAILED TO LOAD IMAGE WITH STB IMAGE");

    }

    stbi_image_free(data);

    return texture;

}

static void init_overlay_texture
(
    std::unordered_map<textureName, unsigned int>& texture_ids,
    const textureName& tex, unsigned int& tex_id, const int& tex_unit, 
    const std::string& tex_path, const char* png_file, 
    const std::string& err_message
) 
{

    glActiveTexture(GL_TEXTURE0 + tex_unit);

    try {

        tex_id = load_texture(tex_path + png_file);

    }

    catch (std::exception& e) {

        std::cerr << e.what() << err_message << std::endl;
        return;

    }

    texture_ids[tex] = tex_id;

}

static void init_texture_geometry
(
    std::unordered_map<textureName, unsigned int>& texture_vaos, std::unordered_map<textureName, unsigned int>& texture_ebos,
    const float* vertices, std::size_t vertices_size, 
    const unsigned int* indices, std::size_t indices_size, 
    const textureName& texture_name,
    int vertex_attrib1, int attrib1_size, int attrib1_stride, void* attrib1_offset, 
    int vertex_attrib2, int attrib2_size, int attrib2_stride, void* attrib2_offset
)
{

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    texture_vaos[texture_name] = VAO;

    if (indices) {

        glGenBuffers(1, &EBO);

        texture_ebos[texture_name] = EBO;

    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_STATIC_DRAW);

    if (indices) {

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);

    }

    glVertexAttribPointer(vertex_attrib1, attrib1_size, GL_FLOAT, GL_FALSE, attrib1_stride, attrib1_offset);
    glEnableVertexAttribArray(vertex_attrib1);

    glVertexAttribPointer(vertex_attrib2, attrib2_size, GL_FLOAT, GL_FALSE, attrib2_stride, attrib2_offset);
    glEnableVertexAttribArray(vertex_attrib2);

}

namespace GLutil {

    GLFWwindow* window = nullptr;

    void destroy_gl_context() {

        for (const std::pair<textureName, unsigned int>& vaos : texture_vaos) {

            glDeleteVertexArrays(1, &vaos.second);

        }

        for (const std::pair<textureName, unsigned int>& ebos : texture_ebos) {

            glDeleteBuffers(1, &ebos.second);

        }

        glfwTerminate();

    }

    void init_opengl_context() {

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();

        if (!monitor) {

            throw std::runtime_error("FAILED TO FIND PRIMARY MONITOR");

        }

        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if (!mode) {

            throw std::runtime_error("FAILED TO GET VIDEO MODE");

        }

        int width = mode->width;
        int height = mode->height;

        window = glfwCreateWindow(width, height, "Translater App", NULL, NULL);

        if (window == nullptr) {

            throw std::runtime_error("FAILED TO CREATE GL WINDOW");

        }

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {

            throw std::runtime_error("FAILED TO INITIALIZE GLAD");

        }

        glViewport(0, 0, width, height);

    }

    std::unordered_map<textureName, unsigned int> texture_ids;
    std::unordered_map<textureName, unsigned int> texture_vaos;
    std::unordered_map<textureName, unsigned int> texture_ebos;

    void init_texture_atlas() {

        glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::FONT_TEXTURE);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned int font_texture_id;

        glGenTextures(1, &font_texture_id);
        glBindTexture(GL_TEXTURE_2D, font_texture_id);

        texture_ids[textureName::FONT] = font_texture_id;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_WIDTH, CONSTANTS::TEX_ATLAS::TEXTURE_ATLAS_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    }

    void init_video_texture(const int& frame_width, const int& frame_height) {
        
        constexpr float vertices[] = {

            -1.0f,  1.0f,     0.0f, 0.0f, 
            -1.0f,  -1.0f,    0.0f, 1.0f, 
            1.0f,   -1.0f,    1.0f, 1.0f, 

            -1.0f,  1.0f,     0.0f, 0.0f, 
            1.0f,  -1.0f,     1.0f, 1.0f, 
            1.0f,   1.0f,     1.0f, 0.0f  

        };

        init_texture_geometry
        (
            texture_vaos, texture_ebos,
            vertices, sizeof(vertices), 
            nullptr, 0, 
            textureName::MOVIE,

            0, 2, 4 * sizeof(float), (void*)0,                   // vertex attrib 1                 
            1, 2, 4 * sizeof(float), (void*)(2 * sizeof(float))     // vertex attrib 2
        );

        unsigned int texture;

        glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::VIDEO_TEXTURE);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        texture_ids[textureName::MOVIE] = texture;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame_width, frame_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    }

    void init_overlay_textures() {

        // init the textures

        const std::string tex_path = GET_RESOURCE_FOLDER_PATH() + "textures/";
        const std::string err_message = "\nDid you run cmake --install .?"
        "\nPath=" + tex_path + ""
        "\nVerify that this path exists."
        "\nIn this folder you should have:\nbar.png\nsettings_wheel.png\nsettingsmenu.png\nlanguageselect.png"
        "\nIf you do not, redownload the textures from the GitHub repository and place them in the expected path.";

        unsigned int progress_bar_texture_id;
        unsigned int settings_wheel_texture_id;
        unsigned int settings_menu_texture_id;
        unsigned int language_select_menu_texture_id;

        init_overlay_texture
        (
            texture_ids,
            textureName::PROGRESS_BAR, progress_bar_texture_id, CONSTANTS::TEXTURES::PROGRESS_BAR_TEXTURE, 
            tex_path, "bar.png", 
            err_message
        );

        init_overlay_texture
        (
            texture_ids,
            textureName::SETTINGS_WHEEL, settings_wheel_texture_id, CONSTANTS::TEXTURES::SETTINGS_WHEEL_TEXTURE, 
            tex_path, "settings_wheel.png",
            err_message
        );

        init_overlay_texture
        (
            texture_ids,
            textureName::SETTINGS_MENU, settings_menu_texture_id, CONSTANTS::TEXTURES::SETTINGS_MENU_TEXTURE, 
            tex_path, "settingsmenu.png", 
            err_message
        );

        init_overlay_texture
        (
            texture_ids,
            textureName::LANGUAGE_SELECT, language_select_menu_texture_id, CONSTANTS::TEXTURES::LANGUAGE_SELECT_TEXTURE, 
            tex_path, "languageselect.png", 
            err_message
        );


        // now setup geometry & set the corners of each texture for the button transformations to be according to size and location of whole texture

        constexpr unsigned int indices[] = {

            0, 1, 3,
            1, 2, 3

        };

        constexpr float progress_bar_vertices[] = {

            0.9f, 0.8f, 0.0f,       1.0f, 1.0f,
            0.9f, -0.8f, 0.0f,      1.0f, 0.0f,
            -0.9f, -0.8f, 0.0f,     0.0f, 0.0f,
            -0.9f, 0.8f, 0.0f,      0.0f, 1.0f

        };

        constexpr float vertices[] = {

            0.5f, 0.5f, 0.0f,       1.0f, 1.0f,   
            0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
            -0.5f, 0.5f, 0.0f,      0.0f, 1.0f

        };

        constexpr int stride = 5 * sizeof(float);

        constexpr float tlx_progress_bar = progress_bar_vertices[15];
        constexpr float tly_progress_bar = progress_bar_vertices[16];
        constexpr float brx_progress_bar = progress_bar_vertices[5];
        constexpr float bry_progress_bar = progress_bar_vertices[6];

        constexpr float base_tlx = vertices[15];
        constexpr float base_tly = vertices[16];
        constexpr float base_brx = vertices[5];
        constexpr float base_bry = vertices[6];

        Buttons::set_corner_values(wholeTexture::LOADING_BAR, tlx_progress_bar, tly_progress_bar, brx_progress_bar, bry_progress_bar);
        Buttons::set_corner_values(wholeTexture::SETTINGS_WHEEL, base_tlx, base_tly, base_brx, base_bry);  
        Buttons::set_corner_values(wholeTexture::SETTINGS_MENU, base_tlx, base_tly, base_brx, base_bry);
        Buttons::set_corner_values(wholeTexture::LANGUAGE_SELECT_MENU, base_tlx, base_tly, base_brx, base_bry);

        init_texture_geometry
        (
            texture_vaos, texture_ebos,
            progress_bar_vertices, sizeof(progress_bar_vertices),
            indices, sizeof(indices),
            textureName::PROGRESS_BAR, 

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

        init_texture_geometry
        (
            texture_vaos, texture_ebos,
            vertices, sizeof(vertices), 
            indices, sizeof(indices), 
            textureName::SETTINGS_WHEEL,

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

        init_texture_geometry
        (
            texture_vaos, texture_ebos,
            vertices, sizeof(vertices),
            indices, sizeof(indices),
            textureName::SETTINGS_MENU,

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

        init_texture_geometry
        (
            texture_vaos, texture_ebos,
            vertices, sizeof(vertices),
            indices, sizeof(indices),
            textureName::LANGUAGE_SELECT,

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

    }


    glm::mat4 make_transformation(const float& x_offset, const float& y_offset, const float& x_scale, const float& y_scale) {
    
        glm::mat4 transformation = glm::mat4(1.0f);

        transformation = glm::translate(transformation, glm::vec3(x_offset, y_offset, 0.0f));
        transformation = glm::scale(transformation, glm::vec3(x_scale, y_scale, 1.0f));

        return transformation;

    }

}