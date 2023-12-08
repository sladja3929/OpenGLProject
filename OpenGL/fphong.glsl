#version 330

in  vec4 color;
in	vec3 N3;
in	vec3 L3;
in	vec3 V3;
out vec4 fColor;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uLPos;
uniform vec4 uLCol;
uniform vec4 uKAmb;
uniform vec4 uKDif;
uniform vec4 uKSpc;
uniform float uShine;


void
main()
{
	// Physics-based Rendering (PBR)

	vec3 N = normalize(N3);
	vec3 L = normalize(L3);
	vec3 V = normalize(V3);

	// R = 2*dot(N,L)*N - L
	vec3 R = 2*dot(N, L)*N - L;

	vec4 amb = uKAmb*uLCol;

	float d = max(dot(L,N),0);
	if(d>0.8) d = 1;
	else if(d>0.5) d = 0.7;
	else d = 0.1;

	vec4 dif = uKDif*uLCol*d;

	float s = pow(max(dot(R,V),0), uShine);
	if(s>0.5) s = 1;
	else s = 0;
	vec4 spec = uKSpc*uLCol*s;


	fColor = amb + dif + spec;					// phong illumination model
	fColor.w = 1.0f;
	
	if ( dot(V, N) < 0.2) fColor = vec4(1,0,0,1);


}
