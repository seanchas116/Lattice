#version 330

uniform sampler2D colorSampler;
uniform sampler2D depthSampler;
uniform float opacity;

in vec2 texCoord;

out vec4 fragColor;

void main(void) {
    fragColor = texture(colorSampler, texCoord) * opacity;
    gl_FragDepth = texture(depthSampler, texCoord).r;
}
