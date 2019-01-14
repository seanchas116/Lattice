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

bool mapLineToScreen(float zNear, vec4 p0_cameraSpace, vec4 p1_cameraSpace, out vec3 p0_screenSpace, out vec3 p1_screenSpace) {
    // Don't render lines behind camera
    if (p0_cameraSpace.z > -zNear && p1_cameraSpace.z > -zNear) {
        return false;
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

    p0_screenSpace = vec3(p0, d0);
    p1_screenSpace = vec3(p1, d1);
    return true;
}

void main(void) {
    vec4 p0_modelSpace = gl_in[0].gl_Position;
    vec4 p1_modelSpace = gl_in[1].gl_Position;
    vec3 p0;
    vec3 p1;

    bool ok = mapLineToScreen(zNear, MV * p0_modelSpace, MV * p1_modelSpace, p0, p1);
    if (!ok) {
        EndPrimitive();
        return;
    }

    vec2 direction = p1.xy - p0.xy;
    vec2 offset = normalize(vec2(-direction.y, direction.x)) * (width * 0.5);

    gl_Position = vec4((p0.xy + offset) / (viewportSize * 0.5) - 1.0, p0.z + zOffset, 1);
    vertexColor_geom = vertexColor_vert[0];
    EmitVertex();

    gl_Position = vec4((p0.xy - offset) / (viewportSize * 0.5) - 1.0, p0.z + zOffset, 1);
    vertexColor_geom = vertexColor_vert[0];
    EmitVertex();

    gl_Position = vec4((p1.xy + offset) / (viewportSize * 0.5) - 1.0, p1.z + zOffset, 1);
    vertexColor_geom = vertexColor_vert[1];
    EmitVertex();

    gl_Position = vec4((p1.xy - offset) / (viewportSize * 0.5) - 1.0, p1.z + zOffset, 1);
    vertexColor_geom = vertexColor_vert[1];
    EmitVertex();

    EndPrimitive();
}
