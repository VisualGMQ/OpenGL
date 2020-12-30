#version 410 core
layout (location = 0) in vec2 Pos;
layout (location = 1) in vec2 Tex;

out vec2 TexCoord;

void main() {
    TexCoord = Tex;
    gl_Position = vec4(Pos, 0, 1.0);
}
