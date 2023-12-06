#version 330

in  vec4 color;
in vec4 position;
out vec4 fColor;

uniform samplerCube uCubeTex;

void
main()
{
	vec3 dir = position.xyz;
	dir.y = -dir.y;
	fColor = color;
	fColor = texture(uCubeTex, dir);
}
