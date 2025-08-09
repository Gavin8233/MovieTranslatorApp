
#include "include/GLutil.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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

    unsigned int load_texture(const std::string file) {

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

    std::unordered_map<textureName, unsigned int> texture_ids;
    std::unordered_map<textureName, unsigned int> texture_vaos;
    std::unordered_map<textureName, unsigned int> texture_ebos;

    void init_texture_geometry(float* vertices, std::size_t vertices_size, unsigned int* indices, std::size_t indices_size, const textureName& texture_name,
        int vertex_attrib1, int attrib1_size, int attrib1_stride, void* attrib1_offset, int vertex_attrib2, int attrib2_size, int attrib2_stride, void* attrib2_offset)
        
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

    void init_video_texture(const int& frame_width, const int& frame_height) {
        
        float vertices[] = {

            -1.0f,  1.0f,     0.0f, 0.0f, 
            -1.0f,  -1.0f,    0.0f, 1.0f, 
            1.0f,   -1.0f,    1.0f, 1.0f, 

            -1.0f,  1.0f,     0.0f, 0.0f, 
            1.0f,  -1.0f,     1.0f, 1.0f, 
            1.0f,   1.0f,     1.0f, 0.0f  

        };

        init_texture_geometry
        (
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

        unsigned int progress_bar_texture_id;
        glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::PROGRESS_BAR_TEXTURE);
        progress_bar_texture_id = load_texture(GET_RESOURCE_FOLDER_PATH() + "textures/bar.png");

        unsigned int settings_wheel_texture_id;
        glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::SETTINGS_WHEEL_TEXTURE);
        settings_wheel_texture_id = load_texture(GET_RESOURCE_FOLDER_PATH() + "textures/settings_wheel.png");
        
        unsigned int settings_menu_texture_id;
        glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::SETTINGS_MENU_TEXTURE);
        settings_menu_texture_id = load_texture(GET_RESOURCE_FOLDER_PATH() + "textures/settingsmenu.png");

        unsigned int language_select_menu_texture_id;
        glActiveTexture(GL_TEXTURE0 + CONSTANTS::TEXTURES::LANGUAGE_SELECT_TEXTURE);
        language_select_menu_texture_id = load_texture(GET_RESOURCE_FOLDER_PATH() + "textures/languageselect.png");

        texture_ids[textureName::PROGRESS_BAR] = progress_bar_texture_id;
        texture_ids[textureName::SETTINGS_WHEEL] = settings_wheel_texture_id;
        texture_ids[textureName::SETTINGS_MENU] = settings_menu_texture_id;
        texture_ids[textureName::LANGUAGE_SELECT] = language_select_menu_texture_id;

        unsigned int indices[] = {

            0, 1, 3,
            1, 2, 3

        };

        float progress_bar_vertices[] = {

            0.9f, 0.8f, 0.0f,       1.0f, 1.0f,
            0.9f, -0.8f, 0.0f,      1.0f, 0.0f,
            -0.9f, -0.8f, 0.0f,     0.0f, 0.0f,
            -0.9f, 0.8f, 0.0f,      0.0f, 1.0f

        };

        float vertices[] = {

            0.5f, 0.5f, 0.0f,       1.0f, 1.0f,   
            0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 
            -0.5f, -0.5f, 0.0f,     0.0f, 0.0f,
            -0.5f, 0.5f, 0.0f,      0.0f, 1.0f

        };

        Buttons::set_corner_values(wholeTexture::LOADING_BAR, progress_bar_vertices[15], progress_bar_vertices[16], progress_bar_vertices[5], progress_bar_vertices[6]);
        Buttons::set_corner_values(wholeTexture::SETTINGS_WHEEL, vertices[15], vertices[16], vertices[5], vertices[6]);  
        Buttons::set_corner_values(wholeTexture::SETTINGS_MENU, vertices[15], vertices[16], vertices[5], vertices[6]);
        Buttons::set_corner_values(wholeTexture::LANGUAGE_SELECT_MENU, vertices[15], vertices[16], vertices[5], vertices[6]);

        constexpr int stride = 5 * sizeof(float);

        init_texture_geometry
        (
            progress_bar_vertices, sizeof(progress_bar_vertices),
            indices, sizeof(indices),
            textureName::PROGRESS_BAR, 

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

        init_texture_geometry
        (
            vertices, sizeof(vertices), 
            indices, sizeof(indices), 
            textureName::SETTINGS_WHEEL,

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

        init_texture_geometry(
            vertices, sizeof(vertices),
            indices, sizeof(indices),
            textureName::SETTINGS_MENU,

            3, 3, stride, (void*)0,
            4, 2, stride, (void*)(3 * sizeof(float))
        );

        init_texture_geometry(
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