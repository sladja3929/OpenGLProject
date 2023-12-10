#pragma once

#include <vgl.h>
#include <vec.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

struct MyObjectVertex
{
	vec4 position;
	vec4 color;
	vec3 VNormal = vec3(0, 0, 0);
	vec3 SNormal;
};

class MyObject
{
public:
	MyObject(void);
	~MyObject(void);

	int NumVertices;
	MyObjectVertex * Vertices;
	GLuint vao;
	GLuint buffer;
	bool bInitialized;
	vec3 MinBox;
	vec3 MaxBox;
	std::vector<vec3> VertexInfo;
	std::vector<vec3> FaceInfo;
	vec3* VertexNormalInfo;

	void ReadObjFIle();
	GLuint Init(vec4 color);
	void SetPositionAndOtherAttributes(GLuint program, bool smoothOrFlat);
	void Draw(GLuint program, bool smoothOrFlat);
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

void MyObject::ReadObjFIle()
{
	std::string fileName;

	while (1)
	{
		std::cout << "Input File Path: ";
		std::cin >> fileName;

		std::ifstream file(fileName);

		if (file.is_open())
		{
			std::string line;

			while (std::getline(file, line))
			{
				std::istringstream is(line);
				std::string str;

				if (!(is >> str)) continue;

				else if (str == "v")
				{
					vec3 position;
					is >> position.x >> position.y >> position.z;

					MinBox.x = std::min(MinBox.x, position.x);
					MinBox.y = std::min(MinBox.y, position.y);
					MinBox.z = std::min(MinBox.z, position.z);

					MaxBox.x = std::max(MaxBox.x, position.x);
					MaxBox.y = std::max(MaxBox.y, position.y);
					MaxBox.z = std::max(MaxBox.z, position.z);

					VertexInfo.push_back(position);
				}

				else if (str == "f")
				{
					vec3 vertex;
					is >> vertex.x >> vertex.y >> vertex.z;

					FaceInfo.push_back(vertex);
				}
			}

			file.close();
			break;
		}

		else
		{
			std::cerr << "File Not Found!" << std::endl;
		}
	}
}

GLuint MyObject::Init(vec4 color)
{
	// The Cube should be initialized only once;
	if(bInitialized == true) return vao;

	

	NumVertices = FaceInfo.size() * 3;
	Vertices = new MyObjectVertex[NumVertices];
	VertexNormalInfo = new vec3[VertexInfo.size()]{ vec3(0, 0, 0), };

	for (int i = 0; i < NumVertices / 3; i++)
	{
		vec3 a, b, c;
		a = VertexInfo[FaceInfo[i].x - 1];
		b = VertexInfo[FaceInfo[i].y - 1];
		c = VertexInfo[FaceInfo[i].z - 1];

		vec3 p = b - a;
		vec3 q = c - a;
		vec3 n = normalize(cross(p, q));

		Vertices[i * 3].position = vec4(a, 1);
		Vertices[i * 3 + 1].position = vec4(b, 1);
		Vertices[i * 3 + 2].position = vec4(c, 1);

		Vertices[i * 3].color = color;
		Vertices[i * 3 + 1].color = color;
		Vertices[i * 3 + 2].color = color;

		Vertices[i * 3].SNormal = n;
		Vertices[i * 3 + 1].SNormal = n;
		Vertices[i * 3 + 2].SNormal = n;

		VertexNormalInfo[int(FaceInfo[i].x - 1)] += n;
		VertexNormalInfo[int(FaceInfo[i].y - 1)] += n;
		VertexNormalInfo[int(FaceInfo[i].z - 1)] += n;
	}

	for (int i = 0; i < NumVertices / 3; i++)
	{
		Vertices[i * 3].VNormal = normalize(VertexNormalInfo[int(FaceInfo[i].x - 1)]);
		Vertices[i * 3 + 1].VNormal = normalize(VertexNormalInfo[int(FaceInfo[i].y - 1)]);
		Vertices[i * 3 + 2].VNormal = normalize(VertexNormalInfo[int(FaceInfo[i].z - 1)]);
	}

	VertexInfo.clear();
	FaceInfo.clear();
	delete[] VertexNormalInfo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(MyObjectVertex)*NumVertices, Vertices, GL_STATIC_DRAW);
	
	bInitialized = true;
	return vao;
}

void MyObject::SetPositionAndOtherAttributes(GLuint program, bool smoothOrFlat)
{
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(0));

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, sizeof(MyObjectVertex), BUFFER_OFFSET(sizeof(vec4)));

	auto size = sizeof(vec4) + sizeof(vec4);
	smoothOrFlat ? size : size += sizeof(vec3);

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_TRUE, sizeof(MyObjectVertex), BUFFER_OFFSET(size));
}


void MyObject::Draw(GLuint program, bool smoothOrFlat)
{
	if(!bInitialized) return;			// check whether it is initiazed or not. 

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	SetPositionAndOtherAttributes(program, smoothOrFlat);
	
	glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}