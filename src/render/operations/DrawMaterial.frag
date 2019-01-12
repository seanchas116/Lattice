#version 330

in vec3 normal_cameraSpace;
in vec2 texCoords;

out vec4 fragColor;

const vec3 lightDirection = vec3(0, 0, 1);
uniform vec3 diffuse;
uniform bool hasDiffuseTexture;
uniform sampler2D diffuseTexture;
uniform vec3 ambient;

void main(void) {
    vec3 normalDirection = normalize(normal_cameraSpace);

    vec3 d = hasDiffuseTexture ? texture(diffuseTexture, texCoords).rgb : diffuse;

    float diffuseStrength = clamp(dot(normalDirection, lightDirection), 0.0, 1.0);

    fragColor = vec4(ambient + d * diffuseStrength, 1);
}
