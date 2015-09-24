#version 120

uniform mat4 modelViewProjectionMatrix;

void main(){
    vec4 position = gl_Vertex;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
	gl_FrontColor = gl_Color;
}
