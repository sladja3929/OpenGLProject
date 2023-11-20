
#define _CRT_SECURE_NO_WARNINGS

#include <vgl.h>
#include <InitShader.h>
#include "MyCube.h"
#include "MyUtil.h"

#include <vec.h>
#include <mat.h>

#define MAZE_FILE	"maze2.txt"

#include <iostream>
#include <queue>
#include <vector>

MyCube cube;
GLuint program;

mat4 g_Mat = mat4(1.0f);
GLuint uMat;
GLuint uColor;

float wWidth = 1000;
float wHeight = 500;

vec3 cameraPos = vec3(0, 0, 0);
vec3 viewDirection = vec3(0, 0, -1);
std::pair<int, int> camera_idx;

vec3 goalPos = vec3(0, 0, 0);
std::pair<int, int> goal_idx;

int MazeSize;
char maze[255][255] = { 0 };

float cameraSpeed = 0.1;

int cameraRotateSpeed = 5;
int cameraAngle = 0;

float g_time = 0;

bool isWall = false;
bool isHint = false;
bool isTrace = false;
std::vector<std::pair<int, int>> traceLine;

struct Node
{
	int x, z, g, h;

	Node(int x, int z, int g, int h) : x(x), z(z), g(g), h(h) {}

	bool operator<(const Node& other) const
	{
		return (g + h) > (other.g + other.h);
	}
};

void AStar()
{
	bool visited[255][255] = { false, };
	std::pair<int, int> prev[255][255];
	std::priority_queue<Node> pq;
	traceLine.clear();

	pq.push(Node(camera_idx.first, camera_idx.second, 0, 0));
	prev[camera_idx.first][camera_idx.second] = std::make_pair(-1, -1);
	
	while (!pq.empty())
	{
		Node cur = pq.top();
		pq.pop();

		if (cur.x == goal_idx.first && cur.z == goal_idx.second)
		{
			int x = goal_idx.first;
			int z = goal_idx.second;

			while(prev[x][z].first != -1)
			{
				traceLine.push_back(std::make_pair(x, z));
				int tmp = x;
				x = prev[tmp][z].first;
				z = prev[tmp][z].second;
			}

			traceLine.push_back(std::make_pair(x, z));
			reverse(traceLine.begin(), traceLine.end());
			return;
		}

		visited[cur.x][cur.z] = true;
		
		int dir[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
		for (int i = 0; i < 4; i++)
		{
			int nextX = cur.x + dir[i][0];
			int nextZ = cur.z + dir[i][1];

			if (maze[nextX][nextZ] != '*' && !visited[nextX][nextZ])
			{
				int nextG = cur.g + 1;
				int nextH = sqrt(pow(nextX - goal_idx.first, 2) + pow(nextZ - goal_idx.second, 2));
				
				pq.push(Node(nextX, nextZ, nextG, nextH));

				prev[nextX][nextZ] = std::make_pair(cur.x, cur.z);
			}
		}
	}
}

inline vec3 getPositionFromIndex(int i, int j)
{
	float unit = 1;
	vec3 leftTopPosition = vec3(-MazeSize / 2.0 + unit / 2, 0, -MazeSize / 2.0 + unit / 2);
	vec3 xDir = vec3(1, 0, 0);
	vec3 zDir = vec3(0, 0, 1);
	return leftTopPosition + i * xDir + j * zDir;
}

inline void getCameraIndex()
{
	camera_idx.first = (cameraPos.x + MazeSize / 2) / 1;
	camera_idx.second = (cameraPos.z + MazeSize / 2) / 1;
}

void LoadMaze()
{
	FILE* file = fopen(MAZE_FILE, "r");
	char buf[255];
	fgets(buf, 255, file);
	sscanf(buf, "%d", &MazeSize);
	for (int j = 0; j < MazeSize; j++)
	{
		fgets(buf, 255, file);
		for (int i = 0; i < MazeSize; i++)
		{
			maze[i][j] = buf[i];
			if (maze[i][j] == 'C')				// Setup Camera Position
			{				
				cameraPos = getPositionFromIndex(i, j);
				camera_idx.first = i;
				camera_idx.second = j;
			}
			if (maze[i][j] == 'G')				// Setup Goal Position
			{
				goalPos = getPositionFromIndex(i, j);
				goal_idx.first = i;
				goal_idx.second = j;
			}
		}
	}
	fclose(file);
}

void DrawMaze()
{
	for (int j = 0; j < MazeSize; j++)
		for (int i = 0; i < MazeSize; i++)
			if (maze[i][j] == '*')
			{
				vec3 blockPos = getPositionFromIndex(i, j);
				vec3 color;

				if (length(blockPos - cameraPos) < 0.8)
				{
					isWall = true;
					cameraPos -= (blockPos - cameraPos) * cameraSpeed;
					color = vec3(255, 0, 0);					
				}

				else
				{
					isWall = false;
					color = vec3(i / (float)MazeSize, j / (float)MazeSize, 1);
				}

				mat4 ModelMat = Translate(blockPos);
				glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
				glUniform4f(uColor, color.x, color.y, color.z, 1);
				cube.Draw(program);
			}
}

void myInit()
{
	LoadMaze();
	cube.Init();
	program = InitShader("vshader.glsl", "fshader.glsl");
}

void DrawGrid()
{
	float n = 40;
	float w = MazeSize;
	float h = MazeSize;

	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(0, -0.5, -h / 2 + h / n * i) * Scale(w, 0.02, 0.02);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}
	for (int i = 0; i < n; i++)
	{
		mat4 m = Translate(-w / 2 + w / n * i, -0.5, 0) * Scale(0.02, 0.02, h);
		glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
		glUniform4f(uColor, 1, 1, 1, 1);
		cube.Draw(program);
	}

	if (isHint && !traceLine.empty())
	{
		for (int i = 0; i < traceLine.size() - 1; i++)
		{
			vec3 prev = getPositionFromIndex(traceLine[i].first, traceLine[i].second);
			vec3 next = getPositionFromIndex(traceLine[i + 1].first, traceLine[i + 1].second);
			mat4 m;
			if(prev.x == next.x) m = Translate(prev)* Scale(0.1, 0.1, 0.4);
			else m = Translate(prev) * Scale(0.4, 0.1, 0.1);
			glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * m);
			glUniform4f(uColor, 1, 0, 0, 1);
			cube.Draw(program);
		}
	}
}

