// Vertex shader: Texture-Phong shading
// ================
#version 320 es

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 TexCoord;

// Output data
out vec3 N;
out vec3 E;
out vec3 L;
out vec2 _TexCoord;

struct PointLight {
	vec3 position;
	vec3 color;
	float power;
};

// Values that stay constant for the whole mesh.
uniform mat4 P;
uniform mat4 V;
uniform mat4 M; // position * rotation * scaling

uniform PointLight light;

void main() {
    gl_Position = P * V * M * vec4(pos, 1.0);

    vec4 objPosVCS = V * M * vec4(pos, 1.0); // Position in VCS
    vec4 lightPosVCS = V * vec4(light.position, 1.0); // LightPos in VCS

    // Compute vectors E, L, N in VCS
    E = normalize(-objPosVCS.xyz);
    L = normalize((lightPosVCS - objPosVCS).xyz);
    N = normalize(transpose(inverse(mat3(V * M))) * normal);
    _TexCoord = TexCoord;
}