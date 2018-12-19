#version 330

uniform float width;
uniform mat4 MVP;

layout(location = 0) in vec3 position_modelSpace;

out vec3 position;

void main(void) {
    vec4 pos = MVP * vec4(position_modelSpace, 1);
    gl_Position = pos;
    gl_PointSize = width;
}
