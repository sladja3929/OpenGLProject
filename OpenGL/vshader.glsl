#version 330

uniform float uTime;
uniform bool uWave;

in vec4 vPosition;
in vec4 vColor;

out vec4 color;
out vec4 position;

void main()
{
	float uTheta1 = 40;	//x
	float uTheta3 = uTime * 20;	//z

	float rad1 = uTheta1 / 180.0 * 3.141592;	// x - rotation
	mat4 m = mat4(1.0);
	m[0][0] = 1;			m[1][0] = 0;			m[2][0] = 0;			m[3][0] = 0;
	m[0][1] = 0;			m[1][1] = cos(rad1);	m[2][1] = -sin(rad1);	m[3][1] = 0;
	m[0][2] = 0;			m[1][2] = sin(rad1);	m[2][2] = cos(rad1);	m[3][2] = 0;
	m[0][3] = 0;			m[1][3] = 0;			m[2][3] = 0;			m[3][3] = 1;

	float rad3 = uTheta3 / 180.0 * 3.141592;	// z - rotation
	mat4 o = mat4(1.0);							
	o[0][0] = cos(rad3);	o[1][0] = -sin(rad3);	o[2][0] = 0;			o[3][0] = 0;
	o[0][1] = sin(rad3);	o[1][1] = cos(rad3);	o[2][1] = 0;			o[3][1] = 0;
	o[0][2] = 0;			o[1][2] = 0;			o[2][2] = 1;			o[3][2] = 0;
	o[0][3] = 0;			o[1][3] = 0;			o[2][3] = 0;			o[3][3] = 1;

	float dist = distance(vPosition, vec4(0, 0, 0, 1));	//원점과의 거리

	//거리에 따른 높이 비율, 거리 / 기준거리(최대=1.13137) * 최대높이
	float scale = 0.3 - dist / 0.8 * 0.3;	

	//사인파 공식: Asin(kx - wt + c), A=진폭, k=파수, w=각진동수, c=초기 위상
	vec4 wavePosition = vPosition + vec4(0, 0, clamp(scale, 0, 0.3) * sin(30 * dist - 5 * uTime), 0);

	wavePosition = uWave ? wavePosition : vPosition;

	gl_Position = m * o * wavePosition;
	gl_Position.w = 1.0f;

	color = vColor;
	position = wavePosition;
}