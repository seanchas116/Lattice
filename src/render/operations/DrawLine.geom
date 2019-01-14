#version 330


uniform mat4 P;
uniform mat4 MV;
uniform float width;
uniform vec2 viewportSize;
uniform float zNear;
uniform float zOffset;

layout(lines) in;
layout(triangle_strip, max_vertices = 4) out;

in vec3 vertexColor_vert[];
out vec3 vertexColor_geom;

void main(void) {
    vec4 p0_modelSpace = gl_in[0].gl_Position;
    vec4 p1_modelSpace = gl_in[1].gl_Position;

    vec4 p0_cameraSpace = MV * p0_modelSpace;
    vec4 p1_cameraSpace = MV * p1_modelSpace;

    // Don't render lines behind camera
    if (p0_cameraSpace.z > -zNear && p1_cameraSpace.z > -zNear) {
        EndPrimitive();
        return;
    }

    // Make sure p0.z < p1.z
    if (p1_cameraSpace.z < p0_cameraSpace.z) {
        vec4 tmp = p0_cameraSpace;
        p0_cameraSpace = p1_cameraSpace;
        p1_cameraSpace = tmp;
    }

    // Clip line using near plane
    if (p1_cameraSpace.z > -zNear) {
        vec4 pnear_cameraSpace = mix(p0_cameraSpace, p1_cameraSpace, (-zNear - p0_cameraSpace.z) / (p1_cameraSpace.z - p0_cameraSpace.z));
        p1_cameraSpace = pnear_cameraSpace;
    }

    vec4 p0_clipSpace = P * p0_cameraSpace;
    vec4 p1_clipSpace = P * p1_cameraSpace;

    vec2 p0 = (p0_clipSpace.xy / p0_clipSpace.w + 1.0) * (0.5 * viewportSize);
    vec2 p1 = (p1_clipSpace.xy / p1_clipSpace.w + 1.0) * (0.5 * viewportSize);
    float d0 = p0_clipSpace.z / p0_clipSpace.w;
    float d1 = p1_clipSpace.z / p1_clipSpace.w;

    vec2 direction = p1 - p0;
    vec2 offset = normalize(vec2(-direction.y, direction.x)) * (width * 0.5);

    gl_Position = vec4((p0 + offset) / (viewportSize * 0.5) - 1.0, d0 + zOffset, 1);
    vertexColor_geom = vertexColor_vert[0];
    EmitVertex();

    gl_Position = vec4((p0 - offset) / (viewportSize * 0.5) - 1.0, d0 + zOffset, 1);
    vertexColor_geom = vertexColor_vert[0];
    EmitVertex();

    gl_Position = vec4((p1 + offset) / (viewportSize * 0.5) - 1.0, d1 + zOffset, 1);
    vertexColor_geom = vertexColor_vert[1];
    EmitVertex();

    gl_Position = vec4((p1 - offset) / (viewportSize * 0.5) - 1.0, d1 + zOffset, 1);
    vertexColor_geom = vertexColor_vert[1];
    EmitVertex();

    EndPrimitive();
}
