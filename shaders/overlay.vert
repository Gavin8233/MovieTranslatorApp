#version 330 core

layout(location = 3) in vec3 position;
layout(location = 4) in vec2 tex_coords;

out vec2 tex_coord;

uniform mat4 transform;

void main() {

    gl_Position = transform * vec4(position, 1.0f);
    tex_coord = tex_coords;

}