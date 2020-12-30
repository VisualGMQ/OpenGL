#version 410 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D tex;

void main() {
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
