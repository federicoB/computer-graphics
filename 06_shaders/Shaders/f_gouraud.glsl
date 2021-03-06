// Fragment shader: : Gouraud shading
// ================
#version 320 es

// set floating point precision
precision highp float;

// Ouput data
out vec4 FragColor;

in vec3 LightingColor; 

void main()
{
   FragColor = vec4(LightingColor, 1.0);
}
