
#include "include/shader.hpp"

static std::string load_file_text_into_string(const std::string path) {

    std::string data;
    std::ifstream file;
    
    try {
    
        file.open(path);
    
        std::stringstream data_stream;
    
        data_stream << file.rdbuf();
    
        file.close();
    
        data = data_stream.str();
    
    } 
    catch (std::ifstream::failure f) { throw std::runtime_error("FAILED TO LOAD DATA FROM FILE"); }
    
    return data;

}

void Shader::compile(const std::string vertex_path, const std::string fragment_path) {

    unsigned int s_vert, s_frag;

    std::string vertex_code = load_file_text_into_string(vertex_path);
    std::string fragment_code = load_file_text_into_string(fragment_path);

    const char* vertex_source = vertex_code.c_str();
    const char* fragment_source = fragment_code.c_str();

    s_vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(s_vert, 1, &vertex_source, NULL);
    glCompileShader(s_vert);

    s_frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(s_frag, 1, &fragment_source, NULL);
    glCompileShader(s_frag);

    this->ID = glCreateProgram();
    glAttachShader(this->ID, s_vert);
    glAttachShader(this->ID, s_frag);

    glLinkProgram(this->ID);

    glDeleteShader(s_vert);
    glDeleteShader(s_frag);

}

Shader& Shader::use() {

    glUseProgram(this->ID);

    return *this;

}

void Shader::set_int(const char* name, const int value) {

    unsigned int location = glGetUniformLocation(this->ID, name);

    glUniform1i(location, value);

}

void Shader::set_float(const char* name, const float value) {

    unsigned int location = glGetUniformLocation(this->ID, name);

    glUniform1f(location, value);

}

void Shader::set_vec3(const char* name, const glm::vec3& vals) {

    unsigned int location = glGetUniformLocation(this->ID, name);

    glUniform3f(location, vals.x, vals.y, vals.z);

}

void Shader::set_mat4(const char* name, const glm::mat4& vals) {

    unsigned int location = glGetUniformLocation(this->ID, name);

    glUniformMatrix4fv(location, 1, false, glm::value_ptr(vals));

}