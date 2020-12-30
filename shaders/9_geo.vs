#version 410 core
layout (location = 0) in vec2 Pos;
layout (location = 1) in vec2 Tex;

out vec2 aTexCoord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 project;

void main() {
    gl_Position = project*view*model*vec4(Pos, 0, 1);
    aTexCoord = Tex;
}
