#version 330

layout(location = 0) in vec3 position_modelSpace;
layout(location = 3) in vec3 vertexColor;

out vec3 position;
out vec3 vertexColor_vert;

void main(void) {
    gl_Position = vec4(position_modelSpace, 1);
    vertexColor_vert = vertexColor;
}
