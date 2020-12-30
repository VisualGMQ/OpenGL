#version 410 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;

void main() {
    // use gl_PointCoord to split image by diferent color
    if (gl_PointCoord.x<0)
        FragColor = vec4(1, 0, 0, 1)*texture(tex, TexCoord);
    else
        FragColor = vec4(0, 0, 1, 1)*texture(tex, TexCoord);
}
