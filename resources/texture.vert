#version 330

uniform mat4 P;
uniform mat4 C;
uniform mat4 mT;
uniform mat4 trans;
uniform mat4 mR;
uniform mat4 M;
uniform mat4 N;
uniform mat4 L;
uniform vec4 lightPos;
uniform vec4 camPos;
uniform int shadingMode;
uniform vec2 resolution;
uniform float elapsedTime;
uniform float deadTime;
uniform int enable;

in vec3 pos;

smooth out vec4 smoothColor;

void main()
{
	gl_Position = vec4(pos,1);
	smoothColor = vec4(0,1,1,1);
}
