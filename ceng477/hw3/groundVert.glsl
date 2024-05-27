#version 330 core

uniform mat4 modelingMatrix;
uniform mat4 modelingMatrixWithoutTranslation;
uniform mat4 viewingMatrix;
uniform mat4 projectionMatrix;

layout(location=0) in vec3 inVertex;
layout(location=1) in vec3 inNormal;

out vec4 pos;
out vec3 nor;
out vec4 initialPos;
void main(void)
{
	pos = modelingMatrix * vec4(inVertex, 1);
	nor = inverse(transpose(mat3x3(modelingMatrix))) * inNormal;
	initialPos = modelingMatrixWithoutTranslation * vec4(inVertex, 1);
    gl_Position = projectionMatrix * viewingMatrix * modelingMatrix * vec4(inVertex, 1);
}

