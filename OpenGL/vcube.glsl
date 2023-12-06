#version 330

in  vec4 vPosition;
in  vec4 vColor;
out vec4 color;
out vec4 position;

uniform mat4 uMat;
uniform vec4 uColor;

void main()
{
	
	gl_Position  = uMat*vPosition;
	gl_Position *= vec4(1,1,-1,1);

	position = vPosition;
	color = vColor;
   
}
