#version 330 core

uniform mat4 modelingMatrix;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

vec3 lightPos = vec3(400.0, 600.0, 150.0);   // light position in world coordinates
vec3 eyePos = vec3(0.0, 0.0, 0.0);
out vec4 fragWorldPos;
out vec3 fragWorldNor;
out vec3 lightPosTransformed;
out vec3 eyePosTransformed;

void main(void)
{
	lightPosTransformed = (modelingMatrix * vec4(lightPos, 1)).xyz;
	eyePosTransformed = (modelingMatrix * vec4(lightPos, 1)).xyz;
	fragWorldPos = modelingMatrix * vec4(inVertex, 1);
	fragWorldNor = inverse(transpose(mat3x3(modelingMatrix))) * inNormal;
	
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}

