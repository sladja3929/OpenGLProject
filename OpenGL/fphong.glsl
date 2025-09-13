#version 330

//w: 월드 좌표계로 계산된 View, Pos, Normal
in vec3 N3; 
in vec3 L3; 
in vec3 V3; 
in vec3 wV;
in vec3 wP;
in vec3 wN;

out vec4 fColor;

struct Material {
	vec4  k_d;	// diffuse coefficient
	vec4  k_s;	// specular coefficient
	float n;	// specular exponent
};

struct Sphere {
	vec4     center;
	float    radius;
	Material mtl;
};

struct Ray {
	vec3 pos;
	vec3 dir;
};

struct HitInfo {
	float    t;
	vec4     position;
	vec3     normal;
	Material mtl;
};

uniform mat4 uModelMat; 
uniform mat4 uViewMat; 
uniform mat4 uProjMat; 
uniform vec4 uLPos; 
uniform vec4 uLIntensity;
uniform vec4 uAmb; 
uniform vec4 uDif; 
uniform vec4 uSpc; 
uniform float uShininess; 
uniform samplerCube uCube;
uniform vec4 uEPos;

//구체 위치 정보가 유니폼으로 전달됨
uniform int uNumSphere;
uniform Sphere uSpheres[20];

uniform int uBounceLimit;
uniform int uDrawingMode;



bool IntersectRay( inout HitInfo hit, Ray ray );

vec4 Shade( Material mtl, vec4 position, vec3 normal, vec3 view, bool isFirst )
{
	vec4 color = vec4(0,0,0,1);
	vec3 L = normalize(uLPos.xyz - position.xyz);
	Ray sRay;
	sRay.pos = position.xyz;
	sRay.dir = L;

	//그림자가 지는 영역일 경우 ambient만 반환
	HitInfo sHit;
	if (IntersectRay(sHit, sRay))
	{
		color = uAmb;
		return color;
	}

	//색상 계산: 반사광일 경우 diffuse만 계산
	float NL = max(dot(normal, L), 0.0);
	if (!isFirst) {
		color = uLIntensity * mtl.k_d * NL;
		return color;
	}

	//색상 계산: 첫 번째 발사한 광선일 경우 색상 전체를 계산
	vec3 H = normalize(view + L);		
	float VR = pow(max(dot(H, normal), 0), mtl.n);

	color = uAmb + uLIntensity * mtl.k_d * NL + uLIntensity * mtl.k_s * VR;
	return color;
}

//광선과 구의 방정식을 통해 교차점을 계산, 교차점이 2개일 경우 가까운 점을 반환
bool IntersectRay( inout HitInfo hit, Ray ray )
{
	hit.t = 1e30;
	bool foundHit = false;
	for ( int i=0; i<uNumSphere; ++i ) {
		vec3 rayToSphere = ray.pos.xyz - uSpheres[i].center.xyz;
		float a = dot(ray.dir, ray.dir);
		float b = 2.0 * dot(rayToSphere, ray.dir);
		float c = dot(rayToSphere, rayToSphere) - uSpheres[i].radius * uSpheres[i].radius;
		float d = b * b - 4.0 * a * c;

		if (d > 0.0) {
			float t1 = (-b - sqrt(d)) / (2.0 * a);
			float t2 = (-b + sqrt(d)) / (2.0 * a);
			float t = min(t1, t2);

			if (t > 0.0 && t < hit.t) {
				hit.t = t;
				hit.position.xyz = ray.pos + ray.dir * t;
				hit.position.w = 1;
				hit.normal = normalize(hit.position.xyz - uSpheres[i].center.xyz);
				hit.mtl = uSpheres[i].mtl;
				foundHit = true;
			}
		}		
	}
	return foundHit;
}

vec4 RayTracer( Ray ray )
{
	//첫 번째 교차점을 찾았다면
	HitInfo hit;
	if ( IntersectRay( hit, ray ) ) {
		vec3 view = normalize( -ray.dir );
		vec4 clr = Shade( hit.mtl, hit.position, hit.normal, view, true);

		//최대 반사 횟수에 따라 반사 수행
		vec4 k_s = hit.mtl.k_s;
		for ( int bounce=0; bounce<uBounceLimit; bounce++ ) {

			if ( hit.mtl.k_s.r + hit.mtl.k_s.g + hit.mtl.k_s.b <= 0.0 ) break;
			
			Ray r;
			HitInfo h;
						
			r.pos = hit.position.xyz;
			r.dir = reflect(ray.dir, hit.normal);
			float rIntensity = pow(0.8, bounce); //반사 수행 후 강도를 줄임

			//교차점을 찾을 때 마다 반사 색 누적
			if ( IntersectRay( h, r ) ) {
				vec3 view_reflected = normalize(-r.dir);
				vec4 clr_reflected = Shade(h.mtl, h.position, h.normal, view_reflected, false);
				clr += k_s * clr_reflected;

				hit = h;
				ray = r;
				k_s = hit.mtl.k_s * rIntensity * 0.2;
			} 

			//더 이상 교차점을 찾지 못했다면 환경 색 반환
			else {
				clr += k_s * texture(uCube, vec3(1, -1, 1) * r.dir);
				break;
			}
		}

		return clr;
	} 

	//아무 교차점을 찾지 못했다면 환경 색 반환
	else {
		return texture(uCube, vec3(1,-1,1)*ray.dir);
	}
}

void main()
{
	//일반적인 phong shading
	if(uDrawingMode == 0) 
	{
		vec3 N = normalize(N3); 
		vec3 L = normalize(L3); 
		vec3 V = normalize(V3); 
		vec3 H = normalize(V+L); 

		float NL = max(dot(N, L), 0); 
		float VR = pow(max(dot(H, N), 0), uShininess); 

		fColor = uAmb + uLIntensity*uDif*NL + uLIntensity*uSpc*VR; 
		fColor.w = 1; 

		vec3 viewDir = wP - wV;
		vec3 dir = reflect(viewDir, wN);

		fColor += uSpc*texture(uCube, vec3(1,-1,1)*dir);
	}

	//ray tracing 지원
	else if(uDrawingMode == 1)
	{
		Ray r;
		r.pos = wV;
		r.dir = normalize(wP - wV);
		fColor = RayTracer (r);
	}
}
