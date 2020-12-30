#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in vec2 aTexCoord[];
out vec2 TexCoord;

void main() {
    // first triangle
    gl_Position = gl_in[0].gl_Position - vec4(1, 0, 0 , 0);
    TexCoord = aTexCoord[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position - vec4(1, 0, 0, 0);
    TexCoord = aTexCoord[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position - vec4(1, 0, 0, 0);
    TexCoord = aTexCoord[2];
    EmitVertex();

    // second triangle
    gl_Position = gl_in[0].gl_Position + vec4(1, 0, 0, 0);
    TexCoord = aTexCoord[0];
    EmitVertex();

    gl_Position = gl_in[1].gl_Position + vec4(1, 0, 0, 0);
    TexCoord = aTexCoord[1];
    EmitVertex();

    gl_Position = gl_in[2].gl_Position + vec4(1, 0, 0, 0);
    TexCoord = aTexCoord[2];
    EmitVertex();

    EndPrimitive();
}
