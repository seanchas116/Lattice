#version 330

layout(location = 0) in vec3 position_modelSpace;
layout(location = 1) in vec2 inTexCoords;
layout(location = 2) in vec3 normal_modelSpace;

uniform mat4 MVP;

void main(void) {
    gl_Position = MVP * vec4(position_modelSpace, 1);
}
