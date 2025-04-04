#version 330

//in vec2 pos;
in vec3 color;

uniform uvec2 size;
uniform uvec2 offset;

out vec3 fragColor;

void main()
{
    uint x = uint(gl_VertexID) % size.x + offset.x;
    uint y = uint(gl_VertexID) / size.x + offset.y;
    
    vec2 pos = vec2(x, y) / vec2(size) * 2.0 - 1.0;  // Normalisation entre -1 et 1
    pos.y = -pos.y;  // OpenGL a l'origine en bas à gauche

    gl_Position = vec4(pos, 0.0, 1.0);
    fragColor = color;
}
