#version 330

out ivec4 fragColor;
uniform ivec4 pickableID;

void main(void) {
    fragColor = pickableID;
}
