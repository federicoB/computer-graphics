// Vertex shader: Blinn shading
// ================
#version 320 es

// Input vertex data, different for all executions of this shader.
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 N;
out vec3 L;

struct PointLight {
	vec3 position;
	vec3 color;
	float power;
};

// Values that stay constant for the whole mesh.
uniform mat4 P;
uniform mat4 V;
uniform mat4 M; // position*rotation*scaling

uniform PointLight light;

void main() {
	gl_Position = P * V * M * vec4(aPos, 1.0);

	vec4 vertexPosition = V * M * vec4(aPos, 1.0); // Position in VCS
	vec4 lightPosition = V * vec4(light.position, 1.0); // LightPos in VCS

	// Compute vectors L, N in VCS
	L = normalize((lightPosition - vertexPosition).xyz);
	N = normalize(transpose(inverse(mat3(V * M))) * aNormal);
}   