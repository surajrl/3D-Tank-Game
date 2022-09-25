#version 120

// Attributes
attribute vec3 aVertexPosition;
attribute vec3 aVertexNormal;
attribute vec2 aVertexTexCoord;

// Uniforms
uniform mat4x4 ModelViewMatrix_uniform;
uniform mat4x4 ProjectionMatrix_uniform;
uniform vec3 LightPosition_uniform;

varying vec3 ViewDirection;
varying vec3 LightDirection;
varying vec3 Normal;
varying vec2 uv;



void main()
{	
	uv = aVertexTexCoord;
	
	ViewDirection = -vec3(ModelViewMatrix_uniform * vec4(aVertexPosition, 1.0));
	LightDirection = LightPosition_uniform;
	Normal = (ModelViewMatrix_uniform * vec4(aVertexNormal, 0.0)).xyz;

	gl_Position = ProjectionMatrix_uniform * ModelViewMatrix_uniform  * vec4(aVertexPosition,1.0);
}

