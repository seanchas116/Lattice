#version 330

layout(location = 0) in vec3 position_modelSpace;

out vec3 position;

void main(void) {
    gl_Position = vec4(position_modelSpace, 1);
}
