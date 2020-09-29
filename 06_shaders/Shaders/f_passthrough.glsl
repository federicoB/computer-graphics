// Fragment shader: : Passthrough shading
// ================
#version 320 es

// set floating point precision
precision highp float;

// Ouput data
out vec4 FragColor;

uniform vec4 Color;

void main()
{
   FragColor = Color;
}
