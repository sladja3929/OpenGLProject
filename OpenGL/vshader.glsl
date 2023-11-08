#version 330

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;

uniform mat4 uMat;

void main()
{
	gl_Position  = uMat*vPosition;
	gl_Position *= vec4(1,1,-0.1,1);

    color = vColor;
}
