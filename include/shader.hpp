
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>

class Shader {

    public:

        unsigned int ID;
        
        Shader() {  };

        void compile(const std::string vertex_source, const std::string fragment_source);
        Shader& use();

        void set_int(const char* name, const int value);
        void set_float(const char* name, const float value);
        
        void set_vec3(const char* name, const glm::vec3& vals);
        void set_mat4(const char* name, const glm::mat4& vals);

};



#endif