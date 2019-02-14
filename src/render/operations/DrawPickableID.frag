#version 330

out vec4 fragColor;
uniform vec4 pickableID;

void main(void) {
    fragColor = pickableID;
}
