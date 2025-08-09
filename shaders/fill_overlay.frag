#version 330 core

in vec2 tex_coord;

out vec4 frag_color;

uniform float progress;
uniform float bar_begin_pos;
uniform float bar_end_pos;
uniform vec3 color;

void main() {

    if (tex_coord.x > progress || tex_coord.x < bar_begin_pos || tex_coord.x > bar_end_pos) 
        discard;

    frag_color = vec4(color, 0.2f);

}