void drawCamera()
{
	float cameraSize = 0.5;
	
	mat4 ModelMat = Translate(cameraPos) * RotateY(cameraAngle) * Scale(vec3(cameraSize));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);
	
	ModelMat = Translate(cameraPos + viewDirection * cameraSize / 2) * RotateY(cameraAngle) * Scale(vec3(cameraSize / 2));
	glUseProgram(program);
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 1, 0, 1);
	cube.Draw(program);
}

void drawGoal()
{
	glUseProgram(program);
	float GoalSize = 0.7;

	mat4 ModelMat = Translate(goalPos) * RotateY(g_time * 3) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);

	ModelMat = Translate(goalPos) * RotateY(g_time * 3 + 45) * Scale(vec3(GoalSize));
	glUniformMatrix4fv(uMat, 1, GL_TRUE, g_Mat * ModelMat);
	glUniform4f(uColor, 0, 0, 0, 0);
	cube.Draw(program);
}


void drawScene(bool bDrawCamera = true)
{
	glUseProgram(program);
	uMat = glGetUniformLocation(program, "uMat");
	uColor = glGetUniformLocation(program, "uColor");

	DrawGrid();
	DrawMaze();
	drawGoal();

	if (bDrawCamera)
		drawCamera();



}

void display()
{
	glEnable(GL_DEPTH_TEST);

	float vWidth = wWidth / 2;
	float vHeight = wHeight;

	// LEFT SCREEN : View From Camera (Perspective Projection)
	glViewport(0, 0, vWidth, vHeight);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	float h = 4;
	float aspectRatio = vWidth / vHeight;
	float w = aspectRatio * h;
	mat4 ViewMat = myLookAt(cameraPos, cameraPos + viewDirection, vec3(0, 1, 0));
	mat4 ProjMat = myPerspective(45, aspectRatio, 0.01, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(false);							// drawing scene except the camera


	// RIGHT SCREEN : View from above (Orthographic parallel projection)
	glViewport(vWidth, 0, vWidth, vHeight);
	h = MazeSize;
	w = aspectRatio * h;
	ViewMat = myLookAt(vec3(0, 5, 0), vec3(0, 0, 0), vec3(0, 0, -1));
	ProjMat = myOrtho(-w / 2, w / 2, -h / 2, h / 2, 0, 20);

	g_Mat = ProjMat * ViewMat;
	drawScene(true);


	glutSwapBuffers();
}

void idle()
{
	g_time += 1;

	if ((GetAsyncKeyState('A') & 0x8000) == 0x8000)		// if "A" key is pressed	: Go Left
	{
		cameraAngle = (cameraAngle + cameraRotateSpeed) % 360;
		vec4 rotateM = RotateY(cameraAngle) * vec3(0, 0, -1);
		viewDirection.x = rotateM.x;
		viewDirection.z = rotateM.z;
	}
	if ((GetAsyncKeyState('D') & 0x8000) == 0x8000)		// if "D" key is pressed	: Go Right
	{
		cameraAngle = (cameraAngle - cameraRotateSpeed) % 360;
		vec4 rotateM = RotateY(cameraAngle) * vec3(0, 0, -1);
		viewDirection.x = rotateM.x;
		viewDirection.z = rotateM.z;
	}
	if ((GetAsyncKeyState('W') & 0x8000) == 0x8000 && !isWall)		// if "W" key is pressed	: Go Forward
	{
		cameraPos += cameraSpeed * viewDirection;
		getCameraIndex();
	}
	if ((GetAsyncKeyState('S') & 0x8000) == 0x8000 && !isWall)		// if "S" key is pressed	: Go Backward
	{
		cameraPos += cameraSpeed * -viewDirection;
		getCameraIndex();
	}

	Sleep(16);											// for vSync
	glutPostRedisplay();
}

void myKeyboard(unsigned char c, int x, int  y)
{
	switch (c)
	{
		case 'q':
			AStar();
			isHint ? isHint = false : isHint = true;
			glutPostRedisplay();
			break;

		default:
			break;
	}
}

void reshape(int wx, int wy)
{
	printf("%d %d \n", wx, wy);
	wWidth = wx;
	wHeight = wy;
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(wWidth, wHeight);

	glutCreateWindow("Homework3 (Maze Navigator)");

	glewExperimental = true;
	glewInit();

	printf("OpenGL %s, GLSL %s\n", glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));

	myInit();
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(myKeyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
}