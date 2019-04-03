#version 330

uniform float width;
uniform mat4 MVP;

layout(location = 0) in vec3 position_modelSpace;
layout(location = 1) in vec4 vertexColor;

out vec3 position;
out vec4 vertexColor_vert;

void main(void) {
    vec4 pos = MVP * vec4(position_modelSpace, 1);
    gl_Position = pos;
    vertexColor_vert = vertexColor;
}
