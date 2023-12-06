#pragma once

#include <vgl.h>
#include <vec.h>
#include <mat.h>

struct MyTorusVertex
{
	vec4 position;
	vec3 normal;
};

class MyTorus
{
public:
	MyTorus(void);
	~MyTorus(void);

	int NumVertices;

	MyTorusVertex * Vertices;
	GLuint Init(int slice1, int slice2, float rad1, float rad2);
	void SetPositionAndOtherAttributes(GLuint program);

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	void Draw(GLuint program);
};



MyTorus::MyTorus(void)
{
	bInitialized = false;
	NumVertices = 0;
	Vertices = NULL;
}

MyTorus::~MyTorus(void)
{
	if(Vertices != NULL) 
		delete [] Vertices;
}

GLuint MyTorus::Init(int slice1, int slice2, float r1, float r2)
{
	// The Cube should be initialized only once;
	if(bInitialized == true) return vao;

	NumVertices = slice1*slice2*2*3;
	Vertices = new MyTorusVertex [NumVertices];

	float da = 2*3.141592/slice1;
	float db = 2*3.141592/slice2;
	int cur = 0;

	for(int i=0; i<slice1; i++)
	{
		for(int j=0; j<slice2; j++)
		{
			mat4 mat1 = RotateZ(da*i*180.0/3.141592);
			mat4 mat2 = RotateZ(da*(i+1)*180.0/3.141592);

			mat4 mat3 = RotateX(db*j*180.0/3.141592);
			mat4 mat4 = RotateX(db*(j+1)*180.0/3.141592);

			vec4 na = mat1*mat3*vec4(0, r2, 0, 1);
			vec4 nb = mat1*mat4*vec4(0, r2, 0, 1);
			vec4 nc = mat2*mat4*vec4(0, r2, 0, 1);
			vec4 nd = mat2*mat3*vec4(0, r2, 0, 1);

			vec4 a = mat1*Translate(0,r1,0)*mat3*vec4(0, r2, 0, 1);
			vec4 b = mat1*Translate(0,r1,0)*mat4*vec4(0, r2, 0, 1);
			vec4 c = mat2*Translate(0,r1,0)*mat4*vec4(0, r2, 0, 1);
			vec4 d = mat2*Translate(0,r1,0)*mat3*vec4(0, r2, 0, 1);

			vec3 na3 = normalize(vec3(na.x, na.y, na.z));
			vec3 nb3 = normalize(vec3(nb.x, nb.y, nb.z));
			vec3 nc3 = normalize(vec3(nc.x, nc.y, nc.z));
			vec3 nd3 = normalize(vec3(nd.x, nd.y, nd.z));

			Vertices[cur].position = a;	Vertices[cur].normal = na3; cur ++;
			Vertices[cur].position = b;	Vertices[cur].normal = nb3; cur ++;
			Vertices[cur].position = c;	Vertices[cur].normal = nc3; cur ++;
	
			Vertices[cur].position = c;	Vertices[cur].normal = nc3; cur ++;
			Vertices[cur].position = d; Vertices[cur].normal = nd3; cur ++;
			Vertices[cur].position = a;	Vertices[cur].normal = na3; cur ++;

		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyTorusVertex)*NumVertices, Vertices, GL_STATIC_DRAW);
	
	bInitialized = true;
	return vao;
}

void MyTorus::SetPositionAndOtherAttributes(GLuint program)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyTorusVertex), BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, sizeof(MyTorusVertex), BUFFER_OFFSET(sizeof(vec4)));
	
}


void MyTorus::Draw(GLuint program)
{
	if(!bInitialized) return;			// check whether it is initiazed or not. 
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndOtherAttributes(program);
	
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}