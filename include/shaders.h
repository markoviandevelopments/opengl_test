#ifndef SHADERS_H
#define SHADERS_H

// Vertex Shader for rendering objects
const char *vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 ourColor;

uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    ourColor = color;
}
)";

// Fragment Shader
const char *fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(ourColor, 1.0);
}
)";

const char *textVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec4 vertex; // (position.xy, texcoord.zw)

out vec2 TexCoords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0); // Map to clip space
    TexCoords = vertex.zw; // Pass texcoords to fragment shader
}
)";

const char *textFragmentShaderSource = R"(
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    float alpha = texture(text, TexCoords).r; // Sample glyph alpha
    FragColor = vec4(textColor, alpha);      // Apply color with alpha
}
)";

#endif // SHADERS_H
