#version 330
in vec3 N3; 
in vec3 L3; 
in vec3 V3; 
in vec2 TexCoord;

out vec4 fColor;

uniform mat4 uModelMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uAmb; 
uniform vec4 uDif; 
uniform vec4 uSpc; 
uniform float uShininess; 

uniform float uTime;
uniform sampler2D uTexDif;
uniform sampler2D uTexCld;
uniform sampler2D uTexLgt;
uniform sampler2D uTexSpc;

void main()
{
	vec3 N = normalize(N3); 
	vec3 L = normalize(L3); 
	vec3 V = normalize(V3); 
	vec3 H = normalize(V+L); 

    float NL = max(dot(N, L), 0); 
	float VR = pow(max(dot(H, N), 0), uShininess); 
	vec4 dif = texture2D(uTexDif, TexCoord);
	vec4 lgt = texture2D(uTexLgt, TexCoord);
	vec4 spc = texture2D(uTexSpc, TexCoord);
	vec4 cld = texture2D(uTexCld, TexCoord+vec2(-uTime/10.0f,0));
	
	float NNL = max(-dot(N,L), 0);
	float s = spc.r;

	dif = (1-cld.r)*dif + vec4(1,1,1,1)*cld.r;

	fColor = uAmb + dif*NL + lgt*NNL + s*uSpc*VR; 

	fColor.w = 1;
}
