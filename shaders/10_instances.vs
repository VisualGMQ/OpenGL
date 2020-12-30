#version 410 core
layout (location = 0) in vec2 Pos;
layout (location = 1) in vec2 Tex;

out vec2 TexCoord;

void main() {
    gl_Position = vec4(Pos.x + gl_InstanceID*0.1, Pos.y, 0, 1);
    TexCoord = Tex;
}
