#version 330

in vec4 color;
in vec4 position;

out vec4 fColor;

void main()
{
	vec4 c = color;

	c = mix(c, vec4(1, 0.5, 0, 1), position.z / -0.3);

	fColor = c;
}