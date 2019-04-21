#version 330

layout(location = 0) in vec3 position_modelSpace;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in float width;

out vec3 position;
out vec4 vertexColor_vert;
out float width_vert;

void main(void) {
    gl_Position = vec4(position_modelSpace, 1);
    vertexColor_vert = vertexColor;
    width_vert = width;
}
