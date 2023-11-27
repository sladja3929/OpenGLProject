#version 330

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
out vec4 color;

uniform mat4 uProjMat;
uniform mat4 uModelMat;
uniform vec4 uLPos;
uniform vec4 uLCol;
uniform vec4 uKamb;
uniform vec4 uKDif;
uniform vec4 uKSpc;
uniform float uShine;

void main()
{
	gl_Position  = uProjMat * (uModelMat * vPosition);
	gl_Position *= vec4(1,1,-1,1);	// z축 방향이 반대임

	vec4 P = uModelMat*vPosition;
	vec4 L = uLPos - P;
	vec3 L3 = normalize(L.xyz);
	vec4 N = uModelMat * vec4(vNormal, 0);
	vec3 N3 = normalize(N.xyz);          //camera coord
	vec4 V = vec4(0, 0, 0, 1);
	vec3 V3 = normalize(V.xyz);

	// R = 2*dot(N, L)*N - L
	vec3 R3 = 2 * dot(N3, L3) * N3 - L3;

	vec4 amb = uKamb * uLCol;
	vec4 dif = uKDif * uLCol * max(dot(L3, N3), 0);
	vec4 spec = uKSpc * uLCol * pow(max(dot(R3, V3), 0), uShine);
	
	color = amb + dif + spec;
	color.w = 1.0f;
    //color = vec4(N3, 1);
	//color = vColor;
}
