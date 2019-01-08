#version 330

in vec3 normal_cameraSpace;

out vec4 fragColor;

const vec3 lightDirection = vec3(0, 0, 1);
uniform vec3 diffuse;
uniform vec3 ambient;

void main(void) {
    vec3 normalDirection = normalize(normal_cameraSpace);

    float diffuseStrength = clamp(dot(normalDirection, lightDirection), 0.0, 1.0);

    fragColor = vec4(ambient + diffuse * diffuseStrength, 1);
}
