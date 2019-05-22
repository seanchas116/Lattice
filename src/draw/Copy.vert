#version 330

layout(location = 0) in vec2 inTexCoord;
out vec2 texCoord;

void main(void) {
    gl_Position = vec4(inTexCoord * 2.0 - 1.0, 0, 1);
    texCoord = inTexCoord;
}
