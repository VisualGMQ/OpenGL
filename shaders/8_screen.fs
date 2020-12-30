#version 410 core

in vec2 TexCoord; 
out vec4 FragColor;

uniform sampler2D tex; 

void main() {
    vec3 col = texture(tex, TexCoord).rgb;
    FragColor = vec4(col, 1.0);
}
