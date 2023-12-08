#pragma once

#include <vgl.h>
#include <vec.h>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

struct MyObjectVertex
{
	vec4 position;
	vec4 color;
	vec3 normal;
};

class MyObject
{
public:
	MyObject(void);
	~MyObject(void);

	int NumVertices;

	MyObjectVertex * Vertices;
	GLuint Init(std::string fileName, vec4 color = vec4(0.5,0.5,0.5,1));
	void SetPositionAndOtherAttributes(GLuint program);

	GLuint vao;
	GLuint buffer;
	bool bInitialized;

	void Draw(GLuint program);
};



MyObject::MyObject(void)
{
	bInitialized = false;
	NumVertices = 0;
	Vertices = NULL;
}

MyObject::~MyObject(void)
{
	if(Vertices != NULL) 
		delete [] Vertices;
}

GLuint MyObject::Init(std::string fileName, vec4 color)
{
	// The Cube should be initialized only once;
	if(bInitialized == true) return vao;
	
	std::ifstream file(fileName);
	std::string line;
	std::vector<vec3> VertexInfo;
	int NumFaceInfo = 0;

	while (std::getline(file, line))
	{
		std::istringstream is(line);
		std::string str;

		if (!(is >> str)) continue;

		if (str == "v")
		{
			vec3 position;
			is >> position.x >> position.y >> position.z;
			VertexInfo.push_back(position);
		}

		else if (str == "f") NumFaceInfo++;
	}

	NumVertices = VertexInfo.size() * NumFaceInfo;

	Vertices = new MyObjectVertex[NumVertices];

	while (std::getline(file, line))
	{
		if (line[0] == 'v') 
	}

	int cur = 0;
	for(int j=0; j<lo_slice; j++)
	{
		float y2 = cos(da);
		float r2 = sqrt(1-y2*y2);
	}
	for(int i=0; i<la_slice; i++)
	{
		float y1 = cos(da*i);
		float r1 = sqrt(1-y1*y1);
		float y2 = cos(da*(i+1));
		float r2 = sqrt(1-y2*y2);
		for(int j=0; j<lo_slice; j++)
		{
			vec3 a(r1*cos(db*j), y1, r1*sin(db*j));
			vec3 b(r2*cos(db*j), y2, r2*sin(db*j));
			vec3 c(r2*cos(db*(j+1)), y2, r2*sin(db*(j+1)));
			vec3 d(r1*cos(db*(j+1)), y1, r1*sin(db*(j+1)));
			
			vec3 na = normalize(a);
			vec3 nb = normalize(b);
			vec3 nc = normalize(c);
			vec3 nd = normalize(d);

			if(i!=lo_slice-1)
			{
				vec3 p = a - b;
				vec3 q = c - b;
				vec3 n = normalize(cross(p, q));

				Vertices[cur].position = a;	Vertices[cur].color = color; Vertices[cur].normal = na; cur++;
				Vertices[cur].position = b;	Vertices[cur].color = color; Vertices[cur].normal = nb; cur++;
				Vertices[cur].position = c;	Vertices[cur].color = color; Vertices[cur].normal = nc; cur++;
			}
			if(i!=0)
			{
				vec3 p = c - d;
				vec3 q = a - d;
				vec3 n = normalize(cross(p, q));

				Vertices[cur].position = c;	Vertices[cur].color = color; Vertices[cur].normal = nc; cur ++;
				Vertices[cur].position = d;	Vertices[cur].color = color; Vertices[cur].normal = nd; cur ++;
				Vertices[cur].position = a;	Vertices[cur].color = color; Vertices[cur].normal = na; cur ++;
			}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex)*NumVertices, Vertices, GL_STATIC_DRAW);
	
	bInitialized = true;
	return vao;
}

void MyObject::SetPositionAndOtherAttributes(GLuint program)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4)));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4)+sizeof(vec4)));


}


void MyObject::Draw(GLuint program)
{
	if(!bInitialized) return;			// check whether it is initiazed or not. 
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndOtherAttributes(program);
	
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